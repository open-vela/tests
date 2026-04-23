/****************************************************************************
 * apps/tests/velatest/scenario_testsuites/media/util/media_uv.h
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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/
#ifndef CM_UV_PLAYBACK_UTILS_H
#define CM_UV_PLAYBACK_UTILS_H

#include <pthread.h>
#include <syslog.h>
#ifdef CONFIG_LIBUV_EXTENSION
#include <uv.h>
#include <uv_ext.h>
#endif
#include <cmocka.h>
#include <media_api.h>

#define TEST_UV_MEDIA_LOG(level, fmt, args...) \
    syslog(level, "[ctest][%s:%d] " fmt, __func__, __LINE__, ##args)



#define UV_APPS 3
#define TEST_MAX_NB_SUGGESTS 10
#define SUGGEST_DELAY_TIME 500

typedef struct mediatest_uv_focus_ {
    void* stream;
    void* handle; /* Focus handle. */
    media_uv_callback on_play;
    void* on_play_cookie;
} mediatest_uv_focus_s;

typedef struct stream_suggest
{
  int suggest;
  intmax_t timestamp;
} stream_suggest;


typedef struct mediatest_info {
    void *handle;
    char *stream_type;
    unsigned int ret_position;
    float ret_volume;
    int playing;
    unsigned int latency;
    const char* ret_value;
    void* ret_query_obj;
    unsigned int ret_duration;
    char *file;
    char *option;
    char *url;
    int loop;
    pthread_t thread;

    uv_loop_t *uv_loop;

    int suggest_count;
    stream_suggest suggests[TEST_MAX_NB_SUGGESTS];

    int fd;
    char *buf;
    int size;
    uv_pipe_t *pipe;
    uv_fs_t fs_req;
    uv_write_t write_req;

    mediatest_uv_focus_s *focus_info;
    char* focus;
    int pending_stop;
    unsigned int position;
    float volume;

    sem_t sem;
    const char* wait_operation;
    sem_t wait_prepare_sem;
    sem_t wait_start_sem;
    sem_t wait_pause_sem;
    sem_t wait_stop_sem;
    sem_t wait_seek_sem;
    sem_t wait_complete_sem;
    int last_event;
    int result;
    int last_ret;
    int test_event_count;
    int test_event;
    bool is_cb;
    char* key;
    void *cookie;
    char* target;
    char* value;
    int error;
} mediatest_info_s;


struct mediatest_uv_data
{
    uv_loop_t *uv_loop;
    pthread_t thread;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    bool uv_running;
    mediatest_info_s mediatest_info[UV_APPS];
};

int mediatest_uvplayer_setup(FAR void **state);
int mediatest_uvplayer_teardown(FAR void **state);

int mediatest_uvplayer_qsend(mediatest_info_s *player_state);
int mediatest_uvplayer_open(mediatest_info_s *player_state);
int mediatest_uvplayer_prepare(mediatest_info_s *player_state);
int mediatest_uvplayer_start(mediatest_info_s *player_state);
int mediatest_uvplayer_stop(mediatest_info_s *player_state);
int mediatest_uvplayer_close(mediatest_info_s *player_state);
int mediatest_uvplayer_listen(mediatest_info_s *player_state);
int mediatest_uvplayer_pause(mediatest_info_s *player_state);
int mediatest_uvplayer_seek(mediatest_info_s *player_state);
int mediatest_uvplayer_set_volume(mediatest_info_s *player_state);
int mediatest_uvplayer_get_volume(mediatest_info_s *player_state);
int mediatest_uvplayer_get_position(mediatest_info_s *player_state);
int mediatest_uvplayer_get_duration(mediatest_info_s *player_state);
int mediatest_uvplayer_get_playing(mediatest_info_s *player_state);
int mediatest_uvplayer_get_latency(mediatest_info_s *player_state);
int mediatest_uvplayer_set_looping(mediatest_info_s *player_state);
int mediatest_uvplayer_set_property(mediatest_info_s *player_state);
int mediatest_uvplayer_get_property(mediatest_info_s *player_state);
int mediatest_uvplayer_query(mediatest_info_s *player_state);
int mediatest_uvplayer_reset(mediatest_info_s *player_state);
int mediatest_uvplayer_data_init(struct mediatest_uv_data *media);
void mediatest_uvplayer_reset_sem(mediatest_info_s *player_state, const char *op);
void mediatest_uvplayer_ret_wait(mediatest_info_s *player_state, int timeout);
void mediatest_uvplayer_event_wait(mediatest_info_s *player_state, int event, int timeout);
void mediatest_uvplayer_event_wait_error(mediatest_info_s *player_state, int event, int timeout);
void mediatest_uvplayer_reset_event_sem(void *cookie, int event);
int mediatest_uv_result_deal(mediatest_info_s *player_state, int result);
void mediatest_uvplayer_ret_wait_l(mediatest_info_s *player_state,
    int timeout, int flag);

void *media_utils_uvplayer_thread(void *arg);


int mediatest_uvrecorder_open(mediatest_info_s *player_state);
int mediatest_uvrecorder_prepare(mediatest_info_s *player_state);
int mediatest_uvrecorder_start(mediatest_info_s *player_state);
int mediatest_uvrecorder_start_auto(mediatest_info_s *player_state);
int mediatest_uvrecorder_stop(mediatest_info_s *player_state);
int mediatest_uvrecorder_close(mediatest_info_s *player_state);
int mediatest_uvrecorder_listen(mediatest_info_s *player_state);
int mediatest_uvrecorder_pause(mediatest_info_s *player_state);
void *media_utils_uvrecorder_thread(void *arg);

#endif /* CM_UV_PLAYBACK_UTILS_H */
