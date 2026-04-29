#include "video_test.h"
#include <uikit/uikit.h>
#include "video_test_controller.h"

/**********************
 *   DEFINE
 **********************/

#define PLAY_BUTTON "/data/play.png"
#define PAUSE_BUTTON "/data/pause.png"

/**********************
 *   common variable
 **********************/
typedef struct
{
  char *url;
  lv_obj_t *video;
} video_pri;

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void video_quick_close_open_cb(lv_timer_t *t)
{
  video_pri *video_data = t->user_data;

  lv_obj_del(video_data->video);

  lv_obj_t *video1 = vg_video_create(lv_scr_act());

  vg_video_set_src(video1, video_data->url);
  lv_obj_align(video1, LV_ALIGN_TOP_LEFT, 0, 0);
  vg_video_start(video1);
}

/**********************
 *   TESTCASE
 **********************/

int video_quick_close_open(int argc, char *opt[])
{
  video_pri *video_data;
  video_pri data = {.url = opt[2], .video = NULL};
  video_data = &data;
  video_data->video = vg_video_create(lv_scr_act());

  lv_timer_t *timer =
      lv_timer_create(video_quick_close_open_cb, 12 * 1000, video_data);
  lv_timer_set_repeat_count(timer, 1);
  lv_timer_pause(timer);

  vg_video_set_src(video_data->video, video_data->url);

  lv_obj_align(video_data->video, LV_ALIGN_CENTER, 0, 0);

  vg_video_start(video_data->video);

  lv_timer_resume(timer);
  return 0;
}

int video_basic_play(int argc, char *opt[])
{
  video_pri *video_data;
  video_pri data = {.url = opt[2], .video = NULL};
  video_data = &data;
  video_data->video = vg_video_create(lv_scr_act());
  video_data->video = vg_video_create(lv_scr_act());

  vg_video_set_src(video_data->video, video_data->url);

  lv_obj_align(video_data->video, LV_ALIGN_CENTER, 0, 0);

  vg_video_start(video_data->video);

  return 0;
}

int video_test_controller_1(int argc, char *opt[])
{
  video_pri *video_data;
  video_pri data = {.url = opt[2], .video = NULL};
  video_data = &data;

  video_data->video = video_test_controller_create(lv_scr_act());

  lv_obj_set_size(video_data->video, LV_PCT(100), LV_PCT(100));

  video_test_controller_set_src(video_data->video, video_data->url);

  video_test_controller_set_imgbtn(video_data->video, PLAY_BUTTON,
                                   PAUSE_BUTTON);

  lv_obj_align(video_data->video, LV_ALIGN_CENTER, 0, 0);
  return 0;
}