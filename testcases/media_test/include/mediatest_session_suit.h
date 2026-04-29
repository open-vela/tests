#ifndef _MEDIATEST_SESSION_SUIT_H_
#define _MEDIATEST_SESSION_SUIT_H_

#include "audio_list.h"
#include "mediatest_session.h"
#include <media_api.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#ifdef CONFIG_LIBUV_EXTENSION
#include <uv.h>
#include <uv_ext.h>
#endif

// audio audio Info support maximum Length
#define AUDIO_PLAY_SRC_MAXLEN (250)

#define AUDIO_MQ_PATH "/tmp/audiotest_mq"

// timeupdate TimeoutTime
#define AUDIO_TIMEUPDATE_TIMEOUT (250) // 250ms
// MessageQueueLength
#define AUDIO_PLAYER_MQ_MSG_LEN 10

// Play Status
enum
{
  AUDIO_PLAYSTATE_STOP,
  AUDIO_PLAYSTATE_PLAY,
  AUDIO_PLAYSTATE_PAUSE,
  AUDIO_PLAYSTATE_COMPLETE
};

#define MODE_BUFFER  10
#define MODE_URL  11

typedef struct
{
  void *handle;
  bool used;
  bool muted;
  int loop;
  unsigned int percent; // Play Progress Percentage 0-100%
  unsigned int currenttime;
  unsigned duration;
  int playstate;
  int volume;
  float g_vol;
  int mode;
  void *data;
  int fd;
  int stop_flag;
  int mutedvolume; /* Store volume before mute. */
  char stream[15];
  char focus[15];
  char options[128];
  char src[AUDIO_PLAY_SRC_MAXLEN];
  test_song_entry_t *test_song_entry;
} audio_info_t;

typedef enum
{
  AUDIO_CTRL_PLAY = 0,
  AUDIO_CTRL_SEEK_CURRENTTIME,
  AUDIO_CTRL_LOOP,
  AUDIO_CTRL_VOLUME,
  AUDIO_CTRL_SEEK_PERCENT,
  AUDIO_CTRL_PLAYPREV,
  AUDIO_CTRL_PLAYNEXT,
  AUDIO_CTRL_PAUSE,
  AUDIO_CTRL_STOP,
  AUDIO_CTRL_RESUME,
  AUDIO_CTRL_VOLUMEUP,
  AUDIO_CTRL_VOLUMEDOWN,
  AUDIO_CTRL_GET_PLAY_STATE,
  AUDIO_CTRL_GET_POSITION,
  AUDIO_CTRL_GET_DURATION,
  AUDIO_CTRL_GET_ALL_STATE,
  AUDIO_CTRL_GET_VOLUME,
  AUDIO_CTRL_CLOSE,
  AUDIO_CTRL_GET_ALL_OPENED,
  AUDIO_CTRL_EXIT,
  AUDIO_CTRL_OPEN,
  AUDIO_CTRL_PREPARE,
  AUDIO_CTRL_START,
  AUDIO_CTRL_RESET,
  AUDIO_CTRL_SENDMSG,
  AUDIO_CTRL_GETGRAPHVOLUME,
  AUDIO_CTRL_SETGRAPHVOLUME
} audio_ctrl_t;

typedef enum
{
  PLAYER_RING = 0,
  PLAYER_ALARM,
  PLAYER_ENFORCED,
  PLAYER_NOTIFY,
  PLAYER_TTS,
  PLAYER_HEALTH,
  PLAYER_SPORT,
  PLAYER_INFO,
  PLAYER_MUSIC,
  PLAYER_INTERCOM
} audio_type_t;

typedef struct audio_attr
{
  int loop;
  char *stream;
  char *focus;
  char *url;
  char *options;
  uint32_t msec;
  uint8_t volume;
  char *file;
  int type;
  int mode;
  float g_vol;
} audio_attr_t;

typedef struct audio_msg
{
  audio_ctrl_t cmd;
  audio_attr_t attr;
} audio_msg_t;

int send_msg_audio_manager(audio_msg_t *msg);

#endif