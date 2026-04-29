#include "mediatest_session_suit.h"
#include "media_common_test.h"
#include "mediatest_session.h"
#include <alloca.h>
#include <ctype.h>
#include <fcntl.h>
#include <media_api.h>
#include <mqueue.h>
#include <nuttx/nuttx.h>
#include <pthread.h>
#include <syslog.h>
#include <time.h>

uv_loop_t *g_mediatest_loop;
static pthread_t g_mediatest_thread;
static uv_async_queue_t g_mediatest_queue;
static test_play_list_t *mediatest_play_list;

static int audio_running;
#define MULTI_APPS_COUNT_MAX 11
#define BUFFER_SIZE 1024

static int get_index_by_stream(char *type);
static bool audio_set_control(uint8_t command, audio_info_t *ctx);
static void __audio_media_play(audio_attr_t *attr);
static void __audio_media_pause(audio_attr_t *attr);
static void __audio_media_resume(audio_attr_t *attr);
static void __audio_media_close(audio_attr_t *attr);
static void __audio_media_prev(audio_attr_t *attr);
static void __audio_media_next(audio_attr_t *attr);
static void __audio_media_seek(audio_attr_t *attr);
static void __audio_media_seek_percent(audio_attr_t *attr);
static void __audio_media_set_loop(audio_attr_t *attr);
static void __audio_media_duration(audio_attr_t *attr);
static void __audio_media_position(audio_attr_t *attr);
static void __audio_media_adjust_volume(audio_attr_t *attr);
static void __audio_media_increase_volume(audio_attr_t *attr);
static void __audio_media_decrease_volume(audio_attr_t *attr);
static void __audio_media_get_all(audio_attr_t *attr);
static void __audio_media_get_playstate(audio_attr_t *attr);
static int mediatest_load_local_all_songs(char *path,
                                          test_play_list_t *play_list_);
static void mediatest_load_play_list(int index, char *file);
static int mediatest_audio_init(void);
static void audio_init(audio_info_t *ctx, int index);
int mediatest_audio_loop(void);
static void __audio_media_exit(void);

static void __audio_media_open(audio_attr_t *attr);
static void __audio_media_prepare(audio_attr_t *attr);
static void __audio_media_start(audio_attr_t *attr);
static void __audio_media_stop(audio_attr_t *attr);
static void __audio_media_reset(audio_attr_t *attr);
static void __audio_media_sendmsg(audio_attr_t *attr);
static void __audio_media_set_graphvolume(audio_attr_t *attr);
static void __audio_media_get_graphvolume(audio_attr_t *attr);

static void recorder_notify_cb(struct multi_session_event *event);
static bool record_set_control(uint8_t command, audio_info_t *ctx);
static void __audio_record_play(audio_attr_t *attr);
static void __audio_record_pause(void);
static void __audio_record_resume(void);
static void __audio_record_close(void);

