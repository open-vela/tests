#include <nuttx/config.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

/* Number of data */
#define DATA_LEN 200
#define FILENAME "demo.txt"

static FILE *fp;

static struct testData
{
    int number;   //Data number
    char str[16]; // Data content
} data1[DATA_LEN], data;

static void setup(void)
{
    int ret;

    /* setup：Generate test data */
    for (int k = 0; k < DATA_LEN; k++)
    {
        data1[k].number = k;
        sprintf(data1[k].str, "ABCDEFGHIJ-%4d", k);
    }

    /* Create a file to store test data */
    if ((fp = fopen(FILENAME, "wb+")) == NULL)
    {
        syslog(LOG_ERR, "ERROR: Cannot open file! errno=%d\n", errno);
        exit(1);
    }

    for (int i = 0; i < DATA_LEN; i++)
    {
        /* Store test data to file */
        ret = fwrite(data1, sizeof(struct testData), DATA_LEN, fp);
        if (ret != DATA_LEN)
        {
            syslog(LOG_ERR, "ERROR: Cannot write file! errno=%d", errno);
            exit(1);
        }
        ret = fsync(fileno(fp));
        if (ret < 0)
        {
            syslog(LOG_ERR, "ERROR: Cannot fsync file! errno=%d", errno);
            exit(1);
        }
    }
}

static int randomReadTest(int number)
{
    size_t ret;
    if (number < 1)
    {
        syslog(LOG_ERR, "ERROR: Random read at least once ！\n");
        syslog(LOG_INFO, "skip randomReadTest \n");
        return -1;
    }

    /* Reset file pointer */
    rewind(fp);

    /* Random read */
    for (int l = 0; l < number; l++)
    {
        srand(l);
        ret = fseek(fp, sizeof(struct testData) * (rand() % DATA_LEN), SEEK_SET);
        if (ret < 0)
        {
            syslog(0, "ERROR: randomReadTest cannot fseek file! errno=%d", errno);
            return -1;
        }
        ret = fread(&data, sizeof(struct testData), 1, fp);
        if (ret == 0)
        {
            syslog(LOG_ERR, "ERROR: randomReadTest cannot fread file! errno=%d", errno);
            return -1;
        }
    }
    return 0;
}

static int randomWriteTest(int number)
{
    int ret;

    if (number < 1)
    {
        syslog(LOG_ERR, "ERROR: Random write at least once ！\n");
        syslog(LOG_INFO, "skip randomWriteTest \n");
        return -1;
    }

    /* Reset file pointer */
    rewind(fp);

    /* Mark random write */
    data.number = -1;
    sprintf(data.str, "XXXXXXXXXX-0000");

    /* Random write */
    for (int l = 0; l < number; l++)
    {
        srand(l);
        ret = fseek(fp, sizeof(struct testData) * (rand() % DATA_LEN), SEEK_SET);
        if (ret < 0)
        {
            syslog(LOG_ERR, "ERROR: randomWriteTest cannot fseek file! errno=%d", errno);
            return -1;
        }
        ret = fwrite(&data, sizeof(struct testData), 1, fp);
        if (ret != 1)
        {
            syslog(LOG_ERR, "ERROR: randomWriteTest cannot fwrite file! errno=%d", errno);
            return -1;
        }
    }
    return 0;
}

static void print_data(void)
{
    size_t ret;
    /* Reset file pointer */
    rewind(fp);

    /* print all data */
    for (int i = 0; i < DATA_LEN; i++)
    {
        ret = fread(&data, sizeof(struct testData), 1, fp);
        if (ret == 0)
        {
            return;
        }
        data.str[15] = 0;
        syslog(LOG_INFO, "%d : %s\n", data.number, data.str);
    }
    return;
}

static void cleanup(void)
{
    if (fp)
        fclose(fp);
    unlink(FILENAME);
    return;
}

int main(int argc, FAR char *argv[])
{
    int i, ret;
    clock_t start, finish;
    double duration;
    FAR char *path = NULL;
    int writecount = 10;
    int readcount = 10;

    for (i = 1; i < argc; i++)
    {
        if (strncmp(argv[i], "mountPath=", 10) == 0)
        {
            path = &argv[i][10];
        }
        else if (strncmp(argv[i], "writeCount=", 11) == 0)
        {
            writecount = atoi(&argv[i][11]);
        }
        else if (strncmp(argv[i], "readCount=", 10) == 0)
        {
            readcount = atoi(&argv[i][10]);
        }
    }

    if (path == NULL)
    {
        syslog(LOG_WARNING, "Usage:\n");
        syslog(LOG_WARNING, "cmd  writeCount=<Random write test count> readCount=<Random Read Test Count> mountPath=<File system mount directory>\n");
        exit(1);
    }

    chdir(path);
    syslog(LOG_INFO, "writecount=%d  readcount=%d  mountPath=%s\n", writecount, readcount, path);

    /* Prepare test data */
    setup();

    /* Get start time */
    start = clock();
    /* random read */
    ret = randomReadTest(readcount);
    /* Get finish time */
    finish = clock();
    if (ret == 0)
    {
        duration = (double)(finish - start) / CLOCKS_PER_SEC;
        syslog(LOG_INFO, "randomReadTest : read count=%d, takes %f seconds\n", readcount, duration);
    }

    /* Get start time */
    start = clock();
    /* random write */
    ret = randomWriteTest(writecount);
    /* Get finish time */
    finish = clock();
    if (ret == 0)
    {
        duration = (double)(finish - start) / CLOCKS_PER_SEC;
        syslog(LOG_INFO, "randomWriteTest : write count=%d, takes %f seconds\n", writecount, duration);
    }
    syslog(LOG_INFO, "The test is complete, the data list is as follows：\n");
    print_data();
    cleanup();
    return 0;
}