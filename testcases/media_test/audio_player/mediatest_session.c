/**************************************************************
 *
 * Copyright(c) 2022, Beijing Xiaomi Mobile Software Co., Ltd.
 * All Rights Reserved.
 *
 *************************************************************/

#include "mediatest_session.h"
#include "media_common_test.h"

#include <stdlib.h>

#include "sys/param.h"
#include <netpacket/rpmsg.h>
#include <sys/types.h>

#ifndef CONFIG_MEDIA
#if defined(CONFIG_BES_CP_AUDIO_OPUS) ||                                \
    defined(CONFIG_BES_CP_AUDIO_SILK)
#include "nuttx_audio_codec.h"
#define MULTI_SESSION_BES_AUDIO
#endif
#endif

#define MULTI_SESSION_COUNT_MAX 10

#define LOG_D(fmt, ...)                                                 \
  MEDIATEST_DEBUG(multi_session, fmt, ##__VA_ARGS__)
#define LOG_I(fmt, ...) MEDIATEST_INFO(multi_session, fmt, ##__VA_ARGS__)
#define LOG_W(fmt, ...)                                                 \
  MEDIATEST_WARNING(multi_session, fmt, ##__VA_ARGS__)
#define LOG_E(fmt, ...) MEDIATEST_ERR(multi_session, fmt, ##__VA_ARGS__)

typedef struct
{
  uint32_t command;
  void *handle;
  char *stream;
  char *focus;
  void *user_data;
  char *url;
  uint32_t read;
  uv_pipe_t *pipe;
  multi_session_notify_t cb;
} multi_session_t;

typedef struct
{
  bool request;
  void *handle;
  char *stream;
  void *user_data;
  multi_session_notify_t cb;
} multi_session_audio_focus_t;

typedef enum
{
  MULTI_SESSION_TYPE_CONTROL = 0,
  MULTI_SESSION_TYPE_EVENT
} multi_session_type_t;

typedef struct
{
  bool use;
  void *handle;
  multi_session_avrcp_cb_t event_cb;
} multi_session_avrcp_t;

typedef void (*session_cb)(multi_session_control_t *control);

static uv_loop_t *media_loop = NULL;

static multi_session_t multi_session[MULTI_SESSION_COUNT_MAX] = {0};

static void multi_session_async_send(multi_session_type_t type,
                                     void *data, uint16_t data_len);

static void
multi_session_control_cmd_open(multi_session_control_t *control);
static void
multi_session_control_cmd_play(multi_session_control_t *control);
static void multi_session_control_cmd_recorder_begin(
    multi_session_control_t *control);
static void multi_session_control_cmd_recorder_close(
    multi_session_control_t *control);
static void multi_session_control_cmd_recorder_read(
    multi_session_control_t *control);

#ifdef CONFIG_MEDIA
static multi_session_avrcp_t avrcp_event[MULTI_SESSION_COUNT_MAX] = {0};

static multi_session_audio_focus_t audio_focus[MULTI_SESSION_COUNT_MAX] =
    {0};

static void multi_session_do_notify(void *handle, int event, int result,
                                    const char *extra);

static void
multi_session_control_cmd_prepare(multi_session_control_t *control);
static void
multi_session_control_cmd_start(multi_session_control_t *control);
static void
multi_session_control_cmd_pause(multi_session_control_t *control);
static void
multi_session_control_cmd_stop(multi_session_control_t *control);
static void
multi_session_control_cmd_reset(multi_session_control_t *control);
static void
multi_session_control_cmd_close(multi_session_control_t *control);
static void multi_session_control_cmd_set_graph_volume(
    multi_session_control_t *control);
static void multi_session_control_cmd_get_graph_volume(
    multi_session_control_t *control);
static void
multi_session_control_cmd_set_loop(multi_session_control_t *control);
static void
multi_session_control_cmd_set_seek(multi_session_control_t *control);
static void
multi_session_control_cmd_get_position(multi_session_control_t *control);
static void
multi_session_control_cmd_get_duration(multi_session_control_t *control);
static void multi_session_control_cmd_get_play_state(
    multi_session_control_t *control);
static void
multi_session_control_cmd_get_all(multi_session_control_t *control);
static void multi_session_control_cmd_set_audio_mode(
    multi_session_control_t *control);
static void multi_session_control_cmd_set_mute_mode(
    multi_session_control_t *control);
static void multi_session_control_cmd_get_mute_mode(
    multi_session_control_t *control);
static void multi_session_control_cmd_set_stream_volume(
    multi_session_control_t *control);
static void multi_session_control_cmd_get_stream_volume(
    multi_session_control_t *control);
static void multi_session_control_cmd_inc_stream_volume(
    multi_session_control_t *control);
static void multi_session_control_cmd_dec_stream_volume(
    multi_session_control_t *control);
static void multi_session_control_cmd_play_next_url(
    multi_session_control_t *control);
static void multi_session_control_cmd_recorder_open(
    multi_session_control_t *control);
static void multi_session_control_cmd_recorder_prepare(
    multi_session_control_t *control);
static void multi_session_control_cmd_recorder_start(
    multi_session_control_t *control);
static void multi_session_control_cmd_recorder_pause(
    multi_session_control_t *control);
static void multi_session_control_cmd_recorder_stop(
    multi_session_control_t *control);
static void multi_session_control_cmd_recorder_reset(
    multi_session_control_t *control);
static void multi_session_control_cmd_recorder_get_prop(
    multi_session_control_t *control);
static void
multi_session_control_cmd_set_sco_use(multi_session_control_t *control);
static void
multi_session_control_cmd_set_mic_mode(multi_session_control_t *control);
static void multi_session_control_cmd_update_user_data(
    multi_session_control_t *control);
static void multi_session_control_cmd_focus_request(
    multi_session_control_t *control);
static void multi_session_control_cmd_focus_abandon(
    multi_session_control_t *control);
static void
multi_session_control_cmd_write_start(multi_session_control_t *control);
#else
#ifdef MULTI_SESSION_BES_AUDIO
typedef struct
{
  uint8_t type;
  nuttx_audio_codec_func cb;
  void *user_data;
} bes_audio_t;

static bes_audio_t *g_bes_audio = NULL;

static uv_async_queue_t async_queue = {0};

static void async_queue_cb(uv_async_queue_t *queue_async, void *data)
{
  multi_session_event_t *event = (multi_session_event_t *)data;

  multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &event,
                           sizeof(event));
  free(event);
}

static unsigned int bes_audio_codec_func(unsigned char *buffer,
                                         unsigned int bytes)
{
  if (g_bes_audio == NULL)
    {
      return 1;
    }

  uint8_t *read_data = calloc(1, bytes);
  if (read_data)
    {
      memcpy(read_data, buffer, bytes);
    }

  multi_session_event_t *event =
      calloc(1, sizeof(multi_session_event_t));

  event->user_data = g_bes_audio->user_data;
  event->handle = g_bes_audio;
  event->status = bytes > 0 ? 0 : -1;
  event->command = MULTI_SESSION_EVENT_CMD_RECORDER_READ;
  event->recorder_read.data = read_data;
  event->recorder_read.len = read_data ? bytes : 0;

  uv_async_queue_send(&async_queue, event);
  return 0;
}
#endif /* MULTI_SESSION_BES_AUDIO */
#endif /* CONFIG_MEDIA */

static const struct
{
  uint8_t cmd;
  session_cb cb;
} multi_session_control_data[] = {
    {MULTI_SESSION_CONTROL_CMD_OPEN, multi_session_control_cmd_open},
    {MULTI_SESSION_CONTROL_CMD_PLAY, multi_session_control_cmd_play},
    {MULTI_SESSION_CONTROL_CMD_RECORDER_BEGIN,
     multi_session_control_cmd_recorder_begin},
    {MULTI_SESSION_CONTROL_CMD_RECORDER_CLOSE,
     multi_session_control_cmd_recorder_close},
    {MULTI_SESSION_CONTROL_CMD_RECORDER_READ,
     multi_session_control_cmd_recorder_read},
#ifdef CONFIG_MEDIA
    {MULTI_SESSION_CONTROL_CMD_PREPARE,
     multi_session_control_cmd_prepare},
    {MULTI_SESSION_CONTROL_CMD_START, multi_session_control_cmd_start},
    {MULTI_SESSION_CONTROL_CMD_PAUSE, multi_session_control_cmd_pause},
    {MULTI_SESSION_CONTROL_CMD_STOP, multi_session_control_cmd_stop},
    {MULTI_SESSION_CONTROL_CMD_RESET, multi_session_control_cmd_reset},
    {MULTI_SESSION_CONTROL_CMD_CLOSE, multi_session_control_cmd_close},
    {MULTI_SESSION_CONTROL_CMD_SET_GRAPH_VOLUME,
     multi_session_control_cmd_set_graph_volume},
    {MULTI_SESSION_CONTROL_CMD_GET_GRAPH_VOLUME,
     multi_session_control_cmd_get_graph_volume},
    {MULTI_SESSION_CONTROL_CMD_WRITE_START,
     multi_session_control_cmd_write_start},
    {MULTI_SESSION_CONTROL_CMD_SET_LOOP,
     multi_session_control_cmd_set_loop},
    {MULTI_SESSION_CONTROL_CMD_SET_SEEK,
     multi_session_control_cmd_set_seek},
    {MULTI_SESSION_CONTROL_CMD_GET_POSITION,
     multi_session_control_cmd_get_position},
    {MULTI_SESSION_CONTROL_CMD_GET_DURATION,
     multi_session_control_cmd_get_duration},
    {MULTI_SESSION_CONTROL_CMD_GET_PLAY_STATE,
     multi_session_control_cmd_get_play_state},
    {MULTI_SESSION_CONTROL_CMD_GET_ALL,
     multi_session_control_cmd_get_all},
    {MULTI_SESSION_CONTROL_CMD_SET_AUDIO_MODE,
     multi_session_control_cmd_set_audio_mode},
    {MULTI_SESSION_CONTROL_CMD_SET_MUTE_MODE,
     multi_session_control_cmd_set_mute_mode},
    {MULTI_SESSION_CONTROL_CMD_GET_MUTE_MODE,
     multi_session_control_cmd_get_mute_mode},
    {MULTI_SESSION_CONTROL_CMD_SET_STREAM_VOLUME,
     multi_session_control_cmd_set_stream_volume},
    {MULTI_SESSION_CONTROL_CMD_GET_STREAM_VOLUME,
     multi_session_control_cmd_get_stream_volume},
    {MULTI_SESSION_CONTROL_CMD_INC_STREAM_VOLUME,
     multi_session_control_cmd_inc_stream_volume},
    {MULTI_SESSION_CONTROL_CMD_DEC_STREAM_VOLUME,
     multi_session_control_cmd_dec_stream_volume},
    {MULTI_SESSION_CONTROL_CMD_RECORDER_OPEN,
     multi_session_control_cmd_recorder_open},
    {MULTI_SESSION_CONTROL_CMD_RECORDER_PREPARE,
     multi_session_control_cmd_recorder_prepare},
    {MULTI_SESSION_CONTROL_CMD_RECORDER_START,
     multi_session_control_cmd_recorder_start},
    {MULTI_SESSION_CONTROL_CMD_RECORDER_STOP,
     multi_session_control_cmd_recorder_stop},
    {MULTI_SESSION_CONTROL_CMD_RECORDER_RESET,
     multi_session_control_cmd_recorder_reset},
    {MULTI_SESSION_CONTROL_CMD_RECORDER_PAUSE,
     multi_session_control_cmd_recorder_pause},
    {MULTI_SESSION_CONTROL_CMD_RECORDER_GET_PROP,
     multi_session_control_cmd_recorder_get_prop},
    {MULTI_SESSION_CONTROL_CMD_SET_SCO_USE,
     multi_session_control_cmd_set_sco_use},
    {MULTI_SESSION_CONTROL_CMD_SET_MIC_MODE,
     multi_session_control_cmd_set_mic_mode},
    {MULTI_SESSION_CONTROL_CMD_PLAY_NEXT,
     multi_session_control_cmd_play_next_url},
    {MULTI_SESSION_CONTROL_CMD_LIST_PLAY_NEXT,
     multi_session_control_cmd_play_next_url},
    {MULTI_SESSION_CONTROL_CMD_UPDATE_USER_DATA,
     multi_session_control_cmd_update_user_data},
    {MULTI_SESSION_CONTROL_CMD_FOCUS_REQUEST,
     multi_session_control_cmd_focus_request},
    {MULTI_SESSION_CONTROL_CMD_FOCUS_ABANDON,
     multi_session_control_cmd_focus_abandon}
#endif
};

static uint32_t multi_session_get_index_by_handle(void *handle)
{
  for (uint32_t i = 0; i < MULTI_SESSION_COUNT_MAX; i++)
    {
      if (handle == multi_session[i].handle)
        {
          return i;
        }
    }

  return 0xFFFFFFFF;
}

#ifdef CONFIG_MEDIA
static void multi_session_update_local_url(uint32_t index, char *url)
{
  if (url)
    {
      char *ptr = strdup(url);
      if (ptr)
        {
          if (multi_session[index].url)
            {
              free(multi_session[index].url);
            }
          multi_session[index].url = ptr;
        }
    }
}

static void multi_session_control_cmd_update_user_data(
    multi_session_control_t *control)
{
  uint32_t index = multi_session_get_index_by_handle(control->handle);

  void *old = multi_session[index].user_data;
  multi_session[index].user_data = control->user_data;
  LOG_I("%s 0x%p to 0x%p\n", multi_session[index].stream, old,
        control->user_data);
}

static uint32_t audio_focus_index_get_by_handle(void *handle)
{
  for (uint32_t i = 0; i < nitems(audio_focus); i++)
    {
      if (audio_focus[i].handle == handle)
        {
          return i;
        }
    }
  return 0xFFFFFFFF;
}

static void media_focus_close_cb(void *cookie, int ret)
{
  uint32_t index = *(uint32_t *)cookie;
  LOG_I("focus close cb %p %" PRIu32 " %d\n", cookie, index, ret);
  if (index == 0xFFFFFFFF)
    {
      LOG_I("unknown index\n");
      return;
    }

  audio_focus[index].handle = 0;
  audio_focus[index].request = false;
  audio_focus[index].stream = NULL;
  audio_focus[index].cb = NULL;
  audio_focus[index].user_data = NULL;

  free(cookie);
}

static void media_focus_cb(int play_status, void *data)
{
  LOG_I("media focus cb status %d data %p\n", play_status, data);
  uint32_t index = *(uint32_t *)data;
  if (index == 0xFFFFFFFF)
    {
      LOG_I("unknown index\n");
      return;
    }

  multi_session_event_t event = {.status = 0,
                                 .handle = audio_focus[index].handle,
                                 .user_data =
                                     audio_focus[index].user_data,
                                 .focus.cb = audio_focus[index].cb,
                                 .focus.suggest_action = play_status};

  if (audio_focus[index].request == true)
    {
      audio_focus[index].request = false;
      event.command = MULTI_SESSION_EVENT_CMD_FOCUS_REQUEST;
      if (play_status == MEDIA_FOCUS_STOP)
        {
          event.status = -1;
          event.handle = NULL;
          multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &event,
                                   sizeof(multi_session_event_t));
          media_uv_focus_abandon(audio_focus[index].handle,
                                 media_focus_close_cb);
          return;
        }
    }
  else
    {
      event.command = MULTI_SESSION_EVENT_CMD_FOCUS_CHANGE;
    }

  multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &event,
                           sizeof(multi_session_event_t));
}

