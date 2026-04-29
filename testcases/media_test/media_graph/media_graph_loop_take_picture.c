/****************************************************************************
 * apps/tests/testcases/media_test/media_graph/media_graph_loop_take_picture_test.c
 *
 * Name: media_graph_loop_take_picture_test
 * Example description:
 *  1. media_graph_loop_take_picture_test
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

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int loop_take_picture(int loop, bool preview,char *url, char *stream_type,int num)
{
  struct mediatest_data *media =(struct mediatest_data *)malloc(sizeof(struct mediatest_data));

  RET_CHECK(mediatest_setup(media), "FAIL! malloc failed\n");
  media->url = url;
  media->rept = num;
  media->stream_type=stream_type;

  for (int i = 0; i < loop; i++)
    {
      media->state = PLAYER_IDLE;
      if (preview)
      {
        RET_CHECK(mediatest_send("devsink@preview", "start", NULL),"FAIL! media send pause failed\n");
      }
      RET_CHECK(mediatest_recorder_take_picture(media),"FAIL! media take picture fail\n");
      STATE_CHECK(PLAYER_STOPPED, media, mediatest_common_close, "media recorder state check failed\n");
      if (preview)
      {
        RET_CHECK(mediatest_send("devsink@preview", "stop", NULL),"FAIL! media send play failed\n");
      }
    }
  free(media);
  media = NULL;
  return 0;
out:
  if (media)
    {
      free(media);
      media = NULL;
    };
  return -1;

}

static void show_usage(void)
{
  syslog(LOG_WARNING,
         "Usage: CMD [-p] [-s <stream_type>] [-l <loop_time>] [-r]\n"
         "\t\t-p: set whether preview, default not\n"
         "\t\t-l: loop times,default 100\n"
         "\t\t-u: filename\n"
         "\t\t-s: streame type\n"
         "\t\t-t: Number of photos taken at a time "
         );
  exit(0);
}

int main(int argc, FAR char *argv[])
{
  if (argc == 1)
    show_usage();
  bool preview = false;
  char *url =NULL;
  int ch = 0;
  int num = 1;
  int loop = 100;
  char *stream_type = NULL;

  while ((ch = getopt(argc, argv, "pl:u:s:t:")) != EOF)
    {
      switch (ch)
        {
        case 'p':
          preview = true;
          break;
        case 'l':
          loop = atoi(optarg);
          break;
        case 'u':
          url = optarg;
          break;
        case 's':
          stream_type = optarg;
          break;
        case 't':
          num = atoi(optarg);
          break;
        default:
          show_usage();
          break;
        }
    }
  if (loop_take_picture(loop, preview,url, stream_type,num) < 0)
      {
        syslog(LOG_ERR, "FAIL! media loop take picture fail.\n");
        return -1;
      }
  syslog(LOG_INFO, "TEST PASS! media_graph_loop_take_picture_test pass.\n");
  return 0;
}
