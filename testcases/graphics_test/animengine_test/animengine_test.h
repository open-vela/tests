/*
 * @file animengine_test.h
 *
 */

#ifndef ANIMENGINE_TEST_H
#define ANIMENGINE_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <cJSON.h>
#include <animengine/lvx_animengine_adapter.h>
#include "graphics_test.h"
#include <string.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Testcases
 ****************************************************************************/

int animengine_api_test(int argc, char *argv[]);
int animengine_specification_test(int argc, char *argv[]);
int animengine_stability(int argc, char *argv[]);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*ANIMENGINE_TEST_H*/
