#include <nuttx/config.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <syslog.h>
#include <time.h>
#include <sched.h>
#include <sys/types.h>

static double write_speed(char *filepath, size_t block, int count)
{
    double duration, speed;
    clock_t start, finish;
    int ret, i;
    int out = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0700);
    if (out < 0)
    {
        syslog(LOG_ERR, "open %s fail ! errno=%d\n", filepath, errno);
        return 0;
    }
    char *buf = (char *)malloc(block);
    if (buf == NULL)
    {
        close(out);
        return 0;
    }
    memset(buf, 65, block);
    start = clock();
    for (i = 0; i < count; i++)
    {
        ret = write(out, buf, block);
        if (ret < 0 || ret != block)
        {
            syslog(LOG_INFO, "write failed , errno %d, i %d, block %zu\n", errno, i, block);
            break;
        }
    }
    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    speed = block / 1024.0 * i / duration;
    free(buf);
    close(out);
    return speed;
}

static double read_speed(char *filepath, size_t block, int count)
{
    double duration, speed;
    clock_t start, finish;
    int ret, i;
    int out = open(filepath, O_RDONLY, 0700);
    if (out < 0)
    {
        syslog(LOG_ERR, "open %s fail ! errno=%d\n", filepath, errno);
        return 0;
    }
    char *buf = (char *)malloc(block);
    if (buf == NULL)
    {
        close(out);
        return 0;
    }
    start = clock();
    for (i = 0; i < count; i++)
    {
        ret = read(out, buf, block);
        if (ret <= 0 || ret != block)
        {
            syslog(LOG_INFO, "read failed , errno %d, i %d, block %zu\n", errno, i, block);
            break;
        }
    }
    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    speed = block / 1024.0 * i / duration;
    free(buf);
    close(out);
    return speed;
}

static double rand_write_speed(char *filepath, size_t block, int count)
{
    srand((unsigned int)time(NULL));
    double duration, speed;
    clock_t start, finish;
    int ret, i;
    int out = open(filepath, O_WRONLY, 0700);
    if (out < 0)
    {
        syslog(LOG_ERR, "open %s fail ! errno=%d\n", filepath, errno);
        return 0;
    }
    char *buf = (char *)malloc(block);
    if (buf == NULL)
    {
        close(out);
        return 0;
    }
    memset(buf, 65, block);
    start = clock();
    for (i = 0; i < count; i++)
    {
        ret = lseek(out, (rand() % count) * block, SEEK_SET);
        if (ret < 0)
        {
            syslog(LOG_INFO, "write seek failed , errno %d, i %d, block %zu\n", errno, i, block);
            break;
        }
        ret = write(out, buf, block);
        if (ret < 0 || ret != block)
        {
            syslog(LOG_INFO, "write failed , errno %d, i %d, block %zu\n", errno, i, block);
            break;
        }
    }
    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    speed = block / 1024.0 * i / duration;
    free(buf);
    close(out);
    return speed;
}

static double rand_read_speed(char *filepath, size_t block, int count)
{
    srand((unsigned int)time(NULL));
    double duration, speed;
    clock_t start, finish;
    int ret, i;
    int out = open(filepath, O_RDONLY, 0700);
    if (out < 0)
    {
        syslog(LOG_ERR, "open %s fail ! errno=%d\n", filepath, errno);
        return 0;
    }
    char *buf = (char *)malloc(block);
    if (buf == NULL)
    {
        close(out);
        return 0;
    }
    start = clock();
    for (i = 0; i < count; i++)
    {
        ret = lseek(out, (rand() % count) * block, SEEK_SET);
        if (ret < 0)
        {
            syslog(LOG_INFO, "read seek failed , errno %d, i %d, block %zu\n", errno, i, block);
            break;
        }
        ret = read(out, buf, block);
        if (ret <= 0 || ret != block)
        {
            syslog(LOG_INFO, "read failed , errno %d, i %d, block %zu\n", errno, i, block);
            break;
        }
    }
    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    speed = block / 1024.0 * i / duration;
    free(buf);
    close(out);
    return speed;
}

