#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <dirent.h>
#include <syslog.h>
#include <sys/stat.h>

#define TESTDIR "testdir"
#define TESTFILE "test.txt"
#define MAX_SIZE 1024
#define TEST_OPEN_MAX 100

static void rm_test_dir(char *path)
{

    DIR *dir = opendir(path);
    struct dirent *dp = NULL;
    struct stat st;
    while ((dp = readdir(dir)) != NULL)
    {

        char fullpath[PATH_MAX];

        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
        {
            continue;
        }
        sprintf(fullpath, "%s/%s", path, dp->d_name);
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

int main(int argc, FAR char *argv[])
{
    char *path = "/data";
    if (argc >= 2)
        path = argv[1];
    char testDir[30] = {0};
    char testFile[40] = {0};
    snprintf(testDir, 30, "%s/%s", path, TESTDIR);
    snprintf(testFile, 40, "%s/%s", testDir, TESTFILE);
    int n = 20;

    n = TEST_OPEN_MAX - 20 < 0 ? 10 : TEST_OPEN_MAX - 20;
    char fileName[100] = {0};
    char buf[MAX_SIZE] = {0};
    int fd = 0;
    int fileSystemFlag = 0;
    syslog(LOG_INFO, "Open  n %d\n", n);
    if (NULL == opendir(testDir))
    {
        syslog(LOG_INFO, "Open  test  start ...\n");
        /*Table of Contents does not exist, CreateTable of Contents*/
        int ret = mkdir(testDir, S_IRWXU);
        if (ret != 0)
        {
            syslog(LOG_ERR, "mkdir fail !\n");
            return -1;
        }
        for (int i = 0; i < n; i++)
        {
            sprintf(fileName, "%s/%d.txt", testDir, i);
            fd = open(fileName, O_RDWR | O_CREAT, 0777);
            if (fd < 0)
            {
                syslog(LOG_ERR, "Open file for write fail !\n ");
                exit(EXIT_FAILURE);
            }
        }
        syslog(LOG_INFO, "Open test end ...\n");
    }
    else
    {
        syslog(LOG_INFO, "Verify the file system  ...\n");

        fd = open(testFile, O_RDWR | O_CREAT, 0777);
        if (fd < 0)
        {
            syslog(LOG_ERR, "Open file for write ail !\n ");
            exit(EXIT_FAILURE);
        }
        else
        {

            memset(buf, 0x65, sizeof(buf));
            int wSize = write(fd, buf, sizeof(buf));
            if (wSize < 0)
            {
                syslog(LOG_ERR, "Write file  fail !\n ");
                exit(EXIT_FAILURE);
            }
            else
            {
                close(fd);
            }
        }

        fd = open(testFile, O_RDONLY, 0777);
        if (fd < 0)
        {
            syslog(LOG_ERR, "Open file for write fail !\n ");
            exit(EXIT_FAILURE);
        }
        else
        {

            memset(buf, 0, sizeof(buf));
            int rSize = read(fd, buf, sizeof(buf));
            if (rSize < 0)
            {
                syslog(LOG_ERR, "Read file  fail !\n");
                exit(EXIT_FAILURE);
            }
            else
            {

                for (int i = 0; i < sizeof(buf); i++)
                {
                    if (buf[i] != 0x65)
                    {
                        fileSystemFlag = 1;
                        break;
                    }
                }
            }
        }
        close(fd);
        if (fileSystemFlag)
        {
            syslog(LOG_ERR, "The file system is not functioning properly ...\n");
        }
        else
        {
            syslog(LOG_INFO, "The file system is functioning normally ...\n");
        }
        rm_test_dir(testDir);
        syslog(LOG_INFO, "Poweroff test open api passed ...\n");
    }
    return 0;
}
