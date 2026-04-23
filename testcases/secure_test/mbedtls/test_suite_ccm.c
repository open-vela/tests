#line 2 "suites/main_test.function"
/*
 * *** THIS FILE HAS BEEN MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py
 *
 * Test file      : ./test_suite_ccm.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_ccm.function
 *      Test suite data     : suites/test_suite_ccm.data
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

#if defined(MBEDTLS_CCM_C)
#line 2 "suites/test_suite_ccm.function"
#include "mbedtls/ccm.h"

/* Use the multipart interface to process the encrypted data in two parts
 * and check that the output matches the expected output.
 * The context must have been set up with the key. */
static int check_multipart(mbedtls_ccm_context *ctx,
                           int mode,
                           const data_t *iv,
                           const data_t *add,
                           const data_t *input,
                           const data_t *expected_output,
                           const data_t *tag,
                           size_t n1,
                           size_t n1_add)
{
    int ok = 0;
    uint8_t *output = NULL;
    size_t n2 = input->len - n1;
    size_t n2_add = add->len - n1_add;
    size_t olen;

    /* Sanity checks on the test data */
    TEST_ASSERT(n1 <= input->len);
    TEST_ASSERT(n1_add <= add->len);
    TEST_EQUAL(input->len, expected_output->len);
    TEST_EQUAL(0, mbedtls_ccm_starts(ctx, mode, iv->x, iv->len));
    TEST_EQUAL(0, mbedtls_ccm_set_lengths(ctx, add->len, input->len, tag->len));
    TEST_EQUAL(0, mbedtls_ccm_update_ad(ctx, add->x, n1_add));
    TEST_EQUAL(0, mbedtls_ccm_update_ad(ctx, add->x + n1_add, n2_add));

    /* Allocate a tight buffer for each update call. This way, if the function
     * tries to write beyond the advertised required buffer size, this will
     * count as an overflow for memory sanitizers and static checkers. */
    ASSERT_ALLOC(output, n1);
    olen = 0xdeadbeef;
    TEST_EQUAL(0, mbedtls_ccm_update(ctx, input->x, n1, output, n1, &olen));
    TEST_EQUAL(n1, olen);
    ASSERT_COMPARE(output, olen, expected_output->x, n1);
    mbedtls_free(output);
    output = NULL;

    ASSERT_ALLOC(output, n2);
    olen = 0xdeadbeef;
    TEST_EQUAL(0, mbedtls_ccm_update(ctx, input->x + n1, n2, output, n2, &olen));
    TEST_EQUAL(n2, olen);
    ASSERT_COMPARE(output, olen, expected_output->x + n1, n2);
    mbedtls_free(output);
    output = NULL;

    ASSERT_ALLOC(output, tag->len);
    TEST_EQUAL(0, mbedtls_ccm_finish(ctx, output, tag->len));
    ASSERT_COMPARE(output, tag->len, tag->x, tag->len);
    mbedtls_free(output);
    output = NULL;

    ok = 1;
exit:
    mbedtls_free(output);
    return ok;
}
#if defined(MBEDTLS_SELF_TEST)
#if defined(MBEDTLS_AES_C)
#line 70 "suites/test_suite_ccm.function"
static void test_mbedtls_ccm_self_test(void)
{
    TEST_ASSERT(mbedtls_ccm_self_test(1) == 0);
exit:
    ;
}

static void test_mbedtls_ccm_self_test_wrapper( void ** params )
{
    (void)params;

    test_mbedtls_ccm_self_test(  );
}
#endif /* MBEDTLS_AES_C */
#endif /* MBEDTLS_SELF_TEST */
#line 77 "suites/test_suite_ccm.function"
static void test_mbedtls_ccm_setkey(int cipher_id, int key_size, int result)
{
    mbedtls_ccm_context ctx;
    unsigned char key[32];
    int ret;

    mbedtls_ccm_init(&ctx);

    memset(key, 0x2A, sizeof(key));
    TEST_ASSERT((unsigned) key_size <= 8 * sizeof(key));

    ret = mbedtls_ccm_setkey(&ctx, cipher_id, key, key_size);
    TEST_ASSERT(ret == result);

exit:
    mbedtls_ccm_free(&ctx);
}

