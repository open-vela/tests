
#ifndef __AUDIO_PLAYER_H
#define __AUDIO_PLAYER_H

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include "audio_focus.h"
#include <media_api.h>
#include <mqueue.h>
/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
/* Public */
#define MEDIA_PREPARED_TIMES (10)
#define PLAYER_MQ_PAHT "/tmp/mediatest_player_msg"
#define PLAYER_MQ_PRIO (209)
#define PLAYER_MQ_MSG_LEN (10)

#define MAX_ID_LENGTH (64)
#define MAX_URL_LENGTH (512)

#define MEDIA_EVENT_PREPARING 11
#define MEDIA_EVENT_STARTING 12
#define MEDIA_EVENT_PAUSING 13
#define MEDIA_EVENT_STOPPING 14

#define EVENT_REQUEST_DIAL_DONE 0x71
#define EVENT_REQUEST_START_DTMF_DONE 0x72
#define EVENT_REQUEST_STOP_DTMF_DONE 0x73

#define EVENT_OEM_RIL_REQUEST_RAW_DONE 0x07
#define EVENT_OEM_RIL_REQUEST_STRINGS_DONE 0x08

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * Public Type Definitions
 ****************************************************************************/
typedef enum
{
  PLAYER_PLAY = 0,
  PLAYER_PAUSE,
  PLAYER_RESUME,
  PLAYER_STOP,
  PLAYER_RESET,
  PLAYER_SET_POS,
  PLAYER_GET_POS,
  PLAYER_GET_DUR,
  PLAYER_SET_VOLUME,
  PLAYER_GET_VOLUME,
  PLAYER_ISPLAYING,
  PLAYER_START,
  PLAYER_CLOSE,
  PLAYER_EXIT
} test_player_cmd_t;

typedef enum
{
  PLAYER_MUSIC = 0,
  PLAYER_NOTIFY,
  PLAYER_TTS,
  PLAYER_ALARM,
  PLAYER_SCO,
  PLAYER_RING,
  PLAYER_ENFORCED,
  PLAYER_RECORD,
  PLAYER_HEALTH,
  PLAYER_SPORT,
  PLAYER_INFO
} test_player_type_t;

typedef struct test_player_info
{
  char url[MAX_URL_LENGTH];
  char song_id[MAX_ID_LENGTH];
  test_player_type_t type;
  int state;
  void *handle;
  int last_focus_type;
  int offset_in_ms;
  int position;
  int err_state;
  int query_pause;
  int duration;
  float volume;
  void **af_req_handle;
  int isplaying;
  int af_is_requesting;
} test_player_info_t;

typedef struct test_msg_player
{
  test_player_cmd_t cmd;
  test_player_type_t type;
  void *p1;
} test_msg_player_t;

typedef void (*player_event_callback_t)(test_player_info_t *info,
                                        int event, void *ext_data);

int test_register_player_callback(player_event_callback_t event_cb,
                                  void *ext_data);

int test_player_play(test_player_type_t type, const char *url);

int test_player_start(test_player_type_t type);

int test_player_pause(test_player_type_t type);

int test_player_resume(test_player_type_t type);

int test_player_stop(test_player_type_t type);

int test_player_reset(test_player_type_t type);

int test_player_close(test_player_type_t type);

int test_player_set_position(test_player_type_t type, int pos_in_ms);

int test_player_get_position(test_player_type_t type);

int test_player_get_duration(test_player_type_t type);

int test_player_get_volume(test_player_type_t type);

int test_player_set_volume(test_player_type_t type, float volume);

int test_player_get_isplaying(test_player_type_t type);

test_player_info_t *test_get_music_player_info(void);

test_player_info_t *test_get_notify_player_info(void);

test_player_info_t *test_get_tts_player_info(void);

test_player_info_t *test_get_alarm_player_info(void);

test_player_info_t *test_get_sco_player_info(void);

test_player_info_t *test_get_ring_player_info(void);

test_player_info_t *test_get_enforced_player_info(void);

test_player_info_t *test_get_record_player_info(void);

test_player_info_t *test_get_health_player_info(void);

test_player_info_t *test_get_sport_player_info(void);

test_player_info_t *test_get_info_player_info(void);

int test_clear_player_mq(void);

char *test_get_name_by_player_handle(void *handle);

int test_player_open(test_player_type_t player_type);
int test_set_info(test_player_info_t **test_player_info, test_player_type_t player_type);
void focus_abandon(test_player_info_t *info);
int create_mq(char *path, struct mq_attr *attr);
int player_prepare(test_player_info_t *info);

#ifdef __cplusplus
}
#endif

#endif /* __AUDIO_PLAYER_H */
