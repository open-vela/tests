/****************************************************************************
 * tests/testcases/media_test/media_rpc/rpc_client/media_client07.c
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
#include "media_parcel.h"
#include "media_proxy.h"
#include "media_rpc_test.h"

volatile int f = 1;

/****************************************************************************
 * Function Name: client_thread_fun
 *
 * Description:
 *  A thread function, the thread is created to run this function,
 *  using interface media_proxy_send_with_ack() or interface media_proxy_send()
 *  to send messages, depending on the arg parameters.
 *
 * Input Parameters:
 *   arg      -  An integer variable used to determine whether to use interface
 *                 media_proxy_send_with_ack() or interface media_proxy_send().
 *
 ****************************************************************************/
static const char *cpu = NULL;

void *client_thread_fun(void *arg)
{
    int index = *(int *)arg, looptime = 10000, ret;
    void *client_handle;
    media_parcel in, out;
    int32_t data_with_ack = 1, data = 2, data_ret;
    char *str = "ack", *str1 = "ss";

    /* 0 <= index < MAX_SERVER_CONNECT_NUM */
    if (index < 0 || index >= MAX_SERVER_CONNECT_NUM)
        return NULL;
    media_parcel_init(&in);
    media_parcel_init(&out);
    client_handle = media_proxy_connect(cpu);
    if (!client_handle) {
        syslog(LOG_ERR, "FAIL! media client connect server fail.\n");
        return NULL;
    }

    if (index % 2 == 0) {
        /* Send data with ack */
        media_proxy_set_event_cb(client_handle, cpu, on_listener_test, "lister_user");
        usleep(100000);
        while(looptime--) {
            media_parcel_append_int32(&in, data_with_ack);
            media_parcel_append_string(&in, str);
            ret = media_proxy_send_with_ack(client_handle, &in, &out);

            usleep(1000);
            if (ret != 0) {
                f = 0;
                return NULL;
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
        while(looptime--) {
            media_parcel_append_int32(&in, data);
            media_parcel_append_string(&in, str1);
            ret = media_proxy_send(client_handle, &in);
            usleep(1000);
            if (ret != 0) {
                f = 0;
                return NULL;
            }
            media_parcel_reinit(&in);
        }
    }
    media_parcel_deinit(&in);
    media_parcel_deinit(&out);
    media_proxy_disconnect(client_handle);
    return NULL;
}

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
    if(argc < 2){
        syslog(LOG_ERR, "please input service core\n");
        return -1;
    }
    cpu = argv[1];
    pthread_t threads[MAX_SERVER_CONNECT_NUM];

    /* Create MAX_SERVER_CONNECT_NUM threads */
    for (int i = 0; i < MAX_SERVER_CONNECT_NUM; i++)
        pthread_create(&threads[i], NULL, client_thread_fun, &i);

    for (int i = 0; i < MAX_SERVER_CONNECT_NUM; i++)
        pthread_join(threads[i], NULL);

    if (f)
        syslog(LOG_INFO, "PASS ! media_client07 pass.\n");
    if (!f)
        syslog(LOG_ERR, "FAIL ! media_client07 fail.\n");
    return 0;
}