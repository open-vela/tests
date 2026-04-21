#line 2 "suites/main_test.function"
/*
 * *** THIS FILE HAS BEEN MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py
 *
 * Test file      : ./test_suite_asn1parse.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_asn1parse.function
 *      Test suite data     : suites/test_suite_asn1parse.data
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

#if defined(MBEDTLS_ASN1_PARSE_C)
#line 2 "suites/test_suite_asn1parse.function"
#include <errno.h>
#include <stdlib.h>
#include <limits.h>

#include "mbedtls/bignum.h"
#include "mbedtls/asn1.h"
#if defined(MBEDTLS_ASN1_WRITE_C)
#include "mbedtls/asn1write.h"
#endif

/* Used internally to report an error that indicates a bug in a parsing function. */
#define ERR_PARSE_INCONSISTENCY INT_MAX

/* Use this magic value in some tests to indicate that the expected result
 * should not be checked. */
#define UNPREDICTABLE_RESULT 0x5552

static int nested_parse(unsigned char **const p,
                        const unsigned char *const end)
{
    int ret;
    size_t len = 0;
    size_t len2 = 0;
    unsigned char *const start = *p;
    unsigned char *content_start;
    unsigned char tag;

    /* First get the length, skipping over the tag. */
    content_start = start + 1;
    ret = mbedtls_asn1_get_len(&content_start, end, &len);
    TEST_ASSERT(content_start <= end);
    if (ret != 0) {
        return ret;
    }

    /* Since we have a valid element start (tag and length), retrieve and
     * check the tag. */
    tag = start[0];
    TEST_EQUAL(mbedtls_asn1_get_tag(p, end, &len2, tag ^ 1),
               MBEDTLS_ERR_ASN1_UNEXPECTED_TAG);
    *p = start;
    TEST_EQUAL(mbedtls_asn1_get_tag(p, end, &len2, tag), 0);
    TEST_EQUAL(len, len2);
    TEST_ASSERT(*p == content_start);
    *p = content_start;

    switch (tag & 0x1f) {
        case MBEDTLS_ASN1_BOOLEAN:
        {
            int val = -257;
            *p = start;
            ret = mbedtls_asn1_get_bool(p, end, &val);
            if (ret == 0) {
                TEST_ASSERT(val == 0 || val == 1);
            }
            break;
        }

        case MBEDTLS_ASN1_INTEGER:
        {
#if defined(MBEDTLS_BIGNUM_C)
            mbedtls_mpi mpi;
            mbedtls_mpi_init(&mpi);
            *p = start;
            ret = mbedtls_asn1_get_mpi(p, end, &mpi);
            mbedtls_mpi_free(&mpi);
#else
            *p = start + 1;
            ret = mbedtls_asn1_get_len(p, end, &len);
            *p += len;
#endif
            /* If we're sure that the number fits in an int, also
             * call mbedtls_asn1_get_int(). */
            if (ret == 0 && len < sizeof(int)) {
                int val = -257;
                unsigned char *q = start;
                ret = mbedtls_asn1_get_int(&q, end, &val);
                TEST_ASSERT(*p == q);
            }
            break;
        }

        case MBEDTLS_ASN1_BIT_STRING:
        {
            mbedtls_asn1_bitstring bs;
            *p = start;
            ret = mbedtls_asn1_get_bitstring(p, end, &bs);
            break;
        }

        case MBEDTLS_ASN1_SEQUENCE:
        {
            while (*p <= end && *p < content_start + len && ret == 0) {
                ret = nested_parse(p, content_start + len);
            }
            break;
        }

        case MBEDTLS_ASN1_OCTET_STRING:
        case MBEDTLS_ASN1_NULL:
        case MBEDTLS_ASN1_OID:
        case MBEDTLS_ASN1_UTF8_STRING:
        case MBEDTLS_ASN1_SET:
        case MBEDTLS_ASN1_PRINTABLE_STRING:
        case MBEDTLS_ASN1_T61_STRING:
        case MBEDTLS_ASN1_IA5_STRING:
        case MBEDTLS_ASN1_UTC_TIME:
        case MBEDTLS_ASN1_GENERALIZED_TIME:
        case MBEDTLS_ASN1_UNIVERSAL_STRING:
        case MBEDTLS_ASN1_BMP_STRING:
        default:
            /* No further testing implemented for this tag. */
            *p += len;
            return 0;
    }

    TEST_ASSERT(*p <= end);
    return ret;

exit:
    return ERR_PARSE_INCONSISTENCY;
}

