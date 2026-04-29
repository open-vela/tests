/****************************************************************************
 * Included Files
 ****************************************************************************/
#include "audio_player.h"
#include <errno.h>
#include <fcntl.h>
#include <media_api.h>
#include <mqueue.h>
#include <nuttx/config.h>
#include <nuttx/sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
/* audio player task */
#define AUDIO_PLAYER_TASK_PRIO (100)
#define AUDIO_PLAYER_TASK_STACK_SIZE (10 * 1024)

typedef struct focus_cb_argv
{
  test_player_type_t type;
  char *url;
} focus_cb_argv_t;

/****************************************************************************
 * Private Data
 ****************************************************************************/
static test_player_info_t g_musicplayer_info = {0};
static test_player_info_t g_notifyplayer_info = {0};
static test_player_info_t g_ttsplayer_info = {0};
static test_player_info_t g_alarmplayer_info = {0};
static test_player_info_t g_scoplayer_info = {0};
static test_player_info_t g_ringplayer_info = {0};
static test_player_info_t g_enforcedplayer_info = {0};
static test_player_info_t g_recordplayer_info = {0};
static test_player_info_t g_healthplayer_info = {0};
static test_player_info_t g_sportplayer_info = {0};
static test_player_info_t g_infoplayer_info = {0};

static void *g_musicplayer_handle;
static void *g_notifyplayer_handle;
static void *g_ttsplayer_handle;
static void *g_alarmplayer_handle;
static void *g_scoplayer_handle;
static void *g_ringplayer_handle;
static void *g_enforcedplayer_handle;
static void *g_recordplayer_handle;
static void *g_healthplayer_handle;
static void *g_sportplayer_handle;
static void *g_infoplayer_handle;

static player_event_callback_t player_event_callback = NULL;
static void *player_user_data;

void *test_music_req_handle_0 = NULL;
void *test_notify_req_handle_0 = NULL;
void *test_tts_req_handle_0 = NULL;
void *test_alarm_req_handle_0 = NULL;
void *test_sco_req_handle_0 = NULL;
void *test_ring_req_handle_0 = NULL;
void *test_enforced_req_handle_0 = NULL;
void *test_record_req_handle_0 = NULL;
void *test_health_req_handle_0 = NULL;
void *test_sport_req_handle_0 = NULL;
void *test_info_req_handle_0 = NULL;

static test_player_type_t test_playing_type = 0;

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

char *test_get_name_by_player_handle(void *handle)
{
  if (handle == NULL)
    return NULL;

  char *name = NULL;
  if (handle == g_musicplayer_handle)
    {
      name = "Music";
    }
  else if (handle == g_notifyplayer_handle)
    {
      name = "Notify";
    }
  else if (handle == g_ttsplayer_handle)
    {
      name = "TTS";
    }
  else if (handle == g_alarmplayer_handle)
    {
      name = "Alarm";
    }
  else if (handle == g_scoplayer_handle)
    {
      name = "SCO";
    }
  else if (handle == g_ringplayer_handle)
    {
      name = "Ring";
    }
  else if (handle == g_enforcedplayer_handle)
    {
      name = "Enforced";
    }
  else if (handle == g_recordplayer_handle)
    {
      name = "Record";
    }
  else if (handle == g_sportplayer_handle)
    {
      name = "Sport";
    }
  else if (handle == g_healthplayer_handle)
    {
      name = "Health";
    }
  else if (handle == g_infoplayer_handle)
    {
      name = "Info";
    }

  return name;
}

static void audioplayer_event_callback(void *cookie, int event, int ret,
                                       const char *data)
{
  char *str;
  test_player_info_t *test_player_info = (test_player_info_t *)cookie;
  void *handle = test_player_info->handle;
  test_player_info->state = event;
  test_player_info->err_state = ret;
  syslog(LOG_INFO, "enter %s\n", __func__);
  switch (event)
    {
    case MEDIA_EVENT_PREPARED:
      str = "MEDIA_EVENT_PREPARED";
      /* prepare fail, reset */
      if (test_player_info->err_state < 0)
        {
          if (test_player_info->type == PLAYER_RECORD)
            {
              media_recorder_reset(handle);
            }
          else
            {
              media_player_reset(handle);
            }
          test_player_info->state = MEDIA_EVENT_STOPPED;
        }
      else
        {
          // media_player_set_volume(handle, 0.2);
          if (test_player_info->type == PLAYER_RECORD)
            {
              test_player_start(test_player_info->type);
            }
          else
            {
              media_player_start(handle);
            }
        }
      break;
    case MEDIA_EVENT_STARTED:
      str = "MEDIA_EVENT_STARTED";
      /* start failed, reset. usually occurs when resume fails */
      if (test_player_info->err_state < 0)
        {
          if (test_player_info->type == PLAYER_RECORD)
            {
              media_recorder_reset(handle);
            }
          else
            {
              media_player_reset(handle);
            }
          test_player_info->state = MEDIA_EVENT_STOPPED;
        }
      else
        {
          if (test_player_info->type != PLAYER_RECORD)
            {
              if (test_player_info->offset_in_ms > 0)
                {
                  /* Only seek at first start, resume calling start will not seek
                   */
                  media_player_seek(handle,
                                    test_player_info->offset_in_ms);
                  test_player_info->offset_in_ms = 0;
                }
              media_player_get_duration(
                  test_player_info->handle,
                  (unsigned int *)&test_player_info->duration);
            }
        }
      break;
    case MEDIA_EVENT_PAUSED:
      str = "MEDIA_EVENT_PAUSED";
      break;
    case MEDIA_EVENT_COMPLETED:
      str = "MEDIA_EVENT_COMPLETED";
      media_player_get_position(
          test_player_info->handle,
          (unsigned int *)&test_player_info->position);
      break;
    case MEDIA_EVENT_STOPPED:
      str = "MEDIA_EVENT_STOPPED";
      test_player_info->query_pause = 0;
      break;
    case MEDIA_EVENT_SEEKED:
      str = "MEDIA_EVENT_SEEKED";
      break;
    default:
      str = "UNKNOW EVENT";
      break;
    }
  char *name = test_get_name_by_player_handle(test_player_info->handle);
  syslog(LOG_INFO,
         "%s, handle %p, name %s, music event %s, event %d, ret %d, "
         "info %s "
         "line %d\n",
         __func__, handle, name, str, event, ret,
         data ? data : "NULL", __LINE__);
  /* Call CallbackFunction to notify upper layer */
  if (player_event_callback)
    {
      syslog(LOG_INFO, "test_player_info->type = %d\n\n",
             test_player_info->type);
      player_event_callback(test_player_info, test_player_info->state,
                            player_user_data);
    }
}

