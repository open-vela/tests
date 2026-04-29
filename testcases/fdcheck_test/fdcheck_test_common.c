#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
    int fd1, fd2;

    fd1 = open("/dev/null", O_RDONLY);
    close(fd1);

    fd2 = open("/dev/null", O_RDONLY);
    close(fd1); // fdcheck will throw Assertion failed panic here
    syslog(0, "fd1 %d fd2 %d\n", fd1, fd2);
    syslog(0, "TEST FAILED\n");

    return 0;
}