static void test_mbedtls_ccm_setkey_wrapper( void ** params )
{

    test_mbedtls_ccm_setkey( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
#if defined(MBEDTLS_AES_C)
#line 97 "suites/test_suite_ccm.function"
static void test_ccm_lengths(int msg_len, int iv_len, int add_len, int tag_len, int res)
{
    mbedtls_ccm_context ctx;
    unsigned char key[16];
    unsigned char msg[10];
    unsigned char iv[14];
    unsigned char *add = NULL;
    unsigned char out[10];
    unsigned char tag[18];
    int decrypt_ret;

    mbedtls_ccm_init(&ctx);

    ASSERT_ALLOC_WEAK(add, add_len);
    memset(key, 0, sizeof(key));
    memset(msg, 0, sizeof(msg));
    memset(iv, 0, sizeof(iv));
    memset(out, 0, sizeof(out));
    memset(tag, 0, sizeof(tag));

    TEST_ASSERT(mbedtls_ccm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES,
                                   key, 8 * sizeof(key)) == 0);

    TEST_ASSERT(mbedtls_ccm_encrypt_and_tag(&ctx, msg_len, iv, iv_len, add, add_len,
                                            msg, out, tag, tag_len) == res);

    decrypt_ret = mbedtls_ccm_auth_decrypt(&ctx, msg_len, iv, iv_len, add, add_len,
                                           msg, out, tag, tag_len);

    if (res == 0) {
        TEST_ASSERT(decrypt_ret == MBEDTLS_ERR_CCM_AUTH_FAILED);
    } else {
        TEST_ASSERT(decrypt_ret == res);
    }

exit:
    mbedtls_free(add);
    mbedtls_ccm_free(&ctx);
}

static void test_ccm_lengths_wrapper( void ** params )
{

    test_ccm_lengths( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ) );
}
#endif /* MBEDTLS_AES_C */
#if defined(MBEDTLS_AES_C)
#line 139 "suites/test_suite_ccm.function"
static void test_ccm_star_lengths(int msg_len, int iv_len, int add_len, int tag_len,
                      int res)
{
    mbedtls_ccm_context ctx;
    unsigned char key[16];
    unsigned char msg[10];
    unsigned char iv[14];
    unsigned char add[10];
    unsigned char out[10];
    unsigned char tag[18];
    int decrypt_ret;

    mbedtls_ccm_init(&ctx);

    memset(key, 0, sizeof(key));
    memset(msg, 0, sizeof(msg));
    memset(iv, 0, sizeof(iv));
    memset(add, 0, sizeof(add));
    memset(out, 0, sizeof(out));
    memset(tag, 0, sizeof(tag));

    TEST_ASSERT(mbedtls_ccm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES,
                                   key, 8 * sizeof(key)) == 0);

    TEST_ASSERT(mbedtls_ccm_star_encrypt_and_tag(&ctx, msg_len, iv, iv_len,
                                                 add, add_len, msg, out, tag, tag_len) == res);

    decrypt_ret = mbedtls_ccm_star_auth_decrypt(&ctx, msg_len, iv, iv_len, add,
                                                add_len, msg, out, tag, tag_len);

    if (res == 0 && tag_len != 0) {
        TEST_ASSERT(decrypt_ret == MBEDTLS_ERR_CCM_AUTH_FAILED);
    } else {
        TEST_ASSERT(decrypt_ret == res);
    }

exit:
    mbedtls_ccm_free(&ctx);
}

static void test_ccm_star_lengths_wrapper( void ** params )
{

    test_ccm_star_lengths( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ) );
}
#endif /* MBEDTLS_AES_C */
#line 181 "suites/test_suite_ccm.function"
static void test_mbedtls_ccm_encrypt_and_tag(int cipher_id, data_t *key,
                                 data_t *msg, data_t *iv,
                                 data_t *add, data_t *result)
{
    mbedtls_ccm_context ctx;
    size_t n1, n1_add;
    uint8_t *io_msg_buf = NULL;
    uint8_t *tag_buf = NULL;
    const size_t expected_tag_len = result->len - msg->len;
    const uint8_t *expected_tag = result->x + msg->len;

    /* Prepare input/output message buffer */
    ASSERT_ALLOC(io_msg_buf, msg->len);
    if (msg->len != 0) {
        memcpy(io_msg_buf, msg->x, msg->len);
    }

    /* Prepare tag buffer */
    ASSERT_ALLOC(tag_buf, expected_tag_len);

    mbedtls_ccm_init(&ctx);
    TEST_EQUAL(mbedtls_ccm_setkey(&ctx, cipher_id, key->x, key->len * 8), 0);
    /* Test with input == output */
    TEST_EQUAL(mbedtls_ccm_encrypt_and_tag(&ctx, msg->len, iv->x, iv->len, add->x, add->len,
                                           io_msg_buf, io_msg_buf, tag_buf, expected_tag_len), 0);

    ASSERT_COMPARE(io_msg_buf, msg->len, result->x, msg->len);
    ASSERT_COMPARE(tag_buf, expected_tag_len, expected_tag, expected_tag_len);

    /* Prepare data_t structures for multipart testing */
    const data_t encrypted_expected = { .x = result->x,
                                        .len = msg->len };
    const data_t tag_expected = { .x = (uint8_t *) expected_tag, /* cast to conform with data_t x type */
                                  .len = expected_tag_len };

    for (n1 = 0; n1 <= msg->len; n1 += 1) {
        for (n1_add = 0; n1_add <= add->len; n1_add += 1) {
            mbedtls_test_set_step(n1 * 10000 + n1_add);
            if (!check_multipart(&ctx, MBEDTLS_CCM_ENCRYPT,
                                 iv, add, msg,
                                 &encrypted_expected,
                                 &tag_expected,
                                 n1, n1_add)) {
                goto exit;
            }
        }
    }

exit:
    mbedtls_ccm_free(&ctx);
    mbedtls_free(io_msg_buf);
    mbedtls_free(tag_buf);
}

