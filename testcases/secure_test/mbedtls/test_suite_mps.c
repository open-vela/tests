#line 2 "suites/main_test.function"
/*
 * *** THIS FILE HAS BEEN MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py
 *
 * Test file      : ./test_suite_mps.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_mps.function
 *      Test suite data     : suites/test_suite_mps.data
 *
 */

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#if !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200112L // for fileno() from <stdio.h>
#endif
#endif

#include "mbedtls/build_info.h"

/* Test code may use deprecated identifiers only if the preprocessor symbol
 * MBEDTLS_TEST_DEPRECATED is defined. When building tests, set
 * MBEDTLS_TEST_DEPRECATED explicitly if MBEDTLS_DEPRECATED_WARNING is
 * enabled but the corresponding warnings are not treated as errors.
 */
#if !defined(MBEDTLS_DEPRECATED_REMOVED) && !defined(MBEDTLS_DEPRECATED_WARNING)
#define MBEDTLS_TEST_DEPRECATED
#endif

/*----------------------------------------------------------------------------*/
/* Common helper code */

#line 2 "suites/helpers.function"
/*----------------------------------------------------------------------------*/
/* Headers */

#include <test/helpers.h>
#include <test/macros.h>
#include <test/random.h>
#include <test/bignum_helpers.h>
#include <test/psa_crypto_helpers.h>

#include <stdlib.h>

#if defined(MBEDTLS_ERROR_C)
#include "mbedtls/error.h"
#endif
#include "mbedtls/platform.h"

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
#include "mbedtls/memory_buffer_alloc.h"
#endif

#ifdef _MSC_VER
#include <basetsd.h>
typedef UINT8 uint8_t;
typedef INT32 int32_t;
typedef UINT32 uint32_t;
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#else
#include <stdint.h>
#endif

#include <string.h>

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__)) || defined(__MINGW32__)
#include <strings.h>
#endif

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#endif

/*----------------------------------------------------------------------------*/
/* Status and error constants */

#define DEPENDENCY_SUPPORTED            0   /* Dependency supported by build */
#define KEY_VALUE_MAPPING_FOUND         0   /* Integer expression found */
#define DISPATCH_TEST_SUCCESS           0   /* Test dispatch successful */

#define KEY_VALUE_MAPPING_NOT_FOUND     -1  /* Integer expression not found */
#define DEPENDENCY_NOT_SUPPORTED        -2  /* Dependency not supported */
#define DISPATCH_TEST_FN_NOT_FOUND      -3  /* Test function not found */
#define DISPATCH_INVALID_TEST_DATA      -4  /* Invalid test parameter type.
                                               Only int, string, binary data
                                               and integer expressions are
                                               allowed */
#define DISPATCH_UNSUPPORTED_SUITE      -5  /* Test suite not supported by the
                                               build */

/*----------------------------------------------------------------------------*/
/* Global variables */

/*----------------------------------------------------------------------------*/
/* Helper flags for complex dependencies */

/* Indicates whether we expect mbedtls_entropy_init
 * to initialize some strong entropy source. */
#if !defined(MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES) && \
    (!defined(MBEDTLS_NO_PLATFORM_ENTROPY) ||      \
    defined(MBEDTLS_ENTROPY_HARDWARE_ALT) ||    \
    defined(ENTROPY_NV_SEED))
#define ENTROPY_HAVE_STRONG
#endif


/*----------------------------------------------------------------------------*/
/* Helper Functions */

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
static int redirect_output(FILE *out_stream, const char *path)
{
    int out_fd, dup_fd;
    FILE *path_stream;

    out_fd = fileno(out_stream);
    dup_fd = dup(out_fd);

    if (dup_fd == -1) {
        return -1;
    }

    path_stream = fopen(path, "w");
    if (path_stream == NULL) {
        close(dup_fd);
        return -1;
    }

    fflush(out_stream);
    if (dup2(fileno(path_stream), out_fd) == -1) {
        close(dup_fd);
        fclose(path_stream);
        return -1;
    }

    fclose(path_stream);
    return dup_fd;
}

static int restore_output(FILE *out_stream, int dup_fd)
{
    int out_fd = fileno(out_stream);

    fflush(out_stream);
    if (dup2(dup_fd, out_fd) == -1) {
        close(out_fd);
        close(dup_fd);
        return -1;
    }

    close(dup_fd);
    return 0;
}
#endif /* __unix__ || __APPLE__ __MACH__ */


#line 43 "suites/main_test.function"


/*----------------------------------------------------------------------------*/
/* Test Suite Code */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
#line 2 "suites/test_suite_mps.function"

#include <stdlib.h>

#include "mps_reader.h"

/*
 * Compile-time configuration for test suite.
 */

/* Comment/Uncomment this to disable/enable the
 * testing of the various MPS layers.
 * This can be useful for time-consuming instrumentation
 * tasks such as the conversion of E-ACSL annotations
 * into runtime assertions. */
#define TEST_SUITE_MPS_READER

/* End of compile-time configuration. */

#if defined(TEST_SUITE_MPS_READER)
#line 28 "suites/test_suite_mps.function"
static void test_mbedtls_mps_reader_no_pausing_single_step_single_round(int with_acc)
{
    /* This test exercises the most basic use of the MPS reader:
     * - The 'producing' layer provides a buffer
     * - The 'consuming' layer fetches it in a single go.
     * - After processing, the consuming layer commits the data
     *   and the reader is moved back to producing mode.
     *
     * Parameters:
     * - with_acc: 0 if the reader should be initialized without accumulator.
     *             1 if the reader should be initialized with accumulator.
     *
     *             Whether the accumulator is present or not should not matter,
     *             since the consumer's request can be fulfilled from the data
     *             that the producer has provided.
     */
    unsigned char bufA[100];
    unsigned char acc[10];
    unsigned char *tmp;
    int paused;
    mbedtls_mps_reader rd;
    for (size_t i = 0; (unsigned) i < sizeof(bufA); i++) {
        bufA[i] = (unsigned char) i;
    }

    /* Preparation (lower layer) */
    if (with_acc == 0) {
        mbedtls_mps_reader_init(&rd, NULL, 0);
    } else {
        mbedtls_mps_reader_init(&rd, acc, sizeof(acc));
    }
    TEST_ASSERT(mbedtls_mps_reader_feed(&rd, bufA, sizeof(bufA)) == 0);
    /* Consumption (upper layer) */
    /* Consume exactly what's available */
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 100, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 100, bufA, 100);
    TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
    /* Wrapup (lower layer) */
    TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, &paused) == 0);
    TEST_ASSERT(paused == 0);
    mbedtls_mps_reader_free(&rd);
exit:
    ;
}

static void test_mbedtls_mps_reader_no_pausing_single_step_single_round_wrapper( void ** params )
{

    test_mbedtls_mps_reader_no_pausing_single_step_single_round( *( (int *) params[0] ) );
}
#endif /* TEST_SUITE_MPS_READER */
#if defined(TEST_SUITE_MPS_READER)
#line 73 "suites/test_suite_mps.function"
static void test_mbedtls_mps_reader_no_pausing_single_step_multiple_rounds(int with_acc)
{
    /* This test exercises multiple rounds of the basic use of the MPS reader:
     * - The 'producing' layer provides a buffer
     * - The 'consuming' layer fetches it in a single go.
     * - After processing, the consuming layer commits the data
     *   and the reader is moved back to producing mode.
     *
     * Parameters:
     * - with_acc: 0 if the reader should be initialized without accumulator.
     *             1 if the reader should be initialized with accumulator.
     *
     *             Whether the accumulator is present or not should not matter,
     *             since the consumer's request can be fulfilled from the data
     *             that the producer has provided.
     */

    unsigned char bufA[100], bufB[100];
    unsigned char acc[10];
    unsigned char *tmp;
    mbedtls_mps_reader rd;
    for (size_t i = 0; (unsigned) i < sizeof(bufA); i++) {
        bufA[i] = (unsigned char) i;
    }
    for (size_t i = 0; (unsigned) i < sizeof(bufB); i++) {
        bufB[i] = ~((unsigned char) i);
    }

    /* Preparation (lower layer) */
    if (with_acc == 0) {
        mbedtls_mps_reader_init(&rd, NULL, 0);
    } else {
        mbedtls_mps_reader_init(&rd, acc, sizeof(acc));
    }
    TEST_ASSERT(mbedtls_mps_reader_feed(&rd, bufA, sizeof(bufA)) == 0);
    /* Consumption (upper layer) */
    /* Consume exactly what's available */
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 100, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 100, bufA, 100);
    TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
    /* Preparation */
    TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) == 0);
    TEST_ASSERT(mbedtls_mps_reader_feed(&rd, bufB, sizeof(bufB)) == 0);
    /* Consumption */
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 100, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 100, bufB, 100);
    TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
    /* Wrapup (lower layer) */
    TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) == 0);
    mbedtls_mps_reader_free(&rd);
exit:
    ;
}

static void test_mbedtls_mps_reader_no_pausing_single_step_multiple_rounds_wrapper( void ** params )
{

    test_mbedtls_mps_reader_no_pausing_single_step_multiple_rounds( *( (int *) params[0] ) );
}
#endif /* TEST_SUITE_MPS_READER */
#if defined(TEST_SUITE_MPS_READER)
#line 127 "suites/test_suite_mps.function"
static void test_mbedtls_mps_reader_no_pausing_multiple_steps_single_round(int with_acc)
{
    /* This test exercises one round of the following:
     * - The 'producing' layer provides a buffer
     * - The 'consuming' layer fetches it in multiple calls
     *   to `mbedtls_mps_reader_get()`, without committing in between.
     * - After processing, the consuming layer commits the data
     *   and the reader is moved back to producing mode.
     *
     * Parameters:
     * - with_acc: 0 if the reader should be initialized without accumulator.
     *             1 if the reader should be initialized with accumulator.
     *
     *             Whether the accumulator is present or not should not matter,
     *             since the consumer's requests can be fulfilled from the data
     *             that the producer has provided.
     */

    /* Lower layer provides data that the upper layer fully consumes
     * through multiple `get` calls. */
    unsigned char buf[100];
    unsigned char acc[10];
    unsigned char *tmp;
    mbedtls_mps_size_t tmp_len;
    mbedtls_mps_reader rd;
    for (size_t i = 0; (unsigned) i < sizeof(buf); i++) {
        buf[i] = (unsigned char) i;
    }

    /* Preparation (lower layer) */
    if (with_acc == 0) {
        mbedtls_mps_reader_init(&rd, NULL, 0);
    } else {
        mbedtls_mps_reader_init(&rd, acc, sizeof(acc));
    }
    TEST_ASSERT(mbedtls_mps_reader_feed(&rd, buf, sizeof(buf)) == 0);
    /* Consumption (upper layer) */
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 10, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 10, buf, 10);
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 70, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 70, buf + 10, 70);
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 30, &tmp, &tmp_len) == 0);
    ASSERT_COMPARE(tmp, tmp_len, buf + 80, 20);
    TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
    /* Wrapup (lower layer) */
    TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) == 0);
    mbedtls_mps_reader_free(&rd);
