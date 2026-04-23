/**************************************************************
 *
 * Copyright(c) 2022, Beijing Xiaomi Mobile Software Co., Ltd.
 * All Rights Reserved.
 *
 *************************************************************/

#ifndef _MIWEAR_MULTI_SESSION_H_
#define _MIWEAR_MULTI_SESSION_H_

#include <stdbool.h>
#include <stdint.h>

#include <nuttx/config.h>

#ifdef CONFIG_LIBUV_EXTENSION
#include <uv.h>
#include <uv_ext.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_MEDIA
#include <media_api.h>
#else
#include <media_defs.h>
#include <media_focus.h>
#endif

/* audio mode */
#define MULTI_SESSION_AUDIO_MODE_NORMAL 0
#define MULTI_SESSION_AUDIO_MODE_PHONE 1

#define MULTI_SESSION_BTSCO_UNUSE 0
#define MULTI_SESSION_BTSCO_INUSE 1

/* mute mode */
#define MULTI_SESSION_MUTE_MODE_ON 1
#define MULTI_SESSION_MUTE_MODE_OFF 0

/* mic mode */
#define MULTI_SESSION_MIC_MODE_ON 1
#define MULTI_SESSION_MIC_MODE_OFF 0

#define MULTI_SESSION_CONTROL_CMD_PLAY 0x00
#define MULTI_SESSION_CONTROL_CMD_OPEN 0x01
#define MULTI_SESSION_CONTROL_CMD_PREPARE 0x02
#define MULTI_SESSION_CONTROL_CMD_START 0x03
#define MULTI_SESSION_CONTROL_CMD_PAUSE 0x04
#define MULTI_SESSION_CONTROL_CMD_STOP 0x05
#define MULTI_SESSION_CONTROL_CMD_CLOSE 0x06
#define MULTI_SESSION_CONTROL_CMD_SET_LOOP 0x07
#define MULTI_SESSION_CONTROL_CMD_SET_SEEK 0x08
#define MULTI_SESSION_CONTROL_CMD_GET_POSITION 0x09
#define MULTI_SESSION_CONTROL_CMD_GET_DURATION 0x0A
#define MULTI_SESSION_CONTROL_CMD_GET_PLAY_STATE 0x0B
#define MULTI_SESSION_CONTROL_CMD_GET_ALL 0x0C
#define MULTI_SESSION_CONTROL_CMD_SET_AUDIO_MODE 0x0D
#define MULTI_SESSION_CONTROL_CMD_SET_MUTE_MODE 0x0E
#define MULTI_SESSION_CONTROL_CMD_GET_MUTE_MODE 0x0F
#define MULTI_SESSION_CONTROL_CMD_SET_STREAM_VOLUME 0x10
#define MULTI_SESSION_CONTROL_CMD_GET_STREAM_VOLUME 0x11
#define MULTI_SESSION_CONTROL_CMD_INC_STREAM_VOLUME 0x12
#define MULTI_SESSION_CONTROL_CMD_DEC_STREAM_VOLUME 0x13
#define MULTI_SESSION_CONTROL_CMD_PLAY_NEXT 0x14
#define MULTI_SESSION_CONTROL_CMD_UPDATE_USER_DATA 0x15
#define MULTI_SESSION_CONTROL_CMD_FOCUS_REQUEST 0x16
#define MULTI_SESSION_CONTROL_CMD_FOCUS_ABANDON 0x17
#define MULTI_SESSION_CONTROL_CMD_LIST_PLAY_NEXT 0x18
#define MULTI_SESSION_CONTROL_CMD_SET_SCO_USE 0x19
#define MULTI_SESSION_CONTROL_CMD_SET_MIC_MODE 0x1A
#define MULTI_SESSION_CONTROL_CMD_WRITE_START 0x1B

#define MULTI_SESSION_CONTROL_CMD_RESET 0x90
#define MULTI_SESSION_CONTROL_CMD_SENDMSG 0x91
#define MULTI_SESSION_CONTROL_CMD_SET_GRAPH_VOLUME 0x92
#define MULTI_SESSION_CONTROL_CMD_GET_GRAPH_VOLUME 0x93

