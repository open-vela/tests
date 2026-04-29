/****************************************************************************
 * apps/tests/testcases/media_test/media_graph/media_graph_test.c
 *
 * Name: media_graph_test
 * Example description:
 *  1. media_graph_test
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/nuttx.h>
#include "media_graph_test.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <nuttx/config.h>
#include <nuttx/timers/timer.h>
#include <poll.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

static test_play_list_t *mediatest_play_list = NULL;
static pthread_mutex_t mediatest_ctrl_mutex;
static pthread_t uv_thread = -1;

pthread_mutex_t meidatest_playback_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t mediatest_playback_cond = PTHREAD_COND_INITIALIZER;

static int media_test_loop_living = 0;
static int DEF_SLEEP_DURATION_US = 10 * 1000;//10ms

#ifdef CONFIG_AUDIOUTILS_ALSA_LIB
static int alsapause = 0;
static pthread_mutex_t alsapause_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t alsapause_cond = PTHREAD_COND_INITIALIZER;
#endif

typedef struct
{
  uint16_t channels;
  uint32_t sample_rate;
  uint16_t bits_per_sample;
  uint32_t file_size;
} AudioFmtInfo;

typedef struct
{
  char riff_id[4];    // "RIFF"
  uint32_t riff_size;
  char wave_id[4];    // "WAVE"
  char fmt_id[4];     // "fmt "
  uint32_t fmt_size;
  uint16_t audio_format;
  uint16_t channels;
  uint32_t sample_rate;
  uint32_t byte_rate;
  uint16_t block_align;
  uint16_t bits_per_sample;
  char data_id[4];
  uint32_t data_size;
} WavHeader;

static bool get_ainfo_from_wav(struct mediatest_data *media, AudioFmtInfo *info)
{
  bool ret = false;
  long file_size = 0;
  FILE *file = NULL;
  WavHeader header;

  if (!media || !media->url || !info)
  {
    syslog(LOG_INFO, "%s(%d) param invalid, media: %p, url: %s, info: %p", __func__, __LINE__, media, (media ? media->url : "_"), info);
    goto err;
  }

  file = fopen(media->url, "rb");
  if (!file)
  {
    syslog(LOG_INFO, "%s(%d) failed to open file(%s)", __func__, __LINE__, media->url);
    goto err;
  }

  if (fseek(file, 0, SEEK_END) != 0)
  {
    syslog(LOG_INFO, "%s(%d) fseek file(%s) failed.", __func__, __LINE__, media->url);
    goto err;
  }

  file_size = ftell(file);
  if (file_size < -1)
  {
    syslog(LOG_INFO, "%s(%d) ftell file(%s) failed.", __func__, __LINE__, media->url);
    goto err;
  }

  rewind(file);
  syslog(LOG_INFO, "%s(%d) wav file(%s) size: %ld", __func__, __LINE__, media->url, file_size);

  if (fread(&header, sizeof(WavHeader), 1, file) != 1)
  {
    syslog(LOG_INFO, "%s(%d) failed to read header", __func__, __LINE__);
    goto err;
  }

  if (memcmp(header.riff_id, "RIFF", 4) != 0)
  {
    syslog(LOG_INFO, "%s(%d) the RIFF of header error.", __func__, __LINE__);
    goto err;
  }

  if (memcmp(header.wave_id, "WAVE", 4) != 0)
  {
    syslog(LOG_INFO, "%s(%d) the WAVE of header error.", __func__, __LINE__);
    goto err;
  }

  if (memcmp(header.fmt_id, "fmt ", 4) != 0)
  {
    syslog(LOG_INFO, "%s(%d) the fmt of header error.", __func__, __LINE__);
    goto err;
  }

  if (header.audio_format != 1) // 1 = pcm
  {
    syslog(LOG_INFO, "%s(%d) wav format(%d) error.", __func__, __LINE__, header.audio_format);
    goto err;
  }

  info->sample_rate = header.sample_rate;
  info->channels = header.channels;
  info->bits_per_sample = header.bits_per_sample;
  info->file_size = file_size;

  syslog(LOG_INFO, "%s(%d) wav file: sample_rate: %" PRIu32 ", channels: %" PRIu16 ", bits_per_sample: %" PRIu16 ", file_size: %ld", __func__, __LINE__,
         info->sample_rate, info->channels, info->bits_per_sample, file_size);

  ret = true;

err:
  if (file)
  {
    fclose(file);
  }

  return ret;
}

static const char *mediatest_event2str(int event)
{
  switch (event)
    {
    case MEDIA_EVENT_NOP:
      return "NOP";
    case MEDIA_EVENT_PREPARED:
      return "PREPARED";
    case MEDIA_EVENT_STARTED:
      return "STARTED";
    case MEDIA_EVENT_PAUSED:
      return "PAUSED";
    case MEDIA_EVENT_STOPPED:
      return "STOPPED";
    case MEDIA_EVENT_SEEKED:
      return "SEEKED";
    case MEDIA_EVENT_COMPLETED:
      return "COMPLETED";
    case MEDIA_EVENT_CHANGED:
      return "CHANGED";
    case MEDIA_EVENT_UPDATED:
      return "UPDATED";
    case MEDIA_EVENT_START:
      return "START";
    case MEDIA_EVENT_PAUSE:
      return "START";
    case MEDIA_EVENT_STOP:
      return "STOP";
    case MEDIA_EVENT_PREV_SONG:
      return "PREV_SONG";
    case MEDIA_EVENT_NEXT_SONG:
      return "NEXT_SONG";
    case MEDIA_EVENT_INCREASE_VOLUME:
      return "INCREASE_VOLUME";
    case MEDIA_EVENT_DECREASE_VOLUME:
      return "DECREASE_VOLUME";
    default:
      return "UNKOWN";
    }
}

static void mediatest_ctrl_lock(void)
{
  pthread_mutex_lock(&mediatest_ctrl_mutex);
}

static void mediatest_ctrl_unlock(void)
{
  pthread_mutex_unlock(&mediatest_ctrl_mutex);
}

static void mediatest_controller_callback(void *cookie, int event,
                                          int ret, const char *data)
{
  GET_TIMESTAMP();
  syslog(LOG_INFO, "%s, event %s, event %d, ret %d, line %d\n", __func__,
         mediatest_event2str(event), event, ret, __LINE__);
}

static void mediatest_controllee_callback(void *cookie, int event,
                                          int ret, const char *data)
{
  GET_TIMESTAMP();
  struct mediatest_data *media = (struct mediatest_data *)cookie;

  syslog(LOG_INFO, "%s, event %s ,event %d, ret %d, line %d\n", __func__,
         mediatest_event2str(event), event, ret, __LINE__);

  media_session_notify(media->handle, event, 0, "fake");
}

static void mediatest_music_callback(void *cookie, int event, int ret,
                                     const char *data)
{
  GET_TIMESTAMP();
  struct mediatest_data *media = (struct mediatest_data *)cookie;

  syslog(LOG_INFO, "%s, event %s, event %d, ret %d, line %d\n", __func__,
         mediatest_event2str(event), event, ret, __LINE__);

  switch (event)
    {
    case MEDIA_EVENT_START:
      media_player_start(media->handle);
      break;

    case MEDIA_EVENT_PAUSE:
      media_player_pause(media->handle);
      break;

    case MEDIA_EVENT_STOP:
      media_player_stop(media->handle);
      break;

    case MEDIA_EVENT_INCREASE_VOLUME:
      ret = media_policy_decrease_stream_volume(MEDIA_STREAM_MUSIC);
      break;

    case MEDIA_EVENT_DECREASE_VOLUME:
      ret = media_policy_increase_stream_volume(MEDIA_STREAM_MUSIC);
      break;

    default:
      ret = -ENOSYS;
      break;
    }
  media_session_notify(media->handle, event, ret, NULL);
}

void mediatest_event_callback(void *cookie, int event, int ret,
                              const char *data)
{
  GET_TIMESTAMP();
  struct mediatest_data *media = (struct mediatest_data *)cookie;

  if (media == NULL)
    {
      syslog(LOG_ERR, "media callback data is null\n");
      return;
    }

  media->ret = ret;

  if (event == MEDIA_EVENT_STARTED)
    {
      media->state = PLAYER_STARTED;
    }
  else if (event == MEDIA_EVENT_STOPPED)
    {
      media->state = PLAYER_STOPPED;
    }
  else if (event == MEDIA_EVENT_COMPLETED)
    {
      media->state = PLAYER_COMPLETED;
      media->complete = true;
    }
  else if (event == MEDIA_EVENT_PREPARED)
    {
      media->state = PLAYER_PREPARED;
    }
  else if (event == MEDIA_EVENT_PAUSED)
    {
      media->state = PLAYER_PAUSED;
    }
  else if (event == MEDIA_EVENT_SEEKED)
    {
      media->state = PLAYER_SEEKED;
    }

  if (ret < 0)
    {
      syslog(LOG_ERR, "[MEDIA_TEST] CALLBACK RET FAILED\n");
    }

  syslog(LOG_INFO, "%s, event %s, event %d, ret %d, line %d\n", __func__,
         mediatest_event2str(event), event, ret, __LINE__);
  if (media->extra)
    media_session_notify(media->extra, event, ret, data);
}

static void mediatest_focus_callback(int suggestion, void *cookie)
{
  GET_TIMESTAMP();
  char *str;

  if (suggestion == MEDIA_FOCUS_PLAY)
    {
      str = "MEDIA_FOCUS_PLAY";
    }
  else if (suggestion == MEDIA_FOCUS_STOP)
    {
      str = "MEDIA_FOCUS_STOP";
    }
  else if (suggestion == MEDIA_FOCUS_PAUSE)
    {
      str = "MEDIA_FOCUS_PAUSE";
    }
  else if (suggestion == MEDIA_FOCUS_PLAY_BUT_SILENT)
    {
      str = "MEDIA_FOCUS_PLAY_BUT_SILENT";
    }
  else if (suggestion == MEDIA_FOCUS_PLAY_WITH_DUCK)
    {
      str = "MEDIA_FOCUS_PLAY_WITH_DUCK";
    }
  else if (suggestion == MEDIA_FOCUS_PLAY_WITH_KEEP)
    {
      str = "MEDIA_FOCUS_PLAY_WITH_KEEP";
    }
  else
    {
      str = "UNKOWN";
    }

  syslog(LOG_INFO, "%s, suggestion %s, suggestion %d, line %d\n",
         __func__, str, suggestion, __LINE__);
}

#ifdef CONFIG_LIBUV_EXTENSION
static void mediatest_uv_common_close_cb(void *cookie, int ret)
{
  GET_TIMESTAMP();
  struct mediatest_data *media = cookie;

  syslog(LOG_INFO, "[%s] stream [%s] ret:%d\n", __func__,
         media->stream_type, ret);
  media->uv_waiting = PLAYER_CLOSED;
  media->handle = NULL;
}

static void mediatest_uv_common_open_cb(void *cookie, int ret)
{
  GET_TIMESTAMP();
  struct mediatest_data *media = cookie;

  syslog(LOG_INFO, "[%s] stream [%s] ret:%d\n", __func__,
         media->stream_type, ret);
  media->uv_waiting = PLAYER_OPENED;
}

static void mediatest_uv_common_prepare_cb(void *cookie, int ret)
{
  GET_TIMESTAMP();
  struct mediatest_data *media = cookie;

  syslog(LOG_INFO, "[%s] stream [%s] ret:%d\n", __func__,
         media->stream_type, ret);
}

static void mediatest_uv_common_seek_cb(void *cookie, int ret)
{
  GET_TIMESTAMP();
  struct mediatest_data *media = cookie;

  syslog(LOG_INFO, "[%s] stream [%s] ret:%d\n", __func__,
         media->stream_type, ret);
}

static void mediatest_uv_common_start_cb(void *cookie, int ret)
{
  GET_TIMESTAMP();
  struct mediatest_data *media = cookie;

  syslog(LOG_INFO, "[%s] stream [%s] ret:%d\n", __func__,
         media->stream_type, ret);
}

static void mediatest_uv_common_stop_cb(void *cookie, int ret)
{
  GET_TIMESTAMP();
  struct mediatest_data *media = cookie;

  syslog(LOG_INFO, "[%s] stream [%s] ret:%d\n", __func__,
         media->stream_type, ret);
  free(media->buf);
  media->buf = NULL;
}

static void mediatest_uv_player_reset_cb(void *cookie, int ret)
{
  GET_TIMESTAMP();
  struct mediatest_data *media = cookie;

  syslog(LOG_INFO, "[%s] url:%s ret:%d\n", __func__, media->url, ret);
}

static void mediatest_uv_recorder_reset_cb(void *cookie, int ret)
{
  GET_TIMESTAMP();
  struct mediatest_data *media = cookie;

  printf("[%s] url:%s ret:%d\n", __func__, media->url, ret);
}

static void mediatest_uv_common_pause_cb(void *cookie, int ret)
{
  GET_TIMESTAMP();
  struct mediatest_data *media = cookie;

  syslog(LOG_INFO, "[%s] stream [%s] ret:%d\n", __func__,
         media->stream_type, ret);
}

static void mediatest_uv_player_get_position_cb(void *cookie, int ret,
                                                unsigned position)
{
  GET_TIMESTAMP();
  struct mediatest_data *media = cookie;
  media->position = position;
  media->uv_waiting = PLAYER_GET_POSITION;
  media->ret = ret;
  syslog(LOG_INFO, "[%s] stream [%s] ret:%d val:%u\n", __func__,
         media->stream_type, ret, position);
}

static void mediatest_uv_player_get_duration_cb(void *cookie, int ret,
                                                unsigned duration)
{
  GET_TIMESTAMP();
  struct mediatest_data *media = cookie;
  media->duration = duration;
  media->uv_waiting = PLAYER_GET_DURATION;
  media->ret = ret;

  syslog(LOG_INFO, "[%s] stream [%s] ret:%d val:%u\n", __func__,
         media->stream_type, ret, duration);
}

static void mediatest_uv_player_get_volume_cb(void *cookie, int ret,
                                              float val)
{
  GET_TIMESTAMP();
  struct mediatest_data *media = cookie;
  media->volume = val;
  media->uv_waiting = PLAYER_GET_VOLUME;
  media->ret = ret;
  syslog(LOG_INFO, "[%s] name:%s ret:%d val:%f\n", __func__,
         media->stream_type, ret, val);
}

static void mediatest_uv_player_set_loop_cb(void *cookie, int ret)
{
  GET_TIMESTAMP();
  struct mediatest_data *media = cookie;
  media->uv_waiting = PLAYER_SET_LOOP;
  media->ret = ret;

  syslog(LOG_INFO, "[%s] stream [%s] ret:%d\n", __func__,
         media->stream_type, ret);
}

static void mediatest_uv_player_set_volume_cb(void *cookie, int ret)
{
  GET_TIMESTAMP();
  struct mediatest_data *media = cookie;

  syslog(LOG_INFO, "[%s] stream [%s] ret:%d\n", __func__,
         media->stream_type, ret);
}

static void mediatest_uv_player_get_playing_cb(void *cookie, int ret,
                                               int playing)
{
  GET_TIMESTAMP();
  struct mediatest_data *media = cookie;
  media->playing = playing;
  media->uv_waiting = PLAYER_GET_PLAYING;
  media->ret = ret;

  syslog(LOG_INFO, "[%s] stream [%s] ret:%d, playing:%d\n", __func__,
         media->stream_type, ret, playing);
}

#endif /* CONFIG_LIBUV_EXTENSION */

