/****************************************************************************
 * tests/testcases/media_test/media_rpc/rpc_parcel/media_parcel01.c
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
#include <stdlib.h>
#include <nuttx/config.h>
#include <sys/socket.h>
#include <netpacket/rpmsg.h>
#include <syslog.h>
#include <sys/un.h>
#include <errno.h>
#include "media_parcel.h"
#include "media_rpc_test.h"
#include "media_proxy.h"

/**************************************************************************
Test Suite Name: MediaTest

Author: caixucheng

Date: 2021-11-08

Description:
    1. This test case is used to override the return value of the he recv() interface.
    2. In the first case, the wrong fd is used to receive the message, the value of recv() < 0.
    3. In the second case, the peer socket connection is closed, the value of recv() = 0.

**************************************************************************/
int main(int argc, FAR char *argv[])
{
    if(argc < 2){
        syslog(LOG_ERR, "please input service core\n");
        return -1;
    }
    char *cpu = argv[1];
    media_parcel in, recv_data, in1;
    int family, ret;
    socklen_t len;
    char key[32];
    snprintf(key, sizeof(key), "md:%s", cpu);

#ifdef CONFIG_MEDIA_SERVER
    struct sockaddr_un addr;
    family = PF_LOCAL;
    len = sizeof(struct sockaddr_un);
    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path, key);
#else
    struct sockaddr_rpmsg addr;
    family = AF_RPMSG;
    len = sizeof(struct sockaddr_rpmsg);
    addr.rp_family = AF_RPMSG;
    strcpy(addr.rp_name, key);
    strcpy(addr.rp_cpu, cpu);
#endif

    media_parcel_init(&in);
    media_parcel_init(&in1);
    media_parcel_init(&recv_data);
    if (media_parcel_append_int64(&in, 33) != 0 || media_parcel_append_int64(&in1, 31) != 0) {
        syslog(LOG_ERR, "FAIL! media parcel append int64 fail.\n");
        return 0;
    }

    /* The first case, the wrong fd is used to receive the message, the value of recv() < 0 */
    ret = media_parcel_recv(&recv_data, -1, NULL, 0);
    if (ret != -errno) {
        syslog(LOG_INFO, "FAIL! recv() should error, and errno is %d, now ret is %d\n", errno, ret);
        return 0;
    }

    /* Use the Socket API to connect to the server */
    int fd = socket(family, SOCK_STREAM, 0);
    if (fd <= 0) {
        syslog(LOG_ERR, "FAIL! socket() fail.\n");
        return 0;
    }
    if (connect(fd, (struct sockaddr *)&addr, len) < 0) {
        syslog(LOG_ERR, "FAIL! client socket connect server fail.\n");
        goto out;
    }

    /* Normal Send data */
    if (media_parcel_send(&in1, fd, MEDIA_PARCEL_SEND_ACK, 0) < 0) {
        syslog(LOG_ERR, "FAIL! media_parcel_send fail.\n");
        goto out;
    }
    if (media_parcel_recv(&recv_data, fd, NULL, 0) != 0) {
        syslog(LOG_ERR, "FAIL! recv fail.\n");
        goto out;
    }
    if (media_parcel_get_code(&recv_data) != MEDIA_PARCEL_REPLY) {
        syslog(LOG_ERR, "FAIL! media parcel code is error.\n");
        goto out;
    }

    /* Send data to notify server destory */
    if (media_parcel_send(&in, fd, MEDIA_PARCEL_SEND, 0) < 0) {
        syslog(LOG_ERR, "FAIL! media_parcel_send fail.\n");
        goto out;
    }
    sleep(3);
    ret = media_parcel_recv(&recv_data, fd, NULL, 0);
    if (ret != -EPIPE) {
        syslog(LOG_ERR, "FAIL! recv() should error, and errno is %d, now ret is %d\n", EPIPE, ret);
        goto out;
    }

    syslog(LOG_ERR, "PASS! media_parcel01 pass.\n");
out:
    close(fd);
    media_parcel_deinit(&in);
    media_parcel_deinit(&in1);
    media_parcel_deinit(&recv_data);
    return 0;
}
