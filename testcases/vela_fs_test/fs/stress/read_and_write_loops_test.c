#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/statfs.h>
#include "test.h"
#define TESTFILENAME "loopTestFile"
#define TESTNUM     10
#define BUFFER_SIZE 40
#define OK          0
#define ERROR       -1


/****************************************************************************
 * Name: stress
 * Example description:
     1. Read and write the same file over and over again.
     2. Check that disk usage is as expected.
 * Test item: fopen() fwrite() fflush()
 * Expect results: TEST PASSED
 ****************************************************************************/

static void get_rand_str(char s[], int num, int seed)
{
    char *str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int i, lstr;
    lstr = strlen(str);
    srand((unsigned int)(time((time_t *)NULL) + seed));
    for (i = 0; i < num; i++)
    {
        s[i] = str[(rand() % lstr)];
    }
}

static int write_file(char *filename, char *content)
{
    int rval;
    FILE *fp;

    if ((fp = fopen(filename, "a+")) == NULL)
    {
        syslog(LOG_ERR, "Fail to open file! errno=%d", errno);
        return ERROR;
    }

    rval = fwrite(content, 1, strlen(content), fp);
    fclose(fp);
    if (rval != strlen(content))
    {
        syslog(LOG_ERR, "write file fail! rval=%d, errno=%d\n", rval, errno);
        return ERROR;
    }

    syslog(LOG_INFO, "write to file! content: %s\n", content);
    return OK;
}

static int read_file(char *filename, int seek, char *data)
{
    FILE *fp;
    char buffer[BUFFER_SIZE] = {0};
    size_t ret;
    if ((fp = fopen(filename, "r")) == NULL)
    {
        syslog(LOG_ERR, "Fail to open file! errno=%d", errno);
        return ERROR;
    }

    if (lseek(fileno(fp), seek, SEEK_SET) < 0)
    {
        fclose(fp);
        return ERROR;
    }
    ret = fread(buffer, sizeof(char), BUFFER_SIZE - 1, fp);
    fclose(fp);
    syslog(LOG_INFO, "seek %d from file head, read length: %d, content:%s\n",
           seek, BUFFER_SIZE - 1, buffer);
    if (ret != (BUFFER_SIZE - 1) || memcmp(buffer, data, sizeof(buffer) - 1) != 0)
    {
        syslog(LOG_ERR, "Fail to read, errno=%d", errno);
        return ERROR;
    }
    return OK;
}

long get_file_size_test(char *filename)
{
    FILE *fp;
    long size = 0;
    if ((fp = fopen(filename, "r")) == NULL)
    {
        syslog(LOG_ERR, "Fail to open file! errno=%d", errno);
        return ERROR;
    }
    if (lseek(fileno(fp), 0, SEEK_END) < 0)
    {
        fclose(fp);
        return ERROR;
    }
    size = ftell(fp);
    fclose(fp);
    return size;
}

static int test_flag = 0;

static void do_test(int testnum)
{
    char str[BUFFER_SIZE] = {0};

    test_flag = 0;
    for (int i = 0; i < testnum; i++)
    {

        get_rand_str(str, sizeof(str) - 1, i);
        syslog(LOG_INFO, "write file no.%d\n", i);
        if (write_file(TESTFILENAME, str) < 0)
        {
            goto testerr;
        }
        syslog(LOG_INFO, "read file no.%d\n", i);
        if (read_file(TESTFILENAME, (sizeof(str) - 1) * i, str) < 0)
        {
            goto testerr;
        }
    }

    syslog(LOG_INFO, "file size: %ld K\n", get_file_size_test(TESTFILENAME) / 1024);

    if (get_file_size_test(TESTFILENAME) == testnum * (sizeof(str)-1))
    {
        return;
    }

testerr:
    test_flag = 1;
}

int main(int argc, FAR char *argv[])
{
    struct statfs diskInfo;
    int max_testnum;
    int testnum;
    int ret;

    testnum = argc >= 3? atoi(argv[2]): TESTNUM;
    max_testnum = testnum;

    ret = statfs(argv[1], &diskInfo);
    if (ret == 0)
      {
        if (diskInfo.f_bfree != 0 && diskInfo.f_bfree < INT_MAX/diskInfo.f_bsize)
          {
            max_testnum = diskInfo.f_bfree * diskInfo.f_bsize / BUFFER_SIZE;
          }
        else if (diskInfo.f_blocks < INT_MAX/diskInfo.f_bsize)
          {
            max_testnum = diskInfo.f_blocks * diskInfo.f_bsize / BUFFER_SIZE;
          }

        testnum = max_testnum < testnum? max_testnum: testnum;
      }

    entry_process(argc, argv[1]);
    setup();
    do_test(testnum);
    result_check(test_flag);
    cleanup();
    exit(test_flag);
}
