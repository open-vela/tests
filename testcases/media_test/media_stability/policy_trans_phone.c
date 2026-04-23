/****************************************************************************
 * apps/tests/testcases/media_test/media_stability/policy_trans_phone.c
 *
 * Name: policy_trans_phone
 * Example description:
 *  1. policy_trans_phone
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <media_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include "media_graph_test.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, char **argv)
{
  struct mediatest_data *media =
      (struct mediatest_data *)malloc(sizeof(struct mediatest_data));
  FUN_CHECK(mediatest_setup(media), media, mediatest_common_close, 0,
            "FAIL! malloc failed\n");
  FUN_CHECK(mediatest_getopt(argc, argv, media), media,
            mediatest_common_close, 0,
            "FAIL! url option do not given\n");
  int ret = -1;

  media = (struct mediatest_data *)malloc(sizeof(struct mediatest_data));
  FUN_CHECK(mediatest_setup(media), media, mediatest_common_close, 0,
            "FAIL! malloc failed\n");
  FUN_CHECK(mediatest_player_open(media), media, mediatest_common_close,
            0, "FAIL! media open fail.\n");
  FUN_CHECK(mediatest_common_prepare(media), media,
            mediatest_common_close, 1, "FAIL! media prepare fail\n");
  STATE_CHECK(PLAYER_PREPARED, media, mediatest_common_close,
              "player: prepare event return failed\n");
  media_player_set_looping(media->handle, -1);
  mediatest_common_start(media);
  STATE_CHECK(PLAYER_STARTED, media, mediatest_common_close,
              "player: start event return failed\n");
  while (media->rept-- && media_player_is_playing(media->handle))
    {
      sleep(3);
      if ((ret = media_policy_set_audio_mode("phone")) < 0)
        {
          syslog(LOG_ERR, "media_policy_set_audio_mode phone failed\n");
          return -1;
        }

      sleep(3);
      if ((ret = media_policy_set_audio_mode("normal")) < 0)
        {
          syslog(LOG_ERR, "media_policy_set_audio_mode normal failed\n");
          return -1;
        }
    }

  FUN_CHECK(mediatest_common_close(media), media, mediatest_common_close,
            0, "FAIL! media close fail\n");
  free(media);
  media = NULL;
  syslog(LOG_INFO, "TEST PASSED\n");
  return 0;
}
