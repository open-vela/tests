#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

#define INVALID_SOCKET          -1
#define CLIENT_NUM              1
#define FDCHECK_TEST_LOCALPATH  "fdcheck_local"

static int gFds[FD_SETSIZE];

static void InitFds(void)
{
    for (int i = 0; i < FD_SETSIZE; ++i) {
        gFds[i] = INVALID_SOCKET;
    }
}

static void GetReadfds(fd_set *fds, int *nfd)
{
    for (int i = 0; i < FD_SETSIZE; i++) {
        if (gFds[i] == INVALID_SOCKET) {
            continue;
        }
        FD_SET(gFds[i], fds);
        if (*nfd < gFds[i]) {
            *nfd = gFds[i];
        }
    }
}

static int AddFd(int fd)
{
    for (int i = 0; i < FD_SETSIZE; ++i) {
        if (gFds[i] == INVALID_SOCKET) {
            gFds[i] = fd;
            return 0;
        }
    }
    return -1;
}


static void *ClientsThread(void *param)
{
    int thrNo = (int)(intptr_t)param;
    int fd;

    syslog(LOG_INFO, "<%d>socket client thread started", thrNo);
    fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (fd == INVALID_SOCKET) {
        perror("socket");
        return NULL;
    }

    struct sockaddr_un sa;
    sa.sun_family = AF_LOCAL;
    strncpy(sa.sun_path, FDCHECK_TEST_LOCALPATH, UNIX_PATH_MAX);
    if (connect(fd, (struct sockaddr *)(&sa), sizeof(sa)) == -1) {
        perror("connect");
        return NULL;
    }

    syslog(LOG_INFO,"[%d]<%d>connected to local://%s successful",
           fd, thrNo, FDCHECK_TEST_LOCALPATH);

    const char *msg[] = {
            "ohos, ",
            "hello, ",
            "my name is net_socket_test_011, ",
            "see u next time, ",
            "Bye!",
    };

    for (int i = 0; i < sizeof(msg) / sizeof(msg[0]); ++i) {
        if (send(fd, msg[i], strlen(msg[i]), 0) < 0) {
            syslog(LOG_INFO,"[%d]<%d>send msg [%s] fail", fd, thrNo, msg[i]);
        }
    }

    (void)shutdown(fd, SHUT_RDWR);
    (void)close(fd);
    return param;
}

static int StartClients(pthread_t *cli, int cliNum)
{
    int ret;
    pthread_attr_t attr = {0};
    struct sched_param param = { 0 };
    int policy;
    ret = pthread_getschedparam(pthread_self(), &policy, &param);

    for (int i = 0; i < cliNum; ++i) {
        ret = pthread_attr_init(&attr);
        param.sched_priority = param.sched_priority + 1;
        pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedparam(&attr, &param);

        ret = pthread_create(&cli[i], &attr, ClientsThread, (void *)(intptr_t)i);
        if (ret != 0) {
            syslog(LOG_ERR, "ret: %d", ret);
            return -1;
        }
        ret = pthread_attr_destroy(&attr);
        if (ret != 0) {
            syslog(LOG_ERR, "ret: %d", ret);
            return -1;
        }
    }
    return 0;
}


int main(int argc, char **argv)
{
    int sock_fd1, sock_fd2, ret;
    struct sockaddr_un sa = {0};

    // Create fd1
    sock_fd1 = socket(AF_LOCAL, SOCK_STREAM, 0);
    syslog(0, "sock_fd1: %d\n", sock_fd1);

    // Create client
    sa.sun_family = AF_LOCAL;
    strncpy(sa.sun_path, FDCHECK_TEST_LOCALPATH, UNIX_PATH_MAX);
    ret = bind(sock_fd1, (struct sockaddr *)(&sa), sizeof(sa));

    InitFds();
    AddFd(sock_fd1);

    pthread_t clients[CLIENT_NUM];
    ret = StartClients(clients, CLIENT_NUM);
    
    // Use select to operate fd1
    int nfd;
    fd_set readfds;
    struct timeval timeout;

    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    nfd = 0;
    FD_ZERO(&readfds);
    GetReadfds(&readfds, &nfd);
    ret = select(nfd + 1, &readfds, NULL, NULL, &timeout);
    syslog(LOG_INFO,"select %d", ret);
    if (ret == -1) {
        perror("select");
    } else if (ret == 0) {
        syslog(LOG_INFO, "select timeout");
    }

    // Close fd1
    close(sock_fd1);

    // Create fd2
    sock_fd2 = socket(AF_LOCAL, SOCK_STREAM, 0);
    syslog(0, "sock_fd2: %d\n", sock_fd2);

    // Use select to operate fd1 again
    ret = select(nfd + 1, &readfds, NULL, NULL, &timeout); // fdcheck will throw Assertion failed panic here
    syslog(LOG_INFO,"select %d", ret);
    if (ret == -1) {
        perror("select");
    } else if (ret == 0) {
        syslog(LOG_INFO, "select timeout");
    }

    syslog(0, "TEST FAILED\n");
    return 0;
}
