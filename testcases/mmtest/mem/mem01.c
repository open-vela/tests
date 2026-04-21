#include <nuttx/config.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mmtest.h"

#define TEST_NUM 10

/* random access versus linear */
static int r_opt = 0;
static int test_flag = 0;

/****************************************************************************
 * Name: mem01
 * Example description:
	1. Basic memory stress test
 * Expect results: TEST PASSED
 ****************************************************************************/
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

static void do_test(void)
{
	int loop_count;
	size_t pagesize;
	m_info memInfo;
	char *p, *bigmalloc;
	unsigned long memsize;
	pagesize = 4; // sysconf(_SC_PAGESIZE);

	for (int lc = 0; lc < TEST_NUM; lc++)
	{
		/*	Get system memory information from /proc/meminfo	*/
		memInfo = get_meminfo();
		if (memInfo.mem_free < pagesize)
		{
			printf("memory not enough. \n");
			test_flag = EXIT_FAILURE;
			return;
		}
		/*	Always reserve 1MB memory to avoid OOM Killer.	*/
		memsize = memInfo.mem_largest * 0.9;
		bigmalloc = p = malloc(memsize);

		if (!p)
		{
			printf("malloc - alloc of %ld MB failed", memsize / 1024 / 1024);
			test_flag = EXIT_FAILURE;
			return;
		}

		loop_count = memsize / pagesize;

		for (int i = 0; i < loop_count; i++)
		{
			p = (char *)((uintptr_t)p & (~0x3));
			*(int *)p = 0xdeadbeef ^ i;
			if (r_opt)
			{
				p = bigmalloc +
					(size_t)((double)(memsize - sizeof(int)) * rand() / (RAND_MAX + 1.0));
			}
			else
			{
				p += pagesize;
			}
		}
		free(bigmalloc);
	}
	test_flag = EXIT_SUCCESS;
	return;
}

int main(int argc, FAR char *argv[])
{
	do_test();
	result_check(test_flag);
	exit(test_flag);
}