#define LOG_D(fmt, ...)                                                 \
  MEDIATEST_DEBUG(audio_manager, fmt, ##__VA_ARGS__)
#define LOG_I(fmt, ...) MEDIATEST_INFO(audio_manager, fmt, ##__VA_ARGS__)
#define LOG_W(fmt, ...)                                                 \
  MEDIATEST_WARNING(audio_manager, fmt, ##__VA_ARGS__)
#define LOG_E(fmt, ...) MEDIATEST_ERR(audio_manager, fmt, ##__VA_ARGS__)

static audio_info_t multi_apps[MULTI_APPS_COUNT_MAX] = {0};
static char *stream_lists[] = {"Ring",  "Alarm",    "Enforced", "Notify",
                               "TTS",   "Health",   "Sport",    "Info",
                               "Music", "Intercom", "Record"};
static audio_info_t record_app = {.stream = "Capture",
                                  .focus = MEDIA_SCENARIO_RECORD};
static int share_stream[] = {PLAYER_RING, PLAYER_NOTIFY, PLAYER_HEALTH,
                             PLAYER_INFO};
static int share_app_index = -1;
;
static int get_index_by_stream(char *type)
{
  if (!strcmp(MEDIA_STREAM_RING, type))
    {
      return PLAYER_RING;
    }
  else if (!strcmp(MEDIA_STREAM_ALARM, type))
    {
      return PLAYER_ALARM;
    }
  else if (!strcmp(MEDIA_STREAM_SYSTEM_ENFORCED, type))
    {
      return PLAYER_ENFORCED;
    }
  else if (!strcmp(MEDIA_STREAM_NOTIFICATION, type))
    {
      return PLAYER_NOTIFY;
    }
  else if (!strcmp(MEDIA_STREAM_TTS, type))
    {
      return PLAYER_TTS;
    }
  else if (!strcmp(MEDIA_STREAM_ACCESSIBILITY, type))
    {
      return PLAYER_HEALTH;
    }
  else if (!strcmp(MEDIA_STREAM_SPORT, type))
    {
      return PLAYER_SPORT;
    }
  else if (!strcmp(MEDIA_STREAM_INFO, type))
    {
      return PLAYER_INFO;
    }
  else if (!strcmp(MEDIA_STREAM_MUSIC, type))
    {
      return PLAYER_MUSIC;
    }
  else if (!strcmp(MEDIA_STREAM_COMMUNICATION, type))
    {
      return PLAYER_INTERCOM;
    }
  else
    {
      LOG_E("stream type error\n");
    }

  return 0xFFFFFFFF;
}

/****************************************************************************
 * Media Funtions
 ****************************************************************************/
static void switch_to_upper(char *str)
{
  for (int i = 0; str[i] != '\0'; i++)
    {
      str[i] = toupper(str[i]);
    }
}

static uv_fs_t read_req;

static void audio_file_read_done(uv_fs_t *req)
{
  uv_fs_t close_req;
  audio_info_t *ctx = uv_req_get_data((uv_req_t *)req);
  if (ctx->stop_flag)
    req->result = -1;

  if (req->result < 0)
    {
      LOG_E("[%s][%d] Player Read error: %s\n", __func__, __LINE__,
            uv_strerror(req->result));
      if (ctx->data)
        {
          free(ctx->data);
          ctx->data = NULL;
        }
      return;
    }
  else if (req->result == 0)
    {
      LOG_I("[%s][%d] Player read to end of file\n", __func__, __LINE__);
      uv_fs_close(g_mediatest_loop, &close_req, ctx->fd, NULL);
      if (ctx->data)
        {
          free(ctx->data);
          ctx->data = NULL;
        }
    }
  else
    {
      audio_set_control(MULTI_SESSION_CONTROL_CMD_WRITE_START, ctx);
    }
}

static void audio_file_read(audio_info_t *ctx)
{
  uv_buf_t iov;
  iov = uv_buf_init(ctx->data, BUFFER_SIZE);
  uv_req_set_data((uv_req_t *)&read_req, ctx);
  int ret = uv_fs_read(g_mediatest_loop, &read_req, ctx->fd, &iov, 1, -1,
                       audio_file_read_done);
  if (ret < 0)
    {
      LOG_W("uv fs read ret < 0\n");
    }
}

static void audio_files_play_cb(multi_session_event_t *e)
{
  audio_info_t *ctx = (audio_info_t *)e->user_data;

  LOG_D("audio command 0x%x handle: %p stream %s\n", e->command,
        e->handle, ctx->stream);

  char *upper_stream = strdup(ctx->stream);
  switch_to_upper(upper_stream);
  if (((e->command == MULTI_SESSION_EVENT_CMD_PREPARE) ||
       (e->command == MULTI_SESSION_EVENT_CMD_START)) &&
      (e->status == -1))
    {
      ctx->duration = 1;
      ctx->currenttime = 0;
      ctx->playstate = AUDIO_PLAYSTATE_STOP;
      audio_set_control(MULTI_SESSION_CONTROL_CMD_STOP, ctx);
      free(upper_stream);
      return;
    }
  if ((e->status < 0) && (e->command == MULTI_SESSION_EVENT_CMD_PLAY))
    {
      LOG_E("TEST_%s_EVENT_CMD_OPEN_FAILED\n", upper_stream);
      free(upper_stream);
      return;
    }
  switch (e->command)
    {
    case MULTI_SESSION_EVENT_CMD_PLAY:
      {
        LOG_I("TEST_%s_EVENT_CMD_PLAY\n", upper_stream);
        LOG_I("TEST_%s_EVENT_CMD_PLAY\n", upper_stream);
        ctx->handle = e->handle;
        ctx->used = true;
        int index = get_index_by_stream(ctx->stream);
        for (int i = 0; i < 4; i++)
          {
            if (index == share_stream[i])
              share_app_index = index;
          }
      }
      break;
    case MULTI_SESSION_EVENT_CMD_OPEN:
      {
        LOG_I("TEST_%s_EVENT_CMD_OPEN\n", upper_stream);
        LOG_I("TEST_%s_EVENT_CMD_OPEN\n", upper_stream);
        ctx->handle = e->handle;
        ctx->used = true;
        int index = get_index_by_stream(ctx->stream);
        for (int i = 0; i < 4; i++)
          {
            if (index == share_stream[i])
              share_app_index = index;
          }
      }
      break;
    case MULTI_SESSION_EVENT_CMD_PREPARE:
      LOG_I("TEST_%s_EVENT_CMD_PREPARE\n", upper_stream);
      LOG_I("TEST_%s_EVENT_CMD_PREPARE\n", upper_stream);
      break;
    case MULTI_SESSION_EVENT_CMD_START:
      LOG_I("TEST_%s_EVENT_CMD_START\n", upper_stream);
      LOG_I("TEST_%s_EVENT_CMD_START\n", upper_stream);
      audio_set_control(MULTI_SESSION_CONTROL_CMD_GET_ALL, ctx);
      ctx->playstate = AUDIO_PLAYSTATE_PLAY;
      break;
    case MULTI_SESSION_EVENT_CMD_PAUSE:
      LOG_I("TEST_%s_EVENT_CMD_PAUSE\n", upper_stream);
      LOG_I("TEST_%s_EVENT_CMD_PAUSE\n", upper_stream);
      ctx->playstate = AUDIO_PLAYSTATE_PAUSE;
      break;
    case MULTI_SESSION_EVENT_CMD_STOP:
      {
        LOG_I("TEST_%s_EVENT_CMD_STOP\n", upper_stream);
        LOG_I("TEST_%s_EVENT_CMD_STOP\n", upper_stream);
        ctx->duration = 0;
        ctx->currenttime = 0;
        ctx->percent = 0;
        ctx->playstate = AUDIO_PLAYSTATE_STOP;
      }
      break;
    case MULTI_SESSION_EVENT_CMD_GET_POSITION:
      LOG_I("TEST_%s_EVENT_CMD_GET_POSITION\n", upper_stream);
      LOG_I("TEST_%s_EVENT_CMD_GET_POSITION\n", upper_stream);
      ctx->currenttime = e->get_position.msec;
      ctx->percent = ((float)e->get_position.msec / ctx->duration) * 100;
      break;
    case MULTI_SESSION_EVENT_CMD_GET_DURATION:
      LOG_I("TEST_%s_EVENT_CMD_GET_DURATION\n", upper_stream);
      LOG_I("TEST_%s_EVENT_CMD_GET_DURATION\n", upper_stream);
      ctx->duration = e->get_duration.msec;
      break;
    case MULTI_SESSION_EVENT_CMD_GET_PLAY_STATE:
      LOG_I("TEST_%s_EVENT_CMD_GET_PLAY_START\n", upper_stream);
      LOG_I("TEST_%s_EVENT_CMD_GET_PLAY_START\n", upper_stream);

      if (e->get_play_state.playing == 0)
        {
          LOG_I("TEST_%s_STATE_IS_NOT_PLAYING\n", upper_stream);
          LOG_I("TEST_%s_STATE_IS_NOT_PLAYING\n", upper_stream);
        }
      else if (e->get_play_state.playing == 1)
        {
          LOG_I("TEST_%s_STATE_IS_PLAYING\n", upper_stream);
          LOG_I("TEST_%s_STATE_IS_PLAYING\n", upper_stream);
        }
      else
        {
          LOG_I("TEST_%s_STATE_GET_ERROR\n", upper_stream);
          LOG_I("TEST_%s_STATE_GET_ERROR\n", upper_stream);
        }
      break;
    case MULTI_SESSION_EVENT_CMD_GET_ALL:
      LOG_I("TEST_%s_EVENT_CMD_GET_ALL\n", upper_stream);
      LOG_I("TEST_%s_EVENT_CMD_GET_ALL\n", upper_stream);
      ctx->duration = e->get_all.msec;
      ctx->currenttime = e->get_all.pos;
      ctx->volume = e->get_all.volume;
      ctx->percent = ((float)e->get_all.pos / ctx->duration) * 100;
      break;

    case MULTI_SESSION_EVENT_CMD_COMPLETE:
      LOG_I("TEST_%s_EVENT_CMD_COMPLETE\n", upper_stream);
      LOG_I("TEST_%s_EVENT_CMD_COMPLETE\n", upper_stream);
      ctx->playstate = AUDIO_PLAYSTATE_COMPLETE;
      if (ctx->test_song_entry)
        {
          test_song_entry_t *song_entry = ctx->test_song_entry;
          test_get_next_song_entry(song_entry, &ctx->test_song_entry);
          strncpy(ctx->src, ctx->test_song_entry->song_url,
                  strlen(ctx->test_song_entry->song_url) + 1);
          audio_set_control(MULTI_SESSION_CONTROL_CMD_PLAY_NEXT, ctx);
        }

      break;
    case MULTI_SESSION_EVENT_CMD_SEEK:
      LOG_I("TEST_%s_EVENT_CMD_SEEK\n", upper_stream);
      LOG_I("TEST_%s_EVENT_CMD_SEEK\n", upper_stream);
      break;
    case MULTI_SESSION_EVENT_CMD_PLAYER_WRITE:
      LOG_D("TEST_%s_EVENT_CMD_PLAYER_WRITE\n", upper_stream);
      LOG_D("TEST_%s_EVENT_CMD_PLAYER_WRITE\n", upper_stream);
      audio_file_read(ctx);
      break;
    default:
      LOG_E("mediasuit_player ********** unknown command\n");
      break;
    }
  free(upper_stream);
}

static void audio_get_volum_notify(struct multi_session_event *event)
{
  audio_info_t *ctx = (audio_info_t *)event->user_data;
  if (event->command == MULTI_SESSION_EVENT_CMD_GET_STREAM_VOLUME)
    {
      ctx->volume = event->get_stream_volume.volume;
    }
  return;
}

static bool audio_set_control(uint8_t command, audio_info_t *ctx)
{
  multi_session_control_t *control = (multi_session_control_t *)calloc(
      1, sizeof(multi_session_control_t));
  control->user_data = ctx;
  control->handle = ctx->handle;
  control->command = command;
  if (command == MULTI_SESSION_CONTROL_CMD_PLAY)
    {
      control->play.loop = MULTI_SESSION_CONTROL_LOOP_DISABLE;
      control->play.stream = ctx->stream;
      control->play.focus = ctx->focus;
      control->play.cb = audio_files_play_cb;
      control->play.lock_focus = false;
      if (ctx->options[0] == 0)
        {
          memset(control->play.options, 0,
                 sizeof(control->play.options));
        }
      else
        {
          strlcpy(control->play.options, ctx->options,
                  sizeof(control->play.options));
        }
      if (ctx->src[0] == 0)
        {
          memset(control->play.url, 0, sizeof(control->play.url));
        }
      else
        {
          strlcpy(control->play.url, ctx->src,
                  sizeof(control->play.url));
        }
    }
  else if (command == MULTI_SESSION_CONTROL_CMD_PLAY_NEXT)
    {
      if (ctx->options[0] == 0)
        {
          memset(control->play_next.options, 0,
                 sizeof(control->play_next.options));
        }
      else
        {
          strlcpy(control->play_next.options, ctx->options,
                  sizeof(control->play_next.options));
        }
      if (ctx->src[0] == 0)
        {
          memset(control->play_next.url, 0,
                 sizeof(control->play_next.url));
        }
      else
        {
          strlcpy(control->play_next.url, ctx->src,
                  sizeof(control->play_next.url));
        }
    }
  else if (command == MULTI_SESSION_CONTROL_CMD_SET_SEEK)
    {
      control->set_seek.msec = ctx->currenttime;
    }
  else if (command == MULTI_SESSION_CONTROL_CMD_SET_LOOP)
    {
      control->set_loop.loop = ctx->loop;
    }
  else if (command == MULTI_SESSION_CONTROL_CMD_WRITE_START)
    {
      control->write_buffer.data = ctx->data;
      control->write_buffer.len = BUFFER_SIZE;
    }
  else if (command == MULTI_SESSION_CONTROL_CMD_SET_STREAM_VOLUME)
    {
      control->set_stream_volume.stream = ctx->stream;
      control->set_stream_volume.volume = ctx->volume;
    }
  else if (command == MULTI_SESSION_CONTROL_CMD_INC_STREAM_VOLUME ||
           command == MULTI_SESSION_CONTROL_CMD_DEC_STREAM_VOLUME)
    {
      control->set_stream_volume.stream = ctx->stream;
    }
  else if (command == MULTI_SESSION_CONTROL_CMD_GET_STREAM_VOLUME)
    {
      control->get_stream_volume.stream = ctx->stream;
      control->get_stream_volume.cb = audio_get_volum_notify;
    }
  else if (command == MULTI_SESSION_CONTROL_CMD_SET_GRAPH_VOLUME)
    {
      control->graph_volume.vol = ctx->g_vol;
    }
  else if (command == MULTI_SESSION_CONTROL_CMD_OPEN)
    {
      control->open.stream = ctx->stream;
      control->open.focus = ctx->focus;
      control->open.cb = audio_files_play_cb;
    }
  else if (command == MULTI_SESSION_CONTROL_CMD_PREPARE)
    {
      if (ctx->options[0] == 0)
        {
          memset(control->prepare.options, 0,
                 sizeof(control->prepare.options));
        }
      else
        {
          strlcpy(control->prepare.options, ctx->options,
                  sizeof(control->prepare.options));
        }
      if (ctx->src[0] == 0)
        {
          memset(control->prepare.url, 0, sizeof(control->prepare.url));
        }
      else
        {
          strlcpy(control->prepare.url, ctx->src,
                  sizeof(control->prepare.url));
        }
    }
  else if (command == MULTI_SESSION_CONTROL_CMD_CLOSE)
    {
      ctx->used = false;
      int index = get_index_by_stream(ctx->stream);
      for (int i = 0; i < 4; i++)
        {
          if (index == share_stream[i])
            share_app_index = -1;
        }
    }

  uv_async_queue_send(&g_mediatest_queue, control);

  if (command == MULTI_SESSION_CONTROL_CMD_CLOSE ||
      command == MULTI_SESSION_CONTROL_CMD_STOP ||
      command == MULTI_SESSION_CONTROL_CMD_RESET)
    {
      ctx->stop_flag = true;
      usleep(1000);
      if (ctx->fd > 0)
        {
          close(ctx->fd);
          ctx->fd = -1;
        }
    }
  return true;
}

static void __audio_media_play(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;
  audio_init(&multi_apps[index], index);
  strncpy(multi_apps[index].src, attr->url, strlen(attr->url) + 1);
  if (attr->focus)
    strncpy(multi_apps[index].focus, attr->focus,
            strlen(attr->focus) + 1);

  if (attr->options)
    strncpy(multi_apps[index].options, attr->options,
            strlen(attr->options) + 1);

  if (multi_apps[index].test_song_entry)
    strncpy(multi_apps[index].src,
            multi_apps[index].test_song_entry->song_url,
            strlen(multi_apps[index].test_song_entry->song_url) + 1);

  if (attr->focus)
    strncpy(multi_apps[index].focus, attr->focus,
            strlen(attr->focus) + 1);

  int count = sizeof(share_stream) / sizeof(share_stream[0]);

  int in = 0;
  for (int i = 0; i < count; i++)
    {
      if (index == share_stream[i])
        in = 1;
    }

  if ((in == 1) && (share_app_index >= 0) &&
      ((index <= share_app_index) ||
       (multi_apps[share_app_index].playstate ==
        AUDIO_PLAYSTATE_COMPLETE)))
    {
      audio_set_control(MULTI_SESSION_CONTROL_CMD_CLOSE,
                        &multi_apps[share_app_index]);
      usleep(500 * 1000);
    }
  if (attr->mode == 0)
    {
      multi_apps[index].fd =
          open(multi_apps[index].src, O_RDONLY | O_CLOEXEC, 0666);
      if (multi_apps[index].fd < 0)
        {
          LOG_E("open failed\n");
          return;
        }
      memset(multi_apps[index].src, 0, sizeof(multi_apps[index].src));
      if (multi_apps[index].data)
        {
          LOG_W("already malloc\n");
        }
      else
        {
          multi_apps[index].data = malloc(BUFFER_SIZE);
        }
      multi_apps[index].mode = MODE_BUFFER;
    }

  if (multi_apps[index].used)
    {
      LOG_W("the stream %s is be used\n", attr->stream);
      audio_set_control(MULTI_SESSION_CONTROL_CMD_PLAY_NEXT,
                        &multi_apps[index]);
      return;
    };

  if (attr->file)
    mediatest_load_play_list(index, attr->file);

  audio_set_control(MULTI_SESSION_CONTROL_CMD_PLAY, &multi_apps[index]);
}

static void __audio_media_pause(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;
  if (multi_apps[index].used)
    {
      if (multi_apps[index].playstate == AUDIO_PLAYSTATE_PLAY)
        {
          audio_set_control(MULTI_SESSION_CONTROL_CMD_PAUSE,
                            &multi_apps[index]);
        }
    }
}

static void __audio_media_resume(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;
  if (multi_apps[index].used)
    {
      if (multi_apps[index].playstate == AUDIO_PLAYSTATE_PAUSE)
        {
          audio_set_control(MULTI_SESSION_CONTROL_CMD_START,
                            &multi_apps[index]);
        }
      else if (multi_apps[index].playstate != AUDIO_PLAYSTATE_PLAY)
        {
          audio_set_control(MULTI_SESSION_CONTROL_CMD_PLAY_NEXT,
                            &multi_apps[index]);
        }
    }
}

static void __audio_media_close(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;
  if (multi_apps[index].used)
    {
      multi_apps[index].duration = 0;
      multi_apps[index].currenttime = 0;
      multi_apps[index].playstate = AUDIO_PLAYSTATE_STOP;
      audio_set_control(MULTI_SESSION_CONTROL_CMD_CLOSE,
                        &multi_apps[index]);
      if (multi_apps[index].test_song_entry)
        {
          test_delete_all_song_entry_of_play_list(mediatest_play_list);
          multi_apps[index].test_song_entry = NULL;
          mediatest_play_list = NULL;
        }
    }
}

static void __audio_media_prev(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;
  if (multi_apps[index].used)
    {
      test_song_entry_t *song_entry = multi_apps[index].test_song_entry;
      test_get_prev_song_entry(song_entry,
                               &multi_apps[index].test_song_entry);
      strncpy(multi_apps[index].src,
              multi_apps[index].test_song_entry->song_url,
              strlen(multi_apps[index].test_song_entry->song_url) + 1);
      audio_set_control(MULTI_SESSION_CONTROL_CMD_PLAY_NEXT,
                        &multi_apps[index]);
    }
}

static void __audio_media_next(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;
  if (multi_apps[index].used)
    {
      test_song_entry_t *song_entry = multi_apps[index].test_song_entry;
      test_get_next_song_entry(song_entry,
                               &multi_apps[index].test_song_entry);
      strncpy(multi_apps[index].src,
              multi_apps[index].test_song_entry->song_url,
              strlen(multi_apps[index].test_song_entry->song_url) + 1);
      audio_set_control(MULTI_SESSION_CONTROL_CMD_PLAY_NEXT,
                        &multi_apps[index]);
    }
}

static void __audio_media_seek(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;
  if (multi_apps[index].used)
    {
      multi_apps[index].currenttime = attr->msec;
      audio_set_control(MULTI_SESSION_CONTROL_CMD_SET_SEEK,
                        &multi_apps[index]);
    }
}

static void __audio_media_seek_percent(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;
  if (multi_apps[index].used)
    {
      multi_apps[index].currenttime =
          (unsigned int)((multi_apps[index].duration * attr->msec) /
                         100);
      audio_set_control(MULTI_SESSION_CONTROL_CMD_SET_SEEK,
                        &multi_apps[index]);
    }
}

static void __audio_media_set_loop(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;
  if (multi_apps[index].used)
    {
      multi_apps[index].loop = attr->loop;
      audio_set_control(MULTI_SESSION_CONTROL_CMD_SET_LOOP,
                        &multi_apps[index]);
    }
}

static void __audio_media_duration(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;
  if (multi_apps[index].used)
    {
      audio_set_control(MULTI_SESSION_CONTROL_CMD_GET_DURATION,
                        &multi_apps[index]);
    }
}

static void __audio_media_position(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;
  if (multi_apps[index].used)
    {
      audio_set_control(MULTI_SESSION_CONTROL_CMD_GET_POSITION,
                        &multi_apps[index]);
    }
}

static void __audio_media_adjust_volume(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;
  multi_apps[index].volume = attr->volume;

  if (!multi_apps[index].used)
    multi_apps[index].handle = NULL;

  audio_set_control(MULTI_SESSION_CONTROL_CMD_SET_STREAM_VOLUME,
                    &multi_apps[index]);
}

static void __audio_media_increase_volume(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;

  if (!multi_apps[index].used)
    multi_apps[index].handle = NULL;

  audio_set_control(MULTI_SESSION_CONTROL_CMD_INC_STREAM_VOLUME,
                    &multi_apps[index]);
}

static void __audio_media_decrease_volume(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;

  if (!multi_apps[index].used)
    multi_apps[index].handle = NULL;

  audio_set_control(MULTI_SESSION_CONTROL_CMD_DEC_STREAM_VOLUME,
                    &multi_apps[index]);
}

static void __audio_media_get_volume(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;

  if (!multi_apps[index].used)
    multi_apps[index].handle = NULL;

  audio_set_control(MULTI_SESSION_CONTROL_CMD_GET_STREAM_VOLUME,
                    &multi_apps[index]);
}

static void __audio_media_get_all(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;

  if (multi_apps[index].playstate != AUDIO_PLAYSTATE_PLAY &&
      multi_apps[index].playstate != AUDIO_PLAYSTATE_PAUSE)
    {
      return;
    }

  audio_set_control(MULTI_SESSION_CONTROL_CMD_GET_ALL,
                    &multi_apps[index]);
}

static void __audio_media_get_playstate(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;

  if (multi_apps[index].used)
    {
      audio_set_control(MULTI_SESSION_CONTROL_CMD_GET_PLAY_STATE,
                        &multi_apps[index]);
    }
}

static void __audio_media_getall_opened(void)
{
  for (int i = 0; i < MULTI_APPS_COUNT_MAX; i++)
    {
      if (multi_apps[i].used)
        LOG_I("STREAM OPENED IS %s, PLAYSTART IS %d\n",
              multi_apps[i].stream, multi_apps[i].playstate);

      if (record_app.used)
        LOG_I("STREAM OPENED IS %s, PLAYSTART IS %d\n",
              record_app.stream, record_app.playstate);
    }
}

static void __audio_media_exit(void)
{
  for (int i = 0; i < MULTI_APPS_COUNT_MAX; i++)
    {
      if (multi_apps[i].used)
        {
          audio_set_control(MULTI_SESSION_CONTROL_CMD_CLOSE,
                            &multi_apps[i]);
          if (multi_apps[i].test_song_entry)
            {
              test_delete_all_song_entry_of_play_list(
                  mediatest_play_list);
              multi_apps[i].test_song_entry = NULL;
              mediatest_play_list = NULL;
            }
        }
      if (record_app.used)
        record_set_control(MULTI_SESSION_CONTROL_CMD_RECORDER_CLOSE,
                           &record_app);
    }
}

static void __audio_record_open(audio_attr_t *attr)
{
  if (record_app.used)
    return;
  record_app.used = true;
  strlcpy(record_app.stream, attr->stream, sizeof(record_app.stream));
  record_set_control(MULTI_SESSION_CONTROL_CMD_RECORDER_OPEN,
                     &record_app);
}

static void __audio_record_prepare(audio_attr_t *attr)
{
  if (record_app.used)
    {
      audio_init(&record_app, MULTI_APPS_COUNT_MAX - 1);
      strncpy(record_app.src, attr->url, strlen(attr->url) + 1);
      if (attr->mode == 0)
        {
          // unlink(attr->url);
          // record_app.fd = open(
          //     attr->url, O_CREAT | O_RDWR | O_CLOEXEC | O_TRUNC,
          //     0666);
          // if (record_app.fd < 0)
          //   {
          //     LOG_E("open %s failed\n", attr->url);
          //     return;
          //   }
          memset(record_app.src, 0, sizeof(record_app.src));
          record_app.mode = MODE_BUFFER;
        }

      if (attr->focus)
        strncpy(record_app.focus, attr->focus, strlen(attr->focus) + 1);

      if (attr->options)
        strncpy(record_app.options, attr->options,
                strlen(attr->options) + 1);

      record_set_control(MULTI_SESSION_CONTROL_CMD_RECORDER_PREPARE,
                         &record_app);
    }
}

static void __audio_record_start(audio_attr_t *attr)
{
  if (record_app.used)
    {
      record_set_control(MULTI_SESSION_CONTROL_CMD_RECORDER_START,
                         &record_app);
    }
}

static void __audio_record_stop(audio_attr_t *attr)
{
  if (record_app.used)
    {
      record_set_control(MULTI_SESSION_CONTROL_CMD_RECORDER_STOP,
                         &record_app);
    }
}

static void __audio_record_reset(audio_attr_t *attr)
{
  if (record_app.used)
    {
      record_set_control(MULTI_SESSION_CONTROL_CMD_RECORDER_RESET,
                         &record_app);
    }
}

static void __audio_media_open(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;
  if (multi_apps[index].used)
    return;
  multi_apps[index].used = true;
  audio_set_control(MULTI_SESSION_CONTROL_CMD_OPEN, &multi_apps[index]);
}

static void __audio_media_prepare(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;
  if (multi_apps[index].used)
    {
      audio_init(&multi_apps[index], index);
      strncpy(multi_apps[index].src, attr->url, strlen(attr->url) + 1);
      if (attr->focus)
        strncpy(multi_apps[index].focus, attr->focus,
                strlen(attr->focus) + 1);

      if (attr->options)
        strncpy(multi_apps[index].options, attr->options,
                strlen(attr->options) + 1);

      if (multi_apps[index].test_song_entry)
        strncpy(multi_apps[index].src,
                multi_apps[index].test_song_entry->song_url,
                strlen(multi_apps[index].test_song_entry->song_url) + 1);

      if (attr->focus)
        strncpy(multi_apps[index].focus, attr->focus,
                strlen(attr->focus) + 1);

      if (attr->mode == 0)
        {
          multi_apps[index].fd =
              open(multi_apps[index].src, O_RDONLY | O_CLOEXEC, 0666);
          if (multi_apps[index].fd < 0)
            {
              LOG_E("open failed\n");
              return;
            }
          memset(multi_apps[index].src, 0,
                 sizeof(multi_apps[index].src));
          if (multi_apps[index].data)
            {
              LOG_W("already malloc\n");
            }
          else
            {
              multi_apps[index].data = malloc(BUFFER_SIZE);
            }
          multi_apps[index].mode = MODE_BUFFER;
        }

      audio_set_control(MULTI_SESSION_CONTROL_CMD_PREPARE,
                        &multi_apps[index]);
    }
}

static void __audio_media_start(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;
  if (multi_apps[index].used)
    {
      int count = sizeof(share_stream) / sizeof(share_stream[0]);

      int in = 0;
      for (int i = 0; i < count; i++)
        {
          if (index == share_stream[i])
            in = 1;
        }

      if ((in == 1) && ((index <= share_app_index) ||
                        (multi_apps[share_app_index].playstate ==
                         AUDIO_PLAYSTATE_COMPLETE)))
        {
          audio_set_control(MULTI_SESSION_CONTROL_CMD_CLOSE,
                            &multi_apps[share_app_index]);
          usleep(500 * 1000);
        }

      if (attr->file)
        mediatest_load_play_list(index, attr->file);

      audio_set_control(MULTI_SESSION_CONTROL_CMD_START,
                        &multi_apps[index]);
    }
}

static void __audio_media_stop(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;
  if (multi_apps[index].used)
    {
      audio_set_control(MULTI_SESSION_CONTROL_CMD_STOP,
                        &multi_apps[index]);
    }
}

static void __audio_media_reset(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;
  if (multi_apps[index].used)
    {
      audio_set_control(MULTI_SESSION_CONTROL_CMD_RESET,
                        &multi_apps[index]);
    }
}

static void __audio_media_sendmsg(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;

  audio_set_control(MULTI_SESSION_CONTROL_CMD_SENDMSG,
                    &multi_apps[index]);
}

static void __audio_media_set_graphvolume(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;
  if (multi_apps[index].used)
    {
      if ((attr->g_vol >= 0) && (attr->g_vol <= 1))
        {
          multi_apps[index].g_vol = attr->g_vol;
          audio_set_control(MULTI_SESSION_CONTROL_CMD_SET_GRAPH_VOLUME,
                            &multi_apps[index]);
        }
    }
}

static void __audio_media_get_graphvolume(audio_attr_t *attr)
{
  int index = get_index_by_stream(attr->stream);
  if (index == 0xFFFFFFFF)
    return;
  if (multi_apps[index].used)
    {
      audio_set_control(MULTI_SESSION_CONTROL_CMD_GET_GRAPH_VOLUME,
                        &multi_apps[index]);
    }
}

static int mediatest_load_local_all_songs(char *path,
                                          test_play_list_t *play_list_)
{
  if (!path || !play_list_)
    {
      return -1;
    }
  FILE *fp = fopen(path, "r");
  if (fp == NULL)
    {
      LOG_E("FILE LOAD FAILED\n");
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

static void mediatest_load_play_list(int index, char *file)
{
  LOG_D(
      "**********************************************"
      "***********\n");
  LOG_D("parse play list\n");

  mediatest_play_list = test_play_list_init();
  if (!mediatest_play_list)
    {
      mediatest_play_list = test_play_list_init();
      if (!mediatest_play_list)
        {
          syslog(LOG_ERR, "ERROE test_play_list_init failed\n");
          return;
        }
    }

  test_song_entry_t *test_song_entry __attribute__((unused)) = NULL;

  mediatest_load_local_all_songs(file, mediatest_play_list);
  test_song_entry = container_of(mediatest_play_list->song_head.next,
                                 test_song_entry_t, song_list);

  multi_apps[index].test_song_entry = test_song_entry;

  test_print_play_list();
  return;
}

/* record */

static void recorder_notify_cb(struct multi_session_event *event)
{
  LOG_D("recoder_notify_cb,handle=%p,evt=0x%02x,ret=%d\n", event->handle,
        event->command, event->status);
  audio_info_t *ctx = event->user_data;

  if (((event->command == MULTI_SESSION_EVENT_CMD_PREPARE) ||
       (event->command == MULTI_SESSION_EVENT_CMD_START)) &&
      (event->status == -1))
    {
      ctx->playstate = AUDIO_PLAYSTATE_STOP;
      record_set_control(MULTI_SESSION_CONTROL_CMD_RECORDER_STOP, ctx);
      return;
    }

  switch (event->command)
    {
    case MULTI_SESSION_EVENT_CMD_RECORDER_BEGIN:
      LOG_I("TEST_RECORD_EVENT_CMD_BEGIN\n");
      LOG_I("TEST_RECORD_EVENT_CMD_BEGIN\n");
      ctx->handle = event->handle;
      ctx->used = true;
      ctx->playstate = AUDIO_PLAYSTATE_PLAY;
      break;
    case MULTI_SESSION_EVENT_CMD_RECORDER_OPEN:
      LOG_I("TEST_RECORD_EVENT_CMD_OPEN\n");
      LOG_I("TEST_RECORD_EVENT_CMD_OPEN\n");
      ctx->handle = event->handle;
      ctx->used = true;
      break;
    case MULTI_SESSION_EVENT_CMD_PREPARE:
      LOG_I("TEST_RECORD_EVENT_CMD_PREPARE\n");
      LOG_I("TEST_RECORD_EVENT_CMD_PREPARE\n");
      if (ctx->mode == MODE_BUFFER)
        record_set_control(MULTI_SESSION_CONTROL_CMD_RECORDER_READ, ctx);
      break;
    case MULTI_SESSION_EVENT_CMD_START:
      LOG_I("TEST_RECORD_EVENT_CMD_START\n");
      LOG_I("TEST_RECORD_EVENT_CMD_START\n");
      ctx->playstate = AUDIO_PLAYSTATE_PLAY;
      break;
    case MULTI_SESSION_EVENT_CMD_PAUSE:
      LOG_I("TEST_RECORD_EVENT_CMD_PAUSE\n");
      LOG_I("TEST_RECORD_EVENT_CMD_PAUSE\n");
      ctx->playstate = AUDIO_PLAYSTATE_PAUSE;
      break;
    case MULTI_SESSION_EVENT_CMD_COMPLETE:
      LOG_I("TEST_RECORD_EVENT_CMD_COMPLETE\n");
      LOG_I("TEST_RECORD_EVENT_CMD_COMPLETE\n");
      ctx->playstate = AUDIO_PLAYSTATE_COMPLETE;
      break;
    case MULTI_SESSION_EVENT_CMD_STOP:
      LOG_I("TEST_RECORD_EVENT_CMD_STOP\n");
      LOG_I("TEST_RECORD_EVENT_CMD_STOP\n");
      ctx->playstate = AUDIO_PLAYSTATE_STOP;
      break;
    case MULTI_SESSION_CONTROL_CMD_RECORDER_CLOSE:
      LOG_I("TEST_RECORD_EVENT_CMD_CLOSE\n");
      LOG_I("TEST_RECORD_EVENT_CMD_CLOSE\n");
      break;
    case MULTI_SESSION_EVENT_CMD_RECORDER_READ:
      LOG_D("TEST_RECORD_EVENT_CMD_RECORDER_READ\n");
      LOG_D("TEST_RECORD_EVENT_CMD_RECORDER_READ\n");
      break;
    default:
      LOG_I("unknown command: 0x%02x\n", event->command);
      break;
    }
}

static bool record_set_control(uint8_t command, audio_info_t *ctx)
{
  multi_session_control_t *control = (multi_session_control_t *)calloc(
      1, sizeof(multi_session_control_t));
  control->user_data = ctx;
  control->handle = ctx->handle;
  control->command = command;
  if (command == MULTI_SESSION_CONTROL_CMD_RECORDER_BEGIN)
    {
      control->recorder_begin.stream = ctx->stream;
      control->recorder_begin.focus = ctx->focus;
      control->recorder_begin.cb = recorder_notify_cb;
      control->recorder_begin.lock_focus = false;
      if (ctx->options[0] == 0)
        {
          memset(control->recorder_begin.options, 0,
                 sizeof(control->recorder_begin.options));
        }
      else
        {
          strlcpy(control->recorder_begin.options, ctx->options,
                  sizeof(control->recorder_begin.options));
        }
      if (ctx->src[0] == 0)
        {
          memset(control->recorder_begin.url, 0,
                 sizeof(control->recorder_begin.url));
        }
      else
        {
          strlcpy(control->recorder_begin.url, ctx->src,
                  sizeof(control->recorder_begin.url));
        }
    }
  else if (command == MULTI_SESSION_CONTROL_CMD_RECORDER_PREPARE)
    {
      strlcpy(control->recorder_prepare.url, ctx->src,
              sizeof(control->recorder_prepare.url));
      strlcpy(control->recorder_prepare.options, ctx->options,
              sizeof(control->recorder_prepare.options));
      strlcpy(control->recorder_begin.url, ctx->src,
              sizeof(control->recorder_begin.url));
      strlcpy(control->recorder_begin.options, ctx->options,
              sizeof(control->recorder_begin.options));
    }
  else if (command == MULTI_SESSION_CONTROL_CMD_RECORDER_OPEN)
    {
      control->recorder_open.stream = ctx->stream;
      control->recorder_open.focus = ctx->focus;
      control->recorder_open.cb = recorder_notify_cb;
    }
  else if (command == MULTI_SESSION_CONTROL_CMD_RECORDER_CLOSE)
    {
      ctx->used = false;
    }

  uv_async_queue_send(&g_mediatest_queue, control);

  if (command == MULTI_SESSION_CONTROL_CMD_RECORDER_STOP ||
      command == MULTI_SESSION_CONTROL_CMD_RECORDER_RESET ||
      command == MULTI_SESSION_CONTROL_CMD_RECORDER_CLOSE)
    {
      usleep(1000);
      if (ctx->fd > 0)
        {
          close(ctx->fd);
          ctx->fd = -1;
        }
    }

  return true;
}

static void audio_init(audio_info_t *ctx, int index)
{
  memset(ctx->options, 0, sizeof(ctx->options));
  ctx->muted = false;
  ctx->loop = 0;
  ctx->percent = 0;
  ctx->currenttime = 0;
  ctx->duration = 0;
  ctx->volume = 0;
  ctx->g_vol = 0;
  ctx->mode = 1;
  ctx->stop_flag = 0;
  ctx->mutedvolume = 0;
  memset(ctx->src, 0, sizeof(ctx->src));
  strncpy(ctx->focus, stream_lists[index],
          strlen(stream_lists[index]) + 1);
}

static void __audio_record_play(audio_attr_t *attr)
{
  audio_init(&record_app, MULTI_APPS_COUNT_MAX - 1);
  strncpy(record_app.src, attr->url, strlen(attr->url) + 1);
  strncpy(record_app.stream, attr->stream, strlen(attr->stream) + 1);
  if (attr->focus)
    strncpy(record_app.focus, attr->focus, strlen(attr->focus) + 1);

  if (attr->options)
    strncpy(record_app.options, attr->options,
            strlen(attr->options) + 1);

  if (attr->mode == 0)
    {
      memset(record_app.src, 0, sizeof(record_app.src));
      record_app.mode = MODE_BUFFER;
    }

  if (record_app.used)
    {
      LOG_W("the stream %s is be used\n", MEDIA_STREAM_RECORD);

      record_set_control(MULTI_SESSION_CONTROL_CMD_RECORDER_STOP,
                         &record_app);
      record_set_control(MULTI_SESSION_CONTROL_CMD_RECORDER_PREPARE,
                         &record_app);
      record_set_control(MULTI_SESSION_CONTROL_CMD_RECORDER_START,
                         &record_app);
      return;
    }
  record_set_control(MULTI_SESSION_CONTROL_CMD_RECORDER_BEGIN,
                     &record_app);
}

static void __audio_record_pause(void)
{
  if (record_app.used)
    {
      if (record_app.playstate == AUDIO_PLAYSTATE_PLAY)
        {
          record_set_control(MULTI_SESSION_CONTROL_CMD_RECORDER_PAUSE,
                             &record_app);
        }
    }
}

static void __audio_record_resume(void)
{
  if (record_app.used)
    {
      if (record_app.playstate == AUDIO_PLAYSTATE_PAUSE)
        {
          record_set_control(MULTI_SESSION_CONTROL_CMD_RECORDER_START,
                             &record_app);
        }
      else if (record_app.playstate != AUDIO_PLAYSTATE_PLAY)
        {
          record_set_control(MULTI_SESSION_CONTROL_CMD_RECORDER_STOP,
                             &record_app);
          record_set_control(MULTI_SESSION_CONTROL_CMD_RECORDER_PREPARE,
                             &record_app);
          record_set_control(MULTI_SESSION_CONTROL_CMD_RECORDER_START,
                             &record_app);
        }
    }
}

static void __audio_record_close(void)
{
  if (record_app.used)
    {
      record_app.duration = 0;
      record_app.currenttime = 0;
      record_app.playstate = AUDIO_PLAYSTATE_STOP;
      record_set_control(MULTI_SESSION_CONTROL_CMD_RECORDER_CLOSE,
                         &record_app);
    }
}

static int mediatest_audio_init(void)
{
  int ret;
  for (int i = 0; i < MULTI_APPS_COUNT_MAX; i++)
    {
      strncpy(multi_apps[i].stream, stream_lists[i],
              strlen(stream_lists[i]) + 1);
      strncpy(multi_apps[i].focus, stream_lists[i],
              strlen(stream_lists[i]) + 1);
      multi_apps[i].used = false;
      multi_apps[i].stop_flag = false;
    }
  LOG_I("init");
  return 0;
}

static void mediatest_uvasyncq_close_cb(uv_handle_t *handle)
{
  uv_stop(g_mediatest_loop);
}

static void mediatest_async_queue_cb(uv_async_queue_t *queue_async,
                                     void *data)
{
  if (queue_async == NULL || data == NULL)
    {
      LOG_E("mediatest_player invalid\n");
      return;
    }
  if (!audio_running)
    {
      uv_close((uv_handle_t *)g_mediatest_loop,
               mediatest_uvasyncq_close_cb);
      return;
    }

  multi_session_control_t *control = (multi_session_control_t *)data;
  multi_session_control(control);
  free(data);
}

void *g_mediatest_loop_thread(void *arg)
{
  int ret;
  g_mediatest_loop = malloc(sizeof(uv_loop_t));
  ret = uv_loop_init(g_mediatest_loop);
  if (ret < 0)
    return NULL;
  multi_session_init(g_mediatest_loop);
  ret = uv_async_queue_init(g_mediatest_loop, &g_mediatest_queue,
                            mediatest_async_queue_cb);
  if (ret < 0)
    goto out;

  LOG_I("[%s][%d] running\n", __func__, __LINE__);
  while (1)
    {
      ret = uv_run(g_mediatest_loop, UV_RUN_DEFAULT);
      if (ret == 0)
        break;
    }

out:
  ret = uv_loop_close(g_mediatest_loop);
  free(g_mediatest_loop);

  LOG_I("[%s][%d] exit\n", __func__, __LINE__);
  return NULL;
}

int mediatest_audio_loop(void)
{
  pthread_attr_t attr __attribute__((unused));
  struct sched_param param __attribute__((unused));
  param.sched_priority = 102;

  pthread_attr_init(&attr);

  pthread_attr_setstacksize(&attr, 100 * 1024);

  pthread_attr_setschedparam(&attr, &param);

  pthread_create(&g_mediatest_thread, &attr, g_mediatest_loop_thread,
                 NULL);
  LOG_I("mediatest_player audio app init\n");
  return 0;
}

static int create_mq(char *path)
{
  int mqid = -1;
  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = AUDIO_PLAYER_MQ_MSG_LEN;
  attr.mq_msgsize = sizeof(audio_msg_t);
  attr.mq_curmsgs = 0;
  mqid = mq_open(path, O_RDWR | O_CREAT, NULL, &attr);

  if (mqid == -1)
    {
      LOG_I("mq_open failed width error: %d\n", errno);
    }
  else
    {
      mq_close(mqid);
    }
  return mqid;
}

static int get_mq_curmsgs(char *path)
{
  struct mq_attr mqStat = {0};
  mqd_t mqid = -1;

  mqid = mq_open(path, O_RDWR);
  if (mqid < 0)
    {
      LOG_E("[%s]:Error %d (%s) on mq_open.\n", __func__, errno,
            strerror(errno));
      return -1;
    }
  if (mq_getattr(mqid, &mqStat) == 0)
    {
      LOG_I("get %s current message:%ld\n", path, mqStat.mq_curmsgs);
      mq_close(mqid);
      return mqStat.mq_curmsgs;
    }
  mq_close(mqid);
  return 0;
}

int send_msg_audio_manager(audio_msg_t *msg)
{
  int ret;
  int mqid = -1;

  if (get_mq_curmsgs(AUDIO_MQ_PATH) >= AUDIO_PLAYER_MQ_MSG_LEN - 4)
    {
      LOG_W("[%s]player mq too long, return\n", __func__);
      return 0;
    }
  mqid = mq_open(AUDIO_MQ_PATH, O_RDWR);
  if (mqid < 0)
    {
      LOG_E("Error %d (%s) on mq_open.\n", errno, strerror(errno));
      return -1;
    }

  ret = mq_send(mqid, (const void *)msg, sizeof(*msg), 0);
  mq_close(mqid);
  if (ret < 0)
    {
      LOG_E("Error %d (%s) on mq_send.\n", errno, strerror(errno));
      return -1;
    }
  LOG_W("Send msg to player success.\n");
  return 0;
}

int main(int argc, FAR char *argv[])
{
  audio_msg_t msg;
  int mqid = -1;
  audio_running = 1;
  mediatest_audio_loop();
  mediatest_audio_init();

  LOG_I("mediatest player start successfully ....\n");

  create_mq(AUDIO_MQ_PATH);
  struct videotest_app *player;

  mqid = mq_open(AUDIO_MQ_PATH, O_RDWR);
  if (mqid < 0)
    {
      LOG_E("Error %d (%s) on mq_open.\n", errno, strerror(errno));
    }
  usleep(500 * 1000);

  while (audio_running)
    {
      if (mq_receive(mqid, (void *)&msg, sizeof(audio_msg_t), NULL) ==
          -1)
        {
          LOG_E("msgrcv failed width errno: %d\n", errno);
        }
      LOG_I("******mq received msg!msg.cmd = %d\n", msg.cmd);
      switch (msg.cmd)
        {
        case AUDIO_CTRL_PLAY:
          if (msg.attr.type == 0)
            __audio_media_play(&msg.attr);
          else
            __audio_record_play(&msg.attr);
          break;
        case AUDIO_CTRL_SEEK_CURRENTTIME:
          __audio_media_seek(&msg.attr);
          break;
        case AUDIO_CTRL_LOOP:
          __audio_media_set_loop(&msg.attr);
          break;
        case AUDIO_CTRL_VOLUME:
          __audio_media_adjust_volume(&msg.attr);
          break;
        case AUDIO_CTRL_SEEK_PERCENT:
          __audio_media_seek_percent(&msg.attr);
          break;
        case AUDIO_CTRL_PLAYPREV:
          __audio_media_prev(&msg.attr);
          break;
        case AUDIO_CTRL_PLAYNEXT:
          __audio_media_next(&msg.attr);
          break;
        case AUDIO_CTRL_PAUSE:
          if (msg.attr.type == 0)
            __audio_media_pause(&msg.attr);
          else
            __audio_record_pause();
          break;
        case AUDIO_CTRL_RESUME:
          if (msg.attr.type == 0)
            __audio_media_resume(&msg.attr);
          else
            __audio_record_resume();
          break;
        case AUDIO_CTRL_VOLUMEUP:
          __audio_media_increase_volume(&msg.attr);
          break;
        case AUDIO_CTRL_VOLUMEDOWN:
          __audio_media_decrease_volume(&msg.attr);
          break;
        case AUDIO_CTRL_GET_PLAY_STATE:
          __audio_media_get_playstate(&msg.attr);
          break;
        case AUDIO_CTRL_GET_ALL_STATE:
          __audio_media_get_all(&msg.attr);
          break;
        case AUDIO_CTRL_GET_POSITION:
          __audio_media_position(&msg.attr);
          break;
        case AUDIO_CTRL_GET_DURATION:
          __audio_media_duration(&msg.attr);
          break;
        case AUDIO_CTRL_CLOSE:
          if (msg.attr.type == 0)
            __audio_media_close(&msg.attr);
          else
            __audio_record_close();
          break;
        case AUDIO_CTRL_EXIT:
          __audio_media_exit();
          break;
        case AUDIO_CTRL_GET_ALL_OPENED:
          __audio_media_getall_opened();
          break;
        case AUDIO_CTRL_GET_VOLUME:
          __audio_media_get_volume(&msg.attr);
          break;
        case AUDIO_CTRL_OPEN:
          if (msg.attr.type == 0)
            __audio_media_open(&msg.attr);
          else
            __audio_record_open(&msg.attr);
          break;
        case AUDIO_CTRL_PREPARE:
          if (msg.attr.type == 0)
            __audio_media_prepare(&msg.attr);
          else
            __audio_record_prepare(&msg.attr);
          break;
        case AUDIO_CTRL_START:
          if (msg.attr.type == 0)
            __audio_media_start(&msg.attr);
          else
            __audio_record_start(&msg.attr);
          break;
        case AUDIO_CTRL_STOP:
          if (msg.attr.type == 0)
            __audio_media_stop(&msg.attr);
          else
            __audio_record_stop(&msg.attr);
          break;
        case AUDIO_CTRL_RESET:
          if (msg.attr.type == 0)
            __audio_media_reset(&msg.attr);
          else
            __audio_record_reset(&msg.attr);
          break;
        case AUDIO_CTRL_SENDMSG:
          __audio_media_sendmsg(&msg.attr);
          break;
        case AUDIO_CTRL_SETGRAPHVOLUME:
          __audio_media_set_graphvolume(&msg.attr);
          break;
        case AUDIO_CTRL_GETGRAPHVOLUME:
          __audio_media_get_graphvolume(&msg.attr);
        default:
          break;
        }
    }

  mq_close(mqid);
  pthread_join(g_mediatest_thread, NULL);
  return -1;
}
