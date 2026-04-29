#include "getopt.h"
#include "mediatest_session.h"
#include "mediatest_session_suit.h"
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <system/readline.h>
#include <unistd.h>

typedef int (*mediatest_func)(int argc, char **argv);

typedef struct mediatest_cmd_s
{
  const char *cmd;      /* The command text */
  mediatest_func pfunc; /* Pointer to command handler */
  const char *help;     /* The help text */
} mediatest_cmd;
static audio_info_t audio_ctx;
static inline void audio_parse_cmd(int size, char *info[],
                                   audio_attr_t *pri);
static int audiotool_execute(char *buffer);

static int mediatest_play(int argc, char **argv);
static int mediatest_pause(int argc, char **argv);
static int mediatest_next_song(int argc, char **argv);
static int mediatest_prev_song(int argc, char **argv);
static int mediatest_isplay(int argc, char **argv);
static int mediatest_help(mediatest_cmd cmds[]);
static int mediatest_setvolume(int argc, char **argv);
static int mediatest_quit(int argc, char **argv);
static int mediatest_dump(int argc, char **argv);
static int mediatest_close(int argc, char **argv);
static int mediatest_resume(int argc, char **argv);
static int mediatest_closeall(int argc, char **argv);
static int mediatest_show(int argc, char **argv);
static int mediatest_state(int argc, char **argv);
static int mediatest_seek(int argc, char **argv);
static int mediatest_setloop(int argc, char **argv);
static int mediatest_position(int argc, char **argv);
static int mediatest_duration(int argc, char **argv);
static int mediatest_getvolume(int argc, char **argv);
static int mediatest_open(int argc, char **argv);
static int mediatest_prepare(int argc, char **argv);
static int mediatest_start(int argc, char **argv);
static int mediatest_stop(int argc, char **argv);
static int mediatest_reset(int argc, char **argv);
static int mediatest_volumeup(int argc, char **argv);
static int mediatest_volumedown(int argc, char **argv);
static int mediatest_send(int argc, char **argv);
static int mediatest_set_graphvolume(int argc, char **argv);
static int mediatest_get_graphvolume(int argc, char **argv);

static struct mediatest_cmd_s g_mediatest_cmds[] = {
    {"play", mediatest_play, "play music (play stream_type)"},
    {"q", mediatest_quit, "quit mediatest"},
    {"volume", mediatest_setvolume, "set volume"},
    {"isplay", mediatest_isplay, "isplay stream_type"},
    {"next", mediatest_next_song, "play next song"},
    {"prev", mediatest_prev_song, "play prev song"},
    {"close", mediatest_close, "close player"},
    {"dump", mediatest_dump, "dump info"},
    {"pause", mediatest_pause, "pause streams"},
    {"resume", mediatest_resume, "resume streams"},
    {"closeall", mediatest_closeall, "closeall streams"},
    {"show", mediatest_show, "show opened streams"},
    {"state", mediatest_state, "show stream state"},
    {"seek", mediatest_seek, "seek stream position"},
    {"position", mediatest_position, "get current position"},
    {"duration", mediatest_duration, "get current duration"},
    {"getvolume", mediatest_getvolume, "get stream volume"},
    {"setloop", mediatest_setloop, "set loop count"},
    {"open", mediatest_open, "open stream"},
    {"prepare", mediatest_prepare, "prepare stream"},
    {"start", mediatest_start, "start stream"},
    {"stop", mediatest_stop, "stop stream"},
    {"reset", mediatest_reset, "reset stream"},
    {"volumeup", mediatest_volumeup, "volume up"},
    {"volumedown", mediatest_volumedown, "volume down"},
    {"send", mediatest_send, "send command to filter"},
    {"setgraphvolume", mediatest_set_graphvolume, "set graph volume"},
    {"getgraphvolume", mediatest_get_graphvolume, "set graph volume"},
    {0}};

