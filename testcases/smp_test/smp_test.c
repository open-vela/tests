/****************************************************************************
 * apps/examples/hello/hello_main.c
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

#include <nuttx/config.h>
#include <stdio.h>
#include <syslog.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

static void *service_thread0(void *arg)
{
    int cnt = 10000 * 50000;

    cpu_set_t cpu_mask;

    CPU_ZERO(&cpu_mask);
    CPU_SET(0, &cpu_mask);

    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu_mask);

    while (1)
    {
        if (cnt-- == 0)
        {
            syslog(1, "%s, line %d\n", __func__, __LINE__);
            cnt = 10000 * 10000;
            usleep(20);
            DEBUGASSERT(sched_getcpu() == 0);
        }
    }

    return NULL;
}

static void *service_thread1(void *arg)
{
    int cnt = 1000 * 50000;

    cpu_set_t cpu_mask;

    CPU_ZERO(&cpu_mask);
    CPU_SET(0, &cpu_mask);

    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu_mask);

    while (1)
    {
        if (cnt-- == 0)
        {
            syslog(1, "%s, line %d\n", __func__, __LINE__);
            cnt = 10000 * 10000;
            usleep(10);
            DEBUGASSERT(sched_getcpu() == 0);
        }
    }

    return NULL;
}

static void *service_thread2(void *arg)
{
    int cnt = 1000 * 10000;

    cpu_set_t cpu_mask;

    CPU_ZERO(&cpu_mask);
    CPU_SET(1, &cpu_mask);

    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu_mask);

    while (1)
    {
        if (cnt-- == 0)
        {
            syslog(1, "%s, line %d\n", __func__, __LINE__);
            cnt = 10000 * 10000;
            usleep(100);
            DEBUGASSERT(sched_getcpu() == 1);
        }
    }

    return NULL;
}

/****************************************************************************
 * hello_main
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
#if 0
  uint32_t addr, len;

  addr = strtoul(argv[1], NULL, 16);
  len = strtoul(argv[2], NULL, 16);

  printf("Hello, World!! %x, %x\n", addr, len);

  up_invalidate_dcache(addr, addr + len);
#else
  struct sched_param param;
  pthread_attr_t attr;
  pthread_t thread0, thread1, thread2;
  pthread_attr_init(&attr);
  param.sched_priority = 254;
  pthread_attr_setschedparam(&attr, &param);
  pthread_attr_setstacksize(&attr, 4 * 1024);

  pthread_create(&thread0, &attr, service_thread0, NULL);
  pthread_create(&thread1, &attr, service_thread1, NULL);
  pthread_create(&thread2, &attr, service_thread2, NULL);

  pthread_join(thread0, NULL);
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  printf("%s, line %d\n", __func__, __LINE__);
#endif

  return 0;
}
