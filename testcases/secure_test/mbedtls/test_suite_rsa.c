#line 2 "suites/main_test.function"
/*
 * *** THIS FILE HAS BEEN MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py
 *
 * Test file      : ./test_suite_rsa.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_rsa.function
 *      Test suite data     : suites/test_suite_rsa.data
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

#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_BIGNUM_C)
#if defined(MBEDTLS_GENPRIME)
#line 2 "suites/test_suite_rsa.function"
#include "mbedtls/rsa.h"
#include "rsa_alt_helpers.h"

#include "mbedtls/legacy_or_psa.h"
#line 14 "suites/test_suite_rsa.function"
static void test_rsa_invalid_param(void)
{
    mbedtls_rsa_context ctx;
    const int invalid_padding = 42;
    const int invalid_hash_id = 0xff;
    unsigned char buf[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
    size_t buf_len = sizeof(buf);

    mbedtls_rsa_init(&ctx);

    TEST_EQUAL(mbedtls_rsa_set_padding(&ctx,
                                       invalid_padding,
                                       MBEDTLS_MD_NONE),
               MBEDTLS_ERR_RSA_INVALID_PADDING);

    TEST_EQUAL(mbedtls_rsa_set_padding(&ctx,
                                       MBEDTLS_RSA_PKCS_V21,
                                       invalid_hash_id),
               MBEDTLS_ERR_RSA_INVALID_PADDING);

    TEST_EQUAL(mbedtls_rsa_pkcs1_sign(&ctx, NULL,
                                      NULL, MBEDTLS_MD_NONE,
                                      buf_len,
                                      NULL, buf),
               MBEDTLS_ERR_RSA_BAD_INPUT_DATA);

    TEST_EQUAL(mbedtls_rsa_pkcs1_sign(&ctx, NULL,
                                      NULL, MBEDTLS_MD_SHA256,
                                      0,
                                      NULL, buf),
               MBEDTLS_ERR_RSA_BAD_INPUT_DATA);

    TEST_EQUAL(mbedtls_rsa_pkcs1_verify(&ctx, MBEDTLS_MD_NONE,
                                        buf_len,
                                        NULL, buf),
               MBEDTLS_ERR_RSA_BAD_INPUT_DATA);

    TEST_EQUAL(mbedtls_rsa_pkcs1_verify(&ctx, MBEDTLS_MD_SHA256,
                                        0,
                                        NULL, buf),
               MBEDTLS_ERR_RSA_BAD_INPUT_DATA);

#if !defined(MBEDTLS_PKCS1_V15)
    TEST_EQUAL(mbedtls_rsa_set_padding(&ctx,
                                       MBEDTLS_RSA_PKCS_V15,
                                       MBEDTLS_MD_NONE),
               MBEDTLS_ERR_RSA_INVALID_PADDING);
#endif

#if defined(MBEDTLS_PKCS1_V15)
    TEST_EQUAL(mbedtls_rsa_rsassa_pkcs1_v15_sign(&ctx, NULL,
                                                 NULL, MBEDTLS_MD_NONE,
                                                 buf_len,
                                                 NULL, buf),
               MBEDTLS_ERR_RSA_BAD_INPUT_DATA);

    TEST_EQUAL(mbedtls_rsa_rsassa_pkcs1_v15_sign(&ctx, NULL,
                                                 NULL, MBEDTLS_MD_SHA256,
                                                 0,
                                                 NULL, buf),
               MBEDTLS_ERR_RSA_BAD_INPUT_DATA);

    TEST_EQUAL(mbedtls_rsa_rsassa_pkcs1_v15_verify(&ctx, MBEDTLS_MD_NONE,
                                                   buf_len,
                                                   NULL, buf),
               MBEDTLS_ERR_RSA_BAD_INPUT_DATA);

    TEST_EQUAL(mbedtls_rsa_rsassa_pkcs1_v15_verify(&ctx, MBEDTLS_MD_SHA256,
                                                   0,
                                                   NULL, buf),
               MBEDTLS_ERR_RSA_BAD_INPUT_DATA);


#endif

#if !defined(MBEDTLS_PKCS1_V21)
    TEST_EQUAL(mbedtls_rsa_set_padding(&ctx,
                                       MBEDTLS_RSA_PKCS_V21,
                                       MBEDTLS_MD_NONE),
               MBEDTLS_ERR_RSA_INVALID_PADDING);
#endif

#if defined(MBEDTLS_PKCS1_V21)
    TEST_EQUAL(mbedtls_rsa_rsassa_pss_sign_ext(&ctx, NULL, NULL,
                                               MBEDTLS_MD_NONE, buf_len,
                                               NULL, buf_len,
                                               buf),
               MBEDTLS_ERR_RSA_BAD_INPUT_DATA);

    TEST_EQUAL(mbedtls_rsa_rsassa_pss_sign_ext(&ctx, NULL, NULL,
                                               MBEDTLS_MD_SHA256, 0,
                                               NULL, buf_len,
                                               buf),
               MBEDTLS_ERR_RSA_BAD_INPUT_DATA);

    TEST_EQUAL(mbedtls_rsa_rsassa_pss_verify_ext(&ctx, MBEDTLS_MD_NONE,
                                                 buf_len, NULL,
                                                 MBEDTLS_MD_NONE,
                                                 buf_len, buf),
               MBEDTLS_ERR_RSA_BAD_INPUT_DATA);

    TEST_EQUAL(mbedtls_rsa_rsassa_pss_verify_ext(&ctx, MBEDTLS_MD_SHA256,
                                                 0, NULL,
                                                 MBEDTLS_MD_NONE,
                                                 buf_len, buf),
               MBEDTLS_ERR_RSA_BAD_INPUT_DATA);

    TEST_EQUAL(mbedtls_rsa_rsassa_pss_verify(&ctx, MBEDTLS_MD_NONE,
                                             buf_len,
                                             NULL, buf),
               MBEDTLS_ERR_RSA_BAD_INPUT_DATA);

    TEST_EQUAL(mbedtls_rsa_rsassa_pss_verify(&ctx, MBEDTLS_MD_SHA256,
                                             0,
                                             NULL, buf),
               MBEDTLS_ERR_RSA_BAD_INPUT_DATA);
#endif

exit:
    mbedtls_rsa_free(&ctx);
}

static void test_rsa_invalid_param_wrapper( void ** params )
{
    (void)params;

    test_rsa_invalid_param(  );
}
#line 138 "suites/test_suite_rsa.function"
static void test_rsa_init_free(int reinit)
{
    mbedtls_rsa_context ctx;

    /* Double free is not explicitly documented to work, but we rely on it
     * even inside the library so that you can call mbedtls_rsa_free()
     * unconditionally on an error path without checking whether it has
     * already been called in the success path. */

    mbedtls_rsa_init(&ctx);
    mbedtls_rsa_free(&ctx);

    if (reinit) {
        mbedtls_rsa_init(&ctx);
    }
    mbedtls_rsa_free(&ctx);

    /* This test case always succeeds, functionally speaking. A plausible
     * bug might trigger an invalid pointer dereference or a memory leak. */
    goto exit;
exit:
    ;
}

static void test_rsa_init_free_wrapper( void ** params )
{

    test_rsa_init_free( *( (int *) params[0] ) );
}
#line 162 "suites/test_suite_rsa.function"
static void test_mbedtls_rsa_pkcs1_sign(data_t *message_str, int padding_mode,
                            int digest, int mod, char *input_P,
                            char *input_Q, char *input_N, char *input_E,
                            data_t *result_str, int result)
{
    unsigned char output[256];
    mbedtls_rsa_context ctx;
    mbedtls_mpi N, P, Q, E;
    mbedtls_test_rnd_pseudo_info rnd_info;

    mbedtls_mpi_init(&N); mbedtls_mpi_init(&P);
    mbedtls_mpi_init(&Q); mbedtls_mpi_init(&E);
    mbedtls_rsa_init(&ctx);
    TEST_ASSERT(mbedtls_rsa_set_padding(&ctx, padding_mode,
                                        MBEDTLS_MD_NONE) == 0);

    memset(output, 0x00, sizeof(output));
    memset(&rnd_info, 0, sizeof(mbedtls_test_rnd_pseudo_info));

    TEST_ASSERT(mbedtls_test_read_mpi(&P, input_P) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&Q, input_Q) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&N, input_N) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&E, input_E) == 0);

    TEST_ASSERT(mbedtls_rsa_import(&ctx, &N, &P, &Q, NULL, &E) == 0);
    TEST_ASSERT(mbedtls_rsa_get_len(&ctx) == (size_t) (mod / 8));
    TEST_ASSERT(mbedtls_rsa_complete(&ctx) == 0);
    TEST_ASSERT(mbedtls_rsa_check_privkey(&ctx) == 0);

    TEST_ASSERT(mbedtls_rsa_pkcs1_sign(
                    &ctx, &mbedtls_test_rnd_pseudo_rand, &rnd_info,
                    digest, message_str->len, message_str->x,
                    output) == result);
    if (result == 0) {

        TEST_ASSERT(mbedtls_test_hexcmp(output, result_str->x,
                                        ctx.len, result_str->len) == 0);
    }

