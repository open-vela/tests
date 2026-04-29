#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <dirent.h>
#include <syslog.h>
#include <sys/stat.h>
#define OK 0
#define ERROR -1
#define _1k 1024

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

static char *HomeDir;
static char TestHomeDir[20];
static char Test1HomeDir[35];
static char Test2HomeDir[35];
static char TestFile01[60];
static char TestFile02[60];
static char TestFile03[60];
static char TestFile04[60];
static char EmmcFile[30];

static void setDir(char *path)
{
    HomeDir = path;
    snprintf(TestHomeDir, 20, "%s%s", HomeDir, "/testDir");
    snprintf(Test1HomeDir, 35, "%s%s", TestHomeDir, "/testdir01");
    snprintf(Test2HomeDir, 35, "%s%s", TestHomeDir, "/testdir02");
    snprintf(TestFile01, 60, "%s%s", Test1HomeDir, "/powerOffTestFile01");
    snprintf(TestFile02, 60, "%s%s", Test1HomeDir, "/powerOffTestFile02");
    snprintf(TestFile03, 60, "%s%s", Test2HomeDir, "/powerOffTestFile03");
    snprintf(TestFile04, 60, "%s%s", Test2HomeDir, "/powerOffTestFile04");
    snprintf(EmmcFile, 30, "%s%s", HomeDir, "/emmcfile.txt");
}

/*DS*/
struct arg
{

    char *m_filename; //FilePath

    int m_size; // size Parameter input Size

    int m_isfsync; // whether fsync
};
struct test_case_t
{
    pthread_t m_tid; //Threadtid

    struct arg m_config; //Configure via Parameter input

} tdat[] = {
    {.m_config = {TestFile01, _1k / 4, true}},   //TestLength 256, use fsync
    {.m_config = {TestFile02, _1k * 3, false}},  //TestLength 3k, do not use fsync
    {.m_config = {TestFile03, _1k * 5, true}},   //TestLength 5k, use fsync
    {.m_config = {TestFile04, _1k * 10, false}}, //TestLength 10k, do not use fsyc
};

/**/

static int checkBuffer(char *buf, char c, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (*(buf + i) != c)
        {
            syslog(LOG_ERR, "Check error at position %d\n", i);
            return ERROR;
        }
    }
    return OK;
}

static void rm_test_dir(char *path)
{

    DIR *dir = opendir(path);
    struct dirent *dp = NULL;
    struct stat st;
    printf("%s\n", path);
    while ((dp = readdir(dir)) != NULL)
    {

        char fullpath[300];

        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
        {
            continue;
        }
        sprintf(fullpath, "%s/%s", path, dp->d_name);
        printf("%s\n", fullpath);
        stat(fullpath, &st);
        if (!S_ISDIR(st.st_mode))
        {
            remove(fullpath);
        }
        else
        {
            rm_test_dir(fullpath);
        }
    }

    closedir(dir);
    rmdir(path);
}

/*
 Currently power-off test cannot fully simulate user scenarios, need to enhance test case:
 1. Support read and write under multiple files and multiple Table of Contents, length needs to be less than 512 and greater than 512 (3k, 5k, 10k, etc. not aligned with 512 size) test respectively
 2. Not all users call fsync after calling write, so need to test non-sync case
 3. After each breakpoint restart, need to check the remaining space, total space, and used space of the partition where fs is located.
 4. Currently case does not cover breakpoint during write, need to cover, similarly, need to perform breakpoint test during read, open, fsync and other key operations
4. Currently case does not cover breakpoint during write, need to cover, similarly, need to perform breakpoint test during read, open, fsync and other key operations
*/