void mediatest_common_stop_thread(struct mediatest_data *media)
{
  if (media->thread)
    {
      pthread_join(media->thread, NULL);
      free(media->buf);
      media->thread = 0;
      media->buf = NULL;
    }
  media->stop_flag = false;
  if (media->fd > 0)
    {
      close(media->fd);
      media->fd = -1;
    }
}

int mediatest_player_open(struct mediatest_data *media)
{
  if (!media)
    return -1;

  int ret = 1;

  media->handle = media_player_open(media->stream_type);
  if (!media->handle)
    {
      syslog(LOG_ERR, "media_player_open error\n");
      return -EINVAL;
    }

  ret = media_player_set_event_callback(media->handle, media,
                                        mediatest_event_callback);
  if (ret < 0)
    {
      syslog(LOG_ERR, "media_player_set_event_callback fail \n");
      return 2;
    }
  if (media->stream_type &&
      !strcmp(media->stream_type, MEDIA_STREAM_MUSIC))
    {
      media->extra =
          media_session_register(media, mediatest_music_callback);
    }
  media->type = MEDIATEST_PLAYER;
  syslog(LOG_INFO, "Open %s success.\n", media->stream_type);
  return 0;
}

int mediatest_recorder_open(struct mediatest_data *media)
{
  if (!media)
    return -1;

  int ret;

  media->handle = media_recorder_open(media->stream_type);
  if (!media->handle)
    {
      syslog(LOG_ERR, "media_recorder_open error\n");
      return -EINVAL;
    }
  ret = media_recorder_set_event_callback(media->handle, media,
                                          mediatest_event_callback);
  if (ret < 0)
    {
      syslog(LOG_ERR, "mediatest_recorder_open fail \n");
      return -1;
    }

  media->type = MEDIATEST_RECORDER;
  syslog(LOG_INFO, "recorder success\n");
  return 0;
}

int mediatest_session_open(struct mediatest_data *media)
{
  int ret;

  media->handle = media_session_open(media->stream_type);
  if (!media->handle)
    {
      syslog(LOG_ERR, "media_session_open error\n");
      return -EINVAL;
    }
  ret = media_session_set_event_callback(media->handle, media,
                                         mediatest_controller_callback);
  if (ret < 0)
    {
      syslog(LOG_ERR, "mediatest_session_open fail \n");
      return -1;
    }

  media->type = MEDIATEST_CONTROLLER;
  syslog(LOG_INFO, "session open success\n");
  return 0;
}

#ifdef CONFIG_LIBUV_EXTENSION
int mediatest_uv_player_open(struct mediatest_data *media)
{
  if (!media)
    return -1;

  media->handle =
      media_uv_player_open(g_mediatest_uvloop, media->stream_type,
                           mediatest_uv_common_open_cb, media);
  GET_TIMESTAMP();
  if (!media->handle)
    {
      syslog(LOG_ERR, "media_uv_player_open error\n");
      return -EINVAL;
    }
  if (media_uv_player_listen(media->handle, mediatest_event_callback) <
      0)
    {
      syslog(LOG_WARNING, "uvplayer set callback failed\n");
      return -EINVAL;
    }
  media->type = MEDIATEST_UVPLAYER;
  syslog(LOG_INFO, "uv_player open success\n");
  return 0;
}

int mediatest_uv_recorder_open(struct mediatest_data *media)
{
  if (!media)
    return -1;

  media->handle =
      media_uv_recorder_open(g_mediatest_uvloop, media->stream_type,
                             mediatest_uv_common_open_cb, media);
  GET_TIMESTAMP();
  if (!media->handle)
    {
      syslog(LOG_ERR, "media_uv_recorder_open error\n");
      return -EINVAL;
    }
  if (media_uv_recorder_listen(media->handle, mediatest_event_callback) <
      0)
    {
      syslog(LOG_WARNING, "uvrecorder set callback failed\n");
      return -EINVAL;
    }
  media->type = MEDIATEST_UVRECORDER;
  syslog(LOG_INFO, "uv_recorder open success\n");
  return 0;
}
#endif

int mediatest_common_open(struct mediatest_data *media)
{
  if (!media)
    return -1;

  int ret = -EINVAL;
  switch (media->type)
    {
    case MEDIATEST_PLAYER:
      ret = mediatest_player_open(media);
      break;

    case MEDIATEST_RECORDER:
      ret = mediatest_recorder_open(media);
      break;

    case MEDIATEST_CONTROLLER:
      ret = mediatest_session_open(media);
      break;
#ifdef CONFIG_LIBUV_EXTENSION
    case MEDIATEST_UVPLAYER:
      ret = mediatest_uv_player_open(media);
      break;
    case MEDIATEST_UVRECORDER:
      ret = mediatest_uv_recorder_open(media);
      break;
#endif
    }
  if (ret < 0)
    syslog(LOG_WARNING, "mediatest_common_open failed\n");

  int en = mediatest_send("_", "loglevel", "50");
  if (en < 0)
    {
      syslog(LOG_ERR, "mediatest_send failed\n");
      return -1;
    }
  return ret;
}

int mediatest_session_register(struct mediatest_data *media)
{

  media->handle =
      media_session_register(media, mediatest_controllee_callback);
  GET_TIMESTAMP();
  if (!media->handle)
    {
      syslog(LOG_ERR, "media_session_register error\n");
      return -EINVAL;
    }

  media->type = MEDIATEST_CONTROLLEE;

  syslog(LOG_INFO, "session controllee register\n");

  return 0;
}

