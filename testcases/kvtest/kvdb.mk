ifneq ($(CONFIG_KVDB_TEST_API),)
	PROGNAME += vela_kvdb_property_set_test
	MAINSRC += $(CURDIR)/kvtest/api/property_set.c
	PROGNAME += vela_kvdb_property_delete_test
	MAINSRC += $(CURDIR)/kvtest/api/property_delete.c
	PROGNAME += vela_kvdb_property_get_test
	MAINSRC += $(CURDIR)/kvtest/api/property_get.c
	PROGNAME += vela_kvdb_property_list_test
	MAINSRC += $(CURDIR)/kvtest/api/property_list.c
	PROGNAME += vela_kvdb_property_set_bool_test
	MAINSRC += $(CURDIR)/kvtest/api/property_set_bool.c
	PROGNAME += vela_kvdb_property_get_bool_test
	MAINSRC += $(CURDIR)/kvtest/api/property_get_bool.c
	PROGNAME += vela_kvdb_property_set_int32_test
	MAINSRC += $(CURDIR)/kvtest/api/property_set_int32.c
	PROGNAME += vela_kvdb_property_get_int32_test
	MAINSRC += $(CURDIR)/kvtest/api/property_get_int32.c
	PROGNAME += vela_kvdb_property_set_int64_test
	MAINSRC += $(CURDIR)/kvtest/api/property_set_int64.c
	PROGNAME += vela_kvdb_property_get_int64_test
	MAINSRC += $(CURDIR)/kvtest/api/property_get_int64.c
endif # CONFIG_KVDB_TEST_API

ifneq ($(CONFIG_KVDB_TEST_FUNCTION),)
	PROGNAME += vela_kvdb_function_test01
	MAINSRC += $(CURDIR)/kvtest/function/function01.c
	PROGNAME += vela_kvdb_function_test02
	MAINSRC += $(CURDIR)/kvtest/function/function02.c
	PROGNAME += vela_kvdb_function_test03
	MAINSRC += $(CURDIR)/kvtest/function/function03.c
endif # CONFIG_KVDB_TEST_FUNCTION

ifneq ($(CONFIG_KVDB_TEST_EXCEPTION),)
	PROGNAME += vela_kvdb_exception_test01
	MAINSRC += $(CURDIR)/kvtest/exception/exception01.c
	PROGNAME += vela_kvdb_exception_test02
	MAINSRC += $(CURDIR)/kvtest/exception/exception02.c
	PROGNAME += vela_kvdb_exception_test03
	MAINSRC += $(CURDIR)/kvtest/exception/exception03.c
	PROGNAME += vela_kvdb_exception_test04
	MAINSRC += $(CURDIR)/kvtest/exception/exception04.c
	PROGNAME += vela_kvdb_exception_test05
	MAINSRC += $(CURDIR)/kvtest/exception/exception05.c
endif # CONFIG_KVDB_TEST_EXCEPTION

ifneq ($(CONFIG_KVDB_TEST_STRESS),)
	PROGNAME += vela_kvdb_stress_test01
	MAINSRC += $(CURDIR)/kvtest/stress/stress01.c
	PROGNAME += vela_kvdb_stress_test02
	MAINSRC += $(CURDIR)/kvtest/stress/stress02.c
	PROGNAME += vela_kvdb_stress_test03
	MAINSRC += $(CURDIR)/kvtest/stress/stress03.c
endif # CONFIG_KVDB_TEST_STRESS

ifneq ($(CONFIG_KVDB_TEST_STABILITY),)
	PROGNAME += vela_kvdb_stability_test01
	MAINSRC += $(CURDIR)/kvtest/stability/test01.c
	PROGNAME += vela_kvdb_stability_test02
	MAINSRC += $(CURDIR)/kvtest/stability/test02.c
endif # CONFIG_KVDB_TEST_STABILITY

ifneq ($(CONFIG_KVDB_TEST_MONITOR),)
	PROGNAME += vela_kvdb_monitor_simple
	MAINSRC += $(CURDIR)/kvtest/monitor/property_wait_test.c

	PROGNAME += vela_kvdb_monitor_complex
	MAINSRC += $(CURDIR)/kvtest/monitor/monitor_complex.c
endif # CONFIG_KVDB_TEST_MONITOR

ifneq ($(CONFIG_KVDB_TEST_POWERDOWN),)
	PROGNAME += vela_kvdb_powerdown
	MAINSRC += $(CURDIR)/kvtest/powerdown/powerdown.c
endif