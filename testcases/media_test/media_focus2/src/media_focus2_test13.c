#include "media_focus2_test.h"
#include "media_focus2_utils_test.h"
#include <cmocka.h>
#include <media_api.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

static void *media_utils_focus2_test13_thread(void *arg)
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
static void *media_utils_focus2_test13_third_thread(void *arg)
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
  sleep(2);

out:
  if (chain->handle)
    {
      chain->ret = media_focus_abandon(chain->handle);
      chain->handle = NULL;
    }
  return NULL;
}

static void *media_utils_focus2_test13_initthread(void *arg)
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
  sleep(10);

out:
  if (chain->handle)
    {
      chain->ret = media_focus_abandon(chain->handle);
      chain->handle = NULL;
    }
  return NULL;
}
/*multi app */
void test_media_focus2_13(FAR void **state)
{
  stream_chain_s *chain = (stream_chain_s *)*state;
  chain[0].stream = MEDIA_STREAM_MUSIC;
  chain[1].stream = MEDIA_STREAM_ALARM;
  chain[2].stream = MEDIA_STREAM_RING;
  int ret;
  pthread_attr_t attr;
  struct sched_param param;

  pthread_attr_init(&attr);
  param.sched_priority = 100;
  pthread_attr_setschedparam(&attr, &param);
  pthread_attr_setstacksize(&attr, 4096);
  ret = pthread_create(&chain[0].thread, &attr,
                       media_utils_focus2_test13_initthread,
                       (void *)&chain[0]);
  assert_true(ret >= 0);
  sleep(1);
  ret = pthread_create(&chain[1].thread, &attr,
                       media_utils_focus2_test13_thread,
                       (void *)&chain[1]);
  assert_true(ret >= 0);
  sleep(1);
  ret = pthread_create(&chain[2].thread, &attr,
                       media_utils_focus2_test13_third_thread,
                       (void *)&chain[2]);
  assert_true(ret >= 0);

  pthread_join(chain[0].thread, NULL);
  pthread_join(chain[1].thread, NULL);
  pthread_join(chain[2].thread, NULL);
  chain[0].thread = 0;
  chain[1].thread = 0;
  chain[2].thread = 0;
  assert_true(chain[0].ret >= 0);
  assert_true(chain[1].ret >= 0);
  assert_true(chain[2].ret >= 0);
  assert_true(chain[0].nb_suggests == 5);
  assert_true(chain[1].nb_suggests == 3);
  assert_true(chain[2].nb_suggests == 1);
  assert_true(chain[0].suggests[0].suggest == MEDIA_FOCUS_PLAY);
  assert_true(chain[0].suggests[1].suggest == MEDIA_FOCUS_PAUSE);
  assert_true(chain[0].suggests[2].suggest == MEDIA_FOCUS_PAUSE);
  assert_true(chain[0].suggests[3].suggest == MEDIA_FOCUS_PAUSE);
  assert_true(chain[0].suggests[4].suggest == MEDIA_FOCUS_PLAY);
  assert_true(chain[1].suggests[0].suggest == MEDIA_FOCUS_PLAY);
  assert_true(chain[1].suggests[1].suggest == MEDIA_FOCUS_STOP);
  assert_true(chain[1].suggests[2].suggest == MEDIA_FOCUS_PLAY);
  assert_true(imaxabs(chain[1].suggests[0].timestamp -
                      chain[0].suggests[1].timestamp) >= 0);
  assert_true(imaxabs(chain[2].suggests[0].timestamp -
                      chain[1].suggests[1].timestamp) >= 0);
}