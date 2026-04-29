/****************************************************************************
 * tests/testcases/media_test/media_rpc/rpc_client/media_client06.c
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
#include <pthread.h>
#include <stdlib.h>
#include "media_parcel.h"
#include "media_proxy.h"
#include "media_rpc_test.h"

/**************************************************************************
Test Suite Name: MediaTest

Author: caixucheng

Date: 2021-11-13

Description:
    1. Create MAX_SERVER_CONNECT_NUM threads, each thread to create a client.
    2. Each client connects to the server and sends messages.
    3. One half sends messages through interface media_proxy_send_with_ack()
        and the other sends messages through interface media_proxy_send().

**************************************************************************/
int main(int argc, FAR char *argv[])
{
    if(argc < 3){
        syslog(LOG_ERR, "please input service core and index\n");
        return -1;
    }
    const char *cpu = NULL;
    cpu = argv[1];
    int index, looptime = 10000;
    void *client_handle;
    media_parcel in, out;
    int32_t data_with_ack = 1, data = 2, data_ret;
    char *str = "ack", *str1 = "ss";

    index = atoi(argv[2]);

    /* 0 <= index < MAX_SERVER_CONNECT_NUM */
    if (index < 0 || index >= MAX_SERVER_CONNECT_NUM)
        return 0;
    media_parcel_init(&in);
    media_parcel_init(&out);
    client_handle = media_proxy_connect(cpu);
    if (!client_handle) {
        syslog(LOG_ERR, "FAIL! media client connect server fail.\n");
        return 0;
    }

    if (index % 2 == 0) {
        /* Send data with ack */
        media_proxy_set_event_cb(client_handle, cpu, on_listener_test, "lister_user");
        usleep(100000);
        while(looptime --) {
            media_parcel_append_int32(&in, data_with_ack);
            media_parcel_append_string(&in, str);
            usleep(1000);
            if (media_proxy_send_with_ack(client_handle, &in, &out)!= 0) {
                syslog(LOG_ERR, "FAIL! media_client06 fail.\n");
                return 0;
            }
            media_parcel_read_int32(&out, &data_ret);
            if (data_ret != 23) {
                syslog(LOG_ERR, "FAIL! ack ret data is not right.\n");
                return 0;
            }
            media_parcel_reinit(&in);
            media_parcel_reinit(&out);
        }
    }else {
        /* Send data not ack */
        while(looptime --) {
            media_parcel_append_int32(&in, data);
            media_parcel_append_string(&in, str1);
            usleep(1000);
            if (media_proxy_send(client_handle, &in)!= 0) {
                syslog(LOG_ERR, "FAIL! media_client06 fail.\n");
                return 0;
            }
            media_parcel_reinit(&in);
        }
    }
    media_parcel_deinit(&in);
    media_parcel_deinit(&out);
    media_proxy_disconnect(client_handle);

    syslog(LOG_INFO, "PASS! media_client06 pass.\n");
    return 0;
}