int get_len_step(const data_t *input, size_t buffer_size,
                 size_t actual_length)
{
    unsigned char *buf = NULL;
    unsigned char *p = NULL;
    unsigned char *end;
    size_t parsed_length;
    int ret;

    mbedtls_test_set_step(buffer_size);
    /* Allocate a new buffer of exactly the length to parse each time.
     * This gives memory sanitizers a chance to catch buffer overreads. */
    if (buffer_size == 0) {
        ASSERT_ALLOC(buf, 1);
        end = buf + 1;
        p = end;
    } else {
        ASSERT_ALLOC_WEAK(buf, buffer_size);
        if (buffer_size > input->len) {
            memcpy(buf, input->x, input->len);
            memset(buf + input->len, 'A', buffer_size - input->len);
        } else {
            memcpy(buf, input->x, buffer_size);
        }
        p = buf;
        end = buf + buffer_size;
    }

    ret = mbedtls_asn1_get_len(&p, end, &parsed_length);

    if (buffer_size >= input->len + actual_length) {
        TEST_EQUAL(ret, 0);
        TEST_ASSERT(p == buf + input->len);
        TEST_EQUAL(parsed_length, actual_length);
    } else {
        TEST_EQUAL(ret, MBEDTLS_ERR_ASN1_OUT_OF_DATA);
    }
    mbedtls_free(buf);
    return 1;

exit:
    mbedtls_free(buf);
    return 0;
}

typedef struct {
    const unsigned char *input_start;
    const char *description;
} traverse_state_t;

/* Value returned by traverse_callback if description runs out. */
#define RET_TRAVERSE_STOP 1
/* Value returned by traverse_callback if description has an invalid format
 * (see traverse_sequence_of). */
#define RET_TRAVERSE_ERROR 2


static int traverse_callback(void *ctx, int tag,
                             unsigned char *content, size_t len)
{
    traverse_state_t *state = ctx;
    size_t offset;
    const char *rest = state->description;
    unsigned long n;

    TEST_ASSERT(content > state->input_start);
    offset = content - state->input_start;
    mbedtls_test_set_step(offset);

    if (*rest == 0) {
        return RET_TRAVERSE_STOP;
    }
    n = strtoul(rest, (char **) &rest, 0);
    TEST_EQUAL(n, offset);
    TEST_EQUAL(*rest, ',');
    ++rest;
    n = strtoul(rest, (char **) &rest, 0);
    TEST_EQUAL(n, (unsigned) tag);
    TEST_EQUAL(*rest, ',');
    ++rest;
    n = strtoul(rest, (char **) &rest, 0);
    TEST_EQUAL(n, len);
    if (*rest == ',') {
        ++rest;
    }

    state->description = rest;
    return 0;

exit:
    return RET_TRAVERSE_ERROR;
}

