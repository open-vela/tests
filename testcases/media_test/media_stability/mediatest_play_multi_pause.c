#include "audio_focus.h"
#include "audio_manager_test.h"
#include "audio_player.h"
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

static void show_usage(void)
{
  syslog(LOG_WARNING,
         "Usage: CMD [-t <inter_time>]\n"
         "\t\t-h: inner time to play Music\n"
         "\t\t-u: Alarm play url\n"
         "\t\t-e: stop Alarm and play next Music\n");

  exit(0);
}

int main(int argc, char *argv[])
{
  int ch = 0;
  int time_val = 10;
  int play_time = 2;
  char *url = NULL;
  bool next = false;
  while ((ch = getopt(argc, argv, "h:u:e:")) != EOF)
    {
      switch (ch)
        {
        case 'h':
          time_val = atoi(optarg);
          break;
        case 'u':
          url = optarg;
          break;
        case 'e':
          next = true;
          break;
        default:
          show_usage();
          break;
        }
    }
  test_audio_manager_init();
  test_play_cur_song();
  while (true)
    {
      sleep(time_val);
      test_player_play(PLAYER_ALARM, url);
      sleep(play_time);
      test_player_stop(PLAYER_ALARM);
      if (next)
        {
          sleep(1);
          test_play_next_song();
        }
    }
  return 0;
}