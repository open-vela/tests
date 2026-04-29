/*
 * @file graphics_test.h
 *
 */

#ifndef GRAPHICS_TEST_H
#define GRAPHICS_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <lvgl/lvgl.h>
#include <syslog.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#ifdef CONFIG_VIDEO_WIDGET_TEST
#include "video_test.h"
#include "video_test_controller.h"
#endif

#ifdef CONFIG_ANIMATION_ENGINE_TEST
#include "animengine_test.h"
#endif

/***************************************************************************
* Animation c api test
****************************************************************************/
#ifdef CONFIG_ANIMATION_ENGINE_CTEST
#include "animengine_ctest.h"
#endif

#ifdef CONFIG_ANIM_PHYSICS_ENGINE_TEST
#include "physics_engine_test.h"
#endif

#ifdef CONFIG_GESTURES_TEST
#include "lv_gestures_test.h"
#endif

#ifdef CONFIG_RIVE_TEST
#include "rive_api_test.h"
#endif

#ifdef CONFIG_LV_BLUR_TEST
#include "lv_blur_test.h"
#endif
/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define PASSED 0
#define FAILED 1

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*GRAPHICS_TEST_H*/
