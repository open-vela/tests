/*
 * @file uikit_test.h
 *
 */

#ifndef UIKIT_TEST_H
#define UIKIT_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <errno.h>
#include <lvgl/lvgl.h>
#include <nuttx/config.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

/***************************************************************************
 * markdown test
 ****************************************************************************/
#ifdef CONFIG_MARKDOWN_TEST
#include "markdown_test.h"
#endif

#ifdef CONFIG_SVG_TEST
#include "svg_test.h"
#endif
/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define PASSED 0
#define FAILED 1

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*UIKIT_TEST_H*/
