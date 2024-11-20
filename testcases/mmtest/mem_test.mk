ifneq ($(CONFIG_MM_TEST),)
	PROGNAME += mmtest_mem01
	MAINSRC += $(CURDIR)/mmtest/mem/mem01.c
	PROGNAME += mmtest_mem02
	MAINSRC += $(CURDIR)/mmtest/mem/mem02.c
ifeq ($(CONFIG_ARCH_RISCV),)
	PROGNAME += mmtest_mtest01
	MAINSRC += $(CURDIR)/mmtest/mem/mtest01.c
endif
	PROGNAME += mmtest_mmstress
	MAINSRC += $(CURDIR)/mmtest/mem/mmstress.c
	PROGNAME += mmtest_mmapcorruption
	MAINSRC += $(CURDIR)/mmtest/mem/mmap-corruption.c
	PROGNAME += mmtest_mmstress01
	MAINSRC += $(CURDIR)/mmtest/mem/mmstress01.c
	PROGNAME += mmtest_mmstress02
	MAINSRC += $(CURDIR)/mmtest/mem/mmstress02.c
	PROGNAME += mmtest_mmstress03
	MAINSRC += $(CURDIR)/mmtest/mem/mmstress03.c
endif # no mm test
