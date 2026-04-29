#include <nuttx/config.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <syslog.h>

#define NAMELEN 50
#define DEFAULT_STACKSIZE CONFIG_DEFAULT_TASK_STACKSIZE
#define DEFAULT_PATH "/data" // Default test path
static int len = 8192; // Default data length
static int test_flag = 1; // Flag to track test status
static int total_count = 1000; // Default test count

/* Initialize payload data */
static void init_payload(int *data, int seed)
{
    int c = seed + 20000;
    for (int i = 0; i < len; i++)
    {
        data[i] = (c++);
    }
}

/* Check payload data for consistency */
static int check_payload(const int *data, int seed)
{
    int c = seed + 20000;
    for (int i = 0; i < len; i++)
    {
        if (data[i] != (c++))
        {
            syslog(LOG_ERR, "Data mismatch: actual %d, expected %d\n", data[i], c - 1);
            return -1;
        }
    }
    return 0;
}

/* Thread function: performs write-read consistency test */
static void *write_pthread(void *data)
{
    char *path = (char *)data;
    int testcount = total_count;
    int *payload = malloc(len * sizeof(int));
    if (payload == NULL)
    {
        test_flag = 0;
        syslog(LOG_ERR, "Memory allocation failed\n");
        return NULL;
    }

    while (testcount-- > 0 && test_flag != 0)
    {
        if (testcount % 20 == 0)
        {
            syslog(LOG_INFO, "%s: Remaining count %d\n", path, testcount);
        }

        /* Write data to file */
        FILE *file = fopen(path, "w+");
        if (file == NULL)
        {
            syslog(LOG_ERR, "Failed to open %s for writing, errno=%d\n", path, errno);
            test_flag = 0;
            break;
        }

        init_payload(payload, testcount);
        size_t written = fwrite(payload, sizeof(int), len, file);
        fclose(file);

        if (written != len)
        {
            syslog(LOG_ERR, "Write error in %s, errno=%d\n", path, errno);
            test_flag = 0;
            break;
        }

        /* Clear payload */
        memset(payload, 0, len * sizeof(int));

        /* Read data from file */
        file = fopen(path, "r");
        if (file == NULL)
        {
            syslog(LOG_ERR, "Failed to open %s for reading, errno=%d\n", path, errno);
            test_flag = 0;
            break;
        }

        size_t read = fread(payload, sizeof(int), len, file);
        fclose(file);

        if (read != len)
        {
            syslog(LOG_ERR, "Read error in %s, errno=%d\n", path, errno);
            test_flag = 0;
            break;
        }

        /* Verify payload consistency */
        if (check_payload(payload, testcount) != 0)
        {
            test_flag = 0;
            break;
        }
    }

    unlink(path); // Remove the test file
    free(payload); // Free allocated memory
    return NULL;
}

/* Display program usage instructions */
static void show_usage(void)
{
    syslog(LOG_WARNING, "Usage: CMD [-n <nPthread>] [-l <data len>] [-d <test dir>] [-c <test_count>]\n"
                        "\t-n: Number of threads (default: 5)\n"
                        "\t-l: File data length (default: 8192)\n"
                        "\t-d: Test directory (default: /data)\n"
                        "\t-c: Test count (default: 10000)\n");
    exit(1);
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        show_usage();
    }

    int nPthread = 5; // Default number of threads
    len = 8192;       // Default data length
    test_flag = 1;    // Initialize test flag
    int opt;
    char *path = DEFAULT_PATH; // Default path is /data
    int status = 0;
    pthread_attr_t attr;

    /* Parse command-line arguments */
    while ((opt = getopt(argc, argv, "n:l:d:c:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            nPthread = atoi(optarg);
            break;
        case 'l':
            len = atoi(optarg);
            break;
        case 'd':
            path = optarg;
            break;
        case 'c':
            total_count = atoi(optarg);
            break;
        default:
            show_usage();
        }
    }

    if (path == NULL || strlen(path) == 0)
    {
        syslog(LOG_ERR, "Invalid test directory\n");
        return -1;
    }

    char filename[nPthread][NAMELEN];
    memset(filename, 0, sizeof(filename));
    pthread_t tid[nPthread];

    /* Create threads */
    for (int i = 0; i < nPthread; i++)
    {
        if (snprintf(filename[i], NAMELEN, "%s/test%d", path, i) < 0)
        {
            syslog(LOG_ERR, "Failed to generate filename\n");
            continue;
        }

        syslog(LOG_INFO, "Test filename: %s\n", filename[i]);

        status = pthread_attr_init(&attr);
        if (status != 0)
        {
            syslog(LOG_ERR, "Failed to initialize thread attributes, status=%d\n", status);
            test_flag = 0;
            break;
        }

        status = pthread_attr_setstacksize(&attr, DEFAULT_STACKSIZE);
        if (status != 0)
        {
            syslog(LOG_ERR, "Failed to set thread stack size, status=%d\n", status);
            test_flag = 0;
            break;
        }

        if (pthread_create(&tid[i], &attr, write_pthread, &filename[i]) != 0)
        {
            syslog(LOG_ERR, "Failed to create thread\n");
            test_flag = 0;
            break;
        }
    }

    /* Wait for threads to complete */
    for (int i = 0; i < nPthread; i++)
    {
        pthread_join(tid[i], NULL);
    }

    /* Print test result */
    if (test_flag == 1)
    {
        syslog(LOG_INFO, "TEST PASS\n");
    }
    else
    {
        syslog(LOG_INFO, "TEST FAIL\n");
    }

    return 0;
}
