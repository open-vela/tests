#include <nuttx/config.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <malloc.h>
#include <sys/statfs.h>
#include "test.h"
#define TESTNUMBER 100

/****************************************************************************
 * Name: stress
 * Example description:
     1. Create a file and write it.
     2. repeat step 1 for 100 times.
     3. Check disk memory usage information.
     4. Check that the test returns results.
 * Test item: open() write() read() statfs() etc
 * Expect results: TEST PASSED
 ****************************************************************************/

static void get_rand_str(char s[], int num)
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

static long get_disk_info(char s[])
{
    struct statfs diskInfo;
    statfs(s, &diskInfo);
    syslog(LOG_INFO, "\n--------disk info (%s)--------------------\n", s);
    syslog(LOG_INFO, "                         total : %lld K\n", (long long)diskInfo.f_blocks * (diskInfo.f_bsize / 1024));
    syslog(LOG_INFO, "          The number of blocks : %lld\n", (long long)diskInfo.f_blocks);
    syslog(LOG_INFO, "The number of blocks available : %lld\n", (long long)diskInfo.f_bfree);
    syslog(LOG_INFO, "                    Block size : %u\n", (unsigned int)diskInfo.f_bsize);
    syslog(LOG_INFO, "                     available : %lld K\n", (long long)diskInfo.f_bfree * (diskInfo.f_bsize / 1024));
    syslog(LOG_INFO, "                          used : %lld K\n", (long long)(diskInfo.f_blocks - diskInfo.f_bfree) * (diskInfo.f_bsize / 1024));
    syslog(LOG_INFO, "-------------------------------------------\n");
    return diskInfo.f_bfree * (diskInfo.f_bsize / 1024);
}

static int test_flag = 0;

static void do_test(void)
{
    test_flag = 0;
    int fd = 0, ret, rval, file_len, test_number;
    struct mallinfo mallocinfo;
    char *buffer = NULL;
    char s[CONFIG_NAME_MAX+1]={0};
    char buf[20] = {0};
    test_number = TESTNUMBER;
    getcwd(buf, sizeof(buf));
    get_disk_info(buf);
    while (test_number != 0)
    {
        syslog(LOG_INFO, "\nTest %d\n", TESTNUMBER - test_number + 1);

        mallocinfo = mallinfo();
        ret = mallocinfo.fordblks;
        if (ret == 0)
          {
            /* On some virtual devices, a return of 0 indicates infinity */

            ret = 128;
          }
        else
          {
            ret = ret / 10;
          }

        srand((unsigned)(time(NULL) + test_number));
        file_len = rand() % CONFIG_NAME_MAX + 1;

        syslog(LOG_INFO, "ret = %d file_len = %d\n", ret, file_len);
        buffer = (char *)malloc(ret);
        if (buffer == NULL)
        {
            test_flag = 1;
            syslog(LOG_ERR, "malloc fail !\n");
            break;
        }
        memset(buffer, '#', ret);
        get_rand_str(s, file_len);
        syslog(LOG_INFO, "The file name is randomly generated ! fileName:%s\n", s);
        syslog(LOG_INFO, "creating no.%d file\n", TESTNUMBER - test_number + 1);
        fd = open(s, O_WRONLY | O_CREAT, 0700);
        if (fd < 0)
        {
            syslog(LOG_ERR, "create file:%s  fail ! errno=%d\n", s, errno);
            goto err;
        }
        syslog(LOG_INFO, "create file success !\n");
        rval = write(fd, buffer, ret);
        if (rval == -1)
        {
            syslog(LOG_ERR, "write file fail ! errno=%d\n", errno);
            goto out;
        }

        syslog(LOG_INFO, "write file success !\n");
        close(fd);
        free(buffer);
        ret = remove(s);
        if(ret < 0) {
            syslog(LOG_INFO, "remove file failed! errno=%d\n\n", errno);
            goto out;
        }
        syslog(LOG_INFO, "remove file success !\n\n");
        memset(s, '\0', sizeof(s));
        test_number--;
    }

    return;

out:
    syslog(LOG_ERR, "TEST FAILED !\n");
    close(fd);
err:
    free(buffer);
    test_flag = 1;
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
