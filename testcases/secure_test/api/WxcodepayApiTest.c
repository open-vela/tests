#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include "../../../../vendor/xiaomi/mitee_iot/ca/wxcodepay/wxcodepay_ca_api.h"

/**************************************************************************
Test Suite Name: WxcodepayApiTest

Author: caofuqi

Date: 2021-12-21

Description:
    1. test wxcodepay api

Jira Record:

**************************************************************************/

int main(int argc, char *argv[])
{
    static uint8_t buffer[512];
    static uint32_t len;
    static struct t_case
    {
        char *command;
        int item;
        char *term;
    } tcases[] = {
        {"write", 0, "12345678"},
        {"read", 0, NULL},
        {"check", 0, NULL},
        {"delete", 0, NULL},
        {"write", 1, "sdjfkjsd90&"},
        {"read", 1, NULL},
        {"check", 1, NULL},
        {"delete", 1, NULL},
        {"write", 2, "tome|sd\fyh"},
        {"read", 2, NULL},
        {"check", 2, NULL},
        {"delete", 2, NULL},
        {"write", 3, "@host134"},
        {"read", 3, NULL},
        {"check", 3, NULL},
        {"delete", 3, NULL},
        {"write", 4, "fshdfik 90l+"},
        {"read", 4, NULL},
        {"check", 4, NULL},
        {"delete", 4, NULL},
        {"write", 5, "~sfdhk/sjdf"},
        {"read", 5, NULL},
        {"check", 5, NULL},
        {"delete", 5, NULL},
        {"write", 6, "&**&&&%|$"},
        {"read", 6, NULL},
        {"check", 6, NULL},
        {"delete", 6, NULL},
        {"write", 7, "sjfksdfjkl9++++sdjfksjddsfsdg"},
        {"read", 7, NULL},
        {"check", 7, NULL},
        {"delete", 7, NULL},
        {"write", 8, " "},
        {"read", 8, NULL},
        {"check", 8, NULL},
        {"delete", 8, NULL},
        {"write", 9, "jsfadkjf &&&$"},
        {"read", 9, NULL},
        {"check", 9, NULL},
        {"delete", 9, NULL},
    };

    static char *res[] = {"12345678", "sdjfkjsd90&", "tome|sd\fyh", "@host134", "fshdfik 90l+", "~sfdhk/sjdf", "&**&&&%|$", "sjfksdfjkl9++++sdjfksjddsfsdg", " ", "jsfadkjf &&&$"};
    int CASE_TOTAL = sizeof(tcases) / sizeof(tcases[0]);
    int i = 0;
    int diff;

    for (int testno = 0; testno < CASE_TOTAL; ++testno)
    {
        if (strcmp(tcases[testno].command, "check") == 0)
        {
            if (!is_wxcodepay_tee_data_exited(tcases[testno].item))
            {
                syslog(LOG_ERR, "is_wxcodepay_tee_data_exited failed\n");
                return -1;
            }
        }

        if (strcmp(tcases[testno].command, "delete") == 0)
        {
            if (wxcodepay_tee_data_delete(tcases[testno].item) != 0)
            {
                syslog(LOG_ERR, "wxcodepay delete failed\n");
                return -1;
            }
        }

        if (strcmp(tcases[testno].command, "read") == 0)
        {
            len = 512;
            memset(buffer, 0, 512);

            if (wxcodepay_tee_data_read(tcases[testno].item, buffer, &len) != 0)
            {
                syslog(LOG_ERR, "wxcodepay read failed\n");
                return -1;
            }
            if (len != strlen(res[i]))
            {
                syslog(LOG_ERR, "read len error\n");
                return -1;
            }
            diff = strcmp((char *)buffer, res[i]);
            i++;
            if (diff != 0)
            {
                syslog(LOG_ERR, "read diff error\n");
                return -1;
            }
        }

        if (strcmp(tcases[testno].command, "write") == 0)
        {
            if (wxcodepay_tee_data_write(tcases[testno].item, (uint8_t *)tcases[testno].term, strlen(tcases[testno].term)) != 0)
            {
                syslog(LOG_ERR, "wxcodepay write failed\n");
                return -1;
            }
        }
    }
    syslog(LOG_INFO, "TEST PASSED !\n");
    return 0;
}