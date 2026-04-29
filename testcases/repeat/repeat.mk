ifneq ($(CONFIG_TEST_REPEAT),)
	PROGNAME += repeat
	MAINSRC += $(CURDIR)/repeat/repeat.c
endif
