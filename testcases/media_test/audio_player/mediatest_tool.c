#include "mediatest_tool.h"
#include "audio_focus.h"
#include "audio_manager_test.h"
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <malloc.h>
#include <media_api.h>
#include <nuttx/config.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <system/readline.h>
#include <unistd.h>

#ifdef CONFIG_TELEPHONY
#include "audio_tel_test.h"
#endif

static char args[5][20] = {0};

// static int g_manager_init = 0;

typedef int (*mediatest_func)(void);

struct mediatest_cmd_s
{
  const char *cmd;      /* The command text */
  mediatest_func pfunc; /* Pointer to command handler */
  const char *help;     /* The help text */
};

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

static struct mediatest_cmd_s g_mediatest_cmds[] = {
    {"play", mediatest_play, "play music (play stream_type)"},
    {"stop", mediatest_stop, "stop music (stop stream_type)"},
    {"q", mediatest_quit, "quit mediatest"},
    {"volume", mediatest_setvolume, "set volume"},
    {"p", mediatest_print_focus, "display focus status"},
    {"help", mediatest_help, "display help"},
    {"isplay", mediatest_isplay, "isplay stream_type"},
    {"next", mediatest_next_song, "play next song"},
    {"prev", mediatest_prev_song, "play prev song"},
    {"close", mediatest_close, "close player"},
    {"dump", mediatest_dump, "dump info"},
    {"setint", mediatest_setint, "setint Volume level"},
    {"pause", mediatest_pause, "pause streams"},
#ifdef CONFIG_TELEPHONY
    {"listen", mediatest_listen, "listen"},
    {"dial", mediatest_dial, "dial a phone"},
    {"answer", mediatest_answer, "answer a phone"},
    {"hangup", mediatest_hangup, "hangup current phone"},
#endif
    {0},
};

static int mediatest_play(void)
{
  char *stream_type = "Music";
  if (args[0][0] != '\0')
    {
      stream_type = args[0];
    }
  if (!strcmp("Music", stream_type))
    {
      test_play_cur_song();
    }
  else
    {
      char *url = NULL;
      if (!strcmp("Record", stream_type))
        {
          url = "/data/1.pcm";
        }
      else
        {
          url = "/data/1.mp3";
        }
      syslog(LOG_INFO, "args[%s]\n", args[1]);
      if (args[1][0] != '\0')
        {
          url = args[1];
        }
      test_player_play(get_type_of_player(stream_type), url);
    }
  return 0;
}

static int mediatest_dump(void)
{
  test_play_dump();
  return 0;
}

static int mediatest_pause(void)
{
  char *stream_type = "Music";
  if (args[0][0] != '\0')
    {
      stream_type = args[0];
    }
  test_player_pause(get_type_of_player(stream_type));
  return 0;
}

#ifdef CONFIG_TELEPHONY

static int mediatest_listen(void)
{
  init_tapi_and_modem();
  test_dial_listen();
  return 0;
}

static int mediatest_dial(void)
{
  char *number = args[0];
  player_new_call(number);
  return 0;
}

static int mediatest_answer(void)
{
  palyer_answer_call();
  return 0;
}
static int mediatest_hangup(void)
{
  player_hangup_phone();
  return 0;
}

#endif
static int mediatest_setint(void)
{
  char *stream_type = "Music";
  int volume = 10;
  if (args[0][0] != '\0')
    {
      stream_type = args[0];
    }
  if (args[1][0] != '\0')
    {
      volume = atoi(args[1]);
    }
  test_play_setint(stream_type, volume);
  return 0;
}

static int mediatest_close(void)
{
  char *stream_type = "Music";
  if (args[0][0] != '\0')
    {
      stream_type = args[0];
    }
  test_play_close(get_type_of_player(stream_type));
  return 0;
}

static int mediatest_next_song(void)
{
  int ret = test_play_next_song();
  return ret;
}

