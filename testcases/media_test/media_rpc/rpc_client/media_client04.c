/****************************************************************************
 * tests/testcases/media_test/media_rpc/rpc_client/media_client04.c
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

Date: 2021-11-08

Description:
    1. This test case is testing media_proxy_send().
    2. The handle parameter is null.

**************************************************************************/
int main(int argc, FAR char *argv[])
{
    media_parcel in;
    media_parcel_init(&in);
    if (media_parcel_append_int32(&in, 10)!= 0) {
        syslog(LOG_ERR, "FAIL! media parcel append int32 data fail.\n");
        return 0;
    }

    if (media_proxy_send(NULL, &in) == 0) {
        syslog(LOG_ERR, "FAIL! media client should fail.\n");
        return 0;
    }

    syslog(LOG_INFO, "PASS! media_client04 test pass .\n");
    return 0;
}