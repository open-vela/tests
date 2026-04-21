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
#include <unistd.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/
static int stop = 0;
static void sighandler(int signo) { stop = 1; }

int main(int argc, FAR char *argv[])
{
  int direction = 1;
  stop = 0;
  struct mediatest_data *media =
      (struct mediatest_data *)malloc(sizeof(struct mediatest_data));
  mediatest_setup(media);
  mediatest_getopt(argc, argv, media);
  mediatest_uv_player_enter();
  signal(SIGALRM, sighandler);
  struct mediatest_app *player_test;
  UV_EXECUTE(mediatest_app_init, media);
  ST_CHECK(PLAYER_STARTED, media, "FAILED ! uv player started failed\n");
  sleep(3);
  alarm(media->time);
  while (1)
    {
      UV_EXECUTE(mediatest_uv_policy_set_stream_volume, media);
      UV_EXECUTE(mediatest_uv_policy_get_stream_volume, media);
      UV_WAIT(PLAYER_GET_VOLUME, media,
              "FAILED ! uv player get volume failed\n");
      if (media->volume == 1)
        {
          direction = 1;
        }
      else if (media->volume == 10)
        {
          direction = -1;
        }
      media->volume = media->volume + direction;
      if (stop == 1)
        {
          break;
        }
    }
  syslog(LOG_INFO, "mediastab adjust volume PASSED\n");
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