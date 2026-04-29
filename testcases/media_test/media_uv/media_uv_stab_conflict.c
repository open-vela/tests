#include <media_api.h>
#include <media_player.h>
#include <nuttx/config.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include "media_graph_test.h"

/**********************************************./nu
 ******************************* Public Functions
 ****************************************************************************/
static struct mediatest_app *player_test;
static struct mediatest_data *media;
static struct mediatest_data *media1;


int main(int argc, char *argv[])
{
  int len = 0;
  char line[256];
  media = (struct mediatest_data *)malloc(sizeof(struct mediatest_data));
  media1 =
      (struct mediatest_data *)malloc(sizeof(struct mediatest_data));
  mediatest_setup(media);
  mediatest_setup(media1);
  mediatest_getopt(argc, argv, media);
  mediatest_uv_player_enter();
  media->focus_type = "Music";
  media1->url = media->url;
  media1->stream_type = "Alarm";
  media1->type = media->type;
  media1->focus_type = "Alarm";
  mediatest_load_play_list(media);
  UV_EXECUTE(mediatest_app_init, media);
  ST_CHECK(PLAYER_STARTED, media,
           "FAILED ! mediatest_stab_uv player started failed\n");
  UV_EXECUTE(mediatest_uv_policy_get_stream_volume, media);
  UV_WAIT(PLAYER_GET_VOLUME, media,
          "FAILED ! uv player get volume failed\n");
  if ((int)media->volume <= 0)
    {
      syslog(LOG_ERR, "volume get failed\n");
      goto exit;
    }

  while (1)
    {
      while (!media->complete)
        {
          syslog(LOG_INFO, "Alarm init\n");
          UV_EXECUTE(mediatest_app_init, media1);
          sleep(rand() % 4 + 4);
          media->stop_flag = true;
          usleep(500 * 1000);
          UV_EXECUTE(mediatest_app_stop, media1);
          UV_EXECUTE(mediatest_app_exit, media1);
          if (media->ex)
            {
              UV_EXECUTE(mediatest_app_next, media);
            }
          syslog(LOG_INFO, "Alarm exit\n");
          sleep(10);
        }
      media->complete = false;
    out:
      UV_EXECUTE(mediatest_app_next, media);
      ST_CHECK(PLAYER_STARTED, media,
               "FAILED ! mediatest_stab_uv player started failed\n");
    }
exit:
  media->stop_flag = true;
  usleep(500 * 1000);
  UV_EXECUTE(mediatest_app_stop, media);
  UV_EXECUTE(mediatest_app_exit, media);
  mediatest_uv_exit();
  free(media);
  media = NULL;
  free(media1);
  media1 = NULL;
  return 0;
}