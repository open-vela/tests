#ifndef __TEST_H
#define __TEST_H

#include <nuttx/config.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/stat.h>
#include <syslog.h>

#define TEST_PASS 0
#define TEST_FAIL -1

#define TEST_DIR "testDir"

__attribute__((unused)) static void rm_test_dir(char *path)
{
    DIR *dir = opendir(path);
    struct dirent *dp = NULL;
    char *fullpath;
    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
        {
            continue;
        }

        fullpath = (char *)malloc(CONFIG_PATH_MAX);
        if (fullpath == NULL)
        {
            syslog(LOG_ERR, "rmdir malloc fail\n");
            goto close;
        }
        sprintf(fullpath, "%s/%s", path, dp->d_name);
        if (dp->d_type == 4)
        {
            rm_test_dir(fullpath);
        }
        else
        {
            unlink(fullpath);
        }
        free(fullpath);
    }
close:
    closedir(dir);
    rmdir(path);
}

__attribute__((unused)) static void cleanup(void)
{
    char buf[CONFIG_PATH_MAX] = {0};
    getcwd(buf, sizeof(buf));
    rm_test_dir(buf);
    char *str = basename(buf);
    int len = strlen(str);
    int slen = strlen(buf);
    buf[slen - len] = '\0';
    chdir(buf);
}

__attribute__((unused)) static void setup(void)
{
    int ret;
    char buf[20] = {0};
    getcwd(buf, sizeof(buf));
    strcat(buf, "/");
    strcat(buf, TEST_DIR);
    ret = mkdir(buf, S_IRWXU);
    if (ret == 0)
    {
        ret = chdir(buf);
    }
    else
    {
        if (chdir(buf) == -1)
        {
            syslog(LOG_ERR, "[setup]:setup fail !\n");
            exit(1);
        }
    }
    return;
}

__attribute__((unused)) static void entry_process(int argc, char argv[])
{
    if (argc >= 2)
    {
        syslog(LOG_INFO, "test dir :%s\n", argv);
        chdir(argv);
    }
    else
    {
        syslog(LOG_ERR, "The argument is wrong !\n");
        exit(1);
    }
}

__attribute__((unused)) static void result_check(int ret)
{
    syslog(LOG_INFO, "test completed......\n");
    ret > 0 ? syslog(LOG_ERR, "TEST FAILED !\n") : syslog(LOG_INFO, "TEST PASSED !\n");
}

int do_creat_test01(void);
int do_dup_test01(void);
int do_dup2_test01(void);
int do_fcntl_test01(void);
int do_fcntl_test02(void);
int do_fcntl_test03(void);
int do_fstat_test01(void);
int do_fstat_test02(void);
int do_fstatfs_test01(void);
int do_fsync_test01(void);
int do_fsync_test02(void);
int do_getfilep_test01(void);
int do_mkdir_test01(void);
int do_open_test01(void);
int do_oepndir_test01(void);
int do_oepndir_test02(void);
int do_pread_test01(void);
int do_pwrite_test01(void);
int do_read_test01(void);
int do_readdir_test01(void);
int do_readlink_test01(void);
int do_rename_test01(void);
int do_rename_test02(void);
int do_rewinddir_test01(void);
int do_rmdir_test01(void);
int do_rmdir_test02(void);
int do_rmdir_test03(void);
int do_seek_test01(void);
int do_seek_test02(void);
int do_stat_test01(void);
int do_statfs_test01(void);
int do_symlink_test01(void);
int do_truncate_test01(void);
int do_unlink_test01(void);
int do_write_test01(void);
int do_write_test02(void);
int do_write_test03(void);

#ifdef CONFIG_FS_TEST_FUNCTION
int do_function_fs_append_test(void);
int do_sendfile_test01(void);
int do_sendfile_test02(void);
int do_function_stream01_test(void);
int do_function_stream02_test(void);
int do_function_stream03_test(void);
int do_function_stream04_test(void);
#endif

#endif
