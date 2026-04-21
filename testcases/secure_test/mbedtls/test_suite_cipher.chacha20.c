#line 2 "suites/main_test.function"
/*
 * *** THIS FILE HAS BEEN MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py
 *
 * Test file      : ./test_suite_cipher.chacha20.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_cipher.function
 *      Test suite data     : suites/test_suite_cipher.chacha20.data
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

#if defined(MBEDTLS_CIPHER_C)
#line 2 "suites/test_suite_cipher.function"
#include "mbedtls/cipher.h"
#include "mbedtls/aes.h"

#if defined(MBEDTLS_GCM_C)
#include "mbedtls/gcm.h"
#endif

#if defined(MBEDTLS_CIPHER_MODE_AEAD) || defined(MBEDTLS_NIST_KW_C)
#define MBEDTLS_CIPHER_AUTH_CRYPT
#endif

/* Check the internal consistency of a cipher info structure, and
 * check it against mbedtls_cipher_info_from_xxx(). */
static int check_cipher_info(mbedtls_cipher_type_t type,
                             const mbedtls_cipher_info_t *info)
{
    size_t key_bitlen, block_size, iv_size;

    TEST_ASSERT(info != NULL);
    TEST_EQUAL(type, mbedtls_cipher_info_get_type(info));
    TEST_EQUAL(type, info->type);
    TEST_ASSERT(mbedtls_cipher_info_from_type(type) == info);

    TEST_EQUAL(info->mode, mbedtls_cipher_info_get_mode(info));

    /* Insist that get_name() return the string from the structure and
     * not a copy. A copy would have an unknown storage duration. */
    TEST_ASSERT(mbedtls_cipher_info_get_name(info) == info->name);
    TEST_ASSERT(mbedtls_cipher_info_from_string(info->name) == info);

    key_bitlen = mbedtls_cipher_info_get_key_bitlen(info);
    block_size = mbedtls_cipher_info_get_block_size(info);
    iv_size = mbedtls_cipher_info_get_iv_size(info);
    if (info->type == MBEDTLS_CIPHER_NULL) {
        TEST_ASSERT(key_bitlen == 0);
        TEST_ASSERT(block_size == 1);
        TEST_ASSERT(iv_size == 0);
    } else if (info->mode == MBEDTLS_MODE_XTS) {
        TEST_ASSERT(key_bitlen == 256 ||
                    key_bitlen == 384 ||
                    key_bitlen == 512);
    } else if (!strncmp(info->name, "DES-EDE3-", 9)) {
        TEST_ASSERT(key_bitlen == 192);
        TEST_ASSERT(!mbedtls_cipher_info_has_variable_key_bitlen(info));
        TEST_ASSERT(block_size == 8);
    } else if (!strncmp(info->name, "DES-EDE-", 8)) {
        TEST_ASSERT(key_bitlen == 128);
        TEST_ASSERT(!mbedtls_cipher_info_has_variable_key_bitlen(info));
        TEST_ASSERT(block_size == 8);
    } else if (!strncmp(info->name, "DES-", 4)) {
        TEST_ASSERT(key_bitlen == 64);
        TEST_ASSERT(!mbedtls_cipher_info_has_variable_key_bitlen(info));
        TEST_ASSERT(block_size == 8);
    } else if (!strncmp(info->name, "AES", 3)) {
        TEST_ASSERT(key_bitlen == 128 ||
                    key_bitlen == 192 ||
                    key_bitlen == 256);
        TEST_ASSERT(!mbedtls_cipher_info_has_variable_key_bitlen(info));
        TEST_ASSERT(block_size == 16);
    } else {
        TEST_ASSERT(key_bitlen == 128 ||
                    key_bitlen == 192 ||
                    key_bitlen == 256);
    }

    if (strstr(info->name, "-ECB") != NULL) {
        TEST_ASSERT(iv_size == 0);
        TEST_ASSERT(!mbedtls_cipher_info_has_variable_iv_size(info));
    } else if (strstr(info->name, "-CBC") != NULL ||
               strstr(info->name, "-CTR") != NULL) {
        TEST_ASSERT(iv_size == block_size);
        TEST_ASSERT(!mbedtls_cipher_info_has_variable_iv_size(info));
    } else if (strstr(info->name, "-GCM") != NULL) {
        TEST_ASSERT(iv_size == block_size - 4);
        TEST_ASSERT(mbedtls_cipher_info_has_variable_iv_size(info));
    }

    return 1;

exit:
    return 0;
}

#if defined(MBEDTLS_CIPHER_AUTH_CRYPT)
/* Helper for resetting key/direction
 *
 * The documentation doesn't explicitly say whether calling
 * mbedtls_cipher_setkey() twice is allowed or not. This currently works with
 * the default software implementation, but only by accident. It isn't
 * guaranteed to work with new ciphers or with alternative implementations of
 * individual ciphers, and it doesn't work with the PSA wrappers. So don't do
 * it, and instead start with a fresh context.
 */
static int cipher_reset_key(mbedtls_cipher_context_t *ctx, int cipher_id,
                            int use_psa, size_t tag_len, const data_t *key, int direction)
{
    mbedtls_cipher_free(ctx);
    mbedtls_cipher_init(ctx);

#if !defined(MBEDTLS_USE_PSA_CRYPTO) || !defined(MBEDTLS_TEST_DEPRECATED)
    (void) use_psa;
    (void) tag_len;
#else
    if (use_psa == 1) {
        TEST_ASSERT(0 == mbedtls_cipher_setup_psa(ctx,
                                                  mbedtls_cipher_info_from_type(cipher_id),
                                                  tag_len));
    } else
#endif /* !MBEDTLS_USE_PSA_CRYPTO || !MBEDTLS_TEST_DEPRECATED */
    {
        TEST_ASSERT(0 == mbedtls_cipher_setup(ctx,
                                              mbedtls_cipher_info_from_type(cipher_id)));
    }

    TEST_ASSERT(0 == mbedtls_cipher_setkey(ctx, key->x, 8 * key->len,
                                           direction));
    return 1;

exit:
    return 0;
}

/*
 * Check if a buffer is all-0 bytes:
 * return   1 if it is,
 *          0 if it isn't.
 */
static int buffer_is_all_zero(const uint8_t *buf, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        if (buf[i] != 0) {
            return 0;
        }
    }
    return 1;
}
#endif /* MBEDTLS_CIPHER_AUTH_CRYPT */