int mediatest_common_close(struct mediatest_data *media)
{
  int ret = -EINVAL;
  if (!media->handle)
    return -EINVAL;

  switch (media->type)
    {
    case MEDIATEST_PLAYER:
      if (!media->pending_stop)
        mediatest_common_stop(media);
      ret = media_player_close(media->handle, media->pending_stop);
      if (ret >= 0 && media->extra)
        ret = media_session_unregister(media->extra);
      break;

    case MEDIATEST_RECORDER:
      mediatest_common_stop(media);
      ret = media_recorder_close(media->handle);
      break;

    case MEDIATEST_CONTROLLER:
      ret = media_session_close(media->handle);
      break;

    case MEDIATEST_CONTROLLEE:
      ret = media_session_unregister(media->handle);
      break;

    case MEDIATEST_FOCUS:
      ret = media_focus_abandon(media->handle);
      break;
#ifdef CONFIG_LIBUV_EXTENSION
    case MEDIATEST_UVPLAYER:
      if (!media->pending_stop)
        mediatest_common_stop(media);
      ret = media_uv_player_close(media->handle, media->pending_stop,
                                  mediatest_uv_common_close_cb);
      GET_TIMESTAMP();
      break;

    case MEDIATEST_UVRECORDER:
      mediatest_common_stop(media);
      ret = media_uv_recorder_close(media->handle,
                                    mediatest_uv_common_close_cb);
      GET_TIMESTAMP();
      break;

    case MEDIATEST_UVFOCUS:
      ret = media_uv_focus_abandon(media->handle,
                                   mediatest_uv_common_close_cb);
      GET_TIMESTAMP();
      break;
#endif
    }
  if (ret < 0)
    syslog(LOG_WARNING, "mediatest_common_close failed\n");

  media->handle = NULL;
  media->extra = NULL;
  usleep(500 *1000);
  return ret;
}

int mediatest_common_reset(struct mediatest_data *media)
{
  int ret = -EINVAL;
  if (!media->handle)
    return -EINVAL;

  media->stop_flag = true;
  if (media->thread)
    {
      pthread_join(media->thread, NULL);
      free(media->buf);
      media->thread = 0;
      media->buf = NULL;
    }

  switch (media->type)
    {
    case MEDIATEST_PLAYER:
      ret = media_player_reset(media->handle);
      break;

    case MEDIATEST_RECORDER:
      ret = media_recorder_reset(media->handle);
      break;

#ifdef CONFIG_LIBUV_EXTENSION
    case MEDIATEST_UVPLAYER:
      ret = media_uv_player_reset(media->handle,
                                  mediatest_uv_player_reset_cb, media);
      GET_TIMESTAMP();
      break;

    case MEDIATEST_UVRECORDER:
      ret = media_uv_recorder_reset(
          media->handle, mediatest_uv_recorder_reset_cb, media);
      GET_TIMESTAMP();
      break;
#endif

    default:
      return 0;
    }
  if (ret < 0)
    syslog(LOG_WARNING, "mediatest_common_reset return failed\n");

  usleep(1000);

  mediatest_common_stop_thread(media);
  return ret;
}

int mediatest_process_data(int sockfd, bool player, void *data,
                           size_t len)
{
  int event = player ? POLLOUT : POLLIN;
  struct pollfd sockfds[1];
  int ret;

  sockfds[0].fd = sockfd;
  sockfds[0].events = event;
  ret = poll(sockfds, 1, -1);

  if (ret < 0)
    return -errno;

  if (player)
    return send(sockfd, data, len, 0);
  else
    return recv(sockfd, data, len, 0);
}

void *mediatest_common_thread(void *arg)
{
  struct mediatest_data *media = arg;
  char *tmp;
  int act;
  int ret = -1;
  int sockfd __attribute__((unused)) = 0;
  const int64_t duration_us = DEF_SLEEP_DURATION_US;
  struct timespec cur_time, last_time;

  syslog(LOG_INFO, "%s, start, line %d\n", __func__, __LINE__);

  if (media->mode == MODE_DIRECT)
    {
      if (media->type == MEDIATEST_PLAYER)
        {
          sockfd = media_player_get_socket(media->handle);
        }
      else
        {
          sockfd = media_recorder_get_socket(media->handle);
        }

      if (sockfd < 0)
        {
          syslog(LOG_ERR, "direct mode, socket can't get \n");
          return NULL;
        }

      if (fcntl(sockfd, F_SETFL,
                fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK) < 0)
        return NULL;
    }

  if (media->type == MEDIATEST_PLAYER)
    {
      clock_gettime(CLOCK_MONOTONIC, &cur_time);
      last_time = cur_time;

      while (1)
        {
          if (media->stop_flag)
            goto out;

          act = read(media->fd, media->buf, media->size);
          assert(act >= 0);
          if (act == 0)
            {
              media_player_close_socket(media->handle);
              break;
            }

          tmp = media->buf;
          while (act > 0)
            {
              if (media->mode == MODE_DIRECT)
                {
                  ret = mediatest_process_data(sockfd, true, tmp, act);
                }
              else
                {
                  ret = media_player_write_data(media->handle, tmp, act);
                }

              if (ret == 0)
                {
                  break;
                }
              else if (ret < 0 && errno == EAGAIN)
                {
                  usleep(100);
                  continue;
                }
              else if (ret < 0)
                {
                  syslog(LOG_WARNING,
                         "%s, error ret %d errno %d, line %d\n",
                         __func__, ret, errno, __LINE__);
                  goto out;
                }

              tmp += ret;
              act -= ret;
            }

            clock_gettime(CLOCK_MONOTONIC, &cur_time);
            int64_t elapsed_us = (cur_time.tv_sec - last_time.tv_sec) * 1000000LL + (cur_time.tv_nsec - last_time.tv_nsec) / 1000;
            if (elapsed_us < duration_us)
            {
              usleep(duration_us - elapsed_us);
            }
            last_time = cur_time;
        }
    }
  else
    {
      while (1)
        {
          if (media->stop_flag)
            goto out;

          if (media->mode == MODE_DIRECT)
            {
              ret = mediatest_process_data(sockfd, false, media->buf,
                                           media->size);
            }
          else
            {
              ret = media_recorder_read_data(media->handle, media->buf,
                                             media->size);
            }

          if (ret == 0)
            {
              media_recorder_close(media->handle);
              break;
            }
          else if (ret < 0 && errno == EAGAIN)
            {
              continue;
            }

          if (ret <= 0)
            goto out;

          act = write(media->fd, media->buf, ret);
          assert(act == ret);
        }
    }

  syslog(LOG_INFO, "thread free \n");
out:
  syslog(LOG_WARNING, "%s, end, line %d\n", __func__, __LINE__);
  if (!media->stop_flag)
  {
    pthread_detach(pthread_self());
    media->thread = 0;
    if (media->buf)
    {
      free(media->buf);
      media->buf = NULL;
    }
  }
  return NULL;
}

int mediatest_common_prepare(struct mediatest_data *media)
{
  pthread_t thread;
  bool async_mode = false;
  int ret = -EINVAL;
  AudioFmtInfo ainfo;

  if (!media->handle)
    return -EINVAL;

  memset(&ainfo, 0, sizeof(ainfo));
  if (!get_ainfo_from_wav(media, &ainfo))
  {
    ainfo.bits_per_sample = 16;
    ainfo.channels = 2;
    ainfo.sample_rate = 16000;
  }

  if (media->mode != MODE_URL)
    {
      if (media->thread)
        {
          syslog(LOG_WARNING, "already prepare, can't prepare twice\n");
          return -EPERM;
        }
      if (media->type == MEDIATEST_RECORDER)
        unlink(media->url);

      if ((media->type == MEDIATEST_RECORDER) ||
          (media->type == MEDIATEST_UVRECORDER))
        media->fd = open(media->url,
                         O_CREAT | O_RDWR | O_CLOEXEC | O_TRUNC, 0666);
      else if ((media->type == MEDIATEST_PLAYER) ||
               (media->type == MEDIATEST_UVPLAYER))
        media->fd = open(media->url, O_RDONLY | O_CLOEXEC, 0666);

      if (media->fd < 0)
        {
          syslog(LOG_ERR, "buffer mode, file can't open \n");
          return -EINVAL;
        }
    }

  switch (media->type)
    {
    case MEDIATEST_PLAYER:
      ret = media_player_prepare(
          media->handle, media->mode == MODE_URL ? media->url : NULL,
          media->option);
      break;

    case MEDIATEST_RECORDER:
      ret = media_recorder_prepare(
          media->handle, media->mode == MODE_URL ? media->url : NULL,
          media->option);
      break;

#ifdef CONFIG_LIBUV_EXTENSION
    case MEDIATEST_UVPLAYER:
      ret = media_uv_player_prepare(
          media->handle, media->mode ? media->url : NULL, media->option,
          mediatest_uv_player_connection_cb,
          mediatest_uv_common_prepare_cb, media);
      GET_TIMESTAMP();
      async_mode = true;
      break;

    case MEDIATEST_UVRECORDER:
      ret = media_uv_recorder_prepare(
          media->handle, media->mode ? media->url : NULL, media->option,
          mediatest_uv_recorder_connection_cb,
          mediatest_uv_common_prepare_cb, media);
      GET_TIMESTAMP();
      async_mode = true;
      break;
#endif

    default:
      syslog(LOG_WARNING, "Unsupported type\n");
      ret = -EINVAL;
      break;
    }

  if (ret < 0)
    {
      syslog(LOG_WARNING, "mediatest_common_prepare failed\n");
      goto err;
    }

  if (media->mode != MODE_URL && !async_mode)
    {
      media->size = 10 * ainfo.bits_per_sample / 8 * ainfo.channels * ainfo.sample_rate / 1000;//10ms data
      if (media->size <= 0 || media->size > ainfo.file_size)
      {
        syslog(LOG_INFO, "%s(%d) media->size(%d) error.", __func__, __LINE__, media->size);
        media->size = 640;
      }

      media->buf = malloc(media->size);
      if (media->buf == NULL)
        {
          syslog(LOG_ERR, "media buf malloc fail \n");
          return -1;
        }

      ret =
          pthread_create(&thread, NULL, mediatest_common_thread, media);
      if (ret != 0)
        {
          syslog(LOG_ERR, "create thread fail \n");
          return -1;
        }

      pthread_setname_np(thread, "mediatest_file");
      media->thread = thread;
    }

  return ret;
err:
  if (!media->url && media->fd >= 0)
    {
      close(media->fd);
      media->fd = -1;
    }
  return ret;
}