static void test_mbedtls_ccm_encrypt_and_tag_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data9 = {(uint8_t *) params[9], *( (uint32_t *) params[10] )};

    test_mbedtls_ccm_encrypt_and_tag( *( (int *) params[0] ), &data1, &data3, &data5, &data7, &data9 );
}
#line 237 "suites/test_suite_ccm.function"
static void test_mbedtls_ccm_star_no_tag(int cipher_id, int mode, data_t *key,
                             data_t *msg, data_t *iv, data_t *result)
{
    mbedtls_ccm_context ctx;
    uint8_t *output = NULL;
    size_t olen;

    mbedtls_ccm_init(&ctx);
    TEST_EQUAL(mbedtls_ccm_setkey(&ctx, cipher_id, key->x, key->len * 8), 0);
    TEST_EQUAL(0, mbedtls_ccm_starts(&ctx, mode, iv->x, iv->len));
    TEST_EQUAL(0, mbedtls_ccm_set_lengths(&ctx, 0, msg->len, 0));

    ASSERT_ALLOC(output, msg->len);
    TEST_EQUAL(0, mbedtls_ccm_update(&ctx, msg->x, msg->len, output, msg->len, &olen));
    TEST_EQUAL(result->len, olen);
    ASSERT_COMPARE(output, olen, result->x, result->len);

    TEST_EQUAL(0, mbedtls_ccm_finish(&ctx, NULL, 0));
exit:
    mbedtls_free(output);
    mbedtls_ccm_free(&ctx);
}

static void test_mbedtls_ccm_star_no_tag_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_mbedtls_ccm_star_no_tag( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6, &data8 );
}
#line 262 "suites/test_suite_ccm.function"
static void test_mbedtls_ccm_auth_decrypt(int cipher_id, data_t *key,
                              data_t *msg, data_t *iv,
                              data_t *add, int expected_tag_len, int result,
                              data_t *expected_msg)
{
    mbedtls_ccm_context ctx;
    size_t n1, n1_add;

    const size_t expected_msg_len = msg->len - expected_tag_len;
    const uint8_t *expected_tag = msg->x + expected_msg_len;

    /* Prepare input/output message buffer */
    uint8_t *io_msg_buf = NULL;
    ASSERT_ALLOC(io_msg_buf, expected_msg_len);
    if (expected_msg_len) {
        memcpy(io_msg_buf, msg->x, expected_msg_len);
    }

    mbedtls_ccm_init(&ctx);
    TEST_EQUAL(mbedtls_ccm_setkey(&ctx, cipher_id, key->x, key->len * 8), 0);
    /* Test with input == output */
    TEST_EQUAL(mbedtls_ccm_auth_decrypt(&ctx, expected_msg_len, iv->x, iv->len, add->x, add->len,
                                        io_msg_buf, io_msg_buf, expected_tag, expected_tag_len),
               result);

    if (result == 0) {
        ASSERT_COMPARE(io_msg_buf, expected_msg_len, expected_msg->x, expected_msg_len);

        /* Prepare data_t structures for multipart testing */
        const data_t encrypted = { .x = msg->x,
                                   .len = expected_msg_len };

        const data_t tag_expected = { .x = (uint8_t *) expected_tag,
                                      .len = expected_tag_len };

        for (n1 = 0; n1 <= expected_msg_len; n1 += 1) {
            for (n1_add = 0; n1_add <= add->len; n1_add += 1) {
                mbedtls_test_set_step(n1 * 10000 + n1_add);
                if (!check_multipart(&ctx, MBEDTLS_CCM_DECRYPT,
                                     iv, add, &encrypted,
                                     expected_msg,
                                     &tag_expected,
                                     n1, n1_add)) {
                    goto exit;
                }
            }
        }
    } else {
        size_t i;

        for (i = 0; i < expected_msg_len; i++) {
            TEST_EQUAL(io_msg_buf[i], 0);
        }
    }

exit:
    mbedtls_free(io_msg_buf);
    mbedtls_ccm_free(&ctx);
}

