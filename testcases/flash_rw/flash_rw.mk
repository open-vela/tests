ifneq ($(CONFIG_FLASH_RW_TEST),)
	PROGNAME += vela_flash_rw_stability
	MAINSRC += $(CURDIR)/flash_rw/flash_rw_stability.c
	PROGNAME += vela_flash_rw_task
	MAINSRC += $(CURDIR)/flash_rw/flash_rw_task.c
endif