#line 226 "suites/test_suite_asn1parse.function"
static void test_parse_prefixes(const data_t *input,
                    int full_result,
                    int overfull_result)
{
    /* full_result: expected result from parsing the given string. */
    /* overfull_result: expected_result from parsing the given string plus
     * some trailing garbage. This may be UNPREDICTABLE_RESULT to accept
     * any result: use this for invalid inputs that may or may not become
     * valid depending on what the trailing garbage is. */

    unsigned char *buf = NULL;
    unsigned char *p = NULL;
    size_t buffer_size;
    int ret;

    /* Test every prefix of the input, except the empty string.
     * The first byte of the string is the tag. Without a tag byte,
     * we wouldn't know what to parse the input as.
     * Also test the input followed by an extra byte.
     */
    for (buffer_size = 1; buffer_size <= input->len + 1; buffer_size++) {
        mbedtls_test_set_step(buffer_size);
        /* Allocate a new buffer of exactly the length to parse each time.
         * This gives memory sanitizers a chance to catch buffer overreads. */
        ASSERT_ALLOC(buf, buffer_size);
        memcpy(buf, input->x, buffer_size);
        p = buf;
        ret = nested_parse(&p, buf + buffer_size);

        if (ret == ERR_PARSE_INCONSISTENCY) {
            goto exit;
        }
        if (buffer_size < input->len) {
            TEST_EQUAL(ret, MBEDTLS_ERR_ASN1_OUT_OF_DATA);
        } else if (buffer_size == input->len) {
            TEST_EQUAL(ret, full_result);
        } else { /* ( buffer_size > input->len ) */
            if (overfull_result != UNPREDICTABLE_RESULT) {
                TEST_EQUAL(ret, overfull_result);
            }
        }
        if (ret == 0) {
            TEST_ASSERT(p == buf + input->len);
        }

        mbedtls_free(buf);
        buf = NULL;
    }

exit:
    mbedtls_free(buf);
}

static void test_parse_prefixes_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_parse_prefixes( &data0, *( (int *) params[2] ), *( (int *) params[3] ) );
}
#line 281 "suites/test_suite_asn1parse.function"
static void test_get_len(const data_t *input, int actual_length_arg)
{
    size_t actual_length = actual_length_arg;
    size_t buffer_size;

    /* Test prefixes of a buffer containing the given length string
     * followed by `actual_length` bytes of payload. To save a bit of
     * time, we skip some "boring" prefixes: we don't test prefixes where
     * the payload is truncated more than one byte away from either end,
     * and we only test the empty string on a 1-byte input.
     */
    for (buffer_size = 1; buffer_size <= input->len + 1; buffer_size++) {
        if (!get_len_step(input, buffer_size, actual_length)) {
            goto exit;
        }
    }
    if (!get_len_step(input, input->len + actual_length - 1, actual_length)) {
        goto exit;
    }
    if (!get_len_step(input, input->len + actual_length, actual_length)) {
        goto exit;
    }
exit:
    ;
}

static void test_get_len_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_get_len( &data0, *( (int *) params[2] ) );
}
#line 307 "suites/test_suite_asn1parse.function"
static void test_get_boolean(const data_t *input,
                 int expected_value, int expected_result)
{
    unsigned char *p = input->x;
    int val;
    int ret;
    ret = mbedtls_asn1_get_bool(&p, input->x + input->len, &val);
    TEST_EQUAL(ret, expected_result);
    if (expected_result == 0) {
        TEST_EQUAL(val, expected_value);
        TEST_ASSERT(p == input->x + input->len);
    }
exit:
    ;
}

static void test_get_boolean_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_get_boolean( &data0, *( (int *) params[2] ), *( (int *) params[3] ) );
}
#line 323 "suites/test_suite_asn1parse.function"
static void test_empty_integer(const data_t *input)
{
    unsigned char *p;
#if defined(MBEDTLS_BIGNUM_C)
    mbedtls_mpi actual_mpi;
#endif
    int val;

#if defined(MBEDTLS_BIGNUM_C)
    mbedtls_mpi_init(&actual_mpi);
#endif

    /* An INTEGER with no content is not valid. */
    p = input->x;
    TEST_EQUAL(mbedtls_asn1_get_int(&p, input->x + input->len, &val),
               MBEDTLS_ERR_ASN1_INVALID_LENGTH);

#if defined(MBEDTLS_BIGNUM_C)
    /* INTEGERs are sometimes abused as bitstrings, so the library accepts
     * an INTEGER with empty content and gives it the value 0. */
    p = input->x;
    TEST_EQUAL(mbedtls_asn1_get_mpi(&p, input->x + input->len, &actual_mpi),
               0);
    TEST_EQUAL(mbedtls_mpi_cmp_int(&actual_mpi, 0), 0);
#endif

exit:
#if defined(MBEDTLS_BIGNUM_C)
    mbedtls_mpi_free(&actual_mpi);
#endif
    /*empty cleanup in some configurations*/;
}

