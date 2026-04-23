#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>

#define MAX_RECORD 100

static void show_usages(void)
{
    syslog(LOG_WARNING, "Usage: CMD [-c <repeat nums>] [-i <interval (s)>] [-v] \"cmd you want to exec\"\n"
                        "\t\t-c: set the number of repetitions, default 5\n"
                        "\t\t-i: set the interval (seconds) between the repetitions, default 1 s\n"
                        "\t\t-v: display run time of each rept\n");
    exit(1);
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        show_usages();
    }
    int count = 5;
    // pid_t pid;
    char *cmd;
    int status;
    int failcnt = 0;
    int fail_idx[MAX_RECORD];
    int interval = 1;
    struct timeval start, end, total, delta;
    timerclear(&total);
    bool vlog = 0;
    int o;
    while ((o = getopt(argc, argv, "c:i:v")) != EOF)
    {
        switch (o)
        {
        case 'c':
            count = atoi(optarg);
            break;
        case 'i':
            interval = atoi(optarg);
            break;
        case 'v':
            vlog = 1;
            break;
        default:
            show_usages();
            break;
        }
    }
    if (optind != argc - 1)
    {
        syslog(LOG_ERR, "please input the command you want to exec\n");
        show_usages();
    }
    cmd = argv[optind];
    for (int i = 0; i < count; i++)
    {
        syslog(LOG_INFO, "\nrept: %d\n", i);
        gettimeofday(&start, NULL);
        status = system(cmd);
        gettimeofday(&end, NULL);
        if (WIFEXITED(status))
        {
            if (WEXITSTATUS(status) != 0)
            {
                syslog(LOG_ERR, "child fail, return %d\n", WEXITSTATUS(status));
                if (failcnt < MAX_RECORD)
                {
                    fail_idx[failcnt++] = i;
                }
            }
        }
        else
        {
            syslog(LOG_ERR, "child Abnormal exit, status %d\n", status);
            break;
        }
        timersub(&end, &start, &delta);
        timeradd(&total, &delta, &total);
        if (vlog)
        {
            syslog(LOG_INFO, "rept num %d, takes %lld s %lld us\n", i, (long long)delta.tv_sec, (long long)delta.tv_usec);
        }
        // just sleep
        delta.tv_sec = interval;
        delta.tv_usec = 0;
        select(0, NULL, NULL, NULL, &delta);
    }

    for (int i = 0; i < failcnt; i++)
    {
        syslog(LOG_ERR, "The %d execution failed\n", fail_idx[i]);
    }
    syslog(LOG_INFO, "rept done, total take %lld s %lld us\n", (long long)total.tv_sec, (long long)total.tv_usec);
    return 0;
}