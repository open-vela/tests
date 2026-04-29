/****************************************************************************
 * apps/tests/velatest/scenario_testsuites/media/util/media_uv.c
 *
 * Copyright (C) 2024 Xiaomi Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 *implied. See the License for the specific language governing
 *permissions and limitations under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <errno.h>
#include <media_api.h>
#include <pthread.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <uv.h>
#include <uv_async_queue.h>
#include <uv_ext.h>

#include "media_test_uv_utils.h"

#define API_TIMEOUT 3000000000

static void mediatest_uvplayer_read_cb(uv_fs_t *req);
static void mediatest_uvplayer_start_cb(void *cookie, int ret);
static void mediatest_uvplayer_focus_abandon_cb(void *cookie, int ret);

static void mediatest_trace_suggest(mediatest_info_s *ctx, int suggest)
{
  struct timespec ts;

  if (ctx->suggest_count >= TEST_MAX_NB_SUGGESTS)
    return; /* too much suggests. */

  TEST_UV_MEDIA_LOG(LOG_INFO, "[%s][%s] suggestion:%d\n", __func__,
                    ctx->stream_type, suggest);
  clock_gettime(CLOCK_MONOTONIC, &ts);
  ctx->suggests[ctx->suggest_count].timestamp =
      ts.tv_sec * 1000000ll + ts.tv_nsec / 1000;
  ctx->suggests[ctx->suggest_count].suggest = suggest;
  ctx->suggest_count++;
}

static void mediatest_uvplayer_focus_abandon(mediatest_info_s *ctx)
{
  int ret;
  if (!ctx)
    return;
  mediatest_uv_focus_s *focus_info = ctx->focus_info;
  if (focus_info)
    {
      ret = media_uv_focus_abandon(focus_info->handle,
                                   mediatest_uvplayer_focus_abandon_cb);
      if (ret >= 0)
        {
          focus_info->stream = NULL;
          ctx->focus_info = NULL;
        }
    }
}

static void mediatest_uvplayer_event_callback(void *cookie, int event,
                                              int ret, const char *data)
{
  mediatest_info_s *media = (mediatest_info_s *)cookie;
  media->last_event = event;
  media->result = ret;

  if (event == MEDIA_EVENT_STARTED)
    {
      sem_post(&media->wait_start_sem);
      if (media->test_event == MEDIA_EVENT_STARTED)
        media->test_event_count++;
      TEST_UV_MEDIA_LOG(LOG_INFO, "wait_start_sem:%p\n",
                        &media->wait_start_sem);
    }
  else if (event == MEDIA_EVENT_STOPPED)
    {
      sem_post(&media->wait_stop_sem);
      if (media->test_event == MEDIA_EVENT_STARTED)
        media->test_event_count++;
    }
  else if (event == MEDIA_EVENT_COMPLETED)
    {
      sem_post(&media->wait_complete_sem);
      if (media->test_event == MEDIA_EVENT_STARTED)
        media->test_event_count++;
      mediatest_uvplayer_focus_abandon(media);
    }
  else if (event == MEDIA_EVENT_PREPARED)
    {
      sem_post(&media->wait_prepare_sem);
      if (media->test_event == MEDIA_EVENT_STARTED)
        media->test_event_count++;
    }
  else if (event == MEDIA_EVENT_PAUSED)
    {
      sem_post(&media->wait_pause_sem);
      if (media->test_event == MEDIA_EVENT_STARTED)
        media->test_event_count++;
    }
  else if (event == MEDIA_EVENT_SEEKED)
    {
      sem_post(&media->wait_seek_sem);
      if (media->test_event == MEDIA_EVENT_STARTED)
        media->test_event_count++;
    }
  else
    {
    }

  TEST_UV_MEDIA_LOG(LOG_INFO, "cookie=%p, event=%d, ret=%d, data=%p\n",
                   Cookie, event, ret, data);
}

static void media_uv_player_suggest_cb(int suggest, void *cookie)
{
  mediatest_uv_focus_s *priv =Cookie;
  mediatest_info_s *player_state = priv->stream;
  bool suggest_active = false;

  if (!player_state)
    {
      TEST_UV_MEDIA_LOG(LOG_INFO, "focus:%p suggest:%d canceled\n", priv,
                        suggest);
      return;
    }

  TEST_UV_MEDIA_LOG(LOG_INFO, "%s:%p focus:%p %p suggest:%d\n",
                    player_state->stream_type, player_state->handle,
                    priv, priv->handle, suggest);
  mediatest_trace_suggest(player_state, suggest);
  switch (suggest)
    {
    case MEDIA_FOCUS_PLAY:
      suggest_active = true;
      media_uv_player_set_volume(player_state->handle, 1.0, NULL, NULL);
      media_uv_player_start(player_state->handle, priv->on_play,
                            priv->on_play_cookie);
      break;

    case MEDIA_FOCUS_STOP:
      suggest_active = false;
      media_uv_player_stop(player_state->handle, NULL, NULL);
      break;

    case MEDIA_FOCUS_PAUSE:
      suggest_active = false;
      media_uv_player_pause(player_state->handle, NULL, NULL);
      break;

    case MEDIA_FOCUS_PLAY_BUT_SILENT:
      suggest_active = true;
      media_uv_player_set_volume(player_state->handle, 0.0, NULL, NULL);
      media_uv_player_start(player_state->handle, NULL, NULL);
      break;

    case MEDIA_FOCUS_PLAY_WITH_DUCK:
      suggest_active = true;
      media_uv_player_set_volume(player_state->handle, 0.1, NULL, NULL);
      media_uv_player_start(player_state->handle, NULL, NULL);
      break;

    case MEDIA_FOCUS_PLAY_WITH_KEEP:
      break;
    }

  if (priv->on_play)
    {
      if (!suggest_active) /* Notify user if focus request failed. */
        priv->on_play(priv->on_play_cookie, -EPERM);

      priv->on_play = NULL;
      priv->on_play_cookie = NULL;
    }
}