static void test_mbedtls_ccm_auth_decrypt_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data11 = {(uint8_t *) params[11], *( (uint32_t *) params[12] )};

    test_mbedtls_ccm_auth_decrypt( *( (int *) params[0] ), &data1, &data3, &data5, &data7, *( (int *) params[9] ), *( (int *) params[10] ), &data11 );
}
#line 324 "suites/test_suite_ccm.function"
static void test_mbedtls_ccm_star_encrypt_and_tag(int cipher_id,
                                      data_t *key, data_t *msg,
                                      data_t *source_address, data_t *frame_counter,
                                      int sec_level, data_t *add,
                                      data_t *expected_result, int output_ret)
{
    unsigned char iv[13];
    mbedtls_ccm_context ctx;
    size_t iv_len, expected_tag_len;
    size_t n1, n1_add;
    uint8_t *io_msg_buf = NULL;
    uint8_t *tag_buf = NULL;

    const uint8_t *expected_tag = expected_result->x + msg->len;

    /* Calculate tag length */
    if (sec_level % 4 == 0) {
        expected_tag_len = 0;
    } else {
        expected_tag_len = 1 << (sec_level % 4 + 1);
    }

    /* Prepare input/output message buffer */
    ASSERT_ALLOC(io_msg_buf, msg->len);
    if (msg->len) {
        memcpy(io_msg_buf, msg->x, msg->len);
    }

    /* Prepare tag buffer */
    if (expected_tag_len == 0) {
        ASSERT_ALLOC(tag_buf, 16);
    } else {
        ASSERT_ALLOC(tag_buf, expected_tag_len);
    }

    /* Calculate iv */
    TEST_ASSERT(source_address->len == 8);
    TEST_ASSERT(frame_counter->len == 4);
    memcpy(iv, source_address->x, source_address->len);
    memcpy(iv + source_address->len, frame_counter->x, frame_counter->len);
    iv[source_address->len + frame_counter->len] = sec_level;
    iv_len = sizeof(iv);

    mbedtls_ccm_init(&ctx);
    TEST_EQUAL(mbedtls_ccm_setkey(&ctx, cipher_id,
                                  key->x, key->len * 8), 0);
    /* Test with input == output */
    TEST_EQUAL(mbedtls_ccm_star_encrypt_and_tag(&ctx, msg->len, iv, iv_len,
                                                add->x, add->len, io_msg_buf,
                                                io_msg_buf, tag_buf, expected_tag_len), output_ret);

    ASSERT_COMPARE(io_msg_buf, msg->len, expected_result->x, msg->len);
    ASSERT_COMPARE(tag_buf, expected_tag_len, expected_tag, expected_tag_len);

    if (output_ret == 0) {
        const data_t iv_data = { .x = iv,
                                 .len = iv_len };

        const data_t encrypted_expected = { .x = expected_result->x,
                                            .len = msg->len };
        const data_t tag_expected = { .x = (uint8_t *) expected_tag,
                                      .len = expected_tag_len };

        for (n1 = 0; n1 <= msg->len; n1 += 1) {
            for (n1_add = 0; n1_add <= add->len; n1_add += 1) {
                mbedtls_test_set_step(n1 * 10000 + n1_add);
                if (!check_multipart(&ctx, MBEDTLS_CCM_STAR_ENCRYPT,
                                     &iv_data, add, msg,
                                     &encrypted_expected,
                                     &tag_expected,
                                     n1, n1_add)) {
                    goto exit;
                }
            }
        }
    }

exit:
    mbedtls_ccm_free(&ctx);
    mbedtls_free(io_msg_buf);
    mbedtls_free(tag_buf);
}

static void test_mbedtls_ccm_star_encrypt_and_tag_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};
    data_t data12 = {(uint8_t *) params[12], *( (uint32_t *) params[13] )};

    test_mbedtls_ccm_star_encrypt_and_tag( *( (int *) params[0] ), &data1, &data3, &data5, &data7, *( (int *) params[9] ), &data10, &data12, *( (int *) params[14] ) );
}
#line 409 "suites/test_suite_ccm.function"
static void test_mbedtls_ccm_star_auth_decrypt(int cipher_id,
                                   data_t *key, data_t *msg,
                                   data_t *source_address, data_t *frame_counter,
                                   int sec_level, data_t *add,
                                   data_t *expected_result, int output_ret)
{
    unsigned char iv[13];
    mbedtls_ccm_context ctx;
    size_t iv_len, expected_tag_len;
    size_t n1, n1_add;

    /* Calculate tag length */
    if (sec_level % 4 == 0) {
        expected_tag_len = 0;
    } else {
        expected_tag_len = 1 << (sec_level % 4 + 1);
    }

    const size_t expected_msg_len = msg->len - expected_tag_len;
    const uint8_t *expected_tag = msg->x + expected_msg_len;

    /* Prepare input/output message buffer */
    uint8_t *io_msg_buf = NULL;
    ASSERT_ALLOC(io_msg_buf, expected_msg_len);
    if (expected_msg_len) {
        memcpy(io_msg_buf, msg->x, expected_msg_len);
    }

    /* Calculate iv */
    memset(iv, 0x00, sizeof(iv));
    TEST_ASSERT(source_address->len == 8);
    TEST_ASSERT(frame_counter->len == 4);
    memcpy(iv, source_address->x, source_address->len);
    memcpy(iv + source_address->len, frame_counter->x, frame_counter->len);
    iv[source_address->len + frame_counter->len] = sec_level;
    iv_len = sizeof(iv);

    mbedtls_ccm_init(&ctx);
    TEST_ASSERT(mbedtls_ccm_setkey(&ctx, cipher_id, key->x, key->len * 8) == 0);
    /* Test with input == output */
    TEST_EQUAL(mbedtls_ccm_star_auth_decrypt(&ctx, expected_msg_len, iv, iv_len,
                                             add->x, add->len, io_msg_buf, io_msg_buf,
                                             expected_tag, expected_tag_len), output_ret);

    ASSERT_COMPARE(io_msg_buf, expected_msg_len, expected_result->x, expected_msg_len);

    if (output_ret == 0) {
        const data_t iv_data = { .x = iv,
                                 .len = iv_len };

        const data_t encrypted = { .x = msg->x,
                                   .len = expected_msg_len };

        const data_t tag_expected = { .x = (uint8_t *) expected_tag,
                                      .len = expected_tag_len };

        for (n1 = 0; n1 <= expected_msg_len; n1 += 1) {
            for (n1_add = 0; n1_add <= add->len; n1_add += 1) {
                mbedtls_test_set_step(n1 * 10000 + n1_add);
                if (!check_multipart(&ctx, MBEDTLS_CCM_STAR_DECRYPT,
                                     &iv_data, add, &encrypted,
                                     expected_result,
                                     &tag_expected,
                                     n1, n1_add)) {
                    goto exit;
                }
            }
        }
    }

exit:
    mbedtls_ccm_free(&ctx);
    mbedtls_free(io_msg_buf);
}

