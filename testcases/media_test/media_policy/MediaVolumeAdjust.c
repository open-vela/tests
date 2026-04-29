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

static void show_usage(void)
{
  syslog(LOG_WARNING,
         "Usage: CMD [-s] [-u <url>] [-h <execcute_times>] [-v "
         "<stream_type>] [-e <record_type>]\n"
         "\t\t-s: set stream type, default Music\n"
         "\t\t-h: set running time, default 10\n"
         "\t\t-v: set init volume, default 5\n"
         "\t\t-e: set volume order, default 10\n");

  exit(0);
}

int main(int argc, char *argv[])
{
  stop = 0;
  int ch = 0;
  char *stream = "Music";
  int value = 5;
  int direction = 1;
  int loop_time = 10;
  int volume_order = 10;
  char stream_volume[30];
  signal(SIGALRM, sighandler);

  while ((ch = getopt(argc, argv, "s:h:v:e:")) != EOF)
    {
      switch (ch)
        {
        case 's':
          stream = optarg;
          break;
        case 'h':
          loop_time = atoi(optarg);
          break;
        case 'v':
          value = atoi(optarg);
          break;
        case 'e':
          volume_order = atoi(optarg);
          break;
        default:
          show_usage();
          break;
        }
    }

  alarm(loop_time);
  sprintf(stream_volume, "%s%s", stream, "Volume");
  while (1)
    {
      media_policy_set_int(stream_volume, value, 1);
      if (value == 1)
        {
          direction = 1;
        }
      else if (value == volume_order)
        {
          direction = -1;
        }
      value = value + direction;
      usleep(20 * 1000);
      if (stop == 1)
        {
          break;
        }
    }
  return 0;
}