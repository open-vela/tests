#include <nuttx/config.h>
#include <signal.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>
#include "mmtest.h"

#define SIGENDSIG    -1     /* ending of signal maker                            */
#define NUMPAGES     20    /* default (random) value of number of pages         */
#define PGSIZE       1024   /* default pagesize                                  */
#define NUMTHREAD    5      /* number of threads to spawn default to 32          */
#define READ_FAULT   0      /* instructs routine to simulate read fault          */
#define WRITE_FAULT  1		/* instructs routine to simulate write fault         */
#define COW_FAULT    2		/* instructs routine to simulate copy-on-write fault */
#define THNUM        0		/* array element pointing to number of threads       */
#define MAPADDR      1		/* array element pointing to map address             */
#define PAGESIZ      2		/* array element pointing to page size               */
#define FLTIPE       3		/* array element pointing to fault type              */
#ifndef TRUE
#define TRUE         1
#endif
#ifndef FALSE
#define FALSE        0
#endif
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#endif
#define FAILED       (-1)	/* return status for all funcs indicating failure    */
#define SUCCESS      0		/* return status for all routines indicating success */
#define MAX_PATH     100
#define TMP_DIR      "tmp_dir"   /* thr directory to store test file             */
#define BRKSZ        512*1024    /* program data space allocation value          */

static int test_flag = 0;
static char *BASE_DIR;
static char PATH[MAX_PATH];
//long pagesize = sysconf(_SC_PAGE_SIZE);
static long pagesize = PGSIZE;
static int pages_num = NUMPAGES;    /* number of pages use for tests             */
static int verbose_print = FALSE;   /* print more test information               */
static volatile int alarm_fired;
static volatile int thread_begin;   /* used to coordinate threads                */
static volatile int wait_thread;    /* used to wake up sleeping threads          */

/****************************************************************************
 * Name: mmstress

 * Description:  This is a test program that performs general stress with
                 memory race conditions. It contains six testcases that
                 will test race conditions between simultaneous read fault,
                 write fault, copy on write (COW) fault e.t.c.

 * Usage:        mmstress -h -n TEST NUMBER -p NPAGES -t EXECUTION TIME -v
                          -h                - Help
                          -n TEST NUMBER    - Execute a particular testcase
                          -p NPAGES         - Use NPAGES pages for tests
                          -t EXECUTION TIME - Execute test for a certain time
                          -v                - Verbose output

 * Expect results: TEST PASSED
*****************************************************************************/
static void result_check(int ret)
{
    printf("test completed......\n");
    ret > 0 ? printf("TEST FAILED !\n") : printf("TEST PASSED !\n");
}


static void usage(char *progname)
{
    fprintf(stderr, "usage:%s -h -n test -t time -v\n", progname);
    fprintf(stderr, "\t-h displays all options\n");
    fprintf(stderr, "\t-d temporary directory for test\n");
    fprintf(stderr, "\t-n test number, if no test number\n"
                    "\t   is specified, all the tests will be run\n");
    fprintf(stderr, "\t-p specify the number of pages to\n"
                    "\t   use for allocation\n");
    fprintf(stderr, "\t-t specify the time in hours\n");
    fprintf(stderr, "\t-v verbose output\n");
    exit(1);
}

static void set_timer(int run_time)
{
    struct itimerval timer;

    memset(&timer, 0, sizeof(struct itimerval));
    timer.it_interval.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    timer.it_value.tv_sec = (time_t)(run_time * 3600.0);

    if(setitimer(ITIMER_REAL, &timer, NULL)){
        perror("set_timer(): setitimer()");
        exit(1);
    }
}

static void sig_handler(int signal)
{
    if (signal != SIGALRM){
        fprintf(stderr, "sig_handler(): unexpected signal caught [%d]\n", signal);
        exit(2);
    }

    alarm_fired = 1;
}