static void test_mbedtls_ccm_star_auth_decrypt_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};
    data_t data12 = {(uint8_t *) params[12], *( (uint32_t *) params[13] )};

    test_mbedtls_ccm_star_auth_decrypt( *( (int *) params[0] ), &data1, &data3, &data5, &data7, *( (int *) params[9] ), &data10, &data12, *( (int *) params[14] ) );
}
#line 487 "suites/test_suite_ccm.function"
static void test_mbedtls_ccm_skip_ad(int cipher_id, int mode,
                         data_t *key, data_t *msg, data_t *iv,
                         data_t *result, data_t *tag)
{
    mbedtls_ccm_context ctx;
    uint8_t *output = NULL;
    size_t olen;

    /* Sanity checks on the test data */
    TEST_EQUAL(msg->len, result->len);

    mbedtls_ccm_init(&ctx);
    TEST_EQUAL(mbedtls_ccm_setkey(&ctx, cipher_id, key->x, key->len * 8), 0);
    TEST_EQUAL(0, mbedtls_ccm_starts(&ctx, mode, iv->x, iv->len));
    TEST_EQUAL(0, mbedtls_ccm_set_lengths(&ctx, 0, msg->len, tag->len));

    ASSERT_ALLOC(output, result->len);
    olen = 0xdeadbeef;
    TEST_EQUAL(0, mbedtls_ccm_update(&ctx, msg->x, msg->len, output, result->len, &olen));
    TEST_EQUAL(result->len, olen);
    ASSERT_COMPARE(output, olen, result->x, result->len);
    mbedtls_free(output);
    output = NULL;

    ASSERT_ALLOC(output, tag->len);
    TEST_EQUAL(0, mbedtls_ccm_finish(&ctx, output, tag->len));
    ASSERT_COMPARE(output, tag->len, tag->x, tag->len);
    mbedtls_free(output);
    output = NULL;

exit:
    mbedtls_free(output);
    mbedtls_ccm_free(&ctx);
}

static void test_mbedtls_ccm_skip_ad_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};

    test_mbedtls_ccm_skip_ad( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6, &data8, &data10 );
}
#line 525 "suites/test_suite_ccm.function"
static void test_mbedtls_ccm_skip_update(int cipher_id, int mode,
                             data_t *key, data_t *iv, data_t *add,
                             data_t *tag)
{
    mbedtls_ccm_context ctx;
    uint8_t *output = NULL;

    mbedtls_ccm_init(&ctx);
    TEST_EQUAL(mbedtls_ccm_setkey(&ctx, cipher_id, key->x, key->len * 8), 0);
    TEST_EQUAL(0, mbedtls_ccm_starts(&ctx, mode, iv->x, iv->len));
    TEST_EQUAL(0, mbedtls_ccm_set_lengths(&ctx, add->len, 0, tag->len));

    TEST_EQUAL(0, mbedtls_ccm_update_ad(&ctx, add->x, add->len));

    ASSERT_ALLOC(output, tag->len);
    TEST_EQUAL(0, mbedtls_ccm_finish(&ctx, output, tag->len));
    ASSERT_COMPARE(output, tag->len, tag->x, tag->len);
    mbedtls_free(output);
    output = NULL;

exit:
    mbedtls_free(output);
    mbedtls_ccm_free(&ctx);
}

static void test_mbedtls_ccm_skip_update_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_mbedtls_ccm_skip_update( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6, &data8 );
}
#line 553 "suites/test_suite_ccm.function"
static void test_mbedtls_ccm_overflow_ad(int cipher_id, int mode,
                             data_t *key, data_t *iv,
                             data_t *add)
{
    mbedtls_ccm_context ctx;

    mbedtls_ccm_init(&ctx);
    TEST_EQUAL(mbedtls_ccm_setkey(&ctx, cipher_id, key->x, key->len * 8), 0);
    TEST_EQUAL(0, mbedtls_ccm_starts(&ctx, mode, iv->x, iv->len));
    // use hardcoded values for msg length and tag length. They are not a part of this test
    // subtract 1 from configured auth data length to provoke an overflow
    TEST_EQUAL(0, mbedtls_ccm_set_lengths(&ctx, add->len - 1, 16, 16));

    TEST_EQUAL(MBEDTLS_ERR_CCM_BAD_INPUT, mbedtls_ccm_update_ad(&ctx, add->x, add->len));
exit:
    mbedtls_ccm_free(&ctx);
}

static void test_mbedtls_ccm_overflow_ad_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_mbedtls_ccm_overflow_ad( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6 );
}
#line 574 "suites/test_suite_ccm.function"
static void test_mbedtls_ccm_unexpected_ad(int cipher_id, int mode,
                               data_t *key, data_t *iv,
                               data_t *add)
{
    mbedtls_ccm_context ctx;

    mbedtls_ccm_init(&ctx);
    TEST_EQUAL(mbedtls_ccm_setkey(&ctx, cipher_id, key->x, key->len * 8), 0);
    TEST_EQUAL(0, mbedtls_ccm_starts(&ctx, mode, iv->x, iv->len));
    // use hardcoded values for msg length and tag length. They are not a part of this test
    TEST_EQUAL(0, mbedtls_ccm_set_lengths(&ctx, 0, 16, 16));

    TEST_EQUAL(MBEDTLS_ERR_CCM_BAD_INPUT, mbedtls_ccm_update_ad(&ctx, add->x, add->len));
exit:
    mbedtls_ccm_free(&ctx);
}

