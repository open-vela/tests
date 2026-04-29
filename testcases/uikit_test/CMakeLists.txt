if(CONFIG_UIKIT_TEST)

    set(TESTDIR ${NUTTX_APPS_DIR}/tests/testcases/uikit_test)

    include_directories(${TESTDIR})

    if(CONFIG_MARKDOWN_TEST)
        include_directories(
            ${NUTTX_APPS_DIR}/frameworks/graphics/uikit/include
            ${NUTTX_APPS_DIR}/frameworks/graphics/uikit/src
            ${NUTTX_APPS_DIR}/frameworks/graphics/uikit/tests
            ${NUTTX_APPS_DIR}/external/cmark-gfm/
            ${NUTTX_APPS_DIR}/external/cmark-gfm/cmark-gfm/src
            ${NUTTX_APPS_DIR}/external/cmark-gfm/cmark-gfm/extensions
            ${NUTTX_APPS_DIR}/tests/testcases/uikit_test/markdown)
        file(GLOB MARKDOWN_TEST_CSRCS "${CMAKE_CURRENT_LIST_DIR}/markdown/*.c")
        list(APPEND CSRCS ${MARKDOWN_TEST_CSRCS})
    endif()

    if(CONFIG_SVG_TEST)
        file(GLOB SVG_TEST_CSRCS "${CMAKE_CURRENT_LIST_DIR}/svg/*.c")
        list(APPEND CSRCS ${SVG_TEST_CSRCS})
    endif()

    nuttx_add_application(
        NAME uikit_test
        SRCS uikit_test.c ${CSRCS}
        PRIORITY ${CONFIG_UIKIT_TEST_PRIORITY}
        STACKSIZE ${CONFIG_UIKIT_TEST_STACKSIZE})

endif()
