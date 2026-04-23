#include <media_api.h>
#include <nuttx/config.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

#include "media_graph_test.h"

/**********************************************./nu
 ******************************* Public Functions
 ****************************************************************************/
static struct mediatest_data *media;


int main(int argc, char *argv[])
{
  int len = 0;
  char line[256];
  int tmp;

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
  if (mediatest_player_open(media) < 0)
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
  tmp = media->rept;

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
              while (media->rept-- > 0)
                {
                  srand((unsigned int)time(NULL));
                  double random = (double)rand() / RAND_MAX;
                  media->position =
                      (int)(random * (media->duration - 2000));
                  syslog(LOG_INFO, "THE position is %d\n",
                         media->position);
                  mediatest_seek(media);
                }
              sleep(media->time);
              media->rept = tmp;
            }
          media->complete = false;
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