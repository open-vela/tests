#ifndef TEST_SCENARIO_PLAYBACK_H
#define TEST_SCENARIO_PLAYBACK_H

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <cmocka.h>

#include <media_api.h>
#include "media_test_utils.h"


#ifdef CONFIG_CM_SCENARIO_PLAYBACK_TEST
#define CM_MEDIA_SCENARIO_PLAYBACK_TESTCASES \
    cmocka_unit_test_setup_teardown( \
        test_mul_pause_time, \
        mediatest_sc_common_setup, \
        mediatest_sc_common_teardown), \
    cmocka_unit_test_setup_teardown( \
        test_mul_complete_stop, \
        mediatest_sc_common_setup, \
        mediatest_sc_common_teardown)

#else
#define CM_MEDIA_SCENARIO_PLAYBACK_TESTCASES
#endif


void test_mul_pause_time(void **state);
void test_mul_complete_stop(void **state);

#endif // TEST_SCENARIO_PLAYBACK_H