/**
 * @brief
 *
 * @param play_ret
 * @param call_arg
 */
static void focus_change_listener(int play_ret, void *call_arg)
{
  int ret = 0;

  syslog(
      LOG_INFO,
      "change listener trigered: play ret is: %d, player_type is: %d\n",
      play_ret, ((test_player_info_t *)call_arg)->type);
  // do not run media focus API in change listener directly, or there
  // will be a deadlock!!!

  test_player_info_t *argv = (test_player_info_t *)call_arg;
  int last_focus_type = play_ret;
  syslog(LOG_INFO, "last focus type = %d\n", argv->last_focus_type);
  switch (play_ret)
    {
    case AUDIO_FOCUS_PLAY:
      if (argv->state != MEDIA_EVENT_PAUSED &&
          argv->state != MEDIA_EVENT_PAUSING)
        {
          if (argv->last_focus_type == AUDIO_FOCUS_PLAY_WITH_DUCK)
            {
              syslog(LOG_INFO, "DUCK play resume normal\n");
              ret = test_player_set_volume(argv->type, argv->volume);
              if (ret < 0)
                {
                  syslog(LOG_ERR,
                         "handle: %p set volume failed, ret = %d\n",
                         (argv->handle), ret);
                }
            }
          else
            {
              syslog(LOG_INFO,
                     "PLAYER_TYPE is not PAUSED or PAUSING, need not "
                     "resume "
                     "handle:%p repeatedly\n",
                     (argv->handle));
            }

          last_focus_type = AUDIO_FOCUS_PLAY;
        }
      else
        {
          /* Resume */
          syslog(LOG_DEBUG, "Resume handle:%p, url = %s\n",
                 (argv->handle), argv->url);
          ret = test_player_resume(argv->type);
          if (ret < 0)
            {
              syslog(LOG_ERR, "handle: %p start failed, ret = %d\n",
                     (argv->handle), ret);
            }
          last_focus_type = AUDIO_FOCUS_PLAY;
        }
      break;
    case AUDIO_FOCUS_STOP:
      media_player_reset(argv->handle);
      media_player_close(argv->handle, 0);
      memset(argv, 0, sizeof(test_player_info_t));
      break;
    case AUDIO_FOCUS_PAUSE:
      syslog(LOG_DEBUG, "Pause handle:%p, url = %s\n",
             (argv->handle), argv->url);
      ret = test_player_pause(argv->type);
      if (ret < 0)
        {
          syslog(LOG_ERR, "handle: %p pause failed, ret = %d\n",
                 (argv->handle), ret);
        }
      last_focus_type = AUDIO_FOCUS_PAUSE;
      break;
    case AUDIO_FOCUS_PLAY_BUT_SILENT:
      break;
    case AUDIO_FOCUS_PLAY_WITH_DUCK:
      syslog(LOG_DEBUG, "Play with duck handle:%p, url = %s\n",
             (argv->handle), argv->url);
      test_player_get_volume(argv->type);
      ret = test_player_set_volume(argv->type, 0.3);
      if (ret < 0)
        {
          syslog(LOG_ERR, "handle: %p set volume failed, ret = %d\n",
                 (argv->handle), ret);
        }
      last_focus_type = AUDIO_FOCUS_PLAY_WITH_DUCK;
      break;
    case AUDIO_FOCUS_PLAY_WITH_KEEP:
      break;
    }
  argv->last_focus_type = last_focus_type;
}

int create_mq(char *path, struct mq_attr *attr)
{
  int mqid = -1;
  mqid = mq_open(path, O_RDWR | O_CREAT, NULL, attr);

  if (mqid == -1)
    {
      syslog(LOG_INFO, "mq_open failed width error: %d\n", errno);
    }
  else
    {
      mq_close(mqid);
    }
  return mqid;
}

