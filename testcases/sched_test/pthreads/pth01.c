#include <nuttx/config.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "sched_test.h"
#include <syslog.h>
#define DEFAULT_NUM_THREADS 10

static void sys_error(const char *, int);
static void error(const char *, int);
static void parse_args(void);
static void *thread(void *);

static int num_threads = DEFAULT_NUM_THREADS;
static int test_limit = 0;
static int debug = 0;

/*---------------------------------------------------------------------+
|                               main ()                                |
| ==================================================================== |
|                                                                      |
| Function:  Main program  (see prolog for more details)               |
|                                                                      |
+---------------------------------------------------------------------*/
int main(int argc, FAR char *argv[])
{
	/* init */
	num_threads = DEFAULT_NUM_THREADS;
	test_limit = 0;
	debug = 0;
	/*
	 * Parse command line arguments and print out program header
	 */
	parse_args();

	if (test_limit)
	{
		syslog(LOG_INFO, "Creating as many threads as possible\n");
	}
	else
	{
		syslog(LOG_INFO, "Creating %d threads\n", num_threads);
	}
	thread(0);

	/*
	 * Program completed successfully...
	 */
	syslog(LOG_INFO, "TEST PASSED !\n");
	exit(0);
}

/*---------------------------------------------------------------------+
|                               thread ()                              |
| ==================================================================== |
|                                                                      |
| Function:  Recursively creates threads while num < num_threads       |
|                                                                      |
+---------------------------------------------------------------------*/
void *thread(void *parm)
{
	intptr_t num = (intptr_t)parm;
	pthread_t th;
	pthread_attr_t attr;
	size_t stacksize = PTHREAD_STACK_SIZE;
	int pcrterr;
	if (num >= (UINT8_MAX - 1))
	{
		syslog(LOG_INFO, "Reach the upper limit of the number of threads, exit the test ！ \n");
		return 0;
	}
	syslog(LOG_INFO, "thread no.%"PRIdPTR"\n", num);
	/*
	 * Create threads while num < num_threads...
	 */
	if (test_limit || (num < num_threads))
	{

		if (pthread_attr_init(&attr))
			sys_error("pthread_attr_init failed", __LINE__);
		if (pthread_attr_setstacksize(&attr, stacksize))
			sys_error("pthread_attr_setstacksize failed", __LINE__);
		if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE))
			sys_error("pthread_attr_setdetachstate failed",
					  __LINE__);
		/************************************************/
		/*   pthread_create does not touch errno.  It RETURNS the error
		 *   if it fails.  errno has no bearing on this test, so it was
		 *   removed and replaced with return value check(see man page
		 *   for pthread_create();
		 */
		pcrterr = pthread_create(&th, &attr, thread, (void *)(num + 1));
		if (pcrterr != 0)
		{
			if (test_limit)
			{
				syslog(LOG_INFO, "Testing pthread limit, %d pthreads created.\n", (int)num);
				pthread_exit(0);
			}
			if (pcrterr == EAGAIN)
			{
				syslog(LOG_INFO, "Thread [%d]: unable to create more threads!\n", (int)num);
				return NULL;
			}
			else
				sys_error("pthread_create failed\n", __LINE__);
		}
		pthread_join(th, NULL);
	}

	return 0;
}

static void parse_args(void)
{
	debug++;
	test_limit++;
	num_threads = 10;
}

static void sys_error(const char *msg, int line)
{
	char syserr_msg[256];
	sprintf(syserr_msg, "%s: %s\n", msg, strerror(errno));
	error(syserr_msg, line);
}

static void error(const char *msg, int line)
{
	fprintf(stderr, "ERROR [line: %d] %s\n", line, msg);
	syslog(LOG_INFO, "TEST FAILED !\n");
	exit(EXIT_FAILURE);
}
