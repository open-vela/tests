#ifndef CM_MEDIA_FOCUS2_TEST_H
#define CM_MEDIA_FOCUS2_TEST_H

#include <errno.h>
#include <nuttx/config.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <syslog.h>
#include <cmocka.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifdef CONFIG_CM_MEDIA_FOCUS2_TEST
#define CM_MEDIA_FOCUS2_TESTCASES                                       \
  cmocka_unit_test_setup_teardown(test_media_focus2_01,                 \
                                  test_focus2_common_setup,             \
                                  test_focus2_common_teardown),         \
      cmocka_unit_test_setup_teardown(test_media_focus2_02,             \
                                      test_focus2_common_setup,         \
                                      test_focus2_common_teardown),     \
      cmocka_unit_test_setup_teardown(test_media_focus2_03,             \
                                      test_focus2_common_setup,         \
                                      test_focus2_common_teardown),     \
      cmocka_unit_test_setup_teardown(test_media_focus2_04,             \
                                      test_focus2_common_setup,         \
                                      test_focus2_common_teardown),     \
      cmocka_unit_test_setup_teardown(test_media_focus2_05,             \
                                      test_focus2_common_setup,         \
                                      test_focus2_common_teardown),     \
      cmocka_unit_test_setup_teardown(test_media_focus2_06,             \
                                      test_focus2_common_setup,         \
                                      test_focus2_common_teardown),     \
      cmocka_unit_test_setup_teardown(test_media_focus2_07,             \
                                      test_focus2_common_setup,         \
                                      test_focus2_common_teardown),     \
      cmocka_unit_test_setup_teardown(test_media_focus2_08,             \
                                      test_focus2_common_setup,         \
                                      test_focus2_common_teardown),     \
      cmocka_unit_test_setup_teardown(test_media_focus2_09,             \
                                      test_focus2_common_setup,         \
                                      test_focus2_common_teardown),     \
      cmocka_unit_test_setup_teardown(test_media_focus2_10,             \
                                      test_focus2_common_setup,         \
                                      test_focus2_common_teardown),     \
      cmocka_unit_test_setup_teardown(test_media_focus2_11,             \
                                      test_focus2_common_setup,         \
                                      test_focus2_common_teardown),     \
      cmocka_unit_test_setup_teardown(test_media_focus2_12,             \
                                      test_focus2_common_setup,         \
                                      test_focus2_common_teardown),     \
      cmocka_unit_test_setup_teardown(test_media_focus2_13,             \
                                      test_focus2_common_setup,         \
                                      test_focus2_common_teardown),     \
      cmocka_unit_test_setup_teardown(test_media_uvfocus2_01,           \
                                      test_uv_focus2_common_setup,      \
                                      test_uv_focus2_common_teardown),  \
      cmocka_unit_test_setup_teardown(test_media_uvfocus2_02,           \
                                      test_uv_focus2_common_setup,      \
                                      test_uv_focus2_common_teardown),  \
      cmocka_unit_test_setup_teardown(test_media_uvfocus2_03,           \
                                      test_uv_focus2_common_setup,      \
                                      test_uv_focus2_common_teardown),  \
      cmocka_unit_test_setup_teardown(test_media_uvfocus2_04,           \
                                      test_uv_focus2_common_setup,      \
                                      test_uv_focus2_common_teardown),  \
      cmocka_unit_test_setup_teardown(test_media_uvfocus2_05,           \
                                      test_uv_focus2_common_setup,      \
                                      test_uv_focus2_common_teardown),  \
      cmocka_unit_test_setup_teardown(test_media_uvfocus2_06,           \
                                      test_uv_focus2_common_setup,      \
                                      test_uv_focus2_common_teardown),  \
      cmocka_unit_test_setup_teardown(test_media_uvfocus2_07,           \
                                      test_uv_focus2_common_setup,      \
                                      test_uv_focus2_common_teardown),  \
      cmocka_unit_test_setup_teardown(test_media_uvfocus2_08,           \
                                      test_uv_focus2_common_setup,      \
                                      test_uv_focus2_common_teardown),  \
      cmocka_unit_test_setup_teardown(test_media_uvfocus2_09,           \
                                      test_uv_focus2_common_setup,      \
                                      test_uv_focus2_common_teardown),  \
      cmocka_unit_test_setup_teardown(test_media_uvfocus2_10,           \
                                      test_uv_focus2_common_setup,      \
                                      test_uv_focus2_common_teardown),  \
      cmocka_unit_test_setup_teardown(test_media_uvfocus2_11,           \
                                      test_uv_focus2_common_setup,      \
                                      test_uv_focus2_common_teardown),  \
      cmocka_unit_test_setup_teardown(test_media_uvfocus2_12,           \
                                      test_uv_focus2_common_setup,      \
                                      test_uv_focus2_common_teardown),  \
      cmocka_unit_test_setup_teardown(test_media_uvfocus2_13,           \
                                      test_uv_focus2_common_setup,      \
                                      test_uv_focus2_common_teardown),  \
      cmocka_unit_test_setup_teardown(test_media_hyper_focus2_01,       \
                                      test_uv_focus2_common_setup,      \
                                      test_focus2_hyper_teardown),      \
      cmocka_unit_test_setup_teardown(test_media_hyper_focus2_02,       \
                                      test_uv_focus2_common_setup,      \
                                      test_focus2_hyper_teardown),      \
      cmocka_unit_test_setup_teardown(test_media_hyper_focus2_03,       \
                                      test_uv_focus2_common_setup,      \
                                      test_focus2_hyper_teardown),      \
      cmocka_unit_test_setup_teardown(test_media_hyper_focus2_04,       \
                                      test_uv_focus2_common_setup,      \
                                      test_focus2_hyper_teardown),      \
      cmocka_unit_test_setup_teardown(test_media_stab_focus2_01,        \
                                      test_focus2_common_setup,         \
                                      test_focus2_common_teardown),     \
      cmocka_unit_test_setup_teardown(test_media_stab_uvfocus2_01,      \
                                      test_uv_focus2_common_setup,      \
                                      test_uv_focus2_common_teardown),  \
      cmocka_unit_test_setup_teardown(test_media_stab_hyper_focus2_01,  \
                                      test_uv_focus2_common_setup,      \
                                      test_focus2_hyper_teardown),      \
      cmocka_unit_test_setup_teardown(test_media_stab_focus2_02,        \
                                      test_focus2_common_setup,         \
                                      test_focus2_common_teardown),     \
      cmocka_unit_test_setup_teardown(test_media_stab_uvfocus2_02,      \
                                      test_uv_focus2_common_setup,      \
                                      test_uv_focus2_common_teardown),