static void *Thread2TestPowerOff(void *parameter)
{

    int fd;
    ssize_t w_size = 0;
    ssize_t r_size = 0;
    struct arg *config = ((struct arg *)parameter);
    if (config->m_isfsync)
        syslog(LOG_INFO, "test file: %s test size:%d isfsync: true\n", config->m_filename, config->m_size);
    else
        syslog(LOG_INFO, "test file: %s test size:%d isfsync: false\n", config->m_filename, config->m_size);

    char *buf = (char *)malloc(config->m_size);
    if (NULL == buf)
    {
        syslog(LOG_ERR, "malloc fail\n");
        goto _ERROR;
    }
    /*Determine if File exists*/
    fd = open(config->m_filename, O_RDONLY, 0777);
    if (fd < 0)
    {
        /*File does not exist*/
        fd = open(config->m_filename, O_RDWR | O_CREAT, 0777);
        if (fd < 0)
        {
            syslog(LOG_ERR, "open file Fail !\n ");
            goto _OPENERROR;
        }

        while (1)
        {
            memset(buf, 'A', config->m_size);
            w_size = write(fd, buf, config->m_size);
            if (config->m_isfsync)
                fsync(fd);
            if (w_size > 0)
            {
                syslog(LOG_INFO, "Successfully write %zd bytes to the file !\n", w_size);
                syslog(LOG_INFO, "You can try to power off ... ... \n");
                sleep(1);
            }
            else
            {
                syslog(LOG_ERR, "write error\n");
                goto _WRITEERROR;
            }
        }
    }
    else
    {

        while (1)
        {
            memset(buf, '\0', config->m_size);
            r_size = read(fd, buf, config->m_size);
            if (r_size == 0)
                break;
            else if (r_size < 0)
            {
                syslog(LOG_ERR, "read error\n");
                goto _READERROR;
            }
            else
            {
                if (checkBuffer(buf, 'A', r_size) == ERROR)
                {
                    syslog(LOG_ERR, " Data consistency check failed ... \n");
                    goto _TESTFAIL;
                }
            }
        }
    }

    return (void *)"OK";

_WRITEERROR:
_TESTFAIL:
_READERROR:
    close(fd);
_OPENERROR:
    free(buf);
_ERROR:
    return (void *)"ERROR";
}

static void do_test(void)
{
    /*CreateThread*/
    void *retval;
    for (int i = 0; i < sizeof(tdat) / sizeof(struct test_case_t); i++)
    {

        int ret = pthread_create(&(tdat[i].m_tid), NULL, Thread2TestPowerOff, &(tdat[i].m_config));
        if (ret == 0)
        {
            syslog(LOG_INFO, "SUCCESS:Thread %d pthread_create success.\n", i);
        }
        else
        {
            syslog(LOG_ERR, "Thread %d pthread_create error. \n", i);
            return;
        }
    }
    for (int i = 0; i < sizeof(tdat) / sizeof(struct test_case_t); i++)
    {
        int ret = pthread_join(tdat[i].m_tid, &retval);
        if (ret == 0)
        {
            syslog(LOG_INFO, "SUCCESS:Thread %d pthread_join success.\n", i);
        }
        else
        {
            syslog(LOG_ERR, "Thread %d pthread_join error.\n", i);
            return;
        }
        if (strcmp((char *)retval, "OK") != 0)
        {
            syslog(LOG_ERR, "Thread %d TEST FAILED.\n", i);
        }
    }

    return;
}