static void test_empty_integer_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_empty_integer( &data0 );
}
#line 358 "suites/test_suite_asn1parse.function"
static void test_get_integer(const data_t *input,
                 const char *expected_hex, int expected_result)
{
    unsigned char *p;
#if defined(MBEDTLS_BIGNUM_C)
    mbedtls_mpi expected_mpi;
    mbedtls_mpi actual_mpi;
    mbedtls_mpi complement;
    int expected_result_for_mpi = expected_result;
#endif
    long expected_value;
    int expected_result_for_int = expected_result;
    int val;
    int ret;

#if defined(MBEDTLS_BIGNUM_C)
    mbedtls_mpi_init(&expected_mpi);
    mbedtls_mpi_init(&actual_mpi);
    mbedtls_mpi_init(&complement);
#endif

    errno = 0;
    expected_value = strtol(expected_hex, NULL, 16);
    if (expected_result == 0 &&
        (errno == ERANGE
#if LONG_MAX > INT_MAX
         || expected_value > INT_MAX || expected_value < INT_MIN
#endif
        )) {
        /* The library returns the dubious error code INVALID_LENGTH
         * for integers that are out of range. */
        expected_result_for_int = MBEDTLS_ERR_ASN1_INVALID_LENGTH;
    }
    if (expected_result == 0 && expected_value < 0) {
        /* The library does not support negative INTEGERs and
         * returns the dubious error code INVALID_LENGTH.
         * Test that we preserve the historical behavior. If we
         * decide to change the behavior, we'll also change this test. */
        expected_result_for_int = MBEDTLS_ERR_ASN1_INVALID_LENGTH;
    }

    p = input->x;
    ret = mbedtls_asn1_get_int(&p, input->x + input->len, &val);
    TEST_EQUAL(ret, expected_result_for_int);
    if (ret == 0) {
        TEST_EQUAL(val, expected_value);
        TEST_ASSERT(p == input->x + input->len);
    }

#if defined(MBEDTLS_BIGNUM_C)
    ret = mbedtls_test_read_mpi(&expected_mpi, expected_hex);
    TEST_ASSERT(ret == 0 || ret == MBEDTLS_ERR_MPI_BAD_INPUT_DATA);
    if (ret == MBEDTLS_ERR_MPI_BAD_INPUT_DATA) {
        /* The data overflows the maximum MPI size. */
        expected_result_for_mpi = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
    }
    p = input->x;
    ret = mbedtls_asn1_get_mpi(&p, input->x + input->len, &actual_mpi);
    TEST_EQUAL(ret, expected_result_for_mpi);
    if (ret == 0) {
        if (expected_value >= 0) {
            TEST_ASSERT(mbedtls_mpi_cmp_mpi(&actual_mpi,
                                            &expected_mpi) == 0);
        } else {
            /* The library ignores the sign bit in ASN.1 INTEGERs
             * (which makes sense insofar as INTEGERs are sometimes
             * abused as bit strings), so the result of parsing them
             * is a positive integer such that expected_mpi +
             * actual_mpi = 2^n where n is the length of the content
             * of the INTEGER. (Leading ff octets don't matter for the
             * expected value, but they matter for the actual value.)
             * Test that we don't change from this behavior. If we
             * decide to fix the library to change the behavior on
             * negative INTEGERs, we'll fix this test code. */
            unsigned char *q = input->x + 1;
            size_t len;
            TEST_ASSERT(mbedtls_asn1_get_len(&q, input->x + input->len,
                                             &len) == 0);
            TEST_ASSERT(mbedtls_mpi_lset(&complement, 1) == 0);
            TEST_ASSERT(mbedtls_mpi_shift_l(&complement, len * 8) == 0);
            TEST_ASSERT(mbedtls_mpi_add_mpi(&complement, &complement,
                                            &expected_mpi) == 0);
            TEST_ASSERT(mbedtls_mpi_cmp_mpi(&complement,
                                            &actual_mpi) == 0);
        }
        TEST_ASSERT(p == input->x + input->len);
    }
#endif

exit:
#if defined(MBEDTLS_BIGNUM_C)
    mbedtls_mpi_free(&expected_mpi);
    mbedtls_mpi_free(&actual_mpi);
    mbedtls_mpi_free(&complement);
#endif
    /*empty cleanup in some configurations*/;
}

