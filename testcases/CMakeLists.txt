# ##############################################################################
# apps/tests/testcases/CMakeLists.txt
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

# ~~~
# Important note:
# This CMakeLists.txt is not meant as a top-level CMakeLists.txt. Instead,
# CMake must be run using the top-level CMakeLists.txt from the nuttx
# repository and point to this directory via NUTTX_APPS_DIR. For example:
#   cmake -S <nuttx-dir> -B <build-dir> [...] -DNUTTX_APPS_DIR=<apps-dir>
# ~~~

if(TARGET nuttx)
  if(NOT CONFIG_ARCH_TOOLCHAIN_TASKING)
    add_compile_options(-Wno-unused-function)
  endif()
  nuttx_add_library(inner_testcases)
  target_sources(inner_testcases PRIVATE ${NUTTX_APPS_BINDIR}/dummy.c)
  function(testcase_srcs)
    target_sources(inner_testcases PRIVATE ${ARGN})
  endfunction()

  function(testcase_incdirs)
    target_include_directories(inner_testcases PRIVATE ${ARGN})
  endfunction()

  function(testcase_options)
    target_compile_options(inner_testcases PRIVATE ${ARGN})
  endfunction()

  macro(testcase_app PROG MAIN)
    nuttx_add_application(
      NAME
      ${PROG}
      STACKSIZE
      ${CONFIG_TESTING_TESTCASES_STACKSIZE}
      PRIORITY
      ${CONFIG_TESTING_TESTCASES_PRIORITY}
      SRCS
      ${MAIN}
      INCLUDE_DIRECTORIES
      $<GENEX_EVAL:$<TARGET_PROPERTY:inner_testcases,INCLUDE_DIRECTORIES>>
      MODULE
      ${CONFIG_TESTS_TESTCASES})
  endmacro()

endif()

nuttx_add_subdirectory()
