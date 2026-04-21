/****************************************************************************
 * tests/testcases/media_test/media_rpc/rpc_parcel/media_parcel02.c
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
#include <string.h>
#include <nuttx/config.h>
#include <syslog.h>
#include "media_parcel.h"

/**************************************************************************
Test Suite Name: MediaTest

Author: caixucheng

Date: 2021-11-25

Description:
    1. Override the append and read interfaces of media_parcel.

**************************************************************************/
int main(int argc, FAR char *argv[])
{
    media_parcel parcel;

    uint16_t w_udata16 = 1;
    uint32_t w_udata32 = 2;
    uint64_t w_udata64 = 3;

    char *w_str1 = "test1";
    char *w_str2 = "test2";

    int16_t  w_data16 = 1;
    int32_t  w_data32 = -1;
    int64_t  w_data64 = 0;

    float w_fdata = 1.2;
    double w_ddata = 2.3;

    uint16_t r_udata16;
    uint32_t r_udata32;
    uint64_t r_udata64;

    int16_t r_data16;
    int32_t r_data32;
    int64_t r_data64;

    const char *r_str1;
    const char *r_str2;

    float r_fdata;
    double r_ddata;

    media_parcel_init(&parcel);
    if (media_parcel_append_uint16(&parcel, w_udata16) != 0)
        goto error;
    if (media_parcel_append_uint32(&parcel, w_udata32) != 0)
        goto error;
    if (media_parcel_append_uint64(&parcel, w_udata64) != 0)
        goto error;
    if (media_parcel_append(&parcel, w_str1, strlen(w_str1) + 1) != 0)
        goto error;
    if (media_parcel_append_string(&parcel, w_str2) != 0)
        goto error;
    if (media_parcel_append_int16(&parcel, w_data16) != 0)
        goto error;
    if (media_parcel_append_int32(&parcel, w_data32) != 0)
        goto error;
    if (media_parcel_append_int64(&parcel, w_data64) != 0)
        goto error;
    if (media_parcel_append_float(&parcel, w_fdata) != 0)
        goto error;
    if (media_parcel_append_double(&parcel, w_ddata) != 0)
        goto error;
    if (media_parcel_read_uint16(&parcel, &r_udata16) != 0 || w_udata16 != r_udata16)
        goto error;
    if (media_parcel_read_uint32(&parcel, &r_udata32) != 0 || r_udata32 != w_udata32)
        goto error;
    if (media_parcel_read_uint64(&parcel, &r_udata64) != 0 || r_udata64 != w_udata64)
        goto error;

    r_str1 = media_parcel_read_string(&parcel);
    r_str2 = media_parcel_read_string(&parcel);
    if (r_str1 == NULL || strcmp(r_str1 , w_str1) != 0)
        goto error;
    if (r_str2 == NULL || strcmp(r_str2 , w_str2) != 0)
        goto error;
    if (media_parcel_read_int16(&parcel, &r_data16) != 0 || r_data16 != w_data16)
        goto error;
    if (media_parcel_read_int32(&parcel, &r_data32) != 0 || r_data32 != w_data32)
        goto error;
    if (media_parcel_read_int64(&parcel, &r_data64) != 0 || r_data64 != w_data64)
        goto error;
    if (media_parcel_read_float(&parcel, &r_fdata) != 0 || r_fdata != w_fdata)
        goto error;
    if (media_parcel_read_double(&parcel, &r_ddata) != 0 || r_ddata != w_ddata)
        goto error;

    syslog(LOG_INFO, "PASS ! media rpc parcel test pass \n");
    return 0;

error:
    media_parcel_deinit(&parcel);
    syslog(LOG_ERR, "FAIL! media_parcel02 fail.\n");
    return 0;
}
