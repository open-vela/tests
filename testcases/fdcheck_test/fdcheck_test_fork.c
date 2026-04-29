#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    int fd1, fd2 = 0;
    fd1 = open("/dev/null", O_RDONLY);
    close(fd1);
    pid_t pid = fork();
    if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        syslog(0, "pid:%d, status:%d\n", pid, status);
    } else if (pid == 0) {
        syslog(0, "here is father process\n");
        fd2 = open("/dev/null", O_RDONLY);
        close(fd1);
    } else {
        syslog(0, "fork failed\n");
        return 0;
    }
    syslog(0, "fd1 %d fd2 %d\n", fd1, fd2);
    return 0;
}