static void auto_test(char *filepath)
{
    size_t blocks[9] = {128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768};
    int count[9] = {1760, 880, 440, 220, 110, 55, 28, 14, 7};
    double speeds[4][9];
    syslog(LOG_ERR, "performance_test start\n");
    for (int i = 0; i < 9; i++)
    {
        unlink(filepath);
        speeds[0][i] = write_speed(filepath, blocks[i], count[i]);
        speeds[1][i] = read_speed(filepath, blocks[i], count[i]);
        syslog(LOG_ERR, "block %zu, write speed %lf KB/s, read speed %lf KB/s\n", blocks[i], speeds[0][i], speeds[1][i]);
    }
    unlink(filepath);
    syslog(LOG_ERR, "performance_test finish\n");
    // syslog(LOG_INFO, "%10s%12s%12s%12s%12s%12s%12s%12s%12s\n", " ", "512", "1k", "2k", "4k", "8k", "16k", "32k", "64k");
    // syslog(LOG_INFO, "%10s%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf\n\n", "write", speeds[0][0],
    //        speeds[0][1], speeds[0][2], speeds[0][3], speeds[0][4], speeds[0][5], speeds[0][6], speeds[0][7]);
    // syslog(LOG_INFO, "%10s%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf\n\n", "read", speeds[1][0],
    //        speeds[1][1], speeds[1][2], speeds[1][3], speeds[1][4], speeds[1][5], speeds[1][6], speeds[1][7]);
}

static void show_usages(void)
{
    syslog(LOG_WARNING, "Usage: CMD [-d <dir>] [-f <filepath>] [-b <block size>] [-c <write/read count>] [-m <mode>] [-h]\n"
                        "\t\t-d: set dir\n"
                        "\t\t-f: set filepath\n"
                        "\t\t-b: set block size, can end with KkMm, default 1k\n"
                        "\t\t-c: set write/read count, default 1000\n"
                        "\t\t-m: 0 for auto, 1 for write, 2 for read, 3 for rand write, 4 for rand read, default 0\n"
                        "\t\t-h: show this usages\n");
    exit(1);
}

static size_t bytes(char *s)
{
    size_t n;
    if (sscanf(s, "%zu", &n) < 1)
        return 0;
    if ((s[strlen(s) - 1] == 'k') || (s[strlen(s) - 1] == 'K'))
        n *= 1024;
    if ((s[strlen(s) - 1] == 'm') || (s[strlen(s) - 1] == 'M'))
        n *= (1024 * 1024);
    return n;
}

int main(int argc, FAR char *argv[])
{
    int o;
    int count = 1000;
    char filepath[50];
    memset(filepath, 0, 50);
    size_t block = 1024;
    int mode = 0;
    pid_t pid = getpid();
    struct sched_param param;
    sched_getparam(pid, &param);
    param.sched_priority = 255;
    sched_setparam(pid, &param);
    while ((o = getopt(argc, argv, "d:b:c:m:hf:")) != EOF)
    {
        switch (o)
        {
        case 'd':
            snprintf(filepath, 50, "%s/%s", optarg, "performance_test");
            break;
        case 'b':
            block = bytes(optarg);
            break;
        case 'c':
            count = atoi(optarg);
            break;
        case 'm':
            mode = atoi(optarg);
            break;
        case 'f':
            snprintf(filepath, 50, "%s", optarg);
            break;
        default:
            show_usages();
            break;
        }
    }
    if (strlen(filepath) == 0)
    {
        syslog(LOG_ERR, "please set dir \n");
        return -1;
    }

    if (mode == 0)
    {
        auto_test(filepath);
    }
    else if (mode == 1)
    {
        double speed = write_speed(filepath, block, count);
        syslog(LOG_INFO, "write speed is %lf\n", speed);
    }
    else if (mode == 2)
    {
        double speed = read_speed(filepath, block, count);
        syslog(LOG_INFO, "read speed is %lf\n", speed);
    }
    else if (mode == 3)
    {
        double speed = rand_write_speed(filepath, block, count);
        syslog(LOG_INFO, "random write speed is %lf\n", speed);
    }
    else if (mode == 4)
    {
        double speed = rand_read_speed(filepath, block, count);
        syslog(LOG_INFO, "random read speed is %lf\n", speed);
    }
    else
    {
        syslog(LOG_ERR, "mode error\n");
    }
    // remove(filepath);
    return 0;
}