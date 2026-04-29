#include "media_focus2_utils_test.h"
#include <media_api.h>
#include <sys/time.h>
#include <syslog.h>

void mediatest_trace_suggest(stream_chain_s *ctx, int suggest)
{
  struct timespec ts;

  if (ctx->nb_suggests >= 20)
    return; /* too much suggests. */

  printf("[%s][%s] suggestion:%d\n", __func__, ctx->stream, suggest);
  clock_gettime(CLOCK_MONOTONIC, &ts);
  ctx->suggests[ctx->nb_suggests].timestamp =
      ts.tv_sec * 1000000ll + ts.tv_nsec / 1000;
  ctx->suggests[ctx->nb_suggests].suggest = suggest;
  ctx->nb_suggests++;
}

/******************uv Interface auxiliary Function***********************************************************/

void *media_run_uv_loop(void *arg)
{
  uvfocus2_priv_s *priv = (uvfocus2_priv_s *)arg;

  syslog(LOG_INFO, "Starting uv_run loop...");

  pthread_mutex_lock(&priv->mutex);
  priv->uv_running = true;
  pthread_cond_signal(&priv->cond);
  pthread_mutex_unlock(&priv->mutex);

  while (priv->uv_running)
    {
      int ret = uv_run(priv->loop, UV_RUN_DEFAULT);
      if (ret == 0)
        {
          break;
        }
    }

  syslog(LOG_INFO, "uv_run loop stopped.");

  pthread_mutex_lock(&priv->mutex);
  priv->uv_running = false;
  pthread_cond_signal(&priv->cond);
  pthread_mutex_unlock(&priv->mutex);

  return NULL;
}

void mediafocus_uvasyncq_cb(uv_async_queue_t *asyncq, void *data)
{
  int ret __attribute__((unused));
  if (data == NULL)
    {
      syslog(LOG_ERR, "execute data is null\n");
      return;
    }
  uv_focus2_player *player_test = data;
  syslog(LOG_INFO, "test  started\n");
  ret = player_test->uv_player(player_test->priv);
  if (player_test != NULL)
    {
      free(player_test);
      player_test = NULL;
    }

  syslog(LOG_INFO, "test stoped\n");
}

void init_uvfocus2_priv(uvfocus2_priv_s *priv)
{
  int ret = posix_memalign((void **)&(priv->loop), 4, sizeof(uv_loop_t));
  assert_int_equal(ret, 0);
  assert_non_null(priv->loop);
  for (int i = 0; i < 3; ++i)
    {
      priv->chain[i].stream = NULL;
      priv->chain[i].handle = NULL;
      priv->chain[i].ret = -1;
      priv->chain[i].ready = false;
      priv->chain[i].nb_suggests = 0;
      priv->chain[i].isabandoned = false;
      priv->chain[i].type = 1;
      priv->chain[i].cookie = priv;
      memset(priv->chain[i].suggests, 0,
             sizeof(priv->chain[i].suggests));
    }

  pthread_cond_init(&priv->cond, NULL);
  pthread_mutex_init(&priv->mutex, NULL);
  priv->uv_running = false;
  syslog(LOG_INFO, "Initialized uvfocus_priv_s");
}

void media_on_close(uv_handle_t *handle)
{
  syslog(LOG_INFO, "Handle closed: %p", handle);
  handle->data = NULL;
}

// Stop all active handles CallbackFunction
void media_close_active_handle(uv_handle_t *handle, void *arg)
{
  if (uv_is_active(handle) && !uv_is_closing(handle))
    {
      uv_close(handle, media_on_close);
    }
}

void deinit_uvfocus2_priv(uvfocus2_priv_s *priv)
{
  if (priv->loop)
    {
      // Use uv_walk to Stop all active handles
      uv_walk(priv->loop, media_close_active_handle, NULL);

      // Ensure all handles have Stopped
      while (uv_loop_alive(priv->loop))
        {
          uv_run(priv->loop, UV_RUN_NOWAIT);
        }
      // Stop UV loop
      uv_loop_close(priv->loop);
      free(priv->loop);
      priv->loop = NULL;
    }
  pthread_cond_destroy(&priv->cond);
  pthread_mutex_destroy(&priv->mutex);
  syslog(LOG_INFO, "Deinitialized uvfocus2_priv_s");
}

