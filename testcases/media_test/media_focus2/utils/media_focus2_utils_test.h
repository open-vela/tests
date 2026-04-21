/****************************************************************************
 * tests/testcases/media_test/include/media_focus2_test.h
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef TEST_MEDIA_FOCUS2_UTILS_TEST_H
#define TEST_MEDIA_FOCUS2_UTILS_TEST_H

#include <sys/mount.h>
#include <linux/fs.h>
#include <threads.h>
#ifdef CONFIG_LIBUV_EXTENSION
#include <uv.h>
#include <uv_ext.h>
#endif
#include <cmocka.h>
#include <pthread.h>


#define UV_FOCUS2_EXECUTE(command, priv)                                      \
  do                                                                    \
    {                                                                   \
      uv_focus2_player *player_test = NULL;                      \
      player_test =                                                     \
          (uv_focus2_player *)malloc(sizeof(uv_focus2_player));      \
      player_test->uv_player = command;                                   \
      player_test->priv = priv;                                        \
      uv_async_queue_send(&priv->uvasyncq, player_test);                            \
  } while (0)

typedef struct stream_suggest {
    int suggest;
    intmax_t timestamp;
} stream_suggest;

typedef struct stream_chain_s {
    char *stream;
    bool ready;
    void *handle;
    int ret;
    int nb_suggests;
    pthread_t thread;
    int type;
    bool isabandoned;
    stream_suggest suggests[20];
    void *cookie;
} stream_chain_s;

typedef struct uvfocus2_priv_s {
    pthread_t uv_thread;
    stream_chain_s chain[3];
    uv_loop_t *loop;
    uv_timer_t timer_handles[4];
    int app_id;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    uv_async_queue_t uvasyncq;
    bool uv_running;
} uvfocus2_priv_s;

typedef int (*media_focus_player_test)(uvfocus2_priv_s *);

typedef struct uvfocus2_player_s{
  uvfocus2_priv_s *priv;
  media_focus_player_test uv_player;
} uv_focus2_player;

void init_uvfocus2_priv(uvfocus2_priv_s *priv);
void focus2_uv_change_callback(int play_ret, int req_id, void *cookie);
void focus2_uv_change_reply_callback(int play_ret, int req_id, void *cookie);

void focus2_change_reply_callback(int play_ret, int req_id, void *cookie);
void focus2_change_delay_reply_callback(int play_ret, int req_id, void *cookie);
void focus2_change_delay_callback(int play_ret, int req_id, void *cookie);
void focus2_change_multi_reply_callback(int play_ret, int req_id, void *cookie);

int test_uv_focus2_common_setup(FAR void **state);
int test_uv_focus2_common_teardown(FAR void **state);
void focus2_change_callback(int play_ret, int req_id, void *callback_argv);
void focus2_uv_common_close_cb(void *cookie, int ret);
int test_focus2_common_setup(FAR void **state);
int test_focus2_common_teardown(FAR void **state);
void media_timespec_sub(struct timespec *result, const struct timespec *x, const struct timespec *y);

void *media_run_uv_loop(void *arg);
void media_close_active_handle(uv_handle_t *handle, void *arg);
void media_on_close(uv_handle_t *handle);


void *media_utils_focus2_thread(void *arg);
void mediatest_trace_suggest(stream_chain_s *ctx, int suggest);

int media_utils_uvfocus2_common_abandon(uvfocus2_priv_s *priv);
void focus2_uv_change_reply_overtime_callback(int play_ret, int req_id,
                                     void *cookie);
void focus2_uv_change_overtime_callback(int play_ret, int req_id,
                                     void *cookie);
void focus2_uv_multi_reply_callback(int play_ret, int req_id,
                                     void *cookie);
int test_focus2_hyper_teardown(FAR void **state);
#endif  /* TEST_MEDIA_FOCUS2_UTILS_TEST_H */