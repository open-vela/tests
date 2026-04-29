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

static char test_path[64] = {0};
#define THREAD_1 "thread_1"
#define THREAD_2 "thread_2"

#define TEST_DIR_STAB "StabilityTestDir"

#define TEST_FILE_NAME_1 "dlog.fixedip"
#define TEST_FILE_NAME_2 "network.password"
#define TEST_FILE_NAME_3 "network.ssid"
#define TEST_FILE_NAME_4 "network.wifi_country"
#define TEST_FILE_NAME_5 "network.proversion_stat"
#define TEST_FILE_NAME_6 "ot_config.psm_token"
#define TEST_FILE_NAME_7 "ot_config.ota_dura_time"
#define TEST_FILE_NAME_8 "ot_config.ota_start_time"
#define TEST_FILE_NAME_9 "ot_offline_info"
#define TEST_FILE_NAME_10 "ot_config.uid"
#define TEST_FILE_NAME_11 "oper_monitor.flash_index"

#define TEST_CREAT_DEL_FILE_1 "create_and_delete_file_1"
#define TEST_CREAT_DEL_FILE_2 "create_and_delete_file_2"
#define TEST_CREAT_DEL_FILE_3 "create_and_delete_file_3"
#define TEST_CREAT_DEL_FILE_4 "create_and_delete_file_4"

#define REGULAR_UPDAT_EFILE_PREFIX "regularly_update_files_name_prefix"
#define REGULAR_UPDAT_EFILE_SIZE 100

static int thread_1_status = 0;
static int thread_2_status = 0;

#define OK 0
#define ERROR -1
#define FILL_CHARACTER 0x61
#define WRITE_BUF_SIZE 1024
#define MAX_PATH 100

static char w_buffer[WRITE_BUF_SIZE] = {0};

static void rm_test_dir(char *path)
{
    FAR DIR *dir = opendir(path);
    while (1)
    {
        char fullpath[MAX_PATH];
        struct dirent *ent = readdir(dir);
        if (ent == NULL)
        {
            closedir(dir);
            rmdir(path);
            break;
        }
        int ret = snprintf(fullpath, MAX_PATH, "%s/%s", path, ent->d_name);
        if (ret < 0)
        {
            syslog(LOG_ERR, "snprintf fail\n");
            exit(EXIT_FAILURE);
        }
        if (ent->d_type == 4)
        {
            rm_test_dir(fullpath);
        }
        else
        {
            unlink(fullpath);
        }
    }
}

static void cleanup(void)
{
    char buf[MAX_PATH] = {0};
    getcwd(buf, sizeof(buf));
    rm_test_dir(buf);
}

static int creatFile(char *filename, size_t write_size)
{
    int fd;
    ssize_t size = 0;
    ssize_t total_size = 0;

    fd = open(filename, O_CREAT | O_RDWR, 0777);
    if (fd < 0)
    {
        syslog(LOG_ERR, "[%s] creat file fail !  errno=%d\n", __func__, errno);
        return ERROR;
    }
    else
    {
        syslog(LOG_INFO, "[%s] creat test file, name : %s\n", __func__, filename);
    }

    memset(w_buffer, FILL_CHARACTER, WRITE_BUF_SIZE);

    do
    {
        if (write_size <= WRITE_BUF_SIZE)
        {
            size = write(fd, w_buffer, write_size);
            if (size < 0)
            {
                syslog(LOG_ERR, "[%s] write fail ! errno=%d\n", __func__, errno);
                return ERROR;
            }
        }
        else
        {
            size = write(fd, w_buffer, WRITE_BUF_SIZE);
            if (size < 0)
            {
                syslog(LOG_ERR, "[%s] write fail ! errno=%d\n", __func__, errno);
                return ERROR;
            }
            fsync(fd);
            syslog(LOG_INFO, "do fsync , fd = %d\n", fd);
        }
        total_size = total_size + size;
        write_size = write_size - size;
    } while (write_size > (size_t)0);
    syslog(LOG_INFO, "[%s] the test file, size : %zd\n", __func__, total_size);
    close(fd);
    return OK;
}

static int deleteFile(char *filename)
{
    int val;
    val = unlink(filename);
    if (val == -1)
    {
        syslog(LOG_ERR, "[%s] unlink fail !\n", __func__);
        return ERROR;
    }
    else
    {
        syslog(LOG_INFO, "[%s] delete file : %s\n", __func__, filename);
    }

    return OK;
}

