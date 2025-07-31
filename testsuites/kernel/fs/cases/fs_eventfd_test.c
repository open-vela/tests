/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <pthread.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <sys/eventfd.h>
#include "fstest.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: threadFunc
 ****************************************************************************/
__attribute__((unused)) static void *threadFunc(void *args)
{
    eventfd_t Eventfd01_buffer;
    int fd = *(int *)args;
    for (int i = 1; i < 6; i++)
    {
        read(fd, &Eventfd01_buffer, sizeof(eventfd_t));
        sleep(1);
    }

    return 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsEventfd
 ****************************************************************************/
void TestNuttxFsEventfd(FAR void **state)
{
#ifdef CONFIG_EVENT_FD
    eventfd_t Eventfd01_buf = 1;
    int Eventfd01_ret;
    int Eventfd01_efd;
    pthread_t Eventfd01_tid;
    struct fs_testsuites_state_s *test_state;
    test_state = (struct fs_testsuites_state_s *)*state;

    Eventfd01_efd = eventfd(0, 0);
    assert_int_not_equal(Eventfd01_efd,-1);
    test_state->fd1=Eventfd01_efd;
    assert_true(pthread_create(&Eventfd01_tid, NULL, threadFunc, &Eventfd01_efd)>=0);

    for (int i = 1; i < 5; i++)
    {
        Eventfd01_ret = write(Eventfd01_efd, &Eventfd01_buf, sizeof(eventfd_t));
        assert_int_equal(Eventfd01_ret,sizeof(eventfd_t));
        Eventfd01_buf++;
        sleep(1);
    }
    sleep(2);
#endif
}
