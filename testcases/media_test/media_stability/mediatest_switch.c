#include "audio_focus.h"
#include "audio_manager_test.h"
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <getopt.h>
#include "media_graph_test.h"

static void show_usage(void)
{
    syslog(LOG_WARNING,
           "Usage: CMD [-t <inter_time>]\n"
           "\t\t-t: inner time to pause\n"
           "\t\t-u: Alarm and Ring play url\n");

    exit(0);
}

int main(int argc, char *argv[])
{
    int time_val = 10;
    char *url = NULL;
    int ch = 0;
    while ((ch = getopt(argc, argv, "h:u:")) != EOF)
    {
        switch (ch)
        {
        case 'h':
            time_val = atoi(optarg);
            break;
        case 'u':
            url = optarg;
            break;
        default:
            show_usage();
            break;
        }
    }
    test_audio_manager_init();
    while (true)
    {
        test_play_cur_song();
        sleep(time_val);
        test_player_play(PLAYER_ALARM, url);
        sleep(time_val);
        test_play_common_stop(PLAYER_ALARM);
        sleep(time_val);
        test_player_play(PLAYER_RING, url);
        sleep(time_val);
        test_play_common_stop(PLAYER_RING);
        sleep(time_val);
    }
}