static void
multi_session_control_cmd_focus_request(multi_session_control_t *control)
{
  LOG_I("stream %s\n", control->focus.stream);
  uint32_t *index = calloc(1, sizeof(uint32_t));
  *index = audio_focus_index_get_by_handle(NULL);
  if (*index != 0xFFFFFFFF)
    {
      void *handle = media_uv_focus_request(multi_session_loop(),
                                            control->focus.stream,
                                            media_focus_cb, index);
      if (handle)
        {
          LOG_I("request done %p %p %" PRIu32 ", wait async cb\n",
                handle, index, *index);
          audio_focus[*index].handle = handle;
          audio_focus[*index].cb = control->focus.cb;
          audio_focus[*index].user_data = control->user_data;
          audio_focus[*index].stream = control->focus.stream;
          audio_focus[*index].request = true;
          return;
        }
      else
        {
          LOG_E("request fail\n");
        }
    }

  free(index);

  multi_session_event_t event = {
      .user_data = control->user_data,
      .handle = NULL,
      .status = -1,
      .command = MULTI_SESSION_EVENT_CMD_FOCUS_REQUEST,
      .focus.cb = control->focus.cb,
      .focus.suggest_action = MEDIA_FOCUS_STOP};

  multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &event,
                           sizeof(event));
}

static void write_buffer_done_cb(uv_write_t *req, int status)
{
  if (status != 0)
    {
      LOG_E("write fail %d\n", status);
    }

  void *handle = (void *)uv_req_get_data((uv_req_t *)req);
  uint32_t index = multi_session_get_index_by_handle(handle);
  if (index == 0xFFFFFFFF)
    {
      LOG_E("unknown index\n");
      return;
    }

  free(req);

  multi_session_event_t event = {
      .user_data = multi_session[index].user_data,
      .handle = multi_session[index].handle,
      .status = status,
      .command = MULTI_SESSION_EVENT_CMD_PLAYER_WRITE};

  multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &event,
                           sizeof(event));
}

static void
multi_session_control_cmd_write_start(multi_session_control_t *control)
{
  uint32_t index = multi_session_get_index_by_handle(control->handle);
  if (index == 0xFFFFFFFF)
    {
      LOG_E("unknown index\n");
      return;
    }

  if (control->command == MULTI_SESSION_CONTROL_CMD_CLOSE ||
      control->command == MULTI_SESSION_CONTROL_CMD_STOP ||
      control->command == MULTI_SESSION_CONTROL_CMD_RESET)
    {
      return;
    }

  // LOG_I("%s handle %p data %p len %" PRIu32 "\n",
  //       multi_session[index].stream, multi_session[index].handle,
  //       control->write_buffer.data, control->write_buffer.len);

  if (multi_session[index].pipe == NULL)
    {
      LOG_E("pipe NULL");
      return;
    }

  uv_write_t *write = calloc(1, sizeof(uv_write_t));
  if (write == NULL)
    {
      LOG_E("calloc fail\n");
      return;
    }

  uv_buf_t buf = uv_buf_init((char *)control->write_buffer.data,
                             control->write_buffer.len);
  uv_req_set_data((uv_req_t *)write, multi_session[index].handle);
  uv_write(write, (uv_stream_t *)multi_session[index].pipe, &buf, 1,
           write_buffer_done_cb);
}

static void media_player_prepare_connect_cb(void *cookie, int ret,
                                            void *obj)
{
  if (cookie == NULL || ret < 0 || obj == NULL)
    {
      LOG_E("cookie ret %p %d\n", cookie, ret);
      return;
    }

  uint32_t index = *(uint32_t *)cookie;
  multi_session[index].pipe = (uv_pipe_t *)obj;

  LOG_I("index %" PRIu32 " cookie %p obj %p ret %d handle %p\n", index,
        cookie, obj, ret, multi_session[index].handle);

  multi_session_event_t event = {
      .user_data = multi_session[index].user_data,
      .handle = multi_session[index].handle,
      .status = 0,
      .command = MULTI_SESSION_EVENT_CMD_PLAYER_WRITE};

  multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &event,
                           sizeof(event));
}