static int GetEmmcSize2File(void)
{

    struct statfs diskInfo;
    statfs(HomeDir, &diskInfo);
    long block_size = diskInfo.f_bsize;
    unsigned long disk2total = block_size * diskInfo.f_blocks / _1k;
    unsigned long disk2free = block_size * diskInfo.f_bfree / _1k;

    FILE *fp = fopen(EmmcFile, "r");
    if (fp == NULL)
    {
        fp = fopen(EmmcFile, "w+");
        if (fp == NULL)
        {
        }
        else
        {
            /*After creating File, calculate Size*/

            char buf[1024] = {0};
            sprintf(buf, " Totalsize: %10ldk         Freesize: %10ldk    \n", disk2total, disk2free);
            int size = fwrite(buf, strlen(buf), 1, fp);
            if (size < 0)
            {
                syslog(LOG_ERR, "Fwrite Fail\n");
                fclose(fp);
                return -1;
            }
        }
    }
    else
    {
        /*After power-off restart, after all Thread tests PASS, after deleting testdir, calculate Partition Size again, compare Data in original File, if same, then PASS, Delete the File, otherwise throw Exception*/
        /*After creating File, calculate Size*/

        char buf1[32] = {'\0'};
        char Filedisk2total[32] = {'\0'};
        char buf2[32] = {'\0'};
        char Filedisk2free[32] = {'\0'};

        fscanf(fp, "%s %10s %s %10s", buf1, Filedisk2total, buf2, Filedisk2free);
        Filedisk2total[strlen(Filedisk2total) - 1] = '\0';
        Filedisk2free[strlen(Filedisk2free) - 1] = '\0';
        unsigned int predisk2total = atoi(Filedisk2total);
        unsigned int predisk2free = atoi(Filedisk2free);
        syslog(LOG_INFO, "Before process start emmc size Total: %uk  Free: %uk\n", predisk2total, predisk2free);
        syslog(LOG_INFO, "After process end emmc size Total: %luk  Free: %luk\n", disk2total, disk2free);
        if (disk2total != predisk2total)
        {
            syslog(LOG_ERR, "Emmc size TEST FAIL\n");
            fclose(fp);
            remove(EmmcFile);
            return -1;
        }
        else
        {
            syslog(LOG_INFO, "Emmc size TEST PASS\n");
            fclose(fp);
            remove(EmmcFile);
            return 0;
        }
    }

    fclose(fp);
    return 0;
}

int CreateAllDir(void)
{

    if (NULL == opendir(HomeDir))
    {
        /*Table of Contents does not exist, CreateTable of Contents*/
        int ret = mkdir(HomeDir, S_IRWXU);
        if (ret != 0)
        {
            syslog(LOG_ERR, " mkdir %s fail !\n", HomeDir);
            return -1;
        }
    }
    /*Check if TestRootTable of Contents exists*/
    if (NULL == opendir(TestHomeDir))
    {
        /*Table of Contents does not exist, CreateTable of Contents*/
        int ret = mkdir(TestHomeDir, S_IRWXU);
        if (ret != 0)
        {
            syslog(LOG_ERR, " mkdir %s fail !\n", TestHomeDir);
            return -1;
        }
    }
    /*Check if TestTable of Contents1 exists*/
    if (NULL == opendir(Test1HomeDir))
    {
        /*Table of Contents does not exist, CreateTable of Contents*/
        int ret = mkdir(Test1HomeDir, S_IRWXU);
        if (ret != 0)
        {
            syslog(LOG_ERR, " mkdir %s fail !\n", Test1HomeDir);
            return -1;
        }
    }
    /*Check if TestTable of Contents2 exists*/
    if (NULL == opendir(Test2HomeDir))
    {
        /*Table of Contents does not exist, CreateTable of Contents*/
        int ret = mkdir(Test2HomeDir, S_IRWXU);
        if (ret != 0)
        {
            syslog(LOG_ERR, " mkdir %s fail !\n", Test2HomeDir);
            return -1;
        }
    }
    return 0;
}

int main(int argc, FAR char *argv[])
{
    char *path = "/data";
    if (argc >= 2)
        path = argv[1];
    setDir(path);
    int ret = 0;
    if (opendir(TestHomeDir) == NULL)
    {
        /*First run create File that saves emmcSize under ExecuteTable of Contents*/
        ret = GetEmmcSize2File();
        if (ret < 0)
        {
            return -1;
        }
    }
    /*Create all Table of Contents*/
    ret = CreateAllDir();
    if (ret < 0)
    {
        return ret;
    }

    do_test();

    rm_test_dir(TestHomeDir);

    if (opendir(TestHomeDir) == NULL)
    {
        /*Delete emmcFile after power-off restart test*/
        ret = GetEmmcSize2File();
        if (ret < 0)
        {
            syslog(LOG_ERR, "TestPowerOffCase03 TEST  FAIL\n");
        }
        else
        {
            syslog(LOG_INFO, "TestPowerOffCase03 TEST  PASS\n");
        }
    }
    return 0;
}
