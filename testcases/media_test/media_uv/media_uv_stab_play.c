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

int main(int argc, char *argv[])
{
  media = (struct mediatest_data *)malloc(sizeof(struct mediatest_data));
  int count = 0;
  mediatest_setup(media);
  mediatest_getopt(argc, argv, media);
  mediatest_uv_player_enter();
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
          if (++count >= 4)
            {
              count = 0;
              UV_EXECUTE(mediatest_app_dur_pos, media);
            }
          usleep(250 * 1000);
        }
    out:
      media->complete = false;
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
  return 0;
}