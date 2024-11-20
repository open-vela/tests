/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <syslog.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <time.h>
#include "KvTest.h"
#include "kvdb.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: store_data
 ****************************************************************************/
static void *store_data(void *arg)
{

    char key[TEST_KEY_LENGTH] = {0};
    char data[TEST_VALUE_LENGTH] = {0};
    int ret;
    for (int k = 0; k < 100; k++)
    {
        sprintf(key, "TestNuttxKv19-%d-%d", gettid(), k);
        sprintf(data, "test_data_%s_%d_%d", __func__, gettid(), k);
        ret = property_set(key, data);
        if (ret != 0)
        {
            syslog(LOG_ERR, "Failed to insert data. The test thread number:%d Key:%s value:%s\n", gettid(), key, data);
            (*((int *)arg)) = -1;
            return NULL;
        }

        ret = property_delete(key);
        if (ret != 0)
        {
            syslog(LOG_ERR, "Failed to delete data. The test thread number:%d Key:%s value:%s\n", gettid(), key, data);
            (*((int *)arg)) = -1;
            return NULL;
        }
    }
    return NULL;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxKv19
 ****************************************************************************/
void TestNuttxKv19(FAR void **state)
{
    pthread_t nthread[10];
    int status;
    int num_thread = 3;
    int test_flag = 0;

    for (int i = 0; i < num_thread; i++)
    {
        /* creat test thread */
        status = pthread_create(&nthread[i], NULL, store_data, &test_flag);
        assert_int_equal(status, 0);
    }
    for (int j = 0; j < num_thread; j++)
        pthread_join(nthread[j], NULL);

    /* Check whether any child threads failed to run */
    assert_int_equal(test_flag, 0);
}
