
#include <nuttx/nuttx.h>
#include "audio_focus.h"
#include "audio_list.h"
#include "audio_manager_test.h"
#include "audio_player.h"
#include "kvdb.h"
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <media_api.h>
#include <nuttx/config.h>
#include <nuttx/sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static test_play_list_t *play_list;
static int g_music_stopped = 0;
static int g_notify_stopped = 0;
static int g_tts_stopped = 0;
static int g_alarm_stopped = 0;
static int g_sco_stopped = 0;
static int g_ring_stopped = 0;
static int g_enforced_stopped = 0;
static int g_record_stopped = 0;
static int g_health_stopped = 0;
static int g_sport_stopped = 0;
static int g_info_stopped = 0;
static int g_alarm_completed = 0;
static int g_music_completed = 0;
static int g_tts_completed = 0;
static int g_notify_completed = 0;
static int g_sco_completed = 0;
static int g_ring_completed = 0;
static int g_enforced_completed = 0;
static int g_record_completed = 0;
static int g_health_completed = 0;
static int g_sport_completed = 0;
static int g_info_completed = 0;
static int g_tool_exit = 0;
static pthread_mutex_t play_ctrl_mutex;
static int music_continuous_err_num = 0;
#define PLAY_LIST_PATH "/data/1.txt"

static loop_mode_t g_loop_mode = 1;
static test_playing_song_info_t g_playing_song_info = {0};

extern void *test_music_req_handle_0;
extern void *test_notify_req_handle_0;
extern void *test_tts_req_handle_0;
extern void *test_alarm_req_handle_0;
extern void *test_sco_req_handle_0;
extern void *test_ring_req_handle_0;
extern void *test_enforced_req_handle_0;
extern void *test_record_req_handle_0;
extern void *test_health_req_handle_0;
extern void *test_sport_req_handle_0;
extern void *test_info_req_handle_0;

static test_player_type_t stream_types[] = {
    PLAYER_MUSIC,  PLAYER_NOTIFY, PLAYER_TTS,      PLAYER_ALARM,
    PLAYER_SCO,    PLAYER_RING,   PLAYER_ENFORCED, PLAYER_RECORD,
    PLAYER_HEALTH, PLAYER_SPORT,  PLAYER_INFO};

static test_player_info_t *test_get_player_info(test_player_type_t type)
{
  test_player_info_t *info = NULL;
  switch (type)
    {
    case PLAYER_MUSIC:
      info = test_get_music_player_info();
      break;
    case PLAYER_NOTIFY:
      info = test_get_notify_player_info();
      break;
    case PLAYER_TTS:
      info = test_get_tts_player_info();
      break;
    case PLAYER_ALARM:
      info = test_get_alarm_player_info();
      break;
    case PLAYER_SCO:
      info = test_get_sco_player_info();
      break;
    case PLAYER_RING:
      info = test_get_ring_player_info();
      break;
    case PLAYER_ENFORCED:
      info = test_get_enforced_player_info();
      break;
    case PLAYER_RECORD:
      info = test_get_record_player_info();
      break;
    case PLAYER_HEALTH:
      info = test_get_health_player_info();
      break;
    case PLAYER_SPORT:
      info = test_get_sport_player_info();
      break;
    case PLAYER_INFO:
      info = test_get_info_player_info();
      break;
    }
  return info;
}

static test_player_info_t *
test_get_player_focus_handle(test_player_type_t type)
{
  void *handle = NULL;
  switch (type)
    {
    case PLAYER_MUSIC:
      handle = test_music_req_handle_0;
      break;
    case PLAYER_NOTIFY:
      handle = test_notify_req_handle_0;
      break;
    case PLAYER_TTS:
      handle = test_tts_req_handle_0;
      break;
    case PLAYER_ALARM:
      handle = test_alarm_req_handle_0;
      break;
    case PLAYER_SCO:
      handle = test_sco_req_handle_0;
      break;
    case PLAYER_RING:
      handle = test_ring_req_handle_0;
      break;
    case PLAYER_ENFORCED:
      handle = test_enforced_req_handle_0;
      break;
    case PLAYER_RECORD:
      handle = test_record_req_handle_0;
      break;
    case PLAYER_HEALTH:
      handle = test_health_req_handle_0;
      break;
    case PLAYER_SPORT:
      handle = test_sport_req_handle_0;
      break;
    case PLAYER_INFO:
      handle = test_info_req_handle_0;
      break;
    }
  return handle;
}