/******************************************************************************
 * Function:    thread_fault

 * Description: Executes as a thread function and accesses the memory pages
                depending on the fault_type to be generated. This function
                can cause READ fault, WRITE fault, COW fault.

 * Input:       void *args - argments passed to the exec routine by
                pthread_create()
*******************************************************************************/
static void *thread_fault(void *args)
{
	long *local_args = args;	       /* local pointer to list of arguments   */
	/* local_args[THNUM]   - the thread number   */
	/* local_args[MAPADDR] - map address         */
	/* local_args[PAGESIZ] - page size           */
	/* local_args[FLTIPE]  - fault type          */
    int pgnum_ndx = 0;                 /* index to the number of pages         */
	char read_from_addr = 0;           /* address to which read from page      */
	char write_to_addr[] = { 'a' };	   /* character to be writen to the page   */
	char *start_addr	               /* start address of the page            */
	    = (void *) (local_args[MAPADDR]
			 + (int)local_args[THNUM]
			 * (pages_num / NUMTHREAD)
			 * local_args[PAGESIZ]);

    /*************************************************************/
	/*   The way it was, args could be overwritten by subsequent uses
	 *   of it before this routine had a chance to use the data.
	 *   This flag stops the overwrite until this routine gets to
	 *   here.  At this point, it is done initializing and it is
	 *   safe for the parent thread to continue (which will change
	 *   args).
	 */
    thread_begin = FALSE;

    while (wait_thread)
        sched_yield();

    for (; pgnum_ndx < (pages_num / NUMTHREAD); pgnum_ndx++){
		/* if the fault to be generated is READ_FAULT, read from the page     */
		/* else write a character to the page.                                */
        ((int)local_args[3] == READ_FAULT) ? (read_from_addr = *start_addr)
        :(*start_addr = write_to_addr[0]);
        start_addr += local_args[PAGESIZ];
        if (verbose_print)
            printf("thread_fault(): generating fault type %ld"
            "@page address %p\n", local_args[3], start_addr);
        fflush(NULL);
    }
    pthread_exit(NULL);
}

/******************************************************************************
 * Function:    remove_tmpfiles

 * Description: remove temporary files that were created by the tests.
*******************************************************************************/
static int remove_files(char *filename, char *addr)
{
    if (addr)
        if (munmap(addr, pagesize * pages_num) < 0){
            perror("map_and_thread(): munmap()");
            return FAILED;
        }
    if (strcmp(filename, "NULL") && strcmp(filename, "/dev/zero")){
        if (unlink(filename)){
            perror("map_and_thread(): unlink()");
            return FAILED;
        } else {
            if (verbose_print)
                printf("file %s unlinked\n", filename);
        }
    } else {
        if (verbose_print)
            printf("file %s removed\n", filename);
    }
    return SUCCESS;
}

