#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/param.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/statfs.h>
#include <stdint.h>
#include <errno.h>

#define WRITE_BUF_SIZE 1024
#define ODD_FILE_PREFIX "Fragment_test_0_"
#define EVEN_FILE_PREFIX "Fragment_test_1_"
#define MAXIMUM_SIZE_OF_RANDOM_FILE 128
#define LAGER_FILE_NAME "Fragment_test_largefile"
#define NUMBER_OF_TEST_EXECUTIONS 1000
#define FILE_SYSTEM_MOUNT_POINT "/data"
#define FILE_NAME_LEN 10

#define OK 0
#define ERROR -1

/*---------------------------------------------------------------------------------------------------------------------------
1. Write a 4KB file (or a 1KB small file is fine) until there is no space
2. Delete even-numbered files at intervals
3. Sequentially write xMB files to measure the sequential write performance of the file system until the disk is full
4. Perform sequential read operations on the previously written xMB files to measure the read performance
---------------------------------------------------------------------------------------------------------------------------*/

static char w_buffer[WRITE_BUF_SIZE] = {0};

static int get_random_name(char *random_name, int len)
{
    int i, random_num, seed_str_len;
    struct timeval tv;
    unsigned int seed_num;
    char seed_str[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    seed_str_len = strlen(seed_str);

    gettimeofday(&tv, NULL);
    seed_num = (unsigned int)(tv.tv_sec + tv.tv_usec);
    srand(seed_num);

    for (i = 0; i < len; i++)
    {
        random_num = rand() % seed_str_len;
        random_name[i] = seed_str[random_num];
    }
    return 0;
}

static int creat_file(char *filename, size_t write_size)
{
    int fd;
    ssize_t size = 0;
    fd = open(filename, O_CREAT | O_RDWR, 0777);
    if (fd < 0)
    {
        syslog(LOG_INFO, "[%s] creat file fail ! errno=%d\n", __func__, errno);
        return ERROR;
    }
    memset(w_buffer, 0x65, WRITE_BUF_SIZE);
    while (write_size > 0)
    {
        size = write(fd, w_buffer, MIN(write_size, WRITE_BUF_SIZE));
        if (size < 0)
        {
            if (errno == EFBIG || errno == ENOSPC)
            {
                break;
            }
            close(fd);
            syslog(LOG_INFO, "[%s] write fail, errno %d!\n", __func__, errno);
            return ERROR;
        }
        write_size -= size;
    }

    syslog(LOG_INFO, "[Create] : %s\n", filename);
    close(fd);
    return OK;
}

static int delete_file_matching_prefix(char *dir_path, char *prefix_name)
{
    DIR *dir;
    int number_of_deleted = 0;
    struct dirent *ptr;
    char tmp_name[256] = {0};
    dir = opendir(dir_path);
    while ((ptr = readdir(dir)) != NULL)
    {
        if (strncmp(ptr->d_name, prefix_name, strlen(prefix_name)) == 0)
        {
            memset(tmp_name, '\0', 256);
            int ret = snprintf(tmp_name, 256, "%s/%s", dir_path, ptr->d_name);
            if (ret < 0 || ret >= sizeof(tmp_name))
            {
                syslog(LOG_ERR, "snprintf fail\n");
                exit(EXIT_FAILURE);
            }
            if (unlink(tmp_name) == 0)
            {
                number_of_deleted++;
                syslog(LOG_INFO, "[Delete] : %s\n", tmp_name);
            }
            else
            {
                syslog(LOG_ERR, "[Delete FAIL] : %s, errno=%d\n", tmp_name, errno);
                syslog(LOG_ERR, "exit test !\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    closedir(dir);
    return number_of_deleted;
}

static void show_usages(void)
{
    syslog(LOG_WARNING, "Usage: CMD [-d <test dir path>] [-n <number of test files>] [-s <size of a large file(unit:M)>]"
                        "           [-N <large_file_num>] [-c <test_count>]\n"
                        "\t\t-d: set test dir path.\n"
                        "\t\t-n: The number of small files created in the test.\n"
                        "\t\t-s: Large file size created in test. (unit:M).\n"
                        "\t\t-N: Large file number created in test.\n"
                        "\t\t-c: set number of program executions\n");
    exit(1);
}

int main(int argc, char *argv[])
{

    int random;
    int ret;
    int o;
    char name[256] = {0};
    char tmp_str[FILE_NAME_LEN + 1] = {0};
    char file_test_path[128] = {0};
    int test_count = NUMBER_OF_TEST_EXECUTIONS;
    int creat_total = 0;
    int delete_total = 0;
    int file_no = 0;
    int max_file_number = 10;
    int large_file_size = 1;
    int large_file_num = 100;

    sprintf(file_test_path, "%s", FILE_SYSTEM_MOUNT_POINT);
    while ((o = getopt(argc, argv, "d:n:hs:N:c:")) != EOF)
    {
        switch (o)
        {
        case 'd':
            snprintf(file_test_path, 128, "%s", optarg);
            break;
        case 'n':
            max_file_number = atoi(optarg);
            break;
        case 's':
            large_file_size = atoi(optarg);
            break;
        case 'N':
            large_file_num = atoi(optarg);
            break;
        case 'c':
            test_count = atoi(optarg);
            break;
        default:
            show_usages();
        }
    }

    large_file_size = large_file_size * 1024 * 1024;
    delete_file_matching_prefix(file_test_path, "Fragment_test");
    syslog(LOG_INFO, "cleanning up, test begin in 5s!\n");
    sleep(5);
    for (int j = 0; j < test_count; j++)
    {
        creat_total = 0;
        syslog(LOG_INFO, "[TEST NO.%d] Create some small files !\n", j);
        while (file_no < max_file_number)
        {
            srand(file_no);
            memset(name, '\0', 256);
            memset(tmp_str, '\0', FILE_NAME_LEN);
            get_random_name(tmp_str, rand() % FILE_NAME_LEN + 1);

            if (file_no % 2 == 0)
            {
                snprintf(name, 256, "%s/%s%s_%d", file_test_path, ODD_FILE_PREFIX, tmp_str, file_no++);
            }
            else
            {
                snprintf(name, 256, "%s/%s%s_%d", file_test_path, EVEN_FILE_PREFIX, tmp_str, file_no++);
            }

            random = rand() % MAXIMUM_SIZE_OF_RANDOM_FILE + 1;
            if (creat_file(name, random) == ERROR)
            {
                syslog(LOG_ERR, "Failed to create, may not have enough space !\n");
                break;
            }

            creat_total++;
        }

        syslog(LOG_INFO, "Create a total of %d files\n", creat_total);
        sleep(1);
        syslog(LOG_INFO, "Delete odd prefixed files !\n");
        delete_total = delete_file_matching_prefix(file_test_path, EVEN_FILE_PREFIX);
        syslog(LOG_INFO, "Delete total %d\n", delete_total);
        for (int i = 0; i < large_file_num; i++)
        {
            memset(name, '\0', 256);
            snprintf(name, 256, "%s/%s", file_test_path, LAGER_FILE_NAME);
            if (creat_file(name, large_file_size) == ERROR)
            {
                syslog(LOG_INFO, "Failed to create a large file, size=%d\n", large_file_size);
                return ERROR;
            }
            sleep(1);
            ret = unlink(name);
            if (ret == 0)
            {
                syslog(LOG_INFO, "TEST NO.%d  The test large file was deleted successfully !\n", i);
            }
            else
            {
                syslog(LOG_ERR, "TEST NO.%d  The test large file was deleted FAIL !\n", i);
                return ERROR;
            }
        }

        /* clean */
        delete_file_matching_prefix(file_test_path, "Fragment_test");
        syslog(LOG_INFO, "[TEST NO.%d] TEST ... OK !\n", j);
    }

    return 0;
}