static test_player_type_t test_get_player_type_by_focus(void *handle)
{
  if (handle == NULL)
    return -1;

  test_player_type_t play_type = -1;
  if (handle == test_music_req_handle_0)
    {
      play_type = PLAYER_MUSIC;
    }
  else if (handle == test_notify_req_handle_0)
    {
      play_type = PLAYER_NOTIFY;
    }
  else if (handle == test_tts_req_handle_0)
    {
      play_type = PLAYER_TTS;
    }
  else if (handle == test_alarm_req_handle_0)
    {
      play_type = PLAYER_ALARM;
    }
  if (handle == test_sco_req_handle_0)
    {
      play_type = PLAYER_SCO;
    }
  else if (handle == test_ring_req_handle_0)
    {
      play_type = PLAYER_RING;
    }
  else if (handle == test_enforced_req_handle_0)
    {
      play_type = PLAYER_ENFORCED;
    }
  else if (handle == test_record_req_handle_0)
    {
      play_type = PLAYER_RECORD;
    }
  if (handle == test_sport_req_handle_0)
    {
      play_type = PLAYER_SPORT;
    }
  else if (handle == test_health_req_handle_0)
    {
      play_type = PLAYER_HEALTH;
    }
  else if (handle == test_info_req_handle_0)
    {
      play_type = PLAYER_INFO;
    }
  return play_type;
}

static test_player_info_t *test_get_player_info_by_focus(void *handle)
{
  test_player_info_t *info = NULL;
  if (handle == test_music_req_handle_0)
    {
      info = test_get_music_player_info();
    }
  else if (handle == test_notify_req_handle_0)
    {
      info = test_get_notify_player_info();
    }
  else if (handle == test_tts_req_handle_0)
    {
      info = test_get_tts_player_info();
    }
  else if (handle == test_alarm_req_handle_0)
    {
      info = test_get_alarm_player_info();
    }
  if (handle == test_sco_req_handle_0)
    {
      info = test_get_sco_player_info();
    }
  else if (handle == test_ring_req_handle_0)
    {
      info = test_get_ring_player_info();
    }
  else if (handle == test_enforced_req_handle_0)
    {
      info = test_get_enforced_player_info();
    }
  else if (handle == test_record_req_handle_0)
    {
      info = test_get_record_player_info();
    }
  if (handle == test_sport_req_handle_0)
    {
      info = test_get_sport_player_info();
    }
  else if (handle == test_health_req_handle_0)
    {
      info = test_get_health_player_info();
    }
  else if (handle == test_info_req_handle_0)
    {
      info = test_get_info_player_info();
    }
  return info;
}

void test_play_dump(void)
{
  media_policy_dump(NULL);
  media_graph_dump(NULL);
}

static int
test_update_song_items_to_player_info(test_player_type_t type,
                                      test_song_items_t *items)
{
  int ret = -1;

  test_player_info_t *test_player_info = test_get_player_info(type);
  if (type != PLAYER_MUSIC || !test_player_info)
    {
      return ret;
    }

  /* audio_player items*/
  if (items)
    {
      test_player_info->offset_in_ms = items->offset_in_ms;
    }
  else
    {
      test_player_info->offset_in_ms = 0;
    }
  ret = 0;

  return ret;
}

static void test_update_playing_song_info_by_entry(
    test_playing_song_info_t *info, test_song_entry_t *test_song_entry)
{
  info->test_song_entry = test_song_entry;
}

static void test_play_ctrl_lock(void)
{
  pthread_mutex_lock(&play_ctrl_mutex);
}

static void test_play_ctrl_unlock(void)
{
  pthread_mutex_unlock(&play_ctrl_mutex);
}

