#include <nuttx/config.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define FILEPATH "/data/flash_rw_test"
#define LARGEFILE "/data/flash_rw_large_test"

/****************************************************************************
 * Name: flash_rw_stability
 * Example description: Test long flash reads and writes
 ****************************************************************************/

static size_t write_file(char *file_path, char *buf, size_t len)
{
    FILE *file;
    int ret;
    size_t size = 0;
    file = fopen(file_path, "a+");
    if (file == NULL)
    {
        printf("[write_file] : open file fail !\n");
        return -1;
    }
    fseek(file, 0, SEEK_END);
    ret = fwrite(buf, len, 1, file);
    if (ret < 0)
    {
        printf("[write_file] : write file fail .\n");
        return -1;
    }
    else
    {
        printf("[write_file] : write file success ! write %d bytes .\n", len);
        fsync(fileno(file));
    }
    size = ftell(file);
    fclose(file);
    return size;
}

static int write_large_file(char *file)
{
    int fd;
    int ret;
    char *buf;

    buf = malloc(64 * sizeof(char));
    if (buf == NULL)
    {
        printf("[write_large_file] : malloc fail !\n");
        return -1;
    }
    else
    {
        memset(buf, 'A', 64);
    }

    fd = open(file, O_CREAT | O_RDWR);
    if (fd < 0)
    {
        printf("[write_large_file] : open file fail !\n");
        return -1;
    }
    printf("[write_large_file] : Start writing files, total 4M\n");
    for (int i = 0; i < 1024 * 64; i++)
    {
        ret = write(fd, buf, 64);
        if (ret < 0)
        {
            printf("[write_large_file] : write file fail !\n");
        }
    }
    printf("[write_large_file] : write file success , file size = 4M !\n");
    close(fd);
    free(buf);
    return 0;
}

int main(int argc, FAR char *argv[])
{
    int fd;
    int ret;
    clock_t start, finish;
    printf("start test ...\n");
    /* creat test file  */
    fd = creat(FILEPATH, 0700);
    if (fd == -1)
    {
        printf("creat(%s) failed", FILEPATH);
        goto FAIL;
    }
    close(fd);

    for (int i = 1; i < 10000; i++)
    {
        printf("[%s] : Appends content to a file, write flash !\n", argv[0]);
        start = clock();
        ret = write_file(FILEPATH, "asdfhasjkdfhajkshdf", 20);
        finish = clock();
        if (ret == -1)
        {
            printf("[%s] : write file fail ! ! file=%s\n", argv[0], FILEPATH);
            goto FAIL;
        }
        else
        {
            printf("[%s] : write file success ! ! size=20 byte, takes %f s\n", argv[0], (double)(finish - start) / CLOCKS_PER_SEC);
            printf("[%s] : Current file sizes = %d bytes\n", argv[0], ret);
            if (ret >= 1024 * 128)
            {
                printf("[%s] : File size over 512 KB, remove\n", argv[0]);
                remove(FILEPATH);
                printf("[%s] : Recreate file !\n", argv[0]);
                fd = creat(FILEPATH, 0700);
                if (fd == -1)
                {
                    printf("[%s] : creat(%s) failed", argv[0], FILEPATH);
                    goto FAIL;
                }
                close(fd);
            }
        }
        printf("[%s] : wait 2 seconds !\n", argv[0]);
        sleep(2);
        if (i % 20 == 0)
        {
            printf("[%s] : wirte large file !\n", argv[0]);
            start = clock();
            ret = write_large_file(LARGEFILE);
            finish = clock();
            if (ret == -1)
            {
                printf("[%s] : write large file fail ! ! file=%s\n", argv[0], LARGEFILE);
                goto FAIL;
            }
            else
            {
                printf("[%s] : write large file success ! ! size=4 M, takes %f s\n", argv[0], (double)(finish - start) / CLOCKS_PER_SEC);
            }
            sleep(1);
            printf("[%s] : delete large file !\n", argv[0]);
            remove(LARGEFILE);
        }
        printf("[%s] : wait 1 second !\n", argv[0]);
        sleep(1);
    }
    printf("TEST PASSED !\n");
    exit(EXIT_SUCCESS);
FAIL:
    printf("TEST FAILED !\n");
    exit(EXIT_FAILURE);
}