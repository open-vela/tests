/****************************************************************************
 * tests/testcases/media_test/media_rpc/rpc_server/media_server.c
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
#include <poll.h>
#include <nuttx/config.h>
#include <syslog.h>
#include <stdlib.h>
#include "media_parcel.h"
#include "media_server.h"

static void *server_handle = NULL;

/****************************************************************************
 * Function Name: server_test_transaction
 *
 * Description:
 *  The callback function, used to process messages sent by clients
 *
 ****************************************************************************/
static int server_test_transaction(void *cookie, media_parcel *in, media_parcel *out)
{
    char *test;
    uint32_t data;
    const char *str;
    test = cookie;
    media_parcel_read_uint32(in, &data);
    str = media_parcel_read_string(in);

    if (out) {
        uint32_t re_data = 23;
        char *re_str = "stop";
        media_parcel_append_uint32(out, re_data);
        media_parcel_append_string(out, re_str);
    }

    /* stress test, dont printf log */
    if (data != 1 && data != 2)
        syslog(LOG_INFO, "[media_server] transaction:%p,%ld,%s\n", test, (long int)data, str);

    if (data == 32 || data == 1) {
        media_parcel in1;
        uint32_t in_data = 8;
        media_parcel_init(&in1);
        media_parcel_append_uint32(&in1, in_data);
        media_parcel_append_string(&in1, "notify");
        if (data != 1)
            syslog(LOG_INFO, "[media_server] start notify\n");
        media_server_notify(server_handle, test, &in1);
    }

    if (data == 33) {
        media_server_destroy(server_handle);
        exit(0);
    }
    return 0;
}

/****************************************************************************
 * Function Name: mediarpc_server_start
 *
 * Description:
 *  The rpc server procedure.
 *  After starting the server process, client can connect.
 *
 ****************************************************************************/
static void mediarpc_server_start(void)
{
    struct pollfd fds[16];
    int *conns[16];
    int count;

    server_handle = media_server_create(server_test_transaction);
    if (server_handle == NULL)
        return;

    while (1) {
        count = media_server_get_pollfds(server_handle, fds, (void **)conns, 16);
        poll(fds, count, -1);
        for (int i = 0; i < count; i++)
            if (fds[i].revents)
                media_server_poll_available(server_handle, &fds[i], conns[i]);
    }
}

/* This program is just for the client to test */
int main(int argc, FAR char *argv[])
{
    syslog(LOG_INFO, "[media_server] start server\n");
    mediarpc_server_start();
    return 0;
}
