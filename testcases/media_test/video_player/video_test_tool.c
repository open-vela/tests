#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <math.h>
#include <poll.h>
#include <pthread.h>
#include <stdlib.h>
#include <system/readline.h>
#include <unistd.h>
#ifdef CONFIG_LIBUV_EXTENSION
#include <uv.h>
#include <uv_async_queue.h>
#endif
#include "video_test.h"

#include <media_api.h>
#include <syslog.h>

static video_pri *chain;

typedef int (*videotool_func)(int argc, char **argv);
static test_song_entry_t *g_video_entry;

typedef struct videotool_cmd_s
{
    const char *cmd;      /* The command text */
    videotool_func pfunc; /* Pointer to command handler */
    const char *help;     /* The help text */
} videotool_cmd_t;

static inline void video_parse_cmd(int size, char *info[], video_pri *pri);
static int videotest_play(int argc, char **argv);
static int videotest_stop(int argc, char **argv);
static int videotest_quit(int argc, char **argv);
static int videotest_isplay(int argc, char **argv);
static int videotest_next_song(int argc, char **argv);
static int videotest_prev_song(int argc, char **argv);
static int videotest_pause(int argc, char **argv);
static int videotest_resume(int argc, char **argv);

videotool_cmd_t g_videotest_cmds[] = {
    {"play", videotest_play, "play music (play video)"},
    {"stop", videotest_stop, "stop music (stop video)"},
    {"q", videotest_quit, "quit video"},
    {"isplay", videotest_isplay, "isplay video"},
    {"next", videotest_next_song, "play next video"},
    {"prev", videotest_prev_song, "play prev video"},
    {"pause", videotest_pause, "pause video"},
    {"resume", videotest_resume, "resume video"},
    {0},
};

static int videotool_cmd_help(videotool_cmd_t cmds[]);

static int videotest_play(int argc, char **argv)
{
    memset(chain, 0, sizeof(video_pri));
    if (g_video_entry)
        chain[0].test_song_entry = g_video_entry;
    video_parse_cmd(argc, argv, chain);
    chain->ops = VIDEO_CTRL_PLAY;
    send_msg_video(chain);
    return 0;
}
static int videotest_stop(int argc, char **argv)
{
    chain->ops = VIDEO_CTRL_STOP;
    send_msg_video(chain);
    return 0;
}
static int videotest_quit(int argc, char **argv)
{
    chain->ops = VIDEO_CTRL_QUIT;
    send_msg_video(chain);
    sleep(1);
    return 0;
}
static int videotest_isplay(int argc, char **argv)
{
    chain->ops = VIDEO_CTRL_GET_ISPLAYING;
    send_msg_video(chain);
    return 0;
}
static int videotest_next_song(int argc, char **argv)
{
    chain->ops = VIDEO_CTRL_NEXT;
    send_msg_video(chain);
    return 0;
}
static int videotest_prev_song(int argc, char **argv)
{
    chain->ops = VIDEO_CTRL_PREV;
    send_msg_video(chain);
    return 0;
}
static int videotest_pause(int argc, char **argv)
{
    chain->ops = VIDEO_CTRL_PAUSE;
    send_msg_video(chain);
    return 0;
}
static int videotest_resume(int argc, char **argv)
{
    chain->ops = VIDEO_CTRL_RESUME;
    send_msg_video(chain);
    return 0;
}

static int videotool_cmd_help(videotool_cmd_t cmds[])
{
    int i;

    for (i = 0; cmds[i].cmd; i++)
        printf("%-16s %s\n", cmds[i].cmd, cmds[i].help);

    return 0;
}

static inline void video_parse_cmd(int size, char *info[], video_pri *pri)
{
    pri->url = "/data/1.mp4";
    pri->state = VIDEO_PLAYSTATE_STOP;
    pri->time = 5000;

    char ch;
    while ((ch = getopt(size, info, "u:o:f:a")) != -1)
    {
        switch (ch)
        {
        case 'u':
            if (optarg)
            {
                pri->url = optarg;
            }
            break;
        case 'o':
            if (optarg)
            {
                pri->option = optarg;
            }
            break;
        case 'f':
            if (optarg)
            {
                pri->file = optarg;
            }
            break;
        case 'a':
            if (optarg)
            {
                pri->autoplay = true;
            }
            break;
        default:
            syslog(
                LOG_INFO,
                "\nUsage:  graphics_test %s [-h] -u <url> -o <option> -f <file> -a\n",
                info[0]);
            syslog(LOG_INFO, "-h             help\n");
            syslog(LOG_INFO,
                   "-u <url>      the music that you want to play\n");
            syslog(LOG_INFO,
                   "-f <file>      the file that you want to play\n");
            syslog(LOG_INFO,
                   "-o <option>    the option for preparing player\n");
            syslog(LOG_INFO,
                   "-a   the stability for player\n");
            return;
        }
    }
}

static int videotool_execute(char *buffer)
{
    char *argv[30] = {NULL};
    char *saveptr = NULL;
    int ret = 0;
    int argc;
    int x;

    argv[0] = strtok_r(buffer, " ", &saveptr);
    for (argc = 1; argc < 50 - 1; argc++)
    {
        argv[argc] = strtok_r(NULL, " ", &saveptr);
        if (argv[argc] == NULL)
            break;
    }

    if (!argv[0])
        return ret;

    /* Find the command in our cmd array */

    for (x = 0; g_videotest_cmds[x].cmd; x++)
    {
        if (!strcmp(argv[0], "help"))
        {
            videotool_cmd_help(g_videotest_cmds);
            break;
        }

        if (!strcmp(argv[0], g_videotest_cmds[x].cmd))
        {
            ret = g_videotest_cmds[x].pfunc(argc, argv);
            if (ret < 0)
            {
                printf("cmd %s error %d\n", argv[0], ret);
                ret = 0;
            }

            if (g_videotest_cmds[x].pfunc == videotest_quit)
                ret = -1;

            break;
        }
    }

    if (g_videotest_cmds[x].cmd == NULL)
    {
        printf("Unknown cmd: %s\n", argv[0]);
        videotool_cmd_help(g_videotest_cmds);
    }

    return ret;
}

int main(int argc, char *argv[])
{
    int ret, len;
    char *buffer;
    buffer = malloc(CONFIG_NSH_LINELEN);
    chain = calloc(1, sizeof(video_pri));
    // video_start_thread();
    if (!buffer)
        return -ENOMEM;

    while (1)
    {
        printf("video> ");
        fflush(stdout);

        len = readline_stream(buffer, CONFIG_NSH_LINELEN, stdin, stdout);
        if (len <= 0)
            continue;
        buffer[len] = '\0';

        if (buffer[0] == '!')
        {
#ifdef CONFIG_SYSTEM_SYSTEM
            system(buffer + 1);
#endif
            continue;
        }

        if (buffer[len - 1] == '\n')
            buffer[len - 1] = '\0';

        if (buffer[0] == '\0')
            continue;

        ret = videotool_execute(buffer);

        if (ret < 0)
        {
            printf("Bye-Bye!\n");
            break;
        }
    }

    free(buffer);
    free(chain);
    return 0;
}