/******************************************************************************
 * Function:    map_and_thread

 * Description: Creates mappings with the required properties, of MAP_PRIVATE
                MAP_SHARED and of PROT_RED / PROT_READ|PROT_WRITE.
                Create threads and execute a routine that will generate the
                desired fault condition, viz, read, write and cow fault.

 * Input:       char *tmpfile - name of temporary file that is created
                int   fault_type - type of fault that is to be generated.
*******************************************************************************/
int map_and_thread(char *tmpfile, void *(*exec_func)(void *),
                    int fault_type, int num_thread)
{
    int fd = 0;                /* file descriptor of the file created         */
    char *empty_buf = NULL;    /* empty buffer used to fill temp file         */
	int map_type = 0;	       /* specifies the type of the mapped object     */
    void *map_addr = NULL;     /* adderess where the file mapped              */
    int th_args[5];            /* argument list passed to thread_fault()      */
    int thrd_ndx = 0;          /* index to the number of threads created      */
    void *th_status;           /* status of the thread when it is finished    */

    static pthread_t pthread_ids[NUMTHREAD];    /* the threads ids            */

	/* if the name is not a NULL                                              */
    if (strcmp(tmpfile, "NULL")){
        /* Create a file with permissions 0666, and open it with RDRW perms   */
        if ((fd = open(tmpfile, O_RDWR | O_CREAT,
            S_IRWXO | S_IRWXU | S_IRWXG)) == -1){
            perror("map_and_thread(): open()");
            close(fd);
            fflush(NULL);
            return FAILED;
        }

		/* Write pagesize * pages_num bytes to the file */
        empty_buf = malloc(pagesize * pages_num);
        if (write(fd, empty_buf, pagesize * pages_num) != (pagesize * pages_num)){
            perror("map_and_thread(): write()");
            free(empty_buf);
            fflush(NULL);
            remove_files(tmpfile, NULL);
            close(fd);
            return FAILED;
        }

		/* Map the file, if the required fault type is COW_FAULT map the file */
		/* private, else map the file shared. if READ_FAULT is required to be */
		/* generated map the file with read protection else map with read -   */
		/* write protection.                                                  */
        map_type = (fault_type == COW_FAULT) ? MAP_PRIVATE : MAP_SHARED;
        if ((map_addr = (void *)mmap(0, pagesize * pages_num,
                                    ((fault_type == READ_FAULT) ?
                                    PROT_READ : PROT_READ | PROT_WRITE),
                                    map_type, fd, 0)) == MAP_FAILED){
            perror("map_and_thread(): mmap()");
            free(empty_buf);
            fflush(NULL);
            remove_files(tmpfile, NULL);
            close(fd);
            return FAILED;
        } else {
            if (verbose_print)
                printf("map_and_thread(): mmap success, address = %p\n", map_addr);
            fflush(NULL);
        }
    }

	/* As long as wait is set to TRUE, the thread that will be created will */
	/* loop in its exec routine */
    wait_thread = TRUE;

	/* Create a few threads, ideally number of threads equals number of CPU'S */
	/* so that we can assume that each thread will run on a single CPU in     */
	/* of SMP machines. Currently we will create NR_CPUS number of threads.   */
    th_args[1] = (long)map_addr;
    th_args[2] = pagesize;
    th_args[3] = fault_type;
    do{
        th_args[0] = thrd_ndx;
        th_args[4] = (long)0;

        thread_begin = TRUE;

        if (pthread_create(&pthread_ids[thrd_ndx++], NULL, exec_func,
                            (void *)&th_args)){
            perror("map_and_thread(): pthread_create()");
            thread_begin = FALSE;
            free(empty_buf);
            fflush(NULL);
            remove_files(tmpfile, map_addr);
            close(fd);
            return FAILED;
        } else {
            /* Yield until new thread is done with args. */
            while (thread_begin)
                sched_yield();
        }
    } while (thrd_ndx < num_thread);

    if (verbose_print)
        printf("map_and_thread(): pthread_create() success.\n");
    wait_thread = FALSE;

	/* suspend the execution of the calling thread till the execution of the  */
	/* other thread has been terminated.                                      */
    for (thrd_ndx = 0; thrd_ndx < NUMTHREAD; thrd_ndx++){
        if (pthread_join(pthread_ids[thrd_ndx], &th_status)){
            perror("map_and_thread(): pthread_join()");
            free(empty_buf);
            fflush(NULL);
            remove_files(tmpfile, map_addr);
            close(fd);
            return FAILED;
        } else {
            if ((long)th_status == 1) {
                printf("thread [%ld] - process exited with errors\n",
                        (long)pthread_ids[thrd_ndx]);
                free(empty_buf);
                remove_files(tmpfile, map_addr);
                close(fd);
                exit(1);
            }
        }
    }

    if (remove_files(tmpfile, map_addr) == FAILED) {
        free(empty_buf);
        return FAILED;
    }

    free(empty_buf);
    close(fd);
    return SUCCESS;
}

/******************************************************************************
 * Test1:       Test case tests the race condition between simultaneous read
                faults in the same address space.

 * Description: map a file into memory, create threads and execute a thread
                function that will cause read faults by simultaneously reading
                from this memory space.
*******************************************************************************/
static int test1(void)
{
    chdir(PATH);
    if (verbose_print)
    {
        printf("===== TEST 1 =====\n");
        printf("test1: Test case tests the race condition between "
        "simultaneous read faults in the same address space.\n");
        printf("current working directort: %s\n", PATH);
    }
    return map_and_thread("test_file1", thread_fault, READ_FAULT, NUMTHREAD);
}

/******************************************************************************
 * Test2:       Test case tests the race condition between simultaneous write
                faults in the same address space.

 * Description: map a file into memory, create threads and execute a thread
                function that will cause write faults by simultaneously
                writing to this memory space.
*******************************************************************************/
static int test2(void)
{
    chdir(PATH);
    if (verbose_print)
    {
        printf("===== TEST 2 =====\n");
        printf("test2: Test case tests the race condition between "
        "simultaneous write faults in the same address space.\n");
        printf("current working directort: %s\n", PATH);
    }
    return map_and_thread("test_file2", thread_fault, WRITE_FAULT, NUMTHREAD);
}

/******************************************************************************
 * Test3:       Test case tests the race condition between simultaneous COW
                faults in the same address space.

 * Description: map a file into memory, create threads and execute a thread
                function that will cause COW faults by simultaneously
                writing to this memory space.
*******************************************************************************/
static int test3(void)
{
    chdir(PATH);
    if (verbose_print)
    {
        printf("===== TEST 3 =====\n");
        printf("test3: Test case tests the race condition between "
        "simultaneous COW faults in the same address space.\n");
        printf("current working directort: %s\n", PATH);
    }
    return map_and_thread("test_file3", thread_fault, COW_FAULT, NUMTHREAD);
}

