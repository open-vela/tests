#include <nuttx/config.h>

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#include <cmocka.h>

#include "fstest.h"

static int fs_run_one_case(void)
{
  const struct CMUnitTest tests[] =
  {
    cmocka_unit_test_setup_teardown(FS_CASE_FUNC,
                                    test_nuttx_fs_test_group_setup,
                                    test_nuttx_fs_test_group_teardown),
  };
  int ret;

  ret = cmocka_run_group_tests_name(FS_CASE_NAME, tests, NULL, NULL);
  printf("%s\n", ret == 0 ? "TEST PASSED" : "TEST FAILED");
  return ret;
}

int main(int argc, FAR char *argv[])
{
  (void)argc;
  (void)argv;
  return fs_run_one_case();
}
