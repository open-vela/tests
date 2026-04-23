/****************************************************************************
 * tests/testcases/media_test/media_rpc/rpc_server/media_server03.c
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
#include "media_server.h"
#include "media_rpc_test.h"

/**************************************************************************
Test Suite Name: MediaTest

Author: caixucheng

Date: 2021-11-10

Description:
    1. This test case is test media_server_get_pollfds() interface parameters.
    2. The frist case, test handle is null.
    3. The Second case, test fds is null.
    4. The third case, test count < 2 .

**************************************************************************/
int main(int argc, FAR char *argv[])
{
    struct media_rpc_test {
        void *handle;
        int count;
        struct pollfd *pollfds;
    };
    struct pollfd fds[16];
    int *conns[16];
    void *server_handle = NULL;
    int len = 0, f = 1;

    server_handle = media_server_create(server_test_common_transaction);
    if (!server_handle) {
        syslog(LOG_ERR, "FAIL! media create server fail. \n");
        return 0;
    }

    struct media_rpc_test testcase[] = {
        {NULL, 16, fds},
        {server_handle, 16, NULL},
        {server_handle, 1, fds}
    };
    len = sizeof(testcase) / sizeof(struct media_rpc_test);

    for (int i = 0; i < len; i++) {
        if (media_server_get_pollfds(testcase[i].handle, testcase[i].pollfds, (void **)conns, testcase[i].count) != -EINVAL) {
            syslog(LOG_INFO, "FAIL! testcases %d should fail, and error num is : %d.\n", -EINVAL, i+1);
            f = 0;
        }
    }

    if (server_handle && media_server_destroy(server_handle) == 0) {
        server_handle = NULL;
    }

    if (f)
        syslog(LOG_INFO, "PASS \n");
    return 0;
}