int mediatest_uvplayer_request_focus(mediatest_info_s *player_state)
{
  mediatest_uv_focus_s *priv;

  priv = zalloc(sizeof(mediatest_uv_focus_s));
  if (!priv)
    return -ENOMEM;

  priv->stream = player_state;
  priv->on_play = mediatest_uvplayer_start_cb;
  priv->on_play_cookie = player_state->cookie;
  priv->handle =
      media_uv_focus_request(player_state->uv_loop, player_state->focus,
                             media_uv_player_suggest_cb, priv);
  if (!priv->handle)
    {
      free(priv);
      return -ENOMEM;
    }

  player_state->focus_info = priv;
  TEST_UV_MEDIA_LOG(LOG_INFO, "%s:%p %s:%p %p\n",
                    player_state->stream_type, priv->stream,
                    player_state->focus, priv, priv->handle);
  return 0;
}

/*
static void test_player_uvasyncq_close_cb(uv_handle_t *handle)
{
    TEST_UV_MEDIA_LOG(LOG_INFO, "Bye-Bye!\n");
    uv_stop(&g_ms_uvloop);
}
*/

static void mediatest_uvplayer_write_cb(uv_write_t *req, int status)
{
  mediatest_info_s *media = uv_req_get_data((uv_req_t *)req);
  uv_buf_t iov;

  if (status < 0)
    {
      TEST_UV_MEDIA_LOG(LOG_ERR, "Player write error: %s\n",
                        uv_strerror(status));
      free(media->buf);
      media->buf = NULL;
      return;
    }
  else
    {
      iov = uv_buf_init(media->buf, media->size);
      uv_fs_read(media->uv_loop, &media->fs_req, media->fd, &iov, 1, -1,
                 mediatest_uvplayer_read_cb);
    }
}

static void mediatest_uvplayer_read_cb(uv_fs_t *req)
{
  mediatest_info_s *media = uv_req_get_data((uv_req_t *)req);
  uv_fs_t close_req;
  uv_buf_t iov;

  if (req->result < 0)
    {
      TEST_UV_MEDIA_LOG(LOG_ERR, "Player Read error: %s\n",
                        uv_strerror(req->result));
      free(media->buf);
      media->buf = NULL;
      return;
    }
  else if (req->result == 0)
    {
      TEST_UV_MEDIA_LOG(LOG_INFO, "Player read to end of file\n");
      uv_fs_close(media->uv_loop, &close_req, media->fd, NULL);
    }
  else
    {
      iov = uv_buf_init(media->buf, req->result);
      uv_req_set_data((uv_req_t *)&media->write_req, media);
      uv_write((uv_write_t *)&media->write_req,
               (uv_stream_t *)media->pipe, &iov, 1,
               mediatest_uvplayer_write_cb);
    }
}

static void mediatest_uvplayer_callback(void *cookie, char *op, int ret)
{
  if (!cookie)
    {
      TEST_UV_MEDIA_LOG(LOG_ERR, "cookie is NULL\n");
      return;
    }
  mediatest_info_s *media =Cookie;

  if (!media->wait_operation)
    media->wait_operation = "";

  TEST_UV_MEDIA_LOG(LOG_INFO, "op:%s wop:%s stream:%s ret:%d\n", op,
                    media->wait_operation, media->stream_type, ret);
  if (media->wait_operation != NULL &&
      strcmp(media->wait_operation, op) == 0)
    {
      media->last_ret = ret;
      sem_post(&media->sem);
    }
}

static void mediatest_uvplayer_open_cb(void *cookie, int ret)
{
  mediatest_uvplayer_callback(cookie, "open", ret);
}

static void mediatest_uvplayer_connection_cb(void *cookie, int ret,
                                             void *obj)
{
  mediatest_info_s *media =Cookie;
  uv_buf_t iov;

  TEST_UV_MEDIA_LOG(LOG_INFO, "ret:%d obj:%p\n", ret, obj);
  media->size = 1024;
  media->buf = malloc(1024);
  assert(media->buf);
  media->pipe = obj;

  iov = uv_buf_init(media->buf, media->size);
  uv_req_set_data((uv_req_t *)&media->fs_req, media);
  uv_fs_read(media->uv_loop, &media->fs_req, media->fd, &iov, 1, -1,
             mediatest_uvplayer_read_cb);
}

static void mediatest_uvplayer_focus_abandon_cb(void *cookie, int ret)
{
  free(cookie);
}

static void mediatest_uvplayer_prepare_cb(void *cookie, int ret)
{
  mediatest_uvplayer_callback(cookie, "prepare", ret);
}

static void mediatest_uvplayer_start_cb(void *cookie, int ret)
{
  mediatest_uvplayer_callback(cookie, "start", ret);
}

static void mediatest_uvplayer_stop_cb(void *cookie, int ret)
{
  mediatest_uvplayer_callback(cookie, "stop", ret);
}

static void mediatest_uvplayer_close_cb(void *cookie, int ret)
{
  if (!cookie)
    {
      TEST_UV_MEDIA_LOG(LOG_ERR, "cookie is NULL\n");
      return;
    }
  mediatest_info_s *media =Cookie;
  mediatest_uvplayer_callback(cookie, "close", ret);
  media->handle = NULL;
}

static void mediatest_uvplayer_pause_cb(void *cookie, int ret)
{
  mediatest_uvplayer_callback(cookie, "pause", ret);
}

static void mediatest_uvplayer_seek_cb(void *cookie, int ret)
{
  mediatest_uvplayer_callback(cookie, "seek", ret);
}

static void mediatest_uvplayer_set_volume_cb(void *cookie, int ret)
{
  mediatest_uvplayer_callback(cookie, "set_volume", ret);
}

