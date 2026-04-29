#include <nuttx/config.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/param.h>
#include <sys/statfs.h>
#include "test.h"

/****************************************************************************
 * Name: stress
 * Example description:
     1. Opening multiple files does not close until it fails.
     2. Check that the test returns results.
 * Test item: open()
 * Expect results: TEST PASSED
 ****************************************************************************/

static void rand_file_name(char s[], int num)
{
    char *str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int i, lstr;
    lstr = strlen(str);
    srand((unsigned int)time((time_t *)NULL));
    for (i = 0; i < num; i++)
    {
        s[i] = str[(rand() % lstr)];
    }
}

static int test_flag = 0;

static void do_test(void)
{
    struct statfs diskInfo;
#define MAX_FD_NUM 15
    int fd[MAX_FD_NUM];
    char s[CONFIG_NAME_MAX + 1] = {0};
    int num = 0;
    char buf[20] = {0};
    getcwd(buf, sizeof(buf));
    statfs(buf, &diskInfo);

    syslog(LOG_INFO, "the max name len : : %ld \n", (long int)diskInfo.f_namelen);
    for (int i = 0; i < MAX_FD_NUM; i++)
    {
        rand_file_name(s, MIN(i + 1, CONFIG_NAME_MAX));
        fd[i] = open(s, O_WRONLY | O_CREAT, 0700);
        if (fd[i] > 0)
        {
            syslog(LOG_INFO, "open success !  the file no. : %d, fd=%d\n", i, fd[i]);
            num = i;
        }
        else
        {
            syslog(LOG_ERR, "open fail !!! the file no. : %d, errno=%d\n", i, errno);
            break;
        }
        memset(s, '\0', sizeof(s));
        sleep(1);
    }
    for (int i = 0; i < MAX_FD_NUM; i++)
    {
        if(fd[i] > 0)
            close(fd[i]);
    }
    if (num == (MAX_FD_NUM-1))
    {
        test_flag = 0;
        return;
    }
    test_flag = 1;
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