static void
multi_session_control_cmd_focus_abandon(multi_session_control_t *control)
{
  uint32_t index = audio_focus_index_get_by_handle(control->handle);
  LOG_I("stream %s %p %" PRIu32 "\n", audio_focus[index].stream,
        control->handle, index);
  if (index == 0xFFFFFFFF)
    {
      LOG_E("control handle not match\n");
      return;
    }

  media_uv_focus_abandon(control->handle, media_focus_close_cb);
}
#endif /* CONFIG_MEDIA */

static void multi_session_play_notify(multi_session_control_t *control,
                                      void *handle, int status)
{
  LOG_I("handle status %p %d\n", handle, status);
  multi_session_event_t event = {.user_data = control->user_data,
                                 .handle = handle,
                                 .status = status,
                                 .command = MULTI_SESSION_EVENT_CMD_PLAY,
                                 .play.cb = control->play.cb};
  multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &event,
                           sizeof(event));
}

static void multi_session_open_notify(multi_session_control_t *control,
                                      void *handle, int status)
{
  LOG_I("handle status %p %d\n", handle, status);
  multi_session_event_t event = {.user_data = control->user_data,
                                 .handle = handle,
                                 .status = status,
                                 .command = MULTI_SESSION_EVENT_CMD_OPEN,
                                 .open.cb = control->open.cb};
  multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &event,
                           sizeof(event));
}

#ifdef CONFIG_MEDIA
static void media_close_cb(void *cookie, int ret)
{
  if (cookie == NULL)
    {
      LOG_E("cookie NULL\n");
      return;
    }

  uint32_t index = *(uint32_t *)cookie;
  LOG_I("%s index %" PRIu32 " ret %d %p\n", multi_session[index].stream,
        index, ret, multi_session[index].handle);

  free(cookie);

  if (multi_session[index].url)
    {
      free(multi_session[index].url);
    }

  multi_session[index].command = 0;
  multi_session[index].handle = NULL;
  multi_session[index].stream = NULL;
  multi_session[index].focus = NULL;
  multi_session[index].user_data = NULL;
  multi_session[index].cb = NULL;
  multi_session[index].url = NULL;
  multi_session[index].pipe = NULL;
  multi_session[index].read = 0;
}

static void media_open_cb(void *cookie, int ret)
{
  uint32_t index = *(uint32_t *)cookie;
  LOG_I("index %" PRIu32 " %p ret %d\n", index, cookie, ret);

  multi_session_control_t control = {.user_data =
                                         multi_session[index].user_data};

  void *handle = ret < 0 ? NULL : multi_session[index].handle;
  if (multi_session[index].command == MULTI_SESSION_CONTROL_CMD_OPEN)
    {
      control.open.cb = multi_session[index].cb;
      multi_session_open_notify(&control, handle, ret < 0 ? -1 : 0);
    }
  else
    {
      control.play.cb = multi_session[index].cb;
      multi_session_play_notify(&control, handle, ret < 0 ? -1 : 0);
    }

  if (ret < 0)
    {
      if (multi_session[index].handle)
        {
          media_uv_player_close(multi_session[index].handle, 0,
                                media_close_cb);
        }
    }
}

static void media_vela_event_callback(void *cookie, int event, int ret,
                                      const char *extra)
{
  uint32_t index = *(uint32_t *)cookie;
  if (index == 0xFFFFFFFF || index >= MULTI_SESSION_COUNT_MAX)
    {
      LOG_E("unknown index %" PRIu32 " %p %d %d\n", index, cookie, event,
            ret);
      return;
    }

  LOG_I("index %" PRIu32 " %p event %d ret %d %s %p\n", index, cookie,
        event, ret, multi_session[index].stream,
        multi_session[index].handle);

  multi_session_event_t session_event = {
      .handle = multi_session[index].handle,
      .user_data = multi_session[index].user_data,
      .status = (ret) < (0) ? (-1) : (0)};

  multi_session_do_notify(multi_session[index].handle, event, ret,
                          extra);

  switch (event)
    {
    case MEDIA_EVENT_NOP:
      session_event.command = MULTI_SESSION_EVENT_CMD_ERROR;
      break;
    case MEDIA_EVENT_PREPARED:
      session_event.command = MULTI_SESSION_EVENT_CMD_PREPARE;
      break;
    case MEDIA_EVENT_STARTED:
      session_event.command = MULTI_SESSION_EVENT_CMD_START;
      break;
    case MEDIA_EVENT_PAUSED:
      session_event.command = MULTI_SESSION_EVENT_CMD_PAUSE;
      break;
    case MEDIA_EVENT_STOPPED:
      session_event.command = MULTI_SESSION_EVENT_CMD_STOP;
      break;
    case MEDIA_EVENT_COMPLETED:
      session_event.command = MULTI_SESSION_EVENT_CMD_COMPLETE;
      break;
    case MEDIA_EVENT_SEEKED:
      session_event.command = MULTI_SESSION_EVENT_CMD_SEEK;
      break;
    default:
      LOG_E("unknown event\n");
      return;
    }

  multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &session_event,
                           sizeof(session_event));
}

static void media_player_with_focus(void *cookie, int ret)
{
  if (cookie == NULL)
    {
      LOG_E("cookie NULL\n");
      return;
    }

  uint32_t index = *(uint32_t *)cookie;
  LOG_I("cookie index ret %p %" PRIu32 " %d\n", cookie, index, ret);

  if (ret == -1)
    {
      LOG_I("focus stop\n");
      media_vela_event_callback(cookie, MEDIA_EVENT_STOPPED, 0, NULL);
    }

  free(cookie);
}

#endif

static void
multi_session_control_cmd_play(multi_session_control_t *control)
{
#ifdef CONFIG_MEDIA
  LOG_I("play start %s %s\n", control->play.stream, control->play.focus);

  uint32_t *index = calloc(1, sizeof(uint32_t));
  *index = multi_session_get_index_by_handle(NULL);
  if (*index == 0xFFFFFFFF)
    {
      LOG_E("unknown index\n");
      free(index);
      multi_session_play_notify(control, NULL, -1);
      return;
    }

  void *handle = media_uv_player_open(
      multi_session_loop(), control->play.stream, media_open_cb, index);
  if (handle == NULL)
    {
      LOG_E("open fail\n");
      free(index);
      multi_session_play_notify(control, NULL, -1);
      return;
    }

  int ret = media_uv_player_listen(handle, media_vela_event_callback);
  if (ret < 0)
    {
      LOG_E("listen fail %d\n", ret);
      multi_session_play_notify(control, NULL, -1);
      media_uv_player_close(handle, 0, media_close_cb);
      return;
    }

  LOG_I("prepare %p %s %s\n", handle, control->play.url,
        control->play.options);
  ret = media_uv_player_prepare(
      handle, control->play.url, control->play.options,
      media_player_prepare_connect_cb, NULL, index);
  if (ret < 0)
    {
      LOG_E("prepare fail %d\n", ret);
      multi_session_play_notify(control, NULL, -1);
      media_uv_player_close(handle, 0, media_close_cb);
      return;
    }

  multi_session[*index].command = control->command;
  multi_session[*index].stream = control->play.stream;
  multi_session[*index].focus = control->play.focus;
  multi_session[*index].handle = handle;
  multi_session[*index].user_data = control->user_data;
  multi_session[*index].cb = control->play.cb;

  multi_session[*index].url = strdup(control->play.url);
  LOG_I("ENDENDDGFDSAGDSG\n");
  if (multi_session[*index].url == NULL)
    {
      LOG_E("strdup fail\n");
    }

  if (control->play.loop == MULTI_SESSION_CONTROL_LOOP_DISABLE)
    {
      ret = media_uv_player_set_looping(handle, 0, NULL, NULL);
    }
  else
    {
      ret = media_uv_player_set_looping(handle, -1, NULL, NULL);
    }
  if (ret < 0)
    {
      LOG_E("set loop fail %d\n", ret);
    }

  uint32_t *play_cookie = calloc(1, sizeof(uint32_t));
  *play_cookie = *index;
  ret = media_uv_player_start_auto(handle, control->play.focus,
                                   media_player_with_focus, play_cookie);
  if (ret < 0)
    {
      LOG_E("stat auto fail %d\n", ret);
      free(play_cookie);
      multi_session_play_notify(control, NULL, -1);
      media_uv_player_close(handle, 0, media_close_cb);
      return;
    }
  LOG_I("play end %p %" PRIu32 " %p %p\n", handle, *index, index,
        play_cookie);
#else
  LOG_I("not config media play %s\n", control->play.stream);
  multi_session_play_notify(control, NULL, -1);
#endif
}

