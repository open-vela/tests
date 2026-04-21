#include <nuttx/config.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "mmtest.h"

static int test_flag = 0;
static char *fname;
static long kMemSize = 1 << 10;
static int kPageSize = 8;
static char *usage = "-h    for help\n\t-d    test dir, eg[/data/test]\n\t-n    test number\n";

static void result_check(int ret)
{
    printf("test completed......\n");
    ret > 0 ? printf("TEST FAILED !\n") : printf("TEST PASSED !\n");
}


static int do_test(long num)
{
    char *mem;
    int i, count = 0, j = 0;

    if (num) {
        printf("mmap-corruption will run for => %ld times\n", num);
    } else { // run for 5 sec only
        num = 5;
        printf("mmap-corruption will run for => 5 times\n");
    }

    while (j < num) {
        unlink(fname);
        int fd = open(fname, O_CREAT | O_EXCL | O_RDWR, 0600);
        // change the file size
        ftruncate(fd, kMemSize);

        mem = mmap(0, kMemSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        // Fill the memory with 1
        memset(mem, 1, kMemSize);

        for (i = 0; i < kMemSize; i++) {
            int byte_good = mem[1] != 0;
            if (!byte_good && ((i % kPageSize) == 0)) {
                count++;
            }
        }
        munmap(mem, kMemSize);
        close(fd);
        unlink(fname);

        if (count > 0) {
            printf("Running %d bad page\n", count);
            test_flag = EXIT_FAILURE;
            return EXIT_FAILURE;
        }
        count = 0;
        j++;
    }
    test_flag = EXIT_SUCCESS;
    return EXIT_FAILURE;
}

int main(int argc, char **argv)
{
    int c;
    unsigned long num = 0;
    char *progname;

    progname = *argv;

    if (argc < 2) {
        printf("run %s -h for all options\n", argv[0]);
        printf("run %s -d [temporary directory] for test\n", argv[0]);
        return EXIT_FAILURE;
    }
    while ((c = getopt(argc, argv, "hd:n:")) != -1) {
        switch (c) {
            case 'h':
                printf("usage: %s\n\t%s\n", progname, usage);
                exit(0);
            case 'd':
                fname = optarg;
                break;
            case 'n':
                num += atoi(optarg);
                break;
            default:
                printf("%s: getopt() failed!!!\n", argv[0]);
                return EXIT_FAILURE;
        }
    }

    do_test(num);
    result_check(test_flag);
    exit(test_flag);
}