static int mediatest_prev_song(void)
{
  int ret = test_play_prev_song();
  return ret;
}

static int mediatest_isplay(void)
{
  char *stream_type = "Music";
  int ret = -1;
  if (args[0][0] != '\0')
    {
      stream_type = args[0];
    }
  if (!strcmp("Music", stream_type))
    {
      ret = test_audio_player_is_playing();
    }
  else
    {
      ret = test_play_common_is_playing(get_type_of_player(stream_type));
    }
  syslog(LOG_INFO, "playing type is %s, isplaying:%d\n", stream_type,
         ret);
  return 0;
}

static int mediatest_print_focus(void)
{
  test_audio_focus_debug_stack_display();
  return 0;
}

static int mediatest_help(void)
{
  for (int i = 0; g_mediatest_cmds[i].cmd; i++)
    {
      syslog(LOG_INFO, "%-16s %s\n", g_mediatest_cmds[i].cmd,
             g_mediatest_cmds[i].help);
    }
  return 0;
}

static int mediatest_stop(void)
{
  char *stream_type = "Music";
  if (args[0][0] != '\0')
    {
      stream_type = args[0];
    }
  if (!strcmp("Music", stream_type))
    {
      test_play_stop_song();
    }
  else
    {
      test_play_common_stop(get_type_of_player(stream_type));
    }
  return 0;
}

static int mediatest_setvolume(void)
{
  char *stream_type = args[0];
  float volume = atof(args[1]);
  return test_player_set_volume(get_type_of_player(stream_type), volume);
}

static int mediatest_quit(void)
{
  test_delete_all_play();
  sleep(1);
  return 1;
}

static int deal_data(char *arg)
{
  memset(args, 0, 5 * 20);
  int i = 0;
  char *token = strtok(arg, " ");
  while (token != NULL)
    {
      strncpy(args[i++], token, strlen(token)+1);
      syslog(LOG_INFO, "token is %s\n", token);
      token = strtok(NULL, " ");
    }

  return 0;
}

static int mediatest_execute(char *cmd, char *arg)
{
  int ret = 0;
  int x;

  /* Find the command in our cmd array */
  ret = deal_data(arg);

  for (x = 0; g_mediatest_cmds[x].cmd; x++)
    {
      if (strcmp(cmd, g_mediatest_cmds[x].cmd) == 0)
        {
          ret = g_mediatest_cmds[x].pfunc();
          if (ret < 0)
            syslog(LOG_ERR, "cmd %s error %d\n", cmd, ret);

          if (g_mediatest_cmds[x].pfunc == mediatest_quit)
            ret = 1;

          break;
        }
    }

  if (x == sizeof(g_mediatest_cmds) / sizeof(g_mediatest_cmds[0]))
    syslog(LOG_ERR, "Unknown cmd: %s\n", cmd);

  return ret;
}

int main(int argc, char *argv[])
{
  char *cmd, *arg;
  char *buffer;
  int ret, len;

  buffer = malloc(CONFIG_NSH_LINELEN);
  if (!buffer)
    return -1;

  test_audio_manager_init();

  while (1)
    {
      printf("mediatest> ");
      fflush(stdout);

      len = readline_stream(buffer, CONFIG_NSH_LINELEN, stdin, stdout);

      buffer[len] = '\0';
      if (len < 0)
        continue;

      if (buffer[0] == '!')
        {
#ifdef CONFIG_SYSTEM_SYSTEM
          system(buffer + 1);
#endif
          continue;
        }

      if (buffer[len - 1] == '\n')
        buffer[len - 1] = '\0';

      cmd = strtok_r(buffer, " \n", &arg);
      if (cmd == NULL)
        continue;

      while (*arg == ' ')
        arg++;

      syslog(LOG_INFO, "the cmd is %s\n", cmd);
      ret = mediatest_execute(cmd, arg);
      if (ret > 0)
        break;
    }

  free(buffer);
  return 0;
}
