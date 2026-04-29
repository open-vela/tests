#include <nuttx/config.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>
#include <sys/boardctl.h>

#define OK 0
#define ERROR -1
#define DATA_BUFF_LEN 1024
#define TEST_FILE_NAME_1 "stability_test04_file_1"
#define TEST_FILE_NAME_2 "stability_test04_file_2"
#define BUFFLEN 4096
static char *PARTION_PATH;

/* Check if the filesystem partition is healthy by creating a file */
static int is_partition_available(const char *partitionPath)
{
    int fd;
    int ret;

    ret = chdir(partitionPath);
    if (ret != 0)
    {
        syslog(LOG_ERR, "Failed to switch to partition directory(%s)\n", partitionPath);
        return ERROR;
    }
    ret = mkdir("testAvailableDir", 0700);
    if (ret != 0)
    {
        syslog(LOG_ERR, "Failed to create directory under file system partition !\n");
        return ERROR;
    }
    else
    {
        rmdir("testAvailableDir");
    }

    fd = open("testAvailableFile", O_CREAT | O_RDWR, 0700);
    if (fd < 0)
    {
        syslog(LOG_ERR, "Failed to create file under file system partition !\n");
        return ERROR;
    }
    else
    {
        close(fd);
        unlink("testAvailableFile");
    }
    return OK;
}

__attribute__((unused)) static void do_reboot(void)
{
    syslog(LOG_INFO, "Simulate device reboot ...\n");
    /* Simulate a system restart to test whether the file system is still normal */
#if defined(CONFIG_BOARDCTL_RESET)
    boardctl(BOARDIOC_RESET, 0);
#else
    syslog(LOG_WARNING, "System is not configured with BOARDIOC_RESET !\n");
    syslog(LOG_INFO, "Try to manually power off and restart !\n");
#endif
}

static void do_crash(void)
{
    char *buf = NULL;
    syslog(LOG_INFO, "Simulate system crash ...\n");
    /* Operate on null Pointer, Constructor System crash Scenario. Used to Test File System stability */
    *buf = 'a';
}

static int is_file_exist(const char *filename)
{
    struct stat buffer;
    int exist = stat(filename, &buffer);
    if (exist == 0)
        return OK;
    else
        return ERROR;
}

static int get_file_size_test(const char *filename)
{
    struct stat buffer;
    int exist = stat(filename, &buffer);
    if (exist == 0)
        return buffer.st_size;
    else
        return -1;
}

static int checkBuffer(char *buf, int c)
{
    for (int i = 0; i < strlen(buf); i++)
    {
        if (*(buf++) != (c & 0xff))
        {
            syslog(LOG_ERR, "Check error at position %d\n", i);
            return ERROR;
        }
    }
    return OK;
}

static void *threadroutine_1(void *arg)
{
    /* Wait 10 seconds and then simulate a System Exception Restart */
    sleep(30);
    do_crash();
    return NULL;
}

static void *threadroutine_2(void *arg)
{
    int fd, write_count = 0;
    ssize_t size = 0;
    ssize_t total_size = 0;
    char fileFullPath[64] = {0};
    char *buf = NULL;
    sprintf(fileFullPath, "%s/%s", PARTION_PATH, TEST_FILE_NAME_1);
    fd = open(fileFullPath, O_CREAT | O_RDWR, 0700);
    if (fd < 0)
    {
        syslog(LOG_ERR, "[%s] open file fail ! errno %d , %s\n", __func__, errno, strerror(errno));
        pthread_exit(0);
    }
    buf = (char *)malloc(BUFFLEN * sizeof(char));
    if (buf == NULL)
    {
        syslog(LOG_ERR, "[%s] thread malloc fail ! errno %d , %s\n", __func__, errno, strerror(errno));
        pthread_exit(0);
    }

    memset(buf, 0x65, BUFFLEN);

    do
    {
        size = write(fd, buf, BUFFLEN);
        if (size > 0)
        {
            total_size = total_size + size;
            syslog(LOG_INFO, "[%s] %dth write test file, total write %zd bytes\n", __func__, ++write_count, total_size);
        }
        /* do fsync() after write everytime */
        fsync(fd);
        usleep(200000);
    } while (1);
}

