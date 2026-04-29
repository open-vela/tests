#include "media_graph_test.h"
#include <getopt.h>
#include <media_api.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

static int stop = 0;
static void sighandler(int signo) { stop = 1; }

int main(int argc, char *argv[])
{
  int direction = 1;
  signal(SIGALRM, sighandler);
  char stream_volume[30];

  struct mediatest_data *media =
      (struct mediatest_data *)malloc(sizeof(struct mediatest_data));
  FUN_CHECK(mediatest_setup(media), media, mediatest_common_close, 0,
            "FAIL! malloc failed\n");
  FUN_CHECK(mediatest_getopt(argc, argv, media), media,
            mediatest_common_close, 0,
            "FAIL! url option do not given\n");

  FUN_CHECK(mediatest_common_open(media), media, mediatest_common_close,
            0, "FAIL! media open fail.\n");

  FUN_CHECK(mediatest_common_prepare(media), media,
            mediatest_common_close, 1, "FAIL! media prepare fail\n");
  STATE_CHECK(PLAYER_PREPARED, media, mediatest_common_close,
              "player: prepare event return failed\n");
  FUN_CHECK(mediatest_common_start(media), media, mediatest_common_close,
            1, "FAIL! media start fail\n");
  STATE_CHECK(PLAYER_STARTED, media, mediatest_common_close,
              "player: start event return failed\n");
  sleep(3);

  alarm(media->time);
  sprintf(stream_volume, "%s%s", media->stream_type, "Volume");
  while (1)
    {
      media_policy_set_int(stream_volume, media->volume, 1);
      if (media->volume == 1)
        {
          direction = 1;
        }
      else if (media->volume == 10)
        {
          direction = -1;
        }
      media->volume = media->volume + direction;
      usleep(20 * 1000);
      if (stop == 1)
        {
          break;
        }
    }
  FUN_CHECK(mediatest_common_close(media), media, mediatest_common_close,
            1, "FAIL! media close fail.\n");

  return 0;
}