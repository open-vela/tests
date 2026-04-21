# ##############################################################################
# apps/tests/testcases/secure_test/CMakeLists.txt
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
if(CONFIG_CA_TEST)
  nuttx_add_application(
    NAME
    secure_ca_alipay_test
    STACKSIZE
    ${CONFIG_TESTING_TESTCASES_STACKSIZE}
    PRIORITY
    ${CONFIG_TESTING_TESTCASES_PRIORITY}
    SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/secure_test/api/AlipayApiTest.c
    MODULE
    ${CONFIG_TESTS_TESTCASES})
  nuttx_add_application(
    NAME
    secure_ca_triad_test
    STACKSIZE
    ${CONFIG_TESTING_TESTCASES_STACKSIZE}
    PRIORITY
    ${CONFIG_TESTING_TESTCASES_PRIORITY}
    SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/secure_test/api/TriadApiTest.c
    MODULE
    ${CONFIG_TESTS_TESTCASES})
  if(CONFIG_ARCH_RISCV)
    nuttx_add_application(
      NAME
      secure_ca_wxcodepay_test
      STACKSIZE
      ${CONFIG_TESTING_TESTCASES_STACKSIZE}
      PRIORITY
      ${CONFIG_TESTING_TESTCASES_PRIORITY}
      SRCS
      ${CMAKE_CURRENT_SOURCE_DIR}/secure_test/api/WxcodepayApiTest.c
      MODULE
      ${CONFIG_TESTS_TESTCASES})
  endif()
  nuttx_add_application(
    NAME
    secure_ca_pin_test
    STACKSIZE
    ${CONFIG_TESTING_TESTCASES_STACKSIZE}
    PRIORITY
    ${CONFIG_TESTING_TESTCASES_PRIORITY}
    SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/secure_test/api/PinApiTest.c
    MODULE
    ${CONFIG_TESTS_TESTCASES})
endif()
if(CONFIG_KEYSTORE_TEST)
  include_directories(
    ${NUTTX_DIR}/../external/android/system/security/keystore/include)
  nuttx_add_application(
    NAME
    keystore_test_insert
    STACKSIZE
    ${CONFIG_TESTING_TESTCASES_STACKSIZE}
    PRIORITY
    ${CONFIG_TESTING_TESTCASES_PRIORITY}
    SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/secure_test/keystore/keystore_insert_test.c
    MODULE
    ${CONFIG_TESTS_TESTCASES})
  nuttx_add_application(
    NAME
    keystore_test_get
    STACKSIZE
    ${CONFIG_TESTING_TESTCASES_STACKSIZE}
    PRIORITY
    ${CONFIG_TESTING_TESTCASES_PRIORITY}
    SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/secure_test/keystore/keystore_get_test.c
    MODULE
    ${CONFIG_TESTS_TESTCASES})
  nuttx_add_application(
    NAME
    keystore_test_multi
    STACKSIZE
    ${CONFIG_TESTING_TESTCASES_STACKSIZE}
    PRIORITY
    ${CONFIG_TESTING_TESTCASES_PRIORITY}
    SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/secure_test/keystore/keystore_insert_multi.c
    MODULE
    ${CONFIG_TESTS_TESTCASES})
  nuttx_add_application(
    NAME
    keystore_test_block
    STACKSIZE
    ${CONFIG_TESTING_TESTCASES_STACKSIZE}
    PRIORITY
    ${CONFIG_TESTING_TESTCASES_PRIORITY}
    SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/secure_test/keystore/keystore_block_test.c
    MODULE
    ${CONFIG_TESTS_TESTCASES})
endif()
if(CONFIG_MBEDTLS_TEST)
  set(MBEDTLS_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mbedtls)
  file(GLOB APP_SOURCES ${MBEDTLS_TEST_DIR}/*.c)
  file(GLOB CSRCS ${NUTTX_DIR}/../apps/crypto/mbedtls/mbedtls/tests/src/*.c)
  include_directories(${NUTTX_DIR}/../apps/crypto/mbedtls/mbedtls/tests/include
                      ${NUTTX_DIR}/../apps/crypto/mbedtls/mbedtls/library)
  foreach(APP_SOURCE ${APP_SOURCES})
    string(REPLACE "." "_" APP_NAME_T ${APP_SOURCE})
    get_filename_component(APP_NAME ${APP_NAME_T} NAME_WE)
    set(EXECUTABLE_NAME "mbedtls_${APP_NAME}")
    nuttx_add_application(
      NAME
      ${EXECUTABLE_NAME}
      PRIORITY
      ${CONFIG_TESTING_TESTCASES_PRIORITY}
      STACKSIZE
      ${CONFIG_TESTING_TESTCASES_STACKSIZE}
      SRCS
      ${APP_SOURCE}
      ${CSRCS}
      ${NUTTX_DIR}/../apps/crypto/mbedtls/mbedtls/tests/src/test_helpers/ssl_helpers.c
      MODULE
      ${CONFIG_TESTS_TESTCASES}
    )
  endforeach()
endif()
