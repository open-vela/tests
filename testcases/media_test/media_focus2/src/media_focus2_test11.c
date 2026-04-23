#include "media_focus2_test.h"
#include "media_focus2_utils_test.h"
#include <cmocka.h>
#include <media_api.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

static void *media_utils_focus2_test11_thread(void *arg)
{
  stream_chain_s *chain = (stream_chain_s *)arg;
  int ret = 0;
  chain->handle = media_focus_request2(&ret, chain->stream,
                                       focus2_change_reply_callback, 0,
                                       (void *)chain);
  assert_non_null(chain->handle);
  mediatest_trace_suggest(chain, ret);

  if (chain->handle)
    {
      chain->ret = media_focus_abandon(chain->handle);
      chain->handle = NULL;
    }
  chain->handle = media_focus_request2(&ret, chain->stream,
                                       focus2_change_reply_callback, 0,
                                       (void *)chain);
  assert_non_null(chain->handle);
  mediatest_trace_suggest(chain, ret);
  sleep(2);
  if (chain->handle)
    {
      chain->ret = media_focus_abandon(chain->handle);
      chain->handle = NULL;
    }

  return NULL;
}

/*focus2 + reply*/
void test_media_focus2_11(FAR void **state)
{
  stream_chain_s *chain = (stream_chain_s *)*state;
  chain[0].stream = MEDIA_STREAM_MUSIC;
  int ret;
  pthread_attr_t attr;
  struct sched_param param;

  pthread_attr_init(&attr);
  param.sched_priority = 100;
  pthread_attr_setschedparam(&attr, &param);
  pthread_attr_setstacksize(&attr, 4096);
  ret = pthread_create(&chain[0].thread, &attr,
                       media_utils_focus2_test11_thread,
                       (void *)&chain[0]);
  assert_true(ret >= 0);

  pthread_join(chain[0].thread, NULL);
  chain[0].thread = 0;
  assert_true(chain[0].ret >= 0);
  assert_true(chain[0].nb_suggests == 2);
  assert_true(chain[0].suggests[0].suggest == MEDIA_FOCUS_PLAY);
  assert_true(chain[0].suggests[1].suggest == MEDIA_FOCUS_PLAY);
}