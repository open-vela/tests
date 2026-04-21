/****************************************************************************
 * tests/testcases/media_test/media_rpc/rpc_client/media_client01.c
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
    1. This test case covers most of the interfaces(parcel, client, server).
    2. The serialization interface is used to serialize the data,
        then the client connects to the server,
        and then the media_proxy_send() interface is used to send the data.

**************************************************************************/
int main(int argc, FAR char *argv[])
{
    if(argc < 2){
        syslog(LOG_ERR, "please input service core\n");
        return -1;
    }
    const char *cpu = NULL;
    cpu = argv[1];
    media_parcel in;
    uint32_t in_data = 17;
    char *in_str = "play";
    void *client_handle = NULL;

    media_parcel_init(&in);
    if (media_parcel_append_uint32(&in, in_data) != 0) {
        syslog(LOG_ERR, "FAIL!media parcel append %" PRIu32" fail \n", in_data);
        goto error;
    }

    if (media_parcel_append_string(&in, in_str) != 0) {
        syslog(LOG_ERR, "FAIL!media parcel append %s fail \n", in_str);
        goto error;
    }
    client_handle = media_proxy_connect(cpu);
    if (!client_handle) {
        syslog(LOG_ERR, "FAIL!media client connect server fail. \n");
        goto error;
    }

    syslog(LOG_INFO, "[media_client] media_proxy_send:17,play\n");
    media_proxy_send(client_handle, &in);

    usleep(1000000);
    if (media_proxy_disconnect(client_handle) != 0) {
        syslog(LOG_ERR, "FAIL!media client disconnect fail \n");
        goto error;
    }
    client_handle = NULL;
    syslog(LOG_INFO, "PASS!media_client01 pass \n");

error:
    media_parcel_deinit(&in);
    return 0;
}