exit:
    mbedtls_mpi_free(&N); mbedtls_mpi_free(&P);
    mbedtls_mpi_free(&Q); mbedtls_mpi_free(&E);
    mbedtls_rsa_free(&ctx);
}

static void test_mbedtls_rsa_pkcs1_sign_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data9 = {(uint8_t *) params[9], *( (uint32_t *) params[10] )};

    test_mbedtls_rsa_pkcs1_sign( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), (char *) params[5], (char *) params[6], (char *) params[7], (char *) params[8], &data9, *( (int *) params[11] ) );
}
#line 209 "suites/test_suite_rsa.function"
static void test_mbedtls_rsa_pkcs1_verify(data_t *message_str, int padding_mode,
                              int digest, int mod,
                              char *input_N, char *input_E,
                              data_t *result_str, int result)
{
    mbedtls_rsa_context ctx;
    mbedtls_mpi N, E;

    mbedtls_mpi_init(&N); mbedtls_mpi_init(&E);
    mbedtls_rsa_init(&ctx);
    TEST_ASSERT(mbedtls_rsa_set_padding(&ctx, padding_mode,
                                        MBEDTLS_MD_NONE) == 0);

    TEST_ASSERT(mbedtls_test_read_mpi(&N, input_N) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&E, input_E) == 0);
    TEST_ASSERT(mbedtls_rsa_import(&ctx, &N, NULL, NULL, NULL, &E) == 0);
    TEST_ASSERT(mbedtls_rsa_get_len(&ctx) == (size_t) (mod / 8));
    TEST_ASSERT(mbedtls_rsa_check_pubkey(&ctx) == 0);

    TEST_ASSERT(mbedtls_rsa_pkcs1_verify(&ctx, digest, message_str->len, message_str->x,
                                         result_str->x) == result);

exit:
    mbedtls_mpi_free(&N); mbedtls_mpi_free(&E);
    mbedtls_rsa_free(&ctx);
}

static void test_mbedtls_rsa_pkcs1_verify_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};

    test_mbedtls_rsa_pkcs1_verify( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), (char *) params[5], (char *) params[6], &data7, *( (int *) params[9] ) );
}
#line 239 "suites/test_suite_rsa.function"
static void test_rsa_pkcs1_sign_raw(data_t *hash_result,
                        int padding_mode, int mod,
                        char *input_P, char *input_Q,
                        char *input_N, char *input_E,
                        data_t *result_str)
{
    unsigned char output[256];
    mbedtls_rsa_context ctx;
    mbedtls_mpi N, P, Q, E;
    mbedtls_test_rnd_pseudo_info rnd_info;

    mbedtls_rsa_init(&ctx);
    mbedtls_mpi_init(&N); mbedtls_mpi_init(&P);
    mbedtls_mpi_init(&Q); mbedtls_mpi_init(&E);

    TEST_ASSERT(mbedtls_rsa_set_padding(&ctx, padding_mode,
                                        MBEDTLS_MD_NONE) == 0);

    memset(output, 0x00, sizeof(output));
    memset(&rnd_info, 0, sizeof(mbedtls_test_rnd_pseudo_info));

    TEST_ASSERT(mbedtls_test_read_mpi(&P, input_P) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&Q, input_Q) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&N, input_N) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&E, input_E) == 0);

    TEST_ASSERT(mbedtls_rsa_import(&ctx, &N, &P, &Q, NULL, &E) == 0);
    TEST_ASSERT(mbedtls_rsa_get_len(&ctx) == (size_t) (mod / 8));
    TEST_ASSERT(mbedtls_rsa_complete(&ctx) == 0);
    TEST_ASSERT(mbedtls_rsa_check_privkey(&ctx) == 0);


    TEST_ASSERT(mbedtls_rsa_pkcs1_sign(&ctx, &mbedtls_test_rnd_pseudo_rand,
                                       &rnd_info, MBEDTLS_MD_NONE,
                                       hash_result->len,
                                       hash_result->x, output) == 0);


    TEST_ASSERT(mbedtls_test_hexcmp(output, result_str->x,
                                    ctx.len, result_str->len) == 0);

exit:
    mbedtls_mpi_free(&N); mbedtls_mpi_free(&P);
    mbedtls_mpi_free(&Q); mbedtls_mpi_free(&E);

    mbedtls_rsa_free(&ctx);
}

static void test_rsa_pkcs1_sign_raw_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_rsa_pkcs1_sign_raw( &data0, *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4], (char *) params[5], (char *) params[6], (char *) params[7], &data8 );
}
#line 289 "suites/test_suite_rsa.function"
static void test_rsa_pkcs1_verify_raw(data_t *hash_result,
                          int padding_mode, int mod,
                          char *input_N, char *input_E,
                          data_t *result_str, int correct)
{
    unsigned char output[256];
    mbedtls_rsa_context ctx;

    mbedtls_mpi N, E;
    mbedtls_mpi_init(&N); mbedtls_mpi_init(&E);

    mbedtls_rsa_init(&ctx);
    TEST_ASSERT(mbedtls_rsa_set_padding(&ctx, padding_mode,
                                        MBEDTLS_MD_NONE) == 0);
    memset(output, 0x00, sizeof(output));

    TEST_ASSERT(mbedtls_test_read_mpi(&N, input_N) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&E, input_E) == 0);

    TEST_ASSERT(mbedtls_rsa_import(&ctx, &N, NULL, NULL, NULL, &E) == 0);
    TEST_ASSERT(mbedtls_rsa_get_len(&ctx) == (size_t) (mod / 8));
    TEST_ASSERT(mbedtls_rsa_check_pubkey(&ctx) == 0);


    TEST_ASSERT(mbedtls_rsa_pkcs1_verify(&ctx, MBEDTLS_MD_NONE, hash_result->len, hash_result->x,
                                         result_str->x) == correct);

exit:
    mbedtls_mpi_free(&N); mbedtls_mpi_free(&E);
    mbedtls_rsa_free(&ctx);
}

static void test_rsa_pkcs1_verify_raw_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_rsa_pkcs1_verify_raw( &data0, *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4], (char *) params[5], &data6, *( (int *) params[8] ) );
}
#line 323 "suites/test_suite_rsa.function"
static void test_mbedtls_rsa_pkcs1_encrypt(data_t *message_str, int padding_mode,
                               int mod, char *input_N, char *input_E,
                               data_t *result_str, int result)
{
    unsigned char output[256];
    mbedtls_rsa_context ctx;
    mbedtls_test_rnd_pseudo_info rnd_info;

    mbedtls_mpi N, E;
    mbedtls_mpi_init(&N); mbedtls_mpi_init(&E);

    memset(&rnd_info, 0, sizeof(mbedtls_test_rnd_pseudo_info));

    mbedtls_rsa_init(&ctx);
    TEST_ASSERT(mbedtls_rsa_set_padding(&ctx, padding_mode,
                                        MBEDTLS_MD_NONE) == 0);
    memset(output, 0x00, sizeof(output));

    TEST_ASSERT(mbedtls_test_read_mpi(&N, input_N) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&E, input_E) == 0);

    TEST_ASSERT(mbedtls_rsa_import(&ctx, &N, NULL, NULL, NULL, &E) == 0);
    TEST_ASSERT(mbedtls_rsa_get_len(&ctx) == (size_t) (mod / 8));
    TEST_ASSERT(mbedtls_rsa_check_pubkey(&ctx) == 0);


    TEST_ASSERT(mbedtls_rsa_pkcs1_encrypt(&ctx,
                                          &mbedtls_test_rnd_pseudo_rand,
                                          &rnd_info, message_str->len,
                                          message_str->x,
                                          output) == result);
    if (result == 0) {

        TEST_ASSERT(mbedtls_test_hexcmp(output, result_str->x,
                                        ctx.len, result_str->len) == 0);
    }

