/****************************************************************************
 * tests/testcases/media_test/include/media_graph_test.h
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 *The ASF licenses this file to you under the Apache License, Version 2.0
 *(the "License"); you may not use this file except in compliance with
 *the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 *WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 *implied.  See the License for the specific language governing
 *permissions and limitations under the License.
 *
 ****************************************************************************/

#ifndef __MEDIAGRAPHTEST_H
#define __MEDIAGRAPHTEST_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "audio_list.h"
#include <ctype.h>
#include <getopt.h>
#include <media_api.h>
#include <nuttx/config.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#ifdef CONFIG_AUDIOUTILS_ALSA_LIB
#include <alsa/asoundlib.h>
#endif

#ifdef CONFIG_LIBUV_EXTENSION
#include <uv.h>
#include <uv_ext.h>
#endif
#define PLAYER_IDLE 0
#define PLAYER_PREPARED 1
#define PLAYER_STARTED 2
#define PLAYER_PAUSED 3
#define PLAYER_COMPLETED 4
#define PLAYER_STOPPED 5
#define PLAYER_GET_POSITION 6
#define PLAYER_GET_DURATION 7
#define PLAYER_GET_VOLUME 8
#define PLAYER_GET_PLAYING 9
#define PLAYER_OPENED 10
#define PLAYER_SET_LOOP 11
#define PLAYER_SEEKED 12
#define PLAYER_CLOSED 13

#define MODE_BUFFER 0
#define MODE_URL 1
#define MODE_DIRECT 2

#define MEDIATEST_PLAYER 1
#define MEDIATEST_RECORDER 2
#define MEDIATEST_CONTROLLER 3
#define MEDIATEST_CONTROLLEE 4
#define MEDIATEST_FOCUS 5
#define MEDIATEST_UVPLAYER 6
#define MEDIATEST_UVRECORDER 7
#define MEDIATEST_UVFOCUS 8
#define MEDIATEST_CAMERA 9
#define MEDIATEST_VIDEO 10