int mediatest_common_start(struct mediatest_data *media)
{
  int ret = -EINVAL;
  if (!media->handle)
    return -EINVAL;

  switch (media->type)
    {
    case MEDIATEST_PLAYER:
      ret = media_player_start(media->handle);
      break;

    case MEDIATEST_RECORDER:
      ret = media_recorder_start(media->handle);
      break;

    case MEDIATEST_CONTROLLER:
      ret = media_session_start(media->handle);
      break;
#ifdef CONFIG_LIBUV_EXTENSION
    case MEDIATEST_UVPLAYER:
      if (media->focus_type)
        {
          ret = media_uv_player_start_auto(
              media->handle, media->focus_type,
              mediatest_uv_common_start_cb, media);
          GET_TIMESTAMP();
        }
      else
        {
          ret = media_uv_player_start(
              media->handle, mediatest_uv_common_start_cb, media);
          GET_TIMESTAMP();
        }
      break;

    case MEDIATEST_UVRECORDER:
      if (media->focus_type)
        {
          ret = media_uv_recorder_start_auto(
              media->handle, media->focus_type,
              mediatest_uv_common_start_cb, media);
          GET_TIMESTAMP();
        }
      else
        {
          ret = media_uv_recorder_start(
              media->handle, mediatest_uv_common_start_cb, media);
          GET_TIMESTAMP();
        }

      break;
#endif
    }
  if (ret < 0)
    syslog(LOG_WARNING, "mediatest_common_start failed\n");

  return ret;
}

int mediatest_common_stop(struct mediatest_data *media)
{
  int ret = -EINVAL;
  if (!media->handle)
    return -EINVAL;

  media->stop_flag = true;
  if (media->thread)
    {
      pthread_join(media->thread, NULL);
      free(media->buf);
      media->thread = 0;
      media->buf = NULL;
    }

  switch (media->type)
    {
    case MEDIATEST_PLAYER:
      ret = media_player_stop(media->handle);
      GET_TIMESTAMP();
      break;

    case MEDIATEST_RECORDER:
      ret = media_recorder_stop(media->handle);
      GET_TIMESTAMP();
      break;

#ifdef CONFIG_LIBUV_EXTENSION
    case MEDIATEST_UVPLAYER:
      ret = media_uv_player_stop(media->handle,
                                 mediatest_uv_common_stop_cb, media);
      GET_TIMESTAMP();
      break;

    case MEDIATEST_UVRECORDER:
      ret = media_uv_recorder_stop(media->handle,
                                   mediatest_uv_common_stop_cb, media);
      GET_TIMESTAMP();
      break;
#endif
    }
  usleep(1000);

  mediatest_common_stop_thread(media);

  if (ret < 0)
    syslog(LOG_WARNING, "mediatest_common_stop failed\n");

  return ret;
}

int mediatest_common_pause(struct mediatest_data *media)
{
  int ret = -EINVAL;
  if (!media->handle)
    return -EINVAL;

  switch (media->type)
    {
    case MEDIATEST_PLAYER:
      ret = media_player_pause(media->handle);
      break;

    case MEDIATEST_RECORDER:
      ret = media_recorder_pause(media->handle);
      break;

    case MEDIATEST_CONTROLLER:
      ret = media_session_pause(media->handle);
      break;
#ifdef CONFIG_LIBUV_EXTENSION
    case MEDIATEST_UVPLAYER:
      ret = media_uv_player_pause(media->handle,
                                  mediatest_uv_common_pause_cb, media);
      GET_TIMESTAMP();
      break;

    case MEDIATEST_UVRECORDER:
      ret = media_uv_recorder_pause(media->handle,
                                    mediatest_uv_common_pause_cb, media);
      GET_TIMESTAMP();
      break;
#endif
    }

  if (ret < 0)
    syslog(LOG_WARNING, "mediatest_common_pause return failed\n");

  return ret;
}

int mediatest_player_set_volume(struct mediatest_data *media)
{
  int ret = -EINVAL;

  if (!media->handle)
    return -EINVAL;

  switch (media->type)
    {
    case MEDIATEST_PLAYER:
      ret = media_player_set_volume(media->handle, media->volume);
      break;

    case MEDIATEST_CONTROLLER:
      ret = media_session_set_volume(media->handle, (int)media->volume);
      break;

#ifdef CONFIG_LIBUV_EXTENSION
    case MEDIATEST_UVPLAYER:
      ret = media_uv_player_set_volume(media->handle, media->volume,
                                       mediatest_uv_player_set_volume_cb,
                                       media);
      GET_TIMESTAMP();
      break;
#endif

    default:
      return 0;
    }

  if (ret < 0)
    {
      syslog(LOG_WARNING, "mediatest_player_set_volume failed\n");
      return ret;
    }

  syslog(LOG_INFO, "set volume sucess :%f\n", media->volume);

  return 0;
}

int mediatest_session_inc_dec_volume(struct mediatest_data *media,
                                     char *inde)
{
  int ret = -EINVAL;
  int volume;
  if (!media->handle)
    return -EINVAL;

  switch (media->type)
    {
    case MEDIATEST_CONTROLLER:
      if (strcmp(inde, "+") == 0)
        {
          ret = media_session_get_volume(media->handle, &volume);
          ret = media_session_increase_volume(media->handle);
          syslog(LOG_INFO, "increase volume %d++\n", volume);
        }
      else if (strcmp(inde, "-"))
        {
          ret = media_session_get_volume(media->handle, &volume);
          ret = media_session_decrease_volume(media->handle);
          syslog(LOG_INFO, "decrease volume %d--\n", volume);
        }
      else
        {
          syslog(LOG_ERR,
                 "mediatest_session_inc_dec_volume ptr error\n");
        }
      break;

    default:
      return 0;
    }

  if (ret < 0)
    {
      syslog(LOG_WARNING, "mediatest_session_inc_dec_volume failed\n");
      return ret;
    }

  return 0;
}

int mediatest_player_get_volume(struct mediatest_data *media)
{
  int ret = -EINVAL;

  if (!media->handle)
    return ret;

  switch (media->type)
    {
    case MEDIATEST_PLAYER:
      ret = media_player_get_volume(media->handle, &media->volume);
      break;

    case MEDIATEST_CONTROLLER:
      ret =
          media_session_get_volume(media->handle, (int *)&media->volume);
      break;

#ifdef CONFIG_LIBUV_EXTENSION
    case MEDIATEST_UVPLAYER:
      ret = media_uv_player_get_volume(
          media->handle, mediatest_uv_player_get_volume_cb, media);
      GET_TIMESTAMP();
      break;
#endif

    default:
      return 0;
    }

  if (ret < 0)
    {
      syslog(LOG_WARNING, "mediatest_player_get_volume failed\n");
      return ret;
    }

  syslog(LOG_INFO, "get volume sucess :%f\n", media->volume);
  return ret;
}

int mediatest_player_loop(struct mediatest_data *media)
{
  if (media->loop == 0 || !media->handle)
    return -EINVAL;

  int ret = -1;
  switch (media->type)
    {
    case MEDIATEST_PLAYER:
      ret = media_player_set_looping(media->handle, media->loop);
      break;

#ifdef CONFIG_LIBUV_EXTENSION
    case MEDIATEST_UVPLAYER:
      ret = media_uv_player_set_looping(media->handle, media->loop,
                                        mediatest_uv_player_set_loop_cb,
                                        media);
      GET_TIMESTAMP();
      break;
#endif

    default:
      syslog(LOG_WARNING,
             "[media_test] mediatest set loop type error\n");
      break;
    }
  if (ret < 0)
    syslog(LOG_WARNING, "mediatest_player_loop failed\n");

  media->ret = ret;
  return ret;
}

int mediatest_position(struct mediatest_data *media)
{
  int ret = -EINVAL;

  if (media == NULL || !media->handle)
    return -1;

  switch (media->type)
    {
    case MEDIATEST_PLAYER:
      ret = media_player_get_position(media->handle, &media->position);
      break;

    case MEDIATEST_CONTROLLER:
      ret = media_session_get_position(media->handle, &media->position);
      break;

#ifdef CONFIG_LIBUV_EXTENSION
    case MEDIATEST_UVPLAYER:
      return media_uv_player_get_position(
          media->handle, mediatest_uv_player_get_position_cb, media);
      GET_TIMESTAMP();
#endif
    default:
      return 0;
    }

  if (ret < 0)
    {
      syslog(LOG_WARNING, "Current position ret %d\n", ret);
      return ret;
    }

  return 0;
}

int mediatest_seek(struct mediatest_data *media)
{
  int ret = -EINVAL;
  if (!media->handle)
    return ret;

  switch (media->type)
    {
    case MEDIATEST_PLAYER:
      ret = media_player_seek(media->handle, media->position);
      break;
#ifdef CONFIG_LIBUV_EXTENSION
    case MEDIATEST_UVPLAYER:
      ret = media_uv_player_seek(media->handle, media->position,
                                 mediatest_uv_common_seek_cb, media);
      GET_TIMESTAMP();
      break;
#endif
    default:
      break;
    }

  if (ret < 0)
    {
      syslog(LOG_WARNING, "mediatest_seek failed\n");
    }

  return ret;
}

int mediatest_duration(struct mediatest_data *media)
{
  int ret = -EINVAL;

  if (media == NULL || !media->handle)
    return -1;

  switch (media->type)
    {
    case MEDIATEST_PLAYER:
      ret = media_player_get_duration(media->handle, &media->duration);
      break;

    case MEDIATEST_CONTROLLER:
      ret = media_session_get_duration(media->handle, &media->duration);
      break;
#ifdef CONFIG_LIBUV_EXTENSION
    case MEDIATEST_UVPLAYER:
      ret = media_uv_player_get_duration(
          media->handle, mediatest_uv_player_get_duration_cb, media);
      GET_TIMESTAMP();
      break;
#endif

    default:
      return 0;
    }

  if (ret < 0)
    {
      syslog(LOG_WARNING, "Total duration ret %d\n", ret);
      return ret;
    }

  return 0;
}