static void
multi_session_control_cmd_open(multi_session_control_t *control)
{
#ifdef CONFIG_MEDIA
  LOG_I("open start %s %s\n", control->open.stream, control->open.focus);

  uint32_t *index = calloc(1, sizeof(uint32_t));
  *index = multi_session_get_index_by_handle(NULL);
  if (*index == 0xFFFFFFFF)
    {
      LOG_E("unknown index\n");
      free(index);
      multi_session_open_notify(control, NULL, -1);
      return;
    }

  void *handle = media_uv_player_open(
      multi_session_loop(), control->open.stream, media_open_cb, index);
  if (handle == NULL)
    {
      LOG_E("open fail\n");
      free(index);
      multi_session_open_notify(control, NULL, -1);
      return;
    }

  int ret = media_uv_player_listen(handle, media_vela_event_callback);
  if (ret < 0)
    {
      LOG_E("listen event fail %d\n", ret);
      multi_session_open_notify(control, NULL, -1);
      media_uv_player_close(handle, 0, media_close_cb);
      return;
    }

  multi_session[*index].handle = handle;
  multi_session[*index].command = control->command;
  multi_session[*index].user_data = control->user_data;
  multi_session[*index].cb = control->open.cb;
  multi_session[*index].stream = control->open.stream;
  multi_session[*index].focus = control->open.focus;

  LOG_I("open end %p %" PRIu32 "\n", handle, *index);
#else
  LOG_I("not config media open %s\n", control->open.stream);
  multi_session_open_notify(control, NULL, -1);
#endif
}

#ifdef CONFIG_MEDIA
static void
multi_session_control_cmd_prepare(multi_session_control_t *control)
{
  uint32_t index = multi_session_get_index_by_handle(control->handle);
  LOG_I("%s %p %s\n", multi_session[index].stream, control->handle,
        control->prepare.url);

  int ret = media_uv_player_prepare(
      control->handle, control->prepare.url, control->prepare.options,
      media_player_prepare_connect_cb, NULL, &index);
  if (ret < 0)
    {
      LOG_E("prepare fail %d\n", ret);
    }

  multi_session_update_local_url(index, control->prepare.url);
}

static void
multi_session_control_cmd_start(multi_session_control_t *control)
{
  uint32_t index = multi_session_get_index_by_handle(control->handle);
  LOG_I("%s %p\n", multi_session[index].stream, control->handle);

  uint32_t *play_cookie = calloc(1, sizeof(uint32_t));
  *play_cookie = index;
  int ret = media_uv_player_start_auto(
      control->handle, multi_session[index].focus,
      media_player_with_focus, play_cookie);
  if (ret < 0)
    {
      LOG_E("start auto fail %d\n", ret);
      free(play_cookie);
      return;
    }

  LOG_I("start end %p %" PRIu32 "\n", play_cookie, *play_cookie);
}

static void
multi_session_control_cmd_pause(multi_session_control_t *control)
{
  uint32_t index = multi_session_get_index_by_handle(control->handle);
  LOG_I("%s %p\n", multi_session[index].stream, control->handle);

  int ret = media_uv_player_pause(control->handle, NULL, NULL);
  if (ret < 0)
    {
      LOG_E("pause fail %d\n", ret);
    }
}

static void
multi_session_control_cmd_stop(multi_session_control_t *control)
{
  uint32_t index = multi_session_get_index_by_handle(control->handle);
  LOG_I("%s %p\n", multi_session[index].stream,
        multi_session[index].handle);

  int ret = media_uv_player_stop(control->handle, NULL, NULL);
  if (ret < 0)
    {
      LOG_E("stop fail %d\n", ret);
    }
}

static void
multi_session_control_cmd_reset(multi_session_control_t *control)
{
  uint32_t index = multi_session_get_index_by_handle(control->handle);
  LOG_I("%s %p\n", multi_session[index].stream,
        multi_session[index].handle);

  int ret = media_uv_player_reset(control->handle, NULL, NULL);
  if (ret < 0)
    {
      LOG_E("reset fail %d\n", ret);
    }
}

static void
multi_session_control_cmd_close(multi_session_control_t *control)
{
  uint32_t index = multi_session_get_index_by_handle(control->handle);
  LOG_I("%s %p\n", multi_session[index].stream,
        multi_session[index].handle);

  int ret = media_uv_player_close(control->handle, 0, media_close_cb);
  if (ret < 0)
    {
      LOG_E("close fail %d\n", ret);
    }
}

static void multi_session_control_cmd_set_graph_volume(
    multi_session_control_t *control)
{
  uint32_t index = multi_session_get_index_by_handle(control->handle);
  LOG_I("%s %p\n", multi_session[index].stream,
        multi_session[index].handle);
  int ret = media_uv_player_set_volume(
      control->handle, control->graph_volume.vol, NULL, NULL);
  if (ret < 0)
    {
      LOG_E("set graph volume fail %d\n", ret);
    }
}

static void media_get_graphvolume_cb(void *cookie, int ret, float val)
{
  LOG_I("%s ret: %d val:%f\n", (char *)cookie, ret, val);
}

static void multi_session_control_cmd_get_graph_volume(
    multi_session_control_t *control)
{
  uint32_t index = multi_session_get_index_by_handle(control->handle);
  LOG_I("%s %p\n", multi_session[index].stream,
        multi_session[index].handle);
  int ret = media_uv_player_get_volume(
      control->handle, media_get_graphvolume_cb,
      (void *)multi_session[index].stream);
  if (ret < 0)
    {
      LOG_E("set graph volume fail %d\n", ret);
    }
}

static void
multi_session_control_cmd_set_loop(multi_session_control_t *control)
{
  uint32_t index = multi_session_get_index_by_handle(control->handle);
  LOG_I("%s %p %d\n", multi_session[index].stream,
        multi_session[index].handle, control->set_loop.loop);

  int ret;
  if (MULTI_SESSION_CONTROL_LOOP_DISABLE == control->set_loop.loop)
    {
      ret = media_uv_player_set_looping(control->handle, 0, NULL, NULL);
    }
  else
    {
      ret = media_uv_player_set_looping(
          control->handle, control->set_loop.loop, NULL, NULL);
    }
  if (ret < 0)
    {
      LOG_E("set loop fail %d\n", ret);
    }
}

static void
multi_session_control_cmd_set_seek(multi_session_control_t *control)
{
  uint32_t index = multi_session_get_index_by_handle(control->handle);
  LOG_I("%s %p %" PRIu32 "\n", multi_session[index].stream,
        multi_session[index].handle, control->set_seek.msec);

  int ret = media_uv_player_seek(control->handle, control->set_seek.msec,
                                 NULL, NULL);
  if (ret < 0)
    {
      LOG_E("set seek fail %d\n", ret);
    }
}

static void media_get_position_cb(void *cookie, int ret,
                                  unsigned position)
{
  uint32_t index = multi_session_get_index_by_handle(cookie);
  LOG_D("%s %p %d\n", multi_session[index].stream,
        multi_session[index].handle, position);

  multi_session_event_t event = {
      .user_data = multi_session[index].user_data,
      .handle = multi_session[index].handle,
      .status = (ret) < (0) ? (-1) : (0),
      .command = MULTI_SESSION_EVENT_CMD_GET_POSITION,
      .get_position.msec = position};

  multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &event,
                           sizeof(event));
}

static void
multi_session_control_cmd_get_position(multi_session_control_t *control)
{
  uint32_t index = multi_session_get_index_by_handle(control->handle);
  LOG_D("%s %p\n", multi_session[index].stream,
        multi_session[index].handle);

  int ret = media_uv_player_get_position(
      control->handle, media_get_position_cb, control->handle);
  if (ret < 0)
    {
      LOG_E("get fail %d\n", ret);
    }
}

static void media_get_duration_cb(void *cookie, int ret,
                                  unsigned duration)
{
  uint32_t index = multi_session_get_index_by_handle(cookie);
  LOG_D("%s %p %d\n", multi_session[index].stream,
        multi_session[index].handle, duration);

  multi_session_event_t event = {
      .user_data = multi_session[index].user_data,
      .handle = multi_session[index].handle,
      .status = (ret) < (0) ? (-1) : (0),
      .command = MULTI_SESSION_EVENT_CMD_GET_DURATION,
      .get_duration.msec = duration};

  multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &event,
                           sizeof(event));
}

static void
multi_session_control_cmd_get_duration(multi_session_control_t *control)
{
  uint32_t index = multi_session_get_index_by_handle(control->handle);
  LOG_D("%s %p\n", multi_session[index].stream,
        multi_session[index].handle);

  int ret = media_uv_player_get_duration(
      control->handle, media_get_duration_cb, control->handle);
  if (ret < 0)
    {
      LOG_E("get fail %d\n", ret);
    }
}

static void media_get_playing_cb(void *cookie, int ret, int status)
{
  uint32_t index = multi_session_get_index_by_handle(cookie);
  LOG_D("%s %p %d\n", multi_session[index].stream,
        multi_session[index].handle, status);

  multi_session_event_t event = {
      .user_data = multi_session[index].user_data,
      .handle = multi_session[index].handle,
      .status = (ret) < (0) ? (-1) : (0),
      .command = MULTI_SESSION_EVENT_CMD_GET_PLAY_STATE,
      .get_play_state.playing = status};

  multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &event,
                           sizeof(event));
}

static void multi_session_control_cmd_get_play_state(
    multi_session_control_t *control)
{
  uint32_t index = multi_session_get_index_by_handle(control->handle);
  LOG_D("%s %p\n", multi_session[index].stream,
        multi_session[index].handle);

  int ret = media_uv_player_get_playing(
      control->handle, media_get_playing_cb, control->handle);
  if (ret < 0)
    {
      LOG_E("get fail %d\n", ret);
    }
}

static void media_query_all(void *cookie, int ret, void *object)
{
  uint32_t index = multi_session_get_index_by_handle(cookie);
  if (index == 0xFFFFFFFF)
    {
      LOG_E("unknown index\n");
      return;
    }

  media_metadata_t *data = (media_metadata_t *)object;
  LOG_I(
      "%s %p %d %p f:%d st:%d vol:%d pos:%u dur:%u "
      "ttl:%s art:%s\n",
      multi_session[index].stream, multi_session[index].handle, ret,
      cookie, data->flags, data->state, data->volume, data->position,
      data->duration, data->title, data->artist);

  multi_session_event_t event = {
      .user_data = multi_session[index].user_data,
      .handle = multi_session[index].handle,
      .status = (ret) < (0) ? (-1) : (0),
      .command = MULTI_SESSION_EVENT_CMD_GET_ALL,
      .get_all.playing = data->state,
      .get_all.pos = data->position,
      .get_all.msec = data->duration,
      .get_all.volume = data->volume};

  multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &event,
                           sizeof(event));
}