static int readAndCheck(char *filename)
{
    int fd;
    int cout = 0;
    char buf[513] = {0};
    ssize_t r_size = 0;

    fd = open(filename, O_RDONLY, 0777);
    if (fd < 0)
    {
        syslog(LOG_ERR, "[%s] read fail ! errno=%d\n", __func__, errno);
        return ERROR;
    }

    do
    {
        memset(buf, '\0', 513);
        r_size = read(fd, buf, 512);
        sleep(1);
        for (int i = 0; i < r_size; i++)
        {
            if (buf[i] != (FILL_CHARACTER & 0xff))
            {
                syslog(LOG_INFO, "the Content read this time, buf=%s\n", buf);
                syslog(LOG_INFO, "Check error at file position %d\n", i + cout * 512);
                close(fd);
                return ERROR;
            }
        }
        cout++;
    } while (r_size > 0);
    close(fd);
    return OK;
}

static int fileIsExist(char *filename)
{

    int fd;
    fd = open(filename, O_RDONLY, 0777);
    if (fd >= 0)
    {
        syslog(LOG_INFO, "[%s] File (%s) exists !\n", __func__, filename);
        close(fd);
        return OK;
    }
    else
    {
        syslog(LOG_ERR, "[%s] file (%s) does not exist ! errno=%d\n", __func__, filename, errno);
        return ERROR;
    }
}

static int updateFile(char *filename, size_t write_size)
{
    int fd;

    fd = open(filename, O_WRONLY, 0777);
    if (fd < 0)
    {
        syslog(LOG_ERR, "[%s] update file fail ! errno=%d\n", __func__, errno);
        return ERROR;
    }
    else
    {
        syslog(LOG_INFO, "[%s] Update file size size=%zd\n", __func__, write_size);
    }

    memset(w_buffer, FILL_CHARACTER, WRITE_BUF_SIZE);

    write(fd, w_buffer, write_size);

    close(fd);
    return OK;
}

/****************************************************************************
 * The worker thread_1
 ****************************************************************************/
static void *thread_funtion_1(void *arg)
{
    int ret;
    int num = 0;

    while (1)
    {
        ret = fileIsExist(TEST_FILE_NAME_1);
        if (ret == OK)
        {
            readAndCheck(TEST_FILE_NAME_1);
        }
        fileIsExist(TEST_FILE_NAME_2);
        if (ret == OK)
        {
            readAndCheck(TEST_FILE_NAME_2);
        }
        fileIsExist(TEST_FILE_NAME_3);
        if (ret == OK)
        {
            readAndCheck(TEST_FILE_NAME_3);
        }
        fileIsExist(TEST_FILE_NAME_4);
        if (ret == OK)
        {
            readAndCheck(TEST_FILE_NAME_4);
        }
        fileIsExist(TEST_FILE_NAME_5);
        if (ret == OK)
        {
            readAndCheck(TEST_FILE_NAME_5);
        }
        fileIsExist(TEST_FILE_NAME_6);
        if (ret == OK)
        {
            readAndCheck(TEST_FILE_NAME_6);
        }
        fileIsExist(TEST_FILE_NAME_7);
        if (ret == OK)
        {
            readAndCheck(TEST_FILE_NAME_7);
        }
        fileIsExist(TEST_FILE_NAME_8);
        if (ret == OK)
        {
            readAndCheck(TEST_FILE_NAME_8);
        }
        fileIsExist(TEST_FILE_NAME_9);
        if (ret == OK)
        {
            readAndCheck(TEST_FILE_NAME_9);
        }
        fileIsExist(TEST_FILE_NAME_10);
        if (ret == OK)
        {
            readAndCheck(TEST_FILE_NAME_10);
        }
        fileIsExist(TEST_FILE_NAME_11);
        if (ret == OK)
        {
            readAndCheck(TEST_FILE_NAME_11);
        }
        sleep(2);

        if ((num % 2) == 0)
        {
            creatFile(TEST_CREAT_DEL_FILE_1, 104);
            creatFile(TEST_CREAT_DEL_FILE_2, 2670);
            creatFile(TEST_CREAT_DEL_FILE_3, 4096);
            creatFile(TEST_CREAT_DEL_FILE_4, 5678);
        }
        else
        {
            deleteFile(TEST_CREAT_DEL_FILE_1);
            deleteFile(TEST_CREAT_DEL_FILE_2);
            deleteFile(TEST_CREAT_DEL_FILE_3);
            deleteFile(TEST_CREAT_DEL_FILE_4);
        }
        sleep(1);
        num++;
        if (thread_1_status == -1)
            break;
    }
    return 0;
}

