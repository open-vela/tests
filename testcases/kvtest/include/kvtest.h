#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include "kvdb.h"

#define DEFAULT_STACKSIZE CONFIG_DEFAULT_TASK_STACKSIZE

#define KEY_MAX_LEN PROP_NAME_MAX - 1
#define VALUE_MAX_LEN PROP_VALUE_MAX - 1

/*
    Generate a string of randomly specified length
*/
static char *genRandomString(int length)
{
    int flag, i;
    char *string;
    if ((string = (char *)malloc(length + 1)) == NULL)
    {

        return NULL;
    }

    for (i = 0; i < length; i++)
    {
        flag = rand() % 5;
        switch (flag)
        {
        case 0:
            string[i] = 'A' + rand() % 26;
            break;
        case 1:
            string[i] = 'a' + rand() % 26;
            break;
        case 2:
            string[i] = '0' + rand() % 10;
            break;
        case 3:
            string[i] = '!' + rand() % 3;
            break;
        /* skip $ */
        case 4:
            string[i] = '%' + rand() % 11;
            break;
        default:
            string[i] = 'x';
            break;
        }
    }
    string[length] = '\0';
    return string;
}

static void delete_all(const char *name, const char *value, void *cookie)
{
    property_delete(name);
}

static void clean_up(void)
{
    property_list(delete_all, NULL);
}