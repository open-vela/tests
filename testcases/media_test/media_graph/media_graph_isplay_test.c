/****************************************************************************
 * apps/tests/testcases/media_test/media_graph/media_graph_isplay_test.c
 *
 * Name: media_graph_isplay_test
 * Example description:
 *  1. media_graph_isplay_test
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

  FUN_CHECK(mediatest_player_open(media), media, mediatest_common_close,
            0, "FAIL! media open fail.\n");
  FUN_CHECK(mediatest_common_prepare(media), media,
            mediatest_common_close, 1, "FAIL! media prepare %s fail\n",
            media->url);
  STATE_CHECK(PLAYER_PREPARED, media, mediatest_common_close,
              "player: prepare event return failed\n");
  int isplay = mediatest_player_isplaying(media);
  syslog(LOG_INFO, "before start, isplay is %d\n", isplay);
  if (isplay != 0)
    {
      syslog(LOG_ERR, "FAIL! media isplay is %d.\n", isplay);
      goto out;
    }
  FUN_CHECK(mediatest_common_start(media), media, mediatest_common_close,
            1, "FAIL! media start fail\n");
  STATE_CHECK(PLAYER_STARTED, media, mediatest_common_close,
              "player: start event return failed\n");
  isplay = mediatest_player_isplaying(media);
  syslog(LOG_INFO, "after start, isplay is %d\n", isplay);
  if (isplay != 1)
    {
      syslog(LOG_ERR, "FAIL! media isplay is %d.\n", isplay);
      goto out;
    }
  sleep(3);
  FUN_CHECK(mediatest_common_close(media), media,
            mediatest_common_close, 0, "FAIL! media close fail.\n");
  syslog(LOG_INFO, "PASS ! media_graph_isplay pass \n");
out:

  free(media);
  media = NULL;

  return 0;
}