/****************************************************************************
 * The worker thread_2
 ****************************************************************************/
static void *thread_funtion_2(void *arg)
{
    char name[64] = {0};
    int ret;
    int a;
    syslog(LOG_INFO, "Create a test file for the first time !\n");
    for (int i = 0; i < 25; i++)
    {
        memset(name, '\0', 64);
        sprintf(name, "%s-%d", REGULAR_UPDAT_EFILE_PREFIX, i);
        creatFile(name, REGULAR_UPDAT_EFILE_SIZE);
    }
    while (1)
    {

        for (int i = 0; i < 25; i++)
        {
            memset(name, '\0', 64);
            sprintf(name, "%s-%d", REGULAR_UPDAT_EFILE_PREFIX, i);
            ret = fileIsExist(name);
            if (ret == OK)
            {
                srand((unsigned int)(time(NULL) + i));
                a = rand() % 128;
                updateFile(name, REGULAR_UPDAT_EFILE_SIZE + a);
            }
        }

        sleep(5);
        if (thread_2_status == -1)
            break;
    }

    return 0;
}

/****************************************************************************
 * main
 ****************************************************************************/
int static do_test(void)
{
    int status;
    pthread_t thread1, thread2;
    pthread_attr_t attr1, attr2;
    pthread_attr_init(&attr1);
    pthread_attr_init(&attr2);
    pthread_attr_setstacksize(&attr1, 4096);
    pthread_attr_setstacksize(&attr2, 4096);

    creatFile(TEST_FILE_NAME_1, 10);
    creatFile(TEST_FILE_NAME_2, 10);
    creatFile(TEST_FILE_NAME_3, 12);
    creatFile(TEST_FILE_NAME_4, 4);
    creatFile(TEST_FILE_NAME_5, 4);
    creatFile(TEST_FILE_NAME_6, 17);
    creatFile(TEST_FILE_NAME_7, 9);
    creatFile(TEST_FILE_NAME_8, 9);
    creatFile(TEST_FILE_NAME_9, 74);
    creatFile(TEST_FILE_NAME_10, 9);
    creatFile(TEST_FILE_NAME_11, 5);

    status = pthread_create(&thread1, &attr1, thread_funtion_1, NULL);
    if (status != 0)
    {
        syslog(LOG_ERR, "[main_process] pthread_test  pthread_create failed, status=%d\n", status);
        return -1;
    }
    else
    {
        syslog(LOG_INFO, "[main_process] creat thread success !  name=%s\n", THREAD_1);
    }
    status = pthread_create(&thread2, &attr2, thread_funtion_2, NULL);
    if (status != 0)
    {
        syslog(LOG_ERR, "[main_process] pthread_test, pthread_create failed, status=%d\n", status);
        return -1;
    }
    else
    {
        syslog(LOG_INFO, "[main_process] creat thread success !  name=%s\n", THREAD_2);
    }

    while (1)
    {
        if (thread_2_status == -1 && thread_1_status == -1)
            break;
        sleep(1);
    }

    return 0;
}

static void set(int argc, char *argv[])
{
    int status;
    if (argc == 3)
    {
        if (strcmp("-d", argv[1]) == 0)
        {
            if (strncpy(test_path, argv[2], sizeof(test_path) - 1) == NULL)
            {
                syslog(LOG_ERR, "[main_process] ERROR : strncpy failed !\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    else
    {
        syslog(LOG_WARNING, "The argument is wrong !\n");
        syslog(LOG_WARNING, "usage   CMD [-d  path] !\n");
        exit(1);
    }
    strcat(test_path, "/");
    strcat(test_path, TEST_DIR_STAB);

    status = mkdir(test_path, 0700);
    if (status != 0)
    {
        syslog(LOG_ERR, "[main_process] ERROR : mkdir failed !\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        syslog(LOG_INFO, "[main_process] creat a dir, name=%s\n", test_path);
        chdir(test_path);
    }
    return;
}

int main(int argc, FAR char *argv[])
{
    set(argc, argv);
    do_test();
    cleanup();
    return 0;
}
