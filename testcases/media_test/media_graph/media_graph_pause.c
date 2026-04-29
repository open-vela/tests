/****************************************************************************
 * apps/tests/testcases/media_test/media_graph/media_graph_buffer_pause.c
 *
 * Name: media_graph_buffer_pause
 * Example description:
 *  1. media_graph_buffer_pause
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "media_graph_test.h"
#include <media_api.h>
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

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

  FUN_CHECK(mediatest_common_open(media), media, mediatest_common_close,
            0, "FAIL! media open fail.\n");
  if (media->type == MEDIATEST_PLAYER)
    {
      FUN_CHECK(mediatest_player_loop(media), media,
                mediatest_common_close, 1,
                "FAIL! media set loop fail.\n");
    }

  FUN_CHECK(mediatest_common_prepare(media), media,
            mediatest_common_close, 1, "FAIL! media prepare fail\n");
  STATE_CHECK(PLAYER_PREPARED, media, mediatest_common_close,
              "player: prepare event return failed\n");
  FUN_CHECK(mediatest_common_start(media), media, mediatest_common_close,
            1, "FAIL! media start fail\n");
  STATE_CHECK(PLAYER_STARTED, media, mediatest_common_close,
              "player: start event return failed\n");

  while (media->rept--)
    {
      sleep(3);
      FUN_CHECK(mediatest_common_pause(media), media,
                mediatest_common_close, 1, "FAIL! media pause fail\n");
      STATE_CHECK(PLAYER_PAUSED, media, mediatest_common_close,
                  "player: pause event return failed\n");
      sleep(3);
      FUN_CHECK(mediatest_common_start(media), media,
                mediatest_common_close, 1, "FAIL! media start fail\n");
      STATE_CHECK(PLAYER_STARTED, media, mediatest_common_close,
                  "player: start event return failed\n");
    }
  FUN_CHECK(mediatest_common_close(media), media, mediatest_common_close,
            0, "FAIL! media close fail.\n");

  syslog(LOG_INFO,
         "PASS ! media pause pass, media type: %d mode: %d pass \n",
         media->type, media->mode);

  free(media);
  media = NULL;

  return 0;
}