#line 148 "suites/test_suite_cipher.function"
static void test_mbedtls_cipher_list(void)
{
    const int *cipher_type;

    for (cipher_type = mbedtls_cipher_list(); *cipher_type != 0; cipher_type++) {
        const mbedtls_cipher_info_t *info =
            mbedtls_cipher_info_from_type(*cipher_type);
        mbedtls_test_set_step(*cipher_type);
        if (!check_cipher_info(*cipher_type, info)) {
            goto exit;
        }
    }
exit:
    ;
}

static void test_mbedtls_cipher_list_wrapper( void ** params )
{
    (void)params;

    test_mbedtls_cipher_list(  );
}
#line 164 "suites/test_suite_cipher.function"
static void test_cipher_invalid_param_unconditional(void)
{
    mbedtls_cipher_context_t valid_ctx;
    mbedtls_cipher_context_t invalid_ctx;
    mbedtls_operation_t valid_operation = MBEDTLS_ENCRYPT;
    mbedtls_cipher_padding_t valid_mode = MBEDTLS_PADDING_ZEROS;
    unsigned char valid_buffer[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
    int valid_size = sizeof(valid_buffer);
    int valid_bitlen = valid_size * 8;
    const mbedtls_cipher_info_t *valid_info = mbedtls_cipher_info_from_type(
        *(mbedtls_cipher_list()));
    size_t size_t_var;

    (void) valid_mode; /* In some configurations this is unused */

    mbedtls_cipher_init(&valid_ctx);
    mbedtls_cipher_init(&invalid_ctx);

    TEST_ASSERT(mbedtls_cipher_setup(&valid_ctx, valid_info) == 0);

    /* mbedtls_cipher_setup() */
    TEST_ASSERT(mbedtls_cipher_setup(&valid_ctx, NULL) ==
                MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    /* mbedtls_cipher_get_block_size() */
    TEST_ASSERT(mbedtls_cipher_get_block_size(&invalid_ctx) == 0);

    /* mbedtls_cipher_get_cipher_mode() */
    TEST_ASSERT(mbedtls_cipher_get_cipher_mode(&invalid_ctx) ==
                MBEDTLS_MODE_NONE);

    /* mbedtls_cipher_get_iv_size() */
    TEST_ASSERT(mbedtls_cipher_get_iv_size(&invalid_ctx) == 0);

    /* mbedtls_cipher_get_type() */
    TEST_ASSERT(
        mbedtls_cipher_get_type(&invalid_ctx) ==
        MBEDTLS_CIPHER_NONE);

    /* mbedtls_cipher_get_name() */
    TEST_ASSERT(mbedtls_cipher_get_name(&invalid_ctx) == 0);

    /* mbedtls_cipher_get_key_bitlen() */
    TEST_ASSERT(mbedtls_cipher_get_key_bitlen(&invalid_ctx) ==
                MBEDTLS_KEY_LENGTH_NONE);

    /* mbedtls_cipher_get_operation() */
    TEST_ASSERT(mbedtls_cipher_get_operation(&invalid_ctx) ==
                MBEDTLS_OPERATION_NONE);

    /* mbedtls_cipher_setkey() */
    TEST_ASSERT(
        mbedtls_cipher_setkey(&invalid_ctx,
                              valid_buffer,
                              valid_bitlen,
                              valid_operation) ==
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    /* mbedtls_cipher_set_iv() */
    TEST_ASSERT(
        mbedtls_cipher_set_iv(&invalid_ctx,
                              valid_buffer,
                              valid_size) ==
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    /* mbedtls_cipher_reset() */
    TEST_ASSERT(mbedtls_cipher_reset(&invalid_ctx) ==
                MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
    /* mbedtls_cipher_update_ad() */
    TEST_ASSERT(
        mbedtls_cipher_update_ad(&invalid_ctx,
                                 valid_buffer,
                                 valid_size) ==
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);
#endif /* defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C) */

#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
    /* mbedtls_cipher_set_padding_mode() */
    TEST_ASSERT(mbedtls_cipher_set_padding_mode(&invalid_ctx, valid_mode) ==
                MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);
#endif

    /* mbedtls_cipher_update() */
    TEST_ASSERT(
        mbedtls_cipher_update(&invalid_ctx,
                              valid_buffer,
                              valid_size,
                              valid_buffer,
                              &size_t_var) ==
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    /* mbedtls_cipher_finish() */
    TEST_ASSERT(
        mbedtls_cipher_finish(&invalid_ctx,
                              valid_buffer,
                              &size_t_var) ==
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
    /* mbedtls_cipher_write_tag() */
    TEST_ASSERT(
        mbedtls_cipher_write_tag(&invalid_ctx,
                                 valid_buffer,
                                 valid_size) ==
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    /* mbedtls_cipher_check_tag() */
    TEST_ASSERT(
        mbedtls_cipher_check_tag(&invalid_ctx,
                                 valid_buffer,
                                 valid_size) ==
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);
#endif /* defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C) */

exit:
    mbedtls_cipher_free(&invalid_ctx);
    mbedtls_cipher_free(&valid_ctx);
}

static void test_cipher_invalid_param_unconditional_wrapper( void ** params )
{
    (void)params;

    test_cipher_invalid_param_unconditional(  );
}
#line 287 "suites/test_suite_cipher.function"
static void test_cipher_invalid_param_conditional(void)
{
    mbedtls_cipher_context_t valid_ctx;

    mbedtls_operation_t invalid_operation = 100;
    unsigned char valid_buffer[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
    int valid_size = sizeof(valid_buffer);
    int valid_bitlen = valid_size * 8;

    TEST_EQUAL(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_setkey(&valid_ctx,
                              valid_buffer,
                              valid_bitlen,
                              invalid_operation));

exit:
    ;
}

static void test_cipher_invalid_param_conditional_wrapper( void ** params )
{
    (void)params;

    test_cipher_invalid_param_conditional(  );
}
#if defined(MBEDTLS_AES_C)
#line 309 "suites/test_suite_cipher.function"
static void test_cipher_special_behaviours(void)
{
    const mbedtls_cipher_info_t *cipher_info;
    mbedtls_cipher_context_t ctx;
    unsigned char input[32];
    unsigned char output[32];
#if defined(MBEDTLS_CIPHER_MODE_CBC)
    unsigned char iv[32];
#endif
    size_t olen = 0;

    mbedtls_cipher_init(&ctx);
    memset(input, 0, sizeof(input));
    memset(output, 0, sizeof(output));
#if defined(MBEDTLS_CIPHER_MODE_CBC)
    memset(iv, 0, sizeof(iv));

    /* Check and get info structures */
    cipher_info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_CBC);
    TEST_ASSERT(NULL != cipher_info);

    TEST_ASSERT(0 == mbedtls_cipher_setup(&ctx, cipher_info));

    /* IV too big */
    TEST_ASSERT(mbedtls_cipher_set_iv(&ctx, iv, MBEDTLS_MAX_IV_LENGTH + 1)
                == MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE);

    /* IV too small */
    TEST_ASSERT(mbedtls_cipher_set_iv(&ctx, iv, 0)
                == MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    mbedtls_cipher_free(&ctx);
    mbedtls_cipher_init(&ctx);
#endif /* MBEDTLS_CIPHER_MODE_CBC */
    cipher_info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_ECB);
    TEST_ASSERT(NULL != cipher_info);

    TEST_ASSERT(0 == mbedtls_cipher_setup(&ctx, cipher_info));

    /* Update ECB with partial block */
    TEST_ASSERT(mbedtls_cipher_update(&ctx, input, 1, output, &olen)
                == MBEDTLS_ERR_CIPHER_FULL_BLOCK_EXPECTED);

exit:
    mbedtls_cipher_free(&ctx);
}

static void test_cipher_special_behaviours_wrapper( void ** params )
{
    (void)params;

    test_cipher_special_behaviours(  );
}
#endif /* MBEDTLS_AES_C */
#line 358 "suites/test_suite_cipher.function"
static void test_enc_dec_buf(int cipher_id, char *cipher_string, int key_len,
                 int length_val, int pad_mode)
{
    size_t length = length_val, outlen, total_len, i, block_size, iv_len;
    unsigned char key[64];
    unsigned char iv[16];
    unsigned char ad[13];
    unsigned char tag[16];
    unsigned char inbuf[64];
    unsigned char encbuf[64];
    unsigned char decbuf[64];

    const mbedtls_cipher_info_t *cipher_info;
    mbedtls_cipher_context_t ctx_dec;
    mbedtls_cipher_context_t ctx_enc;

    /*
     * Prepare contexts
     */
    mbedtls_cipher_init(&ctx_dec);
    mbedtls_cipher_init(&ctx_enc);

    memset(key, 0x2a, sizeof(key));

    /* Check and get info structures */
    cipher_info = mbedtls_cipher_info_from_type(cipher_id);
    TEST_ASSERT(NULL != cipher_info);
    TEST_ASSERT(mbedtls_cipher_info_from_string(cipher_string) == cipher_info);
    TEST_ASSERT(strcmp(mbedtls_cipher_info_get_name(cipher_info),
                       cipher_string) == 0);

    /* Initialise enc and dec contexts */
    TEST_ASSERT(0 == mbedtls_cipher_setup(&ctx_dec, cipher_info));
    TEST_ASSERT(0 == mbedtls_cipher_setup(&ctx_enc, cipher_info));

    TEST_ASSERT(0 == mbedtls_cipher_setkey(&ctx_dec, key, key_len, MBEDTLS_DECRYPT));
    TEST_ASSERT(0 == mbedtls_cipher_setkey(&ctx_enc, key, key_len, MBEDTLS_ENCRYPT));

#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
    if (-1 != pad_mode) {
        TEST_ASSERT(0 == mbedtls_cipher_set_padding_mode(&ctx_dec, pad_mode));
        TEST_ASSERT(0 == mbedtls_cipher_set_padding_mode(&ctx_enc, pad_mode));
    }
#else
    (void) pad_mode;
#endif /* MBEDTLS_CIPHER_MODE_WITH_PADDING */

    /*
     * Do a few encode/decode cycles
     */
    for (i = 0; i < 3; i++) {
        memset(iv, 0x00 + i, sizeof(iv));
        memset(ad, 0x10 + i, sizeof(ad));
        memset(inbuf, 0x20 + i, sizeof(inbuf));

        memset(encbuf, 0, sizeof(encbuf));
        memset(decbuf, 0, sizeof(decbuf));
        memset(tag, 0, sizeof(tag));

        if (NULL != strstr(cipher_info->name, "CCM*-NO-TAG")) {
            iv_len = 13; /* For CCM, IV length is expected to be between 7 and 13 bytes.
                          * For CCM*-NO-TAG, IV length must be exactly 13 bytes long. */
        } else if (cipher_info->type == MBEDTLS_CIPHER_CHACHA20 ||
                   cipher_info->type == MBEDTLS_CIPHER_CHACHA20_POLY1305) {
            iv_len = 12;
        } else {
            iv_len = sizeof(iv);
        }

        TEST_ASSERT(0 == mbedtls_cipher_set_iv(&ctx_dec, iv, iv_len));
        TEST_ASSERT(0 == mbedtls_cipher_set_iv(&ctx_enc, iv, iv_len));

        TEST_ASSERT(0 == mbedtls_cipher_reset(&ctx_dec));
        TEST_ASSERT(0 == mbedtls_cipher_reset(&ctx_enc));

#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
        int expected = (cipher_info->mode == MBEDTLS_MODE_GCM ||
                        cipher_info->type == MBEDTLS_CIPHER_CHACHA20_POLY1305) ?
                       0 : MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE;

        TEST_EQUAL(expected, mbedtls_cipher_update_ad(&ctx_dec, ad, sizeof(ad) - i));
        TEST_EQUAL(expected, mbedtls_cipher_update_ad(&ctx_enc, ad, sizeof(ad) - i));
#endif

        block_size = mbedtls_cipher_get_block_size(&ctx_enc);
        TEST_ASSERT(block_size != 0);

        /* encode length number of bytes from inbuf */
        TEST_ASSERT(0 == mbedtls_cipher_update(&ctx_enc, inbuf, length, encbuf, &outlen));
        total_len = outlen;

        TEST_ASSERT(total_len == length ||
                    (total_len % block_size == 0 &&
                     total_len < length &&
                     total_len + block_size > length));

        TEST_ASSERT(0 == mbedtls_cipher_finish(&ctx_enc, encbuf + outlen, &outlen));
        total_len += outlen;

#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
        TEST_EQUAL(expected, mbedtls_cipher_write_tag(&ctx_enc, tag, sizeof(tag)));
#endif

        TEST_ASSERT(total_len == length ||
                    (total_len % block_size == 0 &&
                     total_len > length &&
                     total_len <= length + block_size));

        /* decode the previously encoded string */
        TEST_ASSERT(0 == mbedtls_cipher_update(&ctx_dec, encbuf, total_len, decbuf, &outlen));
        total_len = outlen;

        TEST_ASSERT(total_len == length ||
                    (total_len % block_size == 0 &&
                     total_len < length &&
                     total_len + block_size >= length));

        TEST_ASSERT(0 == mbedtls_cipher_finish(&ctx_dec, decbuf + outlen, &outlen));
        total_len += outlen;

#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
        TEST_EQUAL(expected, mbedtls_cipher_check_tag(&ctx_dec, tag, sizeof(tag)));
#endif

        /* check result */
        TEST_ASSERT(total_len == length);
        TEST_ASSERT(0 == memcmp(inbuf, decbuf, length));
    }

    /*
     * Done
     */
exit:
    mbedtls_cipher_free(&ctx_dec);
    mbedtls_cipher_free(&ctx_enc);
}

static void test_enc_dec_buf_wrapper( void ** params )
{

    test_enc_dec_buf( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ) );
}
#line 497 "suites/test_suite_cipher.function"
static void test_enc_fail(int cipher_id, int pad_mode, int key_len, int length_val,
              int ret)
{
    size_t length = length_val;
    unsigned char key[32];
    unsigned char iv[16];

    const mbedtls_cipher_info_t *cipher_info;
    mbedtls_cipher_context_t ctx;

    unsigned char inbuf[64];
    unsigned char encbuf[64];

    size_t outlen = 0;

    memset(key, 0, 32);
    memset(iv, 0, 16);

    mbedtls_cipher_init(&ctx);

    memset(inbuf, 5, 64);
    memset(encbuf, 0, 64);

    /* Check and get info structures */
    cipher_info = mbedtls_cipher_info_from_type(cipher_id);
    TEST_ASSERT(NULL != cipher_info);

    /* Initialise context */
    TEST_ASSERT(0 == mbedtls_cipher_setup(&ctx, cipher_info));
    TEST_ASSERT(0 == mbedtls_cipher_setkey(&ctx, key, key_len, MBEDTLS_ENCRYPT));
#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
    TEST_ASSERT(0 == mbedtls_cipher_set_padding_mode(&ctx, pad_mode));
#else
    (void) pad_mode;
#endif /* MBEDTLS_CIPHER_MODE_WITH_PADDING */
    TEST_ASSERT(0 == mbedtls_cipher_set_iv(&ctx, iv, 16));
    TEST_ASSERT(0 == mbedtls_cipher_reset(&ctx));
#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
    int expected = (cipher_info->mode == MBEDTLS_MODE_GCM ||
                    cipher_info->type == MBEDTLS_CIPHER_CHACHA20_POLY1305) ?
                   0 : MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE;

    TEST_EQUAL(expected, mbedtls_cipher_update_ad(&ctx, NULL, 0));
#endif

    /* encode length number of bytes from inbuf */
    TEST_ASSERT(0 == mbedtls_cipher_update(&ctx, inbuf, length, encbuf, &outlen));
    TEST_ASSERT(ret == mbedtls_cipher_finish(&ctx, encbuf + outlen, &outlen));

    /* done */
exit:
    mbedtls_cipher_free(&ctx);
}

static void test_enc_fail_wrapper( void ** params )
{

    test_enc_fail( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ) );
}
#line 553 "suites/test_suite_cipher.function"
static void test_dec_empty_buf(int cipher,
                   int expected_update_ret,
                   int expected_finish_ret)
{
    unsigned char key[32];

    unsigned char *iv = NULL;
    size_t iv_len = 16;

    mbedtls_cipher_context_t ctx_dec;
    const mbedtls_cipher_info_t *cipher_info;

    unsigned char encbuf[64];
    unsigned char decbuf[64];

    size_t outlen = 0;

    memset(key, 0, 32);

    mbedtls_cipher_init(&ctx_dec);

    memset(encbuf, 0, 64);
    memset(decbuf, 0, 64);

    /* Initialise context */
    cipher_info = mbedtls_cipher_info_from_type(cipher);
    TEST_ASSERT(NULL != cipher_info);

    if (cipher_info->type == MBEDTLS_CIPHER_CHACHA20 ||
        cipher_info->type == MBEDTLS_CIPHER_CHACHA20_POLY1305) {
        iv_len = 12;
    }

    ASSERT_ALLOC(iv, iv_len);
    memset(iv, 0, iv_len);

    TEST_ASSERT(sizeof(key) * 8 >= cipher_info->key_bitlen);

    TEST_ASSERT(0 == mbedtls_cipher_setup(&ctx_dec, cipher_info));

    TEST_ASSERT(0 == mbedtls_cipher_setkey(&ctx_dec,
                                           key, cipher_info->key_bitlen,
                                           MBEDTLS_DECRYPT));

    TEST_ASSERT(0 == mbedtls_cipher_set_iv(&ctx_dec, iv, iv_len));

    TEST_ASSERT(0 == mbedtls_cipher_reset(&ctx_dec));

#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
    int expected = (cipher_info->mode == MBEDTLS_MODE_GCM ||
                    cipher_info->type == MBEDTLS_CIPHER_CHACHA20_POLY1305) ?
                   0 : MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE;

    TEST_EQUAL(expected, mbedtls_cipher_update_ad(&ctx_dec, NULL, 0));
#endif

    /* decode 0-byte string */
    TEST_ASSERT(expected_update_ret ==
                mbedtls_cipher_update(&ctx_dec, encbuf, 0, decbuf, &outlen));
    TEST_ASSERT(0 == outlen);

    if (expected_finish_ret == 0 &&
        (cipher_info->mode == MBEDTLS_MODE_CBC ||
         cipher_info->mode == MBEDTLS_MODE_ECB)) {
        /* Non-CBC and non-ECB ciphers are OK with decrypting empty buffers and
         * return success, not MBEDTLS_ERR_CIPHER_FULL_BLOCK_EXPECTED, when
         * decrypting an empty buffer.
         * On the other hand, CBC and ECB ciphers need a full block of input.
         */
        expected_finish_ret = MBEDTLS_ERR_CIPHER_FULL_BLOCK_EXPECTED;
    }

    TEST_ASSERT(expected_finish_ret == mbedtls_cipher_finish(
                    &ctx_dec, decbuf + outlen, &outlen));
    TEST_ASSERT(0 == outlen);

exit:
    mbedtls_free(iv);
    mbedtls_cipher_free(&ctx_dec);
}

static void test_dec_empty_buf_wrapper( void ** params )
{

    test_dec_empty_buf( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
#line 636 "suites/test_suite_cipher.function"
static void test_enc_dec_buf_multipart(int cipher_id, int key_len, int first_length_val,
                           int second_length_val, int pad_mode,
                           int first_encrypt_output_len, int second_encrypt_output_len,
                           int first_decrypt_output_len, int second_decrypt_output_len)
{
    size_t first_length = first_length_val;
    size_t second_length = second_length_val;
    size_t length = first_length + second_length;
    size_t block_size;
    size_t iv_len;
    unsigned char key[32];
    unsigned char iv[16];

    mbedtls_cipher_context_t ctx_dec;
    mbedtls_cipher_context_t ctx_enc;
    const mbedtls_cipher_info_t *cipher_info;

    unsigned char inbuf[64];
    unsigned char encbuf[64];
    unsigned char decbuf[64];

    size_t outlen = 0;
    size_t totaloutlen = 0;

    memset(key, 0, 32);
    memset(iv, 0, 16);

    mbedtls_cipher_init(&ctx_dec);
    mbedtls_cipher_init(&ctx_enc);

    memset(inbuf, 5, 64);
    memset(encbuf, 0, 64);
    memset(decbuf, 0, 64);

    /* Initialise enc and dec contexts */
    cipher_info = mbedtls_cipher_info_from_type(cipher_id);
    TEST_ASSERT(NULL != cipher_info);

    TEST_ASSERT(0 == mbedtls_cipher_setup(&ctx_dec, cipher_info));
    TEST_ASSERT(0 == mbedtls_cipher_setup(&ctx_enc, cipher_info));

    TEST_ASSERT(0 == mbedtls_cipher_setkey(&ctx_dec, key, key_len, MBEDTLS_DECRYPT));
    TEST_ASSERT(0 == mbedtls_cipher_setkey(&ctx_enc, key, key_len, MBEDTLS_ENCRYPT));

#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
    if (-1 != pad_mode) {
        TEST_ASSERT(0 == mbedtls_cipher_set_padding_mode(&ctx_dec, pad_mode));
        TEST_ASSERT(0 == mbedtls_cipher_set_padding_mode(&ctx_enc, pad_mode));
    }
#else
    (void) pad_mode;
#endif /* MBEDTLS_CIPHER_MODE_WITH_PADDING */

    if (NULL != strstr(cipher_info->name, "CCM*-NO-TAG")) {
        iv_len = 13; /* For CCM, IV length is expected to be between 7 and 13 bytes.
                      * For CCM*-NO-TAG, IV length must be exactly 13 bytes long. */
    } else if (cipher_info->type == MBEDTLS_CIPHER_CHACHA20 ||
               cipher_info->type == MBEDTLS_CIPHER_CHACHA20_POLY1305) {
        iv_len = 12;
    } else {
        iv_len = sizeof(iv);
    }

    TEST_ASSERT(0 == mbedtls_cipher_set_iv(&ctx_dec, iv, iv_len));
    TEST_ASSERT(0 == mbedtls_cipher_set_iv(&ctx_enc, iv, iv_len));

    TEST_ASSERT(0 == mbedtls_cipher_reset(&ctx_dec));
    TEST_ASSERT(0 == mbedtls_cipher_reset(&ctx_enc));

#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
    int expected = (cipher_info->mode == MBEDTLS_MODE_GCM ||
                    cipher_info->type == MBEDTLS_CIPHER_CHACHA20_POLY1305) ?
                   0 : MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE;

    TEST_EQUAL(expected, mbedtls_cipher_update_ad(&ctx_dec, NULL, 0));
    TEST_EQUAL(expected, mbedtls_cipher_update_ad(&ctx_enc, NULL, 0));
#endif

    block_size = mbedtls_cipher_get_block_size(&ctx_enc);
    TEST_ASSERT(block_size != 0);

    /* encode length number of bytes from inbuf */
    TEST_ASSERT(0 == mbedtls_cipher_update(&ctx_enc, inbuf, first_length, encbuf, &outlen));
    TEST_ASSERT((size_t) first_encrypt_output_len == outlen);
    totaloutlen = outlen;
    TEST_ASSERT(0 ==
                mbedtls_cipher_update(&ctx_enc, inbuf + first_length, second_length,
                                      encbuf + totaloutlen,
                                      &outlen));
    TEST_ASSERT((size_t) second_encrypt_output_len == outlen);
    totaloutlen += outlen;
    TEST_ASSERT(totaloutlen == length ||
                (totaloutlen % block_size == 0 &&
                 totaloutlen < length &&
                 totaloutlen + block_size > length));

    TEST_ASSERT(0 == mbedtls_cipher_finish(&ctx_enc, encbuf + totaloutlen, &outlen));
    totaloutlen += outlen;
    TEST_ASSERT(totaloutlen == length ||
                (totaloutlen % block_size == 0 &&
                 totaloutlen > length &&
                 totaloutlen <= length + block_size));

    /* decode the previously encoded string */
    second_length = totaloutlen - first_length;
    TEST_ASSERT(0 == mbedtls_cipher_update(&ctx_dec, encbuf, first_length, decbuf, &outlen));
    TEST_ASSERT((size_t) first_decrypt_output_len == outlen);
    totaloutlen = outlen;
    TEST_ASSERT(0 ==
                mbedtls_cipher_update(&ctx_dec, encbuf + first_length, second_length,
                                      decbuf + totaloutlen,
                                      &outlen));
    TEST_ASSERT((size_t) second_decrypt_output_len == outlen);
    totaloutlen += outlen;

    TEST_ASSERT(totaloutlen == length ||
                (totaloutlen % block_size == 0 &&
                 totaloutlen < length &&
                 totaloutlen + block_size >= length));

    TEST_ASSERT(0 == mbedtls_cipher_finish(&ctx_dec, decbuf + totaloutlen, &outlen));
    totaloutlen += outlen;

    TEST_ASSERT(totaloutlen == length);

    TEST_ASSERT(0 == memcmp(inbuf, decbuf, length));

exit:
    mbedtls_cipher_free(&ctx_dec);
    mbedtls_cipher_free(&ctx_enc);
}

static void test_enc_dec_buf_multipart_wrapper( void ** params )
{

    test_enc_dec_buf_multipart( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), *( (int *) params[6] ), *( (int *) params[7] ), *( (int *) params[8] ) );
}
#line 770 "suites/test_suite_cipher.function"
static void test_decrypt_test_vec(int cipher_id, int pad_mode, data_t *key,
                      data_t *iv, data_t *cipher,
                      data_t *clear, data_t *ad, data_t *tag,
                      int finish_result, int tag_result)
{
    unsigned char output[265];
    mbedtls_cipher_context_t ctx;
    size_t outlen, total_len;

    mbedtls_cipher_init(&ctx);

    memset(output, 0x00, sizeof(output));

#if !defined(MBEDTLS_GCM_C) && !defined(MBEDTLS_CHACHAPOLY_C)
    ((void) ad);
    ((void) tag);
#endif

    /* Prepare context */
    TEST_ASSERT(0 == mbedtls_cipher_setup(&ctx,
                                          mbedtls_cipher_info_from_type(cipher_id)));
    TEST_ASSERT(0 == mbedtls_cipher_setkey(&ctx, key->x, 8 * key->len, MBEDTLS_DECRYPT));
#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
    if (pad_mode != -1) {
        TEST_ASSERT(0 == mbedtls_cipher_set_padding_mode(&ctx, pad_mode));
    }
#else
    (void) pad_mode;
#endif /* MBEDTLS_CIPHER_MODE_WITH_PADDING */
    TEST_ASSERT(0 == mbedtls_cipher_set_iv(&ctx, iv->x, iv->len));
    TEST_ASSERT(0 == mbedtls_cipher_reset(&ctx));
#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
    int expected = (ctx.cipher_info->mode == MBEDTLS_MODE_GCM ||
                    ctx.cipher_info->type == MBEDTLS_CIPHER_CHACHA20_POLY1305) ?
                   0 : MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE;

    TEST_EQUAL(expected, mbedtls_cipher_update_ad(&ctx, ad->x, ad->len));
#endif

    /* decode buffer and check tag->x */
    total_len = 0;
    TEST_ASSERT(0 == mbedtls_cipher_update(&ctx, cipher->x, cipher->len, output, &outlen));
    total_len += outlen;
    TEST_ASSERT(finish_result == mbedtls_cipher_finish(&ctx, output + outlen,
                                                       &outlen));
    total_len += outlen;
#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
    int tag_expected = (ctx.cipher_info->mode == MBEDTLS_MODE_GCM ||
                        ctx.cipher_info->type == MBEDTLS_CIPHER_CHACHA20_POLY1305) ?
                       tag_result : MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE;

    TEST_EQUAL(tag_expected, mbedtls_cipher_check_tag(&ctx, tag->x, tag->len));
#endif

    /* check plaintext only if everything went fine */
    if (0 == finish_result && 0 == tag_result) {
        TEST_ASSERT(total_len == clear->len);
        TEST_ASSERT(0 == memcmp(output, clear->x, clear->len));
    }

exit:
    mbedtls_cipher_free(&ctx);
}

static void test_decrypt_test_vec_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};
    data_t data12 = {(uint8_t *) params[12], *( (uint32_t *) params[13] )};

    test_decrypt_test_vec( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6, &data8, &data10, &data12, *( (int *) params[14] ), *( (int *) params[15] ) );
}
#if defined(MBEDTLS_CIPHER_AUTH_CRYPT)
#line 836 "suites/test_suite_cipher.function"
static void test_auth_crypt_tv(int cipher_id, data_t *key, data_t *iv,
                   data_t *ad, data_t *cipher, data_t *tag,
                   char *result, data_t *clear, int use_psa)
{
    /*
     * Take an AEAD ciphertext + tag and perform a pair
     * of AEAD decryption and AEAD encryption. Check that
     * this results in the expected plaintext, and that
     * decryption and encryption are inverse to one another.
     */

    int ret;
    int using_nist_kw, using_nist_kw_padding;

    mbedtls_cipher_context_t ctx;
    size_t outlen;

    unsigned char *cipher_plus_tag = NULL;
    size_t cipher_plus_tag_len;
    unsigned char *decrypt_buf = NULL;
    size_t decrypt_buf_len = 0;
    unsigned char *encrypt_buf = NULL;
    size_t encrypt_buf_len = 0;

    /* Null pointers are documented as valid for inputs of length 0.
     * The test framework passes non-null pointers, so set them to NULL.
     * key, cipher and tag can't be empty. */
    if (iv->len == 0) {
        iv->x = NULL;
    }
    if (ad->len == 0) {
        ad->x = NULL;
    }
    if (clear->len == 0) {
        clear->x = NULL;
    }

    mbedtls_cipher_init(&ctx);

    /* Initialize PSA Crypto */
#if defined(MBEDTLS_USE_PSA_CRYPTO)
    if (use_psa == 1) {
        PSA_ASSERT(psa_crypto_init());
    }
#else
    (void) use_psa;
#endif

    /*
     * Are we using NIST_KW? with padding?
     */
    using_nist_kw_padding = cipher_id == MBEDTLS_CIPHER_AES_128_KWP ||
                            cipher_id == MBEDTLS_CIPHER_AES_192_KWP ||
                            cipher_id == MBEDTLS_CIPHER_AES_256_KWP;
    using_nist_kw = cipher_id == MBEDTLS_CIPHER_AES_128_KW ||
                    cipher_id == MBEDTLS_CIPHER_AES_192_KW ||
                    cipher_id == MBEDTLS_CIPHER_AES_256_KW ||
                    using_nist_kw_padding;

    /*
     * Prepare context for decryption
     */
    if (!cipher_reset_key(&ctx, cipher_id, use_psa, tag->len, key,
                          MBEDTLS_DECRYPT)) {
        goto exit;
    }

    /*
     * prepare buffer for decryption
     * (we need the tag appended to the ciphertext)
     */
    cipher_plus_tag_len = cipher->len + tag->len;
    ASSERT_ALLOC(cipher_plus_tag, cipher_plus_tag_len);
    memcpy(cipher_plus_tag, cipher->x, cipher->len);
    memcpy(cipher_plus_tag + cipher->len, tag->x, tag->len);

    /*
     * Compute length of output buffer according to the documentation
     */
    if (using_nist_kw) {
        decrypt_buf_len = cipher_plus_tag_len - 8;
    } else {
        decrypt_buf_len = cipher_plus_tag_len - tag->len;
    }


    /*
     * Try decrypting to a buffer that's 1B too small
     */
    if (decrypt_buf_len != 0) {
        ASSERT_ALLOC(decrypt_buf, decrypt_buf_len - 1);

        outlen = 0;
        ret = mbedtls_cipher_auth_decrypt_ext(&ctx, iv->x, iv->len,
                                              ad->x, ad->len, cipher_plus_tag, cipher_plus_tag_len,
                                              decrypt_buf, decrypt_buf_len - 1, &outlen, tag->len);
        TEST_ASSERT(ret == MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

        mbedtls_free(decrypt_buf);
        decrypt_buf = NULL;
    }

    /*
     * Authenticate and decrypt, and check result
     */
    ASSERT_ALLOC(decrypt_buf, decrypt_buf_len);

    outlen = 0;
    ret = mbedtls_cipher_auth_decrypt_ext(&ctx, iv->x, iv->len,
                                          ad->x, ad->len, cipher_plus_tag, cipher_plus_tag_len,
                                          decrypt_buf, decrypt_buf_len, &outlen, tag->len);

    if (strcmp(result, "FAIL") == 0) {
        TEST_ASSERT(ret == MBEDTLS_ERR_CIPHER_AUTH_FAILED);
        TEST_ASSERT(buffer_is_all_zero(decrypt_buf, decrypt_buf_len));
    } else {
        TEST_ASSERT(ret == 0);
        ASSERT_COMPARE(decrypt_buf, outlen, clear->x, clear->len);
    }

    mbedtls_free(decrypt_buf);
    decrypt_buf = NULL;

    /*
     * Encrypt back if test data was authentic
     */
    if (strcmp(result, "FAIL") != 0) {
        /* prepare context for encryption */
        if (!cipher_reset_key(&ctx, cipher_id, use_psa, tag->len, key,
                              MBEDTLS_ENCRYPT)) {
            goto exit;
        }

        /*
         * Compute size of output buffer according to documentation
         */
        if (using_nist_kw) {
            encrypt_buf_len = clear->len + 8;
            if (using_nist_kw_padding && encrypt_buf_len % 8 != 0) {
                encrypt_buf_len += 8 - encrypt_buf_len % 8;
            }
        } else {
            encrypt_buf_len = clear->len + tag->len;
        }

        /*
         * Try encrypting with an output buffer that's 1B too small
         */
        ASSERT_ALLOC(encrypt_buf, encrypt_buf_len - 1);

        outlen = 0;
        ret = mbedtls_cipher_auth_encrypt_ext(&ctx, iv->x, iv->len,
                                              ad->x, ad->len, clear->x, clear->len,
                                              encrypt_buf, encrypt_buf_len - 1, &outlen, tag->len);
        TEST_ASSERT(ret != 0);

        mbedtls_free(encrypt_buf);
        encrypt_buf = NULL;

        /*
         * Encrypt and check the result
         */
        ASSERT_ALLOC(encrypt_buf, encrypt_buf_len);

        outlen = 0;
        ret = mbedtls_cipher_auth_encrypt_ext(&ctx, iv->x, iv->len,
                                              ad->x, ad->len, clear->x, clear->len,
                                              encrypt_buf, encrypt_buf_len, &outlen, tag->len);
        TEST_ASSERT(ret == 0);

        TEST_ASSERT(outlen == cipher->len + tag->len);
        TEST_ASSERT(memcmp(encrypt_buf, cipher->x, cipher->len) == 0);
        TEST_ASSERT(memcmp(encrypt_buf + cipher->len,
                           tag->x, tag->len) == 0);

        mbedtls_free(encrypt_buf);
        encrypt_buf = NULL;
    }

exit:

    mbedtls_cipher_free(&ctx);
    mbedtls_free(decrypt_buf);
    mbedtls_free(encrypt_buf);
    mbedtls_free(cipher_plus_tag);

#if defined(MBEDTLS_USE_PSA_CRYPTO)
    if (use_psa == 1) {
        PSA_DONE();
    }
#endif /* MBEDTLS_USE_PSA_CRYPTO */
}

static void test_auth_crypt_tv_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data9 = {(uint8_t *) params[9], *( (uint32_t *) params[10] )};
    data_t data12 = {(uint8_t *) params[12], *( (uint32_t *) params[13] )};

    test_auth_crypt_tv( *( (int *) params[0] ), &data1, &data3, &data5, &data7, &data9, (char *) params[11], &data12, *( (int *) params[14] ) );
}
#endif /* MBEDTLS_CIPHER_AUTH_CRYPT */
#line 1031 "suites/test_suite_cipher.function"
static void test_test_vec_ecb(int cipher_id, int operation, data_t *key,
                  data_t *input, data_t *result, int finish_result
                  )
{
    mbedtls_cipher_context_t ctx;
    unsigned char output[32];
    size_t outlen;

    mbedtls_cipher_init(&ctx);

    memset(output, 0x00, sizeof(output));

    /* Prepare context */
    TEST_ASSERT(0 == mbedtls_cipher_setup(&ctx,
                                          mbedtls_cipher_info_from_type(cipher_id)));


    TEST_ASSERT(0 == mbedtls_cipher_setkey(&ctx, key->x, 8 * key->len, operation));

    TEST_ASSERT(0 == mbedtls_cipher_update(&ctx, input->x,
                                           mbedtls_cipher_get_block_size(&ctx),
                                           output, &outlen));
    TEST_ASSERT(outlen == mbedtls_cipher_get_block_size(&ctx));
    TEST_ASSERT(finish_result == mbedtls_cipher_finish(&ctx, output + outlen,
                                                       &outlen));
    TEST_ASSERT(0 == outlen);

    /* check plaintext only if everything went fine */
    if (0 == finish_result) {
        TEST_ASSERT(0 == memcmp(output, result->x,
                                mbedtls_cipher_get_block_size(&ctx)));
    }

exit:
    mbedtls_cipher_free(&ctx);
}

static void test_test_vec_ecb_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_test_vec_ecb( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6, *( (int *) params[8] ) );
}
#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
#line 1070 "suites/test_suite_cipher.function"
static void test_test_vec_crypt(int cipher_id, int operation, data_t *key,
                    data_t *iv, data_t *input, data_t *result,
                    int finish_result, int use_psa)
{
    mbedtls_cipher_context_t ctx;
    unsigned char output[32];
    size_t outlen;

    mbedtls_cipher_init(&ctx);

    memset(output, 0x00, sizeof(output));

    /* Prepare context */
#if !defined(MBEDTLS_USE_PSA_CRYPTO) || !defined(MBEDTLS_TEST_DEPRECATED)
    (void) use_psa;
#else
    if (use_psa == 1) {
        PSA_ASSERT(psa_crypto_init());
        TEST_ASSERT(0 == mbedtls_cipher_setup_psa(&ctx,
                                                  mbedtls_cipher_info_from_type(cipher_id), 0));
    } else
#endif /* !MBEDTLS_USE_PSA_CRYPTO || !MBEDTLS_TEST_DEPRECATED*/
    TEST_ASSERT(0 == mbedtls_cipher_setup(&ctx,
                                          mbedtls_cipher_info_from_type(cipher_id)));

    TEST_ASSERT(0 == mbedtls_cipher_setkey(&ctx, key->x, 8 * key->len, operation));
    if (MBEDTLS_MODE_CBC == ctx.cipher_info->mode) {
        TEST_ASSERT(0 == mbedtls_cipher_set_padding_mode(&ctx, MBEDTLS_PADDING_NONE));
    }

    TEST_ASSERT(finish_result == mbedtls_cipher_crypt(&ctx, iv->len ? iv->x : NULL,
                                                      iv->len, input->x, input->len,
                                                      output, &outlen));
    TEST_ASSERT(result->len == outlen);
    /* check plaintext only if everything went fine */
    if (0 == finish_result) {
        TEST_ASSERT(0 == memcmp(output, result->x, outlen));
    }

exit:
    mbedtls_cipher_free(&ctx);
#if defined(MBEDTLS_USE_PSA_CRYPTO) && defined(MBEDTLS_TEST_DEPRECATED)
    PSA_DONE();
#endif /* MBEDTLS_USE_PSA_CRYPTO && MBEDTLS_TEST_DEPRECATED */
}

static void test_test_vec_crypt_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_test_vec_crypt( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6, &data8, *( (int *) params[10] ), *( (int *) params[11] ) );
}
#endif /* MBEDTLS_CIPHER_MODE_WITH_PADDING */
#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
#line 1118 "suites/test_suite_cipher.function"
static void test_set_padding(int cipher_id, int pad_mode, int ret)
{
    const mbedtls_cipher_info_t *cipher_info;
    mbedtls_cipher_context_t ctx;

    mbedtls_cipher_init(&ctx);

    cipher_info = mbedtls_cipher_info_from_type(cipher_id);
    TEST_ASSERT(NULL != cipher_info);
    TEST_ASSERT(0 == mbedtls_cipher_setup(&ctx, cipher_info));

    TEST_ASSERT(ret == mbedtls_cipher_set_padding_mode(&ctx, pad_mode));

exit:
    mbedtls_cipher_free(&ctx);
}

static void test_set_padding_wrapper( void ** params )
{

    test_set_padding( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
#endif /* MBEDTLS_CIPHER_MODE_WITH_PADDING */
#if defined(MBEDTLS_CIPHER_MODE_CBC)
#line 1137 "suites/test_suite_cipher.function"
static void test_check_padding(int pad_mode, data_t *input, int ret, int dlen_check
                   )
{
    mbedtls_cipher_info_t cipher_info;
    mbedtls_cipher_context_t ctx;
    size_t dlen;

    /* build a fake context just for getting access to get_padding */
    mbedtls_cipher_init(&ctx);
    cipher_info.mode = MBEDTLS_MODE_CBC;
    ctx.cipher_info = &cipher_info;

    TEST_ASSERT(0 == mbedtls_cipher_set_padding_mode(&ctx, pad_mode));


    TEST_ASSERT(ret == ctx.get_padding(input->x, input->len, &dlen));
    if (0 == ret) {
        TEST_ASSERT(dlen == (size_t) dlen_check);
    }
exit:
    ;
}

static void test_check_padding_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_check_padding( *( (int *) params[0] ), &data1, *( (int *) params[3] ), *( (int *) params[4] ) );
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */
#line 1160 "suites/test_suite_cipher.function"
static void test_iv_len_validity(int cipher_id, char *cipher_string,
                     int iv_len_val, int ret)
{
    size_t iv_len = iv_len_val;
    unsigned char iv[16];

    /* Initialise iv buffer */
    memset(iv, 0, sizeof(iv));

    const mbedtls_cipher_info_t *cipher_info;
    mbedtls_cipher_context_t ctx_dec;
    mbedtls_cipher_context_t ctx_enc;

    /*
     * Prepare contexts
     */
    mbedtls_cipher_init(&ctx_dec);
    mbedtls_cipher_init(&ctx_enc);

    /* Check and get info structures */
    cipher_info = mbedtls_cipher_info_from_type(cipher_id);
    TEST_ASSERT(NULL != cipher_info);
    TEST_ASSERT(mbedtls_cipher_info_from_string(cipher_string) == cipher_info);
    TEST_ASSERT(strcmp(mbedtls_cipher_info_get_name(cipher_info),
                       cipher_string) == 0);

    /* Initialise enc and dec contexts */
    TEST_ASSERT(0 == mbedtls_cipher_setup(&ctx_dec, cipher_info));
    TEST_ASSERT(0 == mbedtls_cipher_setup(&ctx_enc, cipher_info));

    TEST_ASSERT(ret == mbedtls_cipher_set_iv(&ctx_dec, iv, iv_len));
    TEST_ASSERT(ret == mbedtls_cipher_set_iv(&ctx_enc, iv, iv_len));

exit:
    mbedtls_cipher_free(&ctx_dec);
    mbedtls_cipher_free(&ctx_enc);
}

static void test_iv_len_validity_wrapper( void ** params )
{

    test_iv_len_validity( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), *( (int *) params[3] ) );
}
#endif /* MBEDTLS_CIPHER_C */


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
    
#if defined(MBEDTLS_CIPHER_C)

        case 0:
            {
                *out_value = MBEDTLS_CIPHER_CHACHA20;
            }
            break;
        case 1:
            {
                *out_value = -1;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
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
    
#if defined(MBEDTLS_CIPHER_C)

        case 0:
            {
#if defined(MBEDTLS_CHACHA20_C)
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

#if defined(MBEDTLS_CIPHER_C)
    test_mbedtls_cipher_list_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_CIPHER_C)
    test_cipher_invalid_param_unconditional_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_CIPHER_C)
    test_cipher_invalid_param_conditional_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_CIPHER_C) && defined(MBEDTLS_AES_C)
    test_cipher_special_behaviours_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_CIPHER_C)
    test_enc_dec_buf_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_CIPHER_C)
    test_enc_fail_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_CIPHER_C)
    test_dec_empty_buf_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_CIPHER_C)
    test_enc_dec_buf_multipart_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_CIPHER_C)
    test_decrypt_test_vec_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_CIPHER_C) && defined(MBEDTLS_CIPHER_AUTH_CRYPT)
    test_auth_crypt_tv_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_CIPHER_C)
    test_test_vec_ecb_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_CIPHER_C) && defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
    test_test_vec_crypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 12 */

#if defined(MBEDTLS_CIPHER_C) && defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
    test_set_padding_wrapper,
#else
    NULL,
#endif
/* Function Id: 13 */

#if defined(MBEDTLS_CIPHER_C) && defined(MBEDTLS_CIPHER_MODE_CBC)
    test_check_padding_wrapper,
#else
    NULL,
#endif
/* Function Id: 14 */

#if defined(MBEDTLS_CIPHER_C)
    test_iv_len_validity_wrapper,
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
    const char *default_filename = "./test_suite_cipher.chacha20.datax";
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
