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
/****************************************************************************
 * apps/tests/testcases/media_test/media_uv/media_uv_loop_play.c
 *
 * Name: media_uv_loop_play
 * Example description:
 *  1. media_uv_loop_play
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
#include <string.h>
#include <syslog.h>
#include <unistd.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  int streams_len = 1;
  int i = 0;
  struct mediatest_app *player_test;
  struct mediatest_data *media =
      (struct mediatest_data *)malloc(sizeof(struct mediatest_data));
  mediatest_setup(media);
  mediatest_getopt(argc, argv, media);
  mediatest_uv_player_enter();

  for (i = 0; media->stream_type[i] != '\0'; i++)
    {
      if (media->stream_type[i] == ',')
        streams_len = streams_len + 1;
    }

  i = 0;
  char *stream_types[streams_len];
  int open[15] = {0};

  char *token = strtok(media->stream_type, ",");
  while (token != NULL)
    {
      stream_types[i++] = token;
      token = strtok(NULL, ",");
    }

  struct mediatest_data *medias[streams_len];

  for (i = 0; i < streams_len; i++)
    {
      medias[i] =
          (struct mediatest_data *)malloc(sizeof(struct mediatest_data));
      if (!medias[i])
        {
          syslog(LOG_ERR, "FAIL! malloc media %d fail.\n", i);
          goto out;
        }

      RET_CHECK(mediatest_setup(medias[i]), "FAIL! malloc failed\n");
      medias[i]->type = media->type;
      medias[i]->stream_type = stream_types[i];
      medias[i]->url = media->url;
      medias[i]->volume = media->volume;
      open[i] = 0;
    }
  for (i = 1; i < streams_len; i++)
    {
      float vol = media->volume + 0.1;
      if (vol > 1)
        vol = 0.1;

      medias[i]->volume = vol;
      media->volume += 0.1;
    }

  for (i = 0; i < streams_len; i++)
    {
      for (int j = 0; j < media->rept; ++j)
        {
          UV_EXECUTE(mediatest_app_init, medias[i]);
          open[i] = 1;
          ST_CHECK(PLAYER_STARTED, medias[i],
                   "FAILED ! uv player started failed\n");
          sleep(media->time);
          medias[i]->stop_flag = true;
          usleep(500 * 1000);

          UV_EXECUTE(mediatest_app_stop, medias[i]);
          UV_EXECUTE(mediatest_app_exit, medias[i]);
          UV_WAIT(PLAYER_CLOSED, medias[i], "mediatest closecb failed\n");
          open[i] = 0;
        }
    }

  syslog(LOG_INFO, "TEST PASS! media_uv_loop_play pass.\n");

out:

  for (i = 0; i < streams_len; i++)
    {
      if (open[i] == 1)
        {
          medias[i]->stop_flag = true;
          usleep(500 * 1000);
          UV_EXECUTE(mediatest_app_stop, medias[i]);
          UV_EXECUTE(mediatest_app_exit, medias[i]);
        }
    }
  mediatest_uv_exit();
  for (i = 0; i < streams_len; i++)
    {
      free(medias[i]);
      medias[i] = NULL;
    }

  free(media);
  media = NULL;

  return 0;
}
