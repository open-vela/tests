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
         "\t\t-a: playing stream\n"
         "\t\t-b: request stream\n"
         "\t\t-u: playing url\n"
         "\t\t-v: request url\n"
         "\t\t-t: playing time\n");

  exit(0);
}

static int get_type_of_player(char *stream)
{
  if (!stream)
    return -1;
  if (!strcmp("Music", stream))
    {
      return PLAYER_MUSIC;
    }
  else if (!strcmp("Alarm", stream))
    {
      return PLAYER_ALARM;
    }
  else if (!strcmp("Notify", stream))
    {
      return PLAYER_NOTIFY;
    }
  else if (!strcmp("TTS", stream))
    {
      return PLAYER_TTS;
    }
  else if (!strcmp("SCO", stream))
    {
      return PLAYER_SCO;
    }
  else if (!strcmp("Ring", stream))
    {
      return PLAYER_RING;
    }
  else if (!strcmp("Enforced", stream))
    {
      return PLAYER_ENFORCED;
    }
  else if (!strcmp("Record", stream))
    {
      return PLAYER_RECORD;
    }
  else if (!strcmp("Health", stream))
    {
      return PLAYER_HEALTH;
    }
  else if (!strcmp("Sport", stream))
    {
      return PLAYER_SPORT;
    }
  else if (!strcmp("Info", stream))
    {
      return PLAYER_INFO;
    }
  else
    {
      return 0;
    }
}

int main(int argc, char *argv[])
{
  int ch = 0;
  char *play_stream = "Music";
  char *req_stream = "Alarm";
  char *play_url = "/data/1.mp3";
  char *req_url = "/data/1.mp3";

  int time_val = 10;
  while ((ch = getopt(argc, argv, "a:b:u:v:t:")) != EOF)
    {
      switch (ch)
        {
        case 'a':
          play_stream = optarg;
          break;
        case 'b':
          req_stream = optarg;
          break;
        case 'u':
          play_url = optarg;
          break;
        case 'v':
          req_url = optarg;
          break;
        case 't':
          time_val = atoi(optarg);
          break;
        default:
          show_usage();
          break;
        }
    }
  test_audio_manager_init();
  test_player_play(get_type_of_player(play_stream), play_url);
  sleep(time_val);
  test_player_play(get_type_of_player(req_stream), req_url);
  sleep(time_val);
  test_delete_all_play();
  return 0;
}