static void mediatest_uvplayer_get_volume_cb(void *cookie, int ret,
                                             float val)
{
  if (!cookie)
    {
      TEST_UV_MEDIA_LOG(LOG_ERR, "cookie is NULL\n");
      return;
    }

  mediatest_info_s *media =Cookie;
  media->ret_volume = val;
  TEST_UV_MEDIA_LOG(LOG_INFO, "op:%s stream:%s ret:%d val:%f\n",
                    "get_volume", media->stream_type, ret, val);
  mediatest_uvplayer_callback(cookie, "get_volume", ret);
}

static void mediatest_uvplayer_get_position_cb(void *cookie, int ret,
                                               unsigned int val)
{
  if (!cookie)
    {
      TEST_UV_MEDIA_LOG(LOG_ERR, "cookie is NULL\n");
      return;
    }

  mediatest_info_s *media =Cookie;
  media->ret_position = val;
  TEST_UV_MEDIA_LOG(LOG_INFO, "op:%s stream:%s ret:%d val:%u\n",
                    "get_position", media->stream_type, ret, val);
  mediatest_uvplayer_callback(cookie, "get_position", ret);
}

static void mediatest_uvplayer_get_duration_cb(void *cookie, int ret,
                                               unsigned int val)
{
  if (!cookie)
    {
      TEST_UV_MEDIA_LOG(LOG_ERR, "cookie is NULL\n");
      return;
    }

  mediatest_info_s *media =Cookie;
  media->ret_duration = val;
  TEST_UV_MEDIA_LOG(LOG_INFO, "op:%s stream:%s ret:%d val:%u\n",
                    "get_duration", media->stream_type, ret, val);
  mediatest_uvplayer_callback(cookie, "get_duration", ret);
}

static void mediatest_uvplayer_get_playing_cb(void *cookie, int ret,
                                              int val)
{
  if (!cookie)
    {
      TEST_UV_MEDIA_LOG(LOG_ERR, "cookie is NULL\n");
      return;
    }

  mediatest_info_s *media =Cookie;
  media->playing = val;
  TEST_UV_MEDIA_LOG(LOG_INFO, "op:%s stream:%s ret:%d val:%d\n",
                    "get_playing", media->stream_type, ret, val);
  mediatest_uvplayer_callback(cookie, "get_playing", ret);
}

static void mediatest_uvplayer_get_latency_cb(void *cookie, int ret,
                                              unsigned int val)
{
  if (!cookie)
    {
      TEST_UV_MEDIA_LOG(LOG_ERR, "cookie is NULL\n");
      return;
    }

  mediatest_info_s *media =Cookie;
  media->latency = val;
  TEST_UV_MEDIA_LOG(LOG_INFO, "op:%s stream:%s ret:%d val:%d\n",
                    "get_latency", media->stream_type, ret, val);
  mediatest_uvplayer_callback(cookie, "get_latency", ret);
}

static void mediatest_uvplayer_set_looping_cb(void *cookie, int ret)
{
  mediatest_uvplayer_callback(cookie, "set_looping", ret);
}

static void mediatest_uvplayer_set_property_cb(void *cookie, int ret)
{
  mediatest_uvplayer_callback(cookie, "set_property", ret);
}

static void mediatest_uvplayer_get_property_cb(void *cookie, int ret,
                                               const char *value)
{
  if (!cookie)
    {
      TEST_UV_MEDIA_LOG(LOG_ERR, "cookie is NULL\n");
      return;
    }

  mediatest_info_s *media =Cookie;
  media->ret_value = value;
  TEST_UV_MEDIA_LOG(LOG_INFO, "op:%s stream:%s ret:%d val:%s\n",
                    "get_property", media->stream_type, ret, value);
  mediatest_uvplayer_callback(cookie, "get_property", ret);
}

static void mediatest_uvplayer_query_cb(void *cookie, int ret, void *obj)
{
  if (!cookie)
    {
      TEST_UV_MEDIA_LOG(LOG_ERR, "cookie is NULL\n");
      return;
    }

  mediatest_info_s *media =Cookie;
  media->ret_query_obj = obj;
  mediatest_uvplayer_callback(cookie, "query", ret);
}

static void mediatest_uvplayer_reset_cb(void *cookie, int ret)
{
  mediatest_uvplayer_callback(cookie, "reset", ret);
}

int mediatest_uvplayer_open(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvplayer_open\n");
  media_uv_callback cb = mediatest_uvplayer_open_cb;
  if (player_state->is_cb)
    cb = NULL;
  void *handle = media_uv_player_open(player_state->uv_loop,
                                      player_state->stream_type, cb,
                                      player_state->cookie);
  if (handle)
    player_state->handle = handle;
  return handle ? 0 : -1;
}

int mediatest_uvplayer_prepare(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvplayer_prepare\n");
  int ret = -1;
  media_uv_callback cb = mediatest_uvplayer_prepare_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_player_prepare(
      player_state->handle, player_state->url, player_state->option,
      mediatest_uvplayer_connection_cb, cb, player_state->cookie);
  return ret;
}

int mediatest_uvplayer_start(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvplayer_start\n");
  int ret = -1;
  media_uv_callback cb = mediatest_uvplayer_start_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_player_start(player_state->handle, cb,
                              player_state->cookie);
  return ret;
}

int mediatest_uvplayer_start_auto(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvplayer_start\n");
  int ret = -1;
  media_uv_callback cb = mediatest_uvplayer_start_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_player_start_auto(player_state->handle,
                                   player_state->focus, cb,
                                   player_state->cookie);
  return ret;
}

int mediatest_uvplayer_stop(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvplayer_stop\n");
  int ret = -1;
  media_uv_callback cb = mediatest_uvplayer_stop_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_player_stop(player_state->handle, cb,
                             player_state->cookie);
  mediatest_uvplayer_focus_abandon(player_state);
  return ret;
}

