#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mount.h>
#include <cmocka.h>


#ifdef CONFIG_CM_MEDIA_FOCUS2_TEST
#include "media_focus2_test.h"
#include "media_focus2_utils_test.h"
#endif

/****************************************************************************
 * Name: cmockmedia_test_main
 ****************************************************************************/

int main(int argc, char *argv[])
{
    /* Add velatest res */

// #ifdef CONFIG_CM_MEDIA_RES_UNITTEST
//       mount("NULL", CONFIG_MEDIA_AUDIO_COMMON_PATH, "hostfs", 0, CONFIG_CM_MEDIA_RES_UNITTEST);
// #endif

    /* Add Test Cases */

    const struct CMUnitTest cmocka_media_focus_test_suit[] = {
#ifdef CONFIG_CM_MEDIA_FOCUS2_TEST
        CM_MEDIA_FOCUS2_TESTCASES
#endif
    };

    /* Run Test cases */

    cmocka_run_group_tests(cmocka_media_focus_test_suit, NULL, NULL);
    return 0;
}