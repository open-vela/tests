#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "mmtest.h"

#define MEMSIZE 8192 * 8192
static int test_flag = 0;
/****************************************************************************
 * Name: mem02
 * Example description:
		 check out alloc function test
 * Expect results: TEST PASSED
 ****************************************************************************/
void on_mem_fault(int sig);

static void result_check(int ret)
{
    printf("test completed......\n");
    ret > 0 ? printf("TEST FAILED !\n") : printf("TEST PASSED !\n");
}


static m_info get_meminfo(void)
{
	int fd, memread, count = 1;
	FAR char *buffer, *p = NULL;
	m_info velaMemInfo;
	fd = open("/proc/meminfo", O_RDONLY);
	if (fd < 0)
	{
		printf("error : open /proc/meminfo failed !\n");
		exit(EXIT_FAILURE);
	}
	buffer = (FAR char *)malloc(IOBUFFERSIZE);
	memset(buffer, '\0', IOBUFFERSIZE);
	if (buffer == NULL)
	{
		close(fd);
		exit(EXIT_FAILURE);
	}
	memread = read(fd, buffer, IOBUFFERSIZE);
	if (memread < 0)
	{
		close(fd);
		exit(EXIT_FAILURE);
	}
	p = strtok(buffer, " \n");
	while (p != NULL)
	{
		count++;
		if (count == 8)
		{
			velaMemInfo.mem_total = atol(strtok(NULL, " \n"));
			velaMemInfo.mem_used = atol(strtok(NULL, " \n"));
			velaMemInfo.mem_free = atol(strtok(NULL, " \n"));
			velaMemInfo.mem_largest = atol(strtok(NULL, " \n"));
		}
		p = strtok(NULL, " \n");
	}
	close(fd);
	free(buffer);
	return velaMemInfo;
}

static int get_pagesize_order(void)
{
	int i, pagesize, size = 0;
	pagesize = 4; // sysconf(_SC_PAGESIZE);
	for (i = 0; size != pagesize; i++)
		size = 1 << i;
	return (i - 1);
}

