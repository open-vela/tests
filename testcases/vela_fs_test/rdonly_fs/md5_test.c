#include <nuttx/config.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <stdlib.h>
#include "md5.h"

#define INPUT_BUFFER_SIZE 1024

int calmd5(char *filename, char *md5String)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        syslog(LOG_ERR, "open %s fail\n", filename);
        return -1;
    }
    md5_state_t md5StateT;
    md5_init(&md5StateT);
    char buffer[INPUT_BUFFER_SIZE];
    while (!feof(file))
    {
        size_t numberOfObjects = fread(buffer, sizeof(char), INPUT_BUFFER_SIZE, file);
        md5_append(&md5StateT, (md5_byte_t *)buffer, numberOfObjects);
    }
    md5_byte_t digest[16];
    md5_finish(&md5StateT, digest);
    char hexBuffer[3];
    int n;
    for (size_t i = 0; i < 16; ++i)
    {
        if (digest[i] < 16)
            n = snprintf(hexBuffer, 3, "0%x", digest[i]);
        else
            n = snprintf(hexBuffer, 3, "%x", digest[i]);
        if (n <= 0)
        {
            syslog(LOG_ERR, "snprintf error\n");
            fclose(file);
            return -1;
        }
        strcat(md5String, hexBuffer);
    }
    fclose(file);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        syslog(LOG_WARNING, "Usage: %s [-f <filename>] [-c <counts>]\n"
                            "\t\t-f: set filename\n"
                            "\t\t-c: set number of program executions\n",
               argv[0]);
        return -EINVAL;
    }
    char md5String[33], md5String_p[33];
    memset(md5String_p, 0, 33);
    int o;
    int count = 1;
    char *filename = NULL;
    while ((o = getopt(argc, argv, "f:c:")) != EOF)
    {
        switch (o)
        {
        case 'f':
            filename = optarg;
            break;
        case 'c':
            count = atoi(optarg);
            break;
        default:
            syslog(LOG_WARNING, "Usage: %s [-f <filename>] [-c <counts>]\n"
                                "\t\t-f: set filename\n"
                                "\t\t-c: set number of program executions\n",
                   argv[0]);
            return -EINVAL;
            break;
        }
    }
    if (filename == NULL)
    {
        syslog(LOG_WARNING, "please set filename\n");
        return -EINVAL;
    }
    for (int i = 0; i < count; i++)
    {
        memset(md5String, 0, 33);
        int ret = calmd5(filename, md5String);
        if (ret != 0)
        {
            syslog(LOG_ERR, "cal md5 error\n");
            break;
        }
        syslog(LOG_INFO, "file %s md5 is %s\n", filename, md5String);
        if (strlen(md5String_p) == 0)
        {
            strcpy(md5String_p, md5String);
        }
        if (strcmp(md5String_p, md5String) != 0)
        {
            syslog(LOG_ERR, "the previous md5 %s is not equal the present md5 %s\n", md5String_p, md5String);
            break;
        }
        usleep(10000);
    }
    return 0;
}
