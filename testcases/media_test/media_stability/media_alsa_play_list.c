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

int main(int argc, char *argv[])
{
  char line[512];
  char url[256];
  int len = 0;
  mediatest_alsa_t *media =
      (mediatest_alsa_t *)malloc(sizeof(mediatest_alsa_t));
  mediatest_alsa_setup(media);
  mediatest_alsa_getopt(argc, argv, media);
  syslog(LOG_INFO, "media_alsa_play_list start\n");

  FILE *fp = fopen(media->file, "r");
  if (!fp)
    {
      syslog(LOG_ERR, "FAIL! media_alsa_play_list file open fail.\n");
      free(media);
      media = NULL;
      return -1;
    }

  while (1)
    {
      if (fgets(line, sizeof(line), fp) != NULL)
        {
          len = strlen(line);

          while (isspace(line[len - 1]))
            len--;

          line[len] = '\0';

          char *token = strtok(line, " ");
          if (token != NULL)
            strcpy(url, token);

          token = strtok(NULL, " ");
          if (token != NULL)
            {
              char *sub_token = strtok(token, ":");

              if (sub_token != NULL)
                {
                  media->channels = atoi(sub_token);
                }

              sub_token = strtok(NULL, ":");
              if (sub_token != NULL)
                {
                  media->bits_per_sample = atoi(sub_token);
                }

              sub_token = strtok(NULL, ":");
              if (sub_token != NULL)
                {
                  media->sample_rate = atoi(sub_token);
                }
            }
          media->url = &url[0];

          syslog(0, " %s line %d PLAY %s !!!!!!!!!!!!!!!! \n", __func__,
                 __LINE__, media->url);

          if (mediatest_alsa_open(media) < 0)
            {
              syslog(LOG_ERR, "FAIL! mediastab open fail.\n");
              free(media);
              media = NULL;
              fclose(fp);
              return 0;
            }
          if (mediatest_alsa_volume(media) < 0)
            {
              syslog(LOG_ERR, "FAIL! mediastab set volume fail.\n");
            }

          if (mediatest_alsa_prepare(media) < 0)
            {
              syslog(LOG_ERR, "FAIL! mediastb start fail\n");
              continue;
            }
          sleep(media->interval);

          while (!media->complete)
            {
              if (media->interval == 0)
                {
                  usleep(250 * 1000);
                }
              else
                {
                  mediatest_alsapause(1);
                  sleep(1);
                  mediatest_alsapause(0);
                  sleep(media->interval);
                }
            }
          media->complete = false;

          if (mediatest_alsa_close(media) < 0)
            syslog(LOG_ERR, "FAIL! media close fail.\n");
        }
      else
        {
          fseek(fp, 0L, SEEK_SET);
        }

      memset(line, 0, sizeof(line));
      memset(url, 0, sizeof(url));
      len = 0;
    }

  free(media);
  media = NULL;

  fclose(fp);

  return 0;
}