int mediatest_uvplayer_close(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvplayer_close\n");
  int ret = -1;
  media_uv_callback cb = mediatest_uvplayer_close_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_player_close(player_state->handle,
                              player_state->pending_stop, cb);
  mediatest_uvplayer_focus_abandon(player_state);
  return ret;
}

int mediatest_uvplayer_listen(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvplayer_listen\n");
  int ret = -1;
  media_event_callback cb = mediatest_uvplayer_event_callback;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_player_listen(player_state->handle, cb);
  return ret;
}

int mediatest_uvplayer_pause(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvplayer_pause\n");
  int ret = -1;
  media_uv_callback cb = mediatest_uvplayer_pause_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_player_pause(player_state->handle, cb,
                              player_state->cookie);
  mediatest_uvplayer_focus_abandon(player_state);
  return ret;
}

int mediatest_uvplayer_seek(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvplayer_seek\n");
  int ret = -1;
  media_uv_callback cb = mediatest_uvplayer_seek_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret =
      media_uv_player_seek(player_state->handle, player_state->position,
                           cb, player_state->cookie);
  return ret;
}

int mediatest_uvplayer_set_volume(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvplayer_set_volume\n");
  int ret = -1;
  media_uv_callback cb = mediatest_uvplayer_set_volume_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_player_set_volume(player_state->handle,
                                   player_state->volume, cb,
                                   player_state->cookie);
  return ret;
}

int mediatest_uvplayer_get_volume(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvplayer_get_volume\n");
  int ret = -1;
  media_uv_float_callback cb = mediatest_uvplayer_get_volume_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_player_get_volume(player_state->handle, cb,
                                   player_state->cookie);
  return ret;
}

int mediatest_uvplayer_get_position(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvplayer_get_position\n");
  int ret = -1;
  media_uv_unsigned_callback cb = mediatest_uvplayer_get_position_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_player_get_position(player_state->handle, cb,
                                     player_state->cookie);
  return ret;
}

int mediatest_uvplayer_get_duration(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvplayer_get_duration\n");
  int ret = -1;
  media_uv_unsigned_callback cb = mediatest_uvplayer_get_duration_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_player_get_duration(player_state->handle, cb,
                                     player_state->cookie);
  return ret;
}

int mediatest_uvplayer_get_playing(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvplayer_get_playing\n");
  int ret = -1;
  media_uv_int_callback cb = mediatest_uvplayer_get_playing_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_player_get_playing(player_state->handle, cb,
                                    player_state->cookie);
  return ret;
}

int mediatest_uvplayer_get_latency(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvplayer_get_latency\n");
  int ret = -1;
  media_uv_unsigned_callback cb = mediatest_uvplayer_get_latency_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_player_get_latency(player_state->handle, cb,
                                    player_state->cookie);
  return ret;
}

int mediatest_uvplayer_set_looping(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvplayer_set_looping\n");
  int ret = -1;
  media_uv_callback cb = mediatest_uvplayer_set_looping_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_player_set_looping(player_state->handle,
                                    player_state->loop, cb,
                                    player_state->cookie);
  return ret;
}

int mediatest_uvplayer_set_property(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvplayer_set_property\n");
  int ret = -1;
  media_uv_callback cb = mediatest_uvplayer_set_property_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_player_set_property(
      player_state->handle, player_state->target, player_state->key,
      player_state->value, cb, player_state->cookie);
  return ret;
}

int mediatest_uvplayer_get_property(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvplayer_get_property\n");
  int ret = -1;
  media_uv_string_callback cb = mediatest_uvplayer_get_property_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_player_get_property(
      player_state->handle, player_state->target, player_state->key, cb,
      player_state->cookie);
  return ret;
}

int mediatest_uvplayer_query(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvplayer_query\n");
  int ret = -1;
  media_uv_object_callback cb = mediatest_uvplayer_query_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_player_query(player_state->handle, cb,
                              player_state->cookie);
  return ret;
}

int mediatest_uvplayer_reset(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvplayer_reset\n");
  int ret = -1;
  media_uv_callback cb = mediatest_uvplayer_reset_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_player_reset(player_state->handle, cb,
                              player_state->cookie);
  mediatest_uvplayer_focus_abandon(player_state);
  return ret;
}

// static void mediatest_uvasyncq_cb(uv_async_queue_t *asyncq, void
// *data)
// {
//   int ret __attribute__((unused));
//   struct player_test_s *player_test = data;
//   syslog(LOG_INFO, "test  started\n");
//   ret = player_test->uv_play(player_test->priv);
//   if (player_test != NULL)
//     {
//       free(player_test);
//       player_test = NULL;
//     }
//   syslog(LOG_INFO, "test stoped\n");
// }

static void *mediatest_uvplayer_uvloop_thread_func(void *arg)
{
  int ret;
  struct mediatest_uv_data *media = arg;

  // ret = posix_memalign((void **)&(media->uv_loop), 4,
  // sizeof(uv_loop_t));
  media->uv_loop = (uv_loop_t *)malloc(sizeof(uv_loop_t));
  for (int i = 0; i < UV_APPS; i++)
    {
      media->mediatest_info[i].uv_loop = media->uv_loop;
      // media->mediatest_info[i].uvasyncq = &media->uvasyncq;
    }
  ret = uv_loop_init(media->uv_loop);
  if (ret < 0)
    return NULL;

  // ret = uv_async_queue_init(media->uv_loop, &media->uvasyncq,
  //                           mediatest_uvasyncq_cb);

  TEST_UV_MEDIA_LOG(LOG_INFO, "running\n");

  pthread_mutex_lock(&media->mutex);
  media->uv_running = true;
  pthread_cond_signal(&media->cond);
  pthread_mutex_unlock(&media->mutex);

  while (media->uv_running)
    {
      ret = uv_run(
          media->uv_loop,
          UV_RUN_ONCE);
      if (ret == UV_EAGAIN || ret == 0)
        {
          usleep(1000); // Short sleep to avoid high CPU usage
        }
    }

  pthread_mutex_lock(&media->mutex);
  media->uv_running = false;
  pthread_cond_signal(&media->cond);
  pthread_mutex_unlock(&media->mutex);
  TEST_UV_MEDIA_LOG(LOG_INFO, "uvloop stop run:%d\n", ret);

  return NULL;
}

