/**
 * @file video_test_controller.h
 *
 */

#ifndef VIDEO_TEST_CONTROLLER_H
#define VIDEO_TEST_CONTROLLER_H

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
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_obj_t obj;
    lv_obj_t* video;
    lv_obj_t* play_imgbtn;
    lv_obj_t* progress_slider;
    lv_obj_t* dur_label;
} video_test_controller_t;

extern const lv_obj_class_t video_test_controller_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t* video_test_controller_create(lv_obj_t* parent);

void video_test_controller_set_imgbtn(lv_obj_t* obj, const void* play_img, const void* pause_img);

void video_test_controller_set_src(lv_obj_t* obj, const char* src);
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LVX_VIDEO_CONTROLLER_H*/
