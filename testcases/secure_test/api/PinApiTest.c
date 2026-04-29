#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include "../../../../vendor/xiaomi/mitee_iot/ca/pin/pin_ca_api.h"

int main(int argc, char *argv[])
{
    struct t_case
    {
        char *command;
        int del;
        char *old;
        char *new;
    } tcases[] = {
        {"store", 1, "helloworld", NULL},
        {"verify", 1, "helloworld", NULL},
        {"change", 1, "helloworld", "12345678"},
        {"hash", 1, NULL, NULL},
        {"check", 1, NULL, NULL},
        {"delete", 1, NULL, NULL},
        {"store", 0, "helloworld&$", NULL},
        {"verify", 0, "helloworld&$", NULL},
        {"change", 0, "helloworld&$", "12345678"},
        {"hash", 0, NULL, NULL},
        {"check", 0, NULL, NULL},
        {"delete", 0, NULL, NULL},
        {"store", 0, " ", NULL},
        {"verify", 0, " ", NULL},
        {"change", 0, " ", "12345678"},
        {"hash", 0, NULL, NULL},
        {"check", 0, NULL, NULL},
        {"delete", 0, NULL, NULL},
        {"store", 1, "0", NULL},
        {"verify", 1, "0", NULL},
        {"change", 1, "0", "12345678"},
        {"hash", 1, NULL, NULL},
        {"check", 1, NULL, NULL},
        {"delete", 1, NULL, NULL},
        {"store", 0, "+12-899", NULL},
        {"verify", 0, "+12-899", NULL},
        {"change", 0, "+12-899", "12345678"},
        {"hash", 0, NULL, NULL},
        {"check", 0, NULL, NULL},
        {"delete", 0, NULL, NULL},
        {"store", 1, "12345678", NULL},
        {"verify", 1, "12345678", NULL},
        {"change", 1, "12345678", "12345678"},
        {"hash", 1, NULL, NULL},
        {"check", 1, NULL, NULL},
        {"delete", 1, NULL, NULL},
    };

    uint32_t __attribute__((unused)) res;
    bool is_deletable;
    int CASE_TOTAL = sizeof(tcases) / sizeof(tcases[0]);

    for (int testno = 0; testno < CASE_TOTAL; ++testno)
    {
        if (tcases[testno].del == 0)
        {
            is_deletable = false;
        }
        else
        {
            is_deletable = true;
        }

        if (strcmp(tcases[testno].command, "store") == 0)
        {
            char *buff = tcases[testno].old;
            if (pin_store(is_deletable, (uint8_t *)buff, strlen(buff)) != 0)
            {
                syslog(LOG_ERR, "pin store failed\n");
                return -1;
            }
        }

        if (strcmp(tcases[testno].command, "verify") == 0)
        {
            char *buff = tcases[testno].old;
            if (pin_verify(is_deletable, (uint8_t *)buff, strlen(buff)) != 0)
            {
                syslog(LOG_ERR, "pin verify failed\n");
                return -1;
            }
        }

        if (strcmp(tcases[testno].command, "change") == 0)
        {
            char *old = tcases[testno].old;
            char *new = tcases[testno].new;
            if (pin_change(is_deletable, (uint8_t *)old, strlen(old),
                           (uint8_t *)new, strlen(new)) != 0)
            {
                syslog(LOG_ERR, "pin change failed\n");
                return -1;
            }
        }

        if (strcmp(tcases[testno].command, "hash") == 0)
        {
            uint8_t sha256[32];
            if (pin_getsha256(is_deletable, sha256, 32) != 0)
            {
                syslog(LOG_ERR, "pin getsha256 failed\n");
                return -1;
            }
        }

        if (strcmp(tcases[testno].command, "check") == 0)
        {
            if (pin_is_exist(is_deletable) == false)
            {
                syslog(LOG_ERR, "pin is exist failed\n");
                return -1;
            }
        }

        if (strcmp(tcases[testno].command, "delete") == 0)
        {
            if (pin_delete(is_deletable) != 0)
            {
                syslog(LOG_ERR, "pin delete failed\n");
            }
        }
    }

    syslog(LOG_INFO, "TEST PASSED !\n");

    return 0;
}