exit:
    mbedtls_mpi_free(&N); mbedtls_mpi_free(&E);
    mbedtls_rsa_free(&ctx);
}

static void test_mbedtls_rsa_pkcs1_encrypt_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_mbedtls_rsa_pkcs1_encrypt( &data0, *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4], (char *) params[5], &data6, *( (int *) params[8] ) );
}
#line 367 "suites/test_suite_rsa.function"
static void test_rsa_pkcs1_encrypt_bad_rng(data_t *message_str, int padding_mode,
                               int mod, char *input_N, char *input_E,
                               data_t *result_str, int result)
{
    unsigned char output[256];
    mbedtls_rsa_context ctx;

    mbedtls_mpi N, E;

    mbedtls_mpi_init(&N); mbedtls_mpi_init(&E);
    mbedtls_rsa_init(&ctx);
    TEST_ASSERT(mbedtls_rsa_set_padding(&ctx, padding_mode,
                                        MBEDTLS_MD_NONE) == 0);
    memset(output, 0x00, sizeof(output));

    TEST_ASSERT(mbedtls_test_read_mpi(&N, input_N) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&E, input_E) == 0);

    TEST_ASSERT(mbedtls_rsa_import(&ctx, &N, NULL, NULL, NULL, &E) == 0);
    TEST_ASSERT(mbedtls_rsa_get_len(&ctx) == (size_t) (mod / 8));
    TEST_ASSERT(mbedtls_rsa_check_pubkey(&ctx) == 0);


    TEST_ASSERT(mbedtls_rsa_pkcs1_encrypt(&ctx, &mbedtls_test_rnd_zero_rand,
                                          NULL, message_str->len,
                                          message_str->x,
                                          output) == result);
    if (result == 0) {

        TEST_ASSERT(mbedtls_test_hexcmp(output, result_str->x,
                                        ctx.len, result_str->len) == 0);
    }

exit:
    mbedtls_mpi_free(&N); mbedtls_mpi_free(&E);
    mbedtls_rsa_free(&ctx);
}

static void test_rsa_pkcs1_encrypt_bad_rng_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_rsa_pkcs1_encrypt_bad_rng( &data0, *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4], (char *) params[5], &data6, *( (int *) params[8] ) );
}
#line 407 "suites/test_suite_rsa.function"
static void test_mbedtls_rsa_pkcs1_decrypt(data_t *message_str, int padding_mode,
                               int mod, char *input_P,
                               char *input_Q, char *input_N,
                               char *input_E, int max_output,
                               data_t *result_str, int result)
{
    unsigned char output[32];
    mbedtls_rsa_context ctx;
    size_t output_len;
    mbedtls_test_rnd_pseudo_info rnd_info;
    mbedtls_mpi N, P, Q, E;

    mbedtls_mpi_init(&N); mbedtls_mpi_init(&P);
    mbedtls_mpi_init(&Q); mbedtls_mpi_init(&E);

    mbedtls_rsa_init(&ctx);
    TEST_ASSERT(mbedtls_rsa_set_padding(&ctx, padding_mode,
                                        MBEDTLS_MD_NONE) == 0);

    memset(output, 0x00, sizeof(output));
    memset(&rnd_info, 0, sizeof(mbedtls_test_rnd_pseudo_info));


    TEST_ASSERT(mbedtls_test_read_mpi(&P, input_P) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&Q, input_Q) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&N, input_N) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&E, input_E) == 0);

    TEST_ASSERT(mbedtls_rsa_import(&ctx, &N, &P, &Q, NULL, &E) == 0);
    TEST_ASSERT(mbedtls_rsa_get_len(&ctx) == (size_t) (mod / 8));
    TEST_ASSERT(mbedtls_rsa_complete(&ctx) == 0);
    TEST_ASSERT(mbedtls_rsa_check_privkey(&ctx) == 0);

    output_len = 0;

    TEST_ASSERT(mbedtls_rsa_pkcs1_decrypt(&ctx, mbedtls_test_rnd_pseudo_rand,
                                          &rnd_info,
                                          &output_len, message_str->x, output,
                                          max_output) == result);
    if (result == 0) {

        TEST_ASSERT(mbedtls_test_hexcmp(output, result_str->x,
                                        output_len,
                                        result_str->len) == 0);
    }

exit:
    mbedtls_mpi_free(&N); mbedtls_mpi_free(&P);
    mbedtls_mpi_free(&Q); mbedtls_mpi_free(&E);
    mbedtls_rsa_free(&ctx);
}

static void test_mbedtls_rsa_pkcs1_decrypt_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data9 = {(uint8_t *) params[9], *( (uint32_t *) params[10] )};

    test_mbedtls_rsa_pkcs1_decrypt( &data0, *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4], (char *) params[5], (char *) params[6], (char *) params[7], *( (int *) params[8] ), &data9, *( (int *) params[11] ) );
}
#line 461 "suites/test_suite_rsa.function"
static void test_mbedtls_rsa_public(data_t *message_str, int mod,
                        char *input_N, char *input_E,
                        data_t *result_str, int result)
{
    unsigned char output[256];
    mbedtls_rsa_context ctx, ctx2; /* Also test mbedtls_rsa_copy() while at it */

    mbedtls_mpi N, E;

    mbedtls_mpi_init(&N); mbedtls_mpi_init(&E);
    mbedtls_rsa_init(&ctx);
    mbedtls_rsa_init(&ctx2);
    memset(output, 0x00, sizeof(output));

    TEST_ASSERT(mbedtls_test_read_mpi(&N, input_N) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&E, input_E) == 0);

    TEST_ASSERT(mbedtls_rsa_import(&ctx, &N, NULL, NULL, NULL, &E) == 0);

    /* Check test data consistency */
    TEST_ASSERT(message_str->len == (size_t) (mod / 8));
    TEST_ASSERT(mbedtls_rsa_get_len(&ctx) == (size_t) (mod / 8));
    TEST_ASSERT(mbedtls_rsa_check_pubkey(&ctx) == 0);

    TEST_ASSERT(mbedtls_rsa_public(&ctx, message_str->x, output) == result);
    if (result == 0) {

        TEST_ASSERT(mbedtls_test_hexcmp(output, result_str->x,
                                        ctx.len, result_str->len) == 0);
    }

    /* And now with the copy */
    TEST_ASSERT(mbedtls_rsa_copy(&ctx2, &ctx) == 0);
    /* clear the original to be sure */
    mbedtls_rsa_free(&ctx);

    TEST_ASSERT(mbedtls_rsa_check_pubkey(&ctx2) == 0);

    memset(output, 0x00, sizeof(output));
    TEST_ASSERT(mbedtls_rsa_public(&ctx2, message_str->x, output) == result);
    if (result == 0) {

        TEST_ASSERT(mbedtls_test_hexcmp(output, result_str->x,
                                        ctx.len, result_str->len) == 0);
    }

exit:
    mbedtls_mpi_free(&N); mbedtls_mpi_free(&E);
    mbedtls_rsa_free(&ctx);
    mbedtls_rsa_free(&ctx2);
}

