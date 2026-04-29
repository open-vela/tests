#include <nuttx/config.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <dirent.h>
#include <errno.h>

#define WRITE_BUF_SIZE 1024
#define TEST_FILE_NAME_PREFIX "fullSpaceTest"
#define TEST_TEMP_FILENAME_LENGTH  (14)
#define TEST_FILENAME_LENGTH       (strlen(TEST_FILE_NAME_PREFIX) + 1 + 14 + 1 + 2)
#define TEST_FILENAME_FORMAT       (%s_%s_%d)
#define MAXIMUM_SIZE_OF_RANDOM_FILE 256
#define NUMBER_OF_RANDOM_FILE 100
#define OK 0
#define ERROR -1

static char w_buffer[WRITE_BUF_SIZE] = {0};

static int get_random_name(char *random_name, int len)
{
    const char seed_str[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int seed_str_len = strlen(seed_str);
    struct timeval tv;
    unsigned int seed_num;

    if (!random_name || len <= 0)
    {
        syslog(LOG_ERR, "[%s] Invalid parameters!\n", __func__);
        return ERROR;
    }

    gettimeofday(&tv, NULL);
    seed_num = (unsigned int)(tv.tv_sec + tv.tv_usec);
    srand(seed_num);

    for (int i = 0; i < len; i++)
    {
        random_name[i] = seed_str[rand() % seed_str_len];
    }
    random_name[len] = '\0'; // Ensure null termination
    return OK;
}

static int creatFile(const char *filename, size_t write_size)
{
    if (!filename)
    {
        syslog(LOG_ERR, "[%s] Invalid filename!\n", __func__);
        return ERROR;
    }

    int fd = open(filename, O_CREAT | O_RDWR, 0777);
    if (fd < 0)
    {
        syslog(LOG_ERR, "[%s] Failed to create file: %s, error: %s\n", __func__, filename, strerror(errno));
        return ERROR;
    }

    memset(w_buffer, 0x65, WRITE_BUF_SIZE); // Fill buffer with 'e'
    while (write_size > 0)
    {
        size_t chunk_size = (write_size < WRITE_BUF_SIZE) ? write_size : WRITE_BUF_SIZE;
        ssize_t written = write(fd, w_buffer, chunk_size);
        if (written < 0)
        {
            syslog(LOG_ERR, "[%s] Write failed for file: %s, error: %s\n", __func__, filename, strerror(errno));
            close(fd);
            return ERROR;
        }
        write_size -= written;
    }

    syslog(LOG_INFO, "[Create] File created: %s\n", filename);
    close(fd);
    return OK;
}

static int delete_file_matching_prefix(const char *dir_path, const char *prefix_name)
{
    int number_of_deleted = 0;
    struct dirent *entry;
    char tmp_name[CONFIG_PATH_MAX];

    if (!dir_path || !prefix_name)
    {
        syslog(LOG_ERR, "[%s] Invalid parameters!\n", __func__);
        return ERROR;
    }

    DIR *dir = opendir(dir_path);
    if (!dir)
    {
        syslog(LOG_ERR, "[%s] Failed to open directory: %s, error: %s\n", __func__, dir_path, strerror(errno));
        return ERROR;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strncmp(entry->d_name, prefix_name, strlen(prefix_name)) == 0)
        {
            snprintf(tmp_name, sizeof(tmp_name), "%s/%s", dir_path, entry->d_name);
            if (unlink(tmp_name) == 0)
            {
                number_of_deleted++;
                syslog(LOG_INFO, "[Delete] File deleted: %s\n", tmp_name);
            }
            else
            {
                syslog(LOG_ERR, "[Delete FAIL] Failed to delete file: %s, error: %s\n", tmp_name, strerror(errno));
            }
        }
    }

    closedir(dir);
    return number_of_deleted;
}

int main(int argc, char *argv[])
{
    char *test_dir = "/data/"; // Default directory
    int test_count = 100;      // Default number of test iterations
    int creat_total = 0;
    int delete_total = 0;
    char file_name[CONFIG_PATH_MAX];
    char tmp_str[CONFIG_NAME_MAX];
    int file_size;
    int opt;
    int pass_count = 0;

    while ((opt = getopt(argc, argv, "d:c:h")) != -1)
    {
        switch (opt)
        {
        case 'd':
            test_dir = optarg;
            break;
        case 'c':
            test_count = atoi(optarg);
            if (test_count <= 0)
            {
                syslog(LOG_ERR, "Invalid test count: %d\n", test_count);
                return EXIT_FAILURE;
            }
            break;
        case 'h':
        default:
            printf("Usage: %s [-d <directory>] [-c <counts>]\n"
                   "  -d: Set test directory (default: /data)\n"
                   "  -c: Set number of program executions (default: 100)\n",
                   argv[0]);
            return EXIT_FAILURE;
        }
    }

    /* test path is "/data" + "/" + "fullSpaceTest" + "_" + "[tmp_str]" + "_" + "xx" */
    if(strlen(test_dir) == 0 || (strlen(test_dir) + 1 + TEST_FILENAME_LENGTH >= CONFIG_PATH_MAX))
    {
        syslog(LOG_ERR, "Invalid test directory: %s\n", test_dir);
        return EXIT_FAILURE;
    }

    if(NUMBER_OF_RANDOM_FILE > 100)
    {
        /* make sure that the TEST_FILENAME_LENGTH is right */
        syslog(LOG_ERR, "Invalid number of each test round of random files: %d\n", NUMBER_OF_RANDOM_FILE);
        return EXIT_FAILURE;
    }

    for (int j = 1; j <= test_count; j++)
    {
        syslog(LOG_INFO, "Test #%d =================================================================\n", j);

        for (int i = 0; i < NUMBER_OF_RANDOM_FILE; i++)
        {
            if(OK != get_random_name(tmp_str, rand() % TEST_TEMP_FILENAME_LENGTH + 1))
            {
                syslog(LOG_ERR, "Test #%d get random file name error\n", j);
                continue;
            }

            snprintf(file_name, sizeof(file_name), "%s/%s_%s_%d", test_dir, TEST_FILE_NAME_PREFIX, tmp_str, i);

            file_size = rand() % MAXIMUM_SIZE_OF_RANDOM_FILE + 1;
            if (creatFile(file_name, file_size) == OK)
            {
                creat_total++;
            }
        }

        sleep(1);

        delete_total = delete_file_matching_prefix(test_dir, TEST_FILE_NAME_PREFIX);

        syslog(LOG_INFO, "Total files created: %d\nTotal files deleted: %d\n", creat_total, delete_total);
        if (creat_total == delete_total && creat_total == NUMBER_OF_RANDOM_FILE)
        {
            syslog(LOG_INFO, "Test #%d PASSED\n", j);
            pass_count++;
        }
        else
        {
            syslog(LOG_ERR, "Test #%d FAILED\n", j);
            return EXIT_FAILURE;
        }

        creat_total = 0;
        delete_total = 0;
        sleep(1);
    }
    if (pass_count == test_count)
    {
        syslog(LOG_INFO, "TEST PASSED !\n");
    }

    return EXIT_SUCCESS;
}