static int player_request_play(test_player_type_t type, char *url)
{
  int ret = -1;
  int play_ret = -1;

  switch (type)
    {
    case PLAYER_MUSIC:
      g_musicplayer_info.af_is_requesting = 1;
      test_music_req_handle_0 = test_audio_focus_request(
          &play_ret, AUDIO_STREAM_MUSIC, &focus_change_listener,
          (void *)&g_musicplayer_info);
      if (test_music_req_handle_0 != NULL)
        {
          syslog(LOG_INFO,
                 "request(PLAYER_MUSIC) success, focus_handle:%p, "
                 "play suggetion is %d\n",
                 test_music_req_handle_0, play_ret);
        }
      else
        {
          syslog(LOG_INFO, "request(PLAYER_MUSIC) failed");
        }
      g_musicplayer_info.af_is_requesting = 0;
      break;

    case PLAYER_NOTIFY:
      g_notifyplayer_info.af_is_requesting = 1;
      test_notify_req_handle_0 = test_audio_focus_request(
          &play_ret, AUDIO_STREAM_NOTIFY, &focus_change_listener,
          (void *)&g_notifyplayer_info);
      if (test_notify_req_handle_0 != NULL)
        {
          syslog(
              LOG_INFO,
              "request(PLAYER_NOTIFY) success, focus_handle:%p, play "
              "suggetion is %d\n",
              test_notify_req_handle_0, play_ret);
        }
      else
        {
          syslog(LOG_INFO, "request(PLAYER_NOTIFY) failed");
        }
      g_notifyplayer_info.af_is_requesting = 0;
      break;

    case PLAYER_TTS:
      g_ttsplayer_info.af_is_requesting = 1;
      test_tts_req_handle_0 = test_audio_focus_request(
          &play_ret, AUDIO_STREAM_TTS, &focus_change_listener,
          (void *)&g_ttsplayer_info);
      if (test_tts_req_handle_0 != NULL)
        {
          syslog(LOG_INFO,
                 "request(PLAYER_TTS) success, focus_handle:%p, play "
                 "suggetion is %d\n",
                 test_tts_req_handle_0, play_ret);
        }
      else
        {
          syslog(LOG_INFO, "request(PLAYER_TTS) failed");
        }
      g_ttsplayer_info.af_is_requesting = 0;
      break;

    case PLAYER_ALARM:
      g_alarmplayer_info.af_is_requesting = 1;
      test_alarm_req_handle_0 = test_audio_focus_request(
          &play_ret, AUDIO_STREAM_ALARM, &focus_change_listener,
          (void *)&g_alarmplayer_info);
      if (test_alarm_req_handle_0 != NULL)
        {
          syslog(LOG_INFO,
                 "request(PLAYER_ALARM) success, focus_handle:%p, "
                 "play suggetion is %d\n",
                 test_alarm_req_handle_0, play_ret);
        }
      else
        {
          syslog(LOG_INFO, "request(PLAYER_ALARM) failed");
        }
      g_alarmplayer_info.af_is_requesting = 0;
      break;

    case PLAYER_SCO:
      g_scoplayer_info.af_is_requesting = 1;
      test_sco_req_handle_0 = test_audio_focus_request(
          &play_ret, AUDIO_STREAM_SCO, &focus_change_listener,
          (void *)&g_alarmplayer_info);
      if (test_sco_req_handle_0 != NULL)
        {
          syslog(LOG_INFO,
                 "request(PLAYER_ALARM) success, focus_handle:%p, "
                 "play suggetion is %d\n",
                 test_sco_req_handle_0, play_ret);
        }
      else
        {
          syslog(LOG_INFO, "request(PLAYER_ALARM) failed");
        }
      g_scoplayer_info.af_is_requesting = 0;
      break;

    case PLAYER_RING:
      g_ringplayer_info.af_is_requesting = 1;
      test_ring_req_handle_0 = test_audio_focus_request(
          &play_ret, AUDIO_STREAM_RING, &focus_change_listener,
          (void *)&g_ringplayer_info);
      if (test_ring_req_handle_0 != NULL)
        {
          syslog(LOG_INFO,
                 "request(PLAYER_ring) success, focus_handle:%p, "
                 "play suggetion is %d\n",
                 test_ring_req_handle_0, play_ret);
        }
      else
        {
          syslog(LOG_INFO, "request(PLAYER_ring) failed");
        }
      g_ringplayer_info.af_is_requesting = 0;
      break;

    case PLAYER_ENFORCED:
      g_enforcedplayer_info.af_is_requesting = 1;
      test_enforced_req_handle_0 = test_audio_focus_request(
          &play_ret, AUDIO_STREAM_ENFORCED, &focus_change_listener,
          (void *)&g_enforcedplayer_info);
      if (test_enforced_req_handle_0 != NULL)
        {
          syslog(LOG_INFO,
                 "request(PLAYER_enforced) success, focus_handle:%p, "
                 "play suggetion is %d\n",
                 test_enforced_req_handle_0, play_ret);
        }
      else
        {
          syslog(LOG_INFO, "request(PLAYER_enforced) failed");
        }
      g_enforcedplayer_info.af_is_requesting = 0;
      break;

    case PLAYER_RECORD:
      g_recordplayer_info.af_is_requesting = 1;
      test_record_req_handle_0 = test_audio_focus_request(
          &play_ret, AUDIO_STREAM_RECORD, &focus_change_listener,
          (void *)&g_recordplayer_info);
      if (test_record_req_handle_0 != NULL)
        {
          syslog(LOG_INFO,
                 "request(PLAYER_record) success, focus_handle:%p, "
                 "play suggetion is %d\n",
                 test_record_req_handle_0, play_ret);
        }
      else
        {
          syslog(LOG_INFO, "request(PLAYER_record) failed");
        }
      g_recordplayer_info.af_is_requesting = 0;
      break;

    case PLAYER_HEALTH:
      g_healthplayer_info.af_is_requesting = 1;
      test_health_req_handle_0 = test_audio_focus_request(
          &play_ret, AUDIO_STREAM_HEALTH, &focus_change_listener,
          (void *)&g_healthplayer_info);
      if (test_health_req_handle_0 != NULL)
        {
          syslog(LOG_INFO,
                 "request(PLAYER_health) success, focus_handle:%p, "
                 "play suggetion is %d\n",
                 test_health_req_handle_0, play_ret);
        }
      else
        {
          syslog(LOG_INFO, "request(PLAYER_health) failed");
        }
      g_healthplayer_info.af_is_requesting = 0;
      break;

    case PLAYER_SPORT:
      g_sportplayer_info.af_is_requesting = 1;
      test_sport_req_handle_0 = test_audio_focus_request(
          &play_ret, AUDIO_STREAM_SPORT, &focus_change_listener,
          (void *)&g_sportplayer_info);
      if (test_sport_req_handle_0 != NULL)
        {
          syslog(LOG_INFO,
                 "request(PLAYER_sport) success, focus_handle:%p, "
                 "play suggetion is %d\n",
                 test_sport_req_handle_0, play_ret);
        }
      else
        {
          syslog(LOG_INFO, "request(PLAYER_sport) failed");
        }
      g_sportplayer_info.af_is_requesting = 0;
      break;

    case PLAYER_INFO:
      g_infoplayer_info.af_is_requesting = 1;
      test_info_req_handle_0 = test_audio_focus_request(
          &play_ret, AUDIO_STREAM_INFO, &focus_change_listener,
          (void *)&g_infoplayer_info);
      if (test_info_req_handle_0 != NULL)
        {
          syslog(LOG_INFO,
                 "request(PLAYER_info) success, focus_handle:%p, "
                 "play suggetion is %d\n",
                 test_info_req_handle_0, play_ret);
        }
      else
        {
          syslog(LOG_INFO, "request(PLAYER_info) failed");
        }
      g_infoplayer_info.af_is_requesting = 0;
      break;
    }
  ret = play_ret;
  test_audio_focus_debug_stack_display();
  return ret;
}