static void
multi_session_control_cmd_get_all(multi_session_control_t *control)
{
  uint32_t index = multi_session_get_index_by_handle(control->handle);
  LOG_I("%s %p\n", multi_session[index].stream,
        multi_session[index].handle);

  int ret = media_uv_player_query(control->handle, media_query_all,
                                  control->handle);
  if (ret < 0)
    {
      LOG_E("fail %d\n", ret);
    }
}

static void multi_session_control_cmd_set_audio_mode(
    multi_session_control_t *control)
{
  LOG_I("%d(0:normal, 1:phone)\n", control->set_audio_mode.audio_mode);
  int ret = 0;
  if (MULTI_SESSION_AUDIO_MODE_NORMAL ==
      control->set_audio_mode.audio_mode)
    {
      ret = media_uv_policy_set_audio_mode(
          multi_session_loop(), MEDIA_AUDIO_MODE_NORMAL, NULL, NULL);
    }
  else if (MULTI_SESSION_AUDIO_MODE_PHONE ==
           control->set_audio_mode.audio_mode)
    {
      ret = media_uv_policy_set_audio_mode(
          multi_session_loop(), MEDIA_AUDIO_MODE_PHONE, NULL, NULL);
    }
  if (ret < 0)
    {
      LOG_E("set fail %d\n", ret);
    }
}

static void
multi_session_control_cmd_set_mute_mode(multi_session_control_t *control)
{
  LOG_I("%d\n", control->set_mute_mode.mute);
  int ret = media_uv_policy_set_mute_mode(
      multi_session_loop(), control->set_mute_mode.mute, NULL, NULL);
  if (ret < 0)
    {
      LOG_E("set fail %d\n", ret);
    }
}

static void media_get_mute_mode_cb(void *cookie, int ret, int val)
{
  if (cookie == NULL)
    {
      LOG_E("cookie NULL\n");
      return;
    }

  LOG_I("get mute end %d %d\n", val, ret);

  multi_session_control_t *control = (multi_session_control_t *)cookie;
  multi_session_event_t event = {
      .user_data = control->user_data,
      .handle = NULL,
      .status = (ret) < (0) ? (-1) : (0),
      .command = MULTI_SESSION_EVENT_CMD_GET_MUTE_MODE,
      .get_mute_mode.mute = val,
      .get_mute_mode.cb = control->get_mute_mode.cb};

  multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &event,
                           sizeof(event));
  free(cookie);
}

static void
multi_session_control_cmd_get_mute_mode(multi_session_control_t *control)
{
  LOG_I("get mute start\n");
  multi_session_control_t *tmp =
      calloc(1, sizeof(multi_session_control_t));
  if (tmp == NULL)
    {
      LOG_E("calloc fail\n");
      return;
    }
  memcpy(tmp, control, sizeof(multi_session_control_t));

  int ret = media_uv_policy_get_mute_mode(multi_session_loop(),
                                          media_get_mute_mode_cb, tmp);
  if (ret < 0)
    {
      LOG_E("get fail %d\n", ret);
      free(tmp);
    }
}

static void multi_session_control_cmd_set_stream_volume(
    multi_session_control_t *control)
{
  LOG_I("%s %d\n", control->set_stream_volume.stream,
        control->set_stream_volume.volume);
  int ret = media_uv_policy_set_stream_volume(
      multi_session_loop(), control->set_stream_volume.stream,
      control->set_stream_volume.volume, NULL, NULL);
  if (ret < 0)
    {
      LOG_E("set fail %d\n", ret);
    }
}

static void media_get_stream_volume_cb(void *cookie, int ret, int val)
{
  if (cookie == NULL)
    {
      LOG_E("cookie NULL\n");
      return;
    }

  multi_session_control_t *control = (multi_session_control_t *)cookie;
  LOG_I("%s %d\n", control->get_stream_volume.stream, val);
  multi_session_event_t event = {
      .user_data = control->user_data,
      .handle = NULL,
      .status = (ret) < (0) ? (-1) : (0),
      .command = MULTI_SESSION_EVENT_CMD_GET_STREAM_VOLUME,
      .get_stream_volume.volume = val,
      .get_stream_volume.cb = control->get_stream_volume.cb};
  multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &event,
                           sizeof(event));

  free(cookie);
}

static void multi_session_control_cmd_get_stream_volume(
    multi_session_control_t *control)
{
  LOG_I("%s\n", control->get_stream_volume.stream);
  multi_session_control_t *tmp =
      calloc(1, sizeof(multi_session_control_t));
  if (tmp == NULL)
    {
      LOG_E("calloc fail\n");
      return;
    }
  memcpy(tmp, control, sizeof(multi_session_control_t));

  int ret = media_uv_policy_get_stream_volume(
      multi_session_loop(), control->get_stream_volume.stream,
      media_get_stream_volume_cb, tmp);
  if (ret < 0)
    {
      LOG_E("get fail %d\n", ret);
      free(tmp);
    }
}

static void multi_session_control_cmd_inc_stream_volume(
    multi_session_control_t *control)
{
  LOG_I("%s\n", control->set_stream_volume.stream);
  int ret = media_uv_policy_increase_stream_volume(
      multi_session_loop(), control->set_stream_volume.stream, NULL,
      NULL);
  if (ret < 0)
    {
      LOG_E("inc fail\n");
    }
}

static void multi_session_control_cmd_dec_stream_volume(
    multi_session_control_t *control)
{
  LOG_I("%s\n", control->set_stream_volume.stream);
  int ret = media_uv_policy_decrease_stream_volume(
      multi_session_loop(), control->set_stream_volume.stream, NULL,
      NULL);
  if (ret < 0)
    {
      LOG_E("dec fail %d\n", ret);
    }
}

static void media_recorder_begin_notify(multi_session_control_t *control,
                                        void *handle, int status)
{
  multi_session_event_t event = {
      .user_data = control->user_data,
      .handle = handle,
      .status = status,
      .command = MULTI_SESSION_EVENT_CMD_RECORDER_BEGIN,
      .recorder_begin.cb = control->recorder_begin.cb};

  multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &event,
                           sizeof(event));
}

static void media_recorder_open_notify(multi_session_control_t *control,
                                       void *handle, int status)
{
  multi_session_event_t event = {
      .user_data = control->user_data,
      .handle = handle,
      .status = status,
      .command = MULTI_SESSION_EVENT_CMD_RECORDER_OPEN,
      .recorder_open.cb = control->recorder_open.cb};

  multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &event,
                           sizeof(event));
}

static void media_recorder_open_cb(void *cookie, int ret)
{
  uint32_t index = *(uint32_t *)cookie;
  LOG_I("index %" PRIu32 " ret %d %p\n", index, ret,
        multi_session[index].handle);

  multi_session_control_t control = {.user_data =
                                         multi_session[index].user_data};

  void *handle = ret < 0 ? NULL : multi_session[index].handle;
  if (multi_session[index].command ==
      MULTI_SESSION_CONTROL_CMD_RECORDER_OPEN)
    {
      control.recorder_open.cb = multi_session[index].cb;
      media_recorder_open_notify(&control, handle, ret < 0 ? -1 : 0);
    }
  else
    {
      control.recorder_begin.cb = multi_session[index].cb;
      media_recorder_begin_notify(&control, handle, ret < 0 ? -1 : 0);
    }

  if (ret < 0)
    {
      if (multi_session[index].handle)
        {
          media_uv_recorder_close(multi_session[index].handle,
                                  media_close_cb);
        }
    }
}

static void media_recorder_with_focus(void *cookie, int ret)
{
  if (cookie == NULL)
    {
      LOG_E("cookie NULL\n");
      return;
    }

  uint32_t index = *(uint32_t *)cookie;
  LOG_I("cookie index ret %p %" PRIu32 " %d\n", cookie, index, ret);

  if (ret == -1)
    {
      LOG_I("focus stop\n");
      media_vela_event_callback(cookie, MEDIA_EVENT_STOPPED, 0, NULL);
    }

  free(cookie);
}

static void alloc_read_buffer(uv_handle_t *handle, size_t suggested_size,
                              uv_buf_t *buf)
{
  buf->base = (char *)calloc(1, suggested_size);
  buf->len = suggested_size;
}

static void read_buffer_done_cb(uv_stream_t *client, ssize_t nread,
                                const uv_buf_t *buf)
{
  uv_pipe_t *pipe = (uv_pipe_t *)client;
  uint32_t index = multi_session_get_index_by_handle(pipe->data);

  multi_session_event_t event = {
      .user_data = multi_session[index].user_data,
      .handle = multi_session[index].handle,
      .status = nread > 0 ? 0 : nread,
      .command = MULTI_SESSION_EVENT_CMD_RECORDER_READ,
      .recorder_read.data = buf->base,
      .recorder_read.len = buf->base ? nread : 0};

  multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &event,
                           sizeof(event));
}

