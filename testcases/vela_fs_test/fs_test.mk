ifneq ($(CONFIG_FS_TEST_STRESS),)
	PROGNAME += vela_fs_stress_write_full_file
	MAINSRC += $(CURDIR)/vela_fs_test/fs/stress/write_full_file.c
	PROGNAME += vela_fs_stress_write_speed
	MAINSRC += $(CURDIR)/vela_fs_test/fs/stress/write_speed.c
	PROGNAME += vela_fs_stress_loop_create_delete_file_test
	MAINSRC += $(CURDIR)/vela_fs_test/fs/stress/loop_create_delete_file_test.c
	PROGNAME += vela_fs_stress_maximum_file_name_test
	MAINSRC += $(CURDIR)/vela_fs_test/fs/stress/maximum_file_name_test.c
	PROGNAME += vela_fs_stress_maximum_number_of_open_file_test
	MAINSRC += $(CURDIR)/vela_fs_test/fs/stress/maximum_number_of_open_file_test.c
	PROGNAME += vela_fs_stress_multi_thread_file_operate_test
	MAINSRC += $(CURDIR)/vela_fs_test/fs/stress/multi_thread_file_operate_test.c
	PROGNAME += vela_fs_stress_read_and_write_loops_test
	MAINSRC += $(CURDIR)/vela_fs_test/fs/stress/read_and_write_loops_test.c
	PROGNAME += vela_fs_random_read_and_write_test
	MAINSRC += $(CURDIR)/vela_fs_test/fs/stress/random_read_and_write_test.c
	PROGNAME += vela_fs_multi_thread_write_test
	MAINSRC += $(CURDIR)/vela_fs_test/fs/stress/multi_thread_write_test.c
	PROGNAME += vela_fs_multi_thread_read_test
	MAINSRC += $(CURDIR)/vela_fs_test/fs/stress/multi_thread_read_test.c
	PROGNAME += vela_fs_multi_thread_read_write_test
	MAINSRC += $(CURDIR)/vela_fs_test/fs/stress/multi_thread_read_write_test.c
	PROGNAME += vela_fs_file_fragmentation_test
	MAINSRC += $(CURDIR)/vela_fs_test/fs/stress/file_fragmentation_test.c
	PROGNAME += vela_fs_file_pressure_test
	MAINSRC += $(CURDIR)/vela_fs_test/fs/stress/file_pressure_test.c
	PROGNAME += performance_test
	MAINSRC += $(CURDIR)/vela_fs_test/fs/stress/performance_test.c
endif # no test fs performance , stress & stream

ifneq ($(CONFIG_FS_TEST_STABILITY),)
	PROGNAME += vela_fs_stability_test01
	MAINSRC += $(CURDIR)/vela_fs_test/fs/stability/test01.c
	PROGNAME += vela_fs_stability_test02
	MAINSRC += $(CURDIR)/vela_fs_test/fs/stability/test02.c
	PROGNAME += vela_fs_stability_test03
	MAINSRC += $(CURDIR)/vela_fs_test/fs/stability/test03.c
	PROGNAME += vela_fs_stability_test04
	MAINSRC += $(CURDIR)/vela_fs_test/fs/stability/test04.c
endif # no test fs stability

ifneq ($(CONFIG_FS_TEST_POWEROFF),)
	PROGNAME += power_off_test01
	MAINSRC += $(CURDIR)/vela_fs_test/fs/power_off/power_off_test01.c
	PROGNAME += power_off_test02
	MAINSRC += $(CURDIR)/vela_fs_test/fs/power_off/power_off_test02.c
	PROGNAME += power_off_test03
	MAINSRC += $(CURDIR)/vela_fs_test/fs/power_off/power_off_test03.c
endif

ifneq ($(CONFIG_FS_TEST_EDONLY),)
	CFLAGS += -I$(CURDIR)/vela_fs_test/rdonly_fs/include
	CSRCS += $(CURDIR)/vela_fs_test/rdonly_fs/lib/md5.c
	PROGNAME += md5_test
	MAINSRC += $(CURDIR)/vela_fs_test/rdonly_fs/md5_test.c
endif