static void test_mbedtls_ccm_unexpected_ad_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_mbedtls_ccm_unexpected_ad( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6 );
}
#line 594 "suites/test_suite_ccm.function"
static void test_mbedtls_ccm_unexpected_text(int cipher_id, int mode,
                                 data_t *key, data_t *msg, data_t *iv,
                                 data_t *add)
{
    mbedtls_ccm_context ctx;
    uint8_t *output = NULL;
    size_t olen;

    mbedtls_ccm_init(&ctx);
    TEST_EQUAL(mbedtls_ccm_setkey(&ctx, cipher_id, key->x, key->len * 8), 0);
    TEST_EQUAL(0, mbedtls_ccm_starts(&ctx, mode, iv->x, iv->len));
    // use hardcoded value for tag length. It is not a part of this test
    TEST_EQUAL(0, mbedtls_ccm_set_lengths(&ctx, add->len, 0, 16));

    TEST_EQUAL(0, mbedtls_ccm_update_ad(&ctx, add->x, add->len));

    ASSERT_ALLOC(output, msg->len);
    olen = 0xdeadbeef;
    TEST_EQUAL(MBEDTLS_ERR_CCM_BAD_INPUT,
               mbedtls_ccm_update(&ctx, msg->x, msg->len, output, msg->len, &olen));
exit:
    mbedtls_free(output);
    mbedtls_ccm_free(&ctx);
}

static void test_mbedtls_ccm_unexpected_text_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_mbedtls_ccm_unexpected_text( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6, &data8 );
}
#line 622 "suites/test_suite_ccm.function"
static void test_mbedtls_ccm_incomplete_ad(int cipher_id, int mode,
                               data_t *key, data_t *iv, data_t *add)
{
    mbedtls_ccm_context ctx;
    uint8_t *output = NULL;

    mbedtls_ccm_init(&ctx);
    TEST_EQUAL(mbedtls_ccm_setkey(&ctx, cipher_id, key->x, key->len * 8), 0);
    TEST_EQUAL(0, mbedtls_ccm_starts(&ctx, mode, iv->x, iv->len));
    // use hardcoded values for msg length and tag length. They are not a part of this test
    TEST_EQUAL(0, mbedtls_ccm_set_lengths(&ctx, add->len, 0, 16));

    TEST_EQUAL(0, mbedtls_ccm_update_ad(&ctx, add->x, add->len - 1));

    ASSERT_ALLOC(output, 16);
    TEST_EQUAL(MBEDTLS_ERR_CCM_BAD_INPUT, mbedtls_ccm_finish(&ctx, output, 16));

exit:
    mbedtls_free(output);
    mbedtls_ccm_free(&ctx);
}

static void test_mbedtls_ccm_incomplete_ad_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_mbedtls_ccm_incomplete_ad( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6 );
}
#line 648 "suites/test_suite_ccm.function"
static void test_mbedtls_ccm_full_ad_and_overflow(int cipher_id, int mode,
                                      data_t *key, data_t *iv,
                                      data_t *add)
{
    mbedtls_ccm_context ctx;

    mbedtls_ccm_init(&ctx);
    TEST_EQUAL(mbedtls_ccm_setkey(&ctx, cipher_id, key->x, key->len * 8), 0);
    TEST_EQUAL(0, mbedtls_ccm_starts(&ctx, mode, iv->x, iv->len));
    // use hardcoded values for msg length and tag length. They are not a part of this test
    TEST_EQUAL(0, mbedtls_ccm_set_lengths(&ctx, add->len, 16, 16));

    // pass full auth data
    TEST_EQUAL(0, mbedtls_ccm_update_ad(&ctx, add->x, add->len));
    // pass 1 extra byte
    TEST_EQUAL(MBEDTLS_ERR_CCM_BAD_INPUT, mbedtls_ccm_update_ad(&ctx, add->x, 1));
exit:
    mbedtls_ccm_free(&ctx);
}

static void test_mbedtls_ccm_full_ad_and_overflow_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_mbedtls_ccm_full_ad_and_overflow( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6 );
}
#line 672 "suites/test_suite_ccm.function"
static void test_mbedtls_ccm_incomplete_ad_and_overflow(int cipher_id, int mode,
                                            data_t *key, data_t *iv,
                                            data_t *add)
{
    mbedtls_ccm_context ctx;
    uint8_t add_second_buffer[2];

    add_second_buffer[0] = add->x[add->len - 1];
    add_second_buffer[1] = 0xAB; // some magic value

    mbedtls_ccm_init(&ctx);
    TEST_EQUAL(mbedtls_ccm_setkey(&ctx, cipher_id, key->x, key->len * 8), 0);
    TEST_EQUAL(0, mbedtls_ccm_starts(&ctx, mode, iv->x, iv->len));
    // use hardcoded values for msg length and tag length. They are not a part of this test
    TEST_EQUAL(0, mbedtls_ccm_set_lengths(&ctx, add->len, 16, 16));

    // pass incomplete auth data
    TEST_EQUAL(0, mbedtls_ccm_update_ad(&ctx, add->x, add->len - 1));
    // pass 2 extra bytes (1 missing byte from previous incomplete pass, and 1 unexpected byte)
    TEST_EQUAL(MBEDTLS_ERR_CCM_BAD_INPUT, mbedtls_ccm_update_ad(&ctx, add_second_buffer, 2));
exit:
    mbedtls_ccm_free(&ctx);
}