static int mediatest_play(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_PLAY;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_pause(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_PAUSE;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_resume(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_RESUME;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_close(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_CLOSE;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_dump(int argc, char **argv)
{
  media_policy_dump(NULL);
  media_graph_dump(NULL);
  media_focus_dump(NULL);
  return 0;
}

static int mediatest_state(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_GET_ALL_STATE;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_seek(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_SEEK_CURRENTTIME;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_position(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_GET_POSITION;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_duration(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_GET_DURATION;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_getvolume(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_GET_VOLUME;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_setloop(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_LOOP;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_next_song(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_PLAYNEXT;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_prev_song(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_PLAYPREV;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_isplay(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_GET_PLAY_STATE;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_show(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_GET_ALL_OPENED;
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_help(mediatest_cmd cmds[])
{
  int i = 0;
  for (i = 0; cmds[i].cmd; i++)
    syslog(LOG_INFO, "%-16s %s\n", cmds[i].cmd, cmds[i].help);
  return 0;
}

static int mediatest_setvolume(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_VOLUME;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_quit(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_EXIT;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_closeall(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_EXIT;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_open(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_OPEN;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}
static int mediatest_prepare(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_PREPARE;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}
static int mediatest_start(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_START;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}
static int mediatest_stop(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_STOP;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_reset(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_RESET;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_volumeup(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_VOLUMEUP;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_volumedown(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_VOLUMEDOWN;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_send(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_SENDMSG;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_set_graphvolume(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_SETGRAPHVOLUME;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static int mediatest_get_graphvolume(int argc, char **argv)
{
  audio_msg_t msg;
  memset(&msg, 0, sizeof(audio_msg_t));
  msg.cmd = AUDIO_CTRL_GETGRAPHVOLUME;
  audio_parse_cmd(argc, argv, &msg.attr);
  send_msg_audio_manager(&msg);
  return 0;
}

static inline void audio_parse_cmd(int size, char *info[],
                                   audio_attr_t *pri)
{
  memset(pri, 0, sizeof(audio_attr_t));
  pri->url = "/data/1.wav";
  pri->stream = "Music";
  pri->type = 0;
  pri->mode = 1;

  char ch;
  while ((ch = getopt(size, info, "s:u:o:f:t:v:m:p:c:r:e:")) != -1)
    {
      switch (ch)
        {
        case 's':
          if (optarg)
            {
              pri->stream = optarg;
            }
          break;
        case 'u':
          if (optarg)
            {
              pri->url = optarg;
            }
          break;
        case 'o':
          if (optarg)
            {
              pri->options = optarg;
            }
          break;
        case 'r':
          if (optarg)
            {
              pri->focus = optarg;
            }
          break;
        case 'f':
          if (optarg)
            {
              pri->file = optarg;
            }
          break;
        case 't':
          if (optarg)
            {
              pri->type = atoi(optarg);
            }
          break;
        case 'v':
          if (optarg)
            {
              pri->volume = atoi(optarg);
            }
          break;
        case 'm':
          if (optarg)
            {
              pri->mode = atoi(optarg);
            }
          break;
        case 'c':
          if (optarg)
            {
              pri->loop = atoi(optarg);
            }
          break;
        case 'p':
          if (optarg)
            {
              pri->msec = atoi(optarg);
            }
          break;
        case 'e':
          if (optarg)
            {
              pri->g_vol = atof(optarg);
            }
          break;
        default:
          syslog(LOG_INFO,
                 "\nUsage:  audioplayer %s [-h] -s <stream> -u <url> -o "
                 "<option> -f <file> -a -t <type> -v <volume>\n",
                 info[0]);
          syslog(LOG_INFO, "-h             help\n");
          syslog(LOG_INFO,
                 "-s <stream> the stream that you want to play");
          syslog(LOG_INFO, "-u <url>  the url that you want to play\n");
          syslog(LOG_INFO, "-f <file> the file that you want to play\n");
          syslog(LOG_INFO,
                 "-o <option>  the option for preparing player\n");
          syslog(LOG_INFO, "-a the stability that you want to play\n");
          syslog(LOG_INFO, "-t <type> the type that you want to play\n");
          syslog(LOG_INFO,
                 "-c <count> the type that you want to play count\n");
          syslog(LOG_INFO,
                 "-m <mode> the type that you want to play mode\n");
          syslog(LOG_INFO,
                 "-p <pos> the type that you want to play pos\n");
          syslog(LOG_INFO,
                 "-v <volume> the volume that you want to play\n");
          return;
        }
    }
}

static int audiotool_execute(char *buffer)
{
  char *argv[30] = {NULL};
  char *saveptr = NULL;
  int ret = 0;
  int argc;
  int x;

  argv[0] = strtok_r(buffer, " ", &saveptr);
  for (argc = 1; argc < 30; argc++)
    {
      argv[argc] = strtok_r(NULL, " ", &saveptr);
      if (argv[argc] == NULL)
        break;
    }

  if (!argv[0])
    return ret;

  /* Find the command in our cmd array */

  for (x = 0; g_mediatest_cmds[x].cmd; x++)
    {
      if (!strcmp(argv[0], "help"))
        {
          mediatest_help(g_mediatest_cmds);
          break;
        }

      if (!strcmp(argv[0], g_mediatest_cmds[x].cmd))
        {
          ret = g_mediatest_cmds[x].pfunc(argc, argv);
          if (ret < 0)
            {
              printf("cmd %s error %d\n", argv[0], ret);
              ret = 0;
            }

          if (g_mediatest_cmds[x].pfunc == mediatest_quit)
            ret = -1;

          break;
        }
    }

  if (g_mediatest_cmds[x].cmd == NULL)
    {
      printf("Unknown cmd: %s\n", argv[0]);
      mediatest_help(g_mediatest_cmds);
    }

  return ret;
}

int main(int argc, char *argv[])
{
  int ret, len;
  char *buffer;
  buffer = malloc(CONFIG_NSH_LINELEN);
  // video_start_thread();
  if (!buffer)
    return -ENOMEM;

  while (1)
    {
      printf("player> ");
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

      ret = audiotool_execute(buffer);

      if (ret < 0)
        {
          printf("Bye-Bye!\n");
          break;
        }
    }

  free(buffer);
  return 0;
}