int media_utils_uvfocus2_common_request(uvfocus2_priv_s *priv)
{
  int id = priv->app_id;
  priv->chain[id].handle = media_uv_focus_request2(
      priv->loop, priv->chain[id].stream, focus2_uv_change_callback, 1,
      (void *)&priv->chain[id]);
  return 0;
}

int media_utils_uvfocus2_common_abandon(uvfocus2_priv_s *priv)
{
  int id = priv->app_id;
  priv->chain[id].ret = media_uv_focus_abandon(
      priv->chain[id].handle, focus2_uv_common_close_cb);
  return 0;
}

int test_uv_focus2_common_setup(FAR void **state)
{
  uvfocus2_priv_s *priv = NULL;
  int ret = posix_memalign((void **)&priv, 4, sizeof(uvfocus2_priv_s));
  assert_int_equal(ret, 0);
  assert_non_null(priv);
  init_uvfocus2_priv(priv);
  *state = priv;

  ret = uv_loop_init(priv->loop);
  assert_int_equal(ret, 0);
  ret = uv_async_queue_init(priv->loop, &priv->uvasyncq,
                            mediafocus_uvasyncq_cb);
  assert_true(ret >= 0);

  // StartThreadRun uv_run
  pthread_t thread;
  ret = pthread_create(&thread, NULL, media_run_uv_loop, priv);
  assert_int_equal(ret, 0);
  priv->uv_thread = thread;

  // Ensure Thread has Started
  pthread_mutex_lock(&priv->mutex);
  while (!priv->uv_running)
    {
      pthread_cond_wait(&priv->cond, &priv->mutex);
    }
  pthread_mutex_unlock(&priv->mutex);

  syslog(LOG_INFO, "Setup complete");
  return 0;
}

int test_uv_focus2_common_teardown(FAR void **state)
{
  uvfocus2_priv_s *priv = *state;
  for (int i = 0; i < 3; ++i)
    {
      if (priv->chain[i].type == 1)
        {
          if (priv->chain[i].handle != NULL)
            {
              priv->app_id = i;
              UV_FOCUS2_EXECUTE(media_utils_uvfocus2_common_abandon,
                                priv);
              pthread_mutex_lock(&priv->mutex);
              while (!priv->chain[i].isabandoned)
                {
                  pthread_cond_wait(&priv->cond, &priv->mutex);
                }
              pthread_mutex_unlock(&priv->mutex);
              priv->chain[i].handle = NULL;
            }
        }
    }

  if (priv)
    {
      // SetStopFlag
      pthread_mutex_lock(&priv->mutex);
      priv->uv_running = false;
      pthread_cond_signal(&priv->cond);
      pthread_mutex_unlock(&priv->mutex);

      uv_close((uv_handle_t *)&priv->uvasyncq, NULL);

      // Stop uv_loop
      uv_stop(priv->loop);
      // WaitThreadEnd
      pthread_join(priv->uv_thread, NULL);
      deinit_uvfocus2_priv(priv);
      free(priv);
      *state = NULL;
    }
  syslog(LOG_INFO, "Teardown complete");
  return 0;
}

void focus2_uv_change_callback(int play_ret, int req_id, void *cookie)
{
  stream_chain_s *chain = (stream_chain_s *)cookie;
  mediatest_trace_suggest(chain, play_ret);
  chain->ready = true;
  uvfocus2_priv_s *priv = (uvfocus2_priv_s *)chain->cookie;
  pthread_mutex_lock(&priv->mutex);
  pthread_cond_signal(&priv->cond);
  pthread_mutex_unlock(&priv->mutex);
}

void focus2_uv_change_reply_callback(int play_ret, int req_id,
                                     void *cookie)
{
  stream_chain_s *chain = (stream_chain_s *)cookie;
  mediatest_trace_suggest(chain, play_ret);
  chain->ready = true;
  uvfocus2_priv_s *priv = (uvfocus2_priv_s *)chain->cookie;
  pthread_mutex_lock(&priv->mutex);
  pthread_cond_signal(&priv->cond);
  pthread_mutex_unlock(&priv->mutex);
  media_uv_focus_reply(chain->handle, req_id);
}