int mediatest_player_isplaying(struct mediatest_data *media)
{
  int ret = -EINVAL;

  if (!media->handle)
    return ret;

  switch (media->type)
    {
    case MEDIATEST_PLAYER:
      ret = media_player_is_playing(media->handle);
      break;
#ifdef CONFIG_LIBUV_EXTENSION
    case MEDIATEST_UVPLAYER:
      ret = media_uv_player_get_playing(
          media->handle, mediatest_uv_player_get_playing_cb, media);
      GET_TIMESTAMP();
      break;
#endif
    default:
      return 0;
    }

  if (ret < 0)
    {
      syslog(LOG_WARNING, "mediatest_player_isplaying failed\n");
      return ret;
    }
  syslog(LOG_INFO, "Is_playing %d\n", ret);

  return ret;
}
int mediatest_playdtmf(struct mediatest_data *media, char *dial_number)
{
  short int *buffer;
  int buffer_size;
  int ret = -EINVAL;

  if (!media->handle)
    return ret;

  buffer_size = media_dtmf_get_buffer_size(dial_number);
  if (buffer_size < 0)
    {
      syslog(LOG_ERR, "playdtmf get buffer size failed\n");
      return -1;
    }
  buffer = (short int *)malloc(buffer_size);
  assert(buffer);

  ret = media_dtmf_generate(dial_number, buffer);
  if (ret < 0)
    goto out;

  ret =
      media_player_prepare(media->handle, NULL, MEDIA_TONE_DTMF_FORMAT);
  if (ret < 0)
    goto out;

  if (media->mode == MODE_DIRECT)
    {
      media->fd = media_player_get_socket(media->handle);
      if (media->fd < 0)
        goto out;

      if (fcntl(media->fd, F_SETFL,
                fcntl(media->fd, F_GETFL, 0) | O_NONBLOCK) < 0)
        goto out;

      ret = mediatest_process_data(media->fd, true, buffer, buffer_size);
    }
  else
    ret = media_player_write_data(media->handle, buffer, buffer_size);

  if (ret == buffer_size)
    {
      media_player_close_socket(media->handle);
      ret = 0;
    }
  else
    {
      syslog(LOG_ERR, "Failed to play DTMF tone.");
    }
out:
  free(buffer);
  buffer = NULL;

  return ret;

  return 0;
}

int mediatest_session_prevsong(struct mediatest_data *media)
{
  int ret = -EINVAL;

  if (!media->handle)
    return ret;

  if (media->type != MEDIATEST_CONTROLLER)
    return 0;

  ret = media_session_prev_song(media->handle);
  if (ret < 0)
    syslog(LOG_WARNING, "mediatest_session_prevsong failed\n");

  return ret;
}

int mediatest_session_nextsong(struct mediatest_data *media)
{
  int ret = -EINVAL;

  if (!media->handle)
    return ret;

  if (media->type != MEDIATEST_CONTROLLER)
    return 0;

  ret = media_session_next_song(media->handle);
  if (ret < 0)
    syslog(LOG_WARNING, "mediatest_session_nextsong failed\n");

  return ret;
}

int mediatest_send(char *target, char *cmd, char *pargs)
{
  int ret = -EINVAL;
  if (!target)
    return ret;

  if (!cmd)
    return ret;

  ret = media_process_command(target, cmd, pargs, NULL, 0);
  if (ret < 0)
    syslog(LOG_WARNING, "mediatest_send failed\n");

  return ret;
}

int mediatest_dump(void)
{
  media_policy_dump(NULL);
  media_graph_dump(NULL);
  media_focus_dump(NULL);

  return 0;
}

int mediatest_setint(const char *name, int value, int apply)
{
  int ret = media_policy_set_int(name, value, apply);
  if (ret < 0)
    {
      syslog(LOG_ERR, "FAIL: %s ,ret is %d\n", __func__, ret);
    }
  return ret;
}

int mediatest_getint(char *name, int *value)
{
  int ret;

  ret = media_policy_get_int(name, value);
  if (ret < 0)
    return -EINVAL;

  syslog(LOG_INFO, "get criterion %s integer value = %d\n", name,
         *value);

  return 0;
}

int mediatest_setstring(char *name, char *value, int apply)
{
  int ret = media_policy_set_string(name, value, apply);
  if (ret < 0)
    {
      syslog(LOG_ERR, "FAIL: %s ,ret is %d\n", __func__, ret);
    }
  return ret;
}

int mediatest_getstring(char *name, char *value)
{
  int ret;

  ret = media_policy_get_string(name, value, strlen(value));
  if (ret < 0)
    {
      syslog(LOG_WARNING, "FAIL: %s ,ret is %d\n", __func__, ret);
      return -EINVAL;
    }

  syslog(LOG_INFO, "get criterion %s string value = '%s'\n", name,
         value);

  return 0;
}

int mediatest_include(char *name, char *value, int apply)
{
  int ret;

  ret = media_policy_include(name, value, apply);
  if (ret < 0)
    {
      syslog(LOG_WARNING, "FAIL: %s ,ret is %d\n", __func__, ret);
      return -EINVAL;
    }
  return ret;
}

int mediatest_exclude(char *name, char *value, int apply)
{
  int ret;

  ret = media_policy_exclude(name, value, apply);
  if (ret < 0)
    {
      syslog(LOG_WARNING, "FAIL: %s ,ret is %d\n", __func__, ret);
      return -EINVAL;
    }
  return ret;
}

int mediatest_contain(char *name, char *value)
{
  int result, ret;

  ret = media_policy_contain(name, value, &result);
  if (ret < 0)
    {
      syslog(LOG_WARNING, "FAIL: %s ,ret is %d\n", __func__, ret);
      return -EINVAL;
    }

  syslog(LOG_INFO, "criterion %s %s value %s\n", name,
         result ? "contains" : "doesn't contain", value);

  return 0;
}

int mediatest_increase(char *name, int apply)
{
  int ret;

  ret = media_policy_increase(name, apply);
  if (ret < 0)
    {
      syslog(LOG_WARNING, "FAIL: %s ,ret is %d\n", __func__, ret);
      return -EINVAL;
    }
  return ret;
}

int mediatest_decrease(char *name, int apply)
{
  int ret;

  ret = media_policy_decrease(name, apply);
  if (ret < 0)
    {
      syslog(LOG_WARNING, "FAIL: %s ,ret is %d\n", __func__, ret);
      return -EINVAL;
    }
  return ret;
}

int mediatest_focus_request(struct mediatest_data *media, char *name)
{
  int suggestion;

  media->handle = media_focus_request(&suggestion, name,
                                      mediatest_focus_callback, media);
  GET_TIMESTAMP();
  if (!media->handle)
    {
      syslog(LOG_ERR, "media_player_request failed\n");
      return 0;
    }

  media->type = MEDIATEST_FOCUS;
  syslog(LOG_INFO, "focus suggestion %d\n", suggestion);
  return 0;
}

#ifdef CONFIG_LIBUV_EXTENSION
static void mediatest_cmd_uv_policy_set_int_cb(void *cookie, int ret)
{
  GET_TIMESTAMP();
  syslog(LOG_INFO, "[%s] name:%s ret:%d\n", __func__, (char *)cookie,
         ret);
  free(cookie);
}

int mediatest_uv_policy_set_int(char *name, int value, int apply)
{
  int ret = media_uv_policy_set_int(
      g_mediatest_uvloop, name, value, apply,
      mediatest_cmd_uv_policy_set_int_cb, strdup(name));
  GET_TIMESTAMP();
  return ret;
}

static void mediatest_cmd_uv_policy_get_int_cb(void *cookie, int ret,
                                               int val)
{
  struct mediatest_data *media = cookie;
  media->volume = (float)val;
  media->uv_waiting = PLAYER_GET_VOLUME;
  media->ret = ret;
  syslog(LOG_INFO, "[%s] name:%s ret:%d val:%d\n", __func__,
         media->stream_type, ret, val);
}

int mediatest_uv_policy_get_int(struct mediatest_data *media)
{
  char name[64];
  int len;

  len =
      snprintf(name, sizeof(name), "%s%s", media->stream_type, "Volume");
  if (len >= sizeof(name))
    return -ENAMETOOLONG;
  int ret =
      media_uv_policy_get_int(g_mediatest_uvloop, name,
                              mediatest_cmd_uv_policy_get_int_cb, media);
  GET_TIMESTAMP();
  return ret;
}

static void mediatest_cmd_uv_policy_set_string_cb(void *cookie, int ret)
{
  GET_TIMESTAMP();
  syslog(LOG_INFO, "[%s] name:%s ret:%d\n", __func__, (char *)cookie,
         ret);
  free(cookie);
}

int mediatest_uv_policy_set_string(char *name, char *value, int apply)
{
  int ret = media_uv_policy_set_string(
      g_mediatest_uvloop, name, value, apply,
      mediatest_cmd_uv_policy_set_string_cb, strdup(name));
  GET_TIMESTAMP();
  return ret;
}

static void mediatest_cmd_uv_policy_get_string_cb(void *cookie, int ret,
                                                  const char *val)
{
  GET_TIMESTAMP();
  syslog(LOG_INFO, "[%s] name:%s ret:%d val:%s\n", __func__,
         (char *)cookie, ret, val);
  free(cookie);
}

int mediatest_uv_policy_get_string(char *name)
{
  int ret = media_uv_policy_get_string(
      g_mediatest_uvloop, name, mediatest_cmd_uv_policy_get_string_cb,
      strdup(name));
  GET_TIMESTAMP();
  return ret;
}

static void mediatest_cmd_uv_policy_increase_cb(void *cookie, int ret)
{
  GET_TIMESTAMP();
  syslog(LOG_INFO, "[%s] name:%s ret:%d\n", __func__, (char *)cookie,
         ret);
  free(cookie);
}

int mediatest_uv_policy_increase(char *name, int apply)
{
  int ret = media_uv_policy_increase(g_mediatest_uvloop, name, apply,
                                     mediatest_cmd_uv_policy_increase_cb,
                                     strdup(name));
  GET_TIMESTAMP();
  return ret;
}

static void mediatest_cmd_uv_policy_decrease_cb(void *cookie, int ret)
{
  GET_TIMESTAMP();
  syslog(LOG_INFO, "[%s] name:%s ret:%d\n", __func__, (char *)cookie,
         ret);
  free(cookie);
}

int mediatest_uv_policy_decrease(char *name, int apply)
{
  int ret = media_uv_policy_decrease(g_mediatest_uvloop, name, apply,
                                     mediatest_cmd_uv_policy_decrease_cb,
                                     strdup(name));
  GET_TIMESTAMP();
  return ret;
}