static void test_play_ctrl_mutex_init(void)
{
  pthread_mutex_init(&play_ctrl_mutex, NULL);
  syslog(LOG_INFO, "play ctrl mutext init:%p\n", &play_ctrl_mutex);
}

static void test_play_ctrl_mutex_deinit(void)
{
  pthread_mutex_destroy(&play_ctrl_mutex);
}

int test_start_player_play(test_player_type_t type,
                           test_song_entry_t *test_song_entry)
{
  int ret = 0;

  syslog(LOG_INFO, "%s\n", __func__);
  /* reset player info */
  test_player_info_t *test_player_info = NULL;
  if (PLAYER_MUSIC == type)
    {
      test_player_info = test_get_music_player_info();
    }
  else if (PLAYER_ALARM == type)
    {
      test_player_info = test_get_alarm_player_info();
    }
  else if (PLAYER_SPORT)
    {
      test_player_info = test_get_sport_player_info();
    }
  if (test_player_info)
    {
      test_player_info->duration = 0;
      test_player_info->position = 0;
    }

  test_update_song_items_to_player_info(
      type, test_song_entry->test_song_items);
  ret = test_player_play(type, test_song_entry->song_url);
  test_update_playing_song_info_by_entry(&g_playing_song_info,
                                         test_song_entry);

  return ret;
}

int test_play_cur_song(void)
{
  int ret = 0;
  TEST_CHECK_PLAYING_SONG(g_playing_song_info);
  test_play_ctrl_lock();
  ret = test_start_player_play(PLAYER_MUSIC,
                               g_playing_song_info.test_song_entry);
  test_play_ctrl_unlock();
  return ret;
}

int test_play_prev_song(void)
{
  int ret = 0;
  TEST_CHECK_PLAYING_SONG(g_playing_song_info);
  test_song_entry_t *test_song_entry =
      g_playing_song_info.test_song_entry;
  int type = PLAYER_MUSIC;

  test_play_ctrl_lock();
  test_get_prev_song_entry(g_playing_song_info.test_song_entry,
                           &test_song_entry);
  if (!test_song_entry)
    {
      syslog(LOG_ERR, "get prev song failed\n");
      test_song_entry = g_playing_song_info.test_song_entry;
    }

  ret = test_start_player_play(type, test_song_entry);
  test_play_ctrl_unlock();

  return ret;
}

int test_play_next_song(void)
{
  int ret = 0;
  TEST_CHECK_PLAYING_SONG(g_playing_song_info);
  // char song_id[24] = {0};
  test_song_entry_t *test_song_entry =
      g_playing_song_info.test_song_entry;
  int type = PLAYER_MUSIC;

  syslog(LOG_INFO, "get next song...\n");
  test_play_ctrl_lock();
  test_get_next_song_entry(g_playing_song_info.test_song_entry,
                           &test_song_entry);
  if (!test_song_entry)
    {
      syslog(LOG_ERR, "get next song failed\n");
      test_song_entry = g_playing_song_info.test_song_entry;
    }

  ret = test_start_player_play(type, test_song_entry);
  test_play_ctrl_unlock();

  return ret;
}

int test_play_resume_song(void)
{
  int ret = 0;
  TEST_CHECK_PLAYING_SONG(g_playing_song_info);
  test_player_info_t *test_player_info = NULL;
  test_player_type_t type = PLAYER_MUSIC;
  test_player_info = test_get_player_info(type);
  if (test_player_info == NULL)
    {
      return -1;
    }
  int state = test_player_info->state;
  if (state == MEDIA_EVENT_PAUSED)
    {
      ret = test_player_resume(type);
    }
  test_player_info->query_pause = 0;
  return ret;
}

int test_play_pause_song(void)
{
  int ret = 0;
  TEST_CHECK_PLAYING_SONG(g_playing_song_info);
  test_player_info_t *test_player_info = NULL;
  test_player_type_t type = PLAYER_MUSIC;
  test_player_info = test_get_player_info(type);
  if (test_player_info == NULL)
    {
      return -1;
    }
  int state = test_player_info->state;
  if (state == MEDIA_EVENT_STARTED || state == MEDIA_EVENT_SEEKED)
    {
      ret = test_player_pause(type);
    }
  test_player_info->query_pause = 1;
  return ret;
}