static void test_mbedtls_ccm_incomplete_ad_and_overflow_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_mbedtls_ccm_incomplete_ad_and_overflow( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6 );
}
#line 699 "suites/test_suite_ccm.function"
static void test_mbedtls_ccm_overflow_update(int cipher_id, int mode,
                                 data_t *key, data_t *msg, data_t *iv,
                                 data_t *add)
{
    mbedtls_ccm_context ctx;
    uint8_t *output = NULL;
    size_t olen;

    mbedtls_ccm_init(&ctx);
    TEST_EQUAL(mbedtls_ccm_setkey(&ctx, cipher_id, key->x, key->len * 8), 0);
    TEST_EQUAL(0, mbedtls_ccm_starts(&ctx, mode, iv->x, iv->len));
    // use hardcoded value for tag length. It is a not a part of this test
    // subtract 1 from configured msg length to provoke an overflow
    TEST_EQUAL(0, mbedtls_ccm_set_lengths(&ctx, add->len, msg->len - 1, 16));

    TEST_EQUAL(0, mbedtls_ccm_update_ad(&ctx, add->x, add->len));

    ASSERT_ALLOC(output, msg->len);
    TEST_EQUAL(MBEDTLS_ERR_CCM_BAD_INPUT, \
               mbedtls_ccm_update(&ctx, msg->x, msg->len, output, msg->len, &olen));
exit:
    mbedtls_free(output);
    mbedtls_ccm_free(&ctx);
}

static void test_mbedtls_ccm_overflow_update_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_mbedtls_ccm_overflow_update( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6, &data8 );
}
#line 727 "suites/test_suite_ccm.function"
static void test_mbedtls_ccm_incomplete_update(int cipher_id, int mode,
                                   data_t *key, data_t *msg, data_t *iv,
                                   data_t *add)
{
    mbedtls_ccm_context ctx;
    uint8_t *output = NULL;
    size_t olen;

    mbedtls_ccm_init(&ctx);
    TEST_EQUAL(mbedtls_ccm_setkey(&ctx, cipher_id, key->x, key->len * 8), 0);
    TEST_EQUAL(0, mbedtls_ccm_starts(&ctx, mode, iv->x, iv->len));
    // use hardcoded value for tag length. It is not a part of this test
    TEST_EQUAL(0, mbedtls_ccm_set_lengths(&ctx, add->len, msg->len, 16));

    TEST_EQUAL(0, mbedtls_ccm_update_ad(&ctx, add->x, add->len));

    ASSERT_ALLOC(output, msg->len);
    olen = 0xdeadbeef;
    TEST_EQUAL(0, mbedtls_ccm_update(&ctx, msg->x, msg->len - 1, output, msg->len, &olen));
    mbedtls_free(output);
    output = NULL;

    ASSERT_ALLOC(output, 16);
    TEST_EQUAL(MBEDTLS_ERR_CCM_BAD_INPUT, mbedtls_ccm_finish(&ctx, output, 16));

exit:
    mbedtls_free(output);
    mbedtls_ccm_free(&ctx);
}

static void test_mbedtls_ccm_incomplete_update_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_mbedtls_ccm_incomplete_update( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6, &data8 );
}
#line 761 "suites/test_suite_ccm.function"
static void test_mbedtls_ccm_full_update_and_overflow(int cipher_id, int mode,
                                          data_t *key, data_t *msg, data_t *iv,
                                          data_t *add)
{
    mbedtls_ccm_context ctx;
    uint8_t *output = NULL;
    size_t olen;

    mbedtls_ccm_init(&ctx);
    TEST_EQUAL(mbedtls_ccm_setkey(&ctx, cipher_id, key->x, key->len * 8), 0);
    TEST_EQUAL(0, mbedtls_ccm_starts(&ctx, mode, iv->x, iv->len));
    // use hardcoded value for tag length. It is a not a part of this test
    TEST_EQUAL(0, mbedtls_ccm_set_lengths(&ctx, add->len, msg->len, 16));

    TEST_EQUAL(0, mbedtls_ccm_update_ad(&ctx, add->x, add->len));

    ASSERT_ALLOC(output, msg->len);
    // pass full text
    TEST_EQUAL(0, mbedtls_ccm_update(&ctx, msg->x, msg->len, output, msg->len, &olen));
    // pass 1 extra byte
    TEST_EQUAL(MBEDTLS_ERR_CCM_BAD_INPUT, \
               mbedtls_ccm_update(&ctx, msg->x, 1, output, 1, &olen));
exit:
    mbedtls_free(output);
    mbedtls_ccm_free(&ctx);
}

