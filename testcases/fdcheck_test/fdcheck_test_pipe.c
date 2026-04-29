#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/stat.h>

#define MAX_MSGSIZE 1000

int main(int argc, char **argv)
{
    int pipefd1[2], pipefd2[2];
    char msg[MAX_MSGSIZE];

    if (pipe(pipefd1) != 0) {
        syslog(LOG_ERR, "pipe failed\n");
        return -1;
    }

    write(pipefd1[1], "vela", 5);
    read(pipefd1[0], msg, MAX_MSGSIZE);
    syslog(0, "%s", msg);
    
    close(pipefd1[1]);
    close(pipefd1[0]);

    if (pipe(pipefd2) != 0) {
        syslog(LOG_ERR, "pipe failed\n");
        return -1;
    }

    read(pipefd1[0], msg, MAX_MSGSIZE);

    syslog(0, "TEST FAILED\n");
    return 0;
}