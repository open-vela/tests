/**
 * @file lv_demos.h
 *
 */

#ifndef LV_DEMOS_H
#define LV_DEMOS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <errno.h>
#include <lvgl/lvgl.h>
#include <nuttx/config.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

#if CONFIG_VECTOR_GRAPHIC_TEST
#include "lv_demo_vector_graphic_test.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

// /**
//  * Call lv_demo_xxx.
//  * @param   info the information which contains demo name and parameters
//  *               needs by lv_demo_xxx.
//  * @size    size of information.
//  */
// bool lv_demos_create(char * info[], int size);

// /**
//  * Show help for lv_demos.
//  */
// void lv_demos_show_help(void);

/**********************
 *      MACROS
 **********************/
#define PASSED 0
#define FAILED 1

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DEMO_H*/