int mediatest_uvplayer_data_init(struct mediatest_uv_data *media)
{
  if (media == NULL)
    return -1;
  for (int i = 0; i < UV_APPS; i++)
    {
      sem_init(&media->mediatest_info[i].sem, 0, 0);
      sem_init(&media->mediatest_info[i].wait_prepare_sem, 0, 0);
      sem_init(&media->mediatest_info[i].wait_start_sem, 0, 0);
      sem_init(&media->mediatest_info[i].wait_pause_sem, 0, 0);
      sem_init(&media->mediatest_info[i].wait_stop_sem, 0, 0);
      sem_init(&media->mediatest_info[i].wait_seek_sem, 0, 0);
      sem_init(&media->mediatest_info[i].wait_complete_sem, 0, 0);

      media->mediatest_info[i].wait_operation = NULL;
      media->mediatest_info[i].buf = NULL;
      media->mediatest_info[i].fd = -1;
      media->mediatest_info[i].handle = NULL;
      media->mediatest_info[i].size = 512;
      media->mediatest_info[i].stream_type = "Music";
      media->mediatest_info[i].ret_position = 0;
      media->mediatest_info[i].ret_duration = 0;
      media->mediatest_info[i].ret_volume = 1;
      media->mediatest_info[i].latency = 0;
      media->mediatest_info[i].ret_value = NULL;
      media->mediatest_info[i].ret_query_obj = NULL;
      media->mediatest_info[i].playing = 0;
      media->mediatest_info[i].last_event = MEDIA_EVENT_NOP;
      media->mediatest_info[i].result = -1;
      media->mediatest_info[i].last_ret = -1;
      media->mediatest_info[i].test_event_count = 0;
      media->mediatest_info[i].test_event = MEDIA_EVENT_NOP;
      media->mediatest_info[i].is_cb = false;
    }
  media->uv_running = false;
  media->thread = 0;
  pthread_cond_init(&media->cond, NULL);
  pthread_mutex_init(&media->mutex, NULL);
  return 0;
}

static int
mediatest_uvplayer_uvloop_thread_init(struct mediatest_uv_data *media)
{
  pthread_t thread;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, 80 * 1024);
  pthread_create(&thread, &attr, mediatest_uvplayer_uvloop_thread_func,
                 media);
  media->thread = thread;
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvplayer_uvloop_thread_init\n");
  return 0;
}

int mediatest_uvplayer_setup(FAR void **state)
{
  struct mediatest_uv_data *player = (struct mediatest_uv_data *)calloc(
      1, sizeof(struct mediatest_uv_data));
  mediatest_uvplayer_data_init(player);
  if (!player)
    return -ENOMEM;
  mediatest_uvplayer_uvloop_thread_init(player);

  pthread_mutex_lock(&player->mutex);
  while (!player->uv_running)
    {
      pthread_cond_wait(&player->cond, &player->mutex);
    }
  pthread_mutex_unlock(&player->mutex);

  *state = player;
  TEST_UV_MEDIA_LOG(LOG_INFO, "media uv setup\n");

  return 0;
}

int mediatest_uvplayer_teardown(FAR void **state)
{
  struct mediatest_uv_data *player = *state;

  if (player)
    {
      for (int i = 0; i < UV_APPS; i++)
        {
          if (player->mediatest_info[i].thread)
            {
              pthread_join(player->mediatest_info[i].thread, NULL);
            }
        }

      TEST_UV_MEDIA_LOG(LOG_INFO, "before close lock\n");
      pthread_mutex_lock(&player->mutex);
      player->uv_running = false;
      pthread_cond_signal(&player->cond);
      pthread_mutex_unlock(&player->mutex);
      for (int i = 0; i < UV_APPS; i++)
        {
          sem_destroy(&player->mediatest_info[i].sem);
          sem_destroy(&player->mediatest_info[i].wait_prepare_sem);
          sem_destroy(&player->mediatest_info[i].wait_start_sem);
          sem_destroy(&player->mediatest_info[i].wait_pause_sem);
          sem_destroy(&player->mediatest_info[i].wait_stop_sem);
          sem_destroy(&player->mediatest_info[i].wait_seek_sem);
          sem_destroy(&player->mediatest_info[i].wait_complete_sem);
        }
      pthread_cond_destroy(&player->cond);
      pthread_mutex_destroy(&player->mutex);
      uv_stop(player->uv_loop);
      uv_loop_close(player->uv_loop);

      TEST_UV_MEDIA_LOG(LOG_INFO, "before join\n");
      pthread_join(player->thread, NULL);
      if (player)
        {
          free(player->uv_loop);
          free(player);
        }

      *state = NULL;
    }

  TEST_UV_MEDIA_LOG(LOG_INFO, "media uv teardown\n");
  return 0;
}

void mediatest_uvplayer_ret_wait(mediatest_info_s *player_state,
                                 int timeout)
{
  mediatest_uvplayer_ret_wait_l(player_state, timeout, 1);
}