exit:
    ;
}

static void test_mbedtls_mps_reader_no_pausing_multiple_steps_single_round_wrapper( void ** params )
{

    test_mbedtls_mps_reader_no_pausing_multiple_steps_single_round( *( (int *) params[0] ) );
}
#endif /* TEST_SUITE_MPS_READER */
#if defined(TEST_SUITE_MPS_READER)
#line 178 "suites/test_suite_mps.function"
static void test_mbedtls_mps_reader_no_pausing_multiple_steps_multiple_rounds(int with_acc)
{
    /* This test exercises one round of fetching a buffer in multiple chunks
     * and passing it back to the producer afterwards, followed by another
     * single-step sequence of feed-fetch-commit-reclaim.
     */
    unsigned char bufA[100], bufB[100];
    unsigned char acc[10];
    unsigned char *tmp;
    mbedtls_mps_size_t tmp_len;
    mbedtls_mps_reader rd;
    for (size_t i = 0; (unsigned) i < sizeof(bufA); i++) {
        bufA[i] = (unsigned char) i;
    }
    for (size_t i = 0; (unsigned) i < sizeof(bufB); i++) {
        bufB[i] = ~((unsigned char) i);
    }

    /* Preparation (lower layer) */
    if (with_acc == 0) {
        mbedtls_mps_reader_init(&rd, NULL, 0);
    } else {
        mbedtls_mps_reader_init(&rd, acc, sizeof(acc));
    }
    TEST_ASSERT(mbedtls_mps_reader_feed(&rd, bufA, sizeof(bufA)) == 0);
    /* Consumption (upper layer) */
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 10, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 10, bufA, 10);
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 70, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 70, bufA + 10, 70);
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 30, &tmp, &tmp_len) == 0);
    ASSERT_COMPARE(tmp, tmp_len, bufA + 80, 20);
    TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
    /* Preparation */
    TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) == 0);
    TEST_ASSERT(mbedtls_mps_reader_feed(&rd, bufB, sizeof(bufB)) == 0);
    /* Consumption */
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 100, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 100, bufB, 100);
    TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
    /* Wrapup */
    TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) == 0);
    mbedtls_mps_reader_free(&rd);
exit:
    ;
}

static void test_mbedtls_mps_reader_no_pausing_multiple_steps_multiple_rounds_wrapper( void ** params )
{

    test_mbedtls_mps_reader_no_pausing_multiple_steps_multiple_rounds( *( (int *) params[0] ) );
}
#endif /* TEST_SUITE_MPS_READER */
#if defined(TEST_SUITE_MPS_READER)
#line 225 "suites/test_suite_mps.function"
static void test_mbedtls_mps_reader_pausing_needed_disabled(void)
{
    /* This test exercises the behaviour of the MPS reader when a read request
     * of the consumer exceeds what has been provided by the producer, and when
     * no accumulator is available in the reader.
     *
     * In this case, we expect the reader to fail.
     */

    unsigned char buf[100];
    unsigned char *tmp;
    mbedtls_mps_reader rd;
    for (size_t i = 0; (unsigned) i < sizeof(buf); i++) {
        buf[i] = (unsigned char) i;
    }

    /* Preparation (lower layer) */
    mbedtls_mps_reader_init(&rd, NULL, 0);
    TEST_ASSERT(mbedtls_mps_reader_feed(&rd, buf, sizeof(buf)) == 0);
    /* Consumption (upper layer) */
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 50, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 50, buf, 50);
    TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 100, &tmp, NULL) ==
                MBEDTLS_ERR_MPS_READER_OUT_OF_DATA);
    /* Wrapup (lower layer) */
    TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) ==
                MBEDTLS_ERR_MPS_READER_NEED_ACCUMULATOR);
    mbedtls_mps_reader_free(&rd);
exit:
    ;
}

static void test_mbedtls_mps_reader_pausing_needed_disabled_wrapper( void ** params )
{
    (void)params;

    test_mbedtls_mps_reader_pausing_needed_disabled(  );
}
#endif /* TEST_SUITE_MPS_READER */
#if defined(TEST_SUITE_MPS_READER)
#line 258 "suites/test_suite_mps.function"
static void test_mbedtls_mps_reader_pausing_needed_buffer_too_small(void)
{
    /* This test exercises the behaviour of the MPS reader with accumulator
     * in the situation where a read request goes beyond the bounds of the
     * current read buffer, _and_ the reader's accumulator is too small to
     * hold the requested amount of data.
     *
     * In this case, we expect mbedtls_mps_reader_reclaim() to fail,
     * but it should be possible to continue fetching data as if
     * there had been no excess request via mbedtls_mps_reader_get()
     * and the call to mbedtls_mps_reader_reclaim() had been rejected
     * because of data remaining.
     */

    unsigned char buf[100];
    unsigned char acc[10];
    unsigned char *tmp;
    mbedtls_mps_reader rd;
    mbedtls_mps_size_t tmp_len;

    for (size_t i = 0; (unsigned) i < sizeof(buf); i++) {
        buf[i] = (unsigned char) i;
    }

    /* Preparation (lower layer) */
    mbedtls_mps_reader_init(&rd, acc, sizeof(acc));
    TEST_ASSERT(mbedtls_mps_reader_feed(&rd, buf, sizeof(buf)) == 0);
    /* Consumption (upper layer) */
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 50, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 50, buf, 50);
    TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 10, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 10, buf + 50, 10);
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 100, &tmp, NULL) ==
                MBEDTLS_ERR_MPS_READER_OUT_OF_DATA);
    /* Wrapup (lower layer) */
    TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) ==
                MBEDTLS_ERR_MPS_READER_ACCUMULATOR_TOO_SMALL);

    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 50, &tmp, &tmp_len) == 0);
    ASSERT_COMPARE(tmp, tmp_len, buf + 50, 50);

    mbedtls_mps_reader_free(&rd);
exit:
    ;
}

static void test_mbedtls_mps_reader_pausing_needed_buffer_too_small_wrapper( void ** params )
{
    (void)params;

    test_mbedtls_mps_reader_pausing_needed_buffer_too_small(  );
}
#endif /* TEST_SUITE_MPS_READER */
#if defined(TEST_SUITE_MPS_READER)
#line 305 "suites/test_suite_mps.function"
static void test_mbedtls_mps_reader_reclaim_overflow(void)
{
    /* This test exercises the behaviour of the MPS reader with accumulator
     * in the situation where upon calling mbedtls_mps_reader_reclaim(), the
     * uncommitted data together with the excess data missing in the last
     * call to mbedtls_mps_reader_get() exceeds the bounds of the type
     * holding the buffer length.
     */

    unsigned char buf[100];
    unsigned char acc[50];
    unsigned char *tmp;
    mbedtls_mps_reader rd;

    for (size_t i = 0; (unsigned) i < sizeof(buf); i++) {
        buf[i] = (unsigned char) i;
    }

    /* Preparation (lower layer) */
    mbedtls_mps_reader_init(&rd, acc, sizeof(acc));
    TEST_ASSERT(mbedtls_mps_reader_feed(&rd, buf, sizeof(buf)) == 0);
    /* Consumption (upper layer) */
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 50, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 50, buf, 50);
    /* Excess request */
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, (mbedtls_mps_size_t) -1, &tmp, NULL) ==
                MBEDTLS_ERR_MPS_READER_OUT_OF_DATA);
    /* Wrapup (lower layer) */
    TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) ==
                MBEDTLS_ERR_MPS_READER_ACCUMULATOR_TOO_SMALL);

    mbedtls_mps_reader_free(&rd);
exit:
    ;
}

