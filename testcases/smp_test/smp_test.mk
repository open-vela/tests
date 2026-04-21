ifneq ($(CONFIG_SMP_TEST),)
	PROGNAME += vela_smp_test
	MAINSRC += $(CURDIR)/smp_test/smp_test.c
endif
