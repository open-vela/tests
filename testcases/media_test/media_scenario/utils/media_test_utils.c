#include "media_test_utils.h"
#include <media_api.h>
#include <sys/time.h>
#include <syslog.h>

void mediatest_sc_trace_suggest(mediatest_chain_sc *ctx, int suggest)
{
  struct timespec ts;

  if (ctx->nb_suggests >= 20)
    return; /* too much suggests. */

  printf("[%s][%s] suggestion:%d\n", __func__, ctx->stream, suggest);
  clock_gettime(CLOCK_MONOTONIC, &ts);
  ctx->suggest[ctx->nb_suggests].timestamp =
      ts.tv_sec * 1000000ll + ts.tv_nsec / 1000;
  ctx->suggest[ctx->nb_suggests].suggest = suggest;
  ctx->nb_suggests++;
}

void media_time_callback_trace(mediatest_chain_sc *ctx, int event)
{
  struct timespec ts;

  if (ctx->nb_cb >= 30)
    return; /* too much suggests. */

  printf("[%s][%s] handle [%p]callback event is %d\n", __func__,
         ctx->stream, ctx->handle, event);
  clock_gettime(CLOCK_MONOTONIC, &ts);
  ctx->cb_trace[ctx->nb_cb].timestamp =
      ts.tv_sec * 1000000ll + ts.tv_nsec / 1000;
  ctx->cb_trace[ctx->nb_cb].event = event;
  ctx->nb_cb++;
}

int mediatest_sc_common_setup(FAR void **state)
{
  mediatest_chain_sc *chain = (mediatest_chain_sc *)malloc(
      sizeof(mediatest_chain_sc) * MEDIATEST_APPS);
  if (chain == NULL)
    {
      return -1;
    }
  memset(chain, 0, sizeof(mediatest_chain_sc) * MEDIATEST_APPS);
  for (int i = 0; i < MEDIATEST_APPS; i++)
    {
      chain[i].last_event = MEDIA_EVENT_NOP;
      chain[i].result = -1;
      sem_init(&chain[i].sem, 0, 0);
      chain[i].flag = 0;
    }

  *state = chain;
  return 0;
}

int mediatest_sc_common_teardown(FAR void **state)
{
  mediatest_chain_sc *chain = (mediatest_chain_sc *)*state;
  if (chain != NULL)
    {
      for (int i = 0; i < MEDIATEST_APPS; i++)
        {
          if (chain[i].handle != 0)
            {
              pthread_join(chain[i].thread, NULL);
              chain[i].thread = 0;
            }
          sem_destroy(&chain[i].sem);
        }
      free(chain);
    }
  return 0;
}

void test_wait_prepared(mediatest_chain_sc *ctx)
{
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_nsec += MEDIATEST_TIMEOUT;
  ts.tv_sec += ts.tv_nsec / 1000000000;
  ts.tv_nsec %= 1000000000;

  if (sem_timedwait(&ctx->sem, &ts) == -1)
    {
      if (errno == ETIMEDOUT)
        {
          printf("sem_timedwait: [%s] [%p] wait_prepared timeout\n",
                 ctx->stream, ctx->handle);
          ctx->flag = -1;
        }
      printf("sem_timedwait: [%s] [%p] wait_prepared error\n",
             ctx->stream, ctx->handle);
      ctx->flag = -1;
    }
  printf("sem_timedwait: [%s] [%p] wait_prepared assertion\n",
         ctx->stream, ctx->handle);
  if (ctx->last_event == MEDIA_EVENT_PREPARED && ctx->result >= 0)
    {
      printf("sem_timedwait: [%s] [%p] wait_prepared success\n",
             ctx->stream, ctx->handle);
    }
  else
    {
      ctx->flag = -1;
    }
}

void test_wait_started(mediatest_chain_sc *ctx)
{
  printf("[%s] [%d] wait_started\n", ctx->stream, ctx->last_event);

  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_nsec += MEDIATEST_TIMEOUT;
  ts.tv_sec += ts.tv_nsec / 1000000000;
  ts.tv_nsec %= 1000000000;

  if (sem_timedwait(&ctx->sem, &ts) == -1)
    {

      if (errno == ETIMEDOUT)
        {
          printf("sem_timedwait: [%s] [%p] wait_started timeout\n",
                 ctx->stream, ctx->handle);
          ctx->flag = -1;
        }
      printf("sem_timedwait: [%s] [%p] wait_started error\n",
             ctx->stream, ctx->handle);
      ctx->flag = -1;
    }
  printf("sem_timedwait: [%s] [%p] wait_started assertion\n",
         ctx->stream, ctx->handle);
  // if (ctx->last_event == MEDIA_EVENT_COMPLETED)
  //   {
  //     printf("sem_1111111111111111111\n");
  //     sem_wait(&ctx->sem);
  //     ctx->flag = 0;
  //     return;
  //   }
  if ((ctx->last_event == MEDIA_EVENT_STARTED || ctx->last_event == MEDIA_EVENT_COMPLETED) &&
      ctx->result >= 0)
    {
      printf("sem_timedwait: [%s] [%p] wait_started success\n",
             ctx->stream, ctx->handle);
      ctx->flag = 0;
    }
  else
    {
      ctx->flag = -1;
    }
}

