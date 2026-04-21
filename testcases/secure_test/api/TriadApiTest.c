#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <unistd.h>
#include "../../../../vendor/xiaomi/mitee_iot/ca/triad/triad_ca_api.h"

/**************************************************************************
Test Suite Name: TriadApiTest

Author: caofuqi

Date: 2021-12-22

Description:
    1. test triad api

Jira Record:

**************************************************************************/
int main(int argc, char *argv[])
{
    static const uint8_t test_key[16] = {
        0x46, 0x78, 0x5A, 0x43, 0x30, 0x76, 0x55, 0x78,
        0x63, 0x56, 0x7A, 0x61, 0x76, 0x4C, 0x74, 0x51};

    static const uint8_t txt[80] = {
        0x37, 0x29, 0xE6, 0x08, 0x9D, 0x3A, 0x36, 0x50,
        0x2A, 0x57, 0x9B, 0x56, 0x2D, 0xAB, 0xA0, 0x61,
        0x6B, 0xDC, 0x30, 0xC4, 0x4C, 0xDA, 0xC3, 0x94,
        0x3B, 0xD6, 0xF2, 0xCB, 0x7C, 0x57, 0x99, 0x88,
        0x61, 0x5B, 0xCC, 0x10, 0x44, 0xFC, 0x2C, 0x4A,
        0xC5, 0x6D, 0xA9, 0xF0, 0x16, 0x6F, 0xCF, 0xA2,
        0x9D, 0x04, 0x34, 0xA4, 0x44, 0xF0, 0xC4, 0xC5,
        0xF1, 0xE9, 0xDC, 0x40, 0xA2, 0xA5, 0x01, 0xF1,
        0xA8, 0x34, 0xAF, 0xA4, 0xE4, 0x59, 0x6B, 0xD3,
        0x1A, 0xF3, 0x3C, 0xBD, 0xF8, 0x0F, 0x74, 0x0C};

    static uint8_t hmac[32];
    static uint8_t key[16];
    static uint8_t did[8];

    if (triad_load_did(did, 8) == 0)
    {
        syslog(LOG_INFO, "load did ok, %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\n",
               did[0], did[1], did[2], did[3], did[4], did[5], did[6], did[7]);
    }
    else
    {
        memset(did, 0x12, 8);
        if (triad_store_did(did, 8) != 0)
        {
            syslog(LOG_ERR, "store did failed\n");
            return -1;
        }
    }

    if (triad_load_key(key, 16) == 0)
    {
        syslog(LOG_INFO, "load key successed\n");
    }
    else
    {
        memcpy(key, test_key, 16);
        if (triad_store_key(key, 16) != 0)
        {
            syslog(LOG_ERR, "store key failed\n");
            return -1;
        }
    }

    if (triad_get_hmac((uint8_t *)txt, 80, hmac, 32) == 0)
    {

        for (int i = 0; i < 32; i++)
        {
            syslog(LOG_INFO, "%02x,", hmac[i]);
        }
        syslog(LOG_INFO, "\n");
    }
    sleep(1);
    syslog(LOG_INFO, "TEST PASSED !\n");
    return 0;
}
