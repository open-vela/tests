#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>

/* define path name */
#define SOCKET_PATH_NAME "UNIX_Socket"
#define BUF_SIZE 1024

/****************************************************************************
 * Name: server
 * Example description: UNIX socket server
 ****************************************************************************/

int main(int argc, char *argv[])
{

    int socket_fd, ret;
    int conn_fd;
    int n, tmp_n;
    struct sockaddr_un server_addr, cli_addr;
    socklen_t c_len;
    char *buf = NULL;
    int total = 0;

    unlink(SOCKET_PATH_NAME);

    /* create unix socket, STREAM */
    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        printf("open socket fail !\n");
        exit(EXIT_FAILURE);
    }

    /* set sun_family */
    server_addr.sun_family = AF_UNIX;

    /* set sun_path */
    strcpy(server_addr.sun_path, SOCKET_PATH_NAME);

    /* bind */
    ret = bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un));
    if (ret < 0)
    {
        printf("bind fail !\n");
        exit(EXIT_FAILURE);
    }

    /* listen */
    if (listen(socket_fd, 5) < 0)
    {
        printf("listen error !\n");
        exit(EXIT_FAILURE);
    }

    buf = (char *)malloc(BUF_SIZE * sizeof(char));
    if (buf == NULL)
    {
        printf("malloc fail !\n");
        exit(EXIT_FAILURE);
    }
    else
        memset(buf, '0', BUF_SIZE);

    c_len = sizeof(cli_addr);

    /* accept */
    conn_fd = accept(socket_fd, (struct sockaddr *)&cli_addr, &c_len);
    if (conn_fd < 0)
    {
        printf("accept error !\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        n = read(conn_fd, buf, BUF_SIZE);
        if (n < 0)
        {
            printf("[server] : stop recev !\n");
            break;
        }
        else if (n == 0)
        {
            sleep(1);
            continue;
        }
        total += n;

        while (n > 0)
        {
            tmp_n = write(conn_fd, buf, n);
            n -= tmp_n;
        }
    }
    printf("[server] : Total data received = %d\n", total);
    close(socket_fd);
    close(conn_fd);
    free(buf);
    return 0;
}