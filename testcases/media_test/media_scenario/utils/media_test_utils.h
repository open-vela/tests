#ifndef MEDIA_TEST_UTILS_H
#define MEDIA_TEST_UTILS_H

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <media_api.h>
#include <poll.h>
#include <pthread.h>
#include <semaphore.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <cmocka.h>

#define MEDIATEST_TIMEOUT 500000000
#define MEDIATEST_APPS 3

#define RES_CHECK(ret, ctx, str, args...)                               \
  do                                                                    \
    {                                                                   \
      if (ret < 0)                                                      \
        {                                                               \
          ctx->flag = -1;                                            \
          syslog(LOG_ERR, str, ##args);                                 \
          goto out;                                                     \
        }                                                               \
  } while (0);

  #define RES_GOAL(ret, ctx, str, args...)                               \
  do                                                                    \
    {                                                                   \
      if (ret < 0)                                                      \
        {                                                               \
          ctx->flag = -1;                                              \
          syslog(LOG_ERR, str, ##args);                                 \
        }                                                               \
  } while (0);

enum play_type
{
  PLAYER = 0,
  RECORDER = 1,
};

typedef struct streams_suggest
{
  int suggest;
  intmax_t timestamp;
} streams_suggest;

typedef struct streams_callback
{
  int event;
  intmax_t timestamp;
} streams_cb;

typedef struct media_test_chain_s
{
  enum play_type type;
  char *stream;
  void *handle;
  void *extra;
  char *url;
  streams_suggest suggest[20];
  streams_cb cb_trace[30];
  int nb_cb;
  int nb_suggests;

  pthread_t thread;
  int fd;

  bool start;
  bool loop;

  bool direct;
  char *buf;
  int size;
  int seek_ms;

  int last_event;
  int result;
  int flag;
  sem_t sem;
} mediatest_chain_sc;


void mediatest_play_without_req(mediatest_chain_sc *player);
void mediatest_common_event_cb(void *cookie, int event, int ret,
                               const char *data);
void test_wait_completed(mediatest_chain_sc *ctx, long long timeout);
void test_wait_seeked(mediatest_chain_sc *ctx);
void test_wait_stopped(mediatest_chain_sc *ctx);
void test_wait_paused(mediatest_chain_sc *ctx);
void test_wait_started(mediatest_chain_sc *ctx);
void test_wait_prepared(mediatest_chain_sc *ctx);
int mediatest_sc_common_teardown(FAR void **state);
int mediatest_sc_common_setup(FAR void **state);
void media_time_trace(mediatest_chain_sc *ctx);
void mediatest_sc_trace_suggest(mediatest_chain_sc *ctx, int suggest);
void mediatest_play_pause(mediatest_chain_sc *player);
void mediatest_play_stop_close(mediatest_chain_sc *player);
void media_time_callback_trace(mediatest_chain_sc *ctx, int event);

#endif
