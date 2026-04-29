/**************************************************************
 *
 * Copyright(c) 2022, Beijing Xiaomi Mobile Software Co., Ltd.
 * All Rights Reserved.
 *
 *************************************************************/

#ifndef _MEDIATEST_SESSION_PLAYER_H_
#define _MEDIATEST_SESSION_PLAYER_H_

#include <stdbool.h>
#include <stdint.h>

#define AUDIO_APP_PLAYER_PAUSE_NORMAL   0  // Normal Pause, such as playback completion or actively initiated Pause
#define AUDIO_APP_PLAYER_PAUSE_PASSIVE  1  // Passive Pause, such as focus interruption

typedef enum {
    PLAYER_CONTROL_COMMAND_PLAY = 0,
    PLAYER_CONTROL_COMMAND_PAUSE,
    PLAYER_CONTROL_COMMAND_STOP,
    PLAYER_CONTROL_COMMAND_PREV,
    PLAYER_CONTROL_COMMAND_NEXT,
    PLAYER_CONTROL_COMMAND_VOLUME,
    PLAYER_CONTROL_COMMAND_SYNC
} player_command_t;

typedef enum{
    PLAYER_SOURCE_NONE         = 0,
    PLAYER_SOURCE_PHONE        = 1,  // device for phone
    PLAYER_SOURCE_WATCH        = 2,  // device for watch
    PLAYER_SOURCE_SHARE_DEVICE = 4,  // device for share
    PLAYER_SOURCE_ALL          = 7   // 4 | 2 | 1
} player_source_t;

/* refer to _PlayerInfo_State */
typedef enum {
    PLAYER_STATE_NONE = 0,
    PLAYER_STATE_PLAYING,
    PLAYER_STATE_PAUSE,
    PLAYER_STATE_STOP,
    PLAYER_STATE_NO_PERMISSION = 10
} player_state_t;

typedef struct {
    player_source_t source;
    player_state_t  state;
    int8_t          volume;
    int32_t         position;
    int32_t         duration;
    char            player_name[64];
    char            title[64];
    char            artist[64];
} player_info_t;

typedef struct {
    bool (*support_share)(void);
    char* (*share_device_current_id_get)(void);
    bool (*share_device_need_send_request_get)(void);
} test_multi_session_share_t;

typedef void (*player_notify_cb_t)(player_info_t* info);

typedef void (*app_cb_t)(const char* name, int state, void* data, int reason);

player_source_t test_multi_session_get_player_source(void);

void test_multi_session_set_player_source(player_source_t source, bool valid);

void test_multi_session_player_data_notify(player_info_t* info);

void test_multi_session_player_notify_cb_register(player_notify_cb_t cb);

void test_multi_session_player_notify_cb_deregister(void);

void test_multi_session_player_control(player_source_t source,
                                         player_command_t command,
                                         uint8_t volume);

void test_multi_session_player_init(void);

void test_multi_session_share_register(test_multi_session_share_t* share);

void multi_session_audio_app_state_monit_reg(app_cb_t cb, void* data);

void multi_session_audio_app_state_monit_unreg(void);

int multi_session_audio_app_state_monit_get(char* name);

#endif /* _MEDIATEST_SESSION_PLAYER_H_ */
