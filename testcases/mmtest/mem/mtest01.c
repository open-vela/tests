#include <nuttx/config.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "mmtest.h"

static int test_flag = 0;

#define ALLOC_THRESHOLD		 (256*1024)  /* memory allocated by each child process */

static pid_t *pid_list;
static int max_pids;
static unsigned long long alloc_maxbytes;

static int chunksize = 64*1024;
static int maxpercent = 20;

/****************************************************************************
 * Name: mtest01
 * Example description:
         * mtest01 mallocs memory <chunksize> at a time until malloc fails.
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



static void do_write_mem(char *mem, int chunk_size)
{
	int i, pagesz = getpagesize();
	for (i = 0; i < chunk_size; i += pagesz)
		*(mem + i) = 'a';
}

static void setup(void)
{
    int pagesize;
	m_info memInfo;
	unsigned long long total_free;

	/* get system meminfo */
	pagesize = sysconf(_SC_PAGESIZE);
    memInfo = get_meminfo();
    if (memInfo.mem_free < pagesize)
    {
        printf("memory not enough. \n");
        test_flag = EXIT_FAILURE;
        return;
    }
	total_free = memInfo.mem_free;
	max_pids = total_free / (unsigned long)ALLOC_THRESHOLD;
	pid_list = malloc(max_pids * sizeof(pid_t));
    if (pid_list == NULL)
    {
        printf("malloc() failed. \n");
        test_flag = EXIT_FAILURE;
        return;
    }

	if (!alloc_maxbytes)
	{
		/* set alloc_maxbytes to the extra amount we want to allocate */
		alloc_maxbytes = ((float)maxpercent / 100.00) * total_free;
		printf("Filling up %d%% of free ram which is %llu kbytes. \n",
			 maxpercent, alloc_maxbytes / 1024);
	}
}

static void cleanup(void)
{
	if(pid_list)
		free(pid_list);
}

static void child_loop_alloc(unsigned long long alloc_bytes)
{
	unsigned long bytecount = 0;
	char *mem;

	printf("... child %d starting, chunksize is %d. \n", getpid(), chunksize);

	while (1) {
		mem = malloc(chunksize);
        if (mem == NULL)
        {
            printf("malloc() failed. \n");
			test_flag = EXIT_FAILURE;
			return;
        }
		do_write_mem(mem, chunksize);

		bytecount += chunksize;
		printf("child %d allocated %lu bytes. \n", getpid(), bytecount);
		if (bytecount >= alloc_bytes)
			break;
	}
	printf("... %lu bytes allocated and used in child %d. \n", bytecount, getpid());
	exit(0);
}

static void do_test(void)
{
	pid_t pid;
	int i = 0, pid_cntr = 0;
	unsigned long long alloc_bytes = alloc_maxbytes;
	const char *write_msg = "";
    write_msg = "(and written to)";

	do {
		pid = vfork();
        if (pid < 0)
		{
            printf("fork() failed. \n");
			test_flag = EXIT_FAILURE;
			return;
		}
		if (pid == 0)
		{
			alloc_bytes = ALLOC_THRESHOLD > alloc_bytes ? alloc_bytes : ALLOC_THRESHOLD;
			child_loop_alloc(alloc_bytes);
		}

		pid_list[pid_cntr++] = pid;

		if (alloc_bytes <= ALLOC_THRESHOLD)
			break;

		alloc_bytes -= ALLOC_THRESHOLD;
	} while (pid_cntr < max_pids);

	for (i = 0; i < pid_cntr; i++)
		kill(pid_list[i], SIGKILL);

	printf("%llu kbytes allocated %s. \n", alloc_maxbytes / 1024, write_msg);

	test_flag = EXIT_SUCCESS;
    return;
}

int main(int argc, FAR char *argv[])
{
        setup();
        do_test();
        cleanup();
        result_check(test_flag);
        exit(test_flag);
}
