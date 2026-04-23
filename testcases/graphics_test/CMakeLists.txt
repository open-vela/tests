if(CONFIG_GRAPHICS_TEST)

    set(TESTDIR ${NUTTX_APPS_DIR}/tests/testcases/graphics_test)

    include_directories(${TESTDIR})

    if(CONFIG_VIDEO_WIDGET_TEST)
        include_directories(
            ${TESTDIR}/video_widget_test/include)
        file(GLOB VIDEO_WIDGET_TEST_CSRCS "${CMAKE_CURRENT_LIST_DIR}/video_widget_test/*.c")
        list(APPEND CSRCS ${VIDEO_WIDGET_TEST_CSRCS})
    endif()

    if(CONFIG_ANIMATION_ENGINE_TEST)
        include_directories(
            ${NUTTX_APPS_DIR}/netutils/cjson/cJSON
            ${NUTTX_APPS_DIR}/frameworks/graphics/animengine/include
            ${NUTTX_APPS_DIR}/tests/testcases/graphics_test/animengine_test)
        file(GLOB ANIMENGINE_TEST_CSRCS "${CMAKE_CURRENT_LIST_DIR}/animengine_test/*.c")
        list(APPEND CSRCS ${ANIMENGINE_TEST_CSRCS})
    endif()

    if(CONFIG_ANIMATION_ENGINE_CTEST)
        include_directories(
            ${NUTTX_APPS_DIR}/frameworks/graphics/animengine/include
            ${NUTTX_APPS_DIR}/frameworks/graphics/animengine/include/animengine
            ${NUTTX_APPS_DIR}/graphics/animengine/lvgl
            ${NUTTX_APPS_DIR}/tests/testcases/graphics_test/animengine_ctest)
        file(GLOB ANIMENGINE_CTEST_CSRCS "${CMAKE_CURRENT_LIST_DIR}/animengine_ctest/*.c")
        list(APPEND CSRCS ${ANIMENGINE_CTEST_CSRCS})
    endif()

    if(CONFIG_ANIM_PHYSICS_ENGINE_TEST)
        include_directories(
            ${NUTTX_APPS_DIR}/frameworks/graphics/animengine/include
            ${NUTTX_APPS_DIR}/frameworks/graphics/animengine/src
            ${NUTTX_APPS_DIR}/graphics/animengine/lvgl
            ${NUTTX_APPS_DIR}/tests/testcases/graphics_test/physics_engine_test)
        file(GLOB PHYSICS_ENGINE_TEST_CSRCS "${CMAKE_CURRENT_LIST_DIR}/physics_engine_test/*.c")
        list(APPEND CSRCS ${PHYSICS_ENGINE_TEST_CSRCS})
    endif()

    if(CONFIG_RIVE_TEST)
        include_directories(
            ${NUTTX_APPS_DIR}/frameworks/graphics/animengine/include
            ${NUTTX_APPS_DIR}/frameworks/graphics/animengine/src
            ${NUTTX_APPS_DIR}/graphics/animengine/lvgl
            ${NUTTX_APPS_DIR}/tests/testcases/graphics_test/rive_test)
        file(GLOB RIVE_TEST_CSRCS "${CMAKE_CURRENT_LIST_DIR}/rive_test/*.c")
        list(APPEND CSRCS ${RIVE_TEST_CSRCS})
    endif()

    if(CONFIG_GESTURES_TEST)
        include_directories(
            ${NUTTX_APPS_DIR}/graphics/lvgl/lvgl
            ${NUTTX_APPS_DIR}/tests/testcases/graphics_test/lv_gestures_test)
        file(GLOB GESTURES_TEST_CSRCS "${CMAKE_CURRENT_LIST_DIR}/lv_gestures_test/*.c")
        list(APPEND CSRCS ${GESTURES_TEST_CSRCS})
    endif()

    if(CONFIG_LV_BLUR_TEST)
        include_directories(
            ${NUTTX_APPS_DIR}/graphics/lvgl/lvgl
            ${NUTTX_APPS_DIR}/tests/testcases/graphics_test/lv_blur_test)
        file(GLOB LV_BLUR_TEST_CSRCS "${CMAKE_CURRENT_LIST_DIR}/lv_blur_test/*.c")
        list(APPEND CSRCS ${LV_BLUR_TEST_CSRCS})
    endif()

    nuttx_add_application(
        NAME graphics_test
        SRCS graphics_test_main.c ${CSRCS}
        PRIORITY ${CONFIG_GRAPHICS_TEST_PRIORITY}
        STACKSIZE ${CONFIG_GRAPHICS_TEST_STACKSIZE})

endif()