static void test_mbedtls_ccm_full_update_and_overflow_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_mbedtls_ccm_full_update_and_overflow( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6, &data8 );
}
#line 792 "suites/test_suite_ccm.function"
static void test_mbedtls_ccm_incomplete_update_overflow(int cipher_id, int mode,
                                            data_t *key, data_t *msg, data_t *iv,
                                            data_t *add)
{
    mbedtls_ccm_context ctx;
    uint8_t *output = NULL;
    size_t olen;
    uint8_t msg_second_buffer[2];

    msg_second_buffer[0] = msg->x[msg->len - 1];
    msg_second_buffer[1] = 0xAB; // some magic value

    mbedtls_ccm_init(&ctx);
    TEST_EQUAL(mbedtls_ccm_setkey(&ctx, cipher_id, key->x, key->len * 8), 0);
    TEST_EQUAL(0, mbedtls_ccm_starts(&ctx, mode, iv->x, iv->len));
    // use hardcoded value for tag length. It is a not a part of this test
    TEST_EQUAL(0, mbedtls_ccm_set_lengths(&ctx, add->len, msg->len, 16));

    TEST_EQUAL(0, mbedtls_ccm_update_ad(&ctx, add->x, add->len));

    ASSERT_ALLOC(output, msg->len + 1);
    // pass incomplete text
    TEST_EQUAL(0, mbedtls_ccm_update(&ctx, msg->x, msg->len - 1, output, msg->len + 1, &olen));
    // pass 2 extra bytes (1 missing byte from previous incomplete pass, and 1 unexpected byte)
    TEST_EQUAL(MBEDTLS_ERR_CCM_BAD_INPUT, \
               mbedtls_ccm_update(&ctx, msg_second_buffer, 2, output +  msg->len - 1, 2, &olen));
exit:
    mbedtls_free(output);
    mbedtls_ccm_free(&ctx);
}

static void test_mbedtls_ccm_incomplete_update_overflow_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_mbedtls_ccm_incomplete_update_overflow( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6, &data8 );
}
#line 826 "suites/test_suite_ccm.function"
static void test_mbedtls_ccm_instant_finish(int cipher_id, int mode,
                                data_t *key, data_t *iv)
{
    mbedtls_ccm_context ctx;
    uint8_t *output = NULL;

    mbedtls_ccm_init(&ctx);
    TEST_EQUAL(mbedtls_ccm_setkey(&ctx, cipher_id, key->x, key->len * 8), 0);
    TEST_EQUAL(0, mbedtls_ccm_starts(&ctx, mode, iv->x, iv->len));
    // use hardcoded values for add length, msg length and tag length.
    // They are not a part of this test
    TEST_EQUAL(0, mbedtls_ccm_set_lengths(&ctx, 16, 16, 16));

    ASSERT_ALLOC(output, 16);
    TEST_EQUAL(MBEDTLS_ERR_CCM_BAD_INPUT, mbedtls_ccm_finish(&ctx, output, 16));

exit:
    mbedtls_free(output);
    mbedtls_ccm_free(&ctx);
}

static void test_mbedtls_ccm_instant_finish_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};

    test_mbedtls_ccm_instant_finish( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4 );
}
#endif /* MBEDTLS_CCM_C */


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
    
#if defined(MBEDTLS_CCM_C)

        case 0:
            {
                *out_value = MBEDTLS_CIPHER_ID_AES;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_CIPHER_ID_CAMELLIA;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_ERR_CCM_BAD_INPUT;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_CIPHER_ID_DES;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_ERR_CCM_AUTH_FAILED;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_CCM_ENCRYPT;
            }
            break;
        case 6:
            {
                *out_value = MBEDTLS_CCM_STAR_ENCRYPT;
            }
            break;
        case 7:
            {
                *out_value = MBEDTLS_CCM_DECRYPT;
            }
            break;
        case 8:
            {
                *out_value = MBEDTLS_CCM_STAR_DECRYPT;
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
    
#if defined(MBEDTLS_CCM_C)

        case 0:
            {
#if defined(MBEDTLS_AES_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(MBEDTLS_CAMELLIA_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(MBEDTLS_DES_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 3:
            {
#if !defined(MBEDTLS_CCM_ALT)
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

#if defined(MBEDTLS_CCM_C) && defined(MBEDTLS_SELF_TEST) && defined(MBEDTLS_AES_C)
    test_mbedtls_ccm_self_test_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_setkey_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_CCM_C) && defined(MBEDTLS_AES_C)
    test_ccm_lengths_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_CCM_C) && defined(MBEDTLS_AES_C)
    test_ccm_star_lengths_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_encrypt_and_tag_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_star_no_tag_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_auth_decrypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_star_encrypt_and_tag_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_star_auth_decrypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_skip_ad_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_skip_update_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_overflow_ad_wrapper,
#else
    NULL,
#endif
/* Function Id: 12 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_unexpected_ad_wrapper,
#else
    NULL,
#endif
/* Function Id: 13 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_unexpected_text_wrapper,
#else
    NULL,
#endif
/* Function Id: 14 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_incomplete_ad_wrapper,
#else
    NULL,
#endif
/* Function Id: 15 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_full_ad_and_overflow_wrapper,
#else
    NULL,
#endif
/* Function Id: 16 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_incomplete_ad_and_overflow_wrapper,
#else
    NULL,
#endif
/* Function Id: 17 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_overflow_update_wrapper,
#else
    NULL,
#endif
/* Function Id: 18 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_incomplete_update_wrapper,
#else
    NULL,
#endif
/* Function Id: 19 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_full_update_and_overflow_wrapper,
#else
    NULL,
#endif
/* Function Id: 20 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_incomplete_update_overflow_wrapper,
#else
    NULL,
#endif
/* Function Id: 21 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_instant_finish_wrapper,
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
    const char *default_filename = "./test_suite_ccm.datax";
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
