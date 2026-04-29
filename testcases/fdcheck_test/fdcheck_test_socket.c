#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>

/* define path name */
#define SOCKET_PATH_NAME "UNIX_Socket"
#define BUF_SIZE 1024

int main(int argc, char **argv)
{
    int sock_fd1, sock_fd2, ret;
    struct sockaddr_un client_addr;
    
    // Create fd1
    sock_fd1 = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd1 < 0)
    {
        printf("opening stream socket fail !\n");
        exit(EXIT_FAILURE);
    }

    // First connect
    client_addr.sun_family = AF_UNIX;
    strcpy(client_addr.sun_path, SOCKET_PATH_NAME);    
    ret = connect(sock_fd1, (struct sockaddr *)&client_addr, sizeof(struct sockaddr_un));
    if (ret < 0)
    {
        syslog(0, "connect error at first connect!\n");
        exit(EXIT_FAILURE);
    }

    syslog(0, "sock_fd1: %d\n", sock_fd1);
    close(sock_fd1);

    // Create fd2
    sock_fd2 = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd2 < 0)
    {
        printf("opening stream socket fail !\n");
        exit(EXIT_FAILURE);
    }
    syslog(0, "sock_fd2: %d\n", sock_fd2);

    // Second connect
    // fdcheck will throw Assertion failed panic here
    ret = connect(sock_fd1, (struct sockaddr *)&client_addr, sizeof(struct sockaddr_un));
    if (ret < 0)
    {
        syslog(0, "connect error at second connect!\n");
        exit(EXIT_FAILURE);
    }

    syslog(0, "TEST FAILED\n");
    return 0;
}