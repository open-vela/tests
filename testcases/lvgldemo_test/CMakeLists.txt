if(CONFIG_LVGLDEMO_TEST)

    set(TESTDIR ${NUTTX_APPS_DIR}/tests/testcases/lvgldemo_test)

    include_directories(${TESTDIR})

    if(CONFIG_VECTOR_GRAPHIC_TEST)
        include_directories(
            ${NUTTX_APPS_DIR}/graphics/lvgl/lvgl
            ${NUTTX_APPS_DIR}/tests/testcases/lvgldemo_test
            ${NUTTX_APPS_DIR}/tests/testcases/lvgldemo_test/vector_graphic_test)
        file(GLOB_RECURSE VECTOR_GRAPHIC_TEST_CSRCS "${CMAKE_CURRENT_LIST_DIR}/vector_graphic_test/*.c")
        list(APPEND CSRCS ${VECTOR_GRAPHIC_TEST_CSRCS})
    endif()

    nuttx_add_application(
        NAME lvgldemo_test
        SRCS lvgldemo_test.c ${CSRCS}
        PRIORITY ${CONFIG_LVGLDEMO_TEST_PRIORITY}
        STACKSIZE ${CONFIG_LVGLDEMO_TEST_STACKSIZE})

endif()
