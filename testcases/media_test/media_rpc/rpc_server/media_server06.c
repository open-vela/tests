/****************************************************************************
 * tests/testcases/media_test/media_rpc/rpc_server/media_server06.c
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
#include <errno.h>
#include <pthread.h>
#include "media_server.h"
#include "media_proxy.h"
#include "media_rpc_test.h"

static void *server_test6(void *server_handle)
{
    struct pollfd fds[16];
    int *conns[16];
    int count = 0, index = 0, ret, f = 1;
    while (1) {
        count = media_server_get_pollfds(server_handle, fds, (void **)conns, 16);
        if (count < 0) {
            syslog(LOG_ERR, "FAIL! media server get pollfds fail.\n");
            break;
        }
        /* Test accept() and recv() over, exit server */
        if (index == 2) {
            if (f)
                syslog(LOG_ERR, "PASS! media_server06 pass.\n");
            break;
        }

        index++;
        poll(fds, count, -1);
        for (int i = 0; i < count; i++) {
            if (fds[i].revents) {
                if (conns[i] == NULL) {
                    /* conns is null, test accept() */
                    ret = media_server_poll_available(server_handle, &fds[i], conns[i]);
                    syslog(LOG_INFO, "conns is null, test accept(), ret = %d\n", ret);
                    if (ret != 0) {
                        f = 0;
                        syslog(LOG_ERR, "FAIL! media server test accept() fail.\n");
                        return NULL;
                    }
                } else {
                    /* conns is not null, fd is null, test recv() */
                    ret = media_server_poll_available(server_handle, NULL, conns[i]);
                    syslog(LOG_INFO, "conns is not null, fd is null, test recv() %d\n", ret);
                    if (ret != -EINVAL) {
                        f = 0;
                        syslog(LOG_ERR, "FAIL! test recv(), fd is null, should fail.\n");
                        return NULL;
                    }
                }
            }
        }
    }
    return NULL;
}

/**************************************************************************
Test Suite Name: MediaTest

Author: caixucheng

Date: 2021-11-13

Description:
    1. Test the fd parameter of function media_server_poll_available().
    2. fd is null.

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
    media_parcel in;
    int32_t data = 2;
    void *client_handle = NULL, *server_handle = NULL;

    media_parcel_init(&in);
    if (media_parcel_append_int32(&in, data) != 0) {
        syslog(LOG_ERR, "FAIL! media parcel append int32 fail.\n");
        media_parcel_deinit(&in);
        return 0;
    }
    server_handle = media_server_create(server_test_common_transaction);
    if (!server_handle) {
        syslog(LOG_ERR, "FAIL! media create server fail.\n");
        return 0;
    }

    pthread_create(&server_thread, NULL, server_test6, server_handle);
    client_handle = media_proxy_connect(cpu);
    if (media_proxy_send(client_handle, &in) != 0) {
        syslog(LOG_ERR, "FAIL! media client send data fail.\n");
        media_proxy_disconnect(client_handle);
        client_handle = NULL;
        return 0;
    }

    pthread_join(server_thread, NULL);
    if (client_handle)
        media_proxy_disconnect(client_handle);
    if (server_handle)
        media_server_destroy(server_handle);
    return 0;
}