int test_play_stop_song(void)
{
  int ret = 0;
  test_player_info_t *audio_player_info = test_get_music_player_info();

  if (audio_player_info == NULL)
    {
      return -1;
    }
  ret = test_player_reset(PLAYER_MUSIC);
  if (test_music_req_handle_0)
    {
      test_audio_focus_debug_stack_display();
      test_audio_focus_abandon(test_music_req_handle_0);
      test_audio_focus_debug_stack_display();
      test_music_req_handle_0 = NULL;
    }

  return ret;
}

int test_play_common_stop(test_player_type_t type)
{
  int ret = 0;
  void *handle = test_get_player_focus_handle(type);
  test_player_info_t *test_player_info = test_get_player_info(type);

  if (test_player_info == NULL)
    {
      return -1;
    }
  ret = test_player_stop(type);
  if (handle)
    {
      test_audio_focus_debug_stack_display();
      test_audio_focus_abandon(handle);
      test_audio_focus_debug_stack_display();
      handle = NULL;
    }

  return ret;
}

int test_get_current_position(test_player_type_t type)
{
  int ret = -1;
  test_player_info_t *info = test_get_player_info(type);
  if (!info)
    {
      return -1;
    }
  if ((!info->state) || (info->state == MEDIA_EVENT_STOPPED))
    {
      return ret;
    }
  if (test_player_get_position(type) == 0)
    {
      return info->position;
    }
  return ret;
}

int test_get_play_duration(test_player_type_t type)
{
  int ret = -1;

  test_player_info_t *info = test_get_player_info(type);

  if (info == NULL)
    return ret;

  if ((!info->state) || (info->state == MEDIA_EVENT_STOPPED))
    {
      return ret;
    }
  if (test_player_get_duration(type) == 0)
    {
      return info->duration;
    }
  return ret;
}

int test_audio_player_is_playing(void)
{
  test_player_info_t *audio_player_info = test_get_music_player_info();

  if (audio_player_info &&
      audio_player_info->state == MEDIA_EVENT_STARTED)
    {
      return 1;
    }
  else if (audio_player_info &&
           audio_player_info->state == MEDIA_EVENT_PAUSED)
    {
      return 2;
    }
  else if (audio_player_info &&
           (audio_player_info->state == MEDIA_EVENT_NOP ||
            audio_player_info->state == MEDIA_EVENT_STOPPED))
    {
      return 0;
    }
  return -1;
}

int test_play_common_is_playing(test_player_type_t type)
{
  test_player_info_t *test_player_info = test_get_player_info(type);

  if (test_player_info && test_player_info->state == MEDIA_EVENT_STARTED)
    {
      return 1;
    }
  else if (test_player_info &&
           test_player_info->state == MEDIA_EVENT_PAUSED)
    {
      return 2;
    }
  else if (test_player_info &&
           (test_player_info->state == MEDIA_EVENT_NOP ||
            test_player_info->state == MEDIA_EVENT_STOPPED))
    {
      return 0;
    }
  return -1;
}

int test_get_player_play_status(void)
{
  int ret = test_audio_player_is_playing();

  syslog(LOG_INFO, "[%s]: audio_player state: %d\n", __func__, ret);
  return ret;
}