static void *threadroutine_3(void *arg)
{
    int fd, write_count = 0;
    ssize_t size = 0;
    ssize_t total_size = 0;
    char fileFullPath[64] = {0};
    char *buf = NULL;
    sprintf(fileFullPath, "%s/%s", PARTION_PATH, TEST_FILE_NAME_2);
    fd = open(fileFullPath, O_CREAT | O_RDWR, 0700);
    if (fd < 0)
    {
        syslog(LOG_ERR, "[%s] open file fail ! errno %d , %s\n", __func__, errno, strerror(errno));
        pthread_exit(0);
    }
    buf = (char *)malloc(BUFFLEN * sizeof(char));
    if (buf == NULL)
    {
        syslog(LOG_ERR, "[%s] thread malloc fail ! errno %d , %s\n", __func__, errno, strerror(errno));
        pthread_exit(0);
    }

    memset(buf, 0x65, BUFFLEN);

    do
    {
        size = write(fd, buf, BUFFLEN);
        if (size > 0)
        {
            total_size = total_size + size;
            syslog(LOG_INFO, "[%s] %dth write test file, total write %zd bytes\n", __func__, ++write_count, total_size);
        }
        usleep(200000);
    } while (1);
}

static int file_check(char *filename)
{
    int fd;
    char *check_buf = NULL;
    ssize_t r_size = 0;
    fd = open(TEST_FILE_NAME_1, O_RDONLY, 0777);
    if (fd < 0)
    {
        syslog(LOG_ERR, "open file for read Fail !\n ");
        exit(1);
    }

    check_buf = (char *)malloc(sizeof(char) * 512);

    do
    {
        memset(check_buf, '\0', 512);
        r_size = read(fd, check_buf, 512);
        if (r_size <= 0)
        {
            syslog(LOG_INFO, "Failed to read file or file is empty ! filename:%s\n", filename);
            break;
        }
        check_buf[r_size] = 0;
        if (checkBuffer(check_buf, 0x65) == ERROR)
        {
            syslog(LOG_ERR, "Data consistency check failed ... \n");
            close(fd);
            free(check_buf);
            return ERROR;
        }
    } while (r_size > 0);

    close(fd);
    free(check_buf);
    return OK;
}

int main(int argc, FAR char *argv[])
{
    PARTION_PATH = "/data";
    if (argc >= 2)
        PARTION_PATH = argv[1];
    char filename1[64] = {0};
    char filename2[64] = {0};
    pthread_t pt_1, pt_2, pt_3;

    if (is_partition_available(PARTION_PATH) == OK)
    {
        syslog(LOG_INFO, "fs system partion is OK !\n");
    }
    else
    {
        syslog(LOG_INFO, "fs system partition not available !\n");
        return ERROR;
    }
    sprintf(filename1, "%s/%s", PARTION_PATH, TEST_FILE_NAME_1);
    sprintf(filename2, "%s/%s", PARTION_PATH, TEST_FILE_NAME_2);

    if ((is_file_exist(filename1) == OK) && (is_file_exist(filename2) == OK))
    {
        syslog(LOG_INFO, "file: %s   total size:%d\n", filename1, get_file_size_test(filename1));
        syslog(LOG_INFO, "file: %s   total size:%d\n", filename2, get_file_size_test(filename2));

        if (file_check(TEST_FILE_NAME_1) == ERROR)
        {
            syslog(LOG_ERR, "File content check failed ! file:%s\n", TEST_FILE_NAME_1);
            return ERROR;
        }
        if (file_check(TEST_FILE_NAME_2) == ERROR)
        {
            syslog(LOG_ERR, "File content check failed ! file:%s\n", TEST_FILE_NAME_2);
            return ERROR;
        }

        syslog(LOG_INFO, "TEST PASSED !\n");
        unlink(TEST_FILE_NAME_1);
        unlink(TEST_FILE_NAME_2);
        return 0;
    }
    else
    {
        pthread_create(&pt_1, NULL, (void *)threadroutine_1, NULL);
        pthread_create(&pt_2, NULL, (void *)threadroutine_2, NULL);
        pthread_create(&pt_3, NULL, (void *)threadroutine_3, NULL);

        pthread_join(pt_1, NULL);
        pthread_join(pt_2, NULL);
        pthread_join(pt_3, NULL);

        // sleep(20);
        while (1)
        {
            /* code */
        }
    }

    return 0;
}