/****************************************************************************
 * apps/tests/testcases/media_test/media_graph/media_graph_buffer_start.c
 *
 * Name: media_graph_buffer_start
 * Example description:
 *  1. media_graph_buffer_start
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
  int volume;
  mediatest_setup(media);
  mediatest_getopt(argc, argv, media);
  mediatest_uv_player_enter();
  volume = (int)media->volume;
  struct mediatest_app *player_test;

  UV_EXECUTE(mediatest_app_init, media);
  ST_CHECK(PLAYER_STARTED, media, "FAILED ! uv player started failed\n");
  sleep(media->time);

  UV_EXECUTE(mediatest_uv_policy_set_stream_volume, media);
  UV_EXECUTE(mediatest_uv_policy_increase_stream_volume, media);

  UV_EXECUTE(mediatest_uv_policy_get_stream_volume, media);
  UV_WAIT(PLAYER_GET_VOLUME, media,
          "FAILED ! uv player get volume failed\n");
  if (volume != ((int)media->volume - 1))
    {
      syslog(LOG_ERR, "increase Volume failed\n");
      goto out;
    }
  UV_EXECUTE(mediatest_uv_policy_decrease_stream_volume, media);
  UV_EXECUTE(mediatest_uv_policy_get_stream_volume, media);
  UV_WAIT(PLAYER_GET_VOLUME, media,
          "FAILED ! uv player get volume failed\n");
  if (volume != (int)media->volume)
    {
      syslog(LOG_ERR, "decrease Volume failed\n");
      goto out;
    }
  media->stop_flag = true;
  usleep(500 * 1000);
  UV_EXECUTE(mediatest_app_stop, media);
  ST_CHECK(PLAYER_STOPPED, media, "FAILED ! uv player stoped failed\n");
  syslog(LOG_INFO, "media uvplayer set stream volume PASSED!\n");
out:
  UV_EXECUTE(mediatest_app_exit, media);
  mediatest_uv_exit();
  free(media);
  media = NULL;
  return 0;
}