/****************************************************************************
 * tests/testcases/media_test/media_rpc/rpc_server/media_server01.c
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
#include "media_server.h"

/**************************************************************************
Test Suite Name: MediaTest

Author: caixucheng

Date: 2021-11-09

Description:
    1. This test case is used to test media_server_create() and media_server_destory().
    2. The parameters of media_server_create() and media_server_destory() are null.

**************************************************************************/
int main(int argc, FAR char *argv[])
{
    void *server_handle = media_server_create(NULL);
    if (server_handle)
        syslog(LOG_ERR, "FAIL!media create server should fail.\n");

    if (media_server_destroy(NULL) < 0 && server_handle == NULL) {
        syslog(LOG_INFO, "PASS!media_server01 pass.\n");
        return 0;
    }

    syslog(LOG_ERR, "FAIL!media destroy server should fail.\n");
    return 0;
}