static void test_get_integer_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_get_integer( &data0, (char *) params[2], *( (int *) params[3] ) );
}
#line 458 "suites/test_suite_asn1parse.function"
static void test_get_enum(const data_t *input,
              const char *expected_hex, int expected_result)
{
    unsigned char *p;
    long expected_value;
    int expected_result_for_enum = expected_result;
    int val;
    int ret;

    errno = 0;
    expected_value = strtol(expected_hex, NULL, 16);
    if (expected_result == 0 &&
        (errno == ERANGE
#if LONG_MAX > INT_MAX
         || expected_value > INT_MAX || expected_value < INT_MIN
#endif
        )) {
        /* The library returns the dubious error code INVALID_LENGTH
         * for integers that are out of range. */
        expected_result_for_enum = MBEDTLS_ERR_ASN1_INVALID_LENGTH;
    }
    if (expected_result == 0 && expected_value < 0) {
        /* The library does not support negative INTEGERs and
         * returns the dubious error code INVALID_LENGTH.
         * Test that we preserve the historical behavior. If we
         * decide to change the behavior, we'll also change this test. */
        expected_result_for_enum = MBEDTLS_ERR_ASN1_INVALID_LENGTH;
    }

    p = input->x;
    ret = mbedtls_asn1_get_enum(&p, input->x + input->len, &val);
    TEST_EQUAL(ret, expected_result_for_enum);
    if (ret == 0) {
        TEST_EQUAL(val, expected_value);
        TEST_ASSERT(p == input->x + input->len);
    }
exit:
    ;
}

static void test_get_enum_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_get_enum( &data0, (char *) params[2], *( (int *) params[3] ) );
}
#if defined(MBEDTLS_BIGNUM_C)
#line 498 "suites/test_suite_asn1parse.function"
static void test_get_mpi_too_large(void)
{
    unsigned char *buf = NULL;
    unsigned char *p;
    mbedtls_mpi actual_mpi;
    size_t too_many_octets =
        MBEDTLS_MPI_MAX_LIMBS * sizeof(mbedtls_mpi_uint) + 1;
    size_t size = too_many_octets + 6;

    mbedtls_mpi_init(&actual_mpi);

    ASSERT_ALLOC(buf, size);
    buf[0] = 0x02; /* tag: INTEGER */
    buf[1] = 0x84; /* 4-octet length */
    buf[2] = (too_many_octets >> 24) & 0xff;
    buf[3] = (too_many_octets >> 16) & 0xff;
    buf[4] = (too_many_octets >> 8) & 0xff;
    buf[5] = too_many_octets & 0xff;
    buf[6] = 0x01; /* most significant octet */

    p = buf;
    TEST_EQUAL(mbedtls_asn1_get_mpi(&p, buf + size, &actual_mpi),
               MBEDTLS_ERR_MPI_ALLOC_FAILED);

exit:
    mbedtls_mpi_free(&actual_mpi);
    mbedtls_free(buf);
}

static void test_get_mpi_too_large_wrapper( void ** params )
{
    (void)params;

    test_get_mpi_too_large(  );
}
#endif /* MBEDTLS_BIGNUM_C */
#line 529 "suites/test_suite_asn1parse.function"
static void test_get_bitstring(const data_t *input,
                   int expected_length, int expected_unused_bits,
                   int expected_result, int expected_result_null)
{
    mbedtls_asn1_bitstring bs = { 0xdead, 0x21, NULL };
    unsigned char *p = input->x;

    TEST_EQUAL(mbedtls_asn1_get_bitstring(&p, input->x + input->len, &bs),
               expected_result);
    if (expected_result == 0) {
        TEST_EQUAL(bs.len, (size_t) expected_length);
        TEST_EQUAL(bs.unused_bits, expected_unused_bits);
        TEST_ASSERT(bs.p != NULL);
        TEST_EQUAL(bs.p - input->x + bs.len, input->len);
        TEST_ASSERT(p == input->x + input->len);
    }

    p = input->x;
    TEST_EQUAL(mbedtls_asn1_get_bitstring_null(&p, input->x + input->len,
                                               &bs.len),
               expected_result_null);
    if (expected_result_null == 0) {
        TEST_EQUAL(bs.len, (size_t) expected_length);
        if (expected_result == 0) {
            TEST_ASSERT(p == input->x + input->len - bs.len);
        }
    }
exit:
    ;
}

