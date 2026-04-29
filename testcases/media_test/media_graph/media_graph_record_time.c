/****************************************************************************
 * apps/tests/testcases/media_test/media_graph/media_graph_record_time.c
 *
 * Name: media_graph_record_time
 * Example description:
 *  1. media_graph_record_time
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <media_api.h>
#include <nuttx/config.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#include "media_graph_test.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  struct mediatest_data *media =
      (struct mediatest_data *)malloc(sizeof(struct mediatest_data));
  FUN_CHECK(mediatest_setup(media), media, mediatest_common_close, 0,
            "FAIL! malloc failed\n");
  FUN_CHECK(mediatest_getopt(argc, argv, media), media,
            mediatest_common_close, 0,
            "FAIL! url option do not given\n");
  char *play_format = NULL;
  if (media->type == MEDIATEST_CAMERA)
    {
      play_format = "Video";
      FUN_CHECK(mediatest_send("devsink@preview", "start", NULL), media,
                mediatest_common_close, 1,
                "FAIL! url option do not given\n");
    }
  else if (media->type == MEDIATEST_RECORDER)
    {
      play_format = "Music";
    }
  else
    {
      syslog(LOG_ERR, "stream type failed\n");
      free(media);
      media = NULL;
      return -1;
    }

  FUN_CHECK(mediatest_common_open(media), media, mediatest_common_close,
            0, "FAIL! media open fail.\n");
  FUN_CHECK(mediatest_common_prepare(media), media,
            mediatest_common_close, 1, "FAIL! media prepare fail\n");
  STATE_CHECK(PLAYER_PREPARED, media, mediatest_common_close,
              "recorder: prepare event return failed\n");
  FUN_CHECK(mediatest_common_start(media), media, mediatest_common_close,
            1, "FAIL! media start fail\n");
  STATE_CHECK(PLAYER_STARTED, media, mediatest_common_close,
              "recorder: start event return failed\n");
  sleep(media->time);
  FUN_CHECK(mediatest_common_close(media), media, mediatest_common_close,
            0, "FAIL! media close fail.\n");
  if (media->type == MEDIATEST_CAMERA)
    FUN_CHECK(mediatest_send("devsink@preview", "stop", NULL), media,
              mediatest_common_close, 1,
              "FAIL! url option do not given\n");
  sleep(2);
  media->stream_type = play_format;
  FUN_CHECK(mediatest_player_open(media), media, mediatest_common_close,
            0, "FAIL! media open fail.\n");
  FUN_CHECK(mediatest_common_prepare(media), media,
            mediatest_common_close, 1, "FAIL! media prepare fail\n");
  STATE_CHECK(PLAYER_PREPARED, media, mediatest_common_close,
              "player: prepare event return failed\n");
  FUN_CHECK(mediatest_duration(media), media, mediatest_common_close, 1,
            "FAIL! media get duration failed \n");
  if (abs(media->duration - media->time) < 3000)
    {
      syslog(LOG_ERR, "get duration not match\n");
      goto out;
    }

  FUN_CHECK(mediatest_common_start(media), media, mediatest_common_close,
            1, "FAIL! media start fail\n");
  STATE_CHECK(PLAYER_STARTED, media, mediatest_common_close,
              "player: start event return failed\n");
  sleep(3);
out:
  FUN_CHECK(mediatest_common_close(media), media, mediatest_common_close,
            0, "FAIL! media close fail.\n");

  free(media);
  media = NULL;

  syslog(LOG_INFO, "TEST PASSED\n");
  return 0;
}