void mediatest_uvplayer_ret_wait_l(mediatest_info_s *player_state,
                                   int timeout, int flag)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "sem_timedwait: wait ret begin sem:%p\n",
                    &player_state->sem);
  if (timeout == 0)
    timeout = API_TIMEOUT / 1000000;

  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_nsec += timeout % 1000 * 1000000;

  ts.tv_sec += ts.tv_nsec / 1000000000 + timeout / 1000;
  ts.tv_nsec %= 1000000000;

  if (sem_timedwait(&player_state->sem, &ts) == -1)
    {
      if (errno == ETIMEDOUT)
        {
          TEST_UV_MEDIA_LOG(LOG_INFO,
                            "sem_timedwait: wait ret timeout\n");
          if (flag)
            mediatest_uv_result_deal(player_state, -1);
        }
      perror("sem_timedwait: wait error\n");
      if (flag)
        mediatest_uv_result_deal(player_state, -1);
    }
  TEST_UV_MEDIA_LOG(LOG_INFO, "sem_timedwait: wait ret assertion\n");
  player_state->wait_operation = NULL;
}

void mediatest_uvplayer_reset_sem(mediatest_info_s *media,
                                  const char *op)
{
  media->wait_operation = op;
  sem_destroy(&media->sem);
  sem_init(&media->sem, 0, 0);
  TEST_UV_MEDIA_LOG(LOG_INFO, "wop:%s sem:%p\n", op, &media->sem);
}

static void mediatest_uvplayer_wait(mediatest_info_s *player_state,
                                    int event, sem_t *sem, int timeout,
                                    char *operate)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "sem_timedwait: wait %s begin sem:%p\n",
                    operate, sem);
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_nsec += timeout % 1000 * 1000000;
  ;
  ts.tv_sec += ts.tv_nsec / 1000000000 + timeout / 1000;
  ts.tv_nsec %= 1000000000;

  if (sem_timedwait(sem, &ts) == -1)
    {
      if (errno == ETIMEDOUT)
        {
          TEST_UV_MEDIA_LOG(LOG_INFO, "sem_timedwait: wait %s timeout\n",
                            operate);
          mediatest_uv_result_deal(player_state, -1);
        }
      perror("sem_timedwait: wait error\n");
      mediatest_uv_result_deal(player_state, -1);
    }
  TEST_UV_MEDIA_LOG(LOG_INFO, "sem_timedwait: wait %s assertion\n",
                    operate);
  if (player_state->last_event != event || player_state->result < 0)
    {
      mediatest_uv_result_deal(player_state, -1);
    }
  TEST_UV_MEDIA_LOG(LOG_INFO, "sem_timedwait: wait %s success\n",
                    operate);
}

static void mediatest_uvplayer_wait_error(mediatest_info_s *player_state,
                                          int event, sem_t *sem,
                                          int timeout, char *operate)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "sem_timedwait: wait %s begin sem:%p\n",
                    operate, sem);
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_nsec += timeout % 1000 * 1000000;
  ;
  ts.tv_sec += ts.tv_nsec / 1000000000 + timeout / 1000;
  ts.tv_nsec %= 1000000000;

  if (sem_timedwait(sem, &ts) == -1)
    {
      if (errno == ETIMEDOUT)
        {
          TEST_UV_MEDIA_LOG(LOG_INFO, "sem_timedwait: wait %s timeout\n",
                            operate);
          mediatest_uv_result_deal(player_state, -1);
          ;
        }
      perror("sem_timedwait: wait error\n");
      mediatest_uv_result_deal(player_state, -1);
    }
  TEST_UV_MEDIA_LOG(LOG_INFO, "sem_timedwait: wait %s assertion\n",
                    operate);
  if (player_state->last_event != event || player_state->result >= 0)
    {
      mediatest_uv_result_deal(player_state, -1);
    }
  TEST_UV_MEDIA_LOG(LOG_INFO, "sem_timedwait: wait %s success\n",
                    operate);
}

void mediatest_uvplayer_event_wait(mediatest_info_s *player_state,
                                   int event, int timeout)
{
  if (timeout == 0)
    timeout = API_TIMEOUT / 1000000;

  if (event == MEDIA_EVENT_STARTED)
    {
      mediatest_uvplayer_wait(player_state, event,
                              &player_state->wait_start_sem, timeout,
                              "start");
    }
  else if (event == MEDIA_EVENT_STOPPED)
    {
      mediatest_uvplayer_wait(player_state, event,
                              &player_state->wait_stop_sem, timeout,
                              "stop");
    }
  else if (event == MEDIA_EVENT_COMPLETED)
    {
      mediatest_uvplayer_wait(player_state, event,
                              &player_state->wait_complete_sem, timeout,
                              "complete");
    }
  else if (event == MEDIA_EVENT_PREPARED)
    {
      mediatest_uvplayer_wait(player_state, event,
                              &player_state->wait_prepare_sem, timeout,
                              "prepare");
    }
  else if (event == MEDIA_EVENT_PAUSED)
    {
      mediatest_uvplayer_wait(player_state, event,
                              &player_state->wait_pause_sem, timeout,
                              "pause");
    }
  else if (event == MEDIA_EVENT_SEEKED)
    {
      mediatest_uvplayer_wait(player_state, event,
                              &player_state->wait_seek_sem, timeout,
                              "seek");
    }
  else
    {
    }
}

void mediatest_uvplayer_event_wait_error(mediatest_info_s *player_state,
                                         int event, int timeout)
{
  if (timeout == 0)
    timeout = API_TIMEOUT / 1000000;

  if (event == MEDIA_EVENT_STARTED)
    {
      mediatest_uvplayer_wait_error(player_state, event,
                                    &player_state->wait_start_sem,
                                    timeout, "start");
    }
  else if (event == MEDIA_EVENT_STOPPED)
    {
      mediatest_uvplayer_wait_error(player_state, event,
                                    &player_state->wait_stop_sem,
                                    timeout, "stop");
    }
  else if (event == MEDIA_EVENT_COMPLETED)
    {
      mediatest_uvplayer_wait_error(player_state, event,
                                    &player_state->wait_complete_sem,
                                    timeout, "complete");
    }
  else if (event == MEDIA_EVENT_PREPARED)
    {
      mediatest_uvplayer_wait_error(player_state, event,
                                    &player_state->wait_prepare_sem,
                                    timeout, "prepare");
    }
  else if (event == MEDIA_EVENT_PAUSED)
    {
      mediatest_uvplayer_wait_error(player_state, event,
                                    &player_state->wait_pause_sem,
                                    timeout, "pause");
    }
  else if (event == MEDIA_EVENT_SEEKED)
    {
      mediatest_uvplayer_wait_error(player_state, event,
                                    &player_state->wait_seek_sem,
                                    timeout, "seek");
    }
  else
    {
    }
}

