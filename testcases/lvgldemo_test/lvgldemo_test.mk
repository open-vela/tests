ifneq ($(menuconfig LVGLDEMO_TEST),)
PROGNAME += lvgldemo_test
PRIORITY += $(CONFIG_LVGLDEMO_TEST_PRIORITY)
STACKSIZE += $(CONFIG_LVGLDEMO_TEST_STACKSIZE)
MODULE += $(CONFIG_LVGLDEMO_TEST)

TESTDIR = $(APPDIR)/tests/testcases/lvgldemo_test

ifeq ($(config VECTOR_GRAPHIC_TEST), y)
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/apps/graphics/lvgl/lvgl
CFLAGS += ${INCDIR_PREFIX}$(TESTDIR)/vector_graphic_test
CSRCS += $(wildcard $(TESTDIR)/vector_graphic_test/*.c)
endif # CONFIG_VECTOR_GRAPHIC_TEST

CFLAGS += ${INCDIR_PREFIX}$(TESTDIR)
MAINSRC += $(TESTDIR)/lvgldemo_test.c
endif # CONFIG_LVGLDEMO_TEST
