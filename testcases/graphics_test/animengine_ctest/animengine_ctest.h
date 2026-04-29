/*
 * @file animengine_test.h
 *
 */

#ifndef ANIMENGINE_CTEST_H
#define ANIMENGINE_CTEST_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "lvgl/lvgl.h"
#include <lvx_animengine_adapter.h>
#include "graphics_test.h"
// #include "capi_demo.h"
#include "anim_api.h"
#include <string.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Testcases
 ****************************************************************************/

int animengine_c_api_test(int argc, char *argv[]);
int animengine_c_normal_test(int argc, char *argv[]);
int animengine_c_images_test(int argc, char *argv[]);
int animengine_c_specification_test(int argc, char *argv[]);

// LV_IMG_DECLARE(LV_SYMBOL_IMAGE);
// int animengine_specification_test(int argc, char *argv[]);
// int animengine_stability(int argc, char *argv[]);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*ANIMENGINE_CEST_H*/
