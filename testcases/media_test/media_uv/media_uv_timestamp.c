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
  void *focus_handle = NULL;
  void *stream_handle = NULL;
  struct mediatest_data *media =
      (struct mediatest_data *)malloc(sizeof(struct mediatest_data));
  mediatest_setup(media);
  mediatest_uv_player_enter();
  mediatest_getopt(argc, argv, media);
  struct mediatest_app *player_test;
  UV_EXECUTE(mediatest_common_open, media);
  sleep(1);
  UV_EXECUTE(mediatest_common_prepare, media);
  sleep(1);
  if(media->type == MEDIATEST_UVPLAYER){
    stream_handle = media->handle;
    media->type = MEDIATEST_UVFOCUS;
    UV_EXECUTE(mediatest_uv_focus_request, media);
    sleep(1);
    focus_handle = media->handle;
    media->type = MEDIATEST_UVPLAYER;
    media->handle = stream_handle;
  }
  UV_EXECUTE(mediatest_common_start, media);
  sleep(media->time);
  media->position = 5000;
  UV_EXECUTE(mediatest_seek, media);
  sleep(1);
  UV_EXECUTE(mediatest_player_set_volume, media);
  sleep(1);
  UV_EXECUTE(mediatest_player_get_volume, media);
  sleep(1);
  UV_EXECUTE(mediatest_app_pause, media);
  sleep(1);
  UV_EXECUTE(mediatest_common_start, media);
  sleep(1);
  UV_EXECUTE(mediatest_player_isplaying, media);
  sleep(1);
  UV_EXECUTE(mediatest_duration, media);
  sleep(1);
  UV_EXECUTE(mediatest_position, media);
  sleep(1);
  media->stop_flag = true;
  usleep(500 * 1000);
  UV_EXECUTE(mediatest_app_stop, media);
  sleep(1);
  syslog(LOG_INFO, "media uvplayer timestamp PASSED!\n");

  UV_EXECUTE(mediatest_app_exit, media);
  if(media->type == MEDIATEST_UVPLAYER){
    media->type = MEDIATEST_UVFOCUS;
    media->handle = focus_handle;
    UV_EXECUTE(mediatest_common_close, media);
    sleep(1);
    media->type = MEDIATEST_UVPLAYER;
    media->handle = stream_handle;
  }
  mediatest_uv_exit();
  free(media);
  media = NULL;
  return 0;
}
