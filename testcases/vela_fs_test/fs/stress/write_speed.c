#include <nuttx/config.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include "test.h"

/* Define constants using macros */
#define FILE_NAME "fs_write_speed"  /* File to write to */
#define BUF_SIZE 1024            /* Size of the write buffer */
#define WRITE_TIMES 1000         /* Number of times to write */
#define DEFAULT_PATH "/data"     /* Default value for argv[1] */
#define SUCCESS 0                /* Indicate test success */
#define FAILURE 1                /* Indicate test failure */

/****************************************************************************
 * Name: stress
 * Example description:
 *     1. Open a file.
 *     2. Write to the file 1000 times in a loop.
 *     3. Calculate the speed of writing to the file.
 * Test item: fopen(), fwrite(), fsync()
 * Expect results: TEST PASSED
 ****************************************************************************/

static int test_flag = SUCCESS;

static int do_test(int write_times)
{
    test_flag = SUCCESS;
    FILE *fp;
    clock_t start, finish;
    char buf[BUF_SIZE];
    float duration;
    int int_part;
    int decimal_part;

    /* Initialize buffer with '%' */
    memset(buf, '%', sizeof(buf));

    /* Start timing */
    start = clock();

    /* Open file */
    if ((fp = fopen(FILE_NAME, "a+")) == NULL)
    {
        syslog(LOG_ERR, "Fail to open file! errno=%d", errno);
        return FAILURE; /* Return FAILURE for error */
    }

    /* Write to file in a loop */
    for (int num = 0; num < write_times; num++)
    {
        /* Attempt to write to file */
        if (fwrite(buf, sizeof(buf), 1, fp) != 1)
        {
            syslog(LOG_ERR, "write FAIL !   no.%d, errno=%d\n", num + 1, errno);
            fclose(fp); /* Close the file before returning */
            return FAILURE; /* Return FAILURE for error */
        }
        syslog(LOG_INFO, "Write success! No.%d\n", num + 1);

        /* Flush the output buffer to the file */
        fsync(fileno(fp));
    }

    /* Close the file */
    fclose(fp);

    /* End timing */
    finish = clock();
    duration = (float)(finish - start) / CLOCKS_PER_SEC;
    int_part = (int)duration;
    decimal_part = (int)((duration - int_part) * 1000);

    /* Log the duration */
    syslog(LOG_INFO, "File size: %dK, takes %d.%03d seconds\n", write_times, int_part, decimal_part);

    return SUCCESS; /* Return SUCCESS for success */
}

int main(int argc, FAR char *argv[])
{
    char *file_path = DEFAULT_PATH;      /* Default file path */
    int write_times = WRITE_TIMES;             /* Default write count */
    int opt;                                   /* Option character */

    /* Parse command-line arguments using getopt */
    while ((opt = getopt(argc, argv, "d:c:h")) != -1)
    {
        switch (opt)
        {
            case 'd':
                file_path = optarg; /* Get file path */
                break;
            case 'c':
                write_times = atoi(optarg); /* Get write count */
                if (write_times <= 0) {
                    syslog(LOG_ERR, "Invalid write count: %d\n", write_times);
                    return FAILURE;
                }
                break;
            case 'h':
                /* Display help message */
                syslog(LOG_INFO, "Usage: %s [-d path] [-c count] [-h]\n", argv[0]);
                syslog(LOG_INFO, "  -d [path]   Specify file path (default: %s)\n", DEFAULT_PATH);
                syslog(LOG_INFO, "  -c [count]  Specify write count (default: %d)\n", WRITE_TIMES);
                syslog(LOG_INFO, "  -h          Display this help message\n");
                return SUCCESS;
            default:
                syslog(LOG_ERR, "Invalid arguments!\n");
                syslog(LOG_INFO, "Usage: %s [-d path] [-c count] [-h]\n", argv[0]);
                return FAILURE;
        }
    }

    /* Perform entry process and setup */
    entry_process(2, file_path);
    setup();

    /* Execute the test */
    test_flag = do_test(write_times); /* Get return value from do_test */

    /* Check results and cleanup */
    result_check(test_flag);
    cleanup();

    /* Exit with test_flag */
    exit(test_flag);
}