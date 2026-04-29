#include "media_test_uv_utils.h"
#include "test_scenario_uv_interact.h"
#include <media_api.h>

void mediatest_uv_music_sco(FAR void **state)
{
  struct mediatest_uv_data *chain = (struct mediatest_uv_data *)*state;

  chain->mediatest_info[0].stream_type = MEDIA_STREAM_MUSIC;
  chain->mediatest_info[0].url =
      CONFIG_MEDIATEST_COMMON_PATH CONFIG_MEDIATEST_FOCUS_AUDIO_1;
  chain->mediatest_info[0].cookie = &chain->mediatest_info[0];
  chain->mediatest_info[0].focus = MEDIA_SCENARIO_MUSIC;

  chain->mediatest_info[1].stream_type = MEDIA_STREAM_MUSIC;
  chain->mediatest_info[1].url =
      CONFIG_MEDIATEST_COMMON_PATH CONFIG_MEDIATEST_FOCUS_SHORT_1;
  chain->mediatest_info[1].cookie = &chain->mediatest_info[1];
  chain->mediatest_info[1].focus = MEDIA_SCENARIO_INCALL;

  int ret = 0;
  pthread_attr_t attr;
  struct sched_param param;

  pthread_attr_init(&attr);
  param.sched_priority = 100;
  pthread_attr_setschedparam(&attr, &param);
  pthread_attr_setstacksize(&attr, 4096);
  ret = pthread_create(&chain->mediatest_info[0].thread, &attr,
                       media_utils_uvplayer_thread,
                       (void *)&chain->mediatest_info[0]);
  assert_true(ret >= 0);
  sleep(2);
  ret = pthread_create(&chain->mediatest_info[1].thread, &attr,
                       media_utils_uvplayer_thread,
                       (void *)&chain->mediatest_info[1]);

  pthread_join(chain->mediatest_info[0].thread, NULL);
  chain->mediatest_info[0].thread = 0;
  pthread_join(chain->mediatest_info[1].thread, NULL);
  chain->mediatest_info[1].thread = 0;
  assert_true(chain->mediatest_info[0].error == 0);
  assert_true(chain->mediatest_info[0].suggest_count == 3);
  assert_true(chain->mediatest_info[0].suggests[0].suggest ==
              MEDIA_FOCUS_PLAY);
  assert_true(chain->mediatest_info[0].suggests[1].suggest ==
              MEDIA_FOCUS_PAUSE);
  assert_true(chain->mediatest_info[0].suggests[2].suggest ==
              MEDIA_FOCUS_PLAY);
  assert_true(chain->mediatest_info[1].suggests[0].suggest ==
              MEDIA_FOCUS_PLAY);
  assert_true(imaxabs(chain->mediatest_info[1].suggests[0].timestamp -
                      chain->mediatest_info[0].suggests[1].timestamp) <=
                  SUGGEST_DELAY_TIME * 1000 &&
              chain->mediatest_info[1].suggests[0].timestamp -
                      chain->mediatest_info[0].suggests[1].timestamp >
                  0);
  assert_true(chain->mediatest_info[1].suggest_count == 1);
  assert_true(chain->mediatest_info[1].error == 0);
}