void mediatest_uvplayer_reset_event_sem(void *cookie, int event)
{
  mediatest_info_s *media = (mediatest_info_s *)cookie;

  if (event == MEDIA_EVENT_STARTED)
    {
      sem_destroy(&media->wait_start_sem);
      sem_init(&media->wait_start_sem, 0, 0);
    }
  else if (event == MEDIA_EVENT_STOPPED)
    {
      sem_destroy(&media->wait_stop_sem);
      sem_init(&media->wait_stop_sem, 0, 0);
    }
  else if (event == MEDIA_EVENT_COMPLETED)
    {
      sem_destroy(&media->wait_complete_sem);
      sem_init(&media->wait_complete_sem, 0, 0);
    }
  else if (event == MEDIA_EVENT_PREPARED)
    {
      sem_destroy(&media->wait_prepare_sem);
      sem_init(&media->wait_prepare_sem, 0, 0);
    }
  else if (event == MEDIA_EVENT_PAUSED)
    {
      sem_destroy(&media->wait_pause_sem);
      sem_init(&media->wait_pause_sem, 0, 0);
    }
  else if (event == MEDIA_EVENT_SEEKED)
    {
      sem_destroy(&media->wait_seek_sem);
      sem_init(&media->wait_seek_sem, 0, 0);
    }
}

int mediatest_uv_result_deal(mediatest_info_s *player_state, int result)
{
  if (result < 0)
    {
      if (player_state->handle)
        {
          mediatest_uvplayer_reset_sem(player_state, "close");
          mediatest_uvplayer_stop(player_state);
          mediatest_uvplayer_close(player_state);
          mediatest_uvplayer_ret_wait_l(player_state, 0, 0);
        }
      player_state->error = 1;
    }
  return 0;
}

void *media_utils_uvplayer_thread(void *arg)
{
  mediatest_info_s *player_state = (mediatest_info_s *)arg;
  mediatest_uvplayer_open(player_state);
  mediatest_uvplayer_listen(player_state);
  mediatest_uvplayer_prepare(player_state);

  mediatest_uvplayer_reset_sem(player_state, "get_duration");
  mediatest_uvplayer_get_duration(player_state);
  mediatest_uvplayer_ret_wait(player_state, 0);

  mediatest_uvplayer_reset_event_sem(player_state, MEDIA_EVENT_STARTED);
  mediatest_uvplayer_request_focus(player_state);
  mediatest_uvplayer_event_wait(player_state, MEDIA_EVENT_START, 500);

  usleep(player_state->ret_duration * 1000);
  mediatest_uvplayer_reset_sem(player_state, "close");
  mediatest_uvplayer_stop(player_state);
  mediatest_uvplayer_close(player_state);
  mediatest_uvplayer_ret_wait_l(player_state, 0, 0);
  return NULL;
}

static void media_uv_recorder_suggest_cb(int suggest, void *cookie)
{
  mediatest_uv_focus_s *priv =Cookie;
  mediatest_info_s *recorder = priv->stream;
  bool suggest_active = false;

  if (!recorder)
    {
      TEST_UV_MEDIA_LOG(LOG_ERR, "focus:%p suggest:%d canceled\n", priv,
                        suggest);
      return;
    }

  TEST_UV_MEDIA_LOG(LOG_INFO, "%s:%p focus:%p %p suggest:%d\n",
                    recorder->stream_type, recorder, priv, priv->handle,
                    suggest);
  mediatest_trace_suggest(recorder, suggest);
  switch (suggest)
    {
    case MEDIA_FOCUS_PLAY:
    case MEDIA_FOCUS_PLAY_BUT_SILENT:
    case MEDIA_FOCUS_PLAY_WITH_DUCK:
      suggest_active = true;
      media_uv_recorder_start(recorder->handle, priv->on_play,
                              priv->on_play_cookie);
      break;

    case MEDIA_FOCUS_STOP:
      suggest_active = false;
      media_uv_recorder_stop(recorder->handle, NULL, NULL);
      break;

    case MEDIA_FOCUS_PAUSE:
      suggest_active = false;
      media_uv_recorder_pause(recorder->handle, NULL, NULL);
      break;

    case MEDIA_FOCUS_PLAY_WITH_KEEP:
      break;
    }

  if (priv->on_play)
    {
      if (!suggest_active) /* Notify user if focus request failed. */
        priv->on_play(priv->on_play_cookie, -EPERM);

      priv->on_play = NULL;
      priv->on_play_cookie = NULL;
    }
}

int mediatest_uvrecorder_request_focus(mediatest_info_s *player_state)
{
  mediatest_uv_focus_s *priv;

  priv = zalloc(sizeof(mediatest_uv_focus_s));
  if (!priv)
    return -ENOMEM;

  priv->stream = player_state;
  priv->on_play = mediatest_uvplayer_start_cb;
  priv->on_play_cookie = player_state->cookie;
  priv->handle =
      media_uv_focus_request(player_state->uv_loop, player_state->focus,
                             media_uv_recorder_suggest_cb, priv);
  if (!priv->handle)
    {
      free(priv);
      return -ENOMEM;
    }

  player_state->focus_info = priv;
  TEST_UV_MEDIA_LOG(LOG_INFO, "%s:%p %s:%p %p\n",
                    player_state->stream_type, priv->stream,
                    player_state->focus, priv, priv->handle);
  return 0;
}

