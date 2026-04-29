#include <media_api.h>
#include <nuttx/config.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

#include "media_graph_test.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/
static struct mediatest_data *media;

int main(int argc, char *argv[])
{
  int len = 0;
  char line[256];
  int count = 0;

  media = (struct mediatest_data *)malloc(sizeof(struct mediatest_data));
  FUN_CHECK(mediatest_setup(media), media, mediatest_common_close, 0,
            "FAIL! malloc failed\n");

  mediatest_getopt(argc, argv, media);

  FILE *fp = fopen(media->file, "r");
  if (fp == NULL)
    {
      syslog(LOG_ERR, "FAIL!, open file failed\n");
      free(media);
      media = NULL;
      return -1;
    }
  if (mediatest_common_open(media) < 0)
    {
      syslog(LOG_ERR, "FAIL! mediastab open fail.\n");
      free(media);
      media = NULL;
      fclose(fp);
      return 0;
    }

  if (mediatest_player_set_volume(media) < 0)
    {
      syslog(LOG_ERR, "FAIL! mediastab set volume fail.\n");
      goto exit;
    }

  while (1)
    {
      if (mediatest_common_stop(media) < 0)
        {
          syslog(LOG_ERR, "FAIL! mediastb close fail.\n");
          mediatest_dump();
        }
      if (fgets(line, sizeof(line), fp) != NULL)
        {
          len = strlen(line);

          while (isspace(line[len - 1]))
            len--;

          line[len] = '\0';

          syslog(0, " %s line %d PLAY %s !!!!!!!!!!!!!!!! \n", __func__,
                 __LINE__, line);
          media->url = line;
          if (mediatest_common_prepare_retry(media, 0) < 0)
            {
              syslog(LOG_ERR, "mediastb prepare finally failed!!!!\n");
              mediatest_dump();
              continue;
            }

          if (mediatest_common_start(media) < 0)
            {
              syslog(LOG_ERR, "FAIL! mediastb start fail\n");
              mediatest_dump();
              continue;
            }
          mediatest_dump();

          while (!media->complete)
            {
              if (++count >= 4)
                {
                  count = 0;
                  mediatest_position(media);
                  syslog(LOG_INFO,
                         "Current position is %d, the duration is %d\n",
                         media->position, media->duration);
                }
              usleep(250 * 1000);
            }
          media->complete = false;
          int distance = abs(media->position - media->duration);
          syslog(LOG_INFO,
                 "the song final pos is %d ms, duration is %d ms, gap "
                 "%d ms\n",
                 media->position, media->duration, distance);
          if (distance > 1000)
            {
              syslog(LOG_WARNING,
                     "WARINGING !!! mediastab playing not goto end\n");
            }

          mediatest_dump();
        }
      else
        {
          fseek(fp, 0L, SEEK_SET);
        }

      memset(line, 0, sizeof(line));
    }
exit:
  if (mediatest_common_close(media) < 0)
    syslog(LOG_ERR, "FAIL! media close fail.\n");

  free(media);
  media = NULL;

  fclose(fp);

  return 0;
}