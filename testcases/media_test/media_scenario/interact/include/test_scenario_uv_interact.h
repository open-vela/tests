#ifndef TEST_SCENARIO_INTERACT_H
#define TEST_SCENARIO_INTERACT_H

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <cmocka.h>

#include "media_test_uv_utils.h"
#include <media_api.h>

#ifdef CONFIG_CM_SCENARIO_UV_INTERACT_TEST
#define CM_MEDIA_SCENARIO_INTERACT_TESTCASES                            \
  cmocka_unit_test_setup_teardown(mediatest_uv_music_record,            \
                                  mediatest_uvplayer_setup,             \
                                  mediatest_uvplayer_teardown),         \
      cmocka_unit_test_setup_teardown(mediatest_uv_music_sco,           \
                                      mediatest_uvplayer_setup,         \
                                      mediatest_uvplayer_teardown),     \
      cmocka_unit_test_setup_teardown(mediatest_uv_music_ring,          \
                                      mediatest_uvplayer_setup,         \
                                      mediatest_uvplayer_teardown),     \
      cmocka_unit_test_setup_teardown(mediatest_uv_music_alarm,         \
                                      mediatest_uvplayer_setup,         \
                                      mediatest_uvplayer_teardown),     \
      cmocka_unit_test_setup_teardown(mediatest_uv_music_enforced,      \
                                      mediatest_uvplayer_setup,         \
                                      mediatest_uvplayer_teardown),     \
      cmocka_unit_test_setup_teardown(mediatest_uv_music_notify,        \
                                      mediatest_uvplayer_setup,         \
                                      mediatest_uvplayer_teardown),     \
      cmocka_unit_test_setup_teardown(mediatest_uv_music_tts,           \
                                      mediatest_uvplayer_setup,         \
                                      mediatest_uvplayer_teardown),     \
      cmocka_unit_test_setup_teardown(mediatest_uv_music_health,           \
                                      mediatest_uvplayer_setup,         \
                                      mediatest_uvplayer_teardown),     \
      cmocka_unit_test_setup_teardown(mediatest_uv_music_sport,           \
                                      mediatest_uvplayer_setup,         \
                                      mediatest_uvplayer_teardown),     \
      cmocka_unit_test_setup_teardown(mediatest_uv_music_info,           \
                                      mediatest_uvplayer_setup,         \
                                      mediatest_uvplayer_teardown),     \
      cmocka_unit_test_setup_teardown(mediatest_uv_music_music,           \
                                      mediatest_uvplayer_setup,         \
                                      mediatest_uvplayer_teardown),

#else
#define CM_MEDIA_SCENARIO_INTERACT_TESTCASES
#endif

void mediatest_uv_music_record(FAR void **state);
void mediatest_uv_music_sco(FAR void **state);
void mediatest_uv_music_ring(FAR void **state);
void mediatest_uv_music_alarm(FAR void **state);
void mediatest_uv_music_enforced(FAR void **state);
void mediatest_uv_music_notify(FAR void **state);
void mediatest_uv_music_tts(FAR void **state);
void mediatest_uv_music_health(FAR void **state);
void mediatest_uv_music_sport(FAR void **state);
void mediatest_uv_music_info(FAR void **state);
void mediatest_uv_music_music(FAR void **state);

#endif // TEST_SCENARIO_PLAYBACK_H