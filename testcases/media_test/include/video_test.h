/**
 * @file video_widget_api_test.h
 *
 */

#ifndef VIDEO_WIDGET_API_TEST_H
#define VIDEO_WIDGET_API_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "audio_list.h"
#include "media_common_test.h"
#include <uikit/uikit.h>
#include <uv.h>

#include <ctype.h>
#include <mqueue.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include <errno.h>
#include <uikit/uikit.h>
#include <lvgl/lvgl.h>
#include <stdlib.h>
#include <uv_ext.h>

/*********************
 *      INCLUDES
 *********************/

#define VIDEO_MQ_PATH "/tmp/videotest_mq"
#define PLAYER_MQ_MSG_LEN 10

#define CHECK_PLAYING_VIDEO(p)                                             \
  {                                                                        \
    if (!p->video)                                                         \
      {                                                                    \
        syslog(LOG_ERR,                                                    \
               "%s, line %d,\
                                        check playing video obj failed\n", \
               __func__, __LINE__);                                        \
        return -1;                                                         \
      }                                                                    \
  }

/*********************
 *      DEFINES
 *********************/
typedef struct
{
  char *url;
  char *file;
  char *option;
  lv_obj_t *video;
  int time;
  int state;
  unsigned dur;
  unsigned pos;
  int isplaying;
  bool autoplay;
  int ops;
  test_song_entry_t *test_song_entry;
} video_pri;

/**********************
 * helper method
 **********************/

/**********************
 * testcase
 **********************/

void video_start_thread(void);
static void mediatest_load_play_list(video_pri *pri);
int send_msg_video(video_pri *msg);

/**********************
 *      MACROS
 **********************/

#define VIDEO_CTRL_NEXT 1
#define VIDEO_CTRL_PREV 2
#define VIDEO_CTRL_PLAY 3
#define VIDEO_CTRL_PAUSE 4
#define VIDEO_CTRL_STOP 5
#define VIDEO_CTRL_SEEK 6
#define VIDEO_CTRL_RESUME 7
#define VIDEO_CTRL_VOLUME_UP 8
#define VIDEO_CTRL_VOLUME_DOWN 9

#define VIDEO_CTRL_GET_DURATION 20
#define VIDEO_CTRL_GET_POSITION 21
#define VIDEO_CTRL_GET_ISPLAYING 22

#define VIDEO_EVENT_ONEND 40

#define VIDEO_CTRL_QUIT 50

enum
{
  VIDEO_PLAYSTATE_STOP,
  VIDEO_PLAYSTATE_READY,
  VIDEO_PLAYSTATE_PLAY,
  VIDEO_PLAYSTATE_PAUSE,
};

typedef int (*video_queue_video)(video_pri *);

struct videotest_app
{
  video_pri *priv;
  video_queue_video uv_play;
};

#define SEND_COMMAND(command, media)                                    \
  do                                                                    \
    {                                                                   \
      player =                                                          \
          (struct videotest_app *)malloc(sizeof(struct videotest_app)); \
      player->uv_play = command;                                        \
      player->priv = media;                                             \
      uv_async_queue_send(&g_videotest_queue, player);                  \
  } while (0)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*VIDEO_WIDGET_API_TEST_H*/