void focus2_uv_change_reply_overtime_callback(int play_ret, int req_id,
                                              void *cookie)
{
  stream_chain_s *chain = (stream_chain_s *)cookie;
  mediatest_trace_suggest(chain, play_ret);
  chain->ready = true;
  uvfocus2_priv_s *priv = (uvfocus2_priv_s *)chain->cookie;
  pthread_mutex_lock(&priv->mutex);
  pthread_cond_signal(&priv->cond);
  pthread_mutex_unlock(&priv->mutex);
  usleep(CONFIG_MEDIA_FOCUS_SUGGESTION_TIME_OUT * 1000);
  media_uv_focus_reply(chain->handle, req_id);
}

void focus2_uv_change_overtime_callback(int play_ret, int req_id,
                                        void *cookie)
{
  stream_chain_s *chain = (stream_chain_s *)cookie;
  mediatest_trace_suggest(chain, play_ret);
  chain->ready = true;
  uvfocus2_priv_s *priv = (uvfocus2_priv_s *)chain->cookie;
  pthread_mutex_lock(&priv->mutex);
  pthread_cond_signal(&priv->cond);
  pthread_mutex_unlock(&priv->mutex);
  usleep(CONFIG_MEDIA_FOCUS_SUGGESTION_TIME_OUT * 1000);
}

void focus2_uv_multi_reply_callback(int play_ret, int req_id,
                                    void *cookie)
{
  stream_chain_s *chain = (stream_chain_s *)cookie;
  mediatest_trace_suggest(chain, play_ret);
  chain->ready = true;
  uvfocus2_priv_s *priv = (uvfocus2_priv_s *)chain->cookie;
  pthread_mutex_lock(&priv->mutex);
  pthread_cond_signal(&priv->cond);
  pthread_mutex_unlock(&priv->mutex);
  media_uv_focus_reply(chain->handle, req_id);
  media_uv_focus_reply(chain->handle, req_id);
  media_uv_focus_reply(chain->handle, req_id);
  media_uv_focus_reply(chain->handle, req_id);
}

void focus2_uv_common_close_cb(void *cookie, int ret)
{
  stream_chain_s *chain = (stream_chain_s *)cookie;
  chain->ret = ret;
  chain->ready = true;
  chain->isabandoned = true;
  syslog(LOG_INFO,
         "focus2_uv_common_close_cb: stream=%s, handle=%p, ret=%d",
         chain->stream, chain->handle, ret);

  uvfocus2_priv_s *priv = (uvfocus2_priv_s *)chain->cookie;
  pthread_mutex_lock(&priv->mutex);
  pthread_cond_signal(&priv->cond);
  pthread_mutex_unlock(&priv->mutex);
}

/*********************Mixed Interface auxiliary Function**************************/

int test_focus2_hyper_teardown(FAR void **state)
{
  uvfocus2_priv_s *priv = *state;
  for (int i = 0; i < 3; ++i)
    {
      if (priv->chain[i].handle != 0)
        {
          priv->app_id = i;
          UV_FOCUS2_EXECUTE(media_utils_uvfocus2_common_abandon, priv);
          pthread_mutex_lock(&priv->mutex);
          while (!priv->chain[i].isabandoned)
            {
              pthread_cond_wait(&priv->cond, &priv->mutex);
            }
          pthread_mutex_unlock(&priv->mutex);
          priv->chain[i].handle = NULL;
        }
    }
  for (int i = 0; i < 3; ++i)
    {
      if (priv->chain[i].type == 0)
        {
          if (priv->chain[i].handle != 0)
            {
              pthread_join(priv->chain[i].thread, NULL);
              priv->chain[i].thread = 0;
            }
        }
    }

  if (priv)
    {
      // SetStopFlag
      pthread_mutex_lock(&priv->mutex);
      priv->uv_running = false;
      pthread_cond_signal(&priv->cond);
      pthread_mutex_unlock(&priv->mutex);

      uv_close((uv_handle_t *)&priv->uvasyncq, NULL);

      // Stop uv_loop
      uv_stop(priv->loop);
      // WaitThreadEnd
      pthread_join(priv->uv_thread, NULL);
      deinit_uvfocus2_priv(priv);
      free(priv);
      *state = NULL;
    }
  syslog(LOG_INFO, "Teardown complete");
  return 0;
}

