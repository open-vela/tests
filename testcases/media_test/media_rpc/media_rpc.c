/****************************************************************************
 * tests/testcases/media_test/media_rpc/media_rpc.c
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
#include <stdio.h>
#include <nuttx/config.h>
#include <syslog.h>
#include "media_rpc_test.h"

/* Maximum number of client connections allowed by the server */
#define MAX_SERVER_CONNECT_NUM 10

int on_listener_test(const void *cookie, media_parcel *parcel)
{
    uint32_t data;
    const char *str;
    media_parcel_read_uint32(parcel, &data);
    str = media_parcel_read_string(parcel);
    if (data != 8)
    	syslog(LOG_INFO, "[media_client] listener receive: %d, %s\n", (int)data, str);
    return 0;
}

int server_test_common_transaction(void *cookie, media_parcel *in, media_parcel *out)
{
    return 0;
}