int test_get_which_player_is_playing(void)
{
  void *handle = test_get_af_stack_top();
  if (handle)
    {
      test_player_info_t *info = NULL;
      if (handle == test_music_req_handle_0)
        {
          info = test_get_music_player_info();
        }
      else if (handle == test_notify_req_handle_0)
        {
          info = test_get_notify_player_info();
        }
      else if (handle == test_tts_req_handle_0)
        {
          info = test_get_tts_player_info();
        }
      else if (handle == test_alarm_req_handle_0)
        {
          info = test_get_alarm_player_info();
        }
      else if (handle == test_sco_req_handle_0)
        {
          info = test_get_sco_player_info();
        }
      else if (handle == test_ring_req_handle_0)
        {
          info = test_get_ring_player_info();
        }
      else if (handle == test_enforced_req_handle_0)
        {
          info = test_get_enforced_player_info();
        }
      else if (handle == test_record_req_handle_0)
        {
          info = test_get_record_player_info();
        }
      else if (handle == test_sport_req_handle_0)
        {
          info = test_get_sport_player_info();
        }
      else if (handle == test_health_req_handle_0)
        {
          info = test_get_health_player_info();
        }
      else if (handle == test_info_req_handle_0)
        {
          info = test_get_info_player_info();
        }
      else
        {
          return -1;
        }
      if (!info)
        {
          return -1;
        }
      int state = info->state;
      if (state != MEDIA_EVENT_NOP && state != MEDIA_EVENT_PAUSED &&
          state != MEDIA_EVENT_STOPPED)
        {
          /* Currently playing */
          syslog(LOG_INFO, "[%s]:info->type=%d is playing\n", __func__,
                 info->type);
          return info->type;
        }
      else
        {
          return -1;
        }
    }
  return -1;
}

static void test_player_event_cb(test_player_info_t *info, int event,
                                 void *ext_data)
{
  test_player_info_t *test_player_info = info;

  switch (test_player_info->state)
    {
    case MEDIA_EVENT_STARTED:
      if (test_player_info->type == PLAYER_NOTIFY)
        {
          g_notify_completed = 0;
          g_notify_stopped = 0;
        }
      else if (test_player_info->type == PLAYER_TTS)
        {
          g_tts_completed = 0;
          g_tts_stopped = 0;
        }
      else if (test_player_info->type == PLAYER_MUSIC)
        {
          g_music_completed = 0;
          g_music_stopped = 0;
        }
      else if (test_player_info->type == PLAYER_ALARM)
        {
          g_alarm_completed = 0;
          g_alarm_stopped = 0;
        }
      else if (test_player_info->type == PLAYER_SCO)
        {
          g_sco_completed = 0;
          g_sco_stopped = 0;
        }
      else if (test_player_info->type == PLAYER_RING)
        {
          g_ring_completed = 0;
          g_ring_stopped = 0;
        }
      else if (test_player_info->type == PLAYER_ENFORCED)
        {
          g_enforced_completed = 0;
          g_enforced_stopped = 0;
        }
      else if (test_player_info->type == PLAYER_RECORD)
        {
          g_record_completed = 0;
          g_record_stopped = 0;
        }
      else if (test_player_info->type == PLAYER_HEALTH)
        {
          g_health_completed = 0;
          g_health_stopped = 0;
        }
      else if (test_player_info->type == PLAYER_SPORT)
        {
          g_sport_completed = 0;
          g_sport_stopped = 0;
        }
      else if (test_player_info->type == PLAYER_INFO)
        {
          g_info_completed = 0;
          g_info_stopped = 0;
        }
      break;
    case MEDIA_EVENT_PAUSED:
      break;
    case MEDIA_EVENT_COMPLETED:
      test_player_stop(test_player_info->type);
      if (test_player_info->type == PLAYER_MUSIC)
        {
          g_music_completed = 1;
        }
      else if (test_player_info->type == PLAYER_ALARM)
        {
          g_alarm_completed = 1;
        }
      else if (test_player_info->type == PLAYER_RING)
        {
          g_ring_completed = 1;
        }
      else if (test_player_info->type == PLAYER_SPORT)
        {
          g_sport_completed = 1;
        }
      break;
    case MEDIA_EVENT_STOPPED:
      /* Execute operation after playback completion */
      switch (test_player_info->type)
        {
        case PLAYER_MUSIC:
          syslog(LOG_INFO, "PLAYER_MUSIC stopped\n");
          g_music_stopped = 1;
          break;
        case PLAYER_NOTIFY:
          syslog(LOG_INFO, "PLAYER_NOTIFY stopped\n");
          g_notify_stopped = 1;
          break;
        case PLAYER_TTS:
          syslog(LOG_INFO, "PLAYER_TTS stopped\n");
          g_tts_stopped = 1;
          break;
        case PLAYER_ALARM:
          syslog(LOG_INFO, "PLAYER_ALARM stopped\n");
          g_alarm_stopped = 1;
          break;
        case PLAYER_SCO:
          syslog(LOG_INFO, "PLAYER_SCO stopped\n");
          g_sco_stopped = 1;
          break;
        case PLAYER_RING:
          syslog(LOG_INFO, "PLAYER_RING stopped\n");
          g_ring_stopped = 1;
          break;
        case PLAYER_ENFORCED:
          syslog(LOG_INFO, "PLAYER_ENFORCED stopped\n");
          g_enforced_stopped = 1;
          break;
        case PLAYER_RECORD:
          syslog(LOG_INFO, "PLAYER_RECORD stopped\n");
          g_record_stopped = 1;
          break;
        case PLAYER_HEALTH:
          syslog(LOG_INFO, "PLAYER_HEALTH stopped\n");
          g_health_stopped = 1;
          break;
        case PLAYER_SPORT:
          syslog(LOG_INFO, "PLAYER_SPORT stopped\n");
          g_sport_stopped = 1;
          break;
        case PLAYER_INFO:
          syslog(LOG_INFO, "PLAYER_INFO stopped\n");
          g_info_stopped = 1;
          break;
        default:
          break;
        }
      break;
    case MEDIA_EVENT_NOP:
      break;
    }
}