static void test_get_bitstring_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_get_bitstring( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ) );
}
#line 560 "suites/test_suite_asn1parse.function"
static void test_get_sequence_of(const data_t *input, int tag,
                     const char *description,
                     int expected_result)
{
    /* The description string is a comma-separated list of integers.
     * For each element in the SEQUENCE in input, description contains
     * two integers: the offset of the element (offset from the start
     * of input to the tag of the element) and the length of the
     * element's contents.
     * "offset1,length1,..." */

    mbedtls_asn1_sequence head = { { 0, 0, NULL }, NULL };
    mbedtls_asn1_sequence *cur;
    unsigned char *p = input->x;
    const char *rest = description;
    unsigned long n;
    unsigned int step = 0;

    TEST_EQUAL(mbedtls_asn1_get_sequence_of(&p, input->x + input->len,
                                            &head, tag),
               expected_result);
    if (expected_result == 0) {
        TEST_ASSERT(p == input->x + input->len);

        if (!*rest) {
            TEST_EQUAL(head.buf.tag, 0);
            TEST_ASSERT(head.buf.p == NULL);
            TEST_EQUAL(head.buf.len, 0);
            TEST_ASSERT(head.next == NULL);
        } else {
            cur = &head;
            while (*rest) {
                mbedtls_test_set_step(step);
                TEST_ASSERT(cur != NULL);
                TEST_EQUAL(cur->buf.tag, tag);
                n = strtoul(rest, (char **) &rest, 0);
                TEST_EQUAL(n, (size_t) (cur->buf.p - input->x));
                ++rest;
                n = strtoul(rest, (char **) &rest, 0);
                TEST_EQUAL(n, cur->buf.len);
                if (*rest) {
                    ++rest;
                }
                cur = cur->next;
                ++step;
            }
            TEST_ASSERT(cur == NULL);
        }
    }

exit:
    mbedtls_asn1_sequence_free(head.next);
}

static void test_get_sequence_of_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_get_sequence_of( &data0, *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ) );
}
#line 616 "suites/test_suite_asn1parse.function"
static void test_traverse_sequence_of(const data_t *input,
                          int tag_must_mask, int tag_must_val,
                          int tag_may_mask, int tag_may_val,
                          const char *description,
                          int expected_result)
{
    /* The description string is a comma-separated list of integers.
     * For each element in the SEQUENCE in input, description contains
     * three integers: the offset of the element's content (offset from
     * the start of input to the content of the element), the element's tag,
     * and the length of the element's contents.
     * "offset1,tag1,length1,..." */

    unsigned char *p = input->x;
    traverse_state_t traverse_state = { input->x, description };
    int ret;

    ret = mbedtls_asn1_traverse_sequence_of(&p, input->x + input->len,
                                            (uint8_t) tag_must_mask, (uint8_t) tag_must_val,
                                            (uint8_t) tag_may_mask, (uint8_t) tag_may_val,
                                            traverse_callback, &traverse_state);
    if (ret == RET_TRAVERSE_ERROR) {
        goto exit;
    }
    TEST_EQUAL(ret, expected_result);
    TEST_EQUAL(*traverse_state.description, 0);
exit:
    ;
}

