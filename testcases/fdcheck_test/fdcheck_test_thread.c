#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

static int fdcheck_test_thread_fd1, fdcheck_test_thread_fd2;
static sem_t sem_fdcheck;

static void *thread0_test(void *arg) {
    fdcheck_test_thread_fd1 = open("/dev/null", O_RDONLY);
    close(fdcheck_test_thread_fd1);
    sem_post(&sem_fdcheck);
    return NULL;
}

static void *thread1_test(void *arg) {
    sem_wait(&sem_fdcheck);
    fdcheck_test_thread_fd2 = open("/dev/null", O_RDONLY);
    close(fdcheck_test_thread_fd1); // fdcheck will throw Assertion failed panic here
    return NULL;
}

int main(int argc, char **argv)
{
    // Initialize semaphore to guarantee thread execution order
    sem_init(&sem_fdcheck, 1, 0);
    
    int ret;
    pthread_t thread[2];
    // Create thread 1
    ret = pthread_create(&thread[0], NULL, thread1_test, NULL);
    if (ret < 0) {
        syslog(LOG_ERR, "create pthread error\n");
        exit(1);
    }

    // Create thread 2
    ret = pthread_create(&thread[1], NULL, thread0_test, NULL);
    if (ret < 0) {
        syslog(LOG_ERR, "create pthread error\n");
        exit(1);
    }
    
    pthread_join(thread[0], NULL);
    pthread_join(thread[1], NULL);
    syslog(0, "fd1 %d fd2 %d\n", fdcheck_test_thread_fd1, fdcheck_test_thread_fd2);

    return 0;
}