#define MULTI_SESSION_CONTROL_CMD_RECORDER_OPEN 0x20
#define MULTI_SESSION_CONTROL_CMD_RECORDER_PREPARE 0x21
#define MULTI_SESSION_CONTROL_CMD_RECORDER_START 0x22
#define MULTI_SESSION_CONTROL_CMD_RECORDER_STOP 0x23
#define MULTI_SESSION_CONTROL_CMD_RECORDER_CLOSE 0x24
#define MULTI_SESSION_CONTROL_CMD_RECORDER_READ 0x25
#define MULTI_SESSION_CONTROL_CMD_RECORDER_BEGIN 0x26
#define MULTI_SESSION_CONTROL_CMD_RECORDER_PAUSE 0x27
#define MULTI_SESSION_CONTROL_CMD_RECORDER_GET_PROP 0x28
#define MULTI_SESSION_CONTROL_CMD_RECORDER_RESET 0x23

/* event command */
#define MULTI_SESSION_EVENT_CMD_ERROR 0x30
#define MULTI_SESSION_EVENT_CMD_PLAY 0x31
#define MULTI_SESSION_EVENT_CMD_OPEN 0x32
#define MULTI_SESSION_EVENT_CMD_PREPARE 0x33
#define MULTI_SESSION_EVENT_CMD_START 0x34
#define MULTI_SESSION_EVENT_CMD_PAUSE 0x35
#define MULTI_SESSION_EVENT_CMD_STOP 0x36
#define MULTI_SESSION_EVENT_CMD_SEEK 0x37
#define MULTI_SESSION_EVENT_CMD_COMPLETE 0x38
#define MULTI_SESSION_EVENT_CMD_GET_POSITION 0x39
#define MULTI_SESSION_EVENT_CMD_GET_DURATION 0x3A
#define MULTI_SESSION_EVENT_CMD_GET_PLAY_STATE 0x3B
#define MULTI_SESSION_EVENT_CMD_GET_ALL 0x3C
#define MULTI_SESSION_EVENT_CMD_RECORDER_OPEN 0x3D
#define MULTI_SESSION_EVENT_CMD_RECORDER_BEGIN 0x3E
#define MULTI_SESSION_EVENT_CMD_RECORDER_READ 0x3F
#define MULTI_SESSION_EVENT_CMD_GET_MUTE_MODE 0x40
#define MULTI_SESSION_EVENT_CMD_GET_AUDIO_MODE 0x41
#define MULTI_SESSION_EVENT_CMD_GET_STREAM_VOLUME 0x42
#define MULTI_SESSION_EVENT_CMD_RECORDER_GET_PROP 0x43
#define MULTI_SESSION_EVENT_CMD_FOCUS_REQUEST 0x44
#define MULTI_SESSION_EVENT_CMD_FOCUS_CHANGE 0x45
#define MULTI_SESSION_EVENT_CMD_PLAYER_WRITE 0x46

/* media loop mode */
#define MULTI_SESSION_CONTROL_LOOP_ENABLE 1
#define MULTI_SESSION_CONTROL_LOOP_DISABLE 0

struct multi_session_event;

typedef void (*multi_session_notify_t)(
    struct multi_session_event *event);

typedef void (*multi_session_avrcp_cb_t)(uint32_t event);

typedef struct
{
  uint8_t *data;
  uint32_t len;
} multi_session_write_buffer_t;

typedef struct
{
  float vol;
} multi_session_graph_volume_t;

typedef struct
{
  char *filter;
  char *command;
  
} multi_session_send_command_t;

typedef struct
{
  char *url;
  char *options;
} multi_session_play_list_t;

typedef struct
{
  char *stream;
  char *focus;
  int loop;
  bool lock_focus;
  char url[256];
  char options[128];
  multi_session_notify_t cb;
} multi_session_play_t;

typedef struct
{
  char *stream;
  char *focus;
  bool lock_focus;
  multi_session_notify_t cb;
} multi_session_open_t;

typedef struct
{
  char url[256];
  char options[128];
} multi_session_prepare_t;

typedef struct
{
  int loop;
} multi_session_loop_t;

typedef struct
{
  uint32_t msec;
} multi_session_seek_t;

typedef struct
{
  uint32_t msec;
} multi_session_position_t;

typedef struct
{
  uint32_t msec;
} multi_session_duration_t;

typedef struct
{
  int playing; // 1:playing, 0:idle
} multi_session_play_state_t;

typedef struct
{
  unsigned int msec;
  unsigned int pos;
  int playing; // 1:playing, 0:idle
  int volume;
} multi_session_play_all_t;

typedef struct
{
  char url[64];
  char options[128];
} multi_session_recorder_prepare_t;

typedef struct
{
  void *data;
  size_t len;
} multi_session_recorder_read_t;

