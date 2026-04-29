#include <nuttx/config.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/statfs.h>
#include "test.h"

/****************************************************************************
 * Name: stress
 * Example description:
 *     1. Open a file with a different length of file name.
 *     2. For example: 64, 255, 1000 file name lengths.
 *     3. Check that the test returns results.
 * Test item: creat(), write()
 * Expect results: TEST PASSED
 ****************************************************************************/

/* Generate a random file name of specified length */
void rand_file_name(char file_name[], int num)
{
    const char *str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int i, lstr = strlen(str);

    for (i = 0; i < num; i++)
    {
        file_name[i] = str[rand() % lstr];
    }

    file_name[num] = '\0'; // Ensure null termination
}

/* Perform the test */
static int do_test(void)
{
    int fd = -1;
    int ret = -1;
    int new_length;
    char *backup_dir;
    char file[CONFIG_PATH_MAX] = {0};
    char filedir[CONFIG_PATH_MAX] = {0};

    /* Get the current working directory */
    if (getcwd(filedir, sizeof(filedir)) == NULL)
    {
        syslog(LOG_ERR, "Failed to get current working directory\n");
        return 1;
    }

    /* Allocate memory for the path */
    backup_dir = malloc(strlen(filedir) + 1);
    if (backup_dir == NULL)
    {
        syslog(LOG_ERR, "Memory allocation failed\n");
        return 1;
    }

    backup_dir[0] = '\0';
    strcpy(backup_dir, filedir);

    /* Initialize random seed */
    srand((unsigned int)time(NULL));

    /* Calculate the remaining length for directories */
    int boundaryLen = CONFIG_PATH_MAX - strlen(filedir) - 2 - CONFIG_NAME_MAX;
    if (boundaryLen <= 0)
    {
        syslog(LOG_ERR, "The path length is too long\n");
        ret = 1;
        goto end;
    }

    /* Create nested directories */
    while (boundaryLen > 0)
    {
        if (boundaryLen > CONFIG_NAME_MAX)
        {
            new_length = CONFIG_NAME_MAX;
        }
        else
        {
            new_length = boundaryLen -1;
        }

        syslog(LOG_ERR, "new file length %d\n", new_length);
        rand_file_name(file, new_length);

        if (mkdir(file, 0700) != 0)
        {
            getcwd(filedir, sizeof(filedir));
            syslog(LOG_ERR, "Failed to create directory: %s in path %s\n", file, filedir);
            ret = 1;
            goto end;
        }

        if (chdir(file) != 0)
        {
            getcwd(filedir, sizeof(filedir));
            syslog(LOG_ERR, "Failed to change directory: %s/%s\n", filedir, file);
            ret = 1;
            goto end;
        }

        boundaryLen -= (new_length + 1);
        memset(file, '\0', sizeof(file));
    }

    /* Generate a random file name and create the file */
    rand_file_name(file, CONFIG_NAME_MAX);

    if (getcwd(filedir, sizeof(filedir)) == NULL)
    {
        syslog(LOG_ERR, "Failed to get current working directory\n");
        ret = 1;
        goto end;
    }

    syslog(LOG_INFO, "The max name length: %d\nFile path: %s/%s\n",
           CONFIG_NAME_MAX, filedir, file);

    fd = open(file, O_WRONLY | O_CREAT, 0700);
    if (fd >= 0)
    {
        size_t len = strlen(filedir) + strlen(file) + 2;
        if ((len != CONFIG_PATH_MAX))
        {
            syslog(LOG_ERR, "File created, but File path length error: %zu\n",
                   len);
            syslog(LOG_ERR, "expected File path length: %u\n", CONFIG_PATH_MAX);
            ret = 1;
            goto end;
        }
    }
    else
    {
        syslog(LOG_ERR, "create fail , the file path len : %zu, errno=%d\n", strlen(filedir), errno);
        ret = 1;
        goto end;
    }

    ret = 0;

end:
    if(fd >= 0)
    {
        close(fd);
    }

    if(backup_dir)
    {
        /* Return to the original directory */
        if (chdir(backup_dir) != 0)
        {
            syslog(LOG_ERR, "Failed to change back to original directory: %s\n", backup_dir);
            ret = 1;
        }

        free(backup_dir);
        backup_dir = NULL;
    }

    return ret;
}

int main(int argc, FAR char *argv[])
{
    int test_flag;
    if(argc < 2)
    {
        argc = 2;
        argv[1] = "/data";
    }
    entry_process(argc, argv[1]);
    setup();
    test_flag = do_test();
    result_check(test_flag);
    cleanup();
    exit(test_flag);
}