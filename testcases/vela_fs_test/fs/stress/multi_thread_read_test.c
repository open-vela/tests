#include <nuttx/config.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>

static int data_num = 1000;           // Default number of data entries to write
static int thread_count = 3;          // Default number of threads
static char filename[64] = {0};       // File name to be created and operated upon

/* Structure to store thread-specific file access data */
struct file_data
{
    int fd;          // File descriptor specific to the thread
    int A_count;     // Count of 'A' characters read
    int B_count;     // Count of 'B' characters read
    int C_count;     // Count of 'C' characters read
};

/* Setup function: creates a test file and writes data to it */
static void setup(int *test_flag)
{
    int rval;
    int fd = open(filename, O_RDWR | O_APPEND | O_CREAT, 0777);
    if (fd == -1)
    {
        syslog(LOG_ERR, "Failed to open file %s: %s\n", filename, strerror(errno));
        *test_flag = 1; // Set error flag
        return;
    }

    /* Write "ABC" repeatedly to the file for data_num entries */
    for (int i = 0; i < data_num; i++)
    {
        rval = write(fd, "ABC", 3);
        if (rval == -1)
        {
            syslog(LOG_ERR, "Failed to write to file %s: %s\n", filename, strerror(errno));
            *test_flag = 1;
            close(fd);
            return;
        }
    }
    close(fd);
}

/* Thread function: reads the file data and counts occurrences of 'A', 'B', and 'C' */
static void *read_pthread(void *data)
{
    struct file_data *d = data;
    int rval;
    char buf[4] = {0}; // Buffer to read multiple bytes from the file
    d->A_count = 0;
    d->B_count = 0;
    d->C_count = 0;

    for (int i = 0; i < data_num; ++i)
    {
        /* Read 3 bytes (representing "ABC") per iteration */
        rval = read(d->fd, buf, 3);
        if (rval == 3)
        {
            /* Count occurrences of 'A', 'B', and 'C' */
            for (int j = 0; j < 3; j++)
            {
                if (buf[j] == 'A') d->A_count++;
                if (buf[j] == 'B') d->B_count++;
                if (buf[j] == 'C') d->C_count++;
            }
        }
    }
    return NULL;
}

/* Cleanup function: closes file descriptors and removes the test file */
static void cleanup(struct file_data *data, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (data[i].fd > 0)
            close(data[i].fd);
    }
    unlink(filename); // Remove the test file
}

/* Main function */
int main(int argc, char *argv[])
{
    char *testPath = "/data"; // Default path to store the test file
    int opt;
    int test_flag = 0; // Error flag to track any issues during execution

    /* Parse command-line arguments using getopt */
    while ((opt = getopt(argc, argv, "p:l:t:")) != -1)
    {
        switch (opt)
        {
        case 'p': // Path to test directory
            testPath = optarg;
            break;
        case 'l': // Length of data to write
            data_num = atoi(optarg);
            break;
        case 't': // Number of threads to create
            thread_count = atoi(optarg);
            break;
        default:
            syslog(LOG_WARNING, "Usage: %s [-p <test directory>] [-l <data length>] [-t <thread count>]\n", argv[0]);
            test_flag = 1;
            break;
        }
    }

    /* Set the filename to be used */
    sprintf(filename, "%s/multi_pthread_testfile", testPath);

    /* Create test file and write data */
    setup(&test_flag);
    if (test_flag) // If an error occurred during setup, exit early
    {
        syslog(LOG_ERR, "Setup failed. Exiting...\n");
        exit(test_flag);
    }

    struct file_data data[thread_count];
    pthread_t threads[thread_count];

    /* Open the file for each thread */
    for (int i = 0; i < thread_count; i++)
    {
        data[i].fd = open(filename, O_RDONLY);
        if (data[i].fd == -1)
        {
            syslog(LOG_ERR, "Failed to open file %s for thread %d: %s\n", filename, i, strerror(errno));
            test_flag = 1; // Set error flag
        }
    }

    syslog(LOG_INFO, "Creating threads...\n");

    if (!test_flag) // Only proceed if no errors occurred during file opening
    {
        /* Create threads for reading */
        for (int i = 0; i < thread_count; i++)
        {
            if (pthread_create(&threads[i], NULL, read_pthread, &data[i]) != 0)
            {
                syslog(LOG_ERR, "Failed to create thread %d\n", i);
                test_flag = 1;
                break;
            }
        }

        syslog(LOG_INFO, "Joining threads...\n");

        /* Wait for threads to complete */
        for (int i = 0; i < thread_count; i++)
        {
            if (pthread_join(threads[i], NULL) != 0)
            {
                syslog(LOG_ERR, "Failed to join thread %d\n", i);
                test_flag = 1;
            }
        }

        /* Calculate and verify multi-threaded reading results */
        int total_A = 0, total_B = 0, total_C = 0;
        for (int i = 0; i < thread_count; i++)
        {
            total_A += data[i].A_count;
            total_B += data[i].B_count;
            total_C += data[i].C_count;
        }

        int expected_count = data_num * thread_count;
        if (total_A == expected_count && total_B == expected_count
            && total_C == expected_count)
        {
            syslog(LOG_INFO, "TEST PASS!\n");
        }
        else
        {
            syslog(LOG_ERR, "TEST FAILED! Counts: A=%d, B=%d, C=%d, expect=%d\n",
                   total_A, total_B, total_C, expected_count);
            test_flag = 1;
        }
    }
    else
    {
        syslog(LOG_ERR, "Error occurred during file opening. Threads not created.\n");
    }

    /* Cleanup resources */
    cleanup(data, thread_count);

    /* Exit with the final flag status */
    exit(test_flag);
}