typedef struct
{
  char *stream;
  char *focus;
  bool lock_focus;
  char url[64];
  char options[128];
  multi_session_notify_t cb;
} multi_session_recorder_begin_t;

typedef struct
{
  uint8_t audio_mode;
  multi_session_notify_t cb;
} multi_session_audio_mode_t;

typedef struct
{
  int mute;
  multi_session_notify_t cb;
} multi_session_mute_mode_t;

typedef struct
{
  int mode;
} multi_session_mic_mode_t;

typedef struct
{
  char *stream;
  int volume;
  multi_session_notify_t cb;
} multi_session_stream_volume_t;

typedef struct
{
  char url[256];
  char options[128];
} multi_session_play_next_t;

typedef struct
{
  uint8_t use;
} multi_session_sco_t;

typedef struct
{
  char target[64];
  char key[64];
  void *data;
  int len;
} multi_session_recorder_get_prop_t;

typedef struct
{
  void *data;
  int len;
  char key[64];
} multi_session_recorder_prop_t;

typedef struct
{
  char *stream;
  int suggest_action; // refer to MULTI_SESSION_FOCUS_SUGGEST_ACTION_xxx
                      // or MEDIA_FOCUS_xxx
  multi_session_notify_t cb;
} multi_session_focus_t;

typedef struct
{
  void *user_data;
  void *handle;
  uint8_t command;
  union
  {
    multi_session_play_t play;
    multi_session_open_t open;
    multi_session_prepare_t prepare;
    multi_session_loop_t set_loop;
    multi_session_seek_t set_seek;
    multi_session_open_t recorder_open;
    multi_session_recorder_prepare_t recorder_prepare;
    multi_session_recorder_read_t recorder_read;
    multi_session_recorder_begin_t recorder_begin;
    multi_session_recorder_get_prop_t recorder_get_prop;
    multi_session_mute_mode_t set_mute_mode;
    multi_session_mute_mode_t get_mute_mode;
    multi_session_audio_mode_t set_audio_mode;
    multi_session_audio_mode_t get_audio_mode;
    multi_session_stream_volume_t set_stream_volume;
    multi_session_stream_volume_t get_stream_volume;
    multi_session_sco_t sco;
    multi_session_mic_mode_t mic_mode;
    multi_session_play_next_t play_next;
    multi_session_play_list_t play_list;
    multi_session_focus_t focus;
    multi_session_write_buffer_t write_buffer;
    multi_session_graph_volume_t graph_volume;
    multi_session_send_command_t send_command;
  };
} multi_session_control_t;

struct multi_session_event
{
  void *user_data;
  void *handle;
  uint8_t command;
  int status; /* -1 is error, 0 is ok */
  union
  {
    multi_session_play_t play;
    multi_session_open_t open;
    multi_session_position_t get_position;
    multi_session_duration_t get_duration;
    multi_session_play_state_t get_play_state;
    multi_session_play_all_t get_all;
    multi_session_recorder_read_t recorder_read;
    multi_session_mute_mode_t get_mute_mode;
    multi_session_audio_mode_t get_audio_mode;
    multi_session_stream_volume_t get_stream_volume;
    multi_session_recorder_begin_t recorder_begin;
    multi_session_open_t recorder_open;
    multi_session_recorder_prop_t recorder_prop;
    multi_session_focus_t focus;
  };
};

typedef struct multi_session_event multi_session_event_t;

void multi_session_set_audio_mode(uint8_t mode);

void multi_session_set_global_volume(char *stream, uint8_t volume);

void multi_session_get_global_volume(char *stream,
                                     multi_session_notify_t cb);

void multi_session_inc_global_volume(char *stream);

void multi_session_dec_global_volume(char *stream);

void multi_session_set_mute_mode(uint8_t mode);

void multi_session_get_mute_mode(multi_session_notify_t cb);

void multi_session_set_btsco_use(uint8_t use);

void multi_session_set_mic_mode(uint8_t mode);

int test_session_avrcp_event_register(multi_session_avrcp_cb_t event_cb);

void test_session_avrcp_event_update(int event);

void test_session_avrcp_event_unregister(int index);

void *multi_session_get_stream_handle(const char *stream);

void multi_session_control(multi_session_control_t *control);

uv_loop_t *multi_session_loop(void);

void multi_session_init(uv_loop_t *loop);

#ifdef __cplusplus
}
#endif

#endif /* _MIWEAR_MULTI_SESSION_H_ */
