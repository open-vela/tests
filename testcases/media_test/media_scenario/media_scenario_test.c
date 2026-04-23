#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mount.h>
#include <cmocka.h>

#include "media_test_utils.h"

#ifdef CONFIG_CM_SCENARIO_PLAYBACK_TEST
#include "test_scenario_playback.h"
#endif


#ifdef CONFIG_CM_SCENARIO_UV_INTERACT_TEST
#include "test_scenario_uv_interact.h"
#endif
int main(int argc, char* argv[])
{
      const struct CMUnitTest VelaMediaAutoTestSuite[] =
      {
            #ifdef CONFIG_CM_SCENARIO_PLAYBACK_TEST
            CM_MEDIA_SCENARIO_PLAYBACK_TESTCASES
            #endif

            #ifdef CONFIG_CM_SCENARIO_UV_INTERACT_TEST
            CM_MEDIA_SCENARIO_INTERACT_TESTCASES
            #endif
            };


      /* Run Test cases */

      cmocka_run_group_tests(VelaMediaAutoTestSuite, NULL, NULL);
      return 0;
}