static void media_recorder_prepare_connect_cb(void *cookie, int ret,
                                              void *obj)
{
  if (cookie == NULL || ret < 0)
    {
      LOG_E("cookie ret %p %d\n", cookie, ret);
      return;
    }

  uint32_t index = *(uint32_t *)cookie;
  multi_session[index].pipe = (uv_pipe_t *)obj;

  LOG_I("index %" PRIu32 " %p %p %d %" PRIu32 "\n", index, cookie, obj,
        ret, multi_session[index].read);

  if (multi_session[index].read == 1)
    {
      multi_session[index].read = 0;
      multi_session[index].pipe->data = multi_session[index].handle;
      uv_read_start((uv_stream_t *)multi_session[index].pipe,
                    alloc_read_buffer, read_buffer_done_cb);
    }
}

#endif

static void multi_session_control_cmd_recorder_begin(
    multi_session_control_t *control)
{
  LOG_I("begin stream %s %s\n", control->recorder_begin.stream,
        control->recorder_begin.focus);
#ifdef CONFIG_MEDIA
  uint32_t *index = calloc(1, sizeof(uint32_t));
  *index = multi_session_get_index_by_handle(NULL);
  if (*index == 0xFFFFFFFF)
    {
      LOG_E("unknown index\n");
      free(index);
      media_recorder_begin_notify(control, NULL, -1);
      return;
    }

  void *handle = media_uv_recorder_open(multi_session_loop(),
                                        control->recorder_begin.stream,
                                        media_recorder_open_cb, index);
  if (handle == NULL)
    {
      LOG_E("open fail\n");
      free(index);
      media_recorder_begin_notify(control, NULL, -1);
      return;
    }

  int ret = media_uv_recorder_listen(handle, media_vela_event_callback);
  if (ret < 0)
    {
      LOG_E("listen fail %d\n", ret);
      media_recorder_begin_notify(control, NULL, -1);
      media_uv_recorder_close(handle, media_close_cb);
      return;
    }

  LOG_I("prepare %p url %s opt %s\n", handle,
        control->recorder_begin.url, control->recorder_begin.options);

  if (control->recorder_begin.url[0] == 0)
    {
      ret = media_uv_recorder_prepare(
          handle, control->recorder_begin.url,
          control->recorder_begin.options,
          media_recorder_prepare_connect_cb, NULL, NULL);
    }
  else
    {
      ret = media_uv_recorder_prepare(
          handle, control->recorder_begin.url,
          control->recorder_begin.options, NULL, NULL, NULL);
    }
  if (ret < 0)
    {
      LOG_E("prepare fail %d\n", ret);
      media_recorder_begin_notify(control, NULL, -1);
      media_uv_recorder_close(handle, media_close_cb);
      return;
    }

  uint32_t *recorder_cookie = calloc(1, sizeof(uint32_t));
  *recorder_cookie = *index;
  ret = media_uv_recorder_start_auto(
      handle, control->recorder_begin.focus, media_recorder_with_focus,
      recorder_cookie);
  if (ret < 0)
    {
      LOG_E("start auto fail %d\n", ret);
      free(recorder_cookie);
      media_recorder_begin_notify(control, NULL, -1);
      media_uv_recorder_close(handle, media_close_cb);
      return;
    }

  multi_session[*index].user_data = control->user_data;
  multi_session[*index].command =
      MULTI_SESSION_CONTROL_CMD_RECORDER_BEGIN;
  multi_session[*index].cb = control->recorder_begin.cb;
  multi_session[*index].stream = control->recorder_begin.stream;
  multi_session[*index].focus = control->recorder_begin.focus;
  multi_session[*index].handle = handle;
  multi_session[*index].url = NULL;
  multi_session[*index].pipe = NULL;
  multi_session[*index].read = 0;

  LOG_I("begin end %p %p %" PRIu32 " %p\n", handle, index, *index,
        recorder_cookie);
#else
  multi_session_event_t event = {
      .user_data = control->user_data,
      .command = MULTI_SESSION_EVENT_CMD_RECORDER_BEGIN,
      .recorder_begin.cb = control->recorder_begin.cb};

#ifdef MULTI_SESSION_BES_AUDIO
  uint32_t index = multi_session_get_index_by_handle(NULL);
  if (index == 0xFFFFFFFF)
    {
      goto fail;
    }

  if (g_bes_audio != NULL)
    {
      LOG_E("g_bes_audio is not NULL %p\n", g_bes_audio);
      goto fail;
    }

  if (control->recorder_begin.url[0] != 0)
    {
      LOG_E("is not buffer mode \n");
      goto fail;
    }

  bool opus = false;
  LOG_I("recorder options %s\n", control->recorder_begin.options);
  if (strstr(control->recorder_begin.options, "opus"))
    {
      opus = true;
    }
  LOG_I("type is %s\n", opus ? "opus" : "silk");

  g_bes_audio = calloc(1, sizeof(bes_audio_t));
  if (g_bes_audio == NULL)
    {
      LOG_E("calloc fail\n");
      goto fail;
    }

  g_bes_audio->type = opus ? OPUS_CODEC : SILK_CODEC;
  ;
  g_bes_audio->cb = bes_audio_codec_func;
  g_bes_audio->user_data = control->user_data;

  event.handle = g_bes_audio;
  event.status = 0;

  multi_session[index].cb = control->recorder_begin.cb;
  multi_session[index].stream = control->recorder_begin.stream;
  multi_session[index].handle = g_bes_audio;
  multi_session[index].url = NULL;

  nuttx_audio_encode_open(g_bes_audio->type, g_bes_audio->cb,
                          SAMPLE_RATE_16000, SAMPLE_TIME_20_MS);

  multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &event,
                           sizeof(event));
  return;
fail:
  event.handle = NULL;
  event.status = -1;
  multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &event,
                           sizeof(event));
  return;
#endif /* MULTI_SESSION_BES_AUDIO */
  event.status = -1;
  multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &event,
                           sizeof(event));
#endif
}

#ifdef CONFIG_MEDIA
static void multi_session_control_cmd_recorder_pause(
    multi_session_control_t *control)
{
  LOG_I("%p\n", control->handle);
  int ret = media_uv_recorder_pause(control->handle, NULL, NULL);
  if (ret < 0)
    {
      LOG_E("pause fail %d\n", ret);
    }
}

static void media_recorder_get_prop_cb(void *cookie, int ret,
                                       const char *val)
{
  if (cookie == NULL)
    {
      LOG_E("cookie NULL\n");
      return;
    }

  multi_session_control_t *control = (multi_session_control_t *)cookie;
  LOG_D("run %p %p %d %s\n", control, control->handle, ret, val);

  multi_session_event_t event = {
      .user_data = control->user_data,
      .handle = control->handle,
      .status = ret < 0 ? -1 : 0,
      .command = MULTI_SESSION_EVENT_CMD_RECORDER_GET_PROP,
      .recorder_prop.data = control->recorder_get_prop.data,
      .recorder_prop.len = control->recorder_get_prop.len};

  if (ret >= 0 && val != NULL)
    {
      memcpy(event.recorder_prop.data, val, strlen(val));
      memcpy(event.recorder_prop.key, control->recorder_get_prop.key,
             sizeof(event.recorder_prop.key));
    }

  multi_session_async_send(MULTI_SESSION_TYPE_EVENT, &event,
                           sizeof(event));

  free(cookie);
}

static void multi_session_control_cmd_recorder_get_prop(
    multi_session_control_t *control)
{
  LOG_D("%p\n", control->handle);
  multi_session_control_t *tmp =
      calloc(1, sizeof(multi_session_control_t));
  if (tmp == NULL)
    {
      LOG_E("calloc fail\n");
      return;
    }
  memcpy(tmp, control, sizeof(multi_session_control_t));

  int ret = media_uv_recorder_get_property(
      control->handle, control->recorder_get_prop.target,
      control->recorder_get_prop.key, media_recorder_get_prop_cb, tmp);
  if (ret < 0)
    {
      LOG_E("get fail %d\n", ret);
      free(tmp);
    }
}

static void
multi_session_control_cmd_recorder_stop(multi_session_control_t *control)
{
  LOG_I("%p\n", control->handle);
  int ret = media_uv_recorder_stop(control->handle, NULL, NULL);
  if (ret < 0)
    {
      LOG_E("stop fail %d\n", ret);
    }
}

static void multi_session_control_cmd_recorder_reset(
    multi_session_control_t *control)
{
  LOG_I("%p\n", control->handle);
  int ret = media_uv_recorder_reset(control->handle, NULL, NULL);
  if (ret < 0)
    {
      LOG_E("stop fail %d\n", ret);
    }
}
#endif /* CONFIG_MEDIA */

static void multi_session_control_cmd_recorder_close(
    multi_session_control_t *control)
{
  LOG_I("start %p\n", control->handle);
#ifdef CONFIG_MEDIA
  int ret = 0;

  uint32_t index = multi_session_get_index_by_handle(control->handle);
  if (multi_session[index].pipe != NULL)
    {
      ret = media_uv_recorder_stop(control->handle, NULL, NULL);
      if (ret < 0)
        {
          LOG_E("stop fail %d\n", ret);
        }
    }

  ret = media_uv_recorder_close(control->handle, media_close_cb);
  if (ret < 0)
    {
      LOG_E("close fail %d\n", ret);
    }
#else
#ifdef MULTI_SESSION_BES_AUDIO
  LOG_I("g_bes_audio %p\n", control->handle, g_bes_audio);
  if (control->handle == g_bes_audio)
    {
      nuttx_audio_encode_close(g_bes_audio->type);
      free(g_bes_audio);
      g_bes_audio = NULL;

      multi_session[index].handle = NULL;
      multi_session[index].user_data = NULL;
      multi_session[index].cb = NULL;
      multi_session[index].stream = NULL;
    }
#endif /* MULTI_SESSION_BES_AUDIO */
#endif
  LOG_I("end\n");
}