int mediatest_uv_policy_set_stream_volume(struct mediatest_data *media)
{
  int ret = media_uv_policy_set_stream_volume(
      g_mediatest_uvloop, media->stream_type, (int)media->volume,
      mediatest_cmd_uv_policy_set_int_cb, strdup(media->stream_type));
  GET_TIMESTAMP();
  return ret;
}
int mediatest_uv_policy_get_stream_volume(struct mediatest_data *media)
{
  int ret = media_uv_policy_get_stream_volume(
      g_mediatest_uvloop, media->stream_type,
      mediatest_cmd_uv_policy_get_int_cb, media);
  GET_TIMESTAMP();
  return ret;
}

int mediatest_uv_policy_increase_stream_volume(
    struct mediatest_data *media)
{
  int ret = media_uv_policy_increase_stream_volume(
      g_mediatest_uvloop, media->stream_type,
      mediatest_cmd_uv_policy_increase_cb, strdup(media->stream_type));
  GET_TIMESTAMP();
  return ret;
}
int mediatest_uv_policy_decrease_stream_volume(
    struct mediatest_data *media)
{
  int ret = media_uv_policy_decrease_stream_volume(
      g_mediatest_uvloop, media->stream_type,
      mediatest_cmd_uv_policy_decrease_cb, strdup(media->stream_type));
  GET_TIMESTAMP();
  return ret;
}

static void mediatest_focus_suggest_cb(int suggest, void *cookie)
{
  GET_TIMESTAMP();
  // struct mediatest_data *media = cookie;

  syslog(LOG_INFO, "[%s] suggest:%d\n", __func__, suggest);
}

int mediatest_uv_focus_request(struct mediatest_data *media)
{
  media->handle =
      media_uv_focus_request(g_mediatest_uvloop, media->focus_type,
                             mediatest_focus_suggest_cb, media);
  GET_TIMESTAMP();
  if (!media->handle)
    {
      syslog(LOG_ERR, "%s failed\n", __func__);
      return 0;
    }

  media->type = MEDIATEST_UVFOCUS;
  return 0;
}

static void mediatest_uv_recorder_alloc_cb(uv_handle_t *handle,
                                           size_t suggested_size,
                                           uv_buf_t *buf)
{
  struct mediatest_data *media = uv_handle_get_data(handle);

  if (media->buf)
    {
      buf->base = NULL;
      buf->len = 0;
      return;
    }
  buf->base = malloc(2048);
  assert(buf->base);
  buf->len = 2048;
}

static void mediatest_uv_recorder_write_cb(uv_fs_t *req)
{
  struct mediatest_data *media = uv_req_get_data((uv_req_t *)req);

  if (media->stop_flag)
    req->result = -1;

  if (req->result < 0)
    {
      syslog(LOG_ERR, "[%s][%d] Recorder write to file Failed: %s\n",
             __func__, __LINE__, uv_strerror(req->result));
      free(media->buf);
      media->buf = NULL;
      return;
    }

  free(media->buf);
  media->buf = NULL;
  uv_fs_req_cleanup(req);
}

static void mediatest_uv_recorder_read_cb(uv_stream_t *stream,
                                          ssize_t nread,
                                          const uv_buf_t *buf)
{
  struct mediatest_data *media =
      uv_handle_get_data((uv_handle_t *)stream);
  uv_fs_t close_req;
  uv_buf_t iov;

  if (nread == UV_ENOBUFS)
    {
      usleep(1000);
      return;
    }
  assert(nread <= 2048);
  media->buf = buf->base;
  uv_req_set_data((uv_req_t *)&media->fs_req, media);
  if (nread < 0)
    {
      if (nread != UV_EOF)
        syslog(LOG_ERR, "[%s][%d] Recorder read error %s\n", __func__,
               __LINE__, uv_err_name(nread));
      uv_fs_close(g_mediatest_uvloop, &close_req, media->fd, NULL);
      free(media->buf);
      media->buf = NULL;
      return;
    }

  iov = uv_buf_init(buf->base, nread);
  uv_fs_write(g_mediatest_uvloop, &media->fs_req, media->fd, &iov, 1, -1,
              mediatest_uv_recorder_write_cb);
}

static void mediatest_uv_recorder_connection_cb(void *cookie, int ret,
                                                void *obj)
{
  GET_TIMESTAMP();

  syslog(LOG_INFO, "[%s][%d] ret:%d obj:%p\n", __func__, __LINE__, ret,
         obj);
  if (!obj)
    return;

  uv_handle_set_data(obj, cookie);
  uv_read_start(obj, mediatest_uv_recorder_alloc_cb,
                mediatest_uv_recorder_read_cb);
}

static void mediatest_uv_player_write_cb(uv_write_t *req, int status)
{
  struct mediatest_data *media = uv_req_get_data((uv_req_t *)req);
  uv_buf_t iov;

  if (media->stop_flag)
    status = -1;

  if (status < 0)
    {
      syslog(LOG_ERR, "[%s][%d] Player write error: %s\n", __func__,
             __LINE__, uv_strerror(status));
      free(media->buf);
      media->buf = NULL;
      return;
    }
  else
    {
      iov = uv_buf_init(media->buf, media->size);
      int ret = uv_fs_read(g_mediatest_uvloop, &media->fs_req, media->fd,
                           &iov, 1, -1, mediatest_uv_player_read_cb);
      if (ret < 0)
        syslog(LOG_WARNING, "uv fs read ret failed\n");
    }
}

static void mediatest_uv_player_read_cb(uv_fs_t *req)
{
  struct mediatest_data *media = uv_req_get_data((uv_req_t *)req);
  uv_buf_t iov;

  if (media->stop_flag)
    req->result = -1;

  if (req->result < 0)
    {
      syslog(LOG_ERR, "[%s][%d] Player Read error: %s\n", __func__,
             __LINE__, uv_strerror(req->result));
      return;
    }
  else if (req->result == 0)
    {
      syslog(LOG_INFO, "[%s][%d] Player read to end of file\n", __func__,
             __LINE__);
      mediatest_common_stop(media);
      media->stat = PLAYER_STOPPED;
      return;
    }
  else
    {
      iov = uv_buf_init(media->buf, req->result);
      uv_req_set_data((uv_req_t *)&media->write_req, media);
      uv_write((uv_write_t *)&media->write_req,
               (uv_stream_t *)media->pipe, &iov, 1,
               mediatest_uv_player_write_cb);
    }
}

static void mediatest_uv_player_connection_cb(void *cookie, int ret,
                                              void *obj)
{
  struct mediatest_data *media = cookie;
  uv_buf_t iov;

  syslog(LOG_INFO, "[%s][%d] ret:%d obj:%p\n", __func__, __LINE__, ret,
         obj);

  if (ret < 0 || !obj) {
    syslog(LOG_ERR, "[%s][%d] pipe is closed.\n", __func__, __LINE__);
    return;
  }

  media->size = 2048;
  media->buf = malloc(2048);
  assert(media->buf);
  media->pipe = obj;

  iov = uv_buf_init(media->buf, media->size);
  uv_req_set_data((uv_req_t *)&media->fs_req, media);
  int ret_uv = uv_fs_read(g_mediatest_uvloop, &media->fs_req, media->fd,
                       &iov, 1, -1, mediatest_uv_player_read_cb);
  if (ret_uv < 0)
    syslog(LOG_WARNING, "uv fs read ret failed\n");
}

#endif

static void mediatest_takepic_callback(void *cookie, int event, int ret,
                                       const char *extra)
{
  GET_TIMESTAMP();
  struct mediatest_data *media = cookie;
  if (event == MEDIA_EVENT_COMPLETED)
    {
      media_recorder_finish_picture(media->handle);
      media->handle = NULL;
      media->extra = NULL;
    }
  syslog(LOG_INFO, "[%s][%d] stream:%s event:%s(%d) ret:%d extra:%s\n",
         __func__, __LINE__, media->stream_type,
         mediatest_event2str(event), event, ret, extra);
}

int mediatest_recorder_take_picture(struct mediatest_data *media)
{
  int ret = -EINVAL;
  char *filtername = (char *)media->stream_type;
  char *filename = media->url;
  size_t number = (size_t)media->rept;

  if (!filtername)
    return -EINVAL;

  if (!filename)
    return -EINVAL;

  ret = media_recorder_take_picture(filtername, filename, number);
  if (ret < 0)
    syslog(LOG_WARNING, "mediatest_recorder_take_picture failed\n");

  return ret;
}

int mediatest_recorder_take_picture_async(struct mediatest_data *media)
{
  char *filtername = (char *)media->stream_type;
  char *filename = media->url;
  size_t number = (size_t)media->rept;
  media->handle = media_recorder_start_picture(
      filtername, filename, number, mediatest_takepic_callback, media);
  GET_TIMESTAMP();
  if (!media->handle)
    {
      syslog(LOG_ERR, "media_recorder_start_picture error\n");
      return -EINVAL;
    }
  return 0;
}

#ifdef CONFIG_AUDIOUTILS_ALSA_LIB

void alsa_show_usages(mediatest_alsa_t *media)
{
  syslog(LOG_WARNING,
         "Usage: CMD [-u <url>] [-b <bytes_per_frame>] [-r "
         "<sample_rate>] [-c <channels>] [-w <bits_per_sample>] [-f "
         "<file>]\n");

  if (media != NULL)
    {
      free(media);
      media = NULL;
    }

  exit(0);
}

int mediatest_alsa_getopt(int argc, char *argv[],
                          mediatest_alsa_t *media)
{
  if (media == NULL)
    {
      syslog(LOG_ERR, "mediatest data malloc failed\n");
      return -1;
    }
  if (argc == 1)
    alsa_show_usages(media);

  int ch = 0;
  while ((ch = getopt(argc, argv, "u:b:r:c:w:d:f:v:h:")) != EOF)
    {
      switch (ch)
        {
        case 'u':
          media->url = optarg;
          break;
        case 'b':
          media->bytes_per_frame = atoi(optarg);
          break;
        case 'r':
          media->sample_rate = atoi(optarg);
          break;
        case 'c':
          media->channels = atoi(optarg);
          break;
        case 'w':
          media->bits_per_sample = atoi(optarg);
          break;
        case 'f':
          media->file = optarg;
          break;
        case 'd':
          media->device = optarg;
          break;
        case 'v':
          media->volume = atoi(optarg);
          break;
        case 'h':
          media->interval = atoi(optarg);
          break;
        default:
          alsa_show_usages(media);
          break;
        }
    }
  return 0;
}

