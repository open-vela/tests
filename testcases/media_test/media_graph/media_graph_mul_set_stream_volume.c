/****************************************************************************
 * apps/tests/testcases/media_test/media_graph/media_graph_loop_open.c
 *
 * Name: media_graph_loop_open
 * Example description:
 *  1. media_graph_loop_open
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
  struct mediatest_data *media =
      (struct mediatest_data *)malloc(sizeof(struct mediatest_data));
  FUN_CHECK(mediatest_setup(media), media, mediatest_common_close, 0,
            "FAIL! malloc failed\n");
  mediatest_getopt(argc, argv, media);

  for (i = 0; media->stream_type[i] != '\0'; i++)
    {
      if (media->stream_type[i] == ',')
        streams_len = streams_len + 1;
    }

  i = 0;
  char *stream_types[streams_len];
  int open[15] = {0};
  char stream_volume[30];

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
      float vol = media->volume + 1;
      if (vol > 10)
        vol = 0;

      medias[i]->volume = vol;
      media->volume += 1;
    }

  for (i = 0; i < streams_len; i++)
    {
      RET_CHECK(mediatest_common_open(medias[i]),
                "FAIL! media open fail.\n");
      open[i] = 1;
      RET_CHECK(mediatest_common_prepare(medias[i]),
                "FAIL! media prepare fail\n");
      ST_CHECK(PLAYER_PREPARED, medias[i],
               "player: prepare event return failed\n");
      RET_CHECK(mediatest_common_start(medias[i]),
                "FAIL! media start fail\n");
    }
  sleep(media->time);
  for (i = 0; i < streams_len; i++)
    {
      int value;
      sprintf(stream_volume, "%s%s", stream_types[i], "Volume");
      syslog(LOG_INFO, "stream %s\n", stream_volume);
      if (media_policy_set_int(stream_volume, medias[i]->volume, 1) < 0)
        {
          syslog(LOG_ERR, "media_policy_set_int failed\n");
          goto out;
        }
      media_policy_get_int(stream_volume, &value);
      if (value != medias[i]->volume)
        {
          syslog(LOG_ERR, "media_policy_get_int value not equal set\n");
          goto out;
        }
    }

  syslog(LOG_INFO, "TEST PASS! media_graph_set stream volume pass.\n");

out:

  for (i = 0; i < streams_len; i++)
    {
      if (open[i] == 1)
        {
          mediatest_common_close(medias[i]);
        }
    }

  for (i = 0; i < streams_len; i++)
    {
      free(medias[i]);
      medias[i] = NULL;
    }

  free(media);
  media = NULL;

  return 0;
}