static void
multi_session_control_cmd_recorder_read(multi_session_control_t *control)
{
#ifdef CONFIG_MEDIA
  LOG_D("read handle %p data %p\n", control->handle,
        control->recorder_read.data);

  uint32_t index = multi_session_get_index_by_handle(control->handle);
  if (multi_session[index].pipe != NULL)
    {
      multi_session[index].pipe->data = control->handle;
      uv_read_start((uv_stream_t *)multi_session[index].pipe,
                    alloc_read_buffer, read_buffer_done_cb);
    }
  else
    {
      multi_session[index].read = 1;
      LOG_I("pipe invalid\n");
    }
#else
#ifdef MULTI_SESSION_BES_AUDIO
  bes_audio_t *p_audio = (bes_audio_t *)control->handle;
  LOG_D("p_audio read, %p %p\n", p_audio, g_bes_audio);
  nuttx_audio_encode(p_audio->type);
#endif /* MULTI_SESSION_BES_AUDIO */
#endif /* CONFIG_MEDIA */
}

#ifdef CONFIG_MEDIA
static void
multi_session_control_cmd_recorder_open(multi_session_control_t *control)
{
  LOG_I("open %s %s\n", control->recorder_open.stream,
        control->recorder_open.focus);

  uint32_t *index = calloc(1, sizeof(uint32_t));
  *index = multi_session_get_index_by_handle(NULL);
  if (*index == 0xFFFFFFFF)
    {
      LOG_E("unknown index\n");
      free(index);
      media_recorder_open_notify(control, NULL, -1);
      return;
    }

  void *handle =
      media_uv_recorder_open(multi_session_loop(), control->open.stream,
                             media_recorder_open_cb, index);
  if (handle == NULL)
    {
      LOG_E("open fail\n");
      free(index);
      media_recorder_open_notify(control, NULL, -1);
      return;
    }

  int ret = media_uv_recorder_listen(handle, media_vela_event_callback);
  if (ret < 0)
    {
      LOG_E("listen fail %d\n", ret);
      media_recorder_open_notify(control, NULL, -1);
      media_uv_recorder_close(handle, media_close_cb);
      return;
    }

  multi_session[*index].command =
      MULTI_SESSION_CONTROL_CMD_RECORDER_OPEN;
  multi_session[*index].stream = control->open.stream;
  multi_session[*index].focus = control->open.focus;
  multi_session[*index].user_data = control->user_data;
  multi_session[*index].handle = handle;
  multi_session[*index].cb = control->open.cb;
  multi_session[*index].pipe = NULL;

  LOG_I("end %p %" PRIu32 "\n", handle, *index);
}

static void multi_session_control_cmd_recorder_prepare(
    multi_session_control_t *control)
{
  LOG_I("%p url %s opt %s\n", control->handle,
        control->recorder_prepare.url,
        control->recorder_prepare.options);

  int ret = 0;
  if (control->recorder_begin.url[0] == 0)
    {
      ret = media_uv_recorder_prepare(
          control->handle, control->recorder_begin.url,
          control->recorder_begin.options,
          media_recorder_prepare_connect_cb, NULL, NULL);
    }
  else
    {
      ret = media_uv_recorder_prepare(
          control->handle, control->recorder_begin.url,
          control->recorder_begin.options, NULL, NULL, NULL);
    }
  if (ret < 0)
    {
      LOG_E("prepare fail %d\n", ret);
    }
}

static void multi_session_control_cmd_recorder_start(
    multi_session_control_t *control)
{
  LOG_I("start %p\n", control->handle);
  uint32_t index = multi_session_get_index_by_handle(control->handle);

  uint32_t *recorder_cookie = calloc(1, sizeof(uint32_t));
  *recorder_cookie = index;
  int ret = media_uv_recorder_start_auto(
      control->handle, multi_session[index].focus,
      media_recorder_with_focus, recorder_cookie);
  if (ret < 0)
    {
      LOG_E("start fail %d\n", ret);
      free(recorder_cookie);
      return;
    }

  LOG_I("end %p %d\n", recorder_cookie, (int)*recorder_cookie);
}

static void
multi_session_control_cmd_set_sco_use(multi_session_control_t *control)
{
  LOG_I("%d(0:unused, 1 :used)\n", control->sco.use);

  int ret = media_uv_policy_set_devices_use(
      multi_session_loop(), MEDIA_DEVICE_SCO, control->sco.use, NULL,
      NULL);
  if (ret < 0)
    {
      LOG_E("set fail %d\n", ret);
    }
}

static void
multi_session_control_cmd_set_mic_mode(multi_session_control_t *control)
{
  LOG_I("%d(0:off, 1:on)\n", control->mic_mode.mode);

  int ret = 0;
  if (control->mic_mode.mode == MULTI_SESSION_MIC_MODE_ON)
    {
      ret = media_uv_policy_set_mic_mute(multi_session_loop(), 0, NULL,
                                         NULL);
    }
  else
    {
      ret = media_uv_policy_set_mic_mute(multi_session_loop(), 1, NULL,
                                         NULL);
    }
  if (ret < 0)
    {
      LOG_E("set fail %d\n", ret);
    }
}

static void
multi_session_control_cmd_play_next_url(multi_session_control_t *control)
{
  uint32_t index = multi_session_get_index_by_handle(control->handle);
  LOG_I("play next start %s %p\n", multi_session[index].stream,
        control->handle);

  int ret = media_uv_player_stop(control->handle, NULL, NULL);
  if (ret < 0)
    {
      LOG_E("stop fail %d\n", ret);
    }

  char *url = NULL;
  char *option = NULL;

  if (control->command == MULTI_SESSION_CONTROL_CMD_LIST_PLAY_NEXT)
    {
      url = control->play_list.url;
      option = control->play_list.options;
    }
  else
    {
      url = control->play_next.url;
      option = control->play_next.options;
    }

  LOG_I("prepare url %s\n", url);
  ret = media_uv_player_prepare(control->handle, url, option, NULL, NULL,
                                NULL);
  if (ret < 0)
    {
      LOG_E("prepare fail %d\n", ret);
    }
  else
    {
      multi_session_update_local_url(index, url);
    }

  uint32_t *play_cookie = calloc(1, sizeof(uint32_t));
  *play_cookie = index;
  ret = media_uv_player_start_auto(control->handle,
                                   multi_session[index].focus,
                                   media_player_with_focus, play_cookie);
  if (ret < 0)
    {
      LOG_E("player_play fail %d\n", ret);
      free(play_cookie);
      return;
    }

  LOG_I("play next end %p %d\n", play_cookie, (int)*play_cookie);
}
#endif

void *multi_session_get_stream_handle(const char *stream)
{
  if (stream == NULL)
    {
      LOG_E("stream NULL\n");
      return NULL;
    }

  for (uint32_t i = 0; i < MULTI_SESSION_COUNT_MAX; i++)
    {
      if (multi_session[i].handle)
        {
          if (strcmp(multi_session[i].stream, stream) == 0)
            {
              return multi_session[i].handle;
            }
        }
    }

  return NULL;
}

void multi_session_set_audio_mode(uint8_t mode)
{
#ifdef CONFIG_MEDIA
  LOG_D("set audio mode %d\n", mode);
  multi_session_control_t control = {
      .handle = NULL,
      .command = MULTI_SESSION_CONTROL_CMD_SET_AUDIO_MODE,
      .set_mute_mode.mute = mode,
  };
  multi_session_control(&control);
#endif
}

void multi_session_set_global_volume(char *stream, uint8_t volume)
{
#ifdef CONFIG_MEDIA
  LOG_D("set volume %s %d\n", stream, volume);
  multi_session_control_t control = {
      .handle = NULL,
      .command = MULTI_SESSION_CONTROL_CMD_SET_STREAM_VOLUME,
      .set_stream_volume.stream = stream,
      .set_stream_volume.volume = volume};

  multi_session_control(&control);
#endif
}

void multi_session_get_global_volume(char *stream,
                                     multi_session_notify_t cb)
{
#ifdef CONFIG_MEDIA
  LOG_D("get volume %s\n", stream);
  multi_session_control_t control = {
      .handle = NULL,
      .command = MULTI_SESSION_CONTROL_CMD_GET_STREAM_VOLUME,
      .get_stream_volume.cb = cb,
      .get_stream_volume.stream = stream,
  };

  multi_session_control(&control);
#endif
}

void multi_session_inc_global_volume(char *stream)
{
#ifdef CONFIG_MEDIA
  LOG_D("inc stream %s\n", stream);
  multi_session_control_t control = {
      .handle = NULL,
      .command = MULTI_SESSION_CONTROL_CMD_INC_STREAM_VOLUME,
      .set_stream_volume.stream = stream};

  multi_session_control(&control);
#endif
}

void multi_session_dec_global_volume(char *stream)
{
#ifdef CONFIG_MEDIA
  LOG_D("dec stream %s\n", stream);
  multi_session_control_t control = {
      .handle = NULL,
      .command = MULTI_SESSION_CONTROL_CMD_DEC_STREAM_VOLUME,
      .set_stream_volume.stream = stream};

  multi_session_control(&control);
#endif
}

void multi_session_set_mute_mode(uint8_t mode)
{
#ifdef CONFIG_MEDIA
  LOG_D("set mute mode %d\n", mode);
  multi_session_control_t control = {
      .handle = NULL,
      .command = MULTI_SESSION_CONTROL_CMD_SET_MUTE_MODE,
      .set_mute_mode.mute = mode,
  };
  multi_session_control(&control);
#endif
}

void multi_session_set_mic_mode(uint8_t mode)
{
#ifdef CONFIG_MEDIA
  LOG_D("set mic mode %d\n", mode);
  multi_session_control_t control = {
      .handle = NULL,
      .command = MULTI_SESSION_CONTROL_CMD_SET_MIC_MODE,
      .mic_mode.mode = mode};
  multi_session_control(&control);
#endif
}