/******************************************************************************
 * Test4:       Test case tests the race condition between simultaneous READ
                faults in the same address space. File mapped is /dev/zero

 * Description: Map a file into memory, create threads and execute a thread
                function that will cause READ faults by simultaneously
                writing to this memory space.
*******************************************************************************/
static int test4(void)
{
    if (verbose_print)
    {
        printf("===== TEST 4 =====\n");
        printf("test4: Test case tests the race condition between "
            "simultaneous READ faults in the same address space. "
            "The file mapped is /dev/zero.\n");
    }
	return map_and_thread("/dev/zero", thread_fault, COW_FAULT, NUMTHREAD);
}

static int (*(test_ptr)[]) (void) = {test1, test2, test3, test4};

static void run_test(unsigned int i)
{
    int rc;

    rc = test_ptr[i]();

    if (rc == SUCCESS) {
        printf("TEST %d Passed\n", i + 1);
        test_flag = EXIT_SUCCESS;
    } else {
        printf("TEST %d Failed\n", i + 1);
        test_flag = EXIT_FAILURE;
    }

    if (alarm_fired)
        exit(2);
}

static void mk_tmpdir(void)
{
    int ret;

    sprintf(PATH, "%s/%s", BASE_DIR, TMP_DIR);
    ret = mkdir(PATH, S_IRWXU);
    if (ret == 0)
    {
        printf("make temporary directory %s succeed!\n", PATH);
    } else {
        printf("make temporary directory failed!\n");
        exit(1);
    }
    return;
}

static void rm_tmpdir(void)
{
    DIR *dir = opendir(PATH);
    while (1)
    {
        char fullpath[MAX_PATH] = {0};
        struct dirent *ent = readdir(dir);
        if (ent == NULL)
        {
            closedir(dir);
            rmdir(PATH);
            break;
        }
        strcat(fullpath, PATH);
        strcat(fullpath, "/");
        strcat(fullpath, ent->d_name);
        if (ent->d_type == 2)
        {
            chdir(fullpath);
            rm_tmpdir();
        } else {
            remove(fullpath);
        }
    }
}

int main(int argc, char **argv)
{
    int ch;
    unsigned int i;
    int test_num = 0;
    int test_time = 0;
    int run_once = TRUE;

    static struct signal_info {
        int signum;
        char *signame;
    } sig_info[] = {
        //{SIGHUP, 'SIGHUP'},
        {SIGINT, "SIGINT"},
        {SIGQUIT, "SIGQUIT"},
        //{SIGABRT, 'SIGABRT'},
        //{SIGBUS, 'SIGBUS'},
        //{SIGSEGV, 'SIGSEGV'},
        {SIGALRM, "SIGALRM"},
        {SIGUSR1, "SIGUSR1"},
        {SIGUSR2, "SIGUSR2"},
        {SIGENDSIG, "ENDSIG"}
    };

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    if (argc < 2) {
        printf("run %s -h for all options\n", argv[0]);
        printf("run %s -d [temporary directory] for test\n", argv[0]);
        return EXIT_FAILURE;
    }

    while ((ch = getopt(argc, argv, "hd:n:p:t:v")) != -1) {
        switch (ch) {
            case 'h':
                usage(argv[0]);
                break;
            case 'd':
                BASE_DIR = optarg;
                break;
            case 'n':
                test_num = atoi(optarg);
                break;
            case 'p':
                pages_num = atoi(optarg);
                break;
            case 't':
                printf("Test is scheduled to run for %d hours\n", test_time = atoi(optarg));
                run_once = FALSE;
                break;
            case 'v':
                verbose_print = TRUE;
                break;
            case '?':
                fprintf(stderr, "%s: unknown option - %c ignored\n", argv[0], optopt);
                break;
            default:
                printf("%s: getopt() failed!!!\n", argv[0]);
        }
    }

    set_timer(test_time);

    for (i = 0; sig_info[i].signum != -1; i++) {
        if (signal(sig_info[i].signum, sig_handler) == SIG_ERR) {
            printf("signal(%s) failed", sig_info[i].signame);
        }
    }

    mk_tmpdir();

    do {
        if (!test_num){
            for (i = 0; i < ARRAY_SIZE(test_ptr); i++)
                run_test(i);
        } else {
            if (test_num > (int)ARRAY_SIZE(test_ptr)){
                printf("Invalid test number %i\n", test_num);
            }

            run_test(test_num - 1);
        }

    } while (!run_once);

    rm_tmpdir();
    optind = 0;
    verbose_print = FALSE;
	result_check(test_flag);
	exit(test_flag);
}
