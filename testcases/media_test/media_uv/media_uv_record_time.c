/****************************************************************************
 * apps/tests/testcases/media_test/media_graph/media_graph_buffer_stop.c
 *
 * Name: media_graph_buffer_stop
 * Example description:
 *  1. media_graph_buffer_stop
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
  struct mediatest_app *player_test;
  mediatest_setup(media);
  mediatest_getopt(argc, argv, media);
  mediatest_uv_player_enter();
  UV_EXECUTE(mediatest_app_init, media);
  ST_CHECK(PLAYER_STARTED, media,
           "FAILED ! uv recorder started failed\n");
  sleep(media->time);
  media->stop_flag = true;
  usleep(500 * 1000);
  UV_EXECUTE(mediatest_app_stop, media);
  ST_CHECK(PLAYER_STOPPED, media, "FAILED ! uv recorder exit failed\n");
  UV_EXECUTE(mediatest_app_exit, media);
  sleep(2);
  media->type = MEDIATEST_UVPLAYER;
  media->stream_type = "Music";
  media->mode = MODE_URL;
  UV_EXECUTE(mediatest_app_init, media);
  ST_CHECK(PLAYER_STARTED, media, "FAILED ! uv player started failed\n");

  UV_EXECUTE(mediatest_duration, media);
  UV_WAIT(PLAYER_GET_DURATION, media,
          "FAILED ! uv get duration failed\n");
  int diff = media->duration - media->time * 1000;
  if (abs(diff) > 1000)
    {
      syslog(LOG_ERR, "get duration not suit,diff is %d, test FAILED\n", diff);
      goto out;
    }
  syslog(LOG_INFO, "media uvplayer record time PASSED!\n");

out:
  media->stop_flag = true;
  usleep(500 * 1000);
  UV_EXECUTE(mediatest_app_stop, media);
  UV_EXECUTE(mediatest_app_exit, media);
  mediatest_uv_exit();
  free(media);
  media = NULL;
  return 0;
}