void test_play_close(test_player_type_t type)
{
  test_player_close(type);
}

void test_delete_all_play(void)
{
  void *af_top_handle;
  while (1)
    {
      af_top_handle = test_get_af_stack_top();
      if (af_top_handle == NULL)
        break;
      test_player_type_t type =
          test_get_player_type_by_focus(af_top_handle);
      test_player_stop(type);
      test_audio_focus_debug_stack_display();
      test_audio_focus_abandon(af_top_handle);
      test_audio_focus_debug_stack_display();
      usleep(10 * 1000);
      test_player_close(type);
    }
  int nums = sizeof(stream_types) / sizeof(stream_types[0]);
  for (int i = 0; i < nums; ++i)
    {
      test_player_info_t *tmp = test_get_player_info(stream_types[i]);
      if (tmp != NULL)
        {
          test_player_close(stream_types[i]);
          usleep(10 * 1000);
        }
    }
  g_tool_exit = 1;
}

void test_play_setint(char *stream, int volume)
{
  // char streams[20];
  // snprintf(streams, 20, "%s%s", stream, "Volume");
  media_policy_set_stream_volume(stream, volume);
}

static void test_load_play_list(void)
{
  syslog(LOG_INFO,
         "**************************************************************"
         "**************\n");
  syslog(LOG_INFO, "parse play list\n");

  play_list = test_play_list_init();
  if (!play_list)
    {
      play_list = test_play_list_init();
      if (!play_list)
        {
          syslog(LOG_INFO, "ERROE test_play_list_init failed\n");
          return;
        }
    }

  test_song_entry_t *test_song_entry = NULL;

  test_load_local_all_songs(PLAY_LIST_PATH, play_list);
  test_song_entry = container_of(play_list->song_head.next,
                                 test_song_entry_t, song_list);
  test_update_playing_song_info_by_entry(&g_playing_song_info,
                                         test_song_entry);

  test_print_play_list();
  return;
}

int test_load_local_all_songs(char *path, test_play_list_t *play_list_)
{
  if (!path || !play_list_)
    {
      return -1;
    }
  FILE *fp = fopen(path, "r");
  if (fp == NULL)
    {
      syslog(LOG_ERR, "file of play load failed\n");
      return -1;
    }

  char line[256];
  int id = 1;
  while (fgets(line, sizeof(line), fp) != NULL)
    {
      int len = strlen(line);
      char c_id[200];

      while (isspace(line[len - 1]))
        len--;

      line[len] = '\0';
      sprintf(c_id, "%d", id++);
      test_add_new_song(c_id, line, NULL, 1);
    }
  fclose(fp);
  return 0;
}

int test_get_audio_player_play_mode(void) { return g_loop_mode; }

void test_set_audio_player_play_mode(loop_mode_t mode)
{
  g_loop_mode = mode;
}

