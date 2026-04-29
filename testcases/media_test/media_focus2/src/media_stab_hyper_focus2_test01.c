#include "media_focus2_test.h"
#include "media_focus2_utils_test.h"
#include <cmocka.h>
#include <media_api.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

/*focus2 test
 */

static int media_utils_uvfocus2_test_init(uvfocus2_priv_s *priv)
{
  // int ret = 0;
  priv->chain[0].handle = media_uv_focus_request2(
      priv->loop, priv->chain[0].stream, focus2_uv_change_callback, 1,
      (void *)&priv->chain[0]);
  return 0;
}

static void *media_utils_focus2_test_thread(void *arg)
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
  sleep(3);

out:
  if (chain->handle)
    {
      chain->ret = media_focus_abandon(chain->handle);
      chain->handle = NULL;
    }
  return NULL;
}

/*AAsync，BSync*/
void test_media_stab_hyper_focus2_01(FAR void **state)
{
  uvfocus2_priv_s *priv = (uvfocus2_priv_s *)*state;
  for (int i = 0; i < 300; i++)
    {
      priv->chain[0].stream = MEDIA_STREAM_MUSIC;
      priv->chain[1].stream = MEDIA_STREAM_ALARM;
      priv->chain[1].type = 0;
      UV_FOCUS2_EXECUTE(media_utils_uvfocus2_test_init, priv);

      pthread_mutex_lock(&priv->mutex);
      while (!priv->chain[0].ready)
        {
          pthread_cond_wait(&priv->cond, &priv->mutex);
        }

      pthread_attr_t attr;
      struct sched_param param;

      pthread_attr_init(&attr);
      param.sched_priority = 100;
      pthread_attr_setschedparam(&attr, &param);
      pthread_attr_setstacksize(&attr, 4096);
      int ret = pthread_create(&priv->chain[1].thread, &attr,
                               media_utils_focus2_test_thread,
                               (void *)&priv->chain[1]);
      assert_true(ret >= 0);

      pthread_join(priv->chain[1].thread, NULL);
      priv->chain[1].thread = 0;

      priv->app_id = 0;
      UV_FOCUS2_EXECUTE(media_utils_uvfocus2_common_abandon, priv);
      pthread_mutex_lock(&priv->mutex);
      while (!priv->chain[0].isabandoned)
        {
          pthread_cond_wait(&priv->cond, &priv->mutex);
        }
      pthread_mutex_unlock(&priv->mutex);
      assert_non_null(priv->chain[0].handle);
      priv->chain[0].handle = NULL;

      assert_true(priv->chain[0].ret >= 0);
      assert_true(priv->chain[1].ret >= 0);
      assert_true(priv->chain[0].nb_suggests == 3);
      assert_true(priv->chain[1].nb_suggests == 1);
      assert_true(priv->chain[0].suggests[0].suggest ==
                  MEDIA_FOCUS_PLAY);

      assert_true(priv->chain[1].suggests[0].suggest ==
                  MEDIA_FOCUS_PLAY);
      assert_true(priv->chain[0].suggests[1].suggest ==
                  MEDIA_FOCUS_PAUSE);
      assert_true(priv->chain[0].suggests[2].suggest ==
                  MEDIA_FOCUS_PLAY);
      assert_true(imaxabs(priv->chain[1].suggests[0].timestamp -
                          priv->chain[0].suggests[1].timestamp) > 0);
      for (int j = 0; j < 3; ++j)
        {
          priv->chain[j].stream = NULL;
          priv->chain[j].handle = NULL;
          priv->chain[j].ret = -1;
          priv->chain[j].ready = false;
          priv->chain[j].nb_suggests = 0;
          priv->chain[j].isabandoned = false;
          priv->chain[j].type = 1;
          priv->chain[j].cookie = priv;
          memset(priv->chain[j].suggests, 0,
                 sizeof(priv->chain[j].suggests));
        }
    }
}