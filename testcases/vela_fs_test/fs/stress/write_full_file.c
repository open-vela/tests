#include <nuttx/config.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include "test.h"

/****************************************************************************
 * Name: stress
 * Example description:
     1. Write a file until the disk is used up.
     2. Check that disk usage is as expected.
 * Test item: fopen() fwrite()
 * Expect results: TEST PASSED
 ****************************************************************************/

static int test_flag = 0;
#define LOOP_CNT 2000

static void do_test(void)
{
    int rval;
    FILE *fp;
    clock_t start, finish;
    char buf[512];
    double duration;
    int count = 0;

    memset(buf, '%', sizeof(buf));
    start = clock();
    do
    {
        if((count%LOOP_CNT) == 0) {
          syslog(LOG_INFO, "%dth round start\n",++count);
        }
        if ((fp = fopen("fullTestFile", "a+")) == NULL)
        {
            syslog(LOG_ERR, "Fail to open file! errno=%d", errno);
            test_flag = 1;
            return;
        }

        rval = fwrite(buf, sizeof(buf), 1, fp);
        fclose(fp);
        if (rval == 0)
            break;
    } while (1);
    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    syslog(LOG_INFO, "create full file, takes %f seconds\n", duration);
    return;
}

int main(int argc, FAR char *argv[])
{
    entry_process(argc, argv[1]);
    setup();
    do_test();
    result_check(test_flag);
    cleanup();
    exit(test_flag);
}
