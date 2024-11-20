/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "KvTest.h"
#include "kvdb.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: test_string
 ****************************************************************************/
static void test_string(int run_num)
{
    int ret;
    char key[TEST_KEY_LENGTH] = {0};
    char data[TEST_VALUE_LENGTH] = {0};
    char value[TEST_VALUE_LENGTH] = {0};
    for (int i = 0; i < run_num; i++)
    {
        sprintf(key, "test_key_%s_%d", __func__, i);
        sprintf(data, "test_data_%s_%d", __func__, i);
        ret = property_set(key, data);
        assert_int_equal(ret, 0);
        ret = property_get(key, value, "");
        assert_int_in_range(ret, 0, TEST_VALUE_LENGTH + 1);
        assert_int_equal(strcmp(value, data), 0);
        property_delete(key);
    }
}

/****************************************************************************
 * Name: test_int32
 ****************************************************************************/
static void test_int32(int run_num)
{
    char key[TEST_KEY_LENGTH] = {0};
    int32_t data;
    int32_t value;
    int ret;
    for (int i = 0; i < run_num; i++)
    {
        sprintf(key, "test_key_%s_%d", __func__, i);
        data = rand() % INT32_MAX + 1;
        ret = property_set_int32(key, data);
        assert_int_equal(ret, 0);
        value = property_get_int32(key, 0);
        assert_int_equal(value, data);
        property_delete(key);
    }
}

/****************************************************************************
 * Name: test_int64
 ****************************************************************************/
#ifdef CONFIG_LIBC_LONG_LONG
static void test_int64(int run_num)
{
    char key[TEST_KEY_LENGTH] = {0};
    int64_t data;
    int64_t value;
    int ret;
    for (int i = 0; i < run_num; i++)
    {
        sprintf(key, "test_key_%s_%d", __func__, i);
        data = rand() % INT64_MAX + 1;
        ret = property_set_int64(key, data);
        assert_int_equal(ret, 0);
        value = property_get_int64(key, 0);
        assert_int_equal(value, data);
        property_delete(key);
    }
}
#endif

/****************************************************************************
 * Name: test_bool
 ****************************************************************************/
static void test_bool(int run_num)
{
    char key[TEST_KEY_LENGTH] = {0};
    int data;
    int ret;
    int value;
    for (int i = 0; i < run_num; i++)
    {
        srand((unsigned)time(NULL) + i);
        data = rand() % 2;
        sprintf(key, "test_key_%s_%d", __func__, i);
        ret = property_set_bool(key, data);
        assert_int_equal(ret, 0);
        value = property_get_bool(key, 0);
        assert_int_equal(value, data);
        property_delete(key);
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxKv06
 ****************************************************************************/
void TestNuttxKv06(FAR void **state)
{
    int test_run = 20;
    test_string(test_run);
    test_int32(test_run);
#ifdef CONFIG_LIBC_LONG_LONG
    test_int64(test_run);
#endif
    test_bool(test_run);
}