void multi_session_get_mute_mode(multi_session_notify_t cb)
{
#ifdef CONFIG_MEDIA
  LOG_D("get mute mode\n");
  multi_session_control_t control = {
      .handle = NULL,
      .command = MULTI_SESSION_CONTROL_CMD_GET_MUTE_MODE,
      .get_mute_mode.cb = cb};
  multi_session_control(&control);
#endif
}

void multi_session_set_btsco_use(uint8_t use)
{
#ifdef CONFIG_MEDIA
  LOG_D("set sco %d\n", use);
  multi_session_control_t control = {
      .handle = NULL,
      .command = MULTI_SESSION_CONTROL_CMD_SET_SCO_USE,
      .sco.use = use};
  multi_session_control(&control);
#endif
}

static void test_media_control(multi_session_control_t *control)
{
  if (control->command != MULTI_SESSION_CONTROL_CMD_PLAY &&
      control->command != MULTI_SESSION_CONTROL_CMD_OPEN &&
      control->command != MULTI_SESSION_CONTROL_CMD_RECORDER_OPEN &&
      control->command != MULTI_SESSION_CONTROL_CMD_RECORDER_BEGIN &&
      control->command != MULTI_SESSION_CONTROL_CMD_FOCUS_REQUEST &&
      control->command != MULTI_SESSION_CONTROL_CMD_FOCUS_ABANDON)
    {
      if (multi_session_get_index_by_handle(control->handle) == -1)
        {
          LOG_E("unknown index, cmd 0x%02x, handle %p\n",
                control->command, control->handle);
          return;
        }
    }

  for (uint8_t i = 0; i < nitems(multi_session_control_data); i++)
    {
      if (multi_session_control_data[i].cmd == control->command)
        {
          multi_session_control_data[i].cb(control);
          if (control->command ==
              MULTI_SESSION_CONTROL_CMD_LIST_PLAY_NEXT)
            {
              if (control->play_list.url)
                {
                  free(control->play_list.url);
                  control->play_list.url = NULL;
                }
              if (control->play_list.options)
                {
                  free(control->play_list.options);
                  control->play_list.options = NULL;
                }
            }
          return;
        }
    }

  LOG_E("media control, unknown command\n");
}

static void test_media_event(multi_session_event_t *event)
{
  LOG_D("command 0x%02x handle %p ret %d\n", event->command,
        event->handle, event->status);

  switch (event->command)
    {
    case MULTI_SESSION_EVENT_CMD_PLAY:
      if (event->play.cb)
        {
          event->play.cb(event);
        }
      return;
    case MULTI_SESSION_EVENT_CMD_OPEN:
      if (event->open.cb)
        {
          event->open.cb(event);
        }
      return;
    case MULTI_SESSION_EVENT_CMD_GET_MUTE_MODE:
      event->get_mute_mode.cb(event);
      return;
    case MULTI_SESSION_EVENT_CMD_GET_AUDIO_MODE:
      event->get_audio_mode.cb(event);
      return;
    case MULTI_SESSION_EVENT_CMD_GET_STREAM_VOLUME:
      event->get_stream_volume.cb(event);
      return;
    case MULTI_SESSION_EVENT_CMD_RECORDER_BEGIN:
      if (event->recorder_begin.cb)
        {
          event->recorder_begin.cb(event);
        }
      return;
    case MULTI_SESSION_EVENT_CMD_RECORDER_OPEN:
      if (event->recorder_open.cb)
        {
          event->recorder_open.cb(event);
        }
      return;
    case MULTI_SESSION_EVENT_CMD_FOCUS_REQUEST:
    case MULTI_SESSION_EVENT_CMD_FOCUS_CHANGE:
      event->focus.cb(event);
      return;
    default:
      break;
    }

  uint32_t index = multi_session_get_index_by_handle(event->handle);
  if (index == 0xFFFFFFFF)
    {
      LOG_E("unknown index\n");
      return;
    }

  if (multi_session[index].cb)
    {
      multi_session[index].cb(event);
      if (event->command == MULTI_SESSION_EVENT_CMD_RECORDER_READ &&
          event->recorder_read.data)
        {
          free(event->recorder_read.data);
        }
    }
}

static void multi_session_async_send(multi_session_type_t type,
                                     void *data, uint16_t data_len)
{
  switch (type)
    {
    case MULTI_SESSION_TYPE_CONTROL:
      {
        multi_session_control_t *src = (multi_session_control_t *)data;
        if (src->command == MULTI_SESSION_CONTROL_CMD_LIST_PLAY_NEXT)
          {
            multi_session_control_t des = {0};
            memcpy((uint8_t *)&des, (uint8_t *)data, data_len);

            const char *concat_str = "concat:";
            uint16_t len =
                strlen(src->play_list.url) + strlen(concat_str) + 1;

            des.play_list.url = calloc(1, len);
            if (des.play_list.url == NULL)
              {
                LOG_E("calloc fail\n");
                return;
              }

            strcpy(des.play_list.url, concat_str);
            strcat(des.play_list.url, src->play_list.url);

            for (uint16_t i = 0; i < len; i++)
              {
                if (des.play_list.url[i] == ',')
                  {
                    des.play_list.url[i] = '|';
                  }
              }

            if (src->play_list.options)
              {
                des.play_list.options = strdup(src->play_list.options);
              }

            test_media_control(&des);
          }
        else
          {
            test_media_control((multi_session_control_t *)data);
          }
        break;
      }
    case MULTI_SESSION_TYPE_EVENT:
      test_media_event((multi_session_event_t *)data);
      break;
    default:
      LOG_E("unknown type %d\n", type);
      break;
    }
}

uv_loop_t *multi_session_loop(void) { return media_loop; }

void multi_session_control(multi_session_control_t *control)
{
  multi_session_async_send(MULTI_SESSION_TYPE_CONTROL, control,
                           sizeof(multi_session_control_t));
}

void multi_session_init(uv_loop_t *loop)
{
  memset(multi_session, 0, sizeof(multi_session));

  media_loop = loop;
  // test_multi_session_player_init();

#ifdef MULTI_SESSION_BES_AUDIO
  uv_async_queue_init(multi_session_loop(), &async_queue,
                      async_queue_cb);
#endif

  LOG_W("build %s\n", __TIMESTAMP__);
}

#ifdef CONFIG_MEDIA
static void multi_session_do_notify(void *handle, int event, int result,
                                    const char *extra)
{
  for (int i = 0; i < MULTI_SESSION_COUNT_MAX; i++)
    {
      if (avrcp_event[i].handle)
        {
          media_uv_session_notify(avrcp_event[i].handle, event, result,
                                  extra, NULL, NULL);
          return;
        }
    }
}

static void multi_session_cb(void *cookie, int event, int ret,
                             const char *data)
{
  for (int i = 0; i < MULTI_SESSION_COUNT_MAX; i++)
    {
      if (avrcp_event[i].use && avrcp_event[i].event_cb)
        {
          LOG_I("avrcp event %d %d\n", event, ret);
          avrcp_event[i].event_cb(event);
        }
    }
}
#endif

int test_session_avrcp_event_register(multi_session_avrcp_cb_t event_cb)
{
#ifdef CONFIG_MEDIA
  for (int i = 0; i < MULTI_SESSION_COUNT_MAX; i++)
    {
      if (avrcp_event[i].use == false)
        {
          avrcp_event[i].handle = media_uv_session_register(
              multi_session_loop(), MEDIA_STREAM_MUSIC, multi_session_cb,
              NULL);

          if (avrcp_event[i].handle == NULL)
            {
              LOG_E("session register NULL\n");
              return -1;
            }
          avrcp_event[i].use = true;
          avrcp_event[i].event_cb = event_cb;
          LOG_I("register success\n");
          return i;
        }
    }
#endif
  LOG_E("register fail\n");
  return -1;
}

void test_session_avrcp_event_update(int event)
{
#ifdef CONFIG_MEDIA
  media_metadata_t data = {0};

  for (int i = 0; i < MULTI_SESSION_COUNT_MAX; i++)
    {
      if (avrcp_event[i].handle)
        {
          if (event == MULTI_SESSION_EVENT_CMD_START)
            {
              data.flags = MEDIA_METAFLAG_STATE;
              data.state = 1;
              media_uv_session_update(avrcp_event[i].handle, &data, NULL,
                                      NULL);
            }
          else if (event == MULTI_SESSION_EVENT_CMD_STOP ||
                   event == MULTI_SESSION_EVENT_CMD_PAUSE ||
                   event == MULTI_SESSION_EVENT_CMD_COMPLETE)
            {
              data.flags = MEDIA_METAFLAG_STATE;
              data.state = 0;
              media_uv_session_update(avrcp_event[i].handle, &data, NULL,
                                      NULL);
            }
          return;
        }
    }
#endif
}

#ifdef CONFIG_MEDIA
static void media_avrcp_close_cb(void *cookie, int ret)
{
  LOG_I("%p %d\n", cookie, ret);
}
#endif

void test_session_avrcp_event_unregister(int index)
{
#ifdef CONFIG_MEDIA
  if (index >= 0 && index < MULTI_SESSION_COUNT_MAX)
    {
      if (avrcp_event[index].handle)
        {
          media_uv_session_unregister(avrcp_event[index].handle,
                                      media_avrcp_close_cb);
        }
      avrcp_event[index].event_cb = NULL;
      avrcp_event[index].handle = NULL;
      avrcp_event[index].use = false;
      LOG_E("unregister success\n");
      return;
    }
#endif
  LOG_E("unregister fail\n");
}