/* Check the result value of function */
#define FUN_CHECK(ret, media, action, need_action, str, args...)        \
  do                                                                    \
    {                                                                   \
      if (ret < 0)                                                      \
        {                                                               \
          syslog(LOG_ERR, str, ##args);                                 \
          mediatest_dump();                                             \
          sleep(1);                                                     \
          if (need_action)                                              \
            action(media);                                              \
          free(media);                                                  \
          media = NULL;                                                 \
          return 0;                                                     \
        }                                                               \
  } while (0)

#define STATE_CHECK(stat, media, action, str, args...)                  \
  do                                                                    \
    {                                                                   \
      time_t t0 = time(NULL);                                           \
      int count_ = 0;                                                   \
      while (media->state != stat)                                      \
        {                                                               \
          usleep(100);                                                  \
          count_++;                                                     \
          if (count_ > 80000)                                           \
            {                                                           \
              syslog(LOG_WARNING,                                       \
                     "media event callback wait time failed\n");        \
              mediatest_dump();                                         \
              sleep(1);                                                 \
              action(media);                                            \
              free(media);                                              \
              media = NULL;                                             \
              return -1;                                                \
            }                                                           \
        }                                                               \
      media->state = PLAYER_IDLE;                                       \
      time_t t1 = time(NULL);                                           \
      syslog(LOG_INFO, "the stat %d wait time is %lld\n", stat,         \
             (long long int)(t1 - t0));                                 \
      if (media->ret < 0)                                               \
        {                                                               \
          syslog(LOG_ERR, str, ##args);                                 \
          mediatest_dump();                                             \
          sleep(1);                                                     \
          action(media);                                                \
          free(media);                                                  \
          media = NULL;                                                 \
          return -1;                                                    \
        }                                                               \
  } while (0)

#define RET_CHECK(ret, str, args...)                                    \
  do                                                                    \
    {                                                                   \
      if (ret < 0)                                                      \
        {                                                               \
          syslog(LOG_ERR, str, ##args);                                 \
          goto out;                                                     \
        }                                                               \
  } while (0);

#define ST_CHECK(stat, media, str, args...)                             \
  do                                                                    \
    {                                                                   \
      time_t t0 = time(NULL);                                           \
      int count_ = 0;                                                   \
      while (media->state != stat)                                      \
        {                                                               \
          usleep(100);                                                  \
          count_++;                                                     \
          if (count_ > 80000)                                           \
            {                                                           \
              syslog(LOG_WARNING,                                       \
                     "media event callback wait time failed\n");        \
              mediatest_dump();                                         \
              sleep(1);                                                 \
              goto out;                                                 \
            }                                                           \
        }                                                               \
      media->state = PLAYER_IDLE;                                       \
      time_t t1 = time(NULL);                                           \
      syslog(LOG_INFO, "the stat %d wait time is %lld\n", stat,         \
             (long long int)(t1 - t0));                                 \
      if (media->ret < 0)                                               \
        {                                                               \
          syslog(LOG_ERR, str, ##args);                                 \
          goto out;                                                     \
        }                                                               \
  } while (0);

#define UV_WAIT(stat, media, str, args...)                              \
  do                                                                    \
    {                                                                   \
      int count_ = 0;                                                   \
      while (media->uv_waiting != stat)                                 \
        {                                                               \
          usleep(100 * 1000);                                           \
          count_++;                                                     \
          if (count_ > 60)                                              \
            {                                                           \
              syslog(LOG_WARNING,                                       \
                     "media uv event callback wait time failed\n");     \
              mediatest_dump();                                         \
              sleep(1);                                                 \
              goto out;                                                 \
            }                                                           \
        }                                                               \
      media->uv_waiting = PLAYER_IDLE;                                  \
      if (media->ret < 0)                                               \
        {                                                               \
          syslog(LOG_ERR, str, ##args);                                 \
          mediatest_dump();                                             \
          sleep(1);                                                     \
          goto out;                                                     \
        }                                                               \
  } while (0)

#define UV_EXECUTE(command, media)                                      \
  do                                                                    \
    {                                                                   \
      player_test =                                                     \
          (struct mediatest_app *)malloc(sizeof(struct mediatest_app)); \
      player_test->uv_play = command;                                   \
      player_test->priv = media;                                        \
      mediatest_uv_player_exec(player_test);                            \
  } while (0)

#define GET_TIMESTAMP()                                                 \
  do                                                                    \
    {                                                                   \
      FAR struct timespec ts;                                           \
      clock_gettime(CLOCK_MONOTONIC, &ts);                              \
      int64_t timestamp =                                               \
          (int64_t)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;             \
      syslog(LOG_INFO, "media func %s ,line:%d, timestamp:%" PRId64 "\n", \
             __func__, __LINE__, timestamp);                            \
  } while (0)

struct mediatest_data
{
  int type;
  void *handle;
  pthread_t thread;
  int fd;
  char *buf;
  int size;
  int ret;
  int rept;
  char *stream_type;
  char *url;
  int mode;
  bool stop_flag;
  int state;
  int loop;
  unsigned int position;
  float volume;
  char *option;
  bool complete;
  int time;
  void *extra;
  int act;
  int stat;
  unsigned int duration;
  int pending_stop;
  int playing;
  int uv_waiting;
  char *file;
  char *focus_type;
  test_song_entry_t *test_song_entry;
  int ex;
#ifdef CONFIG_LIBUV_EXTENSION
  uv_pipe_t *pipe; /* pipe handle object */
  uv_fs_t fs_req;
  uv_write_t write_req;
#endif
};

typedef int (*media_uv_player_test)(struct mediatest_data *);
typedef void (*mediatest_alarm_cb)(int);

struct mediatest_app
{
  struct mediatest_data *priv;
  media_uv_player_test uv_play;
};

#ifdef CONFIG_LIBUV_EXTENSION
static uv_loop_t *g_mediatest_uvloop __attribute__((unused));
static uv_async_queue_t g_mediatest_uvasyncq __attribute__((unused));
#endif

#ifdef CONFIG_AUDIOUTILS_ALSA_LIB
typedef struct
{
  char *url;
  snd_pcm_t *handle;
  int bytes_per_frame;
  int sample_rate;
  bool complete;
  int channels;
  int bits_per_sample;
  char *device;
  int volume;
  char *file;
  int interval;
} mediatest_alsa_t;
#endif

/* function */
int mediatest_player_open(struct mediatest_data *media);
int mediatest_recorder_open(struct mediatest_data *media);
int mediatest_common_open(struct mediatest_data *media);
int mediatest_common_close(struct mediatest_data *media);
int mediatest_common_reset(struct mediatest_data *media);
int mediatest_common_start(struct mediatest_data *media);
void mediatest_event_callback(void *cookie, int event, int ret,
                              const char *data);
int mediatest_common_prepare(struct mediatest_data *media);
int mediatest_common_stop(struct mediatest_data *media);
void mediatest_common_stop_thread(struct mediatest_data *media);
int mediatest_player_isplaying(struct mediatest_data *media);
int mediatest_common_pause(struct mediatest_data *media);
int mediatest_player_set_volume(struct mediatest_data *media);
int mediatest_player_get_volume(struct mediatest_data *media);
int mediatest_player_loop(struct mediatest_data *media);
void *mediatest_common_thread(void *arg);
int mediatest_setup(struct mediatest_data *media);
int mediatest_duration(struct mediatest_data *media);
int mediatest_position(struct mediatest_data *media);
int mediatest_seek(struct mediatest_data *media);
int mediatest_process_data(int sockfd, bool player, void *data,
                           size_t len);
int mediatest_getopt(int argc, char *argv[],
                     struct mediatest_data *media);
void show_usages(struct mediatest_data *media);
int mediatest_common_prepare_retry(struct mediatest_data *media,
                                   int retry);
int mediatest_session_open(struct mediatest_data *media);
int mediatest_session_prevsong(struct mediatest_data *media);
int mediatest_session_nextsong(struct mediatest_data *media);
int mediatest_recorder_take_picture(struct mediatest_data *media);
int mediatest_send(char *target, char *cmd, char *pargs);
int mediatest_dump(void);
void mediatest_common_stop_thread(struct mediatest_data *media);
int mediatest_session_register(struct mediatest_data *media);
int mediatest_playdtmf(struct mediatest_data *media, char *dial_number);
int mediatest_setint(const char *name, int value, int apply);
int mediatest_getint(char *name, int *value);
int mediatest_setstring(char *name, char *value, int apply);
int mediatest_getstring(char *name, char *value);
int mediatest_include(char *name, char *value, int apply);
int mediatest_exclude(char *name, char *value, int apply);
int mediatest_contain(char *name, char *value);
int mediatest_increase(char *name, int apply);
int mediatest_decrease(char *name, int apply);
int mediatest_focus_request(struct mediatest_data *media, char *name);
int mediatest_uv_player_enter(void);
int mediatest_app_init(struct mediatest_data *media);
int mediatest_app_stop(struct mediatest_data *media);
int mediatest_app_reset(struct mediatest_data *media);
int mediatest_app_exit(struct mediatest_data *media);
int mediatest_app_playorpause(struct mediatest_data *media);
int mediatest_app_next(struct mediatest_data *media);
int mediatest_app_prev(struct mediatest_data *media);
#ifdef CONFIG_LIBUV_EXTENSION

int mediatest_uv_player_open(struct mediatest_data *media);
void *mediatest_uvloop_thread(void *arg);
void mediatest_uvasyncq_cb(uv_async_queue_t *asyncq, void *data);
void mediatest_uvasyncq_close_cb(uv_handle_t *handle);
int mediatest_uv_player_exec(struct mediatest_app *player_test);
int mediatest_uv_recorder_open(struct mediatest_data *media);
int mediatest_uv_policy_set_int(char *name, int value, int apply);
int mediatest_uv_policy_get_int(struct mediatest_data *media);
int mediatest_uv_policy_set_string(char *name, char *value, int apply);
int mediatest_uv_policy_get_string(char *name);
int mediatest_uv_policy_increase(char *name, int apply);
int mediatest_uv_policy_decrease(char *name, int apply);
int mediatest_uv_focus_request(struct mediatest_data *media);
int mediatest_uv_policy_set_stream_volume(struct mediatest_data *media);
int mediatest_uv_policy_get_stream_volume(struct mediatest_data *media);
int mediatest_uv_policy_increase_stream_volume(
    struct mediatest_data *media);
int mediatest_uv_policy_decrease_stream_volume(
    struct mediatest_data *media);
int mediatest_app_dur_pos(struct mediatest_data *media);
int mediatest_app_pause(struct mediatest_data *media);
int mediatest_app_play(struct mediatest_data *media);
int mediatest_uv_exit(void);
int mediatest_uv_quit(struct mediatest_data *media);
static void mediatest_uv_player_connection_cb(void *cookie, int ret,
                                              void *obj);
static void mediatest_uv_player_read_cb(uv_fs_t *req);
static void mediatest_uv_recorder_connection_cb(void *cookie, int ret,
                                                void *obj);
static void mediatest_uv_player_get_volume_cb(void *cookie, int ret,
                                              float val);
static void mediatest_uv_player_set_loop_cb(void *cookie, int ret);
#endif

int mediatest_close_timer(timer_t timeid);
timer_t mediatest_start_timer(mediatest_alarm_cb cb, long time_val);
void mediatest_load_play_list(struct mediatest_data *media);

#ifdef CONFIG_AUDIOUTILS_ALSA_LIB
int mediatest_alsapause(int enable);
int mediatest_alsa_prepare(mediatest_alsa_t *media);
int mediatest_alsa_open(mediatest_alsa_t *media);
int mediatest_alsa_close(mediatest_alsa_t *media);
int mediatest_alsa_getopt(int argc, char *argv[],
                          mediatest_alsa_t *media);
int mediatest_alsa_volume(mediatest_alsa_t *media);
int mediatest_alsa_setup(mediatest_alsa_t *media);
#endif

#endif