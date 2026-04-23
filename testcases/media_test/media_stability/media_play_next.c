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
         "\t\t-h: inner time \n"
         "\t\t-c: next times,default 1\n"
         "\t\t-e: set pause\n");

  exit(0);
}

int main(int argc, char *argv[])
{
  int ch = 0;
  int time_val = 10;
  int next_time = 1;
  bool paused = false;
  while ((ch = getopt(argc, argv, "h:c:e")) != EOF)
    {
      switch (ch)
        {
        case 'h':
          time_val = atoi(optarg);
          break;
        case 'c':
          next_time = atoi(optarg);
          break;
        case 'e':
          paused = true;
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
      for (int i = 0; i < next_time; ++i)
        {
          if (paused){
            test_play_pause_song();
          }
          test_play_next_song();
          usleep(200 * 1000);
        }
    }
  return 0;
}