static void test_mbedtls_rsa_public_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};

    test_mbedtls_rsa_public( &data0, *( (int *) params[2] ), (char *) params[3], (char *) params[4], &data5, *( (int *) params[7] ) );
}
#line 515 "suites/test_suite_rsa.function"
static void test_mbedtls_rsa_private(data_t *message_str, int mod,
                         char *input_P, char *input_Q,
                         char *input_N, char *input_E,
                         data_t *result_str, int result)
{
    unsigned char output[256];
    mbedtls_rsa_context ctx, ctx2; /* Also test mbedtls_rsa_copy() while at it */
    mbedtls_mpi N, P, Q, E;
    mbedtls_test_rnd_pseudo_info rnd_info;
    int i;

    mbedtls_mpi_init(&N); mbedtls_mpi_init(&P);
    mbedtls_mpi_init(&Q); mbedtls_mpi_init(&E);
    mbedtls_rsa_init(&ctx);
    mbedtls_rsa_init(&ctx2);

    memset(&rnd_info, 0, sizeof(mbedtls_test_rnd_pseudo_info));

    TEST_ASSERT(mbedtls_test_read_mpi(&P, input_P) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&Q, input_Q) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&N, input_N) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&E, input_E) == 0);

    TEST_ASSERT(mbedtls_rsa_import(&ctx, &N, &P, &Q, NULL, &E) == 0);

    /* Check test data consistency */
    TEST_ASSERT(message_str->len == (size_t) (mod / 8));
    TEST_ASSERT(mbedtls_rsa_get_len(&ctx) == (size_t) (mod / 8));
    TEST_ASSERT(mbedtls_rsa_complete(&ctx) == 0);
    TEST_ASSERT(mbedtls_rsa_check_privkey(&ctx) == 0);

    /* repeat three times to test updating of blinding values */
    for (i = 0; i < 3; i++) {
        memset(output, 0x00, sizeof(output));
        TEST_ASSERT(mbedtls_rsa_private(&ctx, mbedtls_test_rnd_pseudo_rand,
                                        &rnd_info, message_str->x,
                                        output) == result);
        if (result == 0) {

            TEST_ASSERT(mbedtls_test_hexcmp(output, result_str->x,
                                            ctx.len,
                                            result_str->len) == 0);
        }
    }

    /* And now one more time with the copy */
    TEST_ASSERT(mbedtls_rsa_copy(&ctx2, &ctx) == 0);
    /* clear the original to be sure */
    mbedtls_rsa_free(&ctx);

    TEST_ASSERT(mbedtls_rsa_check_privkey(&ctx2) == 0);

    memset(output, 0x00, sizeof(output));
    TEST_ASSERT(mbedtls_rsa_private(&ctx2, mbedtls_test_rnd_pseudo_rand,
                                    &rnd_info, message_str->x,
                                    output) == result);
    if (result == 0) {

        TEST_ASSERT(mbedtls_test_hexcmp(output, result_str->x,
                                        ctx2.len,
                                        result_str->len) == 0);
    }

exit:
    mbedtls_mpi_free(&N); mbedtls_mpi_free(&P);
    mbedtls_mpi_free(&Q); mbedtls_mpi_free(&E);

    mbedtls_rsa_free(&ctx); mbedtls_rsa_free(&ctx2);
}

static void test_mbedtls_rsa_private_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};

    test_mbedtls_rsa_private( &data0, *( (int *) params[2] ), (char *) params[3], (char *) params[4], (char *) params[5], (char *) params[6], &data7, *( (int *) params[9] ) );
}
#line 587 "suites/test_suite_rsa.function"
static void test_rsa_check_privkey_null(void)
{
    mbedtls_rsa_context ctx;
    memset(&ctx, 0x00, sizeof(mbedtls_rsa_context));

    TEST_ASSERT(mbedtls_rsa_check_privkey(&ctx) == MBEDTLS_ERR_RSA_KEY_CHECK_FAILED);
exit:
    ;
}

static void test_rsa_check_privkey_null_wrapper( void ** params )
{
    (void)params;

    test_rsa_check_privkey_null(  );
}
#line 597 "suites/test_suite_rsa.function"
static void test_mbedtls_rsa_check_pubkey(char *input_N, char *input_E, int result)
{
    mbedtls_rsa_context ctx;
    mbedtls_mpi N, E;

    mbedtls_mpi_init(&N); mbedtls_mpi_init(&E);
    mbedtls_rsa_init(&ctx);

    if (strlen(input_N)) {
        TEST_ASSERT(mbedtls_test_read_mpi(&N, input_N) == 0);
    }
    if (strlen(input_E)) {
        TEST_ASSERT(mbedtls_test_read_mpi(&E, input_E) == 0);
    }

    TEST_ASSERT(mbedtls_rsa_import(&ctx, &N, NULL, NULL, NULL, &E) == 0);
    TEST_ASSERT(mbedtls_rsa_check_pubkey(&ctx) == result);

exit:
    mbedtls_mpi_free(&N); mbedtls_mpi_free(&E);
    mbedtls_rsa_free(&ctx);
}

static void test_mbedtls_rsa_check_pubkey_wrapper( void ** params )
{

    test_mbedtls_rsa_check_pubkey( (char *) params[0], (char *) params[1], *( (int *) params[2] ) );
}
#line 622 "suites/test_suite_rsa.function"
static void test_mbedtls_rsa_check_privkey(int mod, char *input_P, char *input_Q,
                               char *input_N, char *input_E, char *input_D,
                               char *input_DP, char *input_DQ, char *input_QP,
                               int result)
{
    mbedtls_rsa_context ctx;

    mbedtls_rsa_init(&ctx);

    ctx.len = mod / 8;
    if (strlen(input_P)) {
        TEST_ASSERT(mbedtls_test_read_mpi(&ctx.P, input_P) == 0);
    }
    if (strlen(input_Q)) {
        TEST_ASSERT(mbedtls_test_read_mpi(&ctx.Q, input_Q) == 0);
    }
    if (strlen(input_N)) {
        TEST_ASSERT(mbedtls_test_read_mpi(&ctx.N, input_N) == 0);
    }
    if (strlen(input_E)) {
        TEST_ASSERT(mbedtls_test_read_mpi(&ctx.E, input_E) == 0);
    }
    if (strlen(input_D)) {
        TEST_ASSERT(mbedtls_test_read_mpi(&ctx.D, input_D) == 0);
    }
#if !defined(MBEDTLS_RSA_NO_CRT)
    if (strlen(input_DP)) {
        TEST_ASSERT(mbedtls_test_read_mpi(&ctx.DP, input_DP) == 0);
    }
    if (strlen(input_DQ)) {
        TEST_ASSERT(mbedtls_test_read_mpi(&ctx.DQ, input_DQ) == 0);
    }
    if (strlen(input_QP)) {
        TEST_ASSERT(mbedtls_test_read_mpi(&ctx.QP, input_QP) == 0);
    }
#else
    ((void) input_DP);
    ((void) input_DQ);
    ((void) input_QP);
#endif

    TEST_ASSERT(mbedtls_rsa_check_privkey(&ctx) == result);

exit:
    mbedtls_rsa_free(&ctx);
}

static void test_mbedtls_rsa_check_privkey_wrapper( void ** params )
{

    test_mbedtls_rsa_check_privkey( *( (int *) params[0] ), (char *) params[1], (char *) params[2], (char *) params[3], (char *) params[4], (char *) params[5], (char *) params[6], (char *) params[7], (char *) params[8], *( (int *) params[9] ) );
}
#line 671 "suites/test_suite_rsa.function"
static void test_rsa_check_pubpriv(int mod, char *input_Npub, char *input_Epub,
                       char *input_P, char *input_Q, char *input_N,
                       char *input_E, char *input_D, char *input_DP,
                       char *input_DQ, char *input_QP, int result)
{
    mbedtls_rsa_context pub, prv;

    mbedtls_rsa_init(&pub);
    mbedtls_rsa_init(&prv);

    pub.len = mod / 8;
    prv.len = mod / 8;

    if (strlen(input_Npub)) {
        TEST_ASSERT(mbedtls_test_read_mpi(&pub.N, input_Npub) == 0);
    }
    if (strlen(input_Epub)) {
        TEST_ASSERT(mbedtls_test_read_mpi(&pub.E, input_Epub) == 0);
    }

    if (strlen(input_P)) {
        TEST_ASSERT(mbedtls_test_read_mpi(&prv.P, input_P) == 0);
    }
    if (strlen(input_Q)) {
        TEST_ASSERT(mbedtls_test_read_mpi(&prv.Q, input_Q) == 0);
    }
    if (strlen(input_N)) {
        TEST_ASSERT(mbedtls_test_read_mpi(&prv.N, input_N) == 0);
    }
    if (strlen(input_E)) {
        TEST_ASSERT(mbedtls_test_read_mpi(&prv.E, input_E) == 0);
    }
    if (strlen(input_D)) {
        TEST_ASSERT(mbedtls_test_read_mpi(&prv.D, input_D) == 0);
    }
#if !defined(MBEDTLS_RSA_NO_CRT)
    if (strlen(input_DP)) {
        TEST_ASSERT(mbedtls_test_read_mpi(&prv.DP, input_DP) == 0);
    }
    if (strlen(input_DQ)) {
        TEST_ASSERT(mbedtls_test_read_mpi(&prv.DQ, input_DQ) == 0);
    }
    if (strlen(input_QP)) {
        TEST_ASSERT(mbedtls_test_read_mpi(&prv.QP, input_QP) == 0);
    }
#else
    ((void) input_DP);
    ((void) input_DQ);
    ((void) input_QP);
#endif

    TEST_ASSERT(mbedtls_rsa_check_pub_priv(&pub, &prv) == result);

exit:
    mbedtls_rsa_free(&pub);
    mbedtls_rsa_free(&prv);
}