static void test_mbedtls_mps_reader_reclaim_overflow_wrapper( void ** params )
{
    (void)params;

    test_mbedtls_mps_reader_reclaim_overflow(  );
}
#endif /* TEST_SUITE_MPS_READER */
#if defined(TEST_SUITE_MPS_READER)
#line 341 "suites/test_suite_mps.function"
static void test_mbedtls_mps_reader_pausing(int option)
{
    /* This test exercises the behaviour of the reader when the
     * accumulator is used to fulfill a consumer's request.
     *
     * More detailed:
     * - The producer feeds some data.
     * - The consumer asks for more data than what's available.
     * - The reader remembers the request and goes back to
     *   producing mode, waiting for more data from the producer.
     * - The producer provides another chunk of data which is
     *   sufficient to fulfill the original read request.
     * - The consumer retries the original read request, which
     *   should now succeed.
     *
     * This test comes in multiple variants controlled by the
     * `option` parameter and documented below.
     */

    unsigned char bufA[100], bufB[100];
    unsigned char *tmp;
    unsigned char acc[40];
    int paused;
    mbedtls_mps_reader rd;
    for (size_t i = 0; (unsigned) i < sizeof(bufA); i++) {
        bufA[i] = (unsigned char) i;
    }
    for (size_t i = 0; (unsigned) i < sizeof(bufB); i++) {
        bufB[i] = ~((unsigned char) i);
    }

    /* Preparation (lower layer) */
    mbedtls_mps_reader_init(&rd, acc, sizeof(acc));
    TEST_ASSERT(mbedtls_mps_reader_feed(&rd, bufA, sizeof(bufA)) == 0);

    /* Consumption (upper layer) */
    /* Ask for more than what's available. */
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 80, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 80, bufA, 80);
    TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 10, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 10, bufA + 80, 10);
    switch (option) {
        case 0:  /* Single uncommitted fetch at pausing */
        case 1:
            TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
            break;
        default: /* Multiple uncommitted fetches at pausing */
            break;
    }
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 20, &tmp, NULL) ==
                MBEDTLS_ERR_MPS_READER_OUT_OF_DATA);

    /* Preparation */
    TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, &paused) == 0);
    TEST_ASSERT(paused == 1);
    TEST_ASSERT(mbedtls_mps_reader_feed(&rd, bufB, sizeof(bufB)) == 0);

    /* Consumption */
    switch (option) {
        case 0: /* Single fetch at pausing, re-fetch with commit. */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 20, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 10, bufA + 90, 10);
            ASSERT_COMPARE(tmp + 10, 10, bufB, 10);
            TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
            break;

        case 1: /* Single fetch at pausing, re-fetch without commit. */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 20, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 10, bufA + 90, 10);
            ASSERT_COMPARE(tmp + 10, 10, bufB, 10);
            break;

        case 2: /* Multiple fetches at pausing, repeat without commit. */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 10, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 10, bufA + 80, 10);
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 20, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 10, bufA + 90, 10);
            ASSERT_COMPARE(tmp + 10, 10, bufB, 10);
            break;

        case 3: /* Multiple fetches at pausing, repeat with commit 1. */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 10, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 10, bufA + 80, 10);
            TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 20, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 10, bufA + 90, 10);
            ASSERT_COMPARE(tmp + 10, 10, bufB, 10);
            break;

        case 4: /* Multiple fetches at pausing, repeat with commit 2. */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 10, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 10, bufA + 80, 10);
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 20, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 10, bufA + 90, 10);
            ASSERT_COMPARE(tmp + 10, 10, bufB, 10);
            TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
            break;

        case 5: /* Multiple fetches at pausing, repeat with commit 3. */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 10, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 10, bufA + 80, 10);
            TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 20, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 10, bufA + 90, 10);
            ASSERT_COMPARE(tmp + 10, 10, bufB, 10);
            TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
            break;

        default:
            TEST_ASSERT(0);
    }

    /* In all cases, fetch the rest of the second buffer. */
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 90, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 90, bufB + 10, 90);
    TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);

    /* Wrapup */
    TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) == 0);
    mbedtls_mps_reader_free(&rd);
exit:
    ;
}

static void test_mbedtls_mps_reader_pausing_wrapper( void ** params )
{

    test_mbedtls_mps_reader_pausing( *( (int *) params[0] ) );
}
#endif /* TEST_SUITE_MPS_READER */
#if defined(TEST_SUITE_MPS_READER)
#line 466 "suites/test_suite_mps.function"
static void test_mbedtls_mps_reader_pausing_multiple_feeds(int option)
{
    /* This test exercises the behaviour of the MPS reader
     * in the following situation:
     * - The consumer has asked for more than what's available, so the
     *   reader pauses and waits for further input data via
     *   `mbedtls_mps_reader_feed()`
     * - Multiple such calls to `mbedtls_mps_reader_feed()` are necessary
     *   to fulfill the original request, and the reader needs to do
     *   the necessary bookkeeping under the hood.
     *
     * This test comes in a few variants differing in the number and
     * size of feed calls that the producer issues while the reader is
     * accumulating the necessary data - see the comments below.
     */

    unsigned char bufA[100], bufB[100];
    unsigned char *tmp;
    unsigned char acc[70];
    mbedtls_mps_reader rd;
    mbedtls_mps_size_t fetch_len;
    for (size_t i = 0; (unsigned) i < sizeof(bufA); i++) {
        bufA[i] = (unsigned char) i;
    }
    for (size_t i = 0; (unsigned) i < sizeof(bufB); i++) {
        bufB[i] = ~((unsigned char) i);
    }

    /* Preparation (lower layer) */
    mbedtls_mps_reader_init(&rd, acc, sizeof(acc));
    TEST_ASSERT(mbedtls_mps_reader_feed(&rd, bufA, sizeof(bufA)) == 0);

    /* Consumption (upper layer) */
    /* Ask for more than what's available. */
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 80, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 80, bufA, 80);
    TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
    /* 20 left, ask for 70 -> 50 overhead */
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 70, &tmp, NULL) ==
                MBEDTLS_ERR_MPS_READER_OUT_OF_DATA);

    /* Preparation */
    TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) == 0);
    switch (option) {
        case 0: /* 10 + 10 + 80 byte feed */
            TEST_ASSERT(mbedtls_mps_reader_feed(&rd, bufB, 10) ==
                        MBEDTLS_ERR_MPS_READER_NEED_MORE);
            TEST_ASSERT(mbedtls_mps_reader_feed(&rd, bufB + 10, 10) ==
                        MBEDTLS_ERR_MPS_READER_NEED_MORE);
            TEST_ASSERT(mbedtls_mps_reader_feed(&rd, bufB + 20, 80) == 0);
            break;

        case 1: /* 50 x 1byte */
            for (size_t num_feed = 0; num_feed < 49; num_feed++) {
                TEST_ASSERT(mbedtls_mps_reader_feed(&rd, bufB + num_feed, 1) ==
                            MBEDTLS_ERR_MPS_READER_NEED_MORE);
            }
            TEST_ASSERT(mbedtls_mps_reader_feed(&rd, bufB + 49, 1) == 0);
            break;

        case 2: /* 49 x 1byte + 51bytes */
            for (size_t num_feed = 0; num_feed < 49; num_feed++) {
                TEST_ASSERT(mbedtls_mps_reader_feed(&rd, bufB + num_feed, 1) ==
                            MBEDTLS_ERR_MPS_READER_NEED_MORE);
            }
            TEST_ASSERT(mbedtls_mps_reader_feed(&rd, bufB + 49, 51) == 0);
            break;

        default:
            TEST_ASSERT(0);
            break;
    }

    /* Consumption */
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 70, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 20, bufA + 80, 20);
    ASSERT_COMPARE(tmp + 20, 50, bufB, 50);
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 1000, &tmp, &fetch_len) == 0);
    switch (option) {
        case 0:
            TEST_ASSERT(fetch_len == 50);
            break;

        case 1:
            TEST_ASSERT(fetch_len == 0);
            break;

        case 2:
            TEST_ASSERT(fetch_len == 50);
            break;

        default:
            TEST_ASSERT(0);
            break;
    }
    TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);

    /* Wrapup */
    TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) == 0);
    mbedtls_mps_reader_free(&rd);
exit:
    ;
}

static void test_mbedtls_mps_reader_pausing_multiple_feeds_wrapper( void ** params )
{

    test_mbedtls_mps_reader_pausing_multiple_feeds( *( (int *) params[0] ) );
}
#endif /* TEST_SUITE_MPS_READER */
#if defined(TEST_SUITE_MPS_READER)
#line 571 "suites/test_suite_mps.function"
static void test_mbedtls_mps_reader_reclaim_data_left(int option)
{
    /* This test exercises the behaviour of the MPS reader when a
     * call to mbedtls_mps_reader_reclaim() is made before all data
     * provided by the producer has been fetched and committed. */

    unsigned char buf[100];
    unsigned char *tmp;
    mbedtls_mps_reader rd;
    for (size_t i = 0; (unsigned) i < sizeof(buf); i++) {
        buf[i] = (unsigned char) i;
    }

    /* Preparation (lower layer) */
    mbedtls_mps_reader_init(&rd, NULL, 0);
    TEST_ASSERT(mbedtls_mps_reader_feed(&rd, buf, sizeof(buf)) == 0);

    /* Consumption (upper layer) */
    switch (option) {
        case 0:
            /* Fetch (but not commit) the entire buffer. */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, sizeof(buf), &tmp, NULL)
                        == 0);
            ASSERT_COMPARE(tmp, 100, buf, 100);
            break;

        case 1:
            /* Fetch (but not commit) parts of the buffer. */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, sizeof(buf) / 2,
                                               &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, sizeof(buf) / 2, buf, sizeof(buf) / 2);
            break;

        case 2:
            /* Fetch and commit parts of the buffer, then
             * fetch but not commit the rest of the buffer. */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, sizeof(buf) / 2,
                                               &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, sizeof(buf) / 2, buf, sizeof(buf) / 2);
            TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, sizeof(buf) / 2,
                                               &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, sizeof(buf) / 2,
                           buf + sizeof(buf) / 2,
                           sizeof(buf) / 2);
            break;

        default:
            TEST_ASSERT(0);
            break;
    }

    /* Wrapup */
    TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) ==
                MBEDTLS_ERR_MPS_READER_DATA_LEFT);
    mbedtls_mps_reader_free(&rd);
exit:
    ;
}

static void test_mbedtls_mps_reader_reclaim_data_left_wrapper( void ** params )
{

    test_mbedtls_mps_reader_reclaim_data_left( *( (int *) params[0] ) );
}
#endif /* TEST_SUITE_MPS_READER */
#if defined(TEST_SUITE_MPS_READER)
#line 631 "suites/test_suite_mps.function"
static void test_mbedtls_mps_reader_reclaim_data_left_retry(void)
{
    /* This test exercises the behaviour of the MPS reader when an attempt
     * by the producer to reclaim the reader fails because of more data pending
     * to be processed, and the consumer subsequently fetches more data. */
    unsigned char buf[100];
    unsigned char *tmp;
    mbedtls_mps_reader rd;

    for (size_t i = 0; (unsigned) i < sizeof(buf); i++) {
        buf[i] = (unsigned char) i;
    }

    /* Preparation (lower layer) */
    mbedtls_mps_reader_init(&rd, NULL, 0);
    TEST_ASSERT(mbedtls_mps_reader_feed(&rd, buf, sizeof(buf)) == 0);
    /* Consumption (upper layer) */
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 50, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 50, buf, 50);
    TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 50, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 50, buf + 50, 50);
    /* Preparation */
    TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) ==
                MBEDTLS_ERR_MPS_READER_DATA_LEFT);
    /* Consumption */
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 50, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 50, buf + 50, 50);
    TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
    /* Wrapup */
    TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) == 0);
    mbedtls_mps_reader_free(&rd);