#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/* TEST CASES FUNCTIONS */
void test_media_focus2_01(FAR void **state);
void test_media_focus2_02(FAR void **state);
void test_media_focus2_03(FAR void **state);
void test_media_focus2_04(FAR void **state);
void test_media_focus2_05(FAR void **state);
void test_media_focus2_06(FAR void **state);
void test_media_focus2_07(FAR void **state);
void test_media_focus2_08(FAR void **state);
void test_media_focus2_09(FAR void **state);
void test_media_focus2_10(FAR void **state);
void test_media_focus2_11(FAR void **state);
void test_media_focus2_12(FAR void **state);
void test_media_focus2_13(FAR void **state);
void test_media_uvfocus2_01(FAR void **state);
void test_media_uvfocus2_02(FAR void **state);
void test_media_uvfocus2_03(FAR void **state);
void test_media_uvfocus2_04(FAR void **state);
void test_media_uvfocus2_05(FAR void **state);
void test_media_uvfocus2_06(FAR void **state);
void test_media_uvfocus2_07(FAR void **state);
void test_media_uvfocus2_08(FAR void **state);
void test_media_uvfocus2_09(FAR void **state);
void test_media_uvfocus2_10(FAR void **state);
void test_media_uvfocus2_11(FAR void **state);
void test_media_uvfocus2_12(FAR void **state);
void test_media_uvfocus2_13(FAR void **state);
void test_media_hyper_focus2_01(FAR void **state);
void test_media_hyper_focus2_02(FAR void **state);
void test_media_hyper_focus2_03(FAR void **state);
void test_media_hyper_focus2_04(FAR void **state);

void test_media_stab_focus2_01(FAR void **state);
void test_media_stab_uvfocus2_01(FAR void **state);
void test_media_stab_hyper_focus2_01(FAR void **state);
void test_media_stab_uvfocus2_02(FAR void **state);
void test_media_stab_focus2_02(FAR void **state);

#endif /* CM_MEDIA_FOCUS2_TEST_H */