static void test_rsa_check_pubpriv_wrapper( void ** params )
{

    test_rsa_check_pubpriv( *( (int *) params[0] ), (char *) params[1], (char *) params[2], (char *) params[3], (char *) params[4], (char *) params[5], (char *) params[6], (char *) params[7], (char *) params[8], (char *) params[9], (char *) params[10], *( (int *) params[11] ) );
}
#line 731 "suites/test_suite_rsa.function"
static void test_mbedtls_rsa_gen_key(int nrbits, int exponent, int result)
{
    mbedtls_rsa_context ctx;
    mbedtls_rsa_init(&ctx);

    /* This test uses an insecure RNG, suitable only for testing.
     * In production, always use a cryptographically strong RNG! */
    TEST_ASSERT(mbedtls_rsa_gen_key(&ctx, mbedtls_test_rnd_std_rand, NULL, nrbits,
                                    exponent) == result);
    if (result == 0) {
        TEST_ASSERT(mbedtls_rsa_check_privkey(&ctx) == 0);
        TEST_ASSERT(mbedtls_mpi_cmp_mpi(&ctx.P, &ctx.Q) > 0);
    }

exit:
    mbedtls_rsa_free(&ctx);
}

static void test_mbedtls_rsa_gen_key_wrapper( void ** params )
{

    test_mbedtls_rsa_gen_key( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
#line 751 "suites/test_suite_rsa.function"
static void test_mbedtls_rsa_deduce_primes(char *input_N,
                               char *input_D,
                               char *input_E,
                               char *output_P,
                               char *output_Q,
                               int corrupt, int result)
{
    mbedtls_mpi N, P, Pp, Q, Qp, D, E;

    mbedtls_mpi_init(&N);
    mbedtls_mpi_init(&P);  mbedtls_mpi_init(&Q);
    mbedtls_mpi_init(&Pp); mbedtls_mpi_init(&Qp);
    mbedtls_mpi_init(&D); mbedtls_mpi_init(&E);

    TEST_ASSERT(mbedtls_test_read_mpi(&N, input_N) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&D, input_D) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&E, input_E) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&Qp, output_P) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&Pp, output_Q) == 0);

    if (corrupt) {
        TEST_ASSERT(mbedtls_mpi_add_int(&D, &D, 2) == 0);
    }

    /* Try to deduce P, Q from N, D, E only. */
    TEST_ASSERT(mbedtls_rsa_deduce_primes(&N, &D, &E, &P, &Q) == result);

    if (!corrupt) {
        /* Check if (P,Q) = (Pp, Qp) or (P,Q) = (Qp, Pp) */
        TEST_ASSERT((mbedtls_mpi_cmp_mpi(&P, &Pp) == 0 && mbedtls_mpi_cmp_mpi(&Q, &Qp) == 0) ||
                    (mbedtls_mpi_cmp_mpi(&P, &Qp) == 0 && mbedtls_mpi_cmp_mpi(&Q, &Pp) == 0));
    }

exit:
    mbedtls_mpi_free(&N);
    mbedtls_mpi_free(&P); mbedtls_mpi_free(&Q);
    mbedtls_mpi_free(&Pp); mbedtls_mpi_free(&Qp);
    mbedtls_mpi_free(&D); mbedtls_mpi_free(&E);
}

static void test_mbedtls_rsa_deduce_primes_wrapper( void ** params )
{

    test_mbedtls_rsa_deduce_primes( (char *) params[0], (char *) params[1], (char *) params[2], (char *) params[3], (char *) params[4], *( (int *) params[5] ), *( (int *) params[6] ) );
}
#line 793 "suites/test_suite_rsa.function"
static void test_mbedtls_rsa_deduce_private_exponent(char *input_P,
                                         char *input_Q,
                                         char *input_E,
                                         char *output_D,
                                         int corrupt, int result)
{
    mbedtls_mpi P, Q, D, Dp, E, R, Rp;

    mbedtls_mpi_init(&P); mbedtls_mpi_init(&Q);
    mbedtls_mpi_init(&D); mbedtls_mpi_init(&Dp);
    mbedtls_mpi_init(&E);
    mbedtls_mpi_init(&R); mbedtls_mpi_init(&Rp);

    TEST_ASSERT(mbedtls_test_read_mpi(&P, input_P) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&Q, input_Q) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&E, input_E) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&Dp, output_D) == 0);

    if (corrupt) {
        /* Make E even */
        TEST_ASSERT(mbedtls_mpi_set_bit(&E, 0, 0) == 0);
    }

    /* Try to deduce D from N, P, Q, E. */
    TEST_ASSERT(mbedtls_rsa_deduce_private_exponent(&P, &Q,
                                                    &E, &D) == result);

    if (!corrupt) {
        /*
         * Check that D and Dp agree modulo LCM(P-1, Q-1).
         */

        /* Replace P,Q by P-1, Q-1 */
        TEST_ASSERT(mbedtls_mpi_sub_int(&P, &P, 1) == 0);
        TEST_ASSERT(mbedtls_mpi_sub_int(&Q, &Q, 1) == 0);

        /* Check D == Dp modulo P-1 */
        TEST_ASSERT(mbedtls_mpi_mod_mpi(&R,  &D,  &P) == 0);
        TEST_ASSERT(mbedtls_mpi_mod_mpi(&Rp, &Dp, &P) == 0);
        TEST_ASSERT(mbedtls_mpi_cmp_mpi(&R,  &Rp)     == 0);

        /* Check D == Dp modulo Q-1 */
        TEST_ASSERT(mbedtls_mpi_mod_mpi(&R,  &D,  &Q) == 0);
        TEST_ASSERT(mbedtls_mpi_mod_mpi(&Rp, &Dp, &Q) == 0);
        TEST_ASSERT(mbedtls_mpi_cmp_mpi(&R,  &Rp)     == 0);
    }

exit:

    mbedtls_mpi_free(&P); mbedtls_mpi_free(&Q);
    mbedtls_mpi_free(&D); mbedtls_mpi_free(&Dp);
    mbedtls_mpi_free(&E);
    mbedtls_mpi_free(&R); mbedtls_mpi_free(&Rp);
}