exit:
    ;
}

static void test_mbedtls_mps_reader_reclaim_data_left_retry_wrapper( void ** params )
{
    (void)params;

    test_mbedtls_mps_reader_reclaim_data_left_retry(  );
}
#endif /* TEST_SUITE_MPS_READER */
#if defined(TEST_SUITE_MPS_READER)
#line 667 "suites/test_suite_mps.function"
static void test_mbedtls_mps_reader_multiple_pausing(int option)
{
    /* This test exercises the behaviour of the MPS reader
     * in the following situation:
     * - A read request via `mbedtls_mps_reader_get()` can't
     *   be served and the reader is paused to accumulate
     *   the desired amount of data from the producer.
     * - Once enough data is available, the consumer successfully
     *   reads the data from the reader, but afterwards exceeds
     *   the available data again - pausing is necessary for a
     *   second time.
     */

    unsigned char bufA[100], bufB[20], bufC[10];
    unsigned char *tmp;
    unsigned char acc[50];
    mbedtls_mps_size_t tmp_len;
    mbedtls_mps_reader rd;
    for (size_t i = 0; (unsigned) i < sizeof(bufA); i++) {
        bufA[i] = (unsigned char) i;
    }
    for (size_t i = 0; (unsigned) i < sizeof(bufB); i++) {
        bufB[i] = ~((unsigned char) i);
    }
    for (size_t i = 0; (unsigned) i < sizeof(bufC); i++) {
        bufC[i] = ~((unsigned char) i);
    }

    /* Preparation (lower layer) */
    mbedtls_mps_reader_init(&rd, acc, sizeof(acc));
    TEST_ASSERT(mbedtls_mps_reader_feed(&rd, bufA, sizeof(bufA)) == 0);

    /* Consumption (upper layer) */
    /* Ask for more than what's available. */
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 80, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 80, bufA, 80);
    TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 10, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 10, bufA + 80, 10);
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 20, &tmp, NULL) ==
                MBEDTLS_ERR_MPS_READER_OUT_OF_DATA);

    /* Preparation */
    TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) == 0);
    TEST_ASSERT(mbedtls_mps_reader_feed(&rd, bufB, sizeof(bufB)) == 0);

    switch (option) {
        case 0: /* Fetch same chunks, commit afterwards, and
                 * then exceed bounds of new buffer; accumulator
                 * large enough. */

            /* Consume */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 10, &tmp, &tmp_len) == 0);
            ASSERT_COMPARE(tmp, tmp_len, bufA + 80, 10);
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 20, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 10, bufA + 90, 10);
            ASSERT_COMPARE(tmp + 10, 10, bufB, 10);
            TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 20, &tmp, NULL) ==
                        MBEDTLS_ERR_MPS_READER_OUT_OF_DATA);

            /* Prepare */
            TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) == 0);
            TEST_ASSERT(mbedtls_mps_reader_feed(&rd, bufC, sizeof(bufC)) == 0);;

            /* Consume */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 20, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 10, bufB + 10, 10);
            ASSERT_COMPARE(tmp + 10, 10, bufC, 10);
            break;

        case 1: /* Fetch same chunks, commit afterwards, and
                 * then exceed bounds of new buffer; accumulator
                 * not large enough. */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 10, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 10, bufA + 80, 10);
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 20, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 10, bufA + 90, 10);
            ASSERT_COMPARE(tmp + 10, 10, bufB, 10);
            TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 51, &tmp, NULL) ==
                        MBEDTLS_ERR_MPS_READER_OUT_OF_DATA);

            /* Prepare */
            TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) ==
                        MBEDTLS_ERR_MPS_READER_ACCUMULATOR_TOO_SMALL);
            break;

        case 2: /* Fetch same chunks, don't commit afterwards, and
                 * then exceed bounds of new buffer; accumulator
                 * large enough. */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 10, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 10, bufA + 80, 10);
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 20, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 10, bufA + 90, 10);
            ASSERT_COMPARE(tmp + 10, 10, bufB, 10);
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 20, &tmp, NULL) ==
                        MBEDTLS_ERR_MPS_READER_OUT_OF_DATA);

            /* Prepare */
            TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) == 0);
            TEST_ASSERT(mbedtls_mps_reader_feed(&rd, bufC, sizeof(bufC)) == 0);;

            /* Consume */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 50, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 20, bufA + 80, 20);
            ASSERT_COMPARE(tmp + 20, 20, bufB, 20);
            ASSERT_COMPARE(tmp + 40, 10, bufC, 10);
            break;

        case 3: /* Fetch same chunks, don't commit afterwards, and
                 * then exceed bounds of new buffer; accumulator
                 * not large enough. */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 10, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 10, bufA + 80, 10);
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 20, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 10, bufA + 90, 10);
            ASSERT_COMPARE(tmp + 10, 10, bufB, 10);
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 21, &tmp, NULL) ==
                        MBEDTLS_ERR_MPS_READER_OUT_OF_DATA);

            /* Prepare */
            TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) ==
                        MBEDTLS_ERR_MPS_READER_ACCUMULATOR_TOO_SMALL);
            break;

        default:
            TEST_ASSERT(0);
            break;
    }

    mbedtls_mps_reader_free(&rd);
exit:
    ;
}

static void test_mbedtls_mps_reader_multiple_pausing_wrapper( void ** params )
{

    test_mbedtls_mps_reader_multiple_pausing( *( (int *) params[0] ) );
}
#endif /* TEST_SUITE_MPS_READER */
#if defined(TEST_SUITE_MPS_READER)
#if defined(MBEDTLS_MPS_STATE_VALIDATION)
#line 803 "suites/test_suite_mps.function"
static void test_mbedtls_mps_reader_random_usage(int num_out_chunks,
                                     int max_chunk_size,
                                     int max_request,
                                     int acc_size)

{
    /* Randomly pass a reader object back and forth between lower and
     * upper layer and let each of them call the respective reader API
     * functions in a random fashion.
     *
     * On the lower layer, we're tracking and concatenating
     * the data passed to successful feed calls.
     *
     * For the upper layer, we track and concatenate buffers
     * obtained from successful get calls.
     *
     * As long as the lower layer calls reclaim at least once, (resetting the
     * fetched but not-yet-committed data), this should always lead to the same
     * stream of outgoing/incoming data for the lower/upper layers, even if
     * most of the random calls fail.
     *
     * NOTE: This test uses rand() for random data, which is not optimal.
     *       Instead, it would be better to get the random data from a
     *       static buffer. This both eases reproducibility and allows
     *       simple conversion to a fuzz target.
     */
    int ret;
    unsigned char *acc = NULL;
    unsigned char *outgoing = NULL, *incoming = NULL;
    unsigned char *cur_chunk = NULL;
    size_t cur_out_chunk, out_pos, in_commit, in_fetch;
    int rand_op;  /* Lower layer:
                   * - Reclaim (0)
                   * - Feed (1)
                   * Upper layer:
                   * - Get, do tolerate smaller output (0)
                   * - Get, don't tolerate smaller output (1)
                   * - Commit (2) */
    int mode = 0; /* Lower layer (0) or Upper layer (1) */
    int reclaimed = 1; /* Have to call reclaim at least once before
                        * returning the reader to the upper layer. */
    mbedtls_mps_reader rd;

    if (acc_size > 0) {
        ASSERT_ALLOC(acc, acc_size);
    }

    /* This probably needs to be changed because we want
     * our tests to be deterministic. */
    //    srand( time( NULL ) );

    ASSERT_ALLOC(outgoing, num_out_chunks * max_chunk_size);
    ASSERT_ALLOC(incoming, num_out_chunks * max_chunk_size);

    mbedtls_mps_reader_init(&rd, acc, acc_size);

    cur_out_chunk = 0;
    in_commit = 0;
    in_fetch = 0;
    out_pos = 0;
    while (cur_out_chunk < (unsigned) num_out_chunks) {
        if (mode == 0) {
            /* Choose randomly between reclaim and feed */
            rand_op = rand() % 2;

            if (rand_op == 0) {
                /* Reclaim */
                ret = mbedtls_mps_reader_reclaim(&rd, NULL);

                if (ret == 0) {
                    TEST_ASSERT(cur_chunk != NULL);
                    mbedtls_free(cur_chunk);
                    cur_chunk = NULL;
                }
                reclaimed = 1;
            } else {
                /* Feed reader with a random chunk */
                unsigned char *tmp = NULL;
                size_t tmp_size;
                if (cur_out_chunk == (unsigned) num_out_chunks) {
                    continue;
                }

                tmp_size = (rand() % max_chunk_size) + 1;
                ASSERT_ALLOC(tmp, tmp_size);

                TEST_ASSERT(mbedtls_test_rnd_std_rand(NULL, tmp, tmp_size) == 0);
                ret = mbedtls_mps_reader_feed(&rd, tmp, tmp_size);

                if (ret == 0 || ret == MBEDTLS_ERR_MPS_READER_NEED_MORE) {
                    cur_out_chunk++;
                    memcpy(outgoing + out_pos, tmp, tmp_size);
                    out_pos += tmp_size;
                }

                if (ret == 0) {
                    TEST_ASSERT(cur_chunk == NULL);
                    cur_chunk = tmp;
                } else {
                    mbedtls_free(tmp);
                }

            }

            /* Randomly switch to consumption mode if reclaim
             * was called at least once. */
            if (reclaimed == 1 && rand() % 3 == 0) {
                in_fetch = 0;
                mode = 1;
            }
        } else {
            /* Choose randomly between get tolerating fewer data,
             * get not tolerating fewer data, and commit. */
            rand_op = rand() % 3;
            if (rand_op == 0 || rand_op == 1) {
                mbedtls_mps_size_t get_size, real_size;
                unsigned char *chunk_get;
                get_size = (rand() % max_request) + 1;
                if (rand_op == 0) {
                    ret = mbedtls_mps_reader_get(&rd, get_size, &chunk_get,
                                                 &real_size);
                } else {
                    real_size = get_size;
                    ret = mbedtls_mps_reader_get(&rd, get_size, &chunk_get, NULL);
                }

                /* Check if output is in accordance with what was written */
                if (ret == 0) {
                    memcpy(incoming + in_commit + in_fetch,
                           chunk_get, real_size);
                    TEST_ASSERT(memcmp(incoming + in_commit + in_fetch,
                                       outgoing + in_commit + in_fetch,
                                       real_size) == 0);
                    in_fetch += real_size;
                }
            } else if (rand_op == 2) { /* Commit */
                ret = mbedtls_mps_reader_commit(&rd);
                if (ret == 0) {
                    in_commit += in_fetch;
                    in_fetch = 0;
                }
            }

            /* Randomly switch back to preparation */
            if (rand() % 3 == 0) {
                reclaimed = 0;
                mode = 0;
            }
        }
    }

    /* Cleanup */
    mbedtls_mps_reader_free(&rd);
    mbedtls_free(incoming);
    mbedtls_free(outgoing);
    mbedtls_free(acc);
    mbedtls_free(cur_chunk);
exit:
    ;
}