static int wait_focus_abandon(void **handle, char *name)
{
  int i = 0;
  int time_out = 10; // 1sTimeoutTime
  if (!*handle)
    {
      syslog(LOG_DEBUG, "handle is null in wait_focus_abandon\n");
    }
  while (*handle)
    {
      if (i >= time_out)
        {
          syslog(LOG_ERR,
                 "player reset time out in wait_focus_abandon\n");
          return -1;
        }
      syslog(LOG_INFO, "waitting %s focus abandon...\n", name);
      i++;
      usleep(100 * 1000); // 100ms
    }
  return 0;
}

void focus_abandon(test_player_info_t *info)
{
  void **req_handle = NULL;
  int ret = 0;
  char name[16] = {'\0'};

  if (info->state == MEDIA_EVENT_STOPPING ||
      info->state == MEDIA_EVENT_STOPPED)
    {
      ret = 0; // No longer repeatedly call media_player_reset
    }
  else
    {
      info->state = MEDIA_EVENT_STOPPING;
      if (info->handle != NULL)
        {
          if (info->type == PLAYER_RECORD)
            {
              ret = media_recorder_reset(info->handle);
            }
          else
            {
              ret = media_player_reset(info->handle);
            }
        }
    }

  if (ret < 0)
    {
      syslog(LOG_ERR, "player reset fail in focus_abandon\n");
    }

  if (info->type == PLAYER_MUSIC)
    {
      req_handle = &test_music_req_handle_0;
      strcpy(name, "MUSIC");
    }
  else if (info->type == PLAYER_NOTIFY)
    {
      req_handle = &test_notify_req_handle_0;
      strcpy(name, "NOTIFY");
    }
  else if (info->type == PLAYER_TTS)
    {
      req_handle = &test_tts_req_handle_0;
      strcpy(name, "TTS");
    }
  else if (info->type == PLAYER_ALARM)
    {
      req_handle = &test_alarm_req_handle_0;
      strcpy(name, "ALARM");
    }
  else if (info->type == PLAYER_SCO)
    {
      req_handle = &test_sco_req_handle_0;
      strcpy(name, "SCO");
    }
  else if (info->type == PLAYER_RING)
    {
      req_handle = &test_ring_req_handle_0;
      strcpy(name, "RING");
    }
  else if (info->type == PLAYER_ENFORCED)
    {
      req_handle = &test_enforced_req_handle_0;
      strcpy(name, "ENFORCED");
    }
  else if (info->type == PLAYER_RECORD)
    {
      req_handle = &test_record_req_handle_0;
      strcpy(name, "RECORD");
    }
  else if (info->type == PLAYER_HEALTH)
    {
      req_handle = &test_health_req_handle_0;
      strcpy(name, "HEALTH");
    }
  else if (info->type == PLAYER_SPORT)
    {
      req_handle = &test_sport_req_handle_0;
      strcpy(name, "SPORT");
    }
  else if (info->type == PLAYER_INFO)
    {
      req_handle = &test_info_req_handle_0;
      strcpy(name, "INFO");
    }
  // ret = wait_focus_abandon(req_handle, name);
  // if(ret != 0)
  // {
  if (*req_handle)
    {
      /* Force release */
      test_audio_focus_debug_stack_display();
      test_audio_focus_abandon(*req_handle);
      test_audio_focus_debug_stack_display();
      *req_handle = NULL;
    }
  // }
}