static void test_mbedtls_rsa_deduce_private_exponent_wrapper( void ** params )
{

    test_mbedtls_rsa_deduce_private_exponent( (char *) params[0], (char *) params[1], (char *) params[2], (char *) params[3], *( (int *) params[4] ), *( (int *) params[5] ) );
}
#line 850 "suites/test_suite_rsa.function"
static void test_mbedtls_rsa_import(char *input_N,
                        char *input_P,
                        char *input_Q,
                        char *input_D,
                        char *input_E,
                        int successive,
                        int is_priv,
                        int res_check,
                        int res_complete)
{
    mbedtls_mpi N, P, Q, D, E;
    mbedtls_rsa_context ctx;

    /* Buffers used for encryption-decryption test */
    unsigned char *buf_orig = NULL;
    unsigned char *buf_enc  = NULL;
    unsigned char *buf_dec  = NULL;

    const int have_N = (strlen(input_N) > 0);
    const int have_P = (strlen(input_P) > 0);
    const int have_Q = (strlen(input_Q) > 0);
    const int have_D = (strlen(input_D) > 0);
    const int have_E = (strlen(input_E) > 0);

    mbedtls_rsa_init(&ctx);

    mbedtls_mpi_init(&N);
    mbedtls_mpi_init(&P); mbedtls_mpi_init(&Q);
    mbedtls_mpi_init(&D); mbedtls_mpi_init(&E);

    if (have_N) {
        TEST_ASSERT(mbedtls_test_read_mpi(&N, input_N) == 0);
    }

    if (have_P) {
        TEST_ASSERT(mbedtls_test_read_mpi(&P, input_P) == 0);
    }

    if (have_Q) {
        TEST_ASSERT(mbedtls_test_read_mpi(&Q, input_Q) == 0);
    }

    if (have_D) {
        TEST_ASSERT(mbedtls_test_read_mpi(&D, input_D) == 0);
    }

    if (have_E) {
        TEST_ASSERT(mbedtls_test_read_mpi(&E, input_E) == 0);
    }

    if (!successive) {
        TEST_ASSERT(mbedtls_rsa_import(&ctx,
                                       have_N ? &N : NULL,
                                       have_P ? &P : NULL,
                                       have_Q ? &Q : NULL,
                                       have_D ? &D : NULL,
                                       have_E ? &E : NULL) == 0);
    } else {
        /* Import N, P, Q, D, E separately.
         * This should make no functional difference. */

        TEST_ASSERT(mbedtls_rsa_import(&ctx,
                                       have_N ? &N : NULL,
                                       NULL, NULL, NULL, NULL) == 0);

        TEST_ASSERT(mbedtls_rsa_import(&ctx,
                                       NULL,
                                       have_P ? &P : NULL,
                                       NULL, NULL, NULL) == 0);

        TEST_ASSERT(mbedtls_rsa_import(&ctx,
                                       NULL, NULL,
                                       have_Q ? &Q : NULL,
                                       NULL, NULL) == 0);

        TEST_ASSERT(mbedtls_rsa_import(&ctx,
                                       NULL, NULL, NULL,
                                       have_D ? &D : NULL,
                                       NULL) == 0);

        TEST_ASSERT(mbedtls_rsa_import(&ctx,
                                       NULL, NULL, NULL, NULL,
                                       have_E ? &E : NULL) == 0);
    }

    TEST_ASSERT(mbedtls_rsa_complete(&ctx) == res_complete);

    /* On expected success, perform some public and private
     * key operations to check if the key is working properly. */
    if (res_complete == 0) {
        if (is_priv) {
            TEST_ASSERT(mbedtls_rsa_check_privkey(&ctx) == res_check);
        } else {
            TEST_ASSERT(mbedtls_rsa_check_pubkey(&ctx) == res_check);
        }

        if (res_check != 0) {
            goto exit;
        }

        buf_orig = mbedtls_calloc(1, mbedtls_rsa_get_len(&ctx));
        buf_enc  = mbedtls_calloc(1, mbedtls_rsa_get_len(&ctx));
        buf_dec  = mbedtls_calloc(1, mbedtls_rsa_get_len(&ctx));
        if (buf_orig == NULL || buf_enc == NULL || buf_dec == NULL) {
            goto exit;
        }

        /* This test uses an insecure RNG, suitable only for testing.
         * In production, always use a cryptographically strong RNG! */
        TEST_ASSERT(mbedtls_test_rnd_std_rand(NULL,
                                              buf_orig, mbedtls_rsa_get_len(&ctx)) == 0);

        /* Make sure the number we're generating is smaller than the modulus */
        buf_orig[0] = 0x00;

        TEST_ASSERT(mbedtls_rsa_public(&ctx, buf_orig, buf_enc) == 0);

        if (is_priv) {
            /* This test uses an insecure RNG, suitable only for testing.
             * In production, always use a cryptographically strong RNG! */
            TEST_ASSERT(mbedtls_rsa_private(&ctx, mbedtls_test_rnd_std_rand,
                                            NULL, buf_enc,
                                            buf_dec) == 0);

            TEST_ASSERT(memcmp(buf_orig, buf_dec,
                               mbedtls_rsa_get_len(&ctx)) == 0);
        }
    }

exit:

    mbedtls_free(buf_orig);
    mbedtls_free(buf_enc);
    mbedtls_free(buf_dec);

    mbedtls_rsa_free(&ctx);

    mbedtls_mpi_free(&N);
    mbedtls_mpi_free(&P); mbedtls_mpi_free(&Q);
    mbedtls_mpi_free(&D); mbedtls_mpi_free(&E);
}

static void test_mbedtls_rsa_import_wrapper( void ** params )
{

    test_mbedtls_rsa_import( (char *) params[0], (char *) params[1], (char *) params[2], (char *) params[3], (char *) params[4], *( (int *) params[5] ), *( (int *) params[6] ), *( (int *) params[7] ), *( (int *) params[8] ) );
}
#line 994 "suites/test_suite_rsa.function"
static void test_mbedtls_rsa_export(char *input_N,
                        char *input_P,
                        char *input_Q,
                        char *input_D,
                        char *input_E,
                        int is_priv,
                        int successive)
{
    /* Original MPI's with which we set up the RSA context */
    mbedtls_mpi N, P, Q, D, E;

    /* Exported MPI's */
    mbedtls_mpi Ne, Pe, Qe, De, Ee;

    const int have_N = (strlen(input_N) > 0);
    const int have_P = (strlen(input_P) > 0);
    const int have_Q = (strlen(input_Q) > 0);
    const int have_D = (strlen(input_D) > 0);
    const int have_E = (strlen(input_E) > 0);

    mbedtls_rsa_context ctx;

    mbedtls_rsa_init(&ctx);

    mbedtls_mpi_init(&N);
    mbedtls_mpi_init(&P); mbedtls_mpi_init(&Q);
    mbedtls_mpi_init(&D); mbedtls_mpi_init(&E);

    mbedtls_mpi_init(&Ne);
    mbedtls_mpi_init(&Pe); mbedtls_mpi_init(&Qe);
    mbedtls_mpi_init(&De); mbedtls_mpi_init(&Ee);

    /* Setup RSA context */

    if (have_N) {
        TEST_ASSERT(mbedtls_test_read_mpi(&N, input_N) == 0);
    }

    if (have_P) {
        TEST_ASSERT(mbedtls_test_read_mpi(&P, input_P) == 0);
    }

    if (have_Q) {
        TEST_ASSERT(mbedtls_test_read_mpi(&Q, input_Q) == 0);
    }

    if (have_D) {
        TEST_ASSERT(mbedtls_test_read_mpi(&D, input_D) == 0);
    }

    if (have_E) {
        TEST_ASSERT(mbedtls_test_read_mpi(&E, input_E) == 0);
    }

    TEST_ASSERT(mbedtls_rsa_import(&ctx,
                                   strlen(input_N) ? &N : NULL,
                                   strlen(input_P) ? &P : NULL,
                                   strlen(input_Q) ? &Q : NULL,
                                   strlen(input_D) ? &D : NULL,
                                   strlen(input_E) ? &E : NULL) == 0);

    TEST_ASSERT(mbedtls_rsa_complete(&ctx) == 0);

    /*
     * Export parameters and compare to original ones.
     */

    /* N and E must always be present. */
    if (!successive) {
        TEST_ASSERT(mbedtls_rsa_export(&ctx, &Ne, NULL, NULL, NULL, &Ee) == 0);
    } else {
        TEST_ASSERT(mbedtls_rsa_export(&ctx, &Ne, NULL, NULL, NULL, NULL) == 0);
        TEST_ASSERT(mbedtls_rsa_export(&ctx, NULL, NULL, NULL, NULL, &Ee) == 0);
    }
    TEST_ASSERT(mbedtls_mpi_cmp_mpi(&N, &Ne) == 0);
    TEST_ASSERT(mbedtls_mpi_cmp_mpi(&E, &Ee) == 0);

    /* If we were providing enough information to setup a complete private context,
     * we expect to be able to export all core parameters. */

    if (is_priv) {
        if (!successive) {
            TEST_ASSERT(mbedtls_rsa_export(&ctx, NULL, &Pe, &Qe,
                                           &De, NULL) == 0);
        } else {
            TEST_ASSERT(mbedtls_rsa_export(&ctx, NULL, &Pe, NULL,
                                           NULL, NULL) == 0);
            TEST_ASSERT(mbedtls_rsa_export(&ctx, NULL, NULL, &Qe,
                                           NULL, NULL) == 0);
            TEST_ASSERT(mbedtls_rsa_export(&ctx, NULL, NULL, NULL,
                                           &De, NULL) == 0);
        }

        if (have_P) {
            TEST_ASSERT(mbedtls_mpi_cmp_mpi(&P, &Pe) == 0);
        }

        if (have_Q) {
            TEST_ASSERT(mbedtls_mpi_cmp_mpi(&Q, &Qe) == 0);
        }

        if (have_D) {
            TEST_ASSERT(mbedtls_mpi_cmp_mpi(&D, &De) == 0);
        }

        /* While at it, perform a sanity check */
        TEST_ASSERT(mbedtls_rsa_validate_params(&Ne, &Pe, &Qe, &De, &Ee,
                                                NULL, NULL) == 0);
    }

exit:

    mbedtls_rsa_free(&ctx);

    mbedtls_mpi_free(&N);
    mbedtls_mpi_free(&P); mbedtls_mpi_free(&Q);
    mbedtls_mpi_free(&D); mbedtls_mpi_free(&E);

    mbedtls_mpi_free(&Ne);
    mbedtls_mpi_free(&Pe); mbedtls_mpi_free(&Qe);
    mbedtls_mpi_free(&De); mbedtls_mpi_free(&Ee);
}