int test_audio_manager_init(void)
{
  int ret = 0;
  test_play_ctrl_mutex_init();
  pthread_t thread_id;
  pthread_attr_t attr;
  struct sched_param param;

  pthread_attr_init(&attr);

  param.sched_priority = 200;
  pthread_attr_setstacksize(&attr, 40 * 1024);

  pthread_attr_setschedparam(&attr, &param);

  pthread_create(&thread_id, &attr, test_audio_manager_task, NULL);

  pthread_setname_np(thread_id, "audio_manager");

  pthread_detach(thread_id);
  return ret;
}

static void *test_audio_manager_task(void *arg)
{
  int audio_play_mode = 0;
  test_player_info_t *audio_player_info = NULL;

  test_register_player_callback(test_player_event_cb, NULL);

  test_load_play_list();

  while (1)
    {
      if (g_music_stopped)
        {
          audio_player_info = test_get_music_player_info();
          if (test_music_req_handle_0)
            {
              test_audio_focus_debug_stack_display();
              test_audio_focus_abandon(test_music_req_handle_0);
              test_audio_focus_debug_stack_display();
              test_music_req_handle_0 = NULL;
            }
          g_music_stopped = 0;
          if (g_music_completed)
            {
              syslog(LOG_INFO, "audio normal stop\n");
              int gap = abs(audio_player_info->duration -
                            audio_player_info->position);
              syslog(LOG_INFO, "duration is %d,pos is %d, gap is %d\n",
                     audio_player_info->duration,
                     audio_player_info->position, gap);
              if (gap > 2000)
                {
                  syslog(LOG_WARNING, "Failed to verify integrity!!!\n");
                }

              g_music_completed = 0;
              audio_play_mode = test_get_audio_player_play_mode();

              if (audio_play_mode == LP_SINGLE)
                {
                  /* Single loop */
                  test_play_cur_song();
                }
              else
                {
                  /* Sequential playback */
                  test_play_next_song();
                }
            }
          /* error stop */
          else if (audio_player_info &&
                   audio_player_info->err_state < 0 &&
                   audio_player_info->err_state != -1414092869 &&
                   audio_player_info->err_state != -825242872)
            {
              syslog(LOG_INFO, "audio error stop 1\n");
              /* Sequential playback */
              sleep(2); // Switching too fast will cause play
                        // reset at request time, reset previous prepare
              if (audio_player_info->err_state < 0)
                {
                  if (audio_player_info->err_state == -1094995529 ||
                      audio_player_info->err_state == -5 ||
                      audio_player_info->err_state == -116)
                    {
                      music_continuous_err_num++;

                      syslog(LOG_INFO, "music continuous err_num :%d\n",
                             music_continuous_err_num);

                      syslog(LOG_INFO, "audio error stop 2\n");
                      test_play_next_song();
                    }
                }
            }
          /* force stop */
          else
            {

              syslog(LOG_INFO, "audio force stop\n");
            }
          syslog(LOG_INFO, "clear query pause flag\n");
          if (test_get_music_player_info())
            {
              test_get_music_player_info()->query_pause =
                  0; // clear query pause flag
            }
        }
      if (g_tts_stopped)
        {
          if (g_tts_completed)
            {
              syslog(LOG_INFO, "tts normal stop\n");
              g_tts_completed = 0;
            }
          if (test_tts_req_handle_0)
            {
              test_audio_focus_debug_stack_display();
              test_audio_focus_abandon(test_tts_req_handle_0);
              test_audio_focus_debug_stack_display();
              test_tts_req_handle_0 = NULL;
            }
        }
      if (g_notify_stopped)
        {
          if (g_notify_completed)
            {
              syslog(LOG_INFO, "notify normal stop\n");
              g_notify_completed = 0;
            }
          if (test_notify_req_handle_0)
            {
              test_audio_focus_debug_stack_display();
              test_audio_focus_abandon(test_notify_req_handle_0);
              test_audio_focus_debug_stack_display();
              test_notify_req_handle_0 = NULL;
            }
          g_notify_stopped = 0;
        }
      if (g_alarm_stopped)
        {
          g_alarm_stopped = 0;
          if (g_alarm_completed)
            {
              syslog(LOG_INFO, "alarm normal stop\n");
              g_alarm_completed = 0;
            }
          if (test_alarm_req_handle_0)
            {
              test_audio_focus_debug_stack_display();
              test_audio_focus_abandon(test_alarm_req_handle_0);
              test_audio_focus_debug_stack_display();
              test_alarm_req_handle_0 = NULL;
            }
        }
      if (g_sco_stopped)
        {
          g_sco_stopped = 0;
          if (g_sco_completed)
            {
              syslog(LOG_INFO, "sco normal stop\n");
              g_sco_completed = 0;
            }
          if (test_sco_req_handle_0)
            {
              test_audio_focus_debug_stack_display();
              test_audio_focus_abandon(test_sco_req_handle_0);
              test_audio_focus_debug_stack_display();
              test_sco_req_handle_0 = NULL;
            }
        }
      if (g_ring_stopped)
        {
          g_ring_stopped = 0;
          if (g_ring_completed)
            {
              syslog(LOG_INFO, "ring normal stop\n");
              g_ring_completed = 0;
            }
          if (test_ring_req_handle_0)
            {
              usleep(100 * 1000);
              test_audio_focus_debug_stack_display();
              test_audio_focus_abandon(test_ring_req_handle_0);
              test_audio_focus_debug_stack_display();
              test_ring_req_handle_0 = NULL;
            }
        }
      if (g_enforced_stopped)
        {
          g_enforced_stopped = 0;
          if (g_enforced_completed)
            {
              syslog(LOG_INFO, "enforced normal stop\n");
              g_enforced_completed = 0;
            }
          if (test_enforced_req_handle_0)
            {
              test_audio_focus_debug_stack_display();
              test_audio_focus_abandon(test_enforced_req_handle_0);
              test_audio_focus_debug_stack_display();
              test_enforced_req_handle_0 = NULL;
            }
        }
      if (g_record_stopped)
        {
          g_record_stopped = 0;
          if (g_record_completed)
            {
              syslog(LOG_INFO, "record normal stop\n");
              g_record_completed = 0;
            }
          if (test_record_req_handle_0)
            {
              test_audio_focus_debug_stack_display();
              test_audio_focus_abandon(test_record_req_handle_0);
              test_audio_focus_debug_stack_display();
              test_record_req_handle_0 = NULL;
            }
        }
      if (g_health_stopped)
        {
          g_health_stopped = 0;
          if (g_health_completed)
            {
              syslog(LOG_INFO, "health normal stop\n");
              g_health_completed = 0;
            }
          if (test_health_req_handle_0)
            {
              test_audio_focus_debug_stack_display();
              test_audio_focus_abandon(test_health_req_handle_0);
              test_audio_focus_debug_stack_display();
              test_health_req_handle_0 = NULL;
            }
        }
      if (g_sport_stopped)
        {
          g_sport_stopped = 0;
          if (g_sport_completed)
            {
              syslog(LOG_INFO, "sport normal stop\n");
              g_sport_completed = 0;
            }
          if (test_sport_req_handle_0)
            {
              test_audio_focus_debug_stack_display();
              test_audio_focus_abandon(test_sport_req_handle_0);
              test_audio_focus_debug_stack_display();
              test_sport_req_handle_0 = NULL;
            }
        }
      if (g_info_stopped)
        {
          g_info_stopped = 0;
          if (g_info_completed)
            {
              syslog(LOG_INFO, "info normal stop\n");
              g_info_completed = 0;
            }
          if (test_info_req_handle_0)
            {
              test_audio_focus_debug_stack_display();
              test_audio_focus_abandon(test_info_req_handle_0);
              test_audio_focus_debug_stack_display();
              test_info_req_handle_0 = NULL;
            }
        }
      usleep(10 * 1000);
      if (g_tool_exit == 1)
        {
          syslog(LOG_INFO, "mediatest manager exit\n");
          g_tool_exit = 0;
          test_delete_all_song_entry_of_play_list(play_list);
          break;
        }
    }

  test_play_list_deinit();
  test_play_ctrl_mutex_deinit();
  return 0;
}