int mediatest_alsa_setup(mediatest_alsa_t *media)
{
  media->handle = NULL;
  media->bytes_per_frame = 0;
  media->sample_rate = 0;
  media->complete = false;
  media->channels = 0;
  media->bits_per_sample = 0;
  media->device = "pcm0p";
  media->volume = 10;
  media->interval = 0;
  return 0;
}

static void *mediatest_alsa_thread(void *arg)
{
  mediatest_alsa_t *priv = arg;
  char *buffer;
  int rc, size;
  FILE *pcm_file;

  snd_pcm_uframes_t frames = priv->sample_rate / 1000 * 25;
  size = frames * priv->bytes_per_frame;
  buffer = (char *)malloc(size);

  pcm_file = fopen(priv->url, "rb");
  if (!pcm_file)
    {
      printf("unable to open PCM file\n");
      return 0;
    }

  while (!feof(pcm_file))
    {
      rc = fread(buffer, 1, size, pcm_file);
      if (rc == 0)
        {
          if (feof(pcm_file))
            {
              printf("End of file reached\n");
              break;
            }
          if (ferror(pcm_file))
            {
              printf("Error reading from file\n");
              break;
            }
        }

      int frames_to_write = rc / priv->bytes_per_frame;
      int offset = 0;
      int retries = 0;
      const int max_retries = 5;

      while (frames_to_write > 0)
        {
          pthread_mutex_lock(&alsapause_mutex);
          if (alsapause)
            {
              pthread_cond_wait(&alsapause_cond, &alsapause_mutex);
            }
          pthread_mutex_unlock(&alsapause_mutex);

          rc = snd_pcm_writei(priv->handle, buffer + offset,
                              frames_to_write);
          if (rc == -EAGAIN)
            {
              continue;
            }
          else if (rc == -EPIPE)
            {
              printf("underrun occurred\n");
              snd_pcm_prepare(priv->handle);
            }
          else if (rc == -ESTRPIPE)
            {
              while ((rc = snd_pcm_resume(priv->handle)) == -EAGAIN)
                {
                  sleep(1);
                }
              if (rc < 0)
                {
                  snd_pcm_prepare(priv->handle);
                }
            }
          else if (rc < 0)
            {
              printf("error from writei: %s\n", snd_strerror(rc));
              break;
            }
          else if (rc == 0)
            {
              printf("No frames were written, retrying...\n");
              if (++retries > max_retries)
                {
                  printf("Max retries reached, exiting...\n");
                  break;
                }
              continue;
            }
          else
            {
              frames_to_write -= rc;
              offset += rc * priv->bytes_per_frame;
              retries = 0;
            }
        }
    }

  fclose(pcm_file);
  free(buffer);
  snd_pcm_drain(priv->handle);
  priv->complete = true;
  syslog(LOG_INFO, "alsa thread read end and exit\n");
  return NULL;
}

int mediatest_alsa_close(mediatest_alsa_t *media)
{
  snd_pcm_close(media->handle);
  return 0;
}

int mediatest_alsa_open(mediatest_alsa_t *media)
{
  int rc;
  char *pcm_device = media->device;
  snd_pcm_t *handle;
  rc = snd_pcm_open(&handle, pcm_device, SND_PCM_STREAM_PLAYBACK, 0);
  if (rc < 0)
    {
      syslog(LOG_ERR, "unable to open pcm device: %s\n",
             snd_strerror(rc));
      return -1;
    }
  media->handle = handle;
  return 0;
}

int mediatest_alsa_volume(mediatest_alsa_t *media)
{
  if (!media->handle)
    {
      syslog(LOG_ERR, "alsa handle is null\n");
      return -1;
    }
  if (media->volume <= 0 && media->volume > 100)
    media->volume = 100;

  int rc = snd_pcm_set_volume(media->handle, media->volume);
  if (rc < 0)
    {
      printf("unable to open pcm device: %s\n", snd_strerror(rc));
      return 0;
    }
  return 0;
}

int mediatest_alsa_prepare(mediatest_alsa_t *media)
{
  int dir, rc;
  snd_pcm_t *handle = media->handle;
  snd_pcm_hw_params_t *params;
  snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

  pthread_t thread;
  unsigned int sample_rate = (unsigned int)media->sample_rate;
  int bytes_per_frame = media->bits_per_sample / 8 * media->channels;

  if (media->bits_per_sample == 16)
    {
      format = SND_PCM_FORMAT_S16_LE;
    }
  else
    {
      format = SND_PCM_FORMAT_S32_LE;
    }

  if (access(media->url, F_OK) != 0)
    {
      syslog(LOG_ERR, "file not exist\n");
      return -1;
    }
  snd_pcm_hw_params_alloca(&params);
  snd_pcm_hw_params_any(handle, params);
  snd_pcm_hw_params_set_access(handle, params,
                               SND_PCM_ACCESS_RW_INTERLEAVED);
  snd_pcm_hw_params_set_format(handle, params, format);
  snd_pcm_hw_params_set_channels(handle, params, media->channels);
  snd_pcm_hw_params_set_rate_near(handle, params, &sample_rate,
                                  &dir);

  snd_pcm_hw_params_set_period_time(handle, params, 20 * 1000, dir);
  snd_pcm_hw_params_set_periods(handle, params, 4, dir);

  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0)
    {
      syslog(LOG_ERR, "unable to set hw parameters: %s\n",
             snd_strerror(rc));
      return -1;
    }

  media->bytes_per_frame = bytes_per_frame;

  rc = pthread_create(&thread, NULL, mediatest_alsa_thread, media);
  if (rc >= 0)
    syslog(LOG_INFO, "alsa pthread_create successfully\n");

  return 0;
}

int mediatest_alsapause(int enable)
{
  pthread_mutex_lock(&alsapause_mutex);
  alsapause = enable;
  if (!alsapause)
    {
      pthread_cond_broadcast(&alsapause_cond);
    }
  pthread_mutex_unlock(&alsapause_mutex);

  syslog(LOG_INFO, "alsa  %s\n", enable ? "pause" : "resume");

  return 0;
}

#endif /* CONFIG_AUDIOUTILS_ALSA_LIB */

int mediatest_setup(struct mediatest_data *media)
{
  if (media == NULL)
    return -1;
  media->buf = NULL;
  media->rept = 1;
  media->url = NULL;
  media->fd = -1;
  media->handle = NULL;
  media->ret = 0;
  media->size = 512;
  media->type = MEDIATEST_PLAYER;
  media->mode = MODE_URL;
  media->stream_type = MEDIA_STREAM_MUSIC;
  media->thread = 0;
  media->state = PLAYER_IDLE;
  media->stat = PLAYER_IDLE;
  media->loop = 1;
  media->position = 0;
  media->volume = 5;
  media->option = NULL;
  media->complete = false;
  media->time = 3;
  media->extra = NULL;
  media->pending_stop = 0;
  media->playing = 0;
  media->uv_waiting = -1;
  media->focus_type = NULL;
  media->ex = 0;
  media->stop_flag = false;
  return 0;
}

void show_usages(struct mediatest_data *media)
{
  syslog(
      LOG_WARNING,
      "Usage: CMD [-u <url>] [-c <execcute_times>] [-s "
      "<stream_type>] [-t <type>] [-m <mode>] [-l <loop "
      "times>] [-p <position>] [-v <voice>] [-o <option>] [-h "
      "<running_time>] [-f <file>] [-r <focus>]\n"
      "\t\t-u: set player url, default NULL\n"
      "\t\t-c: set number of executions, default 1\n"
      "\t\t-s: set stream type, default Music\n"
      "\t\t-t: play type,1:player 2:recorder 3:controller 4:controllee "
      "5:focus 6:uvplayer 7:uvrecorder 8:uvfocus 9:camera "
      "10:video, default player\n"
      "\t\t-m: set player mode, 0:buffer 1: url 2:direct, default url\n"
      "\t\t-l: set player loop time, default 1\n"
      "\t\t-p: set player seek position, default 0\n"
      "\t\t-v: set player volume, default 5\n"
      "\t\t-o: set option, default NULL\n"
      "\t\t-h: set  time of running time(s), default 3\n"
      "\t\t-f: set read file, default /data/1.txt\n"
      "\t\t-r: request stream_type focus and play, default NULL\n"
      "\t\t-e: extra option\n");

  if (media != NULL && media->type < MEDIATEST_UVPLAYER)
    {
      free(media);
      media = NULL;
    }

  exit(0);
}

int mediatest_getopt(int argc, char *argv[],
                     struct mediatest_data *media)
{
  if (media == NULL)
    {
      syslog(LOG_ERR, "mediatest data malloc failed\n");
      return -1;
    }
  if (argc == 1)
    show_usages(media);

  int ch = 0;
  while ((ch = getopt(argc, argv, "u:c:s:t:m:l:p:v:o:h:f:r:e:")) != EOF)
    {
      switch (ch)
        {
        case 'u':
          media->url = optarg;
          break;
        case 'c':
          media->rept = atoi(optarg);
          break;
        case 's':
          media->stream_type = optarg;
          break;
        case 't':
          media->type = atoi(optarg);
          break;
        case 'm':
          media->mode = atoi(optarg);
          break;
        case 'l':
          media->loop = atoi(optarg);
          break;
        case 'p':
          media->position = (unsigned int)atoi(optarg);
          break;
        case 'v':
          media->volume = atof(optarg);
          break;
        case 'o':
          media->option = optarg;
          break;
        case 'h':
          media->time = atoi(optarg);
          break;
        case 'f':
          media->file = optarg;
          break;
        case 'r':
          media->focus_type = optarg;
          break;
        case 'e':
          media->ex = atoi(optarg);
          break;
        default:
          show_usages(media);
          break;
        }
    }
  if (media->url == NULL)
    {
      return -1;
    }
  return 0;
}

int mediatest_common_prepare_retry(struct mediatest_data *media,
                                   int retry)
{
  for (int i = 0; i < retry + 1; ++i)
    {
      if (i != 0)
        syslog(LOG_ERR, "prepared failed, the retry of %d\n", i);

      if (mediatest_common_prepare(media) >= 0)
        {
          time_t t0 = time(NULL);
          int count_ = 0;
          while (media->state != PLAYER_PREPARED)
            {
              usleep(4000);
              count_++;
              if (count_ > 2000)
                {
                  syslog(LOG_WARNING,
                         "mediastab event callback wait time failed\n");
                  break;
                }
            }
          time_t t1 = time(NULL);
          syslog(LOG_INFO, "the wait time is %lld\n",
                 (long long int)(t1 - t0));
          mediatest_dump();
          if (media->state == PLAYER_PREPARED && media->ret >= 0)
            {
              syslog(LOG_ERR, "mediastab prepare successed\n");
              return 0;
            }
          syslog(LOG_WARNING, "mediastab prepare callback ret failed\n");
          media->state = PLAYER_IDLE;
        }
      else
        {
          syslog(LOG_WARNING,
                 "mediastab prepare failed, prepare return failed\n");
        }
    }
  return -1;
}