/*********************Sync Interface auxiliary Function **************************/

void focus2_change_callback(int play_ret, int req_id, void *cookie)
{
  stream_chain_s *chain = (stream_chain_s *)cookie;
  mediatest_trace_suggest(chain, play_ret);
}

void focus2_change_reply_callback(int play_ret, int req_id, void *cookie)
{
  stream_chain_s *chain = (stream_chain_s *)cookie;
  mediatest_trace_suggest(chain, play_ret);
  media_focus_reply(chain->handle, req_id);
}

void focus2_change_multi_reply_callback(int play_ret, int req_id,
                                        void *cookie)
{
  stream_chain_s *chain = (stream_chain_s *)cookie;
  mediatest_trace_suggest(chain, play_ret);
  media_focus_reply(chain->handle, req_id);
  media_focus_reply(chain->handle, req_id);
  media_focus_reply(chain->handle, req_id);
}

void focus2_change_delay_reply_callback(int play_ret, int req_id,
                                        void *cookie)
{
  stream_chain_s *chain = (stream_chain_s *)cookie;
  mediatest_trace_suggest(chain, play_ret);
  usleep(CONFIG_MEDIA_FOCUS_SUGGESTION_TIME_OUT * 1000);
  media_focus_reply(chain->handle, req_id);
}

void focus2_change_delay_callback(int play_ret, int req_id, void *cookie)
{
  stream_chain_s *chain = (stream_chain_s *)cookie;
  mediatest_trace_suggest(chain, play_ret);
  usleep(CONFIG_MEDIA_FOCUS_SUGGESTION_TIME_OUT * 1000);
}

void media_timespec_sub(struct timespec *result,
                        const struct timespec *x,
                        const struct timespec *y)
{
  // If x nanoseconds Part is less than y nanoseconds Part, then borrow from seconds Part
  if (x->tv_nsec < y->tv_nsec)
    {
      result->tv_sec = x->tv_sec - y->tv_sec - 1;
      result->tv_nsec = x->tv_nsec + 1000000000 - y->tv_nsec;
    }
  else
    {
      result->tv_sec = x->tv_sec - y->tv_sec;
      result->tv_nsec = x->tv_nsec - y->tv_nsec;
    }

  // Ensure tv_nsec is not negative or greater than or equal to 1 billion
  if (result->tv_nsec >= 1000000000)
    {
      result->tv_sec++;
      result->tv_nsec -= 1000000000;
    }
}

void *media_utils_focus2_thread(void *arg)
{
  stream_chain_s *chain = (stream_chain_s *)arg;
  int ret = 0;
  chain->handle = media_focus_request2(
      &ret, chain->stream, focus2_change_callback, 1, (void *)chain);
  assert_non_null(chain->handle);
  mediatest_trace_suggest(chain, ret);
  if (ret != MEDIA_FOCUS_PLAY)
    {
      goto out;
    }
  sleep(5);

out:
  if (chain->handle)
    {
      chain->ret = media_focus_abandon(chain->handle);
      chain->handle = NULL;
    }
  return NULL;
}

int test_focus2_common_setup(FAR void **state)
{
  stream_chain_s *chain =
      (stream_chain_s *)malloc(sizeof(stream_chain_s) * 3);
  if (chain == NULL)
    {
      return -1;
    }
  for (int i = 0; i < 3; i++)
    {
      chain[i].stream = NULL;
      chain[i].ready = false;
      chain[i].handle = NULL;
      chain[i].ret = -1;
      chain[i].nb_suggests = 0;
      chain[i].isabandoned = false;
      chain[i].type = 0;
      memset(chain[i].suggests, 0, sizeof(chain[i].suggests));
    }

  *state = chain;
  return 0;
}

int test_focus2_common_teardown(FAR void **state)
{
  stream_chain_s *chain = (stream_chain_s *)*state;
  if (chain != NULL)
    {
      for (int i = 0; i < 3; i++)
        {
          if (chain[i].handle != 0)
            {
              pthread_join(chain[i].thread, NULL);
              chain[i].thread = 0;
            }
        }
      free(chain);
    }
  return 0;
}