static void test_mbedtls_rsa_export_wrapper( void ** params )
{

    test_mbedtls_rsa_export( (char *) params[0], (char *) params[1], (char *) params[2], (char *) params[3], (char *) params[4], *( (int *) params[5] ), *( (int *) params[6] ) );
}
#line 1119 "suites/test_suite_rsa.function"
static void test_mbedtls_rsa_validate_params(char *input_N,
                                 char *input_P,
                                 char *input_Q,
                                 char *input_D,
                                 char *input_E,
                                 int prng, int result)
{
    /* Original MPI's with which we set up the RSA context */
    mbedtls_mpi N, P, Q, D, E;

    const int have_N = (strlen(input_N) > 0);
    const int have_P = (strlen(input_P) > 0);
    const int have_Q = (strlen(input_Q) > 0);
    const int have_D = (strlen(input_D) > 0);
    const int have_E = (strlen(input_E) > 0);

    mbedtls_mpi_init(&N);
    mbedtls_mpi_init(&P); mbedtls_mpi_init(&Q);
    mbedtls_mpi_init(&D); mbedtls_mpi_init(&E);

    if (have_N) {
        TEST_ASSERT(mbedtls_test_read_mpi(&N, input_N) == 0);
    }

    if (have_P) {
        TEST_ASSERT(mbedtls_test_read_mpi(&P, input_P) == 0);
    }

    if (have_Q) {
        TEST_ASSERT(mbedtls_test_read_mpi(&Q, input_Q) == 0);
    }

    if (have_D) {
        TEST_ASSERT(mbedtls_test_read_mpi(&D, input_D) == 0);
    }

    if (have_E) {
        TEST_ASSERT(mbedtls_test_read_mpi(&E, input_E) == 0);
    }

    /* This test uses an insecure RNG, suitable only for testing.
     * In production, always use a cryptographically strong RNG! */
    TEST_ASSERT(mbedtls_rsa_validate_params(have_N ? &N : NULL,
                                            have_P ? &P : NULL,
                                            have_Q ? &Q : NULL,
                                            have_D ? &D : NULL,
                                            have_E ? &E : NULL,
                                            prng ? mbedtls_test_rnd_std_rand : NULL,
                                            prng ? NULL : NULL) == result);

exit:
    mbedtls_mpi_free(&N);
    mbedtls_mpi_free(&P); mbedtls_mpi_free(&Q);
    mbedtls_mpi_free(&D); mbedtls_mpi_free(&E);
}

static void test_mbedtls_rsa_validate_params_wrapper( void ** params )
{

    test_mbedtls_rsa_validate_params( (char *) params[0], (char *) params[1], (char *) params[2], (char *) params[3], (char *) params[4], *( (int *) params[5] ), *( (int *) params[6] ) );
}
#line 1177 "suites/test_suite_rsa.function"
static void test_mbedtls_rsa_export_raw(data_t *input_N, data_t *input_P,
                            data_t *input_Q, data_t *input_D,
                            data_t *input_E, int is_priv,
                            int successive)
{
    /* Exported buffers */
    unsigned char bufNe[256];
    unsigned char bufPe[128];
    unsigned char bufQe[128];
    unsigned char bufDe[256];
    unsigned char bufEe[1];

    mbedtls_rsa_context ctx;

    mbedtls_rsa_init(&ctx);

    /* Setup RSA context */
    TEST_ASSERT(mbedtls_rsa_import_raw(&ctx,
                                       input_N->len ? input_N->x : NULL, input_N->len,
                                       input_P->len ? input_P->x : NULL, input_P->len,
                                       input_Q->len ? input_Q->x : NULL, input_Q->len,
                                       input_D->len ? input_D->x : NULL, input_D->len,
                                       input_E->len ? input_E->x : NULL, input_E->len) == 0);

    TEST_ASSERT(mbedtls_rsa_complete(&ctx) == 0);

    /*
     * Export parameters and compare to original ones.
     */

    /* N and E must always be present. */
    if (!successive) {
        TEST_ASSERT(mbedtls_rsa_export_raw(&ctx, bufNe, input_N->len,
                                           NULL, 0, NULL, 0, NULL, 0,
                                           bufEe, input_E->len) == 0);
    } else {
        TEST_ASSERT(mbedtls_rsa_export_raw(&ctx, bufNe, input_N->len,
                                           NULL, 0, NULL, 0, NULL, 0,
                                           NULL, 0) == 0);
        TEST_ASSERT(mbedtls_rsa_export_raw(&ctx, NULL, 0,
                                           NULL, 0, NULL, 0, NULL, 0,
                                           bufEe, input_E->len) == 0);
    }
    TEST_ASSERT(memcmp(input_N->x, bufNe, input_N->len) == 0);
    TEST_ASSERT(memcmp(input_E->x, bufEe, input_E->len) == 0);

    /* If we were providing enough information to setup a complete private context,
     * we expect to be able to export all core parameters. */

    if (is_priv) {
        if (!successive) {
            TEST_ASSERT(mbedtls_rsa_export_raw(&ctx, NULL, 0,
                                               bufPe, input_P->len ? input_P->len : sizeof(bufPe),
                                               bufQe, input_Q->len ? input_Q->len : sizeof(bufQe),
                                               bufDe, input_D->len ? input_D->len : sizeof(bufDe),
                                               NULL, 0) == 0);
        } else {
            TEST_ASSERT(mbedtls_rsa_export_raw(&ctx, NULL, 0,
                                               bufPe, input_P->len ? input_P->len : sizeof(bufPe),
                                               NULL, 0, NULL, 0,
                                               NULL, 0) == 0);

            TEST_ASSERT(mbedtls_rsa_export_raw(&ctx, NULL, 0, NULL, 0,
                                               bufQe, input_Q->len ? input_Q->len : sizeof(bufQe),
                                               NULL, 0, NULL, 0) == 0);

            TEST_ASSERT(mbedtls_rsa_export_raw(&ctx, NULL, 0, NULL, 0, NULL, 0,
                                               bufDe, input_D->len ? input_D->len : sizeof(bufDe),
                                               NULL, 0) == 0);
        }

        if (input_P->len) {
            TEST_ASSERT(memcmp(input_P->x, bufPe, input_P->len) == 0);
        }

        if (input_Q->len) {
            TEST_ASSERT(memcmp(input_Q->x, bufQe, input_Q->len) == 0);
        }

        if (input_D->len) {
            TEST_ASSERT(memcmp(input_D->x, bufDe, input_D->len) == 0);
        }

    }

exit:
    mbedtls_rsa_free(&ctx);
}

