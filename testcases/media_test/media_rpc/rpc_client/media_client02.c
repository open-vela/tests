/****************************************************************************
 * tests/testcases/media_test/media_rpc/rpc_client/media_client02.c
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
    1. This test case covers most of the interfaces(parcel,client,server).
    2. The first test case tested serialized data,  using the media_proxy_send_with_ack() interface to
     send data to the server, and then receive messages from the server.
    3. The second test case, after testing serialized data, sends the data to the server
     using the media_proxy_send() interface, which does not let the server send messages to the client.

**************************************************************************/
int main(int argc, FAR char *argv[])
{
    if(argc < 2){
        syslog(LOG_ERR, "please input service core\n");
        return -1;
    }
    const char *cpu = NULL;
    cpu = argv[1];
    struct media_rpc_test_case {
        uint32_t in_data;
        uint32_t out_data;
        const char  *str;
        void *client_handle;
    } testcases[] = {
        {13, 0, "ack", NULL},
        {32, 0, "satrt_notify", NULL}
    };
    media_parcel in, out;

    int len = sizeof(testcases) / sizeof(struct media_rpc_test_case);
    for (int i = 0; i < len; i++) {
        media_parcel_init(&in);
        if (media_parcel_append_uint32(&in, testcases[i].in_data) != 0) {
            syslog(LOG_ERR, "FAIL! testcase %d : media parcel append uint32 fail.\n",i + 1);
            media_parcel_deinit(&in);
            return 0;
        }
        if (media_parcel_append_string(&in, testcases[i].str) != 0) {
            syslog(LOG_ERR,"FAIL! testcase %d : media parcel append string fail.\n",i + 1);
            media_parcel_deinit(&in);
            return 0;
        }
        media_parcel_init(&out);
        testcases[i].client_handle = media_proxy_connect(cpu);
        if (!testcases[i].client_handle) {
            syslog(LOG_ERR,"FAIL! testcase %d :media client connect server fail.\n",i + 1);
            media_parcel_deinit(&in);
            media_parcel_deinit(&out);
            return 0;
        }
        if (i == 0) {
            syslog(LOG_INFO,"[media_client] media_proxy_set_event_cb\n");
            media_proxy_set_event_cb(testcases[i].client_handle, cpu, on_listener_test, "lister_user");
            usleep(1000000);
            syslog(LOG_INFO,"[media_client] media_proxy_send_with_ack:13,ack\n");
            if (media_proxy_send_with_ack(testcases[i].client_handle, &in, &out) != 0) {
                syslog(LOG_ERR,"FAIL! media client send data with ack fail.\n");
                media_parcel_deinit(&in);
                media_parcel_deinit(&out);
                media_proxy_disconnect(testcases[i].client_handle);
                return 0;
            }

            if (media_parcel_read_uint32(&out, &testcases[i].out_data) != 0 || testcases[i].out_data != 23) {
                syslog(LOG_ERR,"FAIL! testcase %d : media read data fail or not equal.\n", i);
                media_parcel_deinit(&in);
                media_parcel_deinit(&out);
                media_proxy_disconnect(testcases[i].client_handle);
                return 0;
            }
            testcases[i].str = media_parcel_read_string(&out);
            syslog(LOG_INFO,"[media_client] ack:%d,%s\n",(int) testcases[i].out_data, testcases[i].str);
        } else {
            syslog(LOG_INFO,"[media_client] media_proxy_send:32,satrt_notify\n");
            media_proxy_send(testcases[i].client_handle, &in);
        }

        usleep(1000000);
        syslog(LOG_INFO,"testcase %d : [media_client] media_proxy_disconnect\n",i + 1);
        if (media_proxy_disconnect(testcases[i].client_handle) == 0)
            testcases[i].client_handle = NULL;
        media_parcel_deinit(&in);
        media_parcel_deinit(&out);
    }
    syslog(LOG_INFO,"PASS! media_client02 pass.\n");
    return 0;
}