int player_prepare(test_player_info_t *info)
{
  int ret = 0;
  int return_type = AUDIO_FOCUS_PLAY;
  /* Play operation, abandon the focus point of the last same player */

  focus_abandon(info);

  /* request focus */
  return_type = player_request_play(info->type, info->url);
  // }
  if (return_type == -1)
    {
      return -1;
    }
  switch (return_type)
    {
    case AUDIO_FOCUS_PLAY:
      test_player_open(test_playing_type);
      syslog(LOG_DEBUG, "prepare url:%s\n", info->url);
      info->state = MEDIA_EVENT_PREPARING;
      if (info->type == PLAYER_RECORD)
        {
          ret = media_recorder_prepare(
              info->handle, info->url,
              "format=s16le:sample_rate=16000:ch_layout=mono");
        }
      else
        {
          ret = media_player_prepare(info->handle, info->url, NULL);
        }
      break;
    case AUDIO_FOCUS_STOP:
      ret = -1;
      break;
    case AUDIO_FOCUS_PLAY_WITH_DUCK:
      break;
    }
  return ret;
}

static int get_mq_curmsgs(char *path)
{
  struct mq_attr mqStat = {0};
  mqd_t mqid = -1;

  mqid = mq_open(path, O_RDWR);
  if (mqid < 0)
    {
      syslog(LOG_ERR, "[%s]:Error %d (%s) on mq_open.\n", __func__,
             errno, strerror(errno));
      return -1;
    }
  if (mq_getattr(mqid, &mqStat) == 0)
    {
      syslog(LOG_INFO, "get %s current message:%ld\n", path,
             mqStat.mq_curmsgs);
      mq_close(mqid);
      return mqStat.mq_curmsgs;
    }
  mq_close(mqid);
  return 0;
}

static int send_msg_to_player(test_msg_player_t *msg)
{
  int ret;
  int mqid = -1;
  int prio = PLAYER_MQ_PRIO;

  /* Leave some Message margin, when processing a Message if there are time-consuming operations (and a Message End needs to call send_msg_to_player() Interface),
    At this time if other multiple Threads have already called send_msg_to_player() in advance, it will cause mq_send() deadlock
  */
  if (get_mq_curmsgs(PLAYER_MQ_PAHT) >= PLAYER_MQ_MSG_LEN - 4)
    {
      syslog(LOG_INFO, "[%s]player mq too long, return\n", __func__);
      return 0;
    }
  mqid = mq_open(PLAYER_MQ_PAHT, O_RDWR);
  if (mqid < 0)
    {
      syslog(LOG_ERR, "Error %d (%s) on mq_open.\n", errno,
             strerror(errno));
      return -1;
    }

  if (msg->cmd == PLAYER_RESET)
    {
      prio++;
    }
  ret = mq_send(mqid, (const void *)msg, sizeof(*msg), prio);
  mq_close(mqid);
  if (ret < 0)
    {
      syslog(LOG_ERR, "Error %d (%s) on mq_send.\n", errno,
             strerror(errno));
      return -1;
    }
  syslog(LOG_DEBUG, "Send msg to player success.\n");
  return 0;
}

int test_register_player_callback(player_event_callback_t event_cb,
                                  void *ext_data)
{
  if (!event_cb)
    {
      syslog(LOG_ERR,
             "Parames error on test_register_player_callback.\n");
      return -1;
    }
  player_event_callback = (player_event_callback_t)event_cb;
  player_user_data = ext_data;
  return 0;
}

int test_player_play(test_player_type_t type, const char *url)
{

  syslog(LOG_DEBUG, "test_player_play type:%d, url:%s\n", type, url);

  if (!url || strlen(url) == 0)
    {
      return -1;
    }
  test_msg_player_t msg;

  char *msg_buf = malloc(512);
  memset(msg_buf, '\0', 512);
  strcpy(msg_buf, url);

  syslog(LOG_DEBUG, "send play  type:%d, url:%s\n", type, msg_buf);

  msg.type = type;
  msg.cmd = PLAYER_PLAY;
  msg.p1 = msg_buf;

  return send_msg_to_player(&msg);
}

int test_player_start(test_player_type_t type)
{
  test_msg_player_t msg;

  msg.type = type;
  msg.cmd = PLAYER_START;
  msg.p1 = NULL;

  return send_msg_to_player(&msg);
}

int test_player_pause(test_player_type_t type)
{
  test_msg_player_t msg;

  msg.type = type;
  msg.cmd = PLAYER_PAUSE;
  msg.p1 = NULL;

  return send_msg_to_player(&msg);
}

int test_player_resume(test_player_type_t type)
{
  test_msg_player_t msg;

  msg.type = type;
  msg.cmd = PLAYER_RESUME;
  msg.p1 = NULL;

  return send_msg_to_player(&msg);
}