static void test_mbedtls_mps_reader_random_usage_wrapper( void ** params )
{

    test_mbedtls_mps_reader_random_usage( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ) );
}
#endif /* MBEDTLS_MPS_STATE_VALIDATION */
#endif /* TEST_SUITE_MPS_READER */
#if defined(TEST_SUITE_MPS_READER)
#line 964 "suites/test_suite_mps.function"
static void test_mbedtls_reader_inconsistent_usage(int option)
{
    /* This test exercises the behaviour of the MPS reader
     * in the following situation:
     * - The consumer asks for more data than what's available
     * - The reader is paused and receives more data from the
     *   producer until the original read request can be fulfilled.
     * - The consumer does not repeat the original request but
     *   requests data in a different way.
     *
     * The reader does not guarantee that inconsistent read requests
     * after pausing will succeed, and this test triggers some cases
     * where the request fails.
     */

    unsigned char bufA[100], bufB[100];
    unsigned char *tmp;
    unsigned char acc[40];
    mbedtls_mps_reader rd;
    int success = 0;
    for (size_t i = 0; (unsigned) i < sizeof(bufA); i++) {
        bufA[i] = (unsigned char) i;
    }
    for (size_t i = 0; (unsigned) i < sizeof(bufB); i++) {
        bufB[i] = ~((unsigned char) i);
    }

    /* Preparation (lower layer) */
    mbedtls_mps_reader_init(&rd, acc, sizeof(acc));
    TEST_ASSERT(mbedtls_mps_reader_feed(&rd, bufA, sizeof(bufA)) == 0);
    /* Consumption (upper layer) */
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 80, &tmp, NULL) == 0);
    TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 10, &tmp, NULL) == 0);
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 20, &tmp, NULL) ==
                MBEDTLS_ERR_MPS_READER_OUT_OF_DATA);
    /* Preparation */
    TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) == 0);
    TEST_ASSERT(mbedtls_mps_reader_feed(&rd, bufB, sizeof(bufB)) == 0);
    /* Consumption */
    switch (option) {
        case 0:
            /* Ask for buffered data in a single chunk, no commit */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 30, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 20, bufA + 80, 20);
            ASSERT_COMPARE(tmp + 20, 10, bufB, 10);
            success = 1;
            break;

        case 1:
            /* Ask for buffered data in a single chunk, with commit */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 30, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 20, bufA + 80, 20);
            ASSERT_COMPARE(tmp + 20, 10, bufB, 10);
            TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
            success = 1;
            break;

        case 2:
            /* Ask for more than was requested when pausing, #1 */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 31, &tmp, NULL) ==
                        MBEDTLS_ERR_MPS_READER_INCONSISTENT_REQUESTS);
            break;

        case 3:
            /* Ask for more than was requested when pausing #2 */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, (mbedtls_mps_size_t) -1, &tmp, NULL) ==
                        MBEDTLS_ERR_MPS_READER_INCONSISTENT_REQUESTS);
            break;

        case 4:
            /* Asking for buffered data in different
             * chunks than before CAN fail. */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 15, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 15, bufA + 80, 15);
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 10, &tmp, NULL) ==
                        MBEDTLS_ERR_MPS_READER_INCONSISTENT_REQUESTS);
            break;

        case 5:
            /* Asking for buffered data different chunks
             * than before NEED NOT fail - no commits */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 15, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 15, bufA + 80, 15);
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 15, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 5, bufA + 95, 5);
            ASSERT_COMPARE(tmp + 5, 10, bufB, 10);
            success = 1;
            break;

        case 6:
            /* Asking for buffered data different chunks
             * than before NEED NOT fail - intermediate commit */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 15, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 15, bufA + 80, 15);
            TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 15, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 5, bufA + 95, 5);
            ASSERT_COMPARE(tmp + 5, 10, bufB, 10);
            success = 1;
            break;

        case 7:
            /* Asking for buffered data different chunks
             * than before NEED NOT fail - end commit */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 15, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 15, bufA + 80, 15);
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 15, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 5, bufA + 95, 5);
            ASSERT_COMPARE(tmp + 5, 10, bufB, 10);
            TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
            success = 1;
            break;

        case 8:
            /* Asking for buffered data different chunks
             * than before NEED NOT fail - intermediate & end commit */
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 15, &tmp, NULL) == 0);
            ASSERT_COMPARE(tmp, 15, bufA + 80, 15);
            TEST_ASSERT(mbedtls_mps_reader_get(&rd, 15, &tmp, NULL) == 0);
            TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
            ASSERT_COMPARE(tmp, 5, bufA + 95, 5);
            ASSERT_COMPARE(tmp + 5, 10, bufB, 10);
            TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);
            success = 1;
            break;

        default:
            TEST_ASSERT(0);
            break;
    }

    if (success == 1) {
        /* In all succeeding cases, fetch the rest of the second buffer. */
        TEST_ASSERT(mbedtls_mps_reader_get(&rd, 90, &tmp, NULL) == 0);
        ASSERT_COMPARE(tmp, 90, bufB + 10, 90);
        TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);

        /* Wrapup */
        TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) == 0);
    }

    /* Wrapup */
    mbedtls_mps_reader_free(&rd);
exit:
    ;
}

static void test_mbedtls_reader_inconsistent_usage_wrapper( void ** params )
{

    test_mbedtls_reader_inconsistent_usage( *( (int *) params[0] ) );
}
#endif /* TEST_SUITE_MPS_READER */
#if defined(TEST_SUITE_MPS_READER)
#line 1112 "suites/test_suite_mps.function"
static void test_mbedtls_mps_reader_feed_empty(void)
{
    /* This test exercises the behaviour of the reader when it is
     * fed with a NULL buffer. */
    unsigned char buf[100];
    unsigned char *tmp;
    mbedtls_mps_reader rd;
    for (size_t i = 0; (unsigned) i < sizeof(buf); i++) {
        buf[i] = (unsigned char) i;
    }

    /* Preparation (lower layer) */
    mbedtls_mps_reader_init(&rd, NULL, 0);

    TEST_ASSERT(mbedtls_mps_reader_feed(&rd, NULL, sizeof(buf)) ==
                MBEDTLS_ERR_MPS_READER_INVALID_ARG);

    /* Subsequent feed-calls should still succeed. */
    TEST_ASSERT(mbedtls_mps_reader_feed(&rd, buf, sizeof(buf)) == 0);

    /* Consumption (upper layer) */
    TEST_ASSERT(mbedtls_mps_reader_get(&rd, 100, &tmp, NULL) == 0);
    ASSERT_COMPARE(tmp, 100, buf, 100);
    TEST_ASSERT(mbedtls_mps_reader_commit(&rd) == 0);

    /* Wrapup */
    TEST_ASSERT(mbedtls_mps_reader_reclaim(&rd, NULL) == 0);
    mbedtls_mps_reader_free(&rd);
exit:
    ;
}

static void test_mbedtls_mps_reader_feed_empty_wrapper( void ** params )
{
    (void)params;

    test_mbedtls_mps_reader_feed_empty(  );
}
#endif /* TEST_SUITE_MPS_READER */
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */


#line 54 "suites/main_test.function"


/*----------------------------------------------------------------------------*/
/* Test dispatch code */


/**
 * \brief       Evaluates an expression/macro into its literal integer value.
 *              For optimizing space for embedded targets each expression/macro
 *              is identified by a unique identifier instead of string literals.
 *              Identifiers and evaluation code is generated by script:
 *              generate_test_code.py
 *
 * \param exp_id    Expression identifier.
 * \param out_value Pointer to int to hold the integer.
 *
 * \return       0 if exp_id is found. 1 otherwise.
 */
static int get_expression(int32_t exp_id, int32_t *out_value)
{
    int ret = KEY_VALUE_MAPPING_FOUND;

    (void) exp_id;
    (void) out_value;

    switch (exp_id) {
    
#if defined(MBEDTLS_SSL_PROTO_TLS1_3)

#endif

#line 82 "suites/main_test.function"
        default:
        {
            ret = KEY_VALUE_MAPPING_NOT_FOUND;
        }
        break;
    }
    return ret;
}


/**
 * \brief       Checks if the dependency i.e. the compile flag is set.
 *              For optimizing space for embedded targets each dependency
 *              is identified by a unique identifier instead of string literals.
 *              Identifiers and check code is generated by script:
 *              generate_test_code.py
 *
 * \param dep_id    Dependency identifier.
 *
 * \return       DEPENDENCY_SUPPORTED if set else DEPENDENCY_NOT_SUPPORTED
 */
static int dep_check(int dep_id)
{
    int ret = DEPENDENCY_NOT_SUPPORTED;

    (void) dep_id;

    switch (dep_id) {
    
#if defined(MBEDTLS_SSL_PROTO_TLS1_3)

#endif

#line 112 "suites/main_test.function"
        default:
            break;
    }
    return ret;
}


