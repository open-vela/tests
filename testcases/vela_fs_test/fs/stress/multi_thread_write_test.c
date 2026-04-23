#include <nuttx/config.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>
#include <errno.h>

/* Structure to store thread-specific write operation data */
struct op_data
{
    int fd;             // File descriptor specific to the thread
    const char *data;   // Data to be written by the thread
};

static int data_num = 1000; // Default number of data entries to write
static char filename[64] = {0}; // File name to be created and operated upon

/* Thread function: writes data to the file */
static void *write_pthread(void *data)
{
    struct op_data *d = data;

    for (int i = 0; i < data_num; ++i)
    {
        if (write(d->fd, d->data, strlen(d->data)) == -1)
        {
            syslog(LOG_ERR, "Thread failed to write data: %s\n", strerror(errno));
        }
    }

    return NULL;
}

/* Function to get the file size */
static size_t get_file_size(const char *file)
{
    struct stat st;
    if (stat(file, &st) == -1)
    {
        syslog(LOG_ERR, "Failed to get file size for %s: %s\n", file, strerror(errno));
        return (size_t)-1;
    }
    return st.st_size; // Return file size in bytes
}

int main(int argc, char *argv[])
{
    char *testPath = "/data"; // Default path for the test file
    pthread_t t1, t2, t3;
    struct op_data data1, data2, data3;

    int fd;
    int test_flag = 0; // Error flag to track issues during execution
    int opt;

    /* Parse command-line arguments using getopt */
    while ((opt = getopt(argc, argv, "d:l:")) != -1)
    {
        switch (opt)
        {
        case 'd': // Path to test directory
            testPath = optarg;
            break;
        case 'l': // Length of data to write
            data_num = atoi(optarg);
            break;
        default:
            syslog(LOG_WARNING, "Usage: %s [-d <test directory>] [-l <write length>]\n", argv[0]);
            test_flag = 1;
        }
    }

    /* Construct the filename */
    sprintf(filename, "%s/multi_pthread_testfile", testPath);

    /* Open the file for writing */
    fd = open(filename, O_RDWR | O_APPEND | O_CREAT, 0777);
    if (fd == -1)
    {
        syslog(LOG_ERR, "Failed to open file %s: %s\n", filename, strerror(errno));
        test_flag = 1;
    }

    /* Initialize thread-specific data */
    data1.fd = fd;
    data2.fd = fd;
    data3.fd = fd;

    data1.data = "A";
    data2.data = "B";
    data3.data = "C";

    /* Create threads to write data */
    if (!test_flag)
    {
        syslog(LOG_INFO, "Creating threads...\n");
        if (pthread_create(&t1, NULL, write_pthread, &data1) != 0)
        {
            syslog(LOG_ERR, "Failed to create thread t1\n");
            test_flag = 1;
        }
        if (pthread_create(&t2, NULL, write_pthread, &data2) != 0)
        {
            syslog(LOG_ERR, "Failed to create thread t2\n");
            test_flag = 1;
        }
        if (pthread_create(&t3, NULL, write_pthread, &data3) != 0)
        {
            syslog(LOG_ERR, "Failed to create thread t3\n");
            test_flag = 1;
        }
    }

    /* Wait for threads to complete */
    if (!test_flag)
    {
        syslog(LOG_INFO, "Joining threads...\n");
        if (pthread_join(t1, NULL) != 0)
        {
            syslog(LOG_ERR, "Failed to join thread t1\n");
            test_flag = 1;
        }
        if (pthread_join(t2, NULL) != 0)
        {
            syslog(LOG_ERR, "Failed to join thread t2\n");
            test_flag = 1;
        }
        if (pthread_join(t3, NULL) != 0)
        {
            syslog(LOG_ERR, "Failed to join thread t3\n");
            test_flag = 1;
        }
    }

    if (fd > 0)
        close(fd);

    /* Verify the file size */
    if (!test_flag)
    {
        size_t file_size = get_file_size(filename);
        if (file_size == (size_t)-1 || file_size != data_num * 3)
        {
            syslog(LOG_ERR, "File size verification failed. Expected %d bytes, got %zu bytes\n", data_num * 3, file_size);
            test_flag = 1;
        }
        else
        {
            syslog(LOG_INFO, "TEST PASS!\n");
        }
    }

    /* Cleanup resources */
    unlink(filename);

    /* Unified exit status based on test_flag */
    exit(test_flag);
}