int test_player_stop(test_player_type_t type)
{
  test_msg_player_t msg;

  msg.type = type;
  msg.cmd = PLAYER_STOP;
  msg.p1 = NULL;

  return send_msg_to_player(&msg);
}

int test_player_reset(test_player_type_t type)
{
  test_msg_player_t msg;

  msg.type = type;
  msg.cmd = PLAYER_RESET;
  msg.p1 = NULL;

  return send_msg_to_player(&msg);
}

int test_player_set_position(test_player_type_t type, int pos_in_ms)
{
  test_msg_player_t msg;
  int ret = 0;

  if (pos_in_ms < 0)
    {
      return -1;
    }
  int *p = malloc(sizeof(int));
  *p = pos_in_ms;
  msg.type = type;
  msg.cmd = PLAYER_SET_POS;
  msg.p1 = p;

  ret = send_msg_to_player(&msg);

  return ret;
}

int test_player_get_position(test_player_type_t type)
{
  test_msg_player_t msg;
  int ret = 0;

  msg.type = type;
  msg.cmd = PLAYER_GET_POS;
  msg.p1 = NULL;

  ret = send_msg_to_player(&msg);
  usleep(100);

  return ret;
}

int test_player_get_duration(test_player_type_t type)
{
  test_msg_player_t msg;
  int ret = 0;

  msg.type = type;
  msg.cmd = PLAYER_GET_DUR;
  msg.p1 = NULL;

  ret = send_msg_to_player(&msg);
  usleep(100);

  return ret;
}

int test_player_get_volume(test_player_type_t type)
{
  test_msg_player_t msg;
  int ret = 0;

  msg.type = type;
  msg.cmd = PLAYER_GET_VOLUME;
  msg.p1 = NULL;

  ret = send_msg_to_player(&msg);

  return ret;
}
int test_player_get_isplaying(test_player_type_t type)
{
  test_msg_player_t msg;
  int ret = 0;

  msg.type = type;
  msg.cmd = PLAYER_ISPLAYING;
  msg.p1 = NULL;

  ret = send_msg_to_player(&msg);

  return ret;
}

int test_player_close(test_player_type_t type)
{
  test_msg_player_t msg;
  int ret = 0;

  msg.type = type;
  msg.cmd = PLAYER_CLOSE;
  msg.p1 = NULL;

  ret = send_msg_to_player(&msg);

  return ret;
}

int test_player_set_volume(test_player_type_t type, float volume)
{
  test_msg_player_t msg;
  int ret = 0;

  float *p = (float *)malloc(sizeof(float));
  *p = volume;
  msg.type = type;
  msg.cmd = PLAYER_SET_VOLUME;
  msg.p1 = p;

  ret = send_msg_to_player(&msg);

  return ret;
}

test_player_info_t *test_get_music_player_info(void)
{
  if (g_musicplayer_info.handle)
    {
      return (&g_musicplayer_info);
    }
  return NULL;
}

test_player_info_t *test_get_notify_player_info(void)
{
  if (g_notifyplayer_info.handle)
    {
      return (&g_notifyplayer_info);
    }
  return NULL;
}

test_player_info_t *test_get_tts_player_info(void)
{
  if (g_ttsplayer_info.handle)
    {
      return (&g_ttsplayer_info);
    }
  return NULL;
}

test_player_info_t *test_get_alarm_player_info(void)
{
  if (g_alarmplayer_info.handle)
    {
      return (&g_alarmplayer_info);
    }
  return NULL;
}

test_player_info_t *test_get_sco_player_info(void)
{
  if (g_scoplayer_info.handle)
    {
      return (&g_scoplayer_info);
    }
  return NULL;
}

test_player_info_t *test_get_ring_player_info(void)
{
  if (g_ringplayer_info.handle)
    {
      return (&g_ringplayer_info);
    }
  return NULL;
}

test_player_info_t *test_get_enforced_player_info(void)
{
  if (g_enforcedplayer_info.handle)
    {
      return (&g_enforcedplayer_info);
    }
  return NULL;
}

test_player_info_t *test_get_record_player_info(void)
{
  if (g_recordplayer_info.handle)
    {
      return (&g_recordplayer_info);
    }
  return NULL;
}

test_player_info_t *test_get_health_player_info(void)
{
  if (g_healthplayer_info.handle)
    {
      return (&g_healthplayer_info);
    }
  return NULL;
}
test_player_info_t *test_get_sport_player_info(void)
{
  if (g_sportplayer_info.handle)
    {
      return (&g_sportplayer_info);
    }
  return NULL;
}

test_player_info_t *test_get_info_player_info(void)
{
  if (g_infoplayer_info.handle)
    {
      return (&g_infoplayer_info);
    }
  return NULL;
}