void test_wait_paused(mediatest_chain_sc *ctx)
{
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_nsec += MEDIATEST_TIMEOUT;
  ts.tv_sec += ts.tv_nsec / 1000000000;
  ts.tv_nsec %= 1000000000;

  if (sem_timedwait(&ctx->sem, &ts) == -1)
    {
      if (errno == ETIMEDOUT)
        {
          printf("sem_timedwait: [%s] [%p] wait_paused timeout\n",
                 ctx->stream, ctx->handle);
          ctx->flag = -1;
        }
      printf("sem_timedwait: [%s] [%p] wait_paused error\n", ctx->stream,
             ctx->handle);
      ctx->flag = -1;
    }
  printf("sem_timedwait: [%s] [%p] wait_paused assertion\n", ctx->stream,
         ctx->handle);
  if (ctx->last_event == MEDIA_EVENT_PAUSED && ctx->result >= 0)
    {
      printf("sem_timedwait: [%s] [%p] wait_paused success\n",
             ctx->stream, ctx->handle);
    }
  else
    {
      ctx->flag = -1;
    }
}

void test_wait_stopped(mediatest_chain_sc *ctx)
{
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_nsec += MEDIATEST_TIMEOUT;
  ts.tv_sec += ts.tv_nsec / 1000000000;
  ts.tv_nsec %= 1000000000;

  if (sem_timedwait(&ctx->sem, &ts) == -1)
    {
      if (errno == ETIMEDOUT)
        {
          printf("sem_timedwait: [%s] [%p] wait_stopped timeout\n",
                 ctx->stream, ctx->handle);
          ctx->flag = -1;
        }
      printf("sem_timedwait: [%s] [%p] wait_stopped error\n",
             ctx->stream, ctx->handle);
      ctx->flag = -1;
      ;
    }
  printf("sem_timedwait: [%s] [%p] wait_stopped assertion\n",
         ctx->stream, ctx->handle);
  if (ctx->last_event == MEDIA_EVENT_STOPPED && ctx->result >= 0)
    {
      printf("sem_timedwait: [%s] [%p] wait_stopped success\n",
             ctx->stream, ctx->handle);
    }
  else
    {
      ctx->flag = -1;
    }
}

void test_wait_seeked(mediatest_chain_sc *ctx)
{
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_nsec += MEDIATEST_TIMEOUT;
  ts.tv_sec += ts.tv_nsec / 1000000000;
  ts.tv_nsec %= 1000000000;

  if (sem_timedwait(&ctx->sem, &ts) == -1)
    {
      if (errno == ETIMEDOUT)
        {
          printf("sem_timedwait: [%s] [%p] wait_seeked timeout\n",
                 ctx->stream, ctx->handle);
          assert_true(0);
        }
      printf("sem_timedwait: [%s] [%p] wait_seeked error\n", ctx->stream,
             ctx->handle);
      assert_true(0);
    }
  printf("sem_timedwait: [%s] [%p] wait_seeked assertion\n", ctx->stream,
         ctx->handle);
  assert_true(ctx->last_event == MEDIA_EVENT_SEEKED && ctx->result >= 0);
  printf("sem_timedwait: [%s] [%p] wait_seeked success\n", ctx->stream,
         ctx->handle);
}

void test_wait_completed(mediatest_chain_sc *ctx, long long timeout)
{
  struct timespec ts;
  long long sec = timeout / 1000;
  long long nsec = (timeout % 1000) * 1000000;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_nsec += nsec;
  ts.tv_sec += sec;
  if (ts.tv_nsec >= 1000000000)
    {
      ts.tv_sec += ts.tv_nsec / 1000000000;
      ts.tv_nsec %= 1000000000;
    }

  if (sem_timedwait(&ctx->sem, &ts) == -1)
    {
      if (errno == ETIMEDOUT)
        {
          printf("sem_timedwait: [%s] [%p] wait_complete timeout\n",
                 ctx->stream, ctx->handle);
          ctx->flag = -1;
        }
      printf("sem_timedwait: [%s] [%p] wait_complete error\n",
             ctx->stream, ctx->handle);
      ctx->flag = -1;
    }
  printf("sem_timedwait: [%s] [%p] wait_complete assertion\n",
         ctx->stream, ctx->handle);
  if (ctx->last_event == MEDIA_EVENT_COMPLETED && ctx->result >= 0)
    {
      printf("sem_timedwait: [%s] [%p] wait_complete success\n",
             ctx->stream, ctx->handle);
    }
  else
    {
      ctx->flag = -1;
    }
}

void mediatest_common_event_cb(void *cookie, int event, int ret,
                               const char *data)
{
  mediatest_chain_sc *player = (mediatest_chain_sc *)cookie;
  media_time_callback_trace(player, event);
  player->last_event = event;
  player->result = ret;
  sem_post(&player->sem);
  syslog(LOG_INFO, "stream=%s, cookie=%p, event=%d, ret=%d, data=%p\n",
         player->stream, cookie, event, ret, data);
}

void mediatest_play_without_req(mediatest_chain_sc *player)
{
  int ret = -1;
  player->handle = media_player_open(player->stream);
  assert_non_null(player->handle);
  ret = media_player_set_event_callback(player->handle, player,
                                        mediatest_common_event_cb);
  assert_int_equal(ret, 0);
  ret = media_player_prepare(player->handle, player->url, NULL);
  assert_int_equal(ret, 0);
  test_wait_prepared(player);
  ret = media_player_start(player->handle);
  assert_int_equal(ret, 0);
  test_wait_started(player);
  return;
}

void mediatest_play_pause(mediatest_chain_sc *player)
{
  int ret = -1;
  assert_non_null(player->handle);
  ret = media_player_pause(player->handle);
  media_time_callback_trace(player, MEDIA_EVENT_NOP);
  assert_int_equal(ret, 0);
  test_wait_paused(player);
}

void mediatest_play_stop_close(mediatest_chain_sc *player)
{
  int ret = -1;
  ret = media_player_stop(player->handle);
  assert_int_equal(ret, 0);
  test_wait_stopped(player);
  ret = media_player_close(player->handle, 0);
  assert_int_equal(ret, 0);
  player->handle = NULL;
}
