/****************************************************************************
 * tests/testcases/media_test/media_rpc/rpc_server/media_server02.c
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
#include <poll.h>
#include <pthread.h>
#include <errno.h>
#include "media_server.h"
#include "media_proxy.h"
#include "media_parcel.h"
#include "media_rpc_test.h"

static void *server_handle_test2 = NULL;
static int server_test_status2 = 0;
static int thread_status2 = 1;
static int lock = 1;

static int server_test_transaction2(void *cookie, media_parcel *in, media_parcel *out)
{
    char *test;
    uint32_t data, in_data = 1;
    int ret;
    media_parcel in1;
    test = cookie;
    ret = media_parcel_read_uint32(in, &data);
    media_parcel_init(&in1);
    media_parcel_append_uint32(&in1, in_data);

    if (data == 1)
        ret = media_server_notify(NULL, test, &in1);
    else if (data == 2)
        ret = media_server_notify(server_handle_test2, NULL, &in1);

    if (ret != -EINVAL) {
        syslog(LOG_ERR, "FAIL!\n");
        server_test_status2 = 1;
    }
    media_parcel_deinit(&in1);
    if (data == 2)
        thread_status2 = 0;

    return 0;
}

static void *mediarpc_server_start2(void *args)
{
    struct pollfd fds[16];
    int *conns[16];
    int count;

    server_handle_test2 = media_server_create(server_test_transaction2);
    lock = 0;
    if (server_handle_test2 == NULL)
        return NULL;

    while (thread_status2) {
        count = media_server_get_pollfds(server_handle_test2, fds, (void **)conns, 16);
        poll(fds, count, -1);
        for (int i = 0; i < count; i++)
            if (fds[i].revents)
                media_server_poll_available(server_handle_test2, &fds[i], conns[i]);
    }
    media_server_destroy(server_handle_test2);
    return NULL;
}

/**************************************************************************
Test Suite Name: MediaTest

Author: caixucheng

Date: 2021-11-11

Description:
    1. This test case is test media_server_notify() interface parameters.
    2. The frist case, handle is null.
    3. The Second case, conn is null.

**************************************************************************/
int main(int argc, FAR char *argv[])
{
    if(argc < 2){
        syslog(LOG_ERR, "please input service core\n");
        return -1;
    }
    const char *cpu = NULL;
    cpu = argv[1];
    pthread_t server_thread;
    void *client_handle = NULL;
    media_parcel in, out;

    pthread_create(&server_thread, NULL, mediarpc_server_start2, NULL);
    while (lock)
        usleep(10);
    client_handle = media_proxy_connect(cpu);
    if (!client_handle) {
        syslog(LOG_ERR, "FAIL! media client connect server fail.\n");
        thread_status2 = 0;
        return 0;
    }

    if (media_proxy_set_event_cb(client_handle, cpu, on_listener_test, "lister_user") != 0) {
        syslog(LOG_ERR, "FAIL! media client set event fail.\n");
        thread_status2 = 0;
        if (media_proxy_disconnect(client_handle) == 0)
            client_handle = NULL;
    }

    for (int i = 0; i < 2; i++) {
        if (i == 0) {
            media_parcel_init(&in);
            media_parcel_init(&out);
        } else {
            media_parcel_reinit(&in);
            media_parcel_reinit(&out);
        }
        if (media_parcel_append_int32(&in, i + 1) != 0) {
            syslog(LOG_ERR, "FAIL! media parcel append int32 data fail.\n");
            media_parcel_deinit(&in);
        }
        if (media_proxy_send_with_ack(client_handle, &in, &out) != 0)
            syslog(LOG_ERR, "FAIL! media client send data fail.\n");
    }

    if (client_handle != NULL && media_proxy_disconnect(client_handle) == 0)
	    client_handle = NULL;

    pthread_join(server_thread, NULL);
    if (!server_test_status2)
        syslog(LOG_ERR, "PASS! media_server02 pass.\n");
    return 0;
}