/**
 * \brief       Function pointer type for test function wrappers.
 *
 * A test function wrapper decodes the parameters and passes them to the
 * underlying test function. Both the wrapper and the underlying function
 * return void. Test wrappers assume that they are passed a suitable
 * parameter array and do not perform any error detection.
 *
 * \param param_array   The array of parameters. Each element is a `void *`
 *                      which the wrapper casts to the correct type and
 *                      dereferences. Each wrapper function hard-codes the
 *                      number and types of the parameters.
 */
typedef void (*TestWrapper_t)(void **param_array);


/**
 * \brief       Table of test function wrappers. Used by dispatch_test().
 *              This table is populated by script:
 *              generate_test_code.py
 *
 */
static TestWrapper_t test_funcs[] =
{
    /* Function Id: 0 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3) && defined(TEST_SUITE_MPS_READER)
    test_mbedtls_mps_reader_no_pausing_single_step_single_round_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3) && defined(TEST_SUITE_MPS_READER)
    test_mbedtls_mps_reader_no_pausing_single_step_multiple_rounds_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3) && defined(TEST_SUITE_MPS_READER)
    test_mbedtls_mps_reader_no_pausing_multiple_steps_single_round_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3) && defined(TEST_SUITE_MPS_READER)
    test_mbedtls_mps_reader_no_pausing_multiple_steps_multiple_rounds_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3) && defined(TEST_SUITE_MPS_READER)
    test_mbedtls_mps_reader_pausing_needed_disabled_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3) && defined(TEST_SUITE_MPS_READER)
    test_mbedtls_mps_reader_pausing_needed_buffer_too_small_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3) && defined(TEST_SUITE_MPS_READER)
    test_mbedtls_mps_reader_reclaim_overflow_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3) && defined(TEST_SUITE_MPS_READER)
    test_mbedtls_mps_reader_pausing_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3) && defined(TEST_SUITE_MPS_READER)
    test_mbedtls_mps_reader_pausing_multiple_feeds_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3) && defined(TEST_SUITE_MPS_READER)
    test_mbedtls_mps_reader_reclaim_data_left_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3) && defined(TEST_SUITE_MPS_READER)
    test_mbedtls_mps_reader_reclaim_data_left_retry_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3) && defined(TEST_SUITE_MPS_READER)
    test_mbedtls_mps_reader_multiple_pausing_wrapper,
#else
    NULL,
#endif
/* Function Id: 12 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3) && defined(TEST_SUITE_MPS_READER) && defined(MBEDTLS_MPS_STATE_VALIDATION)
    test_mbedtls_mps_reader_random_usage_wrapper,
#else
    NULL,
#endif
/* Function Id: 13 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3) && defined(TEST_SUITE_MPS_READER)
    test_mbedtls_reader_inconsistent_usage_wrapper,
#else
    NULL,
#endif
/* Function Id: 14 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3) && defined(TEST_SUITE_MPS_READER)
    test_mbedtls_mps_reader_feed_empty_wrapper,
#else
    NULL,
#endif

#line 145 "suites/main_test.function"
};

/**
 * \brief        Dispatches test functions based on function index.
 *
 * \param func_idx    Test function index.
 * \param params      The array of parameters to pass to the test function.
 *                    It will be decoded by the #TestWrapper_t wrapper function.
 *
 * \return       DISPATCH_TEST_SUCCESS if found
 *               DISPATCH_TEST_FN_NOT_FOUND if not found
 *               DISPATCH_UNSUPPORTED_SUITE if not compile time enabled.
 */
static int dispatch_test(size_t func_idx, void **params)
{
    int ret = DISPATCH_TEST_SUCCESS;
    TestWrapper_t fp = NULL;

    if (func_idx < (int) (sizeof(test_funcs) / sizeof(TestWrapper_t))) {
        fp = test_funcs[func_idx];
        if (fp) {
            #if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
            mbedtls_test_enable_insecure_external_rng();
            #endif

            fp(params);

            #if defined(MBEDTLS_TEST_MUTEX_USAGE)
            mbedtls_test_mutex_usage_check();
            #endif /* MBEDTLS_TEST_MUTEX_USAGE */
        } else {
            ret = DISPATCH_UNSUPPORTED_SUITE;
        }
    } else {
        ret = DISPATCH_TEST_FN_NOT_FOUND;
    }

    return ret;
}


/**
 * \brief       Checks if test function is supported in this build-time
 *              configuration.
 *
 * \param func_idx    Test function index.
 *
 * \return       DISPATCH_TEST_SUCCESS if found
 *               DISPATCH_TEST_FN_NOT_FOUND if not found
 *               DISPATCH_UNSUPPORTED_SUITE if not compile time enabled.
 */
static int check_test(size_t func_idx)
{
    int ret = DISPATCH_TEST_SUCCESS;
    TestWrapper_t fp = NULL;

    if (func_idx < (int) (sizeof(test_funcs)/sizeof(TestWrapper_t))) {
        fp = test_funcs[func_idx];
        if (fp == NULL) {
            ret = DISPATCH_UNSUPPORTED_SUITE;
        }
    } else {
        ret = DISPATCH_TEST_FN_NOT_FOUND;
    }

    return ret;
}


#line 2 "suites/host_test.function"

/**
 * \brief       Verifies that string is in string parameter format i.e. "<str>"
 *              It also strips enclosing '"' from the input string.
 *
 * \param str   String parameter.
 *
 * \return      0 if success else 1
 */
static int verify_string(char **str)
{
    if ((*str)[0] != '"' ||
        (*str)[strlen(*str) - 1] != '"') {
        mbedtls_fprintf(stderr,
                        "Expected string (with \"\") for parameter and got: %s\n", *str);
        return -1;
    }

    (*str)++;
    (*str)[strlen(*str) - 1] = '\0';

    return 0;
}

/**
 * \brief       Verifies that string is an integer. Also gives the converted
 *              integer value.
 *
 * \param str   Input string.
 * \param value Pointer to int for output value.
 *
 * \return      0 if success else 1
 */
static int verify_int(char *str, int32_t *value)
{
    size_t i;
    int minus = 0;
    int digits = 1;
    int hex = 0;

    for (i = 0; i < strlen(str); i++) {
        if (i == 0 && str[i] == '-') {
            minus = 1;
            continue;
        }

        if (((minus && i == 2) || (!minus && i == 1)) &&
            str[i - 1] == '0' && (str[i] == 'x' || str[i] == 'X')) {
            hex = 1;
            continue;
        }

        if (!((str[i] >= '0' && str[i] <= '9') ||
              (hex && ((str[i] >= 'a' && str[i] <= 'f') ||
                       (str[i] >= 'A' && str[i] <= 'F'))))) {
            digits = 0;
            break;
        }
    }

    if (digits) {
        if (hex) {
            *value = strtol(str, NULL, 16);
        } else {
            *value = strtol(str, NULL, 10);
        }

        return 0;
    }

    mbedtls_fprintf(stderr,
                    "Expected integer for parameter and got: %s\n", str);
    return KEY_VALUE_MAPPING_NOT_FOUND;
}


/**
 * \brief       Usage string.
 *
 */
#define USAGE \
    "Usage: %s [OPTIONS] files...\n\n" \
    "   Command line arguments:\n" \
    "     files...          One or more test data files. If no file is\n" \
    "                       specified the following default test case\n" \
    "                       file is used:\n" \
    "                           %s\n\n" \
    "   Options:\n" \
    "     -v | --verbose    Display full information about each test\n" \
    "     -h | --help       Display this information\n\n", \
    argv[0], \
    "TESTCASE_FILENAME"


/**
 * \brief       Read a line from the passed file pointer.
 *
 * \param f     FILE pointer
 * \param buf   Pointer to memory to hold read line.
 * \param len   Length of the buf.
 *
 * \return      0 if success else -1
 */
static int get_line(FILE *f, char *buf, size_t len)
{
    char *ret;
    int i = 0, str_len = 0, has_string = 0;

    /* Read until we get a valid line */
    do {
        ret = fgets(buf, len, f);
        if (ret == NULL) {
            return -1;
        }

        str_len = strlen(buf);

        /* Skip empty line and comment */
        if (str_len == 0 || buf[0] == '#') {
            continue;
        }
        has_string = 0;
        for (i = 0; i < str_len; i++) {
            char c = buf[i];
            if (c != ' ' && c != '\t' && c != '\n' &&
                c != '\v' && c != '\f' && c != '\r') {
                has_string = 1;
                break;
            }
        }
    } while (!has_string);

    /* Strip new line and carriage return */
    ret = buf + strlen(buf);
    if (ret-- > buf && *ret == '\n') {
        *ret = '\0';
    }
    if (ret-- > buf && *ret == '\r') {
        *ret = '\0';
    }

    return 0;
}

/**
 * \brief       Splits string delimited by ':'. Ignores '\:'.
 *
 * \param buf           Input string
 * \param len           Input string length
 * \param params        Out params found
 * \param params_len    Out params array len
 *
 * \return      Count of strings found.
 */
static int parse_arguments(char *buf, size_t len, char **params,
                           size_t params_len)
{
    size_t cnt = 0, i;
    char *cur = buf;
    char *p = buf, *q;

    params[cnt++] = cur;

    while (*p != '\0' && p < (buf + len)) {
        if (*p == '\\') {
            p++;
            p++;
            continue;
        }
        if (*p == ':') {
            if (p + 1 < buf + len) {
                cur = p + 1;
                TEST_HELPER_ASSERT(cnt < params_len);
                params[cnt++] = cur;
            }
            *p = '\0';
        }

        p++;
    }

    /* Replace newlines, question marks and colons in strings */
    for (i = 0; i < cnt; i++) {
        p = params[i];
        q = params[i];

        while (*p != '\0') {
            if (*p == '\\' && *(p + 1) == 'n') {
                p += 2;
                *(q++) = '\n';
            } else if (*p == '\\' && *(p + 1) == ':') {
                p += 2;
                *(q++) = ':';
            } else if (*p == '\\' && *(p + 1) == '?') {
                p += 2;
                *(q++) = '?';
            } else {
                *(q++) = *(p++);
            }
        }
        *q = '\0';
    }

    return cnt;
}

/**
 * \brief       Converts parameters into test function consumable parameters.
 *              Example: Input:  {"int", "0", "char*", "Hello",
 *                                "hex", "abef", "exp", "1"}
 *                      Output:  {
 *                                0,                // Verified int
 *                                "Hello",          // Verified string
 *                                2, { 0xab, 0xef },// Converted len,hex pair
 *                                9600              // Evaluated expression
 *                               }
 *
 *
 * \param cnt               Parameter array count.
 * \param params            Out array of found parameters.
 * \param int_params_store  Memory for storing processed integer parameters.
 *
 * \return      0 for success else 1
 */
static int convert_params(size_t cnt, char **params, int32_t *int_params_store)
{
    char **cur = params;
    char **out = params;
    int ret = DISPATCH_TEST_SUCCESS;

    while (cur < params + cnt) {
        char *type = *cur++;
        char *val = *cur++;

        if (strcmp(type, "char*") == 0) {
            if (verify_string(&val) == 0) {
                *out++ = val;
            } else {
                ret = (DISPATCH_INVALID_TEST_DATA);
                break;
            }
        } else if (strcmp(type, "int") == 0) {
            if (verify_int(val, int_params_store) == 0) {
                *out++ = (char *) int_params_store++;
            } else {
                ret = (DISPATCH_INVALID_TEST_DATA);
                break;
            }
        } else if (strcmp(type, "hex") == 0) {
            if (verify_string(&val) == 0) {
                size_t len;

                TEST_HELPER_ASSERT(
                    mbedtls_test_unhexify((unsigned char *) val, strlen(val),
                                          val, &len) == 0);

                *int_params_store = len;
                *out++ = val;
                *out++ = (char *) (int_params_store++);
            } else {
                ret = (DISPATCH_INVALID_TEST_DATA);
                break;
            }
        } else if (strcmp(type, "exp") == 0) {
            int exp_id = strtol(val, NULL, 10);
            if (get_expression(exp_id, int_params_store) == 0) {
                *out++ = (char *) int_params_store++;
            } else {
                ret = (DISPATCH_INVALID_TEST_DATA);
                break;
            }
        } else {
            ret = (DISPATCH_INVALID_TEST_DATA);
            break;
        }
    }
    return ret;
}

/**
 * \brief       Tests snprintf implementation with test input.
 *
 * \note
 * At high optimization levels (e.g. gcc -O3), this function may be
 * inlined in run_test_snprintf. This can trigger a spurious warning about
 * potential misuse of snprintf from gcc -Wformat-truncation (observed with
 * gcc 7.2). This warning makes tests in run_test_snprintf redundant on gcc
 * only. They are still valid for other compilers. Avoid this warning by
 * forbidding inlining of this function by gcc.
 *
 * \param n         Buffer test length.
 * \param ref_buf   Expected buffer.
 * \param ref_ret   Expected snprintf return value.
 *
 * \return      0 for success else 1
 */
#if defined(__GNUC__)
__attribute__((__noinline__))
#endif
static int test_snprintf(size_t n, const char *ref_buf, int ref_ret)
{
    int ret;
    char buf[10] = "xxxxxxxxx";
    const char ref[10] = "xxxxxxxxx";

    if (n >= sizeof(buf)) {
        return -1;
    }
    ret = mbedtls_snprintf(buf, n, "%s", "123");
    if (ret < 0 || (size_t) ret >= n) {
        ret = -1;
    }

    if (strncmp(ref_buf, buf, sizeof(buf)) != 0 ||
        ref_ret != ret ||
        memcmp(buf + n, ref + n, sizeof(buf) - n) != 0) {
        return 1;
    }

    return 0;
}

/**
 * \brief       Tests snprintf implementation.
 *
 * \return      0 for success else 1
 */
static int run_test_snprintf(void)
{
    return test_snprintf(0, "xxxxxxxxx",  -1) != 0 ||
           test_snprintf(1, "",           -1) != 0 ||
           test_snprintf(2, "1",          -1) != 0 ||
           test_snprintf(3, "12",         -1) != 0 ||
           test_snprintf(4, "123",         3) != 0 ||
           test_snprintf(5, "123",         3) != 0;
}

/** \brief Write the description of the test case to the outcome CSV file.
 *
 * \param outcome_file  The file to write to.
 *                      If this is \c NULL, this function does nothing.
 * \param argv0         The test suite name.
 * \param test_case     The test case description.
 */
static void write_outcome_entry(FILE *outcome_file,
                                const char *argv0,
                                const char *test_case)
{
    /* The non-varying fields are initialized on first use. */
    static const char *platform = NULL;
    static const char *configuration = NULL;
    static const char *test_suite = NULL;

    if (outcome_file == NULL) {
        return;
    }

    if (platform == NULL) {
        platform = getenv("MBEDTLS_TEST_PLATFORM");
        if (platform == NULL) {
            platform = "unknown";
        }
    }
    if (configuration == NULL) {
        configuration = getenv("MBEDTLS_TEST_CONFIGURATION");
        if (configuration == NULL) {
            configuration = "unknown";
        }
    }
    if (test_suite == NULL) {
        test_suite = strrchr(argv0, '/');
        if (test_suite != NULL) {
            test_suite += 1; // skip the '/'
        } else {
            test_suite = argv0;
        }
    }

    /* Write the beginning of the outcome line.
     * Ignore errors: writing the outcome file is on a best-effort basis. */
    mbedtls_fprintf(outcome_file, "%s;%s;%s;%s;",
                    platform, configuration, test_suite, test_case);
}

/** \brief Write the result of the test case to the outcome CSV file.
 *
 * \param outcome_file  The file to write to.
 *                      If this is \c NULL, this function does nothing.
 * \param unmet_dep_count            The number of unmet dependencies.
 * \param unmet_dependencies         The array of unmet dependencies.
 * \param missing_unmet_dependencies Non-zero if there was a problem tracking
 *                                   all unmet dependencies, 0 otherwise.
 * \param ret                        The test dispatch status (DISPATCH_xxx).
 * \param info                       A pointer to the test info structure.
 */
static void write_outcome_result(FILE *outcome_file,
                                 size_t unmet_dep_count,
                                 int unmet_dependencies[],
                                 int missing_unmet_dependencies,
                                 int ret,
                                 const mbedtls_test_info_t *info)
{
    if (outcome_file == NULL) {
        return;
    }

    /* Write the end of the outcome line.
     * Ignore errors: writing the outcome file is on a best-effort basis. */
    switch (ret) {
        case DISPATCH_TEST_SUCCESS:
            if (unmet_dep_count > 0) {
                size_t i;
                mbedtls_fprintf(outcome_file, "SKIP");
                for (i = 0; i < unmet_dep_count; i++) {
                    mbedtls_fprintf(outcome_file, "%c%d",
                                    i == 0 ? ';' : ':',
                                    unmet_dependencies[i]);
                }
                if (missing_unmet_dependencies) {
                    mbedtls_fprintf(outcome_file, ":...");
                }
                break;
            }
            switch (info->result) {
                case MBEDTLS_TEST_RESULT_SUCCESS:
                    mbedtls_fprintf(outcome_file, "PASS;");
                    break;
                case MBEDTLS_TEST_RESULT_SKIPPED:
                    mbedtls_fprintf(outcome_file, "SKIP;Runtime skip");
                    break;
                default:
                    mbedtls_fprintf(outcome_file, "FAIL;%s:%d:%s",
                                    info->filename, info->line_no,
                                    info->test);
                    break;
            }
            break;
        case DISPATCH_TEST_FN_NOT_FOUND:
            mbedtls_fprintf(outcome_file, "FAIL;Test function not found");
            break;
        case DISPATCH_INVALID_TEST_DATA:
            mbedtls_fprintf(outcome_file, "FAIL;Invalid test data");
            break;
        case DISPATCH_UNSUPPORTED_SUITE:
            mbedtls_fprintf(outcome_file, "SKIP;Unsupported suite");
            break;
        default:
            mbedtls_fprintf(outcome_file, "FAIL;Unknown cause");
            break;
    }
    mbedtls_fprintf(outcome_file, "\n");
    fflush(outcome_file);
}

/**
 * \brief       Desktop implementation of execute_tests().
 *              Parses command line and executes tests from
 *              supplied or default data file.
 *
 * \param argc  Command line argument count.
 * \param argv  Argument array.
 *
 * \return      Program exit status.
 */
static int execute_tests(int argc, const char **argv)
{
    /* Local Configurations and options */
    const char *default_filename = "./test_suite_mps.datax";
    const char *test_filename = NULL;
    const char **test_files = NULL;
    size_t testfile_count = 0;
    int option_verbose = 0;
    size_t function_id = 0;

    /* Other Local variables */
    int arg_index = 1;
    const char *next_arg;
    size_t testfile_index, i, cnt;
    int ret;
    unsigned total_errors = 0, total_tests = 0, total_skipped = 0;
    FILE *file;
    char buf[5000];
    char *params[50];
    /* Store for processed integer params. */
    int32_t int_params[50];
    void *pointer;
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
    int stdout_fd = -1;
#endif /* __unix__ || __APPLE__ __MACH__ */
    const char *outcome_file_name = getenv("MBEDTLS_TEST_OUTCOME_FILE");
    FILE *outcome_file = NULL;

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C) && \
    !defined(TEST_SUITE_MEMORY_BUFFER_ALLOC)
    unsigned char alloc_buf[1000000];
    mbedtls_memory_buffer_alloc_init(alloc_buf, sizeof(alloc_buf));
#endif

#if defined(MBEDTLS_TEST_MUTEX_USAGE)
    mbedtls_test_mutex_usage_init();
#endif

    /*
     * The C standard doesn't guarantee that all-bits-0 is the representation
     * of a NULL pointer. We do however use that in our code for initializing
     * structures, which should work on every modern platform. Let's be sure.
     */
    memset(&pointer, 0, sizeof(void *));
    if (pointer != NULL) {
        mbedtls_fprintf(stderr, "all-bits-zero is not a NULL pointer\n");
        return 1;
    }

    /*
     * Make sure we have a snprintf that correctly zero-terminates
     */
    if (run_test_snprintf() != 0) {
        mbedtls_fprintf(stderr, "the snprintf implementation is broken\n");
        return 1;
    }

    if (outcome_file_name != NULL && *outcome_file_name != '\0') {
        outcome_file = fopen(outcome_file_name, "a");
        if (outcome_file == NULL) {
            mbedtls_fprintf(stderr, "Unable to open outcome file. Continuing anyway.\n");
        }
    }

    while (arg_index < argc) {
        next_arg = argv[arg_index];

        if (strcmp(next_arg, "--verbose") == 0 ||
            strcmp(next_arg, "-v") == 0) {
            option_verbose = 1;
        } else if (strcmp(next_arg, "--help") == 0 ||
                   strcmp(next_arg, "-h") == 0) {
            mbedtls_fprintf(stdout, USAGE);
            mbedtls_exit(EXIT_SUCCESS);
        } else {
            /* Not an option, therefore treat all further arguments as the file
             * list.
             */
            test_files = &argv[arg_index];
            testfile_count = argc - arg_index;
            break;
        }

        arg_index++;
    }

    /* If no files were specified, assume a default */
    if (test_files == NULL || testfile_count == 0) {
        test_files = &default_filename;
        testfile_count = 1;
    }

    /* Initialize the struct that holds information about the last test */
    mbedtls_test_info_reset();

    /* Now begin to execute the tests in the testfiles */
    for (testfile_index = 0;
         testfile_index < testfile_count;
         testfile_index++) {
        size_t unmet_dep_count = 0;
        int unmet_dependencies[20];
        int missing_unmet_dependencies = 0;

        test_filename = test_files[testfile_index];

        file = fopen(test_filename, "r");
        if (file == NULL) {
            mbedtls_fprintf(stderr, "Failed to open test file: %s\n",
                            test_filename);
            if (outcome_file != NULL) {
                fclose(outcome_file);
            }
            return 1;
        }

        while (!feof(file)) {
            if (unmet_dep_count > 0) {
                mbedtls_fprintf(stderr,
                                "FATAL: Dep count larger than zero at start of loop\n");
                mbedtls_exit(MBEDTLS_EXIT_FAILURE);
            }
            unmet_dep_count = 0;
            missing_unmet_dependencies = 0;

            if ((ret = get_line(file, buf, sizeof(buf))) != 0) {
                break;
            }
            mbedtls_fprintf(stdout, "%s%.66s",
                            mbedtls_test_info.result == MBEDTLS_TEST_RESULT_FAILED ?
                            "\n" : "", buf);
            mbedtls_fprintf(stdout, " ");
            for (i = strlen(buf) + 1; i < 67; i++) {
                mbedtls_fprintf(stdout, ".");
            }
            mbedtls_fprintf(stdout, " ");
            fflush(stdout);
            write_outcome_entry(outcome_file, argv[0], buf);

            total_tests++;

            if ((ret = get_line(file, buf, sizeof(buf))) != 0) {
                break;
            }
            cnt = parse_arguments(buf, strlen(buf), params,
                                  sizeof(params) / sizeof(params[0]));

            if (strcmp(params[0], "depends_on") == 0) {
                for (i = 1; i < cnt; i++) {
                    int dep_id = strtol(params[i], NULL, 10);
                    if (dep_check(dep_id) != DEPENDENCY_SUPPORTED) {
                        if (unmet_dep_count <
                            ARRAY_LENGTH(unmet_dependencies)) {
                            unmet_dependencies[unmet_dep_count] = dep_id;
                            unmet_dep_count++;
                        } else {
                            missing_unmet_dependencies = 1;
                        }
                    }
                }

                if ((ret = get_line(file, buf, sizeof(buf))) != 0) {
                    break;
                }
                cnt = parse_arguments(buf, strlen(buf), params,
                                      sizeof(params) / sizeof(params[0]));
            }

            // If there are no unmet dependencies execute the test
            if (unmet_dep_count == 0) {
                mbedtls_test_info_reset();

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
                /* Suppress all output from the library unless we're verbose
                 * mode
                 */
                if (!option_verbose) {
                    stdout_fd = redirect_output(stdout, "/dev/null");
                    if (stdout_fd == -1) {
                        /* Redirection has failed with no stdout so exit */
                        exit(1);
                    }
                }
#endif /* __unix__ || __APPLE__ __MACH__ */

                function_id = strtoul(params[0], NULL, 10);
                if ((ret = check_test(function_id)) == DISPATCH_TEST_SUCCESS) {
                    ret = convert_params(cnt - 1, params + 1, int_params);
                    if (DISPATCH_TEST_SUCCESS == ret) {
                        ret = dispatch_test(function_id, (void **) (params + 1));
                    }
                }

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
                if (!option_verbose && restore_output(stdout, stdout_fd)) {
                    /* Redirection has failed with no stdout so exit */
                    exit(1);
                }
#endif /* __unix__ || __APPLE__ __MACH__ */

            }

            write_outcome_result(outcome_file,
                                 unmet_dep_count, unmet_dependencies,
                                 missing_unmet_dependencies,
                                 ret, &mbedtls_test_info);
            if (unmet_dep_count > 0 || ret == DISPATCH_UNSUPPORTED_SUITE) {
                total_skipped++;
                mbedtls_fprintf(stdout, "----");

                if (1 == option_verbose && ret == DISPATCH_UNSUPPORTED_SUITE) {
                    mbedtls_fprintf(stdout, "\n   Test Suite not enabled");
                }

                if (1 == option_verbose && unmet_dep_count > 0) {
                    mbedtls_fprintf(stdout, "\n   Unmet dependencies: ");
                    for (i = 0; i < unmet_dep_count; i++) {
                        mbedtls_fprintf(stdout, "%d ",
                                        unmet_dependencies[i]);
                    }
                    if (missing_unmet_dependencies) {
                        mbedtls_fprintf(stdout, "...");
                    }
                }
                mbedtls_fprintf(stdout, "\n");
                fflush(stdout);

                unmet_dep_count = 0;
                missing_unmet_dependencies = 0;
            } else if (ret == DISPATCH_TEST_SUCCESS) {
                if (mbedtls_test_info.result == MBEDTLS_TEST_RESULT_SUCCESS) {
                    mbedtls_fprintf(stdout, "PASS\n");
                } else if (mbedtls_test_info.result == MBEDTLS_TEST_RESULT_SKIPPED) {
                    mbedtls_fprintf(stdout, "----\n");
                    total_skipped++;
                } else {
                    total_errors++;
                    mbedtls_fprintf(stdout, "FAILED\n");
                    mbedtls_fprintf(stdout, "  %s\n  at ",
                                    mbedtls_test_info.test);
                    if (mbedtls_test_info.step != (unsigned long) (-1)) {
                        mbedtls_fprintf(stdout, "step %lu, ",
                                        mbedtls_test_info.step);
                    }
                    mbedtls_fprintf(stdout, "line %d, %s",
                                    mbedtls_test_info.line_no,
                                    mbedtls_test_info.filename);
                    if (mbedtls_test_info.line1[0] != 0) {
                        mbedtls_fprintf(stdout, "\n  %s",
                                        mbedtls_test_info.line1);
                    }
                    if (mbedtls_test_info.line2[0] != 0) {
                        mbedtls_fprintf(stdout, "\n  %s",
                                        mbedtls_test_info.line2);
                    }
                }
                fflush(stdout);
            } else if (ret == DISPATCH_INVALID_TEST_DATA) {
                mbedtls_fprintf(stderr, "FAILED: FATAL PARSE ERROR\n");
                fclose(file);
                mbedtls_exit(2);
            } else if (ret == DISPATCH_TEST_FN_NOT_FOUND) {
                mbedtls_fprintf(stderr, "FAILED: FATAL TEST FUNCTION NOT FOUND\n");
                fclose(file);
                mbedtls_exit(2);
            } else {
                total_errors++;
            }
        }
        fclose(file);
    }

    if (outcome_file != NULL) {
        fclose(outcome_file);
    }

    mbedtls_fprintf(stdout,
                    "\n----------------------------------------------------------------------------\n\n");
    if (total_errors == 0) {
        mbedtls_fprintf(stdout, "PASSED");
    } else {
        mbedtls_fprintf(stdout, "FAILED");
    }

    mbedtls_fprintf(stdout, " (%u / %u tests (%u skipped))\n",
                    total_tests - total_errors, total_tests, total_skipped);

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C) && \
    !defined(TEST_SUITE_MEMORY_BUFFER_ALLOC)
