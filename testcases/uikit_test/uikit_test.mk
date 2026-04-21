ifneq ($(CONFIG_UIKIT_TEST),)
PROGNAME += uikit_test
PRIORITY += $(CONFIG_UIKIT_TEST_PRIORITY)
STACKSIZE += $(CONFIG_UIKIT_TEST_STACKSIZE)
MODULE += $(CONFIG_UIKIT_TEST)

TESTDIR = $(APPDIR)/tests/testcases/uikit_test

ifeq ($(CONFIG_MARKDOWN_TEST), y)
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/frameworks/graphics/uikit/include
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/frameworks/graphics/uikit/src
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/frameworks/graphics/uikit/tests
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/external/cmark-gfm/
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/external/cmark-gfm/cmark-gfm/src
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/external/cmark-gfm/cmark-gfm/extensions
CFLAGS += ${INCDIR_PREFIX}$(TESTDIR)/markdown
CSRCS += $(wildcard $(TESTDIR)/markdown/*.c)
endif # CONFIG_MARKDOWN_TEST

CFLAGS += ${INCDIR_PREFIX}$(TESTDIR)
MAINSRC += $(TESTDIR)/uikit_test.c
endif # CONFIG_UIKIT_TEST
