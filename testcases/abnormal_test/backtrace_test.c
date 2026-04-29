#include <nuttx/config.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/****************************************************************************
 * Name: creat
 * Example description:
 	1. Actively trigger a segmentation fault.
	2. Test the function of backtrace.
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
    int delay_time = 0;
    // char *buf = NULL;
    if (argc == 3)
    {
        if (strcmp("-t", argv[1]) == 0)
        {
            delay_time = atoi(argv[2]);
        }
    }
    else
    {
        printf("The argument is wrong !\n");
        printf("usage   CMD [-t  Delay time] & \n");
        exit(1);
    }

    /* How long to wait */
    sleep(delay_time);

    /* Visit NULL address, Trigger segfault */
    // memset(buf, 'A', 1);

    return 0;
}