#ifdef CONFIG_LIBUV_EXTENSION
void mediatest_uvasyncq_close_cb(uv_handle_t *handle)
{
  UNUSED(handle);
  uv_stop(g_mediatest_uvloop);
  handle = NULL;
}

void mediatest_uvasyncq_cb(uv_async_queue_t *asyncq, void *data)
{
  int ret;
  if (data == NULL)
    {
      syslog(LOG_ERR, "execute data is null\n");
      return;
    }
  struct mediatest_app *player_test = data;
  syslog(LOG_INFO, "test  started\n");
  ret = player_test->uv_play(player_test->priv);
  if (player_test != NULL)
    {
      free(player_test);
      player_test = NULL;
    }

  if (ret < 0)
    {
      uv_close((uv_handle_t *)&g_mediatest_uvasyncq,
               mediatest_uvasyncq_close_cb);
    }

  syslog(LOG_INFO, "test stoped\n");
}

void *mediatest_uvloop_thread(void *arg)
{
  int ret;

  g_mediatest_uvloop = malloc(sizeof(uv_loop_t));

  ret = uv_loop_init(g_mediatest_uvloop);
  if (ret < 0)
    return NULL;

  ret = uv_async_queue_init(g_mediatest_uvloop, &g_mediatest_uvasyncq,
                            mediatest_uvasyncq_cb);
  if (ret < 0)
    goto out;

  syslog(LOG_INFO, "[%s][%d] running\n", __func__, __LINE__);
  while (1)
    {
      ret = uv_run(g_mediatest_uvloop, UV_RUN_DEFAULT);
      if (ret == 0)
        break;
    }

out:
  ret = uv_loop_close(g_mediatest_uvloop);
  free(g_mediatest_uvloop);
  syslog(LOG_INFO, "[%s][%d] out:%d\n", __func__, __LINE__, ret);
  media_test_loop_living = 0;
  return NULL;
}

int mediatest_uv_player_enter(void)
{
  if (media_test_loop_living == 1)
    {
      syslog(LOG_WARNING, "mediatest loop is running\n");
      return 0;
    }
  media_test_loop_living = 1;
  pthread_attr_t attr;
  struct sched_param param;
  param.sched_priority = 100;

  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, 80 * 1024);
  pthread_attr_setschedparam(&attr, &param);
  pthread_create(&uv_thread, &attr, mediatest_uvloop_thread, NULL);
  sleep(1);
  return 0;
}

int mediatest_uv_quit(struct mediatest_data *media)
{
  if (mediatest_play_list != NULL)
    {
      test_delete_all_song_entry_of_play_list(mediatest_play_list);
      test_play_list_deinit();
    }
  free(media);
  return -1;
}

int mediatest_uv_exit(void)
{
  struct mediatest_app *player_test =
      (struct mediatest_app *)malloc(sizeof(struct mediatest_app));
  struct mediatest_data *media_exit =
      (struct mediatest_data *)malloc(sizeof(struct mediatest_data));
  player_test->priv = media_exit;
  player_test->uv_play = mediatest_uv_quit;
  sleep(1);
  uv_async_queue_send(&g_mediatest_uvasyncq, player_test);
  pthread_join(uv_thread, NULL);
  return 0;
}

int mediatest_uv_player_exec(struct mediatest_app *player_test)
{
  player_test->priv->uv_waiting = PLAYER_IDLE;
  int ret = uv_async_queue_send(&g_mediatest_uvasyncq, player_test);
  return ret;
}

int mediatest_app_init(struct mediatest_data *media)
{
  if (media == NULL)
    {
      syslog(LOG_ERR, "init media is null\n");
      return -1;
    }
  mediatest_common_open(media);
  mediatest_common_prepare(media);
  mediatest_common_start(media);
  media->stat = PLAYER_STARTED;
  return 0;
}

int mediatest_app_dur_pos(struct mediatest_data *media)
{
  if (media == NULL)
    {
      syslog(LOG_ERR, "app dur_pos media is null\n");
      return -1;
    }
  mediatest_position(media);
  mediatest_duration(media);
  return 0;
}

int mediatest_app_stop(struct mediatest_data *media)
{
  if (media->stat != PLAYER_STOPPED)
    {
      mediatest_common_stop(media);
      media->stat = PLAYER_STOPPED;
    }
  return 0;
}

int mediatest_app_reset(struct mediatest_data *media)
{
  if (media->stat != PLAYER_STOPPED)
    {
      mediatest_common_reset(media);
      media->stat = PLAYER_STOPPED;
    }
  return 0;
}

int mediatest_app_exit(struct mediatest_data *media)
{
  mediatest_common_close(media);
  media->stat = PLAYER_IDLE;
  return 0;
}

int mediatest_app_play(struct mediatest_data *media)
{
  if (media == NULL)
    {
      syslog(LOG_ERR, "app play media is null\n");
      return -1;
    }
  if (media->stat == PLAYER_PAUSED)
    {
      mediatest_common_start(media);
      media->stat = PLAYER_STARTED;
    }
  else if (media->stat == PLAYER_STOPPED || media->stat == PLAYER_IDLE)
    {
      mediatest_common_prepare(media);
      mediatest_common_start(media);
      media->stat = PLAYER_STARTED;
    }

  return 0;
}

int mediatest_app_pause(struct mediatest_data *media)
{
  if (media == NULL)
    {
      syslog(LOG_ERR, "app pause media is null\n");
      return -1;
    }
  if (media->stat == PLAYER_STARTED)
    {
      mediatest_common_pause(media);
      media->stat = PLAYER_PAUSED;
    }

  return 0;
}

int mediatest_app_playorpause(struct mediatest_data *media)
{
  if (media == NULL)
    {
      syslog(LOG_ERR, "media app playorpause media is null\n");
      return -1;
    }
  if (media->stat == PLAYER_STARTED)
    {
      mediatest_common_pause(media);
      media->stat = PLAYER_PAUSED;
    }
  else if (media->stat == PLAYER_PAUSED)
    {
      mediatest_common_start(media);
      media->stat = PLAYER_STARTED;
    }
  else
    {
      mediatest_common_prepare(media);
      mediatest_common_start(media);
      media->stat = PLAYER_STARTED;
    }
  return 0;
}

int mediatest_app_next(struct mediatest_data *media)
{
  if (media == NULL)
    {
      syslog(LOG_ERR, "app next media is null\n");
      return -1;
    }
  test_song_entry_t *test_song_entry = media->test_song_entry;

  syslog(LOG_INFO, "get next song...\n");
  test_get_next_song_entry(media->test_song_entry, &test_song_entry);
  if (!test_song_entry)
    {
      syslog(LOG_ERR, "get next song failed\n");
      test_song_entry = media->test_song_entry;
    }
  syslog(LOG_INFO, "url is %s\n", test_song_entry->song_url);
  media->url = test_song_entry->song_url;
  media->test_song_entry = test_song_entry;
  mediatest_app_stop(media);
  mediatest_app_play(media);
  media->stat = PLAYER_STARTED;
  return 0;
}

int mediatest_app_prev(struct mediatest_data *media)
{
  if (media == NULL)
    {
      syslog(LOG_ERR, "app prev media is null\n");
      return -1;
    }
  test_song_entry_t *test_song_entry = media->test_song_entry;
  syslog(LOG_INFO, "get prev song...\n");
  test_get_prev_song_entry(media->test_song_entry, &test_song_entry);
  if (!test_song_entry)
    {
      syslog(LOG_ERR, "get prev song failed\n");
      test_song_entry = media->test_song_entry;
    }
  media->url = test_song_entry->song_url;
  media->test_song_entry = test_song_entry;
  mediatest_app_stop(media);
  mediatest_app_play(media);
  media->stat = PLAYER_STARTED;
  return 0;
}
#endif

timer_t mediatest_start_timer(mediatest_alarm_cb cb, long time_val)
{
  long time_sec = time_val / 1000;
  long time_nsec = time_val % 1000 * 1000 * 1000;
  timer_t timerid;
  struct sigevent evp;
  memset(&evp, 0, sizeof(struct sigevent));
  struct sigaction act;
  memset(&act, 0, sizeof(act));
  syslog(LOG_INFO, "time_sec is %ld, time_usec is %ld\n", time_sec,
         time_nsec);

  act.sa_handler = cb;
  act.sa_flags = 0;

  sigemptyset(&act.sa_mask);

  if (sigaction(SIGUSR1, &act, NULL) == -1)
    {
      syslog(LOG_ERR, "fail to sigaction");
      exit(-1);
    }
  evp.sigev_signo = SIGUSR1;
  evp.sigev_notify = SIGEV_SIGNAL;

  if (timer_create(CLOCK_REALTIME, &evp, &timerid) == -1)
    {
      syslog(LOG_ERR, "%s", "fail to timer_create");
      exit(-1);
    }

  struct itimerspec it;
  it.it_interval.tv_sec = time_sec;
  it.it_interval.tv_nsec = time_nsec;
  it.it_value.tv_sec = time_sec;
  it.it_value.tv_nsec = time_nsec;

  if (timer_settime(timerid, 0, &it, NULL) == -1)
    {
      syslog(LOG_ERR, "fail to timer_settime");
      exit(-1);
    }
  return timerid;
}

int mediatest_close_timer(timer_t timeid)
{
  int ret = timer_delete(timeid);
  return ret;
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
      syslog(LOG_ERR, "LOAD FILE OPEN FAILED\n");
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

void mediatest_load_play_list(struct mediatest_data *media)
{
  syslog(LOG_INFO,
         "**************************************************************"
         "**************\n");
  syslog(LOG_INFO, "parse play list\n");

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

  test_song_entry_t *test_song_entry = NULL;

  mediatest_load_local_all_songs(media->file, mediatest_play_list);
  test_song_entry = container_of(mediatest_play_list->song_head.next,
                                 test_song_entry_t, song_list);

  media->url = test_song_entry->song_url;
  media->test_song_entry = test_song_entry;

  test_print_play_list();
  return;
}
