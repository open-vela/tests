# ##############################################################################
# apps/tests/testcases/sched_test/CMakeLists.txt
#
# Licensed to the Apache Software Foundation (ASF) under one or more contributor
# license agreements.  See the NOTICE file distributed with this work for

# additional information regarding copyright ownership.  The ASF licenses this
# file to you under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
#
# ##############################################################################

if(CONFIG_SCHED_TEST)
    macro(add_sched_test name path)
        nuttx_add_application(
            NAME ${name}
            STACKSIZE ${CONFIG_TESTING_TESTCASES_STACKSIZE}
            INCLUDE_DIRECTORIES
            (${CMAKE_CURRENT_SOURCE_DIR}/include)
            PRIORITY ${CONFIG_TESTING_TESTCASES_PRIORITY}
            SRCS ${CMAKE_CURRENT_SOURCE_DIR}/${path}
            MODULE
            ${CONFIG_TESTS_TESTCASES})
    endmacro()
    add_sched_test(vela_sched_interface_task_test01 "interface/task_test01.c")
    add_sched_test(vela_sched_interface_task_test02 "interface/task_test02.c")
    add_sched_test(vela_sched_interface_task_test03 "interface/task_test03.c")
    add_sched_test(vela_sched_interface_task_test04 "interface/task_test04.c")
    add_sched_test(vela_sched_interface_task_test05 "interface/task_test05.c")
    add_sched_test(vela_sched_interface_task_test06 "interface/task_test06.c")
    add_sched_test(vela_sched_interface_task_test07 "interface/task_test07.c")

    add_sched_test(vela_sched_interface_pthread_test01 "interface/pthread_test01.c")
    add_sched_test(vela_sched_interface_pthread_test02 "interface/pthread_test02.c")
    add_sched_test(vela_sched_interface_pthread_test03 "interface/pthread_test03.c")
    add_sched_test(vela_sched_interface_pthread_test04 "interface/pthread_test04.c")
    add_sched_test(vela_sched_interface_pthread_test05 "interface/pthread_test05.c")
    add_sched_test(vela_sched_interface_pthread_test06 "interface/pthread_test06.c")

    add_sched_test(vela_sched_pthreads_pth01 "pthreads/pth01.c")
    add_sched_test(vela_sched_pthreads_pth02 "pthreads/pth02.c")
    add_sched_test(vela_sched_pthreads_pth03 "pthreads/pth03.c")
    add_sched_test(vela_sched_pthreads_pth04 "pthreads/pth04.c")
endif()
