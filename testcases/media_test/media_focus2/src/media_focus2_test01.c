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
void test_media_focus2_01(FAR void **state)
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
                       media_utils_focus2_thread, (void *)&chain[0]);
  assert_true(ret >= 0);

  pthread_join(chain[0].thread, NULL);
  chain[0].thread = 0;
  assert_true(chain[0].ret >= 0);
  assert_true(chain[0].nb_suggests == 1);
  assert_true(chain[0].suggests[0].suggest == MEDIA_FOCUS_PLAY);
}