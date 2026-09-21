# apps/tests/testsuites/Makefile
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#     http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

include $(APPDIR)/Make.defs

PRIORITY = $(CONFIG_TESTS_TESTSUITES_PRIORITY)
STACKSIZE = $(CONFIG_TESTS_TESTSUITES_STACKSIZE)
MODULE = $(CONFIG_TESTS_TESTSUITES)

# treat all warning as error
CFLAGS += -Werror

ifneq ($(CONFIG_CM_KVDB_TEST),)
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/frameworks/kvdb
CFLAGS += -I$(CURDIR)/kernel/kv/include
CSRCS += $(wildcard kernel/kv/cases/*.c)
CSRCS += $(wildcard kernel/kv/common/*.c)
PROGNAME += cmocka_kv_test
MAINSRC += $(CURDIR)/kernel/kv/cmocka_kv_test.c
endif

ifneq ($(CONFIG_CM_DFX_TEST),)
CFLAGS += -I$(CURDIR)/kernel/dfx/include
CFLAGS += -I$(APPDIR)/frameworks/dfx/include
CSRCS += $(wildcard kernel/dfx/cases/*.c)
PROGNAME += cmocka_dfx_test
MAINSRC += $(CURDIR)/kernel/dfx/cmocka_dfx_test.c
endif

include $(APPDIR)/Application.mk
