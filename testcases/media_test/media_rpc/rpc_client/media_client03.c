/****************************************************************************
 * tests/testcases/media_test/media_rpc/rpc_client/media_client03.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdio.h>
#include <nuttx/config.h>
#include <syslog.h>
#include "media_parcel.h"
#include "media_proxy.h"
#include "media_rpc_test.h"

/**************************************************************************
Test Suite Name: MediaTest

Author: caixucheng

Date: 2021-11-06

Description:
    1. In the first case, client connects to the server and then disconnects.

**************************************************************************/
int main(int argc, FAR char *argv[])
{
    if(argc < 2){
        syslog(LOG_ERR, "please input service core\n");
        return -1;
    }
    const char *cpu = NULL;
    cpu = argv[1];
    void *clients[MAX_SERVER_CONNECT_NUM + 2];

    /* init array */
    for (int i = 0; i < MAX_SERVER_CONNECT_NUM + 2; i++)
        clients[i] = NULL;

    /* The client connects to the server and then disconnects */
    for (int i = 0; i < MAX_SERVER_CONNECT_NUM + 2; i++) {
        clients[i] = media_proxy_connect(cpu);
        if (!clients[i]) {
            syslog(LOG_ERR, "FAIL ! client connect server fail.\n");
            goto err;
        }
        if (media_proxy_disconnect(clients[i]) != 0) {
            syslog(LOG_ERR, "FAIL ! client disconnect fail.\n");
            goto err;
        }
    }

    syslog(LOG_INFO, "PASS ! media_client03 test pass .\n");
    return 0;

err:
    /* Free menory */
    for (int i = 0; i < MAX_SERVER_CONNECT_NUM + 2; i++) {
        if (!clients[i])
            continue;
        if (media_proxy_disconnect(clients[i]) == 0)
            clients[i] = NULL;
    }
    return 0;
}
