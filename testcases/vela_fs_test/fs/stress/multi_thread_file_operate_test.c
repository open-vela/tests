#include <nuttx/config.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "test.h"

/****************************************************************************
 * Name: stress
 * Example description:
 *     1. Create threads as specified by user (up to 10).
 *     2. Threads read and write to the same file in parallel.
 *     3. Check the test results.
 * Test item: pthread_attr_init(), pthread_attr_setstacksize(),
 *            pthread_create(), fopen(), fwrite()
 * Expect results: TEST PASSED
 ****************************************************************************/
#ifdef CONFIG_TESTING_OSTEST_STACKSIZE
#define STACKSIZE CONFIG_TESTING_OSTEST_STACKSIZE
#else
#define STACKSIZE 8192
#endif
#define MAX_THREADS 10
#define DEFAULT_PATH "/data"        /* Default file path */
#define DEFAULT_THREADS 5           /* Default number of threads */

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct threadpara
{
    int thread_id;                 /* Thread ID */
    const char *filename;          /* File name */
    char content[32];              /* Content to be written */
};

static int test_flag = 0;

/* Thread function */
static void *write_thread(void *arg)
{
    FILE *fp;
    struct threadpara *recv_para;
    recv_para = (struct threadpara *)arg;

    /* Mutex lock */
    pthread_mutex_lock(&mutex);
    syslog(LOG_INFO, "Thread ID: %d, writing content: %s\n",
           recv_para->thread_id, recv_para->content);

    /* Open file */
    fp = fopen(recv_para->filename, "a+");
    if (fp == NULL)
    {
        syslog(LOG_ERR, "Fail to open file! %s errno=%d", recv_para->filename, errno);
        test_flag = 1;
        pthread_mutex_unlock(&mutex);
        pthread_exit(NULL);
    }

    /* Write content */
    if (fwrite(recv_para->content, 1, strlen(recv_para->content), fp) == 0)
    {
        syslog(LOG_ERR, "write file fail ! errno=%d\n", errno);
    }

    fclose(fp);
    pthread_mutex_unlock(&mutex);

    /* Exit thread */
    pthread_exit(NULL);
}

/* Core test function */
static void do_test(int thread_count)
{
    pthread_t nthread[MAX_THREADS];
    pthread_attr_t attr;
    struct threadpara thread_params[MAX_THREADS]; /* Thread parameter array */
    int status;

    /* Initialize thread attributes */
    status = pthread_attr_init(&attr);
    if (status != 0)
    {
        syslog(LOG_ERR, "pthread_attr_init failed\n");
        test_flag = 1;
        return;
    }

    status = pthread_attr_setstacksize(&attr, STACKSIZE);
    if (status != 0)
    {
        syslog(LOG_ERR, "pthread_attr_setstacksize failed\n");
        test_flag = 1;
        return;
    }

    /* Create threads */
    for (int i = 0; i < thread_count; i++)
    {
        thread_params[i].thread_id = i + 1;
        thread_params[i].filename = "multi_thread_file_operate"; /* File name */
        snprintf(thread_params[i].content, sizeof(thread_params[i].content),
                 "Thread%02dData", i + 1);

        status = pthread_create(&nthread[i], &attr, write_thread, &thread_params[i]);
        if (status != 0)
        {
            syslog(LOG_ERR, "pthread_create failed for thread %d\n", i + 1);
            test_flag = 1;
            return;
        }
    }

    /* Wait for all threads to complete */
    for (int i = 0; i < thread_count; i++)
    {
        status = pthread_join(nthread[i], NULL);
        if (status != 0)
        {
            syslog(LOG_ERR, "pthread_join failed for thread %d\n", i + 1);
            test_flag = 1;
            return;
        }
    }
}

/* Main function */
int main(int argc, FAR char *argv[])
{
    char *file_path = DEFAULT_PATH; /* Default file path */
    int thread_count = DEFAULT_THREADS; /* Default thread count */
    int opt;

    /* Parse command-line arguments */
    while ((opt = getopt(argc, argv, "d:c:h")) != -1)
    {
        switch (opt)
        {
            case 'd':
                file_path = optarg; /* Set file path */
                break;
            case 'c':
                thread_count = atoi(optarg); /* Set thread count */
                if (thread_count <= 0 || thread_count > MAX_THREADS)
                {
                    syslog(LOG_ERR, "Invalid thread count: %d (must be 1-%d)\n",
                           thread_count, MAX_THREADS);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'h':
                /* Display help */
                syslog(LOG_INFO, "Usage: %s [-d path] [-c thread_count] [-h]\n", argv[0]);
                syslog(LOG_INFO, "  -d [path]           Specify file path (default: %s)\n", DEFAULT_PATH);
                syslog(LOG_INFO, "  -c [thread_count]   Specify thread count (1-%d, default: %d)\n",
                        MAX_THREADS, DEFAULT_THREADS);
                syslog(LOG_INFO, "  -h                  Display this help message\n");
                exit(EXIT_SUCCESS);
            default:
                syslog(LOG_ERR, "Invalid arguments\n");
                exit(EXIT_FAILURE);
        }
    }

    /* Entry process and setup */
    entry_process(2, file_path);
    setup();

    /* Execute the test */
    do_test(thread_count);

    /* Check results and clean up */
    result_check(test_flag);
    cleanup();

    /* Exit with test flag */
    exit(test_flag);
}