int test_clear_player_mq(void)
{
  struct mq_attr mqStat = {0};
  test_msg_player_t msg = {0};
  mqd_t mqid = -1;
  const struct timespec abstime = {
      .tv_sec = 0,
      .tv_nsec = 1000,
  };

  mqid = mq_open(PLAYER_MQ_PAHT, O_RDWR);
  if (mqid < 0)
    {
      syslog(LOG_ERR, "Error %d (%s) on mq_open.\n", errno,
             strerror(errno));
      return -1;
    }
  if (mq_getattr(mqid, &mqStat) == 0)
    {
      if (mqStat.mq_curmsgs > 0)
        {
          syslog(LOG_INFO, "clear player mq\n");
          for (int i = 0; i < mqStat.mq_curmsgs; i++)
            {
              if (mq_timedreceive(mqid, (void *)&msg,
                                  sizeof(test_msg_player_t), NULL,
                                  &abstime) != -1)
                {
                  if (msg.p1)
                    {
                      free(msg.p1);
                      msg.p1 = NULL;
                    }
                }
            }
        }
    }
  mq_close(mqid);
  return 0;
}

int test_player_open(test_player_type_t player_type)
{
  int ret = -1;
  switch (player_type)
    {
    case PLAYER_MUSIC:
      syslog(LOG_INFO, "THE music handle is %p\n", g_musicplayer_handle);
      if (g_musicplayer_info.handle == NULL)
        {
          g_musicplayer_handle = media_player_open("Music");
          if (g_musicplayer_handle == NULL)
            {
              syslog(LOG_ERR, "audioplayer open failed\n");
            }
          g_musicplayer_info.handle = g_musicplayer_handle;
          g_musicplayer_info.af_req_handle = &test_music_req_handle_0;
          ret = media_player_set_event_callback(
              g_musicplayer_handle, (void *)&g_musicplayer_info,
              audioplayer_event_callback);
          if (ret != 0)
            {
              syslog(LOG_ERR,
                     "Error %d (%s) on "
                     "media_player_set_event_callback.\n",
                     errno, strerror(errno));
            }
        }
      break;
    case PLAYER_NOTIFY:
      if (g_notifyplayer_info.handle == NULL)
        {
          g_notifyplayer_handle = media_player_open("Notify");
          if (g_notifyplayer_handle == NULL)
            {
              syslog(LOG_ERR, "audioplayer open failed\n");
            }
          g_notifyplayer_info.handle = g_notifyplayer_handle;
          g_notifyplayer_info.af_req_handle = &test_notify_req_handle_0;
          ret = media_player_set_event_callback(
              g_notifyplayer_handle, (void *)&g_notifyplayer_info,
              audioplayer_event_callback);
          if (ret != 0)
            {
              syslog(LOG_ERR,
                     "Error %d (%s) on "
                     "media_player_set_event_callback.\n",
                     errno, strerror(errno));
            }
        }
      break;
    case PLAYER_TTS:
      if (g_ttsplayer_info.handle == NULL)
        {
          g_ttsplayer_handle = media_player_open("TTS");
          if (g_ttsplayer_handle == NULL)
            {
              syslog(LOG_ERR, "audioplayer open failed\n");
            }
          g_ttsplayer_info.handle = g_ttsplayer_handle;
          g_ttsplayer_info.af_req_handle = &test_tts_req_handle_0;
          ret = media_player_set_event_callback(
              g_ttsplayer_handle, (void *)&g_ttsplayer_info,
              audioplayer_event_callback);
          if (ret != 0)
            {
              syslog(LOG_ERR,
                     "Error %d (%s) on "
                     "media_player_set_event_callback.\n",
                     errno, strerror(errno));
            }
        }
      break;
    case PLAYER_ALARM:
      if (g_alarmplayer_info.handle == NULL)
        {
          g_alarmplayer_handle = media_player_open("Alarm");
          if (g_alarmplayer_handle == NULL)
            {
              syslog(LOG_ERR, "audioplayer open failed\n");
            }
          g_alarmplayer_info.handle = g_alarmplayer_handle;
          g_alarmplayer_info.af_req_handle = &test_alarm_req_handle_0;
          ret = media_player_set_event_callback(
              g_alarmplayer_handle, (void *)&g_alarmplayer_info,
              audioplayer_event_callback);
          if (ret != 0)
            {
              syslog(LOG_ERR,
                     "Error %d (%s) on "
                     "media_player_set_event_callback.\n",
                     errno, strerror(errno));
            }
        }
      break;
    case PLAYER_RING:
      if (g_ringplayer_info.handle == NULL)
        {
          g_ringplayer_handle = media_player_open("Ring");
          if (g_ringplayer_handle == NULL)
            {
              syslog(LOG_ERR, "audioplayer open failed\n");
            }
          g_ringplayer_info.handle = g_ringplayer_handle;
          g_ringplayer_info.af_req_handle = &test_ring_req_handle_0;
          ret = media_player_set_event_callback(
              g_ringplayer_handle, (void *)&g_ringplayer_info,
              audioplayer_event_callback);
          if (ret != 0)
            {
              syslog(LOG_ERR,
                     "Error %d (%s) on "
                     "media_player_set_event_callback.\n",
                     errno, strerror(errno));
            }
        }
      break;
    case PLAYER_ENFORCED:
      if (g_enforcedplayer_info.handle == NULL)
        {
          g_enforcedplayer_handle = media_player_open("Enforced");
          if (g_enforcedplayer_handle == NULL)
            {
              syslog(LOG_ERR, "audioplayer open failed\n");
            }
          g_enforcedplayer_info.handle = g_enforcedplayer_handle;
          g_enforcedplayer_info.af_req_handle =
              &test_enforced_req_handle_0;
          ret = media_player_set_event_callback(
              g_enforcedplayer_handle, (void *)&g_enforcedplayer_info,
              audioplayer_event_callback);
          if (ret != 0)
            {
              syslog(LOG_ERR,
                     "Error %d (%s) on "
                     "media_player_set_event_callback.\n",
                     errno, strerror(errno));
            }
        }
      break;
    case PLAYER_HEALTH:
      if (g_healthplayer_info.handle == NULL)
        {
          g_healthplayer_handle = media_player_open("Health");
          if (g_healthplayer_handle == NULL)
            {
              syslog(LOG_ERR, "audioplayer open failed\n");
            }
          g_healthplayer_info.handle = g_healthplayer_handle;
          g_healthplayer_info.af_req_handle = &test_health_req_handle_0;
          ret = media_player_set_event_callback(
              g_healthplayer_handle, (void *)&g_healthplayer_info,
              audioplayer_event_callback);
          if (ret != 0)
            {
              syslog(LOG_ERR,
                     "Error %d (%s) on "
                     "media_player_set_event_callback.\n",
                     errno, strerror(errno));
            }
        }
      break;
    case PLAYER_SPORT:
      if (g_sportplayer_info.handle == NULL)
        {
          g_sportplayer_handle = media_player_open("Sport");
          if (g_sportplayer_handle == NULL)
            {
              syslog(LOG_ERR, "audioplayer open failed\n");
            }
          g_sportplayer_info.handle = g_sportplayer_handle;
          g_sportplayer_info.af_req_handle = &test_sport_req_handle_0;
          ret = media_player_set_event_callback(
              g_sportplayer_handle, (void *)&g_sportplayer_info,
              audioplayer_event_callback);
          if (ret != 0)
            {
              syslog(LOG_ERR,
                     "Error %d (%s) on "
                     "media_player_set_event_callback.\n",
                     errno, strerror(errno));
            }
        }
      break;
    case PLAYER_INFO:
      if (g_infoplayer_info.handle == NULL)
        {
          g_infoplayer_handle = media_player_open("Info");
          if (g_infoplayer_handle == NULL)
            {
              syslog(LOG_ERR, "audioplayer open failed\n");
            }
          g_infoplayer_info.handle = g_infoplayer_handle;
          g_infoplayer_info.af_req_handle = &test_info_req_handle_0;
          ret = media_player_set_event_callback(
              g_infoplayer_handle, (void *)&g_infoplayer_info,
              audioplayer_event_callback);
          if (ret != 0)
            {
              syslog(LOG_ERR,
                     "Error %d (%s) on "
                     "media_player_set_event_callback.\n",
                     errno, strerror(errno));
            }
        }
      break;
    case PLAYER_RECORD:
      if (g_recordplayer_info.handle == NULL)
        {
          g_recordplayer_handle =
              media_recorder_open("amoviesink_async");
          if (g_recordplayer_handle == NULL)
            {
              syslog(LOG_ERR, "audioplayer open failed\n");
            }
          g_recordplayer_info.handle = g_recordplayer_handle;
          g_recordplayer_info.af_req_handle = &test_record_req_handle_0;
          ret = media_recorder_set_event_callback(
              g_recordplayer_handle, (void *)&g_recordplayer_info,
              audioplayer_event_callback);
          if (ret != 0)
            {
              syslog(LOG_ERR,
                     "Error %d (%s) on "
                     "media_player_set_event_callback.\n",
                     errno, strerror(errno));
            }
        }
      break;
    case PLAYER_SCO:
      if (g_scoplayer_info.handle == NULL)
        {
          g_scoplayer_handle = media_player_open("SCO");
          if (g_scoplayer_handle == NULL)
            {
              syslog(LOG_ERR, "audioplayer open failed\n");
            }
          g_scoplayer_info.handle = g_scoplayer_handle;
          g_scoplayer_info.af_req_handle = &test_sco_req_handle_0;
          ret = media_player_set_event_callback(
              g_scoplayer_handle, (void *)&g_scoplayer_info,
              audioplayer_event_callback);
          if (ret != 0)
            {
              syslog(LOG_ERR,
                     "Error %d (%s) on "
                     "media_player_set_event_callback.\n",
                     errno, strerror(errno));
            }
        }
      break;
    }
  return ret;
}

int test_set_info(test_player_info_t **test_player_info,
                         test_player_type_t player_type)
{
  switch (player_type)
    {
    case PLAYER_MUSIC:
      *test_player_info = &g_musicplayer_info;
      break;
    case PLAYER_NOTIFY:
      *test_player_info = &g_notifyplayer_info;
      break;
    case PLAYER_TTS:
      *test_player_info = &g_ttsplayer_info;
      break;
    case PLAYER_ALARM:
      *test_player_info = &g_alarmplayer_info;
      break;
    case PLAYER_RING:
      *test_player_info = &g_ringplayer_info;
      break;
    case PLAYER_ENFORCED:
      *test_player_info = &g_enforcedplayer_info;
      break;
    case PLAYER_HEALTH:
      *test_player_info = &g_healthplayer_info;
      break;
    case PLAYER_SPORT:
      *test_player_info = &g_sportplayer_info;
      break;
    case PLAYER_INFO:
      *test_player_info = &g_infoplayer_info;
      break;
    case PLAYER_RECORD:
      *test_player_info = &g_recordplayer_info;
      break;
    case PLAYER_SCO:
      *test_player_info = &g_scoplayer_info;
      break;
    }
  return 0;
}