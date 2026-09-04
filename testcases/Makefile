# apps/tests/testcases/Makefile
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


PRIORITY = $(CONFIG_TESTING_TESTCASES_PRIORITY)
STACKSIZE = $(CONFIG_TESTING_TESTCASES_STACKSIZE)
MODULE = $(CONFIG_TESTS_TESTCASES)

# treat all warning as error
CFLAGS += -Werror
CFLAGS += -Wno-unused-function

ifneq ($(CONFIG_SYSLOG_TEST),)
	include $(CURDIR)/syslog_test/syslog_test.mk
	CFLAGS += -I$(CURDIR)/mmtest/include
endif

ifneq ($(CONFIG_MM_TEST),)
	include $(CURDIR)/mmtest/mem_test.mk
	CFLAGS += -I$(CURDIR)/mmtest/include
endif

ifneq ($(CONFIG_KVDB_TEST),)
	include $(CURDIR)/kvtest/kvdb.mk
	CFLAGS += -I$(CURDIR)/kvtest/include
	CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/frameworks/utils
endif

ifneq ($(CONFIG_FS_TEST),)
	include $(CURDIR)/vela_fs_test/fs_test.mk
	CFLAGS += -I$(APPDIR)/nshlib
	CFLAGS += -I$(CURDIR)/vela_fs_test/fs/include
endif

ifneq ($(CONFIG_SOCKET_RPMSG_TEST),)
	include $(CURDIR)/socket_rpmsg/socket_rpmsg.mk
endif

ifneq ($(CONFIG_RPMSGDEV_TEST),)
	include $(CURDIR)/rpmsgdev/rpmsgdev.mk
endif

ifneq ($(CONFIG_RPMSGBLK_TEST),)
	include $(CURDIR)/rpmsgblk/rpmsgblk.mk
        include $(CURDIR)/mmc_utils/mmc_utils.mk
endif

ifneq ($(CONFIG_RPTUNRESET_TEST),)
	include $(CURDIR)/rptunreset/rptunreset.mk
endif

ifneq ($(CONFIG_SCHED_TEST),)
	include $(CURDIR)/sched_test/sched_test.mk
	CFLAGS += -I$(CURDIR)/sched_test/include
endif

ifneq ($(CONFIG_UORB_CASE_TEST),)
	include $(CURDIR)/uorb_test/uorb_test.mk
	CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/system/uorb
endif

ifneq ($(CONFIG_SOCKET_PERFORMANCE_TEST),)
	include $(CURDIR)/socket_performance/socket_perf.mk
endif

ifneq ($(CONFIG_MEDIAS_TEST),)
	CFLAGS += -I$(CURDIR)/media_test/include
	CFLAGS +=-I$(APPDIR)/frameworks/media
	CFLAGS +=-I$(APPDIR)/frameworks/media/utils
	CFLAGS +=-I$(APPDIR)/external/ffmpeg/ffmpeg
	include $(CURDIR)/media_test/media_test.mk
endif

ifneq ($(CONFIG_TASK_TRACE_TEST),)
	include $(CURDIR)/task_trace_test/task_trace_test.mk
endif

ifneq ($(CONFIG_DHCPC_TEST),)
	include $(CURDIR)/network/dhcp_test/dhcpc_noblock.mk
endif

ifneq ($(CONFIG_NETWORK_TEST),)
	include $(CURDIR)/network/socket/socket.mk
endif

ifneq ($(CONFIG_SOCKET_STAB_TEST),)
	include $(CURDIR)/network/socketstab/socketstab.mk
endif

ifneq ($(CONFIG_LOCAL_SOCKET),)
	include $(CURDIR)/network/localsocket/local_socket.mk
endif

ifneq ($(CONFIG_NETWORK_STABILITY_TEST),)
	include $(CURDIR)/network/stability/socket_stability.mk
endif

ifeq ($(CONFIG_MINSYS_APPLET_TEST)$(CONFIG_APPLET), yy)
	include $(CURDIR)/minsys_applet_test/minsys_applet_test.mk
endif

ifneq ($(CONFIG_SECURE_TEST),)
	include $(CURDIR)/secure_test/secure_test.mk
endif

ifneq ($(CONFIG_TESTCASE_KASAN_TEST),)
	include $(CURDIR)/kasan_test/kasan_test.mk
endif

ifneq ($(CONFIG_CPUFREQ_TEST),)
	include $(CURDIR)/cpufreq_test/cpufreq_test.mk
endif

ifneq ($(CONFIG_REGULATOR_TEST),)
	include $(CURDIR)/regulator_test/regulator_test.mk
endif

ifneq ($(CONFIG_VIBRATOR_TEST),)
	include $(CURDIR)/vibrator_test/vibrator_test.mk
	CFLAGS += -I$(CURDIR)/vibrator_test/include
	CFLAGS += -I${APPDIR}/frameworks/vibrator/include
endif

include $(CURDIR)/flash_rw/flash_rw.mk

include $(CURDIR)/abnormal_test/abnormal_test.mk

include $(CURDIR)/block_device_test/block_test.mk

include $(CURDIR)/libc_test/libc_test.mk

include $(CURDIR)/spi_slave_test/spi_slave_test.mk

include $(CURDIR)/smp_test/smp_test.mk

include $(CURDIR)/http_test/http_test.mk

include $(CURDIR)/repeat/repeat.mk

include $(CURDIR)/graphics_test/graphics_test.mk

ifneq ($(CONFIG_BINDER_AIDL_TEST),)
	include $(CURDIR)/binder_test/binder_test.mk
endif

ifneq ($(CONFIG_FDCHECK_TEST),)
	include $(CURDIR)/fdcheck_test/fdcheck_test.mk
endif

include $(APPDIR)/Application.mk