static void test_mbedtls_rsa_export_raw_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_mbedtls_rsa_export_raw( &data0, &data2, &data4, &data6, &data8, *( (int *) params[10] ), *( (int *) params[11] ) );
}
#line 1268 "suites/test_suite_rsa.function"
static void test_mbedtls_rsa_import_raw(data_t *input_N,
                            data_t *input_P, data_t *input_Q,
                            data_t *input_D, data_t *input_E,
                            int successive,
                            int is_priv,
                            int res_check,
                            int res_complete)
{
    /* Buffers used for encryption-decryption test */
    unsigned char *buf_orig = NULL;
    unsigned char *buf_enc  = NULL;
    unsigned char *buf_dec  = NULL;

    mbedtls_rsa_context ctx;

    mbedtls_rsa_init(&ctx);

    if (!successive) {
        TEST_ASSERT(mbedtls_rsa_import_raw(&ctx,
                                           (input_N->len > 0) ? input_N->x : NULL, input_N->len,
                                           (input_P->len > 0) ? input_P->x : NULL, input_P->len,
                                           (input_Q->len > 0) ? input_Q->x : NULL, input_Q->len,
                                           (input_D->len > 0) ? input_D->x : NULL, input_D->len,
                                           (input_E->len > 0) ? input_E->x : NULL,
                                           input_E->len) == 0);
    } else {
        /* Import N, P, Q, D, E separately.
         * This should make no functional difference. */

        TEST_ASSERT(mbedtls_rsa_import_raw(&ctx,
                                           (input_N->len > 0) ? input_N->x : NULL, input_N->len,
                                           NULL, 0, NULL, 0, NULL, 0, NULL, 0) == 0);

        TEST_ASSERT(mbedtls_rsa_import_raw(&ctx,
                                           NULL, 0,
                                           (input_P->len > 0) ? input_P->x : NULL, input_P->len,
                                           NULL, 0, NULL, 0, NULL, 0) == 0);

        TEST_ASSERT(mbedtls_rsa_import_raw(&ctx,
                                           NULL, 0, NULL, 0,
                                           (input_Q->len > 0) ? input_Q->x : NULL, input_Q->len,
                                           NULL, 0, NULL, 0) == 0);

        TEST_ASSERT(mbedtls_rsa_import_raw(&ctx,
                                           NULL, 0, NULL, 0, NULL, 0,
                                           (input_D->len > 0) ? input_D->x : NULL, input_D->len,
                                           NULL, 0) == 0);

        TEST_ASSERT(mbedtls_rsa_import_raw(&ctx,
                                           NULL, 0, NULL, 0, NULL, 0, NULL, 0,
                                           (input_E->len > 0) ? input_E->x : NULL,
                                           input_E->len) == 0);
    }

    TEST_ASSERT(mbedtls_rsa_complete(&ctx) == res_complete);

    /* On expected success, perform some public and private
     * key operations to check if the key is working properly. */
    if (res_complete == 0) {
        if (is_priv) {
            TEST_ASSERT(mbedtls_rsa_check_privkey(&ctx) == res_check);
        } else {
            TEST_ASSERT(mbedtls_rsa_check_pubkey(&ctx) == res_check);
        }

        if (res_check != 0) {
            goto exit;
        }

        buf_orig = mbedtls_calloc(1, mbedtls_rsa_get_len(&ctx));
        buf_enc  = mbedtls_calloc(1, mbedtls_rsa_get_len(&ctx));
        buf_dec  = mbedtls_calloc(1, mbedtls_rsa_get_len(&ctx));
        if (buf_orig == NULL || buf_enc == NULL || buf_dec == NULL) {
            goto exit;
        }

        /* This test uses an insecure RNG, suitable only for testing.
         * In production, always use a cryptographically strong RNG! */
        TEST_ASSERT(mbedtls_test_rnd_std_rand(NULL,
                                              buf_orig, mbedtls_rsa_get_len(&ctx)) == 0);

        /* Make sure the number we're generating is smaller than the modulus */
        buf_orig[0] = 0x00;

        TEST_ASSERT(mbedtls_rsa_public(&ctx, buf_orig, buf_enc) == 0);

        if (is_priv) {
            /* This test uses an insecure RNG, suitable only for testing.
             * In production, always use a cryptographically strong RNG! */
            TEST_ASSERT(mbedtls_rsa_private(&ctx, mbedtls_test_rnd_std_rand,
                                            NULL, buf_enc,
                                            buf_dec) == 0);

            TEST_ASSERT(memcmp(buf_orig, buf_dec,
                               mbedtls_rsa_get_len(&ctx)) == 0);
        }
    }

exit:

    mbedtls_free(buf_orig);
    mbedtls_free(buf_enc);
    mbedtls_free(buf_dec);

    mbedtls_rsa_free(&ctx);
}

static void test_mbedtls_rsa_import_raw_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_mbedtls_rsa_import_raw( &data0, &data2, &data4, &data6, &data8, *( (int *) params[10] ), *( (int *) params[11] ), *( (int *) params[12] ), *( (int *) params[13] ) );
}
#if defined(MBEDTLS_SELF_TEST)
#line 1377 "suites/test_suite_rsa.function"
static void test_rsa_selftest(void)
{
    TEST_ASSERT(mbedtls_rsa_self_test(1) == 0);
exit:
    ;
}

static void test_rsa_selftest_wrapper( void ** params )
{
    (void)params;

    test_rsa_selftest(  );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_GENPRIME */
#endif /* MBEDTLS_BIGNUM_C */
#endif /* MBEDTLS_RSA_C */


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
    
#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)

        case 0:
            {
                *out_value = MBEDTLS_RSA_PKCS_V15;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_MD_SHA1;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_ERR_RSA_VERIFY_FAILED;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_MD_SHA224;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_MD_SHA256;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_MD_SHA384;
            }
            break;
        case 6:
            {
                *out_value = MBEDTLS_MD_SHA512;
            }
            break;
        case 7:
            {
                *out_value = MBEDTLS_MD_MD5;
            }
            break;
        case 8:
            {
                *out_value = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
            }
            break;
        case 9:
            {
                *out_value = MBEDTLS_MD_RIPEMD160;
            }
            break;
        case 10:
            {
                *out_value = MBEDTLS_ERR_RSA_PRIVATE_FAILED + MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
            }
            break;
        case 11:
            {
                *out_value = MBEDTLS_ERR_RSA_OUTPUT_TOO_LARGE;
            }
            break;
        case 12:
            {
                *out_value = MBEDTLS_ERR_RSA_KEY_CHECK_FAILED;
            }
            break;
        case 13:
            {
                *out_value = MBEDTLS_ERR_RSA_PUBLIC_FAILED + MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
            }
            break;
        case 14:
            {
                *out_value = MBEDTLS_ERR_MPI_NOT_ACCEPTABLE;
            }
            break;
        case 15:
            {
                *out_value = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
            }
            break;
        case 16:
            {
                *out_value = MBEDTLS_ERR_RSA_RNG_FAILED;
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
    
#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)

        case 0:
            {
#if defined(MBEDTLS_HAS_ALG_SHA_1_VIA_LOWLEVEL_OR_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(MBEDTLS_PKCS1_V15)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(MBEDTLS_HAS_ALG_SHA_224_VIA_LOWLEVEL_OR_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 3:
            {
#if defined(MBEDTLS_HAS_ALG_SHA_256_VIA_LOWLEVEL_OR_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 4:
            {
#if defined(MBEDTLS_HAS_ALG_SHA_384_VIA_LOWLEVEL_OR_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 5:
            {
#if defined(MBEDTLS_HAS_ALG_SHA_512_VIA_LOWLEVEL_OR_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 6:
            {
#if defined(MBEDTLS_HAS_ALG_MD5_VIA_LOWLEVEL_OR_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 7:
            {
#if defined(MBEDTLS_HAS_ALG_RIPEMD160_VIA_LOWLEVEL_OR_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 8:
            {
#if !defined(MBEDTLS_RSA_NO_CRT)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 9:
            {
#if (MBEDTLS_MPI_MAX_SIZE>=1024)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 10:
            {
#if defined(MBEDTLS_SELF_TEST)
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

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_rsa_invalid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_rsa_init_free_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_pkcs1_sign_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_pkcs1_verify_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_rsa_pkcs1_sign_raw_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_rsa_pkcs1_verify_raw_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_pkcs1_encrypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_rsa_pkcs1_encrypt_bad_rng_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_pkcs1_decrypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_public_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_private_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_rsa_check_privkey_null_wrapper,
#else
    NULL,
#endif
/* Function Id: 12 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_check_pubkey_wrapper,
#else
    NULL,
#endif
/* Function Id: 13 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_check_privkey_wrapper,
#else
    NULL,
#endif
/* Function Id: 14 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_rsa_check_pubpriv_wrapper,
#else
    NULL,
#endif
/* Function Id: 15 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_gen_key_wrapper,
#else
    NULL,
#endif
/* Function Id: 16 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_deduce_primes_wrapper,
#else
    NULL,
#endif
/* Function Id: 17 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_deduce_private_exponent_wrapper,
#else
    NULL,
#endif
/* Function Id: 18 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_import_wrapper,
#else
    NULL,
#endif
/* Function Id: 19 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_export_wrapper,
#else
    NULL,
#endif
/* Function Id: 20 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_validate_params_wrapper,
#else
    NULL,
#endif
/* Function Id: 21 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_export_raw_wrapper,
#else
    NULL,
#endif
/* Function Id: 22 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_import_raw_wrapper,
#else
    NULL,
#endif
/* Function Id: 23 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME) && defined(MBEDTLS_SELF_TEST)
    test_rsa_selftest_wrapper,
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
    const char *default_filename = "./test_suite_rsa.datax";
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
