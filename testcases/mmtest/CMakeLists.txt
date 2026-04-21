# ##############################################################################
# apps/tests/testcases/mmtest/CMakeLists.txt
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

if(CONFIG_MM_TEST)
  # Include directories
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
  nuttx_add_application(
    NAME
    vela_mmtest_mem01
    STACKSIZE
    ${CONFIG_TESTING_TESTCASES_STACKSIZE}
    PRIORITY
    ${CONFIG_TESTING_TESTCASES_PRIORITY}
    SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/mem/mem01.c
    MODULE
    ${CONFIG_TESTS_TESTCASES})
  nuttx_add_application(
    NAME
    vela_mmtest_mem02
    STACKSIZE
    ${CONFIG_TESTING_TESTCASES_STACKSIZE}
    PRIORITY
    ${CONFIG_TESTING_TESTCASES_PRIORITY}
    SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/mem/mem02.c
    MODULE
    ${CONFIG_TESTS_TESTCASES})
  if(CONFIG_ARCH_RISCV)
    nuttx_add_application(
      NAME
      vela_mmtest_mtest01
      STACKSIZE
      ${CONFIG_TESTING_TESTCASES_STACKSIZE}
      PRIORITY
      ${CONFIG_TESTING_TESTCASES_PRIORITY}
      SRCS
      ${CMAKE_CURRENT_SOURCE_DIR}/mem/mtest01.c
      MODULE
      ${CONFIG_TESTS_TESTCASES})
  endif()
  nuttx_add_application(
    NAME
    vela_mmtest_mmstress
    STACKSIZE
    ${CONFIG_TESTING_TESTCASES_STACKSIZE}
    PRIORITY
    ${CONFIG_TESTING_TESTCASES_PRIORITY}
    SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/mem/mmstress.c
    MODULE
    ${CONFIG_TESTS_TESTCASES})
  nuttx_add_application(
    NAME
    vela_mmtest_mmapcorruption
    STACKSIZE
    ${CONFIG_TESTING_TESTCASES_STACKSIZE}
    PRIORITY
    ${CONFIG_TESTING_TESTCASES_PRIORITY}
    SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/mem/mmap-corruption.c
    MODULE
    ${CONFIG_TESTS_TESTCASES})
  nuttx_add_application(
    NAME
    vela_mmtest_mmstress01
    STACKSIZE
    ${CONFIG_TESTING_TESTCASES_STACKSIZE}
    PRIORITY
    ${CONFIG_TESTING_TESTCASES_PRIORITY}
    SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/mem/mmstress01.c
    MODULE
    ${CONFIG_TESTS_TESTCASES})
  nuttx_add_application(
    NAME
    vela_mmtest_mmstress02
    STACKSIZE
    ${CONFIG_TESTING_TESTCASES_STACKSIZE}
    PRIORITY
    ${CONFIG_TESTING_TESTCASES_PRIORITY}
    SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/mem/mmstress02.c
    MODULE
    ${CONFIG_TESTS_TESTCASES})
  nuttx_add_application(
    NAME
    vela_mmtest_mmstress03
    STACKSIZE
    ${CONFIG_TESTING_TESTCASES_STACKSIZE}
    PRIORITY
    ${CONFIG_TESTING_TESTCASES_PRIORITY}
    SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/mem/mmstress03.c
    MODULE
    ${CONFIG_TESTS_TESTCASES})
endif()
