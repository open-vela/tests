#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <syslog.h>


#define OK 0
#define ERROR -1
#define CHECK_CHARACTER 0x65
#define WRITE_BUF_SIZE 100
#define READ_BUF_SIZE 100
#define FILE_NAME "powerOffTestFile"

static int checkFileExist(char *fileName)
{
    int fd;
    fd = open(fileName, O_RDONLY, 0777);
    if (fd > 0)
    {
        syslog(LOG_INFO, "[%s] The test file exists and can be opened !\n", __func__);
        close(fd);
        return OK;
    }
    else
    {
        syslog(LOG_ERR, "[%s] Test file does not exist !\n", __func__);
        return ERROR;
    }
}

static int checkBuffer(char *buf, int c)
{
    for (int i = 0; i < strlen(buf); i++)
    {
        if (*(buf++) != (c & 0xff))
        {
            syslog(LOG_ERR, "Check error at position %d, expect %d, actual %d\n", i, c, *(buf - 1));
            return ERROR;
        }
    }
    return OK;
}

int main(int argc, FAR char *argv[])
{
    char *path = "/data";
    if(argc >= 2)
        path = argv[1];
    char filename[40] = {0};
    snprintf(filename, 40, "%s/%s", path, FILE_NAME);
    int is_exist;
    int fd;
    ssize_t w_size = 0;
    ssize_t r_size = 0;
    char buf[WRITE_BUF_SIZE + 1] = {0};
    char r_buf[READ_BUF_SIZE + 1] = {0};

    syslog(LOG_INFO, "START TEST ...\n");
    memset(buf, CHECK_CHARACTER, WRITE_BUF_SIZE);

    is_exist = checkFileExist(filename);
    if (is_exist == OK)
    {
        fd = open(filename, O_RDONLY, 0777);
        if (fd < 0)
        {
            syslog(LOG_ERR, "open file for read Fail !\n ");
            exit(EXIT_FAILURE);
        }
        do
        {
            memset(r_buf, '\0', READ_BUF_SIZE);
            syslog(LOG_INFO, "Read the file that was being written before power Off \n");
            r_size = read(fd, r_buf, READ_BUF_SIZE);
            syslog(LOG_INFO, "Check data consistency \n");
            if (checkBuffer(r_buf, 0x65) == ERROR)
            {
                syslog(LOG_ERR, "Data consistency check failed ... \n");
                syslog(LOG_ERR, "TEST FAILED\n");
                unlink(filename);
                exit(EXIT_FAILURE);
            }
        } while (r_size > 0);
    }
    else
    {
        fd = open(filename, O_RDWR | O_CREAT, 0777);
        if (fd < 0)
        {
            syslog(LOG_ERR, "open file for write Fail !\n ");
            exit(EXIT_FAILURE);
        }
        while (1)
        {
            w_size = write(fd, buf, WRITE_BUF_SIZE);
            fsync(fd);
            if (w_size > 0)
            {
                syslog(LOG_INFO, "Successfully write %zd bytes to the file !\n", w_size);
                syslog(LOG_INFO, "You can try to power off ... ... \n");
                sleep(1);
            }
        }
    }
    syslog(LOG_INFO, "TEST PASSED\n");

    unlink(filename);
    return 0;
}
