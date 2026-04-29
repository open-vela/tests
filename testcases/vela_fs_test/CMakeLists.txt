# ##############################################################################
# apps/tests/testcases/vela_fs_test/CMakeLists.txt
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

if(CONFIG_FS_TEST)

  testcase_incdirs(${NUTTX_APPS_DIR}/nshlib
                   ${CMAKE_CURRENT_LIST_DIR}/fs/include)

  if(CONFIG_FS_TEST_STRESS)
    testcase_app(vela_fs_stress_write_full_file fs/stress/write_full_file.c)
    testcase_app(vela_fs_stress_write_speed fs/stress/write_speed.c)
    testcase_app(vela_fs_stress_loop_create_delete_file_test
                 fs/stress/loop_create_delete_file_test.c)
    testcase_app(vela_fs_stress_maximum_file_name_test
                 fs/stress/maximum_file_name_test.c)
    testcase_app(vela_fs_stress_maximum_number_of_open_file_test
                 fs/stress/maximum_number_of_open_file_test.c)
    testcase_app(vela_fs_stress_multi_thread_file_operate_test
                 fs/stress/multi_thread_file_operate_test.c)
    testcase_app(vela_fs_stress_read_and_write_loops_test
                 fs/stress/read_and_write_loops_test.c)
    testcase_app(vela_fs_random_read_and_write_test
                 fs/stress/random_read_and_write_test.c)
    testcase_app(vela_fs_multi_thread_write_test
                 fs/stress/multi_thread_write_test.c)
    testcase_app(vela_fs_multi_thread_read_test
                 fs/stress/multi_thread_read_test.c)

    testcase_app(vela_fs_multi_thread_read_write_test
                 fs/stress/multi_thread_read_write_test.c)
    testcase_app(vela_fs_file_fragmentation_test
                 fs/stress/file_fragmentation_test.c)
    testcase_app(vela_fs_file_pressure_test fs/stress/file_pressure_test.c)
    testcase_app(performance_test fs/stress/performance_test.c)
  endif()

  if(CONFIG_FS_TEST_STABILITY)
    testcase_app(vela_fs_stability_test01 fs/stability/test01.c)
    testcase_app(vela_fs_stability_test02 fs/stability/test02.c)
    testcase_app(vela_fs_stability_test03 fs/stability/test03.c)
    testcase_app(vela_fs_stability_test04 fs/stability/test04.c)
  endif()

  if(CONFIG_FS_TEST_POWEROFF)
    testcase_app(power_off_test01 fs/power_off/power_off_test01.c)
    testcase_app(power_off_test02 fs/power_off/power_off_test02.c)
    testcase_app(power_off_test03 fs/power_off/power_off_test03.c)
  endif()

  if(CONFIG_FS_TEST_EDONLY)
    testcase_incdirs(${CMAKE_CURRENT_LIST_DIR}/rdonly_fs/include)
    testcase_srcs(${CMAKE_CURRENT_LIST_DIR}/rdonly_fs/lib/md5.c)
    testcase_app(md5_test rdonly_fs/md5_test.c)
  endif()

endif()
