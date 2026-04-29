#include "media_test_utils.h"
#include <cmocka.h>
#include <media_api.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

static void *media_play_first(void *arg)
{
  int ret = -1;
  mediatest_chain_sc *ctx = (mediatest_chain_sc *)arg;
  ctx->stream = MEDIA_STREAM_MUSIC;
  ctx->url = CONFIG_MEDIATEST_COMMON_PATH CONFIG_MEDIATEST_COMMON_AUDIO_1;
  ctx->handle = media_player_open(ctx->stream);
  assert_non_null(ctx->handle);
  ret = media_player_set_event_callback(ctx->handle, ctx,
                                        mediatest_common_event_cb);
  RES_CHECK(ret, ctx, "set event callback failed\n");
  ret = media_player_prepare(ctx->handle, ctx->url, NULL);
  RES_CHECK(ret, ctx, "prepare failed\n");
  test_wait_prepared(ctx);
  RES_CHECK(ctx->flag, ctx, "prepare callback failed\n");
  ret = media_player_start(ctx->handle);
  RES_CHECK(ret, ctx, "start failed\n");
  test_wait_started(ctx);
  RES_CHECK(ctx->flag, ctx, "start callback failed\n");
  sleep(5);
out:
  ret = media_player_stop(ctx->handle);
  RES_GOAL(ret, ctx, "stop failed\n");
  test_wait_stopped(ctx);
  media_player_close(ctx->handle, 0);
  RES_GOAL(ret, ctx, "close failed\n");
  ctx->handle = NULL;
  return NULL;
}

static void *media_play_second(void *arg)
{
  int ret = -1;
  mediatest_chain_sc *ctx = (mediatest_chain_sc *)arg;
  ctx->stream = MEDIA_STREAM_ALARM;
  ctx->url = CONFIG_MEDIATEST_COMMON_PATH CONFIG_MEDIATEST_COMMON_AUDIO_1;
  ctx->handle = media_player_open(ctx->stream);
  assert_non_null(ctx->handle);
  ret = media_player_set_event_callback(ctx->handle, ctx,
                                        mediatest_common_event_cb);
  RES_CHECK(ret, ctx, "set event callback failed\n");
  ret = media_player_prepare(ctx->handle, ctx->url, NULL);
  RES_CHECK(ret, ctx, "prepare failed\n");
  test_wait_prepared(ctx);
  RES_CHECK(ctx->flag, ctx, "prepare callback failed\n");
  ret = media_player_start(ctx->handle);
  RES_CHECK(ret, ctx, "start failed\n");
  test_wait_started(ctx);
  RES_CHECK(ctx->flag, ctx, "start callback failed\n");
  sleep(1);
  ret = media_player_pause(ctx->handle);
  RES_CHECK(ret, ctx, "pause failed\n");
  media_time_callback_trace(ctx, MEDIA_EVENT_NOP);
  test_wait_paused(ctx);
  sleep(1);
out:
  ret = media_player_stop(ctx->handle);
  RES_GOAL(ret, ctx, "stop failed\n");
  test_wait_stopped(ctx);
  media_player_close(ctx->handle, 0);
  RES_GOAL(ret, ctx, "close failed\n");
  ctx->handle = NULL;
  return NULL;
}

void test_mul_pause_time(void **state)
{
  mediatest_chain_sc *chain = *state;
  int ret;
  pthread_attr_t attr;
  struct sched_param param;

  pthread_attr_init(&attr);
  param.sched_priority = 100;
  pthread_attr_setschedparam(&attr, &param);
  pthread_attr_setstacksize(&attr, 4096);
  ret = pthread_create(&chain[0].thread, &attr, media_play_first,
                       (void *)&chain[0]);
  assert_true(ret >= 0);
  pthread_attr_t attr1;
  struct sched_param param1;

  pthread_attr_init(&attr1);
  param1.sched_priority = 60;
  pthread_attr_setschedparam(&attr1, &param1);
  pthread_attr_setstacksize(&attr1, 4096);
  ret = pthread_create(&chain[1].thread, &attr1, media_play_second,
                       (void *)&chain[1]);
  assert_true(ret >= 0);

  pthread_join(chain[0].thread, NULL);
  pthread_join(chain[1].thread, NULL);
  assert_true(chain[0].flag >= 0);
  assert_true(chain[1].flag >= 0);
  chain[0].thread = 0;
  chain[1].thread = 0;
  printf("%lld\n", imaxabs(chain[1].cb_trace[3].timestamp -
                           chain[1].cb_trace[2].timestamp));
  assert(imaxabs(chain[1].cb_trace[3].timestamp -
                 chain[1].cb_trace[2].timestamp) < 500000);
}