/****************************************************************************
 * tests/testcases/media_test/media_rpc/rpc_client/media_client05.c
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

Date: 2021-11-09

Description:
    1. This test case is test media_proxy_set_event_cb() function.
    2. In the first case, the handle parameter is null.
    3. In the second case, the even_cb parameter is null.

**************************************************************************/
int main(int argc, FAR char *argv[])
{
    if(argc < 2){
        syslog(LOG_ERR, "please input service core\n");
        return -1;
    }
    const char *cpu = NULL;
    cpu = argv[1];
    void *client_handle = NULL;
    if (media_proxy_set_event_cb(NULL, cpu, on_listener_test, "lister_user") == 0) {
        syslog(LOG_ERR, "FAIL! media client set event, handle is NULL, should fail\n");
        return 0;
    }

    client_handle = media_proxy_connect(cpu);
    if (!client_handle) {
        syslog(LOG_ERR, "FAIL! media client connect server fail.\n");
        return 0;
    }
    if (media_proxy_set_event_cb(client_handle, cpu, NULL, "lister_user") == 0) {
        syslog(LOG_ERR, "FAIL! media client set event, event is NULL, should fail\n");
        return 0;
    }

    /* Free memory*/
    if (media_proxy_disconnect(client_handle) == 0)
        client_handle = NULL;

    syslog(LOG_INFO, "PASS! media_rpc_client05 pass.\n");
    return 0;
}