#if defined(MBEDTLS_MEMORY_DEBUG)
    mbedtls_memory_buffer_alloc_status();
#endif
    mbedtls_memory_buffer_alloc_free();
#endif

    return total_errors != 0;
}


#line 217 "suites/main_test.function"

/*----------------------------------------------------------------------------*/
/* Main Test code */


/**
 * \brief       Program main. Invokes platform specific execute_tests().
 *
 * \param argc      Command line arguments count.
 * \param argv      Array of command line arguments.
 *
 * \return       Exit code.
 */
int main(int argc, const char *argv[])
{
#if defined(MBEDTLS_TEST_HOOKS)
    extern void (*mbedtls_test_hook_test_fail)(const char *test, int line, const char *file);
    mbedtls_test_hook_test_fail = &mbedtls_test_fail;
#if defined(MBEDTLS_ERROR_C)
    mbedtls_test_hook_error_add = &mbedtls_test_err_add_check;
#endif
#endif

    int ret = mbedtls_test_platform_setup();
    if (ret != 0) {
        mbedtls_fprintf(stderr,
                        "FATAL: Failed to initialize platform - error %d\n",
                        ret);
        return -1;
    }

    ret = execute_tests(argc, argv);
    mbedtls_test_platform_teardown();
    return ret;
}