static void mediatest_uvrecorder_alloc_cb(uv_handle_t *handle,
                                          size_t suggested_size,
                                          uv_buf_t *buf)
{
  mediatest_info_s *record = uv_handle_get_data(handle);

  if (record->buf)
    {
      buf->base = NULL;
      buf->len = 0;
      return;
    }

  buf->base = malloc(2048);
  // assert_non_null(buf->base);
  buf->len = 2048;
}

static void mediatest_uvrecorder_write_cb(uv_fs_t *req)
{
  mediatest_info_s *record = uv_req_get_data((uv_req_t *)req);

  if (req->result < 0)
    {
      syslog(LOG_ERR, "[%s] Write error %s\n", __func__,
             uv_err_name(req->result));
      free(record->buf);
      record->buf = NULL;
      return;
    }

  free(record->buf);
  record->buf = NULL;
  uv_fs_req_cleanup(req);
}

static void mediatest_uvrecorder_read_cb(uv_stream_t *stream,
                                         ssize_t nread,
                                         const uv_buf_t *buf)
{
  mediatest_info_s *record = uv_handle_get_data((uv_handle_t *)stream);
  uv_fs_t close_req;
  uv_buf_t iov;

  if (nread == UV_ENOBUFS)
    {
      usleep(1000);
      return;
    }
  record->buf = buf->base;
  uv_req_set_data((uv_req_t *)&record->fs_req, record);
  if (nread < 0)
    {
      if (nread != UV_EOF)
        printf("[%s][%d] Recorder read error %s\n", __func__, __LINE__,
               uv_err_name(nread));
      uv_fs_close(record->uv_loop, &close_req, record->fd, NULL);
      free(record->buf);
      record->buf = NULL;
      return;
    }

  iov = uv_buf_init(buf->base, nread);
  uv_fs_write(record->uv_loop, &record->fs_req, record->fd, &iov, 1, -1,
              mediatest_uvrecorder_write_cb);
}

static void mediatest_uvrecorder_connection_cb(void *cookie, int ret,
                                               void *obj)
{
  mediatest_info_s *record =Cookie;

  syslog(LOG_INFO, "[%s] ret:%d obj:%p record:%p \n", __func__, ret, obj,
         record);
  if (!obj)
    return;

  uv_handle_set_data(obj,Cookie);
  uv_read_start(obj, mediatest_uvrecorder_alloc_cb,
                mediatest_uvrecorder_read_cb);
}

int mediatest_uvrecorder_open(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvrecorder_open\n");
  media_uv_callback cb = mediatest_uvplayer_open_cb;
  if (player_state->is_cb)
    cb = NULL;
  void *handle = media_uv_recorder_open(player_state->uv_loop,
                                        player_state->stream_type, cb,
                                        player_state->cookie);
  if (handle)
    player_state->handle = handle;
  return handle ? 0 : -1;
}

int mediatest_uvrecorder_prepare(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvrecorder_prepare\n");
  int ret = -1;
  media_uv_callback cb = mediatest_uvplayer_prepare_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_recorder_prepare(
      player_state->handle, player_state->url, player_state->option,
      mediatest_uvrecorder_connection_cb, cb, player_state->cookie);
  return ret;
}

int mediatest_uvrecorder_start(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvrecorder_start\n");
  int ret = -1;
  media_uv_callback cb = mediatest_uvplayer_start_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_recorder_start(player_state->handle, cb,
                                player_state->cookie);
  return ret;
}

int mediatest_uvrecorder_start_auto(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvrecorder_start\n");
  int ret = -1;
  media_uv_callback cb = mediatest_uvplayer_start_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_recorder_start_auto(player_state->handle,
                                     player_state->focus, cb,
                                     player_state->cookie);
  return ret;
}

int mediatest_uvrecorder_stop(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvrecorder_stop\n");
  int ret = -1;
  media_uv_callback cb = mediatest_uvplayer_stop_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_recorder_stop(player_state->handle, cb,
                               player_state->cookie);
  mediatest_uvplayer_focus_abandon(player_state);
  return ret;
}

int mediatest_uvrecorder_close(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvrecorder_close\n");
  int ret = -1;
  media_uv_callback cb = mediatest_uvplayer_close_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_recorder_close(player_state->handle, cb);
  mediatest_uvplayer_focus_abandon(player_state);
  return ret;
}

int mediatest_uvrecorder_listen(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvrecorder_listen\n");
  int ret = -1;
  media_event_callback cb = mediatest_uvplayer_event_callback;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_recorder_listen(player_state->handle, cb);
  return ret;
}

int mediatest_uvrecorder_pause(mediatest_info_s *player_state)
{
  TEST_UV_MEDIA_LOG(LOG_INFO, "mediatest_uvrecorder_pause\n");
  int ret = -1;
  media_uv_callback cb = mediatest_uvplayer_pause_cb;
  if (player_state->is_cb)
    cb = NULL;
  ret = media_uv_recorder_pause(player_state->handle, cb,
                                player_state->cookie);
  mediatest_uvplayer_focus_abandon(player_state);
  return ret;
}

void *media_utils_uvrecorder_thread(void *arg)
{
  mediatest_info_s *player_state = (mediatest_info_s *)arg;
  mediatest_uvrecorder_open(player_state);
  mediatest_uvrecorder_listen(player_state);
  mediatest_uvrecorder_prepare(player_state);

  mediatest_uvplayer_reset_event_sem(player_state, MEDIA_EVENT_STARTED);
  mediatest_uvrecorder_request_focus(player_state);
  mediatest_uvplayer_event_wait(player_state, MEDIA_EVENT_START, 500);

  usleep(player_state->ret_duration * 1000);
  mediatest_uvplayer_reset_sem(player_state, "close");
  mediatest_uvrecorder_stop(player_state);
  mediatest_uvrecorder_close(player_state);
  mediatest_uvplayer_ret_wait_l(player_state, 0, 0);
  return NULL;
}