static void test_traverse_sequence_of_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_traverse_sequence_of( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), (char *) params[6], *( (int *) params[7] ) );
}
#line 646 "suites/test_suite_asn1parse.function"
static void test_get_alg(const data_t *input,
             int oid_offset, int oid_length,
             int params_tag, int params_offset, int params_length,
             int total_length,
             int expected_result)
{
    mbedtls_asn1_buf oid = { -1, 0, NULL };
    mbedtls_asn1_buf params = { -1, 0, NULL };
    unsigned char *p = input->x;
    int ret;

    TEST_EQUAL(mbedtls_asn1_get_alg(&p, input->x + input->len,
                                    &oid, &params),
               expected_result);
    if (expected_result == 0) {
        TEST_EQUAL(oid.tag, MBEDTLS_ASN1_OID);
        TEST_EQUAL(oid.p - input->x, oid_offset);
        TEST_EQUAL(oid.len, (size_t) oid_length);
        TEST_EQUAL(params.tag, params_tag);
        if (params_offset != 0) {
            TEST_EQUAL(params.p - input->x, params_offset);
        } else {
            TEST_ASSERT(params.p == NULL);
        }
        TEST_EQUAL(params.len, (size_t) params_length);
        TEST_EQUAL(p - input->x, total_length);
    }

    ret = mbedtls_asn1_get_alg_null(&p, input->x + input->len, &oid);
    if (expected_result == 0 && params_offset == 0) {
        TEST_EQUAL(oid.tag, MBEDTLS_ASN1_OID);
        TEST_EQUAL(oid.p - input->x, oid_offset);
        TEST_EQUAL(oid.len, (size_t) oid_length);
        TEST_EQUAL(p - input->x, total_length);
    } else {
        TEST_ASSERT(ret != 0);
    }
exit:
    ;
}

static void test_get_alg_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_get_alg( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), *( (int *) params[6] ), *( (int *) params[7] ), *( (int *) params[8] ) );
}
#line 687 "suites/test_suite_asn1parse.function"
static void test_find_named_data(data_t *oid0, data_t *oid1, data_t *oid2, data_t *oid3,
                     data_t *needle, int from, int position)
{
    mbedtls_asn1_named_data nd[] = {
        { { 0x06, oid0->len, oid0->x }, { 0, 0, NULL }, NULL, 0 },
        { { 0x06, oid1->len, oid1->x }, { 0, 0, NULL }, NULL, 0 },
        { { 0x06, oid2->len, oid2->x }, { 0, 0, NULL }, NULL, 0 },
        { { 0x06, oid3->len, oid3->x }, { 0, 0, NULL }, NULL, 0 },
    };
    mbedtls_asn1_named_data *pointers[ARRAY_LENGTH(nd) + 1];
    size_t i;
    const mbedtls_asn1_named_data *found;

    for (i = 0; i < ARRAY_LENGTH(nd); i++) {
        pointers[i] = &nd[i];
    }
    pointers[ARRAY_LENGTH(nd)] = NULL;
    for (i = 0; i < ARRAY_LENGTH(nd); i++) {
        nd[i].next = pointers[i+1];
    }

    found = mbedtls_asn1_find_named_data((const mbedtls_asn1_named_data *) pointers[from],
                                         (const char *) needle->x,
                                         needle->len);
    TEST_ASSERT(found == pointers[position]);
exit:
    ;
}

static void test_find_named_data_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_find_named_data( &data0, &data2, &data4, &data6, &data8, *( (int *) params[10] ), *( (int *) params[11] ) );
}
#if !defined(MBEDTLS_DEPRECATED_REMOVED)
#if !defined(MBEDTLS_DEPRECATED_WARNING)
#line 716 "suites/test_suite_asn1parse.function"
static void test_free_named_data_null(void)
{
    mbedtls_asn1_free_named_data(NULL);
    goto exit; /* Silence unused label warning */
exit:
    ;
}

static void test_free_named_data_null_wrapper( void ** params )
{
    (void)params;

    test_free_named_data_null(  );
}
#endif /* !MBEDTLS_DEPRECATED_WARNING */
#endif /* !MBEDTLS_DEPRECATED_REMOVED */
#if !defined(MBEDTLS_DEPRECATED_REMOVED)
#if !defined(MBEDTLS_DEPRECATED_WARNING)
#line 724 "suites/test_suite_asn1parse.function"
static void test_free_named_data(int with_oid, int with_val, int with_next)
{
    mbedtls_asn1_named_data next =
    { { 0x06, 0, NULL }, { 0, 0xcafe, NULL }, NULL, 0 };
    mbedtls_asn1_named_data head =
    { { 0x06, 0, NULL }, { 0, 0, NULL }, NULL, 0 };

    if (with_oid) {
        ASSERT_ALLOC(head.oid.p, 1);
    }
    if (with_val) {
        ASSERT_ALLOC(head.val.p, 1);
    }
    if (with_next) {
        head.next = &next;
    }

    mbedtls_asn1_free_named_data(&head);
    TEST_ASSERT(head.oid.p == NULL);
    TEST_ASSERT(head.val.p == NULL);
    TEST_ASSERT(head.next == NULL);
    TEST_ASSERT(next.val.len == 0xcafe);

exit:
    mbedtls_free(head.oid.p);
    mbedtls_free(head.val.p);
}

static void test_free_named_data_wrapper( void ** params )
{

    test_free_named_data( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
#endif /* !MBEDTLS_DEPRECATED_WARNING */
#endif /* !MBEDTLS_DEPRECATED_REMOVED */
#line 754 "suites/test_suite_asn1parse.function"
static void test_free_named_data_list(int length)
{
    mbedtls_asn1_named_data *head = NULL;
    int i;

    for (i = 0; i < length; i++) {
        mbedtls_asn1_named_data *new = NULL;
        ASSERT_ALLOC(new, 1);
        new->next = head;
        head = new;
    }

    mbedtls_asn1_free_named_data_list(&head);
    TEST_ASSERT(head == NULL);
    /* Most of the point of the test is that it doesn't leak memory.
     * So this test is only really useful under a memory leak detection
     * framework. */
exit:
    mbedtls_asn1_free_named_data_list(&head);
}

static void test_free_named_data_list_wrapper( void ** params )
{

    test_free_named_data_list( *( (int *) params[0] ) );
}
#endif /* MBEDTLS_ASN1_PARSE_C */


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
    
#if defined(MBEDTLS_ASN1_PARSE_C)

        case 0:
            {
                *out_value = MBEDTLS_ERR_ASN1_OUT_OF_DATA;
            }
            break;
        case 1:
            {
                *out_value = UNPREDICTABLE_RESULT;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_ERR_ASN1_INVALID_LENGTH;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_ERR_ASN1_LENGTH_MISMATCH;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_ERR_ASN1_UNEXPECTED_TAG;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_ERR_ASN1_INVALID_DATA;
            }
            break;
        case 6:
            {
                *out_value = RET_TRAVERSE_STOP;
            }
            break;
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
    
#if defined(MBEDTLS_ASN1_PARSE_C)

        case 0:
            {
#if defined(MBEDTLS_TEST_DEPRECATED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
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

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_parse_prefixes_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_get_len_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_get_boolean_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_empty_integer_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_get_integer_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_get_enum_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_ASN1_PARSE_C) && defined(MBEDTLS_BIGNUM_C)
    test_get_mpi_too_large_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_get_bitstring_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_get_sequence_of_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_traverse_sequence_of_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_get_alg_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_find_named_data_wrapper,
#else
    NULL,
#endif
/* Function Id: 12 */

#if defined(MBEDTLS_ASN1_PARSE_C) && !defined(MBEDTLS_DEPRECATED_REMOVED) && !defined(MBEDTLS_DEPRECATED_WARNING)
    test_free_named_data_null_wrapper,
#else
    NULL,
#endif
/* Function Id: 13 */

#if defined(MBEDTLS_ASN1_PARSE_C) && !defined(MBEDTLS_DEPRECATED_REMOVED) && !defined(MBEDTLS_DEPRECATED_WARNING)
    test_free_named_data_wrapper,
#else
    NULL,
#endif
/* Function Id: 14 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_free_named_data_list_wrapper,
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
    const char *default_filename = "./test_suite_asn1parse.datax";
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
