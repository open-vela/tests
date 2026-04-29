ifneq ($(CONFIG_GRAPHICS_TEST),)

PROGNAME += graphics_test
PRIORITY += $(CONFIG_GRAPHICS_TEST_PRIORITY)
STACKSIZE += $(CONFIG_GRAPHICS_TEST_STACKSIZE)
MODULE += $(CONFIG_GRAPHICS_TEST)

TESTDIR = $(APPDIR)/tests/testcases/graphics_test

ifeq ($(CONFIG_VIDEO_WIDGET_TEST), y)
CFLAGS += ${INCDIR_PREFIX}$(TESTDIR)/video_widget_test/include
CSRCS  += $(wildcard $(TESTDIR)/video_widget_test/*.c)
endif # CONFIG_VIDEO_WIDGET_TEST

ifeq ($(CONFIG_ANIMATION_ENGINE_TEST), y)
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/netutils/cjson/cJSON
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/frameworks/graphics/animengine/include
CFLAGS += ${INCDIR_PREFIX}$(TESTDIR)/animengine_test
CSRCS  += $(wildcard $(TESTDIR)/animengine_test/*.c)
endif # CONFIG_ANIMATION_ENGINE_TEST

ifeq ($(CONFIG_ANIMATION_ENGINE_CTEST), y)
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/frameworks/graphics/animengine/include
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/frameworks/graphics/animengine/include/animengine
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/frameworks/graphics/animengine/porting/lvgl
# CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/frameworks/graphics/animengine/examples/api_demo
CFLAGS += ${INCDIR_PREFIX}$(TESTDIR)/animengine_ctest
CSRCS  += $(wildcard $(TESTDIR)/animengine_ctest/*.c)
endif # CONFIG_ANIMATION_ENGINE_CTEST

ifeq ($(CONFIG_ANIM_PHYSICS_ENGINE_TEST), y)
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/frameworks/graphics/animengine/include
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/frameworks/graphics/animengine/src
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/frameworks/graphics/animengine/porting/lvgl
CFLAGS += ${INCDIR_PREFIX}$(TESTDIR)/physics_engine_test
CSRCS += $(wildcard $(TESTDIR)/physics_engine_test/*.c)
endif

ifeq ($(CONFIG_RIVE_TEST), y)
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/frameworks/graphics/animengine/include
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/frameworks/graphics/animengine/src
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/frameworks/graphics/animengine/porting/lvgl
CFLAGS += ${INCDIR_PREFIX}$(TESTDIR)/rive_test
CSRCS  += $(wildcard $(TESTDIR)/rive_test/*.c)
endif # CONFIG_RIVE_TEST

# GESTURE_TEST
ifeq ($(CONFIG_GESTURES_TEST), y)
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/apps/graphics/lvgl/lvgl
CFLAGS += ${INCDIR_PREFIX}$(TESTDIR)/lv_gestures_test
CSRCS += $(wildcard $(TESTDIR)/lv_gestures_test/*.c)
endif

ifeq ($(CONFIG_LV_BLUR_TEST), y)
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/apps/graphics/lvgl/lvgl
CFLAGS += ${INCDIR_PREFIX}$(TESTDIR)/lv_blur_test
CSRCS += $(wildcard $(TESTDIR)/lv_blur_test/*.c)
endif

CFLAGS  += ${INCDIR_PREFIX}$(TESTDIR)
MAINSRC += $(TESTDIR)/graphics_test_main.c
endif #CONFIG_GRAPHICS_TEST

