/**
 * @file video_widget_api_test.h
 *
 */

#ifndef VIDEO_WIDGET_API_TEST_H
#define VIDEO_WIDGET_API_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "graphics_test.h"

/*********************
 *      DEFINES
 *********************/


/**********************
 * helper method
 **********************/
static void video_quick_close_open_cb(lv_timer_t *t);

/**********************
 * testcase
 **********************/
int video_test_controller_1(int argc, char *opt[]);
int video_quick_close_open(int argc, char *opt[]);
int video_basic_play(int argc, char *opt[]);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*VIDEO_WIDGET_API_TEST_H*/