static void do_test(void)
{
	int i;
	char *pm1 = NULL;
	char *pm2 = NULL;
	char *pm3 = NULL;
	char *pm4 = NULL;
	void *memptr;
	long laddr;
	int iteration_count;
	int size; /* Size to memory to be valloced */
	int pagesize;
	m_info memInfo;
	int pagesize_order = get_pagesize_order();
	int memsize = MEMSIZE; /* Size of memory to allocate */
	pagesize = 4;		   // sysconf(_SC_PAGESIZE);
	memInfo = get_meminfo();
	if (memInfo.mem_largest < pagesize)
	{
		printf("memory not enough. \n");
		test_flag = EXIT_FAILURE;
		return;
	}
	/*	Always reserve 1MB memory to avoid OOM Killer.	*/
	memsize = memInfo.mem_largest * 0.9;

	/*--------------------------------------------------------------------*/

	/* check out calloc/free */
	if ((pm2 = pm1 = calloc(memsize, 1)) == NULL)
	{
		printf("calloc - alloc of %dMB failed.\n", memsize / 1024 / 1024);
		test_flag = EXIT_FAILURE;
		return;
	}

	for (i = 0; i < memsize; i++)
		if (*pm2++ != 0)
		{
			printf("calloc returned non zero memory.\n");
			test_flag = EXIT_FAILURE;
			return;
		}

	pm2 = pm1;
	for (i = 0; i < memsize; i++)
		*pm2++ = 'X';
	pm2 = pm1;
	for (i = 0; i < memsize; i++)
		if (*pm2++ != 'X')
		{
			printf("could not write/verify memory.\n");
			test_flag = EXIT_FAILURE;
			return;
		}
	free(pm1);
	printf("calloc - calloc of %uMB of memory succeeded.\n", memsize / 1024 / 1024);
	test_flag = EXIT_SUCCESS;

	/*--------------------------------------------------------------------*/

	/* check out malloc/free */
	if ((pm2 = pm1 = malloc(memsize)) == NULL)
	{
		printf("malloc did not alloc memory.\n");
		test_flag = EXIT_FAILURE;
		return;
	}

	for (i = 0; i < memsize; i++)
		*pm2++ = 'X';
	pm2 = pm1;
	for (i = 0; i < memsize; i++)
		if (*pm2++ != 'X')
		{
			printf("could not write/verify memory.\n");
			test_flag = EXIT_FAILURE;
			return;
		}
	free(pm1);

	printf("malloc - malloc of %uMB of memory succeeded.\n", memsize / 1024 / 1024);
	test_flag = EXIT_SUCCESS;

	/*--------------------------------------------------------------------*/

	/* check out realloc */
	pm4 = pm3 = malloc(10);
	for (i = 0; i < 10; i++)
		*pm4++ = 'X';

	/* realloc with reduced size */
	pm4 = realloc(pm3, 5);
	pm3 = pm4;
	/* verify contents did not change */
	for (i = 0; i < 5; i++)
	{
		if (*pm4++ != 'X')
		{
			printf("realloc changed memory contents.\n");
			test_flag = EXIT_FAILURE;
			return;
		}
	}

	printf("realloc - realloc of 5 bytes succeeded.\n");
	test_flag = EXIT_SUCCESS;

	/* realloc with increased size after fragmenting memory */
	pm4 = realloc(pm3, 15);
	pm3 = pm4;
	/* verify contents did not change */
	for (i = 0; i < 5; i++)
	{
		if (*pm3++ != 'X')
		{
			printf("realloc changed memory contents.\n");
			test_flag = EXIT_FAILURE;
			return;
		}
	}

	printf("realloc - realloc of 15 bytes succeeded.\n");
	test_flag = EXIT_SUCCESS;

	/*--------------------------------------------------------------------*/

	/* Check out for valloc failures */
	/*
	 * Setup to catch the memory fault, otherwise the core might
	 * be dumped on failures.
	if ((calloc(SIGSEGV, on_mem_fault)) == SIG_ERR)
	{
		printf("Could not get signal handler for SIGSEGV.\n");
		test_flag = EXIT_FAILURE;
		return;
	}
	 */

	srand(1); /* Ensure Determinism */

	for (iteration_count = 15000; iteration_count > 0; iteration_count--)
	{

		/*
		 * size is a fraction of 100000 and is determined by rand().
		 */
		size = (int)((rand() / (float)RAND_MAX) * 10000) + 1;
		memptr = valloc(size);

		if (memptr == NULL)
		{
			printf("Valloc return NULL ptr.\n");
			continue;
		}

		/*
		 * Check to see if valloc returns unaligned data.
		 * This can be done by copying the memory address into
		 * a variable and the by diving and multipying the address
		 * by the pagesize order and checking.
		 */
		laddr = (long)memptr;
		if (((laddr >> pagesize_order) << pagesize_order) != laddr)
		{
			printf("Valloc returned unaligned data.\n");
			test_flag = EXIT_FAILURE;
			return;
		}
		free(memptr);
	}
	printf("valloc - valloc of rand() size of memory succeeded for 15000 iteration.\n");
	test_flag = EXIT_SUCCESS;
	return;
}

/*
 * void
 * on_mem_fault(int sig)
 *
 *	on_mem_fault() is a signal handler used by the valloc test-case
 *	(block 3). This function will catch the signal, indicate a failure,
 *	write to the log file (a failure message) and exit the test.
 */
void on_mem_fault(int sig)
{
	printf("\tTest failed on receipt of a SIGSEGV signal.\n");
	test_flag = EXIT_FAILURE;
	return;
}

int main(int argc, FAR char *argv[])
{
	do_test();
	result_check(test_flag);
	exit(test_flag);
}
