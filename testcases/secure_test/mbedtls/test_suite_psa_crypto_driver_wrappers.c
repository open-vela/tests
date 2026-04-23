#line 2 "suites/main_test.function"
/*
 * *** THIS FILE HAS BEEN MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py
 *
 * Test file      : ./test_suite_psa_crypto_driver_wrappers.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_psa_crypto_driver_wrappers.function
 *      Test suite data     : suites/test_suite_psa_crypto_driver_wrappers.data
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

#if defined(MBEDTLS_PSA_CRYPTO_C)
#if defined(MBEDTLS_PSA_CRYPTO_DRIVERS)
#if defined(PSA_CRYPTO_DRIVER_TEST)
#line 2 "suites/test_suite_psa_crypto_driver_wrappers.function"
#include "test/drivers/test_driver.h"

/* Auxiliary variables for pake tests.
   Global to silent the compiler when unused. */
size_t pake_expected_hit_count = 0;
int pake_in_driver = 0;

/* The only two JPAKE user/peer identifiers supported for the time being. */
static const uint8_t jpake_server_id[] = { 's', 'e', 'r', 'v', 'e', 'r' };
static const uint8_t jpake_client_id[] = { 'c', 'l', 'i', 'e', 'n', 't' };

#if defined(PSA_WANT_ALG_JPAKE) && defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR) && \
    defined(PSA_WANT_ECC_SECP_R1_256) && defined(PSA_WANT_ALG_SHA_256)
static void ecjpake_do_round(psa_algorithm_t alg, unsigned int primitive,
                             psa_pake_operation_t *server,
                             psa_pake_operation_t *client,
                             int client_input_first,
                             int round)
{
    unsigned char *buffer0 = NULL, *buffer1 = NULL;
    size_t buffer_length = (
        PSA_PAKE_OUTPUT_SIZE(alg, primitive, PSA_PAKE_STEP_KEY_SHARE) +
        PSA_PAKE_OUTPUT_SIZE(alg, primitive, PSA_PAKE_STEP_ZK_PUBLIC) +
        PSA_PAKE_OUTPUT_SIZE(alg, primitive, PSA_PAKE_STEP_ZK_PROOF)) * 2;
    /* The output should be exactly this size according to the spec */
    const size_t expected_size_key_share =
        PSA_PAKE_OUTPUT_SIZE(alg, primitive, PSA_PAKE_STEP_KEY_SHARE);
    /* The output should be exactly this size according to the spec */
    const size_t expected_size_zk_public =
        PSA_PAKE_OUTPUT_SIZE(alg, primitive, PSA_PAKE_STEP_ZK_PUBLIC);
    /* The output can be smaller: the spec allows stripping leading zeroes */
    const size_t max_expected_size_zk_proof =
        PSA_PAKE_OUTPUT_SIZE(alg, primitive, PSA_PAKE_STEP_ZK_PROOF);
    size_t buffer0_off = 0;
    size_t buffer1_off = 0;
    size_t s_g1_len, s_g2_len, s_a_len;
    size_t s_g1_off, s_g2_off, s_a_off;
    size_t s_x1_pk_len, s_x2_pk_len, s_x2s_pk_len;
    size_t s_x1_pk_off, s_x2_pk_off, s_x2s_pk_off;
    size_t s_x1_pr_len, s_x2_pr_len, s_x2s_pr_len;
    size_t s_x1_pr_off, s_x2_pr_off, s_x2s_pr_off;
    size_t c_g1_len, c_g2_len, c_a_len;
    size_t c_g1_off, c_g2_off, c_a_off;
    size_t c_x1_pk_len, c_x2_pk_len, c_x2s_pk_len;
    size_t c_x1_pk_off, c_x2_pk_off, c_x2s_pk_off;
    size_t c_x1_pr_len, c_x2_pr_len, c_x2s_pr_len;
    size_t c_x1_pr_off, c_x2_pr_off, c_x2s_pr_off;
    psa_status_t status;

    ASSERT_ALLOC(buffer0, buffer_length);
    ASSERT_ALLOC(buffer1, buffer_length);

    switch (round) {
        case 1:
            /* Server first round Output */
            PSA_ASSERT(psa_pake_output(server, PSA_PAKE_STEP_KEY_SHARE,
                                       buffer0 + buffer0_off,
                                       512 - buffer0_off, &s_g1_len));
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_EQUAL(s_g1_len, expected_size_key_share);
            s_g1_off = buffer0_off;
            buffer0_off += s_g1_len;
            PSA_ASSERT(psa_pake_output(server, PSA_PAKE_STEP_ZK_PUBLIC,
                                       buffer0 + buffer0_off,
                                       512 - buffer0_off, &s_x1_pk_len));
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_EQUAL(s_x1_pk_len, expected_size_zk_public);
            s_x1_pk_off = buffer0_off;
            buffer0_off += s_x1_pk_len;
            PSA_ASSERT(psa_pake_output(server, PSA_PAKE_STEP_ZK_PROOF,
                                       buffer0 + buffer0_off,
                                       512 - buffer0_off, &s_x1_pr_len));
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_LE_U(s_x1_pr_len, max_expected_size_zk_proof);
            s_x1_pr_off = buffer0_off;
            buffer0_off += s_x1_pr_len;
            PSA_ASSERT(psa_pake_output(server, PSA_PAKE_STEP_KEY_SHARE,
                                       buffer0 + buffer0_off,
                                       512 - buffer0_off, &s_g2_len));
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_EQUAL(s_g2_len, expected_size_key_share);
            s_g2_off = buffer0_off;
            buffer0_off += s_g2_len;
            PSA_ASSERT(psa_pake_output(server, PSA_PAKE_STEP_ZK_PUBLIC,
                                       buffer0 + buffer0_off,
                                       512 - buffer0_off, &s_x2_pk_len));
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_EQUAL(s_x2_pk_len, expected_size_zk_public);
            s_x2_pk_off = buffer0_off;
            buffer0_off += s_x2_pk_len;
            PSA_ASSERT(psa_pake_output(server, PSA_PAKE_STEP_ZK_PROOF,
                                       buffer0 + buffer0_off,
                                       512 - buffer0_off, &s_x2_pr_len));
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_LE_U(s_x2_pr_len, max_expected_size_zk_proof);
            s_x2_pr_off = buffer0_off;
            buffer0_off += s_x2_pr_len;

            if (client_input_first == 1) {
                /* Client first round Input */
                status = psa_pake_input(client, PSA_PAKE_STEP_KEY_SHARE,
                                        buffer0 + s_g1_off, s_g1_len);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                           pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
                TEST_EQUAL(status, PSA_SUCCESS);

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PUBLIC,
                                        buffer0 + s_x1_pk_off,
                                        s_x1_pk_len);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                           pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
                TEST_EQUAL(status, PSA_SUCCESS);

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PROOF,
                                        buffer0 + s_x1_pr_off,
                                        s_x1_pr_len);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                           pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
                TEST_EQUAL(status, PSA_SUCCESS);

                status = psa_pake_input(client, PSA_PAKE_STEP_KEY_SHARE,
                                        buffer0 + s_g2_off,
                                        s_g2_len);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                           pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
                TEST_EQUAL(status, PSA_SUCCESS);

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PUBLIC,
                                        buffer0 + s_x2_pk_off,
                                        s_x2_pk_len);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                           pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
                TEST_EQUAL(status, PSA_SUCCESS);

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PROOF,
                                        buffer0 + s_x2_pr_off,
                                        s_x2_pr_len);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                           pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
                TEST_EQUAL(status, PSA_SUCCESS);
            }

            /* Adjust for indirect client driver setup in first pake_output call. */
            pake_expected_hit_count++;

            /* Client first round Output */
            PSA_ASSERT(psa_pake_output(client, PSA_PAKE_STEP_KEY_SHARE,
                                       buffer1 + buffer1_off,
                                       512 - buffer1_off, &c_g1_len));
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_EQUAL(c_g1_len, expected_size_key_share);
            c_g1_off = buffer1_off;
            buffer1_off += c_g1_len;
            PSA_ASSERT(psa_pake_output(client, PSA_PAKE_STEP_ZK_PUBLIC,
                                       buffer1 + buffer1_off,
                                       512 - buffer1_off, &c_x1_pk_len));
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_EQUAL(c_x1_pk_len, expected_size_zk_public);
            c_x1_pk_off = buffer1_off;
            buffer1_off += c_x1_pk_len;
            PSA_ASSERT(psa_pake_output(client, PSA_PAKE_STEP_ZK_PROOF,
                                       buffer1 + buffer1_off,
                                       512 - buffer1_off, &c_x1_pr_len));
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_LE_U(c_x1_pr_len, max_expected_size_zk_proof);
            c_x1_pr_off = buffer1_off;
            buffer1_off += c_x1_pr_len;
            PSA_ASSERT(psa_pake_output(client, PSA_PAKE_STEP_KEY_SHARE,
                                       buffer1 + buffer1_off,
                                       512 - buffer1_off, &c_g2_len));
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_EQUAL(c_g2_len, expected_size_key_share);
            c_g2_off = buffer1_off;
            buffer1_off += c_g2_len;
            PSA_ASSERT(psa_pake_output(client, PSA_PAKE_STEP_ZK_PUBLIC,
                                       buffer1 + buffer1_off,
                                       512 - buffer1_off, &c_x2_pk_len));
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_EQUAL(c_x2_pk_len, expected_size_zk_public);
            c_x2_pk_off = buffer1_off;
            buffer1_off += c_x2_pk_len;
            PSA_ASSERT(psa_pake_output(client, PSA_PAKE_STEP_ZK_PROOF,
                                       buffer1 + buffer1_off,
                                       512 - buffer1_off, &c_x2_pr_len));
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_LE_U(c_x2_pr_len, max_expected_size_zk_proof);
            c_x2_pr_off = buffer1_off;
            buffer1_off += c_x2_pr_len;

            if (client_input_first == 0) {
                /* Client first round Input */
                status = psa_pake_input(client, PSA_PAKE_STEP_KEY_SHARE,
                                        buffer0 + s_g1_off, s_g1_len);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                           pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
                TEST_EQUAL(status, PSA_SUCCESS);

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PUBLIC,
                                        buffer0 + s_x1_pk_off,
                                        s_x1_pk_len);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                           pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
                TEST_EQUAL(status, PSA_SUCCESS);

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PROOF,
                                        buffer0 + s_x1_pr_off,
                                        s_x1_pr_len);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                           pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
                TEST_EQUAL(status, PSA_SUCCESS);

                status = psa_pake_input(client, PSA_PAKE_STEP_KEY_SHARE,
                                        buffer0 + s_g2_off,
                                        s_g2_len);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                           pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
                TEST_EQUAL(status, PSA_SUCCESS);

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PUBLIC,
                                        buffer0 + s_x2_pk_off,
                                        s_x2_pk_len);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                           pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
                TEST_EQUAL(status, PSA_SUCCESS);

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PROOF,
                                        buffer0 + s_x2_pr_off,
                                        s_x2_pr_len);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                           pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
                TEST_EQUAL(status, PSA_SUCCESS);
            }

            /* Server first round Input */
            status = psa_pake_input(server, PSA_PAKE_STEP_KEY_SHARE,
                                    buffer1 + c_g1_off, c_g1_len);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_EQUAL(status, PSA_SUCCESS);

            status = psa_pake_input(server, PSA_PAKE_STEP_ZK_PUBLIC,
                                    buffer1 + c_x1_pk_off, c_x1_pk_len);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_EQUAL(status, PSA_SUCCESS);

            status = psa_pake_input(server, PSA_PAKE_STEP_ZK_PROOF,
                                    buffer1 + c_x1_pr_off, c_x1_pr_len);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_EQUAL(status, PSA_SUCCESS);

            status = psa_pake_input(server, PSA_PAKE_STEP_KEY_SHARE,
                                    buffer1 + c_g2_off, c_g2_len);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_EQUAL(status, PSA_SUCCESS);

            status = psa_pake_input(server, PSA_PAKE_STEP_ZK_PUBLIC,
                                    buffer1 + c_x2_pk_off, c_x2_pk_len);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_EQUAL(status, PSA_SUCCESS);

            status = psa_pake_input(server, PSA_PAKE_STEP_ZK_PROOF,
                                    buffer1 + c_x2_pr_off, c_x2_pr_len);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_EQUAL(status, PSA_SUCCESS);

            break;

        case 2:
            /* Server second round Output */
            buffer0_off = 0;

            PSA_ASSERT(psa_pake_output(server, PSA_PAKE_STEP_KEY_SHARE,
                                       buffer0 + buffer0_off,
                                       512 - buffer0_off, &s_a_len));
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_EQUAL(s_a_len, expected_size_key_share);
            s_a_off = buffer0_off;
            buffer0_off += s_a_len;
            PSA_ASSERT(psa_pake_output(server, PSA_PAKE_STEP_ZK_PUBLIC,
                                       buffer0 + buffer0_off,
                                       512 - buffer0_off, &s_x2s_pk_len));
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_EQUAL(s_x2s_pk_len, expected_size_zk_public);
            s_x2s_pk_off = buffer0_off;
            buffer0_off += s_x2s_pk_len;
            PSA_ASSERT(psa_pake_output(server, PSA_PAKE_STEP_ZK_PROOF,
                                       buffer0 + buffer0_off,
                                       512 - buffer0_off, &s_x2s_pr_len));
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_LE_U(s_x2s_pr_len, max_expected_size_zk_proof);
            s_x2s_pr_off = buffer0_off;
            buffer0_off += s_x2s_pr_len;

            if (client_input_first == 1) {
                /* Client second round Input */
                status = psa_pake_input(client, PSA_PAKE_STEP_KEY_SHARE,
                                        buffer0 + s_a_off, s_a_len);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                           pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
                TEST_EQUAL(status, PSA_SUCCESS);

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PUBLIC,
                                        buffer0 + s_x2s_pk_off,
                                        s_x2s_pk_len);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                           pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
                TEST_EQUAL(status, PSA_SUCCESS);

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PROOF,
                                        buffer0 + s_x2s_pr_off,
                                        s_x2s_pr_len);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                           pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
                TEST_EQUAL(status, PSA_SUCCESS);
            }

            /* Client second round Output */
            buffer1_off = 0;

            PSA_ASSERT(psa_pake_output(client, PSA_PAKE_STEP_KEY_SHARE,
                                       buffer1 + buffer1_off,
                                       512 - buffer1_off, &c_a_len));
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_EQUAL(c_a_len, expected_size_key_share);
            c_a_off = buffer1_off;
            buffer1_off += c_a_len;
            PSA_ASSERT(psa_pake_output(client, PSA_PAKE_STEP_ZK_PUBLIC,
                                       buffer1 + buffer1_off,
                                       512 - buffer1_off, &c_x2s_pk_len));
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_EQUAL(c_x2s_pk_len, expected_size_zk_public);
            c_x2s_pk_off = buffer1_off;
            buffer1_off += c_x2s_pk_len;
            PSA_ASSERT(psa_pake_output(client, PSA_PAKE_STEP_ZK_PROOF,
                                       buffer1 + buffer1_off,
                                       512 - buffer1_off, &c_x2s_pr_len));
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_LE_U(c_x2s_pr_len, max_expected_size_zk_proof);
            c_x2s_pr_off = buffer1_off;
            buffer1_off += c_x2s_pr_len;

            if (client_input_first == 0) {
                /* Client second round Input */
                status = psa_pake_input(client, PSA_PAKE_STEP_KEY_SHARE,
                                        buffer0 + s_a_off, s_a_len);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                           pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
                TEST_EQUAL(status, PSA_SUCCESS);

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PUBLIC,
                                        buffer0 + s_x2s_pk_off,
                                        s_x2s_pk_len);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                           pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
                TEST_EQUAL(status, PSA_SUCCESS);

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PROOF,
                                        buffer0 + s_x2s_pr_off,
                                        s_x2s_pr_len);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                           pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
                TEST_EQUAL(status, PSA_SUCCESS);
            }

            /* Server second round Input */
            status = psa_pake_input(server, PSA_PAKE_STEP_KEY_SHARE,
                                    buffer1 + c_a_off, c_a_len);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_EQUAL(status, PSA_SUCCESS);

            status = psa_pake_input(server, PSA_PAKE_STEP_ZK_PUBLIC,
                                    buffer1 + c_x2s_pk_off, c_x2s_pk_len);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_EQUAL(status, PSA_SUCCESS);

            status = psa_pake_input(server, PSA_PAKE_STEP_ZK_PROOF,
                                    buffer1 + c_x2s_pr_off, c_x2s_pr_len);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
                       pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
            TEST_EQUAL(status, PSA_SUCCESS);

            break;
    }

exit:
    mbedtls_free(buffer0);
    mbedtls_free(buffer1);
}
#endif /* PSA_WANT_ALG_JPAKE */

#if defined(PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY)
/* Sanity checks on the output of RSA encryption.
 *
 * \param modulus               Key modulus. Must not have leading zeros.
 * \param private_exponent      Key private exponent.
 * \param alg                   An RSA algorithm.
 * \param input_data            The input plaintext.
 * \param buf                   The ciphertext produced by the driver.
 * \param length                Length of \p buf in bytes.
 */
static int sanity_check_rsa_encryption_result(
    psa_algorithm_t alg,
    const data_t *modulus, const data_t *private_exponent,
    const data_t *input_data,
    uint8_t *buf, size_t length)
{
#if defined(MBEDTLS_BIGNUM_C)
    mbedtls_mpi N, D, C, X;
    mbedtls_mpi_init(&N);
    mbedtls_mpi_init(&D);
    mbedtls_mpi_init(&C);
    mbedtls_mpi_init(&X);
#endif /* MBEDTLS_BIGNUM_C */

    int ok = 0;

    TEST_ASSERT(length == modulus->len);

#if defined(MBEDTLS_BIGNUM_C)
    /* Perform the private key operation */
    TEST_ASSERT(mbedtls_mpi_read_binary(&N, modulus->x, modulus->len) == 0);
    TEST_ASSERT(mbedtls_mpi_read_binary(&D,
                                        private_exponent->x,
                                        private_exponent->len) == 0);
    TEST_ASSERT(mbedtls_mpi_read_binary(&C, buf, length) == 0);
    TEST_ASSERT(mbedtls_mpi_exp_mod(&X, &C, &D, &N, NULL) == 0);

    /* Sanity checks on the padded plaintext */
    TEST_ASSERT(mbedtls_mpi_write_binary(&X, buf, length) == 0);

    if (alg == PSA_ALG_RSA_PKCS1V15_CRYPT) {
        TEST_ASSERT(length > input_data->len + 2);
        TEST_EQUAL(buf[0], 0x00);
        TEST_EQUAL(buf[1], 0x02);
        TEST_EQUAL(buf[length - input_data->len - 1], 0x00);
        ASSERT_COMPARE(buf + length - input_data->len, input_data->len,
                       input_data->x, input_data->len);
    } else if (PSA_ALG_IS_RSA_OAEP(alg)) {
        TEST_EQUAL(buf[0], 0x00);
        /* The rest is too hard to check */
    } else {
        TEST_ASSERT(!"Encryption result sanity check not implemented for RSA algorithm");
    }
#endif /* MBEDTLS_BIGNUM_C */

    ok = 1;

exit:
#if defined(MBEDTLS_BIGNUM_C)
    mbedtls_mpi_free(&N);
    mbedtls_mpi_free(&D);
    mbedtls_mpi_free(&C);
    mbedtls_mpi_free(&X);
#endif /* MBEDTLS_BIGNUM_C */
    return ok;
}
#endif
#line 492 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_sign_hash(int key_type_arg,
               int alg_arg,
               int force_status_arg,
               data_t *key_input,
               data_t *data_input,
               data_t *expected_output,
               int fake_output,
               int expected_status_arg)
{
    psa_status_t force_status = force_status_arg;
    psa_status_t expected_status = expected_status_arg;
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_algorithm_t alg = alg_arg;
    size_t key_bits;
    psa_key_type_t key_type = key_type_arg;
    unsigned char *signature = NULL;
    size_t signature_size;
    size_t signature_length = 0xdeadbeef;
    psa_status_t actual_status;
    mbedtls_test_driver_signature_sign_hooks =
        mbedtls_test_driver_signature_hooks_init();

    PSA_ASSERT(psa_crypto_init());
    psa_set_key_type(&attributes,
                     key_type);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&attributes, alg);
    psa_import_key(&attributes,
                   key_input->x, key_input->len,
                   &key);

    mbedtls_test_driver_signature_sign_hooks.forced_status = force_status;
    if (fake_output == 1) {
        mbedtls_test_driver_signature_sign_hooks.forced_output =
            expected_output->x;
        mbedtls_test_driver_signature_sign_hooks.forced_output_length =
            expected_output->len;
    }

    /* Allocate a buffer which has the size advertized by the
     * library. */
    PSA_ASSERT(psa_get_key_attributes(key, &attributes));
    key_bits = psa_get_key_bits(&attributes);
    signature_size = PSA_SIGN_OUTPUT_SIZE(key_type, key_bits, alg);

    TEST_ASSERT(signature_size != 0);
    TEST_ASSERT(signature_size <= PSA_SIGNATURE_MAX_SIZE);
    ASSERT_ALLOC(signature, signature_size);

    actual_status = psa_sign_hash(key, alg,
                                  data_input->x, data_input->len,
                                  signature, signature_size,
                                  &signature_length);
    TEST_EQUAL(actual_status, expected_status);
    if (expected_status == PSA_SUCCESS) {
        ASSERT_COMPARE(signature, signature_length,
                       expected_output->x, expected_output->len);
    }
    TEST_EQUAL(mbedtls_test_driver_signature_sign_hooks.hits, 1);

exit:
    psa_reset_key_attributes(&attributes);
    psa_destroy_key(key);
    mbedtls_free(signature);
    PSA_DONE();
    mbedtls_test_driver_signature_sign_hooks =
        mbedtls_test_driver_signature_hooks_init();
}

static void test_sign_hash_wrapper( void ** params )
{
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};

    test_sign_hash( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), &data3, &data5, &data7, *( (int *) params[9] ), *( (int *) params[10] ) );
}
#line 564 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_verify_hash(int key_type_arg,
                 int key_type_public_arg,
                 int alg_arg,
                 int force_status_arg,
                 int register_public_key,
                 data_t *key_input,
                 data_t *data_input,
                 data_t *signature_input,
                 int expected_status_arg)
{
    psa_status_t force_status = force_status_arg;
    psa_status_t expected_status = expected_status_arg;
    psa_algorithm_t alg = alg_arg;
    psa_key_type_t key_type = key_type_arg;
    psa_key_type_t key_type_public = key_type_public_arg;
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t actual_status;
    mbedtls_test_driver_signature_verify_hooks =
        mbedtls_test_driver_signature_hooks_init();

    PSA_ASSERT(psa_crypto_init());
    if (register_public_key) {
        psa_set_key_type(&attributes, key_type_public);
        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_VERIFY_HASH);
        psa_set_key_algorithm(&attributes, alg);
        psa_import_key(&attributes,
                       key_input->x, key_input->len,
                       &key);
    } else {
        psa_set_key_type(&attributes, key_type);
        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_VERIFY_HASH);
        psa_set_key_algorithm(&attributes, alg);
        psa_import_key(&attributes,
                       key_input->x, key_input->len,
                       &key);
    }

    mbedtls_test_driver_signature_verify_hooks.forced_status = force_status;

    actual_status = psa_verify_hash(key, alg,
                                    data_input->x, data_input->len,
                                    signature_input->x, signature_input->len);
    TEST_EQUAL(actual_status, expected_status);
    TEST_EQUAL(mbedtls_test_driver_signature_verify_hooks.hits, 1);

exit:
    psa_reset_key_attributes(&attributes);
    psa_destroy_key(key);
    PSA_DONE();
    mbedtls_test_driver_signature_verify_hooks =
        mbedtls_test_driver_signature_hooks_init();
}

static void test_verify_hash_wrapper( void ** params )
{
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data9 = {(uint8_t *) params[9], *( (uint32_t *) params[10] )};

    test_verify_hash( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), &data5, &data7, &data9, *( (int *) params[11] ) );
}
#line 620 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_sign_message(int key_type_arg,
                  int alg_arg,
                  int force_status_arg,
                  data_t *key_input,
                  data_t *data_input,
                  data_t *expected_output,
                  int fake_output,
                  int expected_status_arg)
{
    psa_status_t force_status = force_status_arg;
    psa_status_t expected_status = expected_status_arg;
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_algorithm_t alg = alg_arg;
    size_t key_bits;
    psa_key_type_t key_type = key_type_arg;
    unsigned char *signature = NULL;
    size_t signature_size;
    size_t signature_length = 0xdeadbeef;
    psa_status_t actual_status;
    mbedtls_test_driver_signature_sign_hooks =
        mbedtls_test_driver_signature_hooks_init();

    PSA_ASSERT(psa_crypto_init());
    psa_set_key_type(&attributes, key_type);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE);
    psa_set_key_algorithm(&attributes, alg);
    psa_import_key(&attributes,
                   key_input->x, key_input->len,
                   &key);

    mbedtls_test_driver_signature_sign_hooks.forced_status = force_status;
    if (fake_output == 1) {
        mbedtls_test_driver_signature_sign_hooks.forced_output =
            expected_output->x;
        mbedtls_test_driver_signature_sign_hooks.forced_output_length =
            expected_output->len;
    }

    /* Allocate a buffer which has the size advertized by the
     * library. */
    PSA_ASSERT(psa_get_key_attributes(key, &attributes));
    key_bits = psa_get_key_bits(&attributes);
    signature_size = PSA_SIGN_OUTPUT_SIZE(key_type, key_bits, alg);

    TEST_ASSERT(signature_size != 0);
    TEST_ASSERT(signature_size <= PSA_SIGNATURE_MAX_SIZE);
    ASSERT_ALLOC(signature, signature_size);

    actual_status = psa_sign_message(key, alg,
                                     data_input->x, data_input->len,
                                     signature, signature_size,
                                     &signature_length);
    TEST_EQUAL(actual_status, expected_status);
    if (expected_status == PSA_SUCCESS) {
        ASSERT_COMPARE(signature, signature_length,
                       expected_output->x, expected_output->len);
    }
    /* In the builtin algorithm the driver is called twice. */
    TEST_EQUAL(mbedtls_test_driver_signature_sign_hooks.hits,
               force_status == PSA_ERROR_NOT_SUPPORTED ? 2 : 1);

exit:
    psa_reset_key_attributes(&attributes);
    psa_destroy_key(key);
    mbedtls_free(signature);
    PSA_DONE();
    mbedtls_test_driver_signature_sign_hooks =
        mbedtls_test_driver_signature_hooks_init();
}

static void test_sign_message_wrapper( void ** params )
{
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};

    test_sign_message( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), &data3, &data5, &data7, *( (int *) params[9] ), *( (int *) params[10] ) );
}
#line 693 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_verify_message(int key_type_arg,
                    int key_type_public_arg,
                    int alg_arg,
                    int force_status_arg,
                    int register_public_key,
                    data_t *key_input,
                    data_t *data_input,
                    data_t *signature_input,
                    int expected_status_arg)
{
    psa_status_t force_status = force_status_arg;
    psa_status_t expected_status = expected_status_arg;
    psa_algorithm_t alg = alg_arg;
    psa_key_type_t key_type = key_type_arg;
    psa_key_type_t key_type_public = key_type_public_arg;
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t actual_status;
    mbedtls_test_driver_signature_verify_hooks =
        mbedtls_test_driver_signature_hooks_init();

    PSA_ASSERT(psa_crypto_init());
    if (register_public_key) {
        psa_set_key_type(&attributes, key_type_public);
        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_VERIFY_MESSAGE);
        psa_set_key_algorithm(&attributes, alg);
        psa_import_key(&attributes,
                       key_input->x, key_input->len,
                       &key);
    } else {
        psa_set_key_type(&attributes, key_type);
        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_VERIFY_MESSAGE);
        psa_set_key_algorithm(&attributes, alg);
        psa_import_key(&attributes,
                       key_input->x, key_input->len,
                       &key);
    }

    mbedtls_test_driver_signature_verify_hooks.forced_status = force_status;

    actual_status = psa_verify_message(key, alg,
                                       data_input->x, data_input->len,
                                       signature_input->x, signature_input->len);
    TEST_EQUAL(actual_status, expected_status);
    /* In the builtin algorithm the driver is called twice. */
    TEST_EQUAL(mbedtls_test_driver_signature_verify_hooks.hits,
               force_status == PSA_ERROR_NOT_SUPPORTED ? 2 : 1);

exit:
    psa_reset_key_attributes(&attributes);
    psa_destroy_key(key);
    PSA_DONE();
    mbedtls_test_driver_signature_verify_hooks =
        mbedtls_test_driver_signature_hooks_init();
}

static void test_verify_message_wrapper( void ** params )
{
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data9 = {(uint8_t *) params[9], *( (uint32_t *) params[10] )};

    test_verify_message( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), &data5, &data7, &data9, *( (int *) params[11] ) );
}
#if defined(PSA_WANT_ALG_ECDSA)
#if defined(PSA_WANT_ECC_SECP_R1_256)
#line 751 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_generate_key(int force_status_arg,
                  data_t *fake_output,
                  int expected_status_arg)
{
    psa_status_t force_status = force_status_arg;
    psa_status_t expected_status = expected_status_arg;
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_algorithm_t alg = PSA_ALG_ECDSA(PSA_ALG_SHA_256);
    const uint8_t *expected_output = NULL;
    size_t expected_output_length = 0;
    psa_status_t actual_status;
    uint8_t actual_output[PSA_KEY_EXPORT_ECC_KEY_PAIR_MAX_SIZE(256)] = { 0 };
    size_t actual_output_length;
    mbedtls_test_driver_key_management_hooks =
        mbedtls_test_driver_key_management_hooks_init();

    psa_set_key_type(&attributes,
                     PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&attributes, 256);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_EXPORT);
    psa_set_key_algorithm(&attributes, alg);

    if (fake_output->len > 0) {
        expected_output =
            mbedtls_test_driver_key_management_hooks.forced_output =
                fake_output->x;

        expected_output_length =
            mbedtls_test_driver_key_management_hooks.forced_output_length =
                fake_output->len;
    }

    mbedtls_test_driver_key_management_hooks.hits = 0;
    mbedtls_test_driver_key_management_hooks.forced_status = force_status;

    PSA_ASSERT(psa_crypto_init());

    actual_status = psa_generate_key(&attributes, &key);
    TEST_EQUAL(mbedtls_test_driver_key_management_hooks.hits, 1);
    TEST_EQUAL(actual_status, expected_status);

    if (actual_status == PSA_SUCCESS) {
        psa_export_key(key, actual_output, sizeof(actual_output), &actual_output_length);

        if (fake_output->len > 0) {
            ASSERT_COMPARE(actual_output, actual_output_length,
                           expected_output, expected_output_length);
        } else {
            size_t zeroes = 0;
            for (size_t i = 0; i < sizeof(actual_output); i++) {
                if (actual_output[i] == 0) {
                    zeroes++;
                }
            }
            TEST_ASSERT(zeroes != sizeof(actual_output));
        }
    }
exit:
    psa_reset_key_attributes(&attributes);
    psa_destroy_key(key);
    PSA_DONE();
    mbedtls_test_driver_key_management_hooks =
        mbedtls_test_driver_key_management_hooks_init();
}

static void test_generate_key_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_generate_key( *( (int *) params[0] ), &data1, *( (int *) params[3] ) );
}
#endif /* PSA_WANT_ECC_SECP_R1_256 */
#endif /* PSA_WANT_ALG_ECDSA */
#line 819 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_validate_key(int force_status_arg,
                  int location,
                  int owner_id_arg,
                  int id_arg,
                  int key_type_arg,
                  data_t *key_input,
                  int expected_status_arg)
{
    psa_key_lifetime_t lifetime =
        PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION( \
            PSA_KEY_PERSISTENCE_DEFAULT, location);
    mbedtls_svc_key_id_t id = mbedtls_svc_key_id_make(owner_id_arg, id_arg);
    psa_status_t force_status = force_status_arg;
    psa_status_t expected_status = expected_status_arg;
    psa_key_type_t key_type = key_type_arg;
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t actual_status;
    mbedtls_test_driver_key_management_hooks =
        mbedtls_test_driver_key_management_hooks_init();

    psa_set_key_id(&attributes, id);
    psa_set_key_type(&attributes,
                     key_type);
    psa_set_key_lifetime(&attributes, lifetime);
    psa_set_key_bits(&attributes, 0);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_EXPORT);

    mbedtls_test_driver_key_management_hooks.forced_status = force_status;

    PSA_ASSERT(psa_crypto_init());

    actual_status = psa_import_key(&attributes, key_input->x, key_input->len, &key);
    TEST_EQUAL(mbedtls_test_driver_key_management_hooks.hits, 1);
    TEST_EQUAL(actual_status, expected_status);
    TEST_EQUAL(mbedtls_test_driver_key_management_hooks.location, location);
exit:
    psa_reset_key_attributes(&attributes);
    psa_destroy_key(key);
    PSA_DONE();
    mbedtls_test_driver_key_management_hooks =
        mbedtls_test_driver_key_management_hooks_init();
}

static void test_validate_key_wrapper( void ** params )
{
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};

    test_validate_key( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), &data5, *( (int *) params[7] ) );
}
#line 865 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_export_key(int force_status_arg,
                data_t *fake_output,
                int key_in_type_arg,
                data_t *key_in,
                int key_out_type_arg,
                data_t *expected_output,
                int expected_status_arg)
{
    psa_status_t force_status = force_status_arg;
    psa_status_t expected_status = expected_status_arg;
    psa_key_handle_t handle = 0;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_type_t input_key_type = key_in_type_arg;
    psa_key_type_t output_key_type = key_out_type_arg;
    const uint8_t *expected_output_ptr = NULL;
    size_t expected_output_length = 0;
    psa_status_t actual_status;
    uint8_t actual_output[PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(256)] = { 0 };
    size_t actual_output_length;
    mbedtls_test_driver_key_management_hooks =
        mbedtls_test_driver_key_management_hooks_init();

    psa_set_key_type(&attributes, input_key_type);
    psa_set_key_bits(&attributes, 256);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_EXPORT);

    PSA_ASSERT(psa_crypto_init());
    PSA_ASSERT(psa_import_key(&attributes, key_in->x, key_in->len, &handle));

    if (fake_output->len > 0) {
        expected_output_ptr =
            mbedtls_test_driver_key_management_hooks.forced_output =
                fake_output->x;

        expected_output_length =
            mbedtls_test_driver_key_management_hooks.forced_output_length =
                fake_output->len;
    } else {
        expected_output_ptr = expected_output->x;
        expected_output_length = expected_output->len;
    }

    mbedtls_test_driver_key_management_hooks.hits = 0;
    mbedtls_test_driver_key_management_hooks.forced_status = force_status;

    if (PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY(output_key_type)) {
        actual_status = psa_export_public_key(handle,
                                              actual_output,
                                              sizeof(actual_output),
                                              &actual_output_length);
    } else {
        actual_status = psa_export_key(handle,
                                       actual_output,
                                       sizeof(actual_output),
                                       &actual_output_length);
    }
    TEST_EQUAL(actual_status, expected_status);

    if (PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY(output_key_type) &&
        !PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY(input_key_type)) {
        TEST_EQUAL(mbedtls_test_driver_key_management_hooks.hits, 1);
    }

    if (actual_status == PSA_SUCCESS) {
        ASSERT_COMPARE(actual_output, actual_output_length,
                       expected_output_ptr, expected_output_length);
    }
exit:
    psa_reset_key_attributes(&attributes);
    psa_destroy_key(handle);
    PSA_DONE();
    mbedtls_test_driver_key_management_hooks =
        mbedtls_test_driver_key_management_hooks_init();
}

static void test_export_key_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};

    test_export_key( *( (int *) params[0] ), &data1, *( (int *) params[3] ), &data4, *( (int *) params[6] ), &data7, *( (int *) params[9] ) );
}
#line 942 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_key_agreement(int alg_arg,
                   int force_status_arg,
                   int our_key_type_arg,
                   data_t *our_key_data,
                   data_t *peer_key_data,
                   data_t *expected_output,
                   data_t *fake_output,
                   int expected_status_arg)
{
    psa_status_t force_status = force_status_arg;
    psa_status_t expected_status = expected_status_arg;
    psa_algorithm_t alg = alg_arg;
    psa_key_type_t our_key_type = our_key_type_arg;
    mbedtls_svc_key_id_t our_key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    const uint8_t *expected_output_ptr = NULL;
    size_t expected_output_length = 0;
    unsigned char *actual_output = NULL;
    size_t actual_output_length = ~0;
    size_t key_bits;
    psa_status_t actual_status;
    mbedtls_test_driver_key_agreement_hooks =
        mbedtls_test_driver_key_agreement_hooks_init();

    PSA_ASSERT(psa_crypto_init());

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DERIVE);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, our_key_type);
    PSA_ASSERT(psa_import_key(&attributes,
                              our_key_data->x, our_key_data->len,
                              &our_key));

    PSA_ASSERT(psa_get_key_attributes(our_key, &attributes));
    key_bits = psa_get_key_bits(&attributes);

    TEST_LE_U(expected_output->len,
              PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE(our_key_type, key_bits));
    TEST_LE_U(PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE(our_key_type, key_bits),
              PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE);

    if (fake_output->len > 0) {
        expected_output_ptr =
            mbedtls_test_driver_key_agreement_hooks.forced_output =
                fake_output->x;

        expected_output_length =
            mbedtls_test_driver_key_agreement_hooks.forced_output_length =
                fake_output->len;
    } else {
        expected_output_ptr = expected_output->x;
        expected_output_length = expected_output->len;
    }

    mbedtls_test_driver_key_agreement_hooks.hits = 0;
    mbedtls_test_driver_key_agreement_hooks.forced_status = force_status;

    ASSERT_ALLOC(actual_output, expected_output->len);
    actual_status = psa_raw_key_agreement(alg, our_key,
                                          peer_key_data->x, peer_key_data->len,
                                          actual_output, expected_output->len,
                                          &actual_output_length);
    TEST_EQUAL(actual_status, expected_status);
    TEST_EQUAL(mbedtls_test_driver_key_agreement_hooks.hits, 1);

    if (actual_status == PSA_SUCCESS) {
        ASSERT_COMPARE(actual_output, actual_output_length,
                       expected_output_ptr, expected_output_length);
    }
    mbedtls_free(actual_output);
    actual_output = NULL;
    actual_output_length = ~0;

exit:
    psa_reset_key_attributes(&attributes);
    psa_destroy_key(our_key);
    PSA_DONE();
    mbedtls_test_driver_key_agreement_hooks =
        mbedtls_test_driver_key_agreement_hooks_init();
}


static void test_key_agreement_wrapper( void ** params )
{
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data9 = {(uint8_t *) params[9], *( (uint32_t *) params[10] )};

    test_key_agreement( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), &data3, &data5, &data7, &data9, *( (int *) params[11] ) );
}
#line 1026 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_cipher_encrypt_validation(int alg_arg,
                               int key_type_arg,
                               data_t *key_data,
                               data_t *input)
{
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_type_t key_type = key_type_arg;
    psa_algorithm_t alg = alg_arg;
    size_t iv_size = PSA_CIPHER_IV_LENGTH(key_type, alg);
    unsigned char *output1 = NULL;
    size_t output1_buffer_size = 0;
    size_t output1_length = 0;
    unsigned char *output2 = NULL;
    size_t output2_buffer_size = 0;
    size_t output2_length = 0;
    size_t function_output_length = 0;
    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    mbedtls_test_driver_cipher_hooks = mbedtls_test_driver_cipher_hooks_init();

    PSA_ASSERT(psa_crypto_init());

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, key_type);

    output1_buffer_size = PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(key_type, alg, input->len);
    output2_buffer_size = PSA_CIPHER_UPDATE_OUTPUT_SIZE(key_type, alg, input->len) +
                          PSA_CIPHER_FINISH_OUTPUT_SIZE(key_type, alg);
    ASSERT_ALLOC(output1, output1_buffer_size);
    ASSERT_ALLOC(output2, output2_buffer_size);

    PSA_ASSERT(psa_import_key(&attributes, key_data->x, key_data->len,
                              &key));

    PSA_ASSERT(psa_cipher_encrypt(key, alg, input->x, input->len, output1,
                                  output1_buffer_size, &output1_length));
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 1);
    mbedtls_test_driver_cipher_hooks.hits = 0;

    PSA_ASSERT(psa_cipher_encrypt_setup(&operation, key, alg));
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 1);
    mbedtls_test_driver_cipher_hooks.hits = 0;

    PSA_ASSERT(psa_cipher_set_iv(&operation, output1, iv_size));
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 1);
    mbedtls_test_driver_cipher_hooks.hits = 0;

    PSA_ASSERT(psa_cipher_update(&operation,
                                 input->x, input->len,
                                 output2, output2_buffer_size,
                                 &function_output_length));
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 1);
    mbedtls_test_driver_cipher_hooks.hits = 0;

    output2_length += function_output_length;
    PSA_ASSERT(psa_cipher_finish(&operation,
                                 output2 + output2_length,
                                 output2_buffer_size - output2_length,
                                 &function_output_length));
    /* Finish will have called abort as well, so expecting two hits here */
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 2);
    mbedtls_test_driver_cipher_hooks.hits = 0;

    output2_length += function_output_length;

    PSA_ASSERT(psa_cipher_abort(&operation));
    // driver function should've been called as part of the finish() core routine
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 0);
    ASSERT_COMPARE(output1 + iv_size, output1_length - iv_size,
                   output2, output2_length);

exit:
    psa_cipher_abort(&operation);
    mbedtls_free(output1);
    mbedtls_free(output2);
    psa_destroy_key(key);
    PSA_DONE();
    mbedtls_test_driver_cipher_hooks = mbedtls_test_driver_cipher_hooks_init();
}

static void test_cipher_encrypt_validation_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};

    test_cipher_encrypt_validation( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4 );
}
#line 1109 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_cipher_encrypt_multipart(int alg_arg,
                              int key_type_arg,
                              data_t *key_data,
                              data_t *iv,
                              data_t *input,
                              int first_part_size_arg,
                              int output1_length_arg,
                              int output2_length_arg,
                              data_t *expected_output,
                              int mock_output_arg,
                              int force_status_arg,
                              int expected_status_arg)
{
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_type_t key_type = key_type_arg;
    psa_algorithm_t alg = alg_arg;
    psa_status_t status;
    psa_status_t expected_status = expected_status_arg;
    psa_status_t force_status = force_status_arg;
    size_t first_part_size = first_part_size_arg;
    size_t output1_length = output1_length_arg;
    size_t output2_length = output2_length_arg;
    unsigned char *output = NULL;
    size_t output_buffer_size = 0;
    size_t function_output_length = 0;
    size_t total_output_length = 0;
    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    mbedtls_test_driver_cipher_hooks = mbedtls_test_driver_cipher_hooks_init();
    mbedtls_test_driver_cipher_hooks.forced_status = force_status;

    /* Test operation initialization */
    mbedtls_psa_cipher_operation_t mbedtls_operation =
        MBEDTLS_PSA_CIPHER_OPERATION_INIT;

    mbedtls_transparent_test_driver_cipher_operation_t transparent_operation =
        MBEDTLS_TRANSPARENT_TEST_DRIVER_CIPHER_OPERATION_INIT;

    mbedtls_opaque_test_driver_cipher_operation_t opaque_operation =
        MBEDTLS_OPAQUE_TEST_DRIVER_CIPHER_OPERATION_INIT;

    operation.ctx.mbedtls_ctx = mbedtls_operation;
    operation.ctx.transparent_test_driver_ctx = transparent_operation;
    operation.ctx.opaque_test_driver_ctx = opaque_operation;

    PSA_ASSERT(psa_crypto_init());

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, key_type);

    PSA_ASSERT(psa_import_key(&attributes, key_data->x, key_data->len,
                              &key));

    PSA_ASSERT(psa_cipher_encrypt_setup(&operation, key, alg));
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 1);
    mbedtls_test_driver_cipher_hooks.hits = 0;

    PSA_ASSERT(psa_cipher_set_iv(&operation, iv->x, iv->len));
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, (force_status == PSA_SUCCESS ? 1 : 0));
    mbedtls_test_driver_cipher_hooks.hits = 0;

    output_buffer_size = ((size_t) input->len +
                          PSA_BLOCK_CIPHER_BLOCK_LENGTH(key_type));
    ASSERT_ALLOC(output, output_buffer_size);

    if (mock_output_arg) {
        mbedtls_test_driver_cipher_hooks.forced_output = expected_output->x;
        mbedtls_test_driver_cipher_hooks.forced_output_length = expected_output->len;
    }

    TEST_ASSERT(first_part_size <= input->len);
    PSA_ASSERT(psa_cipher_update(&operation, input->x, first_part_size,
                                 output, output_buffer_size,
                                 &function_output_length));
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, (force_status == PSA_SUCCESS ? 1 : 0));
    mbedtls_test_driver_cipher_hooks.hits = 0;

    TEST_ASSERT(function_output_length == output1_length);
    total_output_length += function_output_length;

    if (first_part_size < input->len) {
        PSA_ASSERT(psa_cipher_update(&operation,
                                     input->x + first_part_size,
                                     input->len - first_part_size,
                                     output + total_output_length,
                                     output_buffer_size - total_output_length,
                                     &function_output_length));
        TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 1);
        mbedtls_test_driver_cipher_hooks.hits = 0;

        TEST_ASSERT(function_output_length == output2_length);
        total_output_length += function_output_length;
    }

    if (mock_output_arg) {
        mbedtls_test_driver_cipher_hooks.forced_output = NULL;
        mbedtls_test_driver_cipher_hooks.forced_output_length = 0;
    }

    status =  psa_cipher_finish(&operation,
                                output + total_output_length,
                                output_buffer_size - total_output_length,
                                &function_output_length);
    /* Finish will have called abort as well, so expecting two hits here */
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, (force_status == PSA_SUCCESS ? 2 : 0));
    mbedtls_test_driver_cipher_hooks.hits = 0;
    total_output_length += function_output_length;
    TEST_EQUAL(status, expected_status);

    if (expected_status == PSA_SUCCESS) {
        PSA_ASSERT(psa_cipher_abort(&operation));
        TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 0);

        ASSERT_COMPARE(expected_output->x, expected_output->len,
                       output, total_output_length);
    }

exit:
    psa_cipher_abort(&operation);
    mbedtls_free(output);
    psa_destroy_key(key);
    PSA_DONE();
    mbedtls_test_driver_cipher_hooks = mbedtls_test_driver_cipher_hooks_init();
}

static void test_cipher_encrypt_multipart_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data11 = {(uint8_t *) params[11], *( (uint32_t *) params[12] )};

    test_cipher_encrypt_multipart( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6, *( (int *) params[8] ), *( (int *) params[9] ), *( (int *) params[10] ), &data11, *( (int *) params[13] ), *( (int *) params[14] ), *( (int *) params[15] ) );
}
#line 1237 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_cipher_decrypt_multipart(int alg_arg,
                              int key_type_arg,
                              data_t *key_data,
                              data_t *iv,
                              data_t *input,
                              int first_part_size_arg,
                              int output1_length_arg,
                              int output2_length_arg,
                              data_t *expected_output,
                              int mock_output_arg,
                              int force_status_arg,
                              int expected_status_arg)
{
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_type_t key_type = key_type_arg;
    psa_algorithm_t alg = alg_arg;
    psa_status_t status;
    psa_status_t expected_status = expected_status_arg;
    psa_status_t force_status = force_status_arg;
    size_t first_part_size = first_part_size_arg;
    size_t output1_length = output1_length_arg;
    size_t output2_length = output2_length_arg;
    unsigned char *output = NULL;
    size_t output_buffer_size = 0;
    size_t function_output_length = 0;
    size_t total_output_length = 0;
    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    mbedtls_test_driver_cipher_hooks = mbedtls_test_driver_cipher_hooks_init();
    mbedtls_test_driver_cipher_hooks.forced_status = force_status;

    /* Test operation initialization */
    mbedtls_psa_cipher_operation_t mbedtls_operation =
        MBEDTLS_PSA_CIPHER_OPERATION_INIT;

    mbedtls_transparent_test_driver_cipher_operation_t transparent_operation =
        MBEDTLS_TRANSPARENT_TEST_DRIVER_CIPHER_OPERATION_INIT;

    mbedtls_opaque_test_driver_cipher_operation_t opaque_operation =
        MBEDTLS_OPAQUE_TEST_DRIVER_CIPHER_OPERATION_INIT;

    operation.ctx.mbedtls_ctx = mbedtls_operation;
    operation.ctx.transparent_test_driver_ctx = transparent_operation;
    operation.ctx.opaque_test_driver_ctx = opaque_operation;

    PSA_ASSERT(psa_crypto_init());

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, key_type);

    PSA_ASSERT(psa_import_key(&attributes, key_data->x, key_data->len,
                              &key));

    PSA_ASSERT(psa_cipher_decrypt_setup(&operation, key, alg));
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 1);
    mbedtls_test_driver_cipher_hooks.hits = 0;

    PSA_ASSERT(psa_cipher_set_iv(&operation, iv->x, iv->len));
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, (force_status == PSA_SUCCESS ? 1 : 0));
    mbedtls_test_driver_cipher_hooks.hits = 0;

    output_buffer_size = ((size_t) input->len +
                          PSA_BLOCK_CIPHER_BLOCK_LENGTH(key_type));
    ASSERT_ALLOC(output, output_buffer_size);

    if (mock_output_arg) {
        mbedtls_test_driver_cipher_hooks.forced_output = expected_output->x;
        mbedtls_test_driver_cipher_hooks.forced_output_length = expected_output->len;
    }

    TEST_ASSERT(first_part_size <= input->len);
    PSA_ASSERT(psa_cipher_update(&operation,
                                 input->x, first_part_size,
                                 output, output_buffer_size,
                                 &function_output_length));
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, (force_status == PSA_SUCCESS ? 1 : 0));
    mbedtls_test_driver_cipher_hooks.hits = 0;

    TEST_ASSERT(function_output_length == output1_length);
    total_output_length += function_output_length;

    if (first_part_size < input->len) {
        PSA_ASSERT(psa_cipher_update(&operation,
                                     input->x + first_part_size,
                                     input->len - first_part_size,
                                     output + total_output_length,
                                     output_buffer_size - total_output_length,
                                     &function_output_length));
        TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, (force_status == PSA_SUCCESS ? 1 : 0));
        mbedtls_test_driver_cipher_hooks.hits = 0;

        TEST_ASSERT(function_output_length == output2_length);
        total_output_length += function_output_length;
    }

    if (mock_output_arg) {
        mbedtls_test_driver_cipher_hooks.forced_output = NULL;
        mbedtls_test_driver_cipher_hooks.forced_output_length = 0;
    }

    status = psa_cipher_finish(&operation,
                               output + total_output_length,
                               output_buffer_size - total_output_length,
                               &function_output_length);
    /* Finish will have called abort as well, so expecting two hits here */
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, (force_status == PSA_SUCCESS ? 2 : 0));
    mbedtls_test_driver_cipher_hooks.hits = 0;
    total_output_length += function_output_length;
    TEST_EQUAL(status, expected_status);

    if (expected_status == PSA_SUCCESS) {
        PSA_ASSERT(psa_cipher_abort(&operation));
        TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 0);

        ASSERT_COMPARE(expected_output->x, expected_output->len,
                       output, total_output_length);
    }

exit:
    psa_cipher_abort(&operation);
    mbedtls_free(output);
    psa_destroy_key(key);
    PSA_DONE();
    mbedtls_test_driver_cipher_hooks = mbedtls_test_driver_cipher_hooks_init();
}

static void test_cipher_decrypt_multipart_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data11 = {(uint8_t *) params[11], *( (uint32_t *) params[12] )};

    test_cipher_decrypt_multipart( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6, *( (int *) params[8] ), *( (int *) params[9] ), *( (int *) params[10] ), &data11, *( (int *) params[13] ), *( (int *) params[14] ), *( (int *) params[15] ) );
}
#line 1366 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_cipher_decrypt(int alg_arg,
                    int key_type_arg,
                    data_t *key_data,
                    data_t *iv,
                    data_t *input_arg,
                    data_t *expected_output,
                    int mock_output_arg,
                    int force_status_arg,
                    int expected_status_arg)
{
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_status_t status;
    psa_key_type_t key_type = key_type_arg;
    psa_algorithm_t alg = alg_arg;
    psa_status_t expected_status = expected_status_arg;
    psa_status_t force_status = force_status_arg;
    unsigned char *input = NULL;
    size_t input_buffer_size = 0;
    unsigned char *output = NULL;
    size_t output_buffer_size = 0;
    size_t output_length = 0;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    mbedtls_test_driver_cipher_hooks = mbedtls_test_driver_cipher_hooks_init();
    mbedtls_test_driver_cipher_hooks.forced_status = force_status;

    PSA_ASSERT(psa_crypto_init());

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, key_type);

    /* Allocate input buffer and copy the iv and the plaintext */
    input_buffer_size = ((size_t) input_arg->len + (size_t) iv->len);
    if (input_buffer_size > 0) {
        ASSERT_ALLOC(input, input_buffer_size);
        memcpy(input, iv->x, iv->len);
        memcpy(input + iv->len, input_arg->x, input_arg->len);
    }

    output_buffer_size = PSA_CIPHER_DECRYPT_OUTPUT_SIZE(key_type, alg, input_buffer_size);
    ASSERT_ALLOC(output, output_buffer_size);

    PSA_ASSERT(psa_import_key(&attributes, key_data->x, key_data->len,
                              &key));

    if (mock_output_arg) {
        mbedtls_test_driver_cipher_hooks.forced_output = expected_output->x;
        mbedtls_test_driver_cipher_hooks.forced_output_length = expected_output->len;
    }

    status = psa_cipher_decrypt(key, alg, input, input_buffer_size, output,
                                output_buffer_size, &output_length);
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 1);
    mbedtls_test_driver_cipher_hooks.hits = 0;

    TEST_EQUAL(status, expected_status);

    if (expected_status == PSA_SUCCESS) {
        ASSERT_COMPARE(expected_output->x, expected_output->len,
                       output, output_length);
    }

exit:
    mbedtls_free(input);
    mbedtls_free(output);
    psa_destroy_key(key);
    PSA_DONE();
    mbedtls_test_driver_cipher_hooks = mbedtls_test_driver_cipher_hooks_init();
}

static void test_cipher_decrypt_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_cipher_decrypt( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6, &data8, *( (int *) params[10] ), *( (int *) params[11] ), *( (int *) params[12] ) );
}
#line 1438 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_cipher_entry_points(int alg_arg, int key_type_arg,
                         data_t *key_data, data_t *iv,
                         data_t *input)
{
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_status_t status;
    psa_key_type_t key_type = key_type_arg;
    psa_algorithm_t alg = alg_arg;
    unsigned char *output = NULL;
    size_t output_buffer_size = 0;
    size_t function_output_length = 0;
    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    mbedtls_test_driver_cipher_hooks = mbedtls_test_driver_cipher_hooks_init();

    ASSERT_ALLOC(output, input->len + 16);
    output_buffer_size = input->len + 16;

    PSA_ASSERT(psa_crypto_init());

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, key_type);

    PSA_ASSERT(psa_import_key(&attributes, key_data->x, key_data->len,
                              &key));

    /*
     * Test encrypt failure
     * First test that if we don't force a driver error, encryption is
     * successful, then force driver error.
     */
    status = psa_cipher_encrypt(
        key, alg, input->x, input->len,
        output, output_buffer_size, &function_output_length);
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 1);
    TEST_EQUAL(status, PSA_SUCCESS);
    mbedtls_test_driver_cipher_hooks.hits = 0;

    mbedtls_test_driver_cipher_hooks.forced_status = PSA_ERROR_GENERIC_ERROR;
    /* Set the output buffer in a given state. */
    for (size_t i = 0; i < output_buffer_size; i++) {
        output[i] = 0xa5;
    }

    status = psa_cipher_encrypt(
        key, alg, input->x, input->len,
        output, output_buffer_size, &function_output_length);
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 1);
    TEST_EQUAL(status, PSA_ERROR_GENERIC_ERROR);
    /*
     * Check that the output buffer is still in the same state.
     * This will fail if the output buffer is used by the core to pass the IV
     * it generated to the driver (and is not restored).
     */
    for (size_t i = 0; i < output_buffer_size; i++) {
        TEST_EQUAL(output[i], 0xa5);
    }
    mbedtls_test_driver_cipher_hooks.hits = 0;

    /* Test setup call, encrypt */
    mbedtls_test_driver_cipher_hooks.forced_status = PSA_ERROR_GENERIC_ERROR;
    status = psa_cipher_encrypt_setup(&operation, key, alg);
    /* When setup fails, it shouldn't call any further entry points */
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 1);
    TEST_EQUAL(status, mbedtls_test_driver_cipher_hooks.forced_status);
    mbedtls_test_driver_cipher_hooks.hits = 0;
    status = psa_cipher_set_iv(&operation, iv->x, iv->len);
    TEST_EQUAL(status, PSA_ERROR_BAD_STATE);
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 0);

    /* Test setup call failure, decrypt */
    status = psa_cipher_decrypt_setup(&operation, key, alg);
    /* When setup fails, it shouldn't call any further entry points */
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 1);
    TEST_EQUAL(status, mbedtls_test_driver_cipher_hooks.forced_status);
    mbedtls_test_driver_cipher_hooks.hits = 0;
    status = psa_cipher_set_iv(&operation, iv->x, iv->len);
    TEST_EQUAL(status, PSA_ERROR_BAD_STATE);
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 0);

    /* Test IV setting failure */
    mbedtls_test_driver_cipher_hooks.forced_status = PSA_SUCCESS;
    status = psa_cipher_encrypt_setup(&operation, key, alg);
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 1);
    TEST_EQUAL(status, mbedtls_test_driver_cipher_hooks.forced_status);
    mbedtls_test_driver_cipher_hooks.hits = 0;

    mbedtls_test_driver_cipher_hooks.forced_status = PSA_ERROR_GENERIC_ERROR;
    status = psa_cipher_set_iv(&operation, iv->x, iv->len);
    /* When setting the IV fails, it should call abort too */
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 2);
    TEST_EQUAL(status, mbedtls_test_driver_cipher_hooks.forced_status);
    /* Failure should prevent further operations from executing on the driver */
    mbedtls_test_driver_cipher_hooks.hits = 0;
    status = psa_cipher_update(&operation,
                               input->x, input->len,
                               output, output_buffer_size,
                               &function_output_length);
    TEST_EQUAL(status, PSA_ERROR_BAD_STATE);
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 0);
    psa_cipher_abort(&operation);

    /* Test IV generation failure */
    mbedtls_test_driver_cipher_hooks.forced_status = PSA_SUCCESS;
    status = psa_cipher_encrypt_setup(&operation, key, alg);
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 1);
    TEST_EQUAL(status, mbedtls_test_driver_cipher_hooks.forced_status);
    mbedtls_test_driver_cipher_hooks.hits = 0;

    mbedtls_test_driver_cipher_hooks.forced_status = PSA_ERROR_GENERIC_ERROR;
    /* Set the output buffer in a given state. */
    for (size_t i = 0; i < 16; i++) {
        output[i] = 0xa5;
    }

    status = psa_cipher_generate_iv(&operation, output, 16, &function_output_length);
    /* When generating the IV fails, it should call abort too */
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 2);
    TEST_EQUAL(status, mbedtls_test_driver_cipher_hooks.forced_status);
    /*
     * Check that the output buffer is still in the same state.
     * This will fail if the output buffer is used by the core to pass the IV
     * it generated to the driver (and is not restored).
     */
    for (size_t i = 0; i < 16; i++) {
        TEST_EQUAL(output[i], 0xa5);
    }
    /* Failure should prevent further operations from executing on the driver */
    mbedtls_test_driver_cipher_hooks.hits = 0;
    status = psa_cipher_update(&operation,
                               input->x, input->len,
                               output, output_buffer_size,
                               &function_output_length);
    TEST_EQUAL(status, PSA_ERROR_BAD_STATE);
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 0);
    psa_cipher_abort(&operation);

    /* Test update failure */
    mbedtls_test_driver_cipher_hooks.forced_status = PSA_SUCCESS;
    status = psa_cipher_encrypt_setup(&operation, key, alg);
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 1);
    TEST_EQUAL(status, mbedtls_test_driver_cipher_hooks.forced_status);
    mbedtls_test_driver_cipher_hooks.hits = 0;

    status = psa_cipher_set_iv(&operation, iv->x, iv->len);
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 1);
    TEST_EQUAL(status, mbedtls_test_driver_cipher_hooks.forced_status);
    mbedtls_test_driver_cipher_hooks.hits = 0;

    mbedtls_test_driver_cipher_hooks.forced_status = PSA_ERROR_GENERIC_ERROR;
    status = psa_cipher_update(&operation,
                               input->x, input->len,
                               output, output_buffer_size,
                               &function_output_length);
    /* When the update call fails, it should call abort too */
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 2);
    TEST_EQUAL(status, mbedtls_test_driver_cipher_hooks.forced_status);
    /* Failure should prevent further operations from executing on the driver */
    mbedtls_test_driver_cipher_hooks.hits = 0;
    status = psa_cipher_update(&operation,
                               input->x, input->len,
                               output, output_buffer_size,
                               &function_output_length);
    TEST_EQUAL(status, PSA_ERROR_BAD_STATE);
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 0);
    psa_cipher_abort(&operation);

    /* Test finish failure */
    mbedtls_test_driver_cipher_hooks.forced_status = PSA_SUCCESS;
    status = psa_cipher_encrypt_setup(&operation, key, alg);
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 1);
    TEST_EQUAL(status, mbedtls_test_driver_cipher_hooks.forced_status);
    mbedtls_test_driver_cipher_hooks.hits = 0;

    status = psa_cipher_set_iv(&operation, iv->x, iv->len);
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 1);
    TEST_EQUAL(status, mbedtls_test_driver_cipher_hooks.forced_status);
    mbedtls_test_driver_cipher_hooks.hits = 0;

    status = psa_cipher_update(&operation,
                               input->x, input->len,
                               output, output_buffer_size,
                               &function_output_length);
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 1);
    TEST_EQUAL(status, mbedtls_test_driver_cipher_hooks.forced_status);
    mbedtls_test_driver_cipher_hooks.hits = 0;

    mbedtls_test_driver_cipher_hooks.forced_status = PSA_ERROR_GENERIC_ERROR;
    status = psa_cipher_finish(&operation,
                               output + function_output_length,
                               output_buffer_size - function_output_length,
                               &function_output_length);
    /* When the finish call fails, it should call abort too */
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 2);
    TEST_EQUAL(status, mbedtls_test_driver_cipher_hooks.forced_status);
    /* Failure should prevent further operations from executing on the driver */
    mbedtls_test_driver_cipher_hooks.hits = 0;
    status = psa_cipher_update(&operation,
                               input->x, input->len,
                               output, output_buffer_size,
                               &function_output_length);
    TEST_EQUAL(status, PSA_ERROR_BAD_STATE);
    TEST_EQUAL(mbedtls_test_driver_cipher_hooks.hits, 0);
    psa_cipher_abort(&operation);

exit:
    psa_cipher_abort(&operation);
    mbedtls_free(output);
    psa_destroy_key(key);
    PSA_DONE();
    mbedtls_test_driver_cipher_hooks = mbedtls_test_driver_cipher_hooks_init();
}

static void test_cipher_entry_points_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_cipher_entry_points( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6 );
}
#line 1654 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_aead_encrypt(int key_type_arg, data_t *key_data,
                  int alg_arg,
                  data_t *nonce,
                  data_t *additional_data,
                  data_t *input_data,
                  data_t *expected_result,
                  int forced_status_arg)
{
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_type_t key_type = key_type_arg;
    psa_algorithm_t alg = alg_arg;
    size_t key_bits;
    psa_status_t forced_status = forced_status_arg;
    unsigned char *output_data = NULL;
    size_t output_size = 0;
    size_t output_length = 0;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;
    mbedtls_test_driver_aead_hooks = mbedtls_test_driver_aead_hooks_init();

    PSA_ASSERT(psa_crypto_init());

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, key_type);

    PSA_ASSERT(psa_import_key(&attributes, key_data->x, key_data->len,
                              &key));
    PSA_ASSERT(psa_get_key_attributes(key, &attributes));
    key_bits = psa_get_key_bits(&attributes);

    output_size = input_data->len + PSA_AEAD_TAG_LENGTH(key_type, key_bits,
                                                        alg);
    /* For all currently defined algorithms, PSA_AEAD_ENCRYPT_OUTPUT_SIZE
     * should be exact. */
    TEST_EQUAL(output_size,
               PSA_AEAD_ENCRYPT_OUTPUT_SIZE(key_type, alg, input_data->len));
    TEST_ASSERT(output_size <=
                PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE(input_data->len));
    ASSERT_ALLOC(output_data, output_size);

    mbedtls_test_driver_aead_hooks.forced_status = forced_status;
    status = psa_aead_encrypt(key, alg,
                              nonce->x, nonce->len,
                              additional_data->x, additional_data->len,
                              input_data->x, input_data->len,
                              output_data, output_size,
                              &output_length);
    TEST_EQUAL(mbedtls_test_driver_aead_hooks.hits_encrypt, 1);
    TEST_EQUAL(mbedtls_test_driver_aead_hooks.driver_status, forced_status);

    TEST_EQUAL(status, (forced_status == PSA_ERROR_NOT_SUPPORTED) ?
               PSA_SUCCESS : forced_status);

    if (status == PSA_SUCCESS) {
        ASSERT_COMPARE(expected_result->x, expected_result->len,
                       output_data, output_length);
    }

exit:
    psa_destroy_key(key);
    mbedtls_free(output_data);
    PSA_DONE();
    mbedtls_test_driver_aead_hooks = mbedtls_test_driver_aead_hooks_init();
}

static void test_aead_encrypt_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};

    test_aead_encrypt( *( (int *) params[0] ), &data1, *( (int *) params[3] ), &data4, &data6, &data8, &data10, *( (int *) params[12] ) );
}
#line 1722 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_aead_decrypt(int key_type_arg, data_t *key_data,
                  int alg_arg,
                  data_t *nonce,
                  data_t *additional_data,
                  data_t *input_data,
                  data_t *expected_data,
                  int forced_status_arg)
{
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_type_t key_type = key_type_arg;
    psa_algorithm_t alg = alg_arg;
    size_t key_bits;
    psa_status_t forced_status = forced_status_arg;
    unsigned char *output_data = NULL;
    size_t output_size = 0;
    size_t output_length = 0;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;
    mbedtls_test_driver_aead_hooks = mbedtls_test_driver_aead_hooks_init();

    PSA_ASSERT(psa_crypto_init());

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, key_type);

    PSA_ASSERT(psa_import_key(&attributes, key_data->x, key_data->len,
                              &key));
    PSA_ASSERT(psa_get_key_attributes(key, &attributes));
    key_bits = psa_get_key_bits(&attributes);

    output_size = input_data->len - PSA_AEAD_TAG_LENGTH(key_type, key_bits,
                                                        alg);
    ASSERT_ALLOC(output_data, output_size);

    mbedtls_test_driver_aead_hooks.forced_status = forced_status;
    status = psa_aead_decrypt(key, alg,
                              nonce->x, nonce->len,
                              additional_data->x,
                              additional_data->len,
                              input_data->x, input_data->len,
                              output_data, output_size,
                              &output_length);
    TEST_EQUAL(mbedtls_test_driver_aead_hooks.hits_decrypt, 1);
    TEST_EQUAL(mbedtls_test_driver_aead_hooks.driver_status, forced_status);

    TEST_EQUAL(status, (forced_status == PSA_ERROR_NOT_SUPPORTED) ?
               PSA_SUCCESS : forced_status);

    if (status == PSA_SUCCESS) {
        ASSERT_COMPARE(expected_data->x, expected_data->len,
                       output_data, output_length);
    }

exit:
    psa_destroy_key(key);
    mbedtls_free(output_data);
    PSA_DONE();
    mbedtls_test_driver_aead_hooks = mbedtls_test_driver_aead_hooks_init();
}

static void test_aead_decrypt_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};

    test_aead_decrypt( *( (int *) params[0] ), &data1, *( (int *) params[3] ), &data4, &data6, &data8, &data10, *( (int *) params[12] ) );
}
#line 1785 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_mac_sign(int key_type_arg,
              data_t *key_data,
              int alg_arg,
              data_t *input,
              data_t *expected_mac,
              int forced_status_arg)
{
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_type_t key_type = key_type_arg;
    psa_algorithm_t alg = alg_arg;
    psa_mac_operation_t operation = PSA_MAC_OPERATION_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t *actual_mac = NULL;
    size_t mac_buffer_size =
        PSA_MAC_LENGTH(key_type, PSA_BYTES_TO_BITS(key_data->len), alg);
    size_t mac_length = 0;
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t forced_status = forced_status_arg;
    mbedtls_test_driver_mac_hooks = mbedtls_test_driver_mac_hooks_init();

    TEST_ASSERT(mac_buffer_size <= PSA_MAC_MAX_SIZE);
    /* We expect PSA_MAC_LENGTH to be exact. */
    TEST_ASSERT(expected_mac->len == mac_buffer_size);

    PSA_ASSERT(psa_crypto_init());

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, key_type);

    PSA_ASSERT(psa_import_key(&attributes, key_data->x, key_data->len,
                              &key));

    ASSERT_ALLOC(actual_mac, mac_buffer_size);
    mbedtls_test_driver_mac_hooks.forced_status = forced_status;

    /*
     * Calculate the MAC, one-shot case.
     */
    status = psa_mac_compute(key, alg,
                             input->x, input->len,
                             actual_mac, mac_buffer_size,
                             &mac_length);

    TEST_EQUAL(mbedtls_test_driver_mac_hooks.hits, 1);
    if (forced_status == PSA_SUCCESS ||
        forced_status == PSA_ERROR_NOT_SUPPORTED) {
        PSA_ASSERT(status);
    } else {
        TEST_EQUAL(forced_status, status);
    }

    PSA_ASSERT(psa_mac_abort(&operation));
    TEST_EQUAL(mbedtls_test_driver_mac_hooks.hits, 1);

    if (forced_status == PSA_SUCCESS) {
        ASSERT_COMPARE(expected_mac->x, expected_mac->len,
                       actual_mac, mac_length);
    }

    mbedtls_free(actual_mac);
    actual_mac = NULL;

exit:
    psa_mac_abort(&operation);
    psa_destroy_key(key);
    PSA_DONE();
    mbedtls_free(actual_mac);
    mbedtls_test_driver_mac_hooks = mbedtls_test_driver_mac_hooks_init();
}

static void test_mac_sign_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_mac_sign( *( (int *) params[0] ), &data1, *( (int *) params[3] ), &data4, &data6, *( (int *) params[8] ) );
}
#line 1858 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_mac_sign_multipart(int key_type_arg,
                        data_t *key_data,
                        int alg_arg,
                        data_t *input,
                        data_t *expected_mac,
                        int fragments_count,
                        int forced_status_arg)
{
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_type_t key_type = key_type_arg;
    psa_algorithm_t alg = alg_arg;
    psa_mac_operation_t operation = PSA_MAC_OPERATION_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t *actual_mac = NULL;
    size_t mac_buffer_size =
        PSA_MAC_LENGTH(key_type, PSA_BYTES_TO_BITS(key_data->len), alg);
    size_t mac_length = 0;
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t forced_status = forced_status_arg;
    uint8_t *input_x = input->x;
    mbedtls_test_driver_mac_hooks = mbedtls_test_driver_mac_hooks_init();

    TEST_ASSERT(mac_buffer_size <= PSA_MAC_MAX_SIZE);
    /* We expect PSA_MAC_LENGTH to be exact. */
    TEST_ASSERT(expected_mac->len == mac_buffer_size);

    PSA_ASSERT(psa_crypto_init());

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, key_type);

    PSA_ASSERT(psa_import_key(&attributes, key_data->x, key_data->len,
                              &key));

    ASSERT_ALLOC(actual_mac, mac_buffer_size);
    mbedtls_test_driver_mac_hooks.forced_status = forced_status;

    /*
     * Calculate the MAC, multipart case.
     */
    status = psa_mac_sign_setup(&operation, key, alg);
    TEST_EQUAL(mbedtls_test_driver_mac_hooks.hits, 1);

    if (forced_status == PSA_SUCCESS ||
        forced_status == PSA_ERROR_NOT_SUPPORTED) {
        PSA_ASSERT(status);
    } else {
        TEST_EQUAL(forced_status, status);
    }

    if (fragments_count) {
        TEST_ASSERT((input->len / fragments_count) > 0);
    }

    for (int i = 0; i < fragments_count; i++) {
        int fragment_size = input->len / fragments_count;
        if (i == fragments_count - 1) {
            fragment_size += (input->len % fragments_count);
        }

        status = psa_mac_update(&operation,
                                input_x, fragment_size);
        if (forced_status == PSA_SUCCESS) {
            TEST_EQUAL(mbedtls_test_driver_mac_hooks.hits, 2 + i);
        } else {
            TEST_EQUAL(mbedtls_test_driver_mac_hooks.hits, 1);
        }
        if (forced_status == PSA_SUCCESS ||
            forced_status == PSA_ERROR_NOT_SUPPORTED) {
            PSA_ASSERT(status);
        } else {
            TEST_EQUAL(PSA_ERROR_BAD_STATE, status);
        }
        input_x += fragment_size;
    }

    status = psa_mac_sign_finish(&operation,
                                 actual_mac, mac_buffer_size,
                                 &mac_length);
    if (forced_status == PSA_SUCCESS) {
        TEST_EQUAL(mbedtls_test_driver_mac_hooks.hits, 3 + fragments_count);
    } else {
        TEST_EQUAL(mbedtls_test_driver_mac_hooks.hits, 1);
    }

    if (forced_status == PSA_SUCCESS ||
        forced_status == PSA_ERROR_NOT_SUPPORTED) {
        PSA_ASSERT(status);
    } else {
        TEST_EQUAL(PSA_ERROR_BAD_STATE, status);
    }

    PSA_ASSERT(psa_mac_abort(&operation));
    if (forced_status == PSA_SUCCESS) {
        TEST_EQUAL(mbedtls_test_driver_mac_hooks.hits, 3 + fragments_count);
    } else {
        TEST_EQUAL(mbedtls_test_driver_mac_hooks.hits, 1);
    }

    if (forced_status == PSA_SUCCESS) {
        ASSERT_COMPARE(expected_mac->x, expected_mac->len,
                       actual_mac, mac_length);
    }

    mbedtls_free(actual_mac);
    actual_mac = NULL;

exit:
    psa_mac_abort(&operation);
    psa_destroy_key(key);
    PSA_DONE();
    mbedtls_free(actual_mac);
    mbedtls_test_driver_mac_hooks = mbedtls_test_driver_mac_hooks_init();
}

static void test_mac_sign_multipart_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_mac_sign_multipart( *( (int *) params[0] ), &data1, *( (int *) params[3] ), &data4, &data6, *( (int *) params[8] ), *( (int *) params[9] ) );
}
#line 1976 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_mac_verify(int key_type_arg,
                data_t *key_data,
                int alg_arg,
                data_t *input,
                data_t *expected_mac,
                int forced_status_arg)
{
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_type_t key_type = key_type_arg;
    psa_algorithm_t alg = alg_arg;
    psa_mac_operation_t operation = PSA_MAC_OPERATION_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;
    psa_status_t forced_status = forced_status_arg;
    mbedtls_test_driver_mac_hooks = mbedtls_test_driver_mac_hooks_init();

    TEST_ASSERT(expected_mac->len <= PSA_MAC_MAX_SIZE);

    PSA_ASSERT(psa_crypto_init());

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, key_type);

    PSA_ASSERT(psa_import_key(&attributes, key_data->x, key_data->len,
                              &key));

    mbedtls_test_driver_mac_hooks.forced_status = forced_status;

    /*
     * Verify the MAC, one-shot case.
     */
    status = psa_mac_verify(key, alg,
                            input->x, input->len,
                            expected_mac->x, expected_mac->len);
    TEST_EQUAL(mbedtls_test_driver_mac_hooks.hits, 1);
    if (forced_status == PSA_SUCCESS ||
        forced_status == PSA_ERROR_NOT_SUPPORTED) {
        PSA_ASSERT(status);
    } else {
        TEST_EQUAL(forced_status, status);
    }

    PSA_ASSERT(psa_mac_abort(&operation));
    TEST_EQUAL(mbedtls_test_driver_mac_hooks.hits, 1);
exit:
    psa_mac_abort(&operation);
    psa_destroy_key(key);
    PSA_DONE();
    mbedtls_test_driver_mac_hooks = mbedtls_test_driver_mac_hooks_init();
}

static void test_mac_verify_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_mac_verify( *( (int *) params[0] ), &data1, *( (int *) params[3] ), &data4, &data6, *( (int *) params[8] ) );
}
#line 2030 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_mac_verify_multipart(int key_type_arg,
                          data_t *key_data,
                          int alg_arg,
                          data_t *input,
                          data_t *expected_mac,
                          int fragments_count,
                          int forced_status_arg)
{
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_type_t key_type = key_type_arg;
    psa_algorithm_t alg = alg_arg;
    psa_mac_operation_t operation = PSA_MAC_OPERATION_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;
    psa_status_t forced_status = forced_status_arg;
    uint8_t *input_x = input->x;
    mbedtls_test_driver_mac_hooks = mbedtls_test_driver_mac_hooks_init();

    TEST_ASSERT(expected_mac->len <= PSA_MAC_MAX_SIZE);

    PSA_ASSERT(psa_crypto_init());

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, key_type);

    PSA_ASSERT(psa_import_key(&attributes, key_data->x, key_data->len,
                              &key));

    mbedtls_test_driver_mac_hooks.forced_status = forced_status;

    /*
     * Verify the MAC, multi-part case.
     */
    status = psa_mac_verify_setup(&operation, key, alg);
    TEST_EQUAL(mbedtls_test_driver_mac_hooks.hits, 1);

    if (forced_status == PSA_SUCCESS ||
        forced_status == PSA_ERROR_NOT_SUPPORTED) {
        PSA_ASSERT(status);
    } else {
        TEST_EQUAL(forced_status, status);
    }

    if (fragments_count) {
        TEST_ASSERT((input->len / fragments_count) > 0);
    }

    for (int i = 0; i < fragments_count; i++) {
        int fragment_size = input->len / fragments_count;
        if (i == fragments_count - 1) {
            fragment_size += (input->len % fragments_count);
        }

        status = psa_mac_update(&operation,
                                input_x, fragment_size);
        if (forced_status == PSA_SUCCESS) {
            TEST_EQUAL(mbedtls_test_driver_mac_hooks.hits, 2 + i);
        } else {
            TEST_EQUAL(mbedtls_test_driver_mac_hooks.hits, 1);
        }

        if (forced_status == PSA_SUCCESS ||
            forced_status == PSA_ERROR_NOT_SUPPORTED) {
            PSA_ASSERT(status);
        } else {
            TEST_EQUAL(PSA_ERROR_BAD_STATE, status);
        }
        input_x += fragment_size;
    }

    status = psa_mac_verify_finish(&operation,
                                   expected_mac->x,
                                   expected_mac->len);
    if (forced_status == PSA_SUCCESS) {
        TEST_EQUAL(mbedtls_test_driver_mac_hooks.hits, 3 + fragments_count);
    } else {
        TEST_EQUAL(mbedtls_test_driver_mac_hooks.hits, 1);
    }

    if (forced_status == PSA_SUCCESS ||
        forced_status == PSA_ERROR_NOT_SUPPORTED) {
        PSA_ASSERT(status);
    } else {
        TEST_EQUAL(PSA_ERROR_BAD_STATE, status);
    }


    PSA_ASSERT(psa_mac_abort(&operation));
    if (forced_status == PSA_SUCCESS) {
        TEST_EQUAL(mbedtls_test_driver_mac_hooks.hits, 3 + fragments_count);
    } else {
        TEST_EQUAL(mbedtls_test_driver_mac_hooks.hits, 1);
    }

exit:
    psa_mac_abort(&operation);
    psa_destroy_key(key);
    PSA_DONE();
    mbedtls_test_driver_mac_hooks = mbedtls_test_driver_mac_hooks_init();
}

static void test_mac_verify_multipart_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_mac_verify_multipart( *( (int *) params[0] ), &data1, *( (int *) params[3] ), &data4, &data6, *( (int *) params[8] ), *( (int *) params[9] ) );
}
#if defined(PSA_CRYPTO_DRIVER_TEST)
#if defined(MBEDTLS_PSA_CRYPTO_DRIVERS)
#if defined(MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS)
#line 2134 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_builtin_key_export(int builtin_key_id_arg,
                        int builtin_key_type_arg,
                        int builtin_key_bits_arg,
                        int builtin_key_algorithm_arg,
                        data_t *expected_output,
                        int expected_status_arg)
{
    psa_key_id_t builtin_key_id = (psa_key_id_t) builtin_key_id_arg;
    psa_key_type_t builtin_key_type = (psa_key_type_t) builtin_key_type_arg;
    psa_algorithm_t builtin_key_alg = (psa_algorithm_t) builtin_key_algorithm_arg;
    size_t builtin_key_bits = (size_t) builtin_key_bits_arg;
    psa_status_t expected_status = expected_status_arg;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    mbedtls_svc_key_id_t key = mbedtls_svc_key_id_make(0, builtin_key_id);
    uint8_t *output_buffer = NULL;
    size_t output_size = 0;
    psa_status_t actual_status;

    PSA_ASSERT(psa_crypto_init());
    ASSERT_ALLOC(output_buffer, expected_output->len);

    actual_status = psa_export_key(key, output_buffer, expected_output->len, &output_size);

    if (expected_status == PSA_SUCCESS) {
        PSA_ASSERT(actual_status);
        TEST_EQUAL(output_size, expected_output->len);
        ASSERT_COMPARE(output_buffer, output_size,
                       expected_output->x, expected_output->len);

        PSA_ASSERT(psa_get_key_attributes(key, &attributes));
        TEST_EQUAL(psa_get_key_bits(&attributes), builtin_key_bits);
        TEST_EQUAL(psa_get_key_type(&attributes), builtin_key_type);
        TEST_EQUAL(psa_get_key_algorithm(&attributes), builtin_key_alg);
    } else {
        if (actual_status != expected_status) {
            fprintf(stderr, "Expected %d but got %d\n", expected_status, actual_status);
        }
        TEST_EQUAL(actual_status, expected_status);
        TEST_EQUAL(output_size, 0);
    }

exit:
    mbedtls_free(output_buffer);
    psa_reset_key_attributes(&attributes);
    psa_destroy_key(key);
    PSA_DONE();
}

static void test_builtin_key_export_wrapper( void ** params )
{
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};

    test_builtin_key_export( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), &data4, *( (int *) params[6] ) );
}
#endif /* MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS */
#endif /* MBEDTLS_PSA_CRYPTO_DRIVERS */
#endif /* PSA_CRYPTO_DRIVER_TEST */
#if defined(PSA_CRYPTO_DRIVER_TEST)
#if defined(MBEDTLS_PSA_CRYPTO_DRIVERS)
#if defined(MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS)
#line 2185 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_builtin_pubkey_export(int builtin_key_id_arg,
                           int builtin_key_type_arg,
                           int builtin_key_bits_arg,
                           int builtin_key_algorithm_arg,
                           data_t *expected_output,
                           int expected_status_arg)
{
    psa_key_id_t builtin_key_id = (psa_key_id_t) builtin_key_id_arg;
    psa_key_type_t builtin_key_type = (psa_key_type_t) builtin_key_type_arg;
    psa_algorithm_t builtin_key_alg = (psa_algorithm_t) builtin_key_algorithm_arg;
    size_t builtin_key_bits = (size_t) builtin_key_bits_arg;
    psa_status_t expected_status = expected_status_arg;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    mbedtls_svc_key_id_t key = mbedtls_svc_key_id_make(0, builtin_key_id);
    uint8_t *output_buffer = NULL;
    size_t output_size = 0;
    psa_status_t actual_status;

    PSA_ASSERT(psa_crypto_init());
    ASSERT_ALLOC(output_buffer, expected_output->len);

    actual_status = psa_export_public_key(key, output_buffer, expected_output->len, &output_size);

    if (expected_status == PSA_SUCCESS) {
        PSA_ASSERT(actual_status);
        TEST_EQUAL(output_size, expected_output->len);
        ASSERT_COMPARE(output_buffer, output_size,
                       expected_output->x, expected_output->len);

        PSA_ASSERT(psa_get_key_attributes(key, &attributes));
        TEST_EQUAL(psa_get_key_bits(&attributes), builtin_key_bits);
        TEST_EQUAL(psa_get_key_type(&attributes), builtin_key_type);
        TEST_EQUAL(psa_get_key_algorithm(&attributes), builtin_key_alg);
    } else {
        TEST_EQUAL(actual_status, expected_status);
        TEST_EQUAL(output_size, 0);
    }

exit:
    mbedtls_free(output_buffer);
    psa_reset_key_attributes(&attributes);
    psa_destroy_key(key);
    PSA_DONE();
}

static void test_builtin_pubkey_export_wrapper( void ** params )
{
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};

    test_builtin_pubkey_export( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), &data4, *( (int *) params[6] ) );
}
#endif /* MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS */
#endif /* MBEDTLS_PSA_CRYPTO_DRIVERS */
#endif /* PSA_CRYPTO_DRIVER_TEST */
#line 2233 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_hash_compute(int alg_arg,
                  data_t *input, data_t *hash,
                  int forced_status_arg,
                  int expected_status_arg)
{
    psa_algorithm_t alg = alg_arg;
    psa_status_t forced_status = forced_status_arg;
    psa_status_t expected_status = expected_status_arg;
    unsigned char *output = NULL;
    size_t output_length;

    mbedtls_test_driver_hash_hooks = mbedtls_test_driver_hash_hooks_init();
    mbedtls_test_driver_hash_hooks.forced_status = forced_status;

    PSA_ASSERT(psa_crypto_init());
    ASSERT_ALLOC(output, PSA_HASH_LENGTH(alg));

    TEST_EQUAL(psa_hash_compute(alg, input->x, input->len,
                                output, PSA_HASH_LENGTH(alg),
                                &output_length), expected_status);
    TEST_EQUAL(mbedtls_test_driver_hash_hooks.hits, 1);
    TEST_EQUAL(mbedtls_test_driver_hash_hooks.driver_status, forced_status);

    if (expected_status == PSA_SUCCESS) {
        ASSERT_COMPARE(output, output_length, hash->x, hash->len);
    }

exit:
    mbedtls_free(output);
    PSA_DONE();
    mbedtls_test_driver_hash_hooks = mbedtls_test_driver_hash_hooks_init();
}

static void test_hash_compute_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};

    test_hash_compute( *( (int *) params[0] ), &data1, &data3, *( (int *) params[5] ), *( (int *) params[6] ) );
}
#line 2268 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_hash_multipart_setup(int alg_arg,
                          data_t *input, data_t *hash,
                          int forced_status_arg,
                          int expected_status_arg)
{
    psa_algorithm_t alg = alg_arg;
    psa_status_t forced_status = forced_status_arg;
    psa_status_t expected_status = expected_status_arg;
    unsigned char *output = NULL;
    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    size_t output_length;

    mbedtls_test_driver_hash_hooks = mbedtls_test_driver_hash_hooks_init();
    ASSERT_ALLOC(output, PSA_HASH_LENGTH(alg));

    PSA_ASSERT(psa_crypto_init());

    mbedtls_test_driver_hash_hooks.forced_status = forced_status;
    TEST_EQUAL(psa_hash_setup(&operation, alg), expected_status);
    TEST_EQUAL(mbedtls_test_driver_hash_hooks.hits, 1);
    TEST_EQUAL(mbedtls_test_driver_hash_hooks.driver_status, forced_status);

    if (expected_status == PSA_SUCCESS) {
        PSA_ASSERT(psa_hash_update(&operation, input->x, input->len));
        TEST_EQUAL(mbedtls_test_driver_hash_hooks.hits,
                   forced_status == PSA_ERROR_NOT_SUPPORTED ? 1 : 2);
        TEST_EQUAL(mbedtls_test_driver_hash_hooks.driver_status, forced_status);

        PSA_ASSERT(psa_hash_finish(&operation,
                                   output, PSA_HASH_LENGTH(alg),
                                   &output_length));
        TEST_EQUAL(mbedtls_test_driver_hash_hooks.hits,
                   forced_status == PSA_ERROR_NOT_SUPPORTED ? 1 : 4);
        TEST_EQUAL(mbedtls_test_driver_hash_hooks.driver_status, forced_status);

        ASSERT_COMPARE(output, output_length, hash->x, hash->len);
    }

exit:
    psa_hash_abort(&operation);
    mbedtls_free(output);
    PSA_DONE();
    mbedtls_test_driver_hash_hooks = mbedtls_test_driver_hash_hooks_init();
}

static void test_hash_multipart_setup_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};

    test_hash_multipart_setup( *( (int *) params[0] ), &data1, &data3, *( (int *) params[5] ), *( (int *) params[6] ) );
}
#line 2315 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_hash_multipart_update(int alg_arg,
                           data_t *input, data_t *hash,
                           int forced_status_arg)
{
    psa_algorithm_t alg = alg_arg;
    psa_status_t forced_status = forced_status_arg;
    unsigned char *output = NULL;
    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    size_t output_length;

    mbedtls_test_driver_hash_hooks = mbedtls_test_driver_hash_hooks_init();
    ASSERT_ALLOC(output, PSA_HASH_LENGTH(alg));

    PSA_ASSERT(psa_crypto_init());

    /*
     * Update inactive operation, the driver shouldn't be called.
     */
    TEST_EQUAL(psa_hash_update(&operation, input->x, input->len),
               PSA_ERROR_BAD_STATE);
    TEST_EQUAL(mbedtls_test_driver_hash_hooks.hits, 0);

    PSA_ASSERT(psa_hash_setup(&operation, alg));
    TEST_EQUAL(mbedtls_test_driver_hash_hooks.hits, 1);
    TEST_EQUAL(mbedtls_test_driver_hash_hooks.driver_status, PSA_SUCCESS);

    mbedtls_test_driver_hash_hooks.forced_status = forced_status;
    TEST_EQUAL(psa_hash_update(&operation, input->x, input->len),
               forced_status);
    /* One or two more calls to the driver interface: update or update + abort */
    TEST_EQUAL(mbedtls_test_driver_hash_hooks.hits,
               forced_status == PSA_SUCCESS ? 2 : 3);
    TEST_EQUAL(mbedtls_test_driver_hash_hooks.driver_status, forced_status);

    if (forced_status == PSA_SUCCESS) {
        mbedtls_test_driver_hash_hooks = mbedtls_test_driver_hash_hooks_init();
        PSA_ASSERT(psa_hash_finish(&operation,
                                   output, PSA_HASH_LENGTH(alg),
                                   &output_length));
        /* Two calls to the driver interface: update + abort */
        TEST_EQUAL(mbedtls_test_driver_hash_hooks.hits, 2);
        TEST_EQUAL(mbedtls_test_driver_hash_hooks.driver_status, PSA_SUCCESS);

        ASSERT_COMPARE(output, output_length, hash->x, hash->len);
    }

exit:
    psa_hash_abort(&operation);
    mbedtls_free(output);
    PSA_DONE();
    mbedtls_test_driver_hash_hooks = mbedtls_test_driver_hash_hooks_init();
}

static void test_hash_multipart_update_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};

    test_hash_multipart_update( *( (int *) params[0] ), &data1, &data3, *( (int *) params[5] ) );
}
#line 2370 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_hash_multipart_finish(int alg_arg,
                           data_t *input, data_t *hash,
                           int forced_status_arg)
{
    psa_algorithm_t alg = alg_arg;
    psa_status_t forced_status = forced_status_arg;
    unsigned char *output = NULL;
    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    size_t output_length;

    mbedtls_test_driver_hash_hooks = mbedtls_test_driver_hash_hooks_init();
    ASSERT_ALLOC(output, PSA_HASH_LENGTH(alg));

    PSA_ASSERT(psa_crypto_init());

    /*
     * Finish inactive operation, the driver shouldn't be called.
     */
    TEST_EQUAL(psa_hash_finish(&operation, output, PSA_HASH_LENGTH(alg),
                               &output_length),
               PSA_ERROR_BAD_STATE);
    TEST_EQUAL(mbedtls_test_driver_hash_hooks.hits, 0);

    PSA_ASSERT(psa_hash_setup(&operation, alg));
    TEST_EQUAL(mbedtls_test_driver_hash_hooks.hits, 1);
    TEST_EQUAL(mbedtls_test_driver_hash_hooks.driver_status, PSA_SUCCESS);

    PSA_ASSERT(psa_hash_update(&operation, input->x, input->len));
    TEST_EQUAL(mbedtls_test_driver_hash_hooks.hits, 2);
    TEST_EQUAL(mbedtls_test_driver_hash_hooks.driver_status, PSA_SUCCESS);

    mbedtls_test_driver_hash_hooks.forced_status = forced_status;
    TEST_EQUAL(psa_hash_finish(&operation,
                               output, PSA_HASH_LENGTH(alg),
                               &output_length),
               forced_status);
    /* Two more calls to the driver interface: finish + abort */
    TEST_EQUAL(mbedtls_test_driver_hash_hooks.hits, 4);
    TEST_EQUAL(mbedtls_test_driver_hash_hooks.driver_status, forced_status);

    if (forced_status == PSA_SUCCESS) {
        ASSERT_COMPARE(output, output_length, hash->x, hash->len);
    }

exit:
    psa_hash_abort(&operation);
    mbedtls_free(output);
    PSA_DONE();
    mbedtls_test_driver_hash_hooks = mbedtls_test_driver_hash_hooks_init();
}

static void test_hash_multipart_finish_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};

    test_hash_multipart_finish( *( (int *) params[0] ), &data1, &data3, *( (int *) params[5] ) );
}
#line 2423 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_hash_clone(int alg_arg,
                data_t *input, data_t *hash,
                int forced_status_arg)
{
    psa_algorithm_t alg = alg_arg;
    psa_status_t forced_status = forced_status_arg;
    unsigned char *output = NULL;
    psa_hash_operation_t source_operation = PSA_HASH_OPERATION_INIT;
    psa_hash_operation_t target_operation = PSA_HASH_OPERATION_INIT;
    size_t output_length;

    mbedtls_test_driver_hash_hooks = mbedtls_test_driver_hash_hooks_init();
    ASSERT_ALLOC(output, PSA_HASH_LENGTH(alg));

    PSA_ASSERT(psa_crypto_init());

    /*
     * Clone inactive operation, the driver shouldn't be called.
     */
    TEST_EQUAL(psa_hash_clone(&source_operation, &target_operation),
               PSA_ERROR_BAD_STATE);
    TEST_EQUAL(mbedtls_test_driver_hash_hooks.hits, 0);

    PSA_ASSERT(psa_hash_setup(&source_operation, alg));
    TEST_EQUAL(mbedtls_test_driver_hash_hooks.hits, 1);
    TEST_EQUAL(mbedtls_test_driver_hash_hooks.driver_status, PSA_SUCCESS);

    mbedtls_test_driver_hash_hooks.forced_status = forced_status;
    TEST_EQUAL(psa_hash_clone(&source_operation, &target_operation),
               forced_status);
    TEST_EQUAL(mbedtls_test_driver_hash_hooks.hits,
               forced_status == PSA_SUCCESS ? 2 : 3);
    TEST_EQUAL(mbedtls_test_driver_hash_hooks.driver_status, forced_status);

    if (forced_status == PSA_SUCCESS) {
        mbedtls_test_driver_hash_hooks = mbedtls_test_driver_hash_hooks_init();
        PSA_ASSERT(psa_hash_update(&target_operation,
                                   input->x, input->len));
        TEST_EQUAL(mbedtls_test_driver_hash_hooks.hits, 1);
        TEST_EQUAL(mbedtls_test_driver_hash_hooks.driver_status, PSA_SUCCESS);

        PSA_ASSERT(psa_hash_finish(&target_operation,
                                   output, PSA_HASH_LENGTH(alg),
                                   &output_length));
        TEST_EQUAL(mbedtls_test_driver_hash_hooks.hits, 3);
        TEST_EQUAL(mbedtls_test_driver_hash_hooks.driver_status, PSA_SUCCESS);

        ASSERT_COMPARE(output, output_length, hash->x, hash->len);
    }

exit:
    psa_hash_abort(&source_operation);
    psa_hash_abort(&target_operation);
    mbedtls_free(output);
    PSA_DONE();
    mbedtls_test_driver_hash_hooks = mbedtls_test_driver_hash_hooks_init();
}

static void test_hash_clone_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};

    test_hash_clone( *( (int *) params[0] ), &data1, &data3, *( (int *) params[5] ) );
}
#line 2483 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_asymmetric_encrypt_decrypt(int alg_arg,
                                data_t *key_data,
                                data_t *input_data,
                                data_t *label,
                                data_t *fake_output_encrypt,
                                data_t *fake_output_decrypt,
                                int forced_status_encrypt_arg,
                                int forced_status_decrypt_arg,
                                int expected_status_encrypt_arg,
                                int expected_status_decrypt_arg)
{
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_type_t key_type = PSA_KEY_TYPE_RSA_KEY_PAIR;
    psa_algorithm_t alg = alg_arg;
    size_t key_bits;
    unsigned char *output = NULL;
    size_t output_size;
    size_t output_length = ~0;
    unsigned char *output2 = NULL;
    size_t output2_size;
    size_t output2_length = ~0;
    psa_status_t forced_status_encrypt = forced_status_encrypt_arg;
    psa_status_t forced_status_decrypt = forced_status_decrypt_arg;
    psa_status_t expected_status_encrypt = expected_status_encrypt_arg;
    psa_status_t expected_status_decrypt = expected_status_decrypt_arg;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    PSA_ASSERT(psa_crypto_init());
    mbedtls_test_driver_asymmetric_encryption_hooks =
        mbedtls_test_driver_asymmetric_encryption_hooks_init();

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, key_type);

    PSA_ASSERT(psa_import_key(&attributes, key_data->x, key_data->len,
                              &key));

    /* Determine the maximum ciphertext length */
    PSA_ASSERT(psa_get_key_attributes(key, &attributes));
    key_bits = psa_get_key_bits(&attributes);

    mbedtls_test_driver_asymmetric_encryption_hooks.forced_status =
        forced_status_encrypt;
    if (fake_output_encrypt->len > 0) {
        mbedtls_test_driver_asymmetric_encryption_hooks.forced_output =
            fake_output_encrypt->x;
        mbedtls_test_driver_asymmetric_encryption_hooks.forced_output_length =
            fake_output_encrypt->len;
        output_size = fake_output_encrypt->len;
        ASSERT_ALLOC(output, output_size);
    } else {
        output_size = PSA_ASYMMETRIC_ENCRYPT_OUTPUT_SIZE(key_type, key_bits, alg);
        TEST_ASSERT(output_size <= PSA_ASYMMETRIC_ENCRYPT_OUTPUT_MAX_SIZE);
        ASSERT_ALLOC(output, output_size);
    }

    /* We test encryption by checking that encrypt-then-decrypt gives back
     * the original plaintext because of the non-optional random
     * part of encryption process which prevents using fixed vectors. */
    TEST_EQUAL(psa_asymmetric_encrypt(key, alg,
                                      input_data->x, input_data->len,
                                      label->x, label->len,
                                      output, output_size,
                                      &output_length), expected_status_encrypt);
    /* We don't know what ciphertext length to expect, but check that
     * it looks sensible. */
    TEST_ASSERT(output_length <= output_size);

    if (expected_status_encrypt == PSA_SUCCESS) {
        if (fake_output_encrypt->len > 0) {
            ASSERT_COMPARE(fake_output_encrypt->x, fake_output_encrypt->len,
                           output, output_length);
        } else {
            mbedtls_test_driver_asymmetric_encryption_hooks.forced_status =
                forced_status_decrypt;
            if (fake_output_decrypt->len > 0) {
                mbedtls_test_driver_asymmetric_encryption_hooks.forced_output =
                    fake_output_decrypt->x;
                mbedtls_test_driver_asymmetric_encryption_hooks.forced_output_length =
                    fake_output_decrypt->len;
                output2_size = fake_output_decrypt->len;
                ASSERT_ALLOC(output2, output2_size);
            } else {
                output2_size = input_data->len;
                TEST_ASSERT(output2_size <=
                            PSA_ASYMMETRIC_DECRYPT_OUTPUT_SIZE(key_type, key_bits, alg));
                TEST_ASSERT(output2_size <= PSA_ASYMMETRIC_DECRYPT_OUTPUT_MAX_SIZE);
                ASSERT_ALLOC(output2, output2_size);
            }

            TEST_EQUAL(psa_asymmetric_decrypt(key, alg,
                                              output, output_length,
                                              label->x, label->len,
                                              output2, output2_size,
                                              &output2_length), expected_status_decrypt);
            if (expected_status_decrypt == PSA_SUCCESS) {
                if (fake_output_decrypt->len > 0) {
                    ASSERT_COMPARE(fake_output_decrypt->x, fake_output_decrypt->len,
                                   output2, output2_length);
                } else {
                    ASSERT_COMPARE(input_data->x, input_data->len,
                                   output2, output2_length);
                }
            }
        }
    }

exit:
    /*
     * Key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes(&attributes);

    psa_destroy_key(key);
    mbedtls_free(output);
    mbedtls_free(output2);
    PSA_DONE();
}

static void test_asymmetric_encrypt_decrypt_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data9 = {(uint8_t *) params[9], *( (uint32_t *) params[10] )};

    test_asymmetric_encrypt_decrypt( *( (int *) params[0] ), &data1, &data3, &data5, &data7, &data9, *( (int *) params[11] ), *( (int *) params[12] ), *( (int *) params[13] ), *( (int *) params[14] ) );
}
#line 2606 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_asymmetric_decrypt(int alg_arg,
                        data_t *key_data,
                        data_t *input_data,
                        data_t *label,
                        data_t *expected_output_data,
                        data_t *fake_output_decrypt,
                        int forced_status_decrypt_arg,
                        int expected_status_decrypt_arg)
{
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_type_t key_type = PSA_KEY_TYPE_RSA_KEY_PAIR;
    psa_algorithm_t alg = alg_arg;
    unsigned char *output = NULL;
    size_t output_size;
    size_t output_length = ~0;
    psa_status_t forced_status_decrypt = forced_status_decrypt_arg;
    psa_status_t expected_status_decrypt = expected_status_decrypt_arg;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    PSA_ASSERT(psa_crypto_init());
    mbedtls_test_driver_asymmetric_encryption_hooks =
        mbedtls_test_driver_asymmetric_encryption_hooks_init();

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, key_type);

    PSA_ASSERT(psa_import_key(&attributes, key_data->x, key_data->len,
                              &key));

    mbedtls_test_driver_asymmetric_encryption_hooks.forced_status =
        forced_status_decrypt;

    if (fake_output_decrypt->len > 0) {
        mbedtls_test_driver_asymmetric_encryption_hooks.forced_output =
            fake_output_decrypt->x;
        mbedtls_test_driver_asymmetric_encryption_hooks.forced_output_length =
            fake_output_decrypt->len;
        output_size = fake_output_decrypt->len;
        ASSERT_ALLOC(output, output_size);
    } else {
        output_size = expected_output_data->len;
        ASSERT_ALLOC(output, expected_output_data->len);
    }

    TEST_EQUAL(psa_asymmetric_decrypt(key, alg,
                                      input_data->x, input_data->len,
                                      label->x, label->len,
                                      output, output_size,
                                      &output_length), expected_status_decrypt);
    if (expected_status_decrypt == PSA_SUCCESS) {
        TEST_EQUAL(output_length, expected_output_data->len);
        ASSERT_COMPARE(expected_output_data->x, expected_output_data->len,
                       output, output_length);
    }
exit:
    /*
     * Key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes(&attributes);

    psa_destroy_key(key);
    mbedtls_free(output);
    PSA_DONE();
}

static void test_asymmetric_decrypt_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data9 = {(uint8_t *) params[9], *( (uint32_t *) params[10] )};

    test_asymmetric_decrypt( *( (int *) params[0] ), &data1, &data3, &data5, &data7, &data9, *( (int *) params[11] ), *( (int *) params[12] ) );
}
#line 2675 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_asymmetric_encrypt(int alg_arg,
                        data_t *key_data,
                        data_t *modulus,
                        data_t *private_exponent,
                        data_t *input_data,
                        data_t *label,
                        data_t *fake_output_encrypt,
                        int forced_status_encrypt_arg,
                        int expected_status_encrypt_arg)
{
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_type_t key_type = PSA_KEY_TYPE_RSA_PUBLIC_KEY;
    psa_algorithm_t alg = alg_arg;
    unsigned char *output = NULL;
    size_t output_size;
    size_t output_length = ~0;
    psa_status_t forced_status_encrypt = forced_status_encrypt_arg;
    psa_status_t expected_status_encrypt = expected_status_encrypt_arg;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    PSA_ASSERT(psa_crypto_init());
    mbedtls_test_driver_asymmetric_encryption_hooks =
        mbedtls_test_driver_asymmetric_encryption_hooks_init();

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, key_type);

    PSA_ASSERT(psa_import_key(&attributes, key_data->x, key_data->len,
                              &key));

    PSA_ASSERT(psa_get_key_attributes(key, &attributes));
    size_t key_bits = psa_get_key_bits(&attributes);

    mbedtls_test_driver_asymmetric_encryption_hooks.forced_status =
        forced_status_encrypt;

    if (fake_output_encrypt->len > 0) {
        mbedtls_test_driver_asymmetric_encryption_hooks.forced_output =
            fake_output_encrypt->x;
        mbedtls_test_driver_asymmetric_encryption_hooks.forced_output_length =
            fake_output_encrypt->len;
        output_size = fake_output_encrypt->len;
        ASSERT_ALLOC(output, output_size);
    } else {
        output_size = PSA_ASYMMETRIC_ENCRYPT_OUTPUT_SIZE(key_type, key_bits, alg);
        ASSERT_ALLOC(output, output_size);
    }

    TEST_EQUAL(psa_asymmetric_encrypt(key, alg,
                                      input_data->x, input_data->len,
                                      label->x, label->len,
                                      output, output_size,
                                      &output_length), expected_status_encrypt);
    if (expected_status_encrypt == PSA_SUCCESS) {
        if (fake_output_encrypt->len > 0) {
            TEST_EQUAL(fake_output_encrypt->len, output_length);
            ASSERT_COMPARE(fake_output_encrypt->x, fake_output_encrypt->len,
                           output, output_length);
        } else {
            /* Perform sanity checks on the output */
#if PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY
            if (PSA_KEY_TYPE_IS_RSA(key_type)) {
                if (!sanity_check_rsa_encryption_result(
                        alg, modulus, private_exponent,
                        input_data,
                        output, output_length)) {
                    goto exit;
                }
            } else
#endif
            {
                (void) modulus;
                (void) private_exponent;
                TEST_ASSERT(!"Encryption sanity checks not implemented for this key type");
            }
        }
    }
exit:
    /*
     * Key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes(&attributes);

    psa_destroy_key(key);
    mbedtls_free(output);
    PSA_DONE();
}

static void test_asymmetric_encrypt_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data9 = {(uint8_t *) params[9], *( (uint32_t *) params[10] )};
    data_t data11 = {(uint8_t *) params[11], *( (uint32_t *) params[12] )};

    test_asymmetric_encrypt( *( (int *) params[0] ), &data1, &data3, &data5, &data7, &data9, &data11, *( (int *) params[13] ), *( (int *) params[14] ) );
}
#line 2767 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_aead_encrypt_setup(int key_type_arg, data_t *key_data,
                        int alg_arg,
                        data_t *nonce,
                        data_t *additional_data,
                        data_t *input_data,
                        data_t *expected_ciphertext,
                        data_t *expected_tag,
                        int forced_status_arg,
                        int expected_status_arg)
{
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_type_t key_type = key_type_arg;
    psa_algorithm_t alg = alg_arg;
    size_t key_bits;
    psa_status_t forced_status = forced_status_arg;
    psa_status_t expected_status = expected_status_arg;
    uint8_t *output_data = NULL;
    size_t output_size = 0;
    size_t output_length = 0;
    size_t finish_output_length = 0;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;
    size_t tag_length = 0;
    uint8_t tag_buffer[PSA_AEAD_TAG_MAX_SIZE];

    psa_aead_operation_t operation = psa_aead_operation_init();

    mbedtls_test_driver_aead_hooks = mbedtls_test_driver_aead_hooks_init();

    PSA_INIT();

    mbedtls_test_driver_aead_hooks.forced_status = forced_status;

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, key_type);

    PSA_ASSERT(psa_import_key(&attributes, key_data->x, key_data->len,
                              &key));
    PSA_ASSERT(psa_get_key_attributes(key, &attributes));
    key_bits = psa_get_key_bits(&attributes);

    output_size = input_data->len + PSA_AEAD_TAG_LENGTH(key_type, key_bits,
                                                        alg);

    /* For all currently defined algorithms, PSA_AEAD_ENCRYPT_OUTPUT_SIZE
     * should be exact. */
    TEST_EQUAL(output_size,
               PSA_AEAD_ENCRYPT_OUTPUT_SIZE(key_type, alg, input_data->len));
    TEST_ASSERT(output_size <=
                PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE(input_data->len));
    ASSERT_ALLOC(output_data, output_size);

    status = psa_aead_encrypt_setup(&operation, key, alg);

    TEST_EQUAL(status, expected_status);
    TEST_EQUAL(mbedtls_test_driver_aead_hooks.hits_encrypt_setup, 1);

    if (status == PSA_SUCCESS) {
        /* Set the nonce. */
        PSA_ASSERT(psa_aead_set_nonce(&operation, nonce->x, nonce->len));

        TEST_EQUAL(mbedtls_test_driver_aead_hooks.hits_set_nonce,
                   forced_status == PSA_SUCCESS ? 1 : 0);

        /* Check hooks hits and
         * set length (additional data and data to encrypt) */
        PSA_ASSERT(psa_aead_set_lengths(&operation, additional_data->len,
                                        input_data->len));

        TEST_EQUAL(mbedtls_test_driver_aead_hooks.hits_set_lengths,
                   forced_status == PSA_SUCCESS ? 1 : 0);

        /* Pass the additional data */
        PSA_ASSERT(psa_aead_update_ad(&operation, additional_data->x,
                                      additional_data->len));

        TEST_EQUAL(mbedtls_test_driver_aead_hooks.hits_update_ad,
                   forced_status == PSA_SUCCESS ? 1 : 0);

        /* Pass the data to encrypt */
        PSA_ASSERT(psa_aead_update(&operation, input_data->x, input_data->len,
                                   output_data, output_size, &output_length));

        TEST_EQUAL(mbedtls_test_driver_aead_hooks.hits_update,
                   forced_status == PSA_SUCCESS ? 1 : 0);

        /* Finish the encryption operation */
        PSA_ASSERT(psa_aead_finish(&operation, output_data + output_length,
                                   output_size - output_length,
                                   &finish_output_length, tag_buffer,
                                   PSA_AEAD_TAG_MAX_SIZE, &tag_length));

        TEST_EQUAL(mbedtls_test_driver_aead_hooks.hits_finish,
                   forced_status == PSA_SUCCESS ? 1 : 0);

        TEST_EQUAL(mbedtls_test_driver_aead_hooks.hits_abort,
                   forced_status == PSA_SUCCESS ? 1 : 0);

        /* Compare output_data and expected_ciphertext */
        ASSERT_COMPARE(expected_ciphertext->x, expected_ciphertext->len,
                       output_data, output_length + finish_output_length);

        /* Compare tag and expected_tag */
        ASSERT_COMPARE(expected_tag->x, expected_tag->len, tag_buffer, tag_length);
    }

exit:
    /* Cleanup */
    PSA_ASSERT(psa_destroy_key(key));
    mbedtls_free(output_data);
    PSA_DONE();
    mbedtls_test_driver_aead_hooks = mbedtls_test_driver_aead_hooks_init();
}

static void test_aead_encrypt_setup_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};
    data_t data12 = {(uint8_t *) params[12], *( (uint32_t *) params[13] )};

    test_aead_encrypt_setup( *( (int *) params[0] ), &data1, *( (int *) params[3] ), &data4, &data6, &data8, &data10, &data12, *( (int *) params[14] ), *( (int *) params[15] ) );
}
#line 2884 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_aead_decrypt_setup(int key_type_arg, data_t *key_data,
                        int alg_arg,
                        data_t *nonce,
                        data_t *additional_data,
                        data_t *input_ciphertext,
                        data_t *input_tag,
                        data_t *expected_result,
                        int forced_status_arg,
                        int expected_status_arg)
{
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_type_t key_type = key_type_arg;
    psa_algorithm_t alg = alg_arg;
    unsigned char *output_data = NULL;
    size_t output_size = 0;
    size_t output_length = 0;
    size_t verify_output_length = 0;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t forced_status = forced_status_arg;
    psa_status_t expected_status = expected_status_arg;
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;

    psa_aead_operation_t operation = psa_aead_operation_init();
    mbedtls_test_driver_aead_hooks = mbedtls_test_driver_aead_hooks_init();

    PSA_INIT();

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, key_type);

    PSA_ASSERT(psa_import_key(&attributes, key_data->x, key_data->len,
                              &key));

    output_size = input_ciphertext->len;

    ASSERT_ALLOC(output_data, output_size);

    mbedtls_test_driver_aead_hooks.forced_status = forced_status;

    status = psa_aead_decrypt_setup(&operation, key, alg);

    TEST_EQUAL(status, (forced_status == PSA_ERROR_NOT_SUPPORTED) ?
               PSA_SUCCESS : forced_status);

    TEST_EQUAL(status, expected_status);
    TEST_EQUAL(mbedtls_test_driver_aead_hooks.hits_decrypt_setup, 1);

    if (status == PSA_SUCCESS) {
        PSA_ASSERT(psa_aead_set_nonce(&operation, nonce->x, nonce->len));
        TEST_EQUAL(mbedtls_test_driver_aead_hooks.hits_set_nonce,
                   forced_status == PSA_SUCCESS ? 1 : 0);

        PSA_ASSERT(psa_aead_set_lengths(&operation, additional_data->len,
                                        input_ciphertext->len));

        TEST_EQUAL(mbedtls_test_driver_aead_hooks.hits_set_lengths,
                   forced_status == PSA_SUCCESS ? 1 : 0);

        PSA_ASSERT(psa_aead_update_ad(&operation, additional_data->x,
                                      additional_data->len));

        TEST_EQUAL(mbedtls_test_driver_aead_hooks.hits_update_ad,
                   forced_status == PSA_SUCCESS ? 1 : 0);

        PSA_ASSERT(psa_aead_update(&operation, input_ciphertext->x,
                                   input_ciphertext->len, output_data,
                                   output_size, &output_length));

        TEST_EQUAL(mbedtls_test_driver_aead_hooks.hits_update,
                   forced_status == PSA_SUCCESS ? 1 : 0);

        /* Offset applied to output_data in order to handle cases where verify()
         * outputs further data */
        PSA_ASSERT(psa_aead_verify(&operation, output_data + output_length,
                                   output_size - output_length,
                                   &verify_output_length, input_tag->x,
                                   input_tag->len));

        TEST_EQUAL(mbedtls_test_driver_aead_hooks.hits_verify,
                   forced_status == PSA_SUCCESS ? 1 : 0);

        /* Since this is a decryption operation,
         * finish should never be hit */
        TEST_EQUAL(mbedtls_test_driver_aead_hooks.hits_finish, 0);

        TEST_EQUAL(mbedtls_test_driver_aead_hooks.hits_abort,
                   forced_status == PSA_SUCCESS ? 1 : 0);

        ASSERT_COMPARE(expected_result->x, expected_result->len,
                       output_data, output_length + verify_output_length);
    }

exit:
    PSA_ASSERT(psa_destroy_key(key));
    mbedtls_free(output_data);
    PSA_DONE();
}

static void test_aead_decrypt_setup_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};
    data_t data12 = {(uint8_t *) params[12], *( (uint32_t *) params[13] )};

    test_aead_decrypt_setup( *( (int *) params[0] ), &data1, *( (int *) params[3] ), &data4, &data6, &data8, &data10, &data12, *( (int *) params[14] ), *( (int *) params[15] ) );
}
#if defined(PSA_WANT_ALG_JPAKE)
#line 2985 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_pake_operations(data_t *pw_data, int forced_status_setup_arg, int forced_status_arg,
                     data_t *forced_output, int expected_status_arg,
                     int fut)
{
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_status_t forced_status = forced_status_arg;
    psa_status_t forced_status_setup = forced_status_setup_arg;
    psa_status_t expected_status = expected_status_arg;
    psa_pake_operation_t operation = psa_pake_operation_init();
    psa_pake_cipher_suite_t cipher_suite = psa_pake_cipher_suite_init();
    psa_key_derivation_operation_t implicit_key =
        PSA_KEY_DERIVATION_OPERATION_INIT;
    psa_pake_primitive_t primitive = PSA_PAKE_PRIMITIVE(
        PSA_PAKE_PRIMITIVE_TYPE_ECC,
        PSA_ECC_FAMILY_SECP_R1, 256);
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    unsigned char *input_buffer = NULL;
    const size_t size_key_share = PSA_PAKE_INPUT_SIZE(PSA_ALG_JPAKE, primitive,
                                                      PSA_PAKE_STEP_KEY_SHARE);
    unsigned char *output_buffer = NULL;
    size_t output_len = 0;
    size_t output_size = PSA_PAKE_OUTPUT_SIZE(PSA_ALG_JPAKE, primitive,
                                              PSA_PAKE_STEP_KEY_SHARE);
    int in_driver = (forced_status_setup_arg == PSA_SUCCESS);

    ASSERT_ALLOC(input_buffer,
                 PSA_PAKE_INPUT_SIZE(PSA_ALG_JPAKE, primitive,
                                     PSA_PAKE_STEP_KEY_SHARE));
    memset(input_buffer, 0xAA, size_key_share);

    ASSERT_ALLOC(output_buffer,
                 PSA_PAKE_INPUT_SIZE(PSA_ALG_JPAKE, primitive,
                                     PSA_PAKE_STEP_KEY_SHARE));
    memset(output_buffer, 0x55, output_size);

    PSA_INIT();

    mbedtls_test_driver_pake_hooks = mbedtls_test_driver_pake_hooks_init();

    if (pw_data->len > 0) {
        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DERIVE);
        psa_set_key_algorithm(&attributes, PSA_ALG_JPAKE);
        psa_set_key_type(&attributes, PSA_KEY_TYPE_PASSWORD);
        PSA_ASSERT(psa_import_key(&attributes, pw_data->x, pw_data->len,
                                  &key));
    }

    psa_pake_cs_set_algorithm(&cipher_suite, PSA_ALG_JPAKE);
    psa_pake_cs_set_primitive(&cipher_suite, primitive);
    psa_pake_cs_set_hash(&cipher_suite, PSA_ALG_SHA_256);

    mbedtls_test_driver_pake_hooks.forced_status = forced_status_setup;

    /* Collecting input stage (no driver entry points) */

    TEST_EQUAL(psa_pake_setup(&operation, &cipher_suite),
               PSA_SUCCESS);

    PSA_ASSERT(psa_pake_set_user(&operation, jpake_server_id, sizeof(jpake_server_id)));
    PSA_ASSERT(psa_pake_set_peer(&operation, jpake_client_id, sizeof(jpake_client_id)));

    TEST_EQUAL(psa_pake_set_password_key(&operation, key),
               PSA_SUCCESS);

    TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total, 0);

    /* Computation stage (driver entry points) */

    switch (fut) {
        case 0: /* setup (via input) */
            /* --- psa_pake_input (driver: setup, input) --- */
            mbedtls_test_driver_pake_hooks.forced_setup_status = forced_status_setup;
            mbedtls_test_driver_pake_hooks.forced_status = forced_status;
            TEST_EQUAL(psa_pake_input(&operation, PSA_PAKE_STEP_KEY_SHARE,
                                      input_buffer, size_key_share),
                       expected_status);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total, 1);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.setup, 1);
            break;

        case 1: /* setup (via output) */
            /* --- psa_pake_output (driver: setup, output) --- */
            mbedtls_test_driver_pake_hooks.forced_setup_status = forced_status_setup;
            mbedtls_test_driver_pake_hooks.forced_status = forced_status;
            TEST_EQUAL(psa_pake_output(&operation, PSA_PAKE_STEP_KEY_SHARE,
                                       output_buffer, output_size, &output_len),
                       expected_status);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total, 1);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.setup, 1);
            break;

        case 2: /* input */
            /* --- psa_pake_input (driver: setup, input, abort) --- */
            mbedtls_test_driver_pake_hooks.forced_setup_status = forced_status_setup;
            mbedtls_test_driver_pake_hooks.forced_status = forced_status;
            TEST_EQUAL(psa_pake_input(&operation, PSA_PAKE_STEP_KEY_SHARE,
                                      input_buffer, size_key_share),
                       expected_status);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total, in_driver ? 3 : 1);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.setup, 1);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.input, in_driver ? 1 : 0);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.abort, in_driver ? 1 : 0);
            break;

        case 3: /* output */
            /* --- psa_pake_output (driver: setup, output, (abort)) --- */
            mbedtls_test_driver_pake_hooks.forced_setup_status = forced_status_setup;
            mbedtls_test_driver_pake_hooks.forced_status = forced_status;
            if (forced_output->len > 0) {
                mbedtls_test_driver_pake_hooks.forced_output = forced_output->x;
                mbedtls_test_driver_pake_hooks.forced_output_length = forced_output->len;
            }
            TEST_EQUAL(psa_pake_output(&operation, PSA_PAKE_STEP_KEY_SHARE,
                                       output_buffer, output_size, &output_len),
                       expected_status);

            if (forced_output->len > 0) {
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total, in_driver ? 2 : 1);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.setup, 1);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.output, in_driver ? 1 : 0);
                TEST_EQUAL(output_len, forced_output->len);
                TEST_EQUAL(memcmp(output_buffer, forced_output->x, output_len), 0);
            } else {
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total, in_driver ? 3 : 1);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.setup, 1);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.output, in_driver ? 1 : 0);
                TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.abort, in_driver ? 1 : 0);
            }
            break;

        case 4: /* get_implicit_key */
            /* Call driver setup indirectly */
            TEST_EQUAL(psa_pake_input(&operation, PSA_PAKE_STEP_KEY_SHARE,
                                      input_buffer, size_key_share),
                       PSA_SUCCESS);

            /* Simulate that we are ready to get implicit key. */
            operation.computation_stage.jpake.input_step = PSA_PAKE_STEP_DERIVE;
            operation.computation_stage.jpake.output_step = PSA_PAKE_STEP_DERIVE;

            /* --- psa_pake_get_implicit_key --- */
            mbedtls_test_driver_pake_hooks.forced_status = forced_status;
            memset(&mbedtls_test_driver_pake_hooks.hits, 0,
                   sizeof(mbedtls_test_driver_pake_hooks.hits));
            TEST_EQUAL(psa_pake_get_implicit_key(&operation, &implicit_key),
                       expected_status);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total, 2);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.implicit_key, 1);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.abort, 1);

            break;

        case 5: /* abort */
            /* Call driver setup indirectly */
            TEST_EQUAL(psa_pake_input(&operation, PSA_PAKE_STEP_KEY_SHARE,
                                      input_buffer, size_key_share),
                       PSA_SUCCESS);

            /* --- psa_pake_abort --- */
            mbedtls_test_driver_pake_hooks.forced_status = forced_status;
            memset(&mbedtls_test_driver_pake_hooks.hits, 0,
                   sizeof(mbedtls_test_driver_pake_hooks.hits));
            TEST_EQUAL(psa_pake_abort(&operation), expected_status);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total, 1);
            TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.abort, 1);
            break;

        default:
            break;
    }

    /* Clean up */
    mbedtls_test_driver_pake_hooks.forced_setup_status = PSA_SUCCESS;
    mbedtls_test_driver_pake_hooks.forced_status = PSA_SUCCESS;
    TEST_EQUAL(psa_pake_abort(&operation), PSA_SUCCESS);
exit:
    /*
     * Key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes(&attributes);
    mbedtls_free(input_buffer);
    mbedtls_free(output_buffer);
    psa_destroy_key(key);
    mbedtls_test_driver_pake_hooks =
        mbedtls_test_driver_pake_hooks_init();
    PSA_DONE();
}

static void test_pake_operations_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};

    test_pake_operations( &data0, *( (int *) params[2] ), *( (int *) params[3] ), &data4, *( (int *) params[6] ), *( (int *) params[7] ) );
}
#endif /* PSA_WANT_ALG_JPAKE */
#if defined(PSA_WANT_ALG_JPAKE)
#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR)
#if defined(PSA_WANT_ECC_SECP_R1_256)
#if defined(PSA_WANT_ALG_SHA_256)
#line 3176 "suites/test_suite_psa_crypto_driver_wrappers.function"
static void test_ecjpake_rounds(int alg_arg, int primitive_arg, int hash_arg,
                    int derive_alg_arg, data_t *pw_data,
                    int client_input_first, int in_driver)
{
    psa_pake_cipher_suite_t cipher_suite = psa_pake_cipher_suite_init();
    psa_pake_operation_t server = psa_pake_operation_init();
    psa_pake_operation_t client = psa_pake_operation_init();
    psa_algorithm_t alg = alg_arg;
    psa_algorithm_t hash_alg = hash_arg;
    psa_algorithm_t derive_alg = derive_alg_arg;
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_derivation_operation_t server_derive =
        PSA_KEY_DERIVATION_OPERATION_INIT;
    psa_key_derivation_operation_t client_derive =
        PSA_KEY_DERIVATION_OPERATION_INIT;
    pake_in_driver = in_driver;
    /* driver setup is called indirectly through pake_output/pake_input */
    if (pake_in_driver) {
        pake_expected_hit_count = 2;
    } else {
        pake_expected_hit_count = 1;
    }

    PSA_INIT();

    mbedtls_test_driver_pake_hooks = mbedtls_test_driver_pake_hooks_init();

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DERIVE);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_PASSWORD);
    PSA_ASSERT(psa_import_key(&attributes, pw_data->x, pw_data->len,
                              &key));

    psa_pake_cs_set_algorithm(&cipher_suite, alg);
    psa_pake_cs_set_primitive(&cipher_suite, primitive_arg);
    psa_pake_cs_set_hash(&cipher_suite, hash_alg);

    /* Get shared key */
    PSA_ASSERT(psa_key_derivation_setup(&server_derive, derive_alg));
    PSA_ASSERT(psa_key_derivation_setup(&client_derive, derive_alg));

    if (PSA_ALG_IS_TLS12_PSK_TO_MS(derive_alg)) {
        PSA_ASSERT(psa_key_derivation_input_bytes(&server_derive,
                                                  PSA_KEY_DERIVATION_INPUT_SEED,
                                                  (const uint8_t *) "", 0));
        PSA_ASSERT(psa_key_derivation_input_bytes(&client_derive,
                                                  PSA_KEY_DERIVATION_INPUT_SEED,
                                                  (const uint8_t *) "", 0));
    }

    if (!pake_in_driver) {
        mbedtls_test_driver_pake_hooks.forced_setup_status = PSA_ERROR_NOT_SUPPORTED;
    }

    PSA_ASSERT(psa_pake_setup(&server, &cipher_suite));
    TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total, 0);
    PSA_ASSERT(psa_pake_setup(&client, &cipher_suite));
    TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total, 0);


    PSA_ASSERT(psa_pake_set_user(&server, jpake_server_id, sizeof(jpake_server_id)));
    PSA_ASSERT(psa_pake_set_peer(&server, jpake_client_id, sizeof(jpake_client_id)));
    TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total, 0);
    PSA_ASSERT(psa_pake_set_user(&client, jpake_client_id, sizeof(jpake_client_id)));
    PSA_ASSERT(psa_pake_set_peer(&client, jpake_server_id, sizeof(jpake_server_id)));
    TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total, 0);
    PSA_ASSERT(psa_pake_set_password_key(&server, key));
    TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total, 0);
    PSA_ASSERT(psa_pake_set_password_key(&client, key));
    TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total, 0);

    /* First round */
    ecjpake_do_round(alg, primitive_arg, &server, &client,
                     client_input_first, 1);

    /* Second round */
    ecjpake_do_round(alg, primitive_arg, &server, &client,
                     client_input_first, 2);

    /* After the key is obtained operation is aborted.
       Adapt counter of expected hits. */
    if (pake_in_driver) {
        pake_expected_hit_count++;
    }

    PSA_ASSERT(psa_pake_get_implicit_key(&server, &server_derive));
    TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
               pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);

    /* After the key is obtained operation is aborted.
       Adapt counter of expected hits. */
    if (pake_in_driver) {
        pake_expected_hit_count++;
    }

    PSA_ASSERT(psa_pake_get_implicit_key(&client, &client_derive));
    TEST_EQUAL(mbedtls_test_driver_pake_hooks.hits.total,
               pake_in_driver ? pake_expected_hit_count++ : pake_expected_hit_count);
exit:
    psa_key_derivation_abort(&server_derive);
    psa_key_derivation_abort(&client_derive);
    psa_destroy_key(key);
    psa_pake_abort(&server);
    psa_pake_abort(&client);
    PSA_DONE();
}

static void test_ecjpake_rounds_wrapper( void ** params )
{
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};

    test_ecjpake_rounds( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), &data4, *( (int *) params[6] ), *( (int *) params[7] ) );
}
#endif /* PSA_WANT_ALG_SHA_256 */
#endif /* PSA_WANT_ECC_SECP_R1_256 */
#endif /* PSA_WANT_KEY_TYPE_ECC_KEY_PAIR */
#endif /* PSA_WANT_ALG_JPAKE */
#endif /* PSA_CRYPTO_DRIVER_TEST */
#endif /* MBEDTLS_PSA_CRYPTO_DRIVERS */
#endif /* MBEDTLS_PSA_CRYPTO_C */


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
    
#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)

        case 0:
            {
                *out_value = PSA_KEY_TYPE_ECC_KEY_PAIR( PSA_ECC_FAMILY_SECP_R1 );
            }
            break;
        case 1:
            {
                *out_value = PSA_ALG_DETERMINISTIC_ECDSA( PSA_ALG_SHA_256 );
            }
            break;
        case 2:
            {
                *out_value = PSA_SUCCESS;
            }
            break;
        case 3:
            {
                *out_value = PSA_ERROR_NOT_SUPPORTED;
            }
            break;
        case 4:
            {
                *out_value = PSA_ERROR_GENERIC_ERROR;
            }
            break;
        case 5:
            {
                *out_value = PSA_KEY_TYPE_RSA_KEY_PAIR;
            }
            break;
        case 6:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_SIGN_RAW;
            }
            break;
        case 7:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256);
            }
            break;
        case 8:
            {
                *out_value = PSA_KEY_TYPE_ECC_PUBLIC_KEY( PSA_ECC_FAMILY_SECP_R1 );
            }
            break;
        case 9:
            {
                *out_value = PSA_ALG_ECDSA( PSA_ALG_SHA_256 );
            }
            break;
        case 10:
            {
                *out_value = PSA_KEY_TYPE_RSA_PUBLIC_KEY;
            }
            break;
        case 11:
            {
                *out_value = PSA_ALG_RSA_PSS(PSA_ALG_SHA_256);
            }
            break;
        case 12:
            {
                *out_value = PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_SHA_256);
            }
            break;
        case 13:
            {
                *out_value = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1);
            }
            break;
        case 14:
            {
                *out_value = PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_256);
            }
            break;
        case 15:
            {
                *out_value = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1);
            }
            break;
        case 16:
            {
                *out_value = PSA_ALG_ECDSA(PSA_ALG_SHA_256);
            }
            break;
        case 17:
            {
                *out_value = PSA_KEY_LOCATION_LOCAL_STORAGE;
            }
            break;
        case 18:
            {
                *out_value = PSA_CRYPTO_TEST_DRIVER_LOCATION;
            }
            break;
        case 19:
            {
                *out_value = PSA_ALG_ECDH;
            }
            break;
        case 20:
            {
                *out_value = PSA_ALG_CTR;
            }
            break;
        case 21:
            {
                *out_value = PSA_KEY_TYPE_AES;
            }
            break;
        case 22:
            {
                *out_value = PSA_ALG_CCM;
            }
            break;
        case 23:
            {
                *out_value = PSA_ERROR_INSUFFICIENT_MEMORY;
            }
            break;
        case 24:
            {
                *out_value = PSA_ALG_GCM;
            }
            break;
        case 25:
            {
                *out_value = PSA_KEY_TYPE_HMAC;
            }
            break;
        case 26:
            {
                *out_value = PSA_ALG_HMAC(PSA_ALG_SHA_224);
            }
            break;
        case 27:
            {
                *out_value = PSA_ALG_CMAC;
            }
            break;
        case 28:
            {
                *out_value = MBEDTLS_PSA_KEY_ID_BUILTIN_MIN;
            }
            break;
        case 29:
            {
                *out_value = MBEDTLS_PSA_KEY_ID_BUILTIN_MAX - 1;
            }
            break;
        case 30:
            {
                *out_value = MBEDTLS_PSA_KEY_ID_BUILTIN_MAX;
            }
            break;
        case 31:
            {
                *out_value = MBEDTLS_PSA_KEY_ID_BUILTIN_MIN - 1;
            }
            break;
        case 32:
            {
                *out_value = PSA_ERROR_INVALID_HANDLE;
            }
            break;
        case 33:
            {
                *out_value = MBEDTLS_PSA_KEY_ID_BUILTIN_MAX + 1;
            }
            break;
        case 34:
            {
                *out_value = MBEDTLS_PSA_KEY_ID_BUILTIN_MIN + 1;
            }
            break;
        case 35:
            {
                *out_value = PSA_ALG_ECDSA(PSA_ALG_ANY_HASH);
            }
            break;
        case 36:
            {
                *out_value = PSA_ERROR_INVALID_ARGUMENT;
            }
            break;
        case 37:
            {
                *out_value = PSA_ALG_SHA_256;
            }
            break;
        case 38:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_CRYPT;
            }
            break;
        case 39:
            {
                *out_value = PSA_ALG_RSA_OAEP(PSA_ALG_SHA_256);
            }
            break;
        case 40:
            {
                *out_value = PSA_ALG_JPAKE;
            }
            break;
        case 41:
            {
                *out_value = PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC, PSA_ECC_FAMILY_SECP_R1, 256);
            }
            break;
        case 42:
            {
                *out_value = PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_SHA_256);
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
    
#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)

        case 0:
            {
#if defined(PSA_WANT_ALG_DETERMINISTIC_ECDSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(PSA_WANT_ECC_SECP_R1_256)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 3:
            {
#if defined(PSA_WANT_ALG_SHA_256)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 4:
            {
#if defined(MBEDTLS_PSA_BUILTIN_ALG_ECDSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 5:
            {
#if defined(PSA_WANT_ALG_RSA_PKCS1V15_SIGN)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 6:
            {
#if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 7:
            {
#if defined(MBEDTLS_MD_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 8:
            {
#if defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PKCS1V15_CRYPT)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 9:
            {
#if defined(PSA_WANT_ALG_ECDSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 10:
            {
#if defined(PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 11:
            {
#if defined(PSA_WANT_ALG_RSA_PSS)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 12:
            {
#if defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PSS)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 13:
            {
#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_KEY_PAIR)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 14:
            {
#if !defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_KEY_PAIR)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 15:
            {
#if defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 16:
            {
#if defined(MBEDTLS_PSA_BUILTIN_ECC_SECP_R1_256)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 17:
            {
#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_PUBLIC_KEY)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 18:
            {
#if defined(PSA_WANT_ALG_ECDH)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 19:
            {
#if defined(MBEDTLS_PSA_BUILTIN_ALG_ECDH)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 20:
            {
#if defined(PSA_WANT_ALG_CTR)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 21:
            {
#if defined(PSA_WANT_KEY_TYPE_AES)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 22:
            {
#if defined(MBEDTLS_PSA_BUILTIN_ALG_CTR)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 23:
            {
#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_AES)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 24:
            {
#if defined(MBEDTLS_PSA_BUILTIN_CIPHER)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 25:
            {
#if defined(PSA_WANT_ALG_CCM)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 26:
            {
#if defined(MBEDTLS_AES_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 27:
            {
#if defined(MBEDTLS_CCM_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 28:
            {
#if defined(PSA_WANT_ALG_GCM)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 29:
            {
#if defined(MBEDTLS_GCM_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 30:
            {
#if defined(PSA_WANT_ALG_HMAC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 31:
            {
#if defined(PSA_WANT_ALG_SHA_224)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 32:
            {
#if defined(PSA_WANT_KEY_TYPE_HMAC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 33:
            {
#if defined(MBEDTLS_PSA_ACCEL_ALG_HMAC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 34:
            {
#if defined(MBEDTLS_PSA_BUILTIN_ALG_HMAC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 35:
            {
#if defined(PSA_WANT_ALG_CMAC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 36:
            {
#if defined(MBEDTLS_PSA_ACCEL_ALG_CMAC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 37:
            {
#if defined(MBEDTLS_PSA_BUILTIN_ALG_CMAC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 38:
            {
#if defined(MBEDTLS_PSA_ACCEL_ALG_SHA_256)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 39:
            {
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_256)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 40:
            {
#if !defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_256)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 41:
            {
#if !defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PKCS1V15_CRYPT)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 42:
            {
#if defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_OAEP)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 43:
            {
#if !defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_OAEP)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 44:
            {
#if !defined(MBEDTLS_PSA_BUILTIN_PAKE)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 45:
            {
#if defined(PSA_WANT_ALG_JPAKE)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 46:
            {
#if defined(PSA_WANT_ALG_TLS12_PSK_TO_MS)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 47:
            {
#if defined(MBEDTLS_PSA_BUILTIN_ALG_JPAKE)
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

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_sign_hash_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_verify_hash_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_sign_message_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_verify_message_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST) && defined(PSA_WANT_ALG_ECDSA) && defined(PSA_WANT_ECC_SECP_R1_256)
    test_generate_key_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_validate_key_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_export_key_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_key_agreement_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_cipher_encrypt_validation_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_cipher_encrypt_multipart_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_cipher_decrypt_multipart_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_cipher_decrypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 12 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_cipher_entry_points_wrapper,
#else
    NULL,
#endif
/* Function Id: 13 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_aead_encrypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 14 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_aead_decrypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 15 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_mac_sign_wrapper,
#else
    NULL,
#endif
/* Function Id: 16 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_mac_sign_multipart_wrapper,
#else
    NULL,
#endif
/* Function Id: 17 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_mac_verify_wrapper,
#else
    NULL,
#endif
/* Function Id: 18 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_mac_verify_multipart_wrapper,
#else
    NULL,
#endif
/* Function Id: 19 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST) && defined(PSA_CRYPTO_DRIVER_TEST) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS)
    test_builtin_key_export_wrapper,
#else
    NULL,
#endif
/* Function Id: 20 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST) && defined(PSA_CRYPTO_DRIVER_TEST) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS)
    test_builtin_pubkey_export_wrapper,
#else
    NULL,
#endif
/* Function Id: 21 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_hash_compute_wrapper,
#else
    NULL,
#endif
/* Function Id: 22 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_hash_multipart_setup_wrapper,
#else
    NULL,
#endif
/* Function Id: 23 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_hash_multipart_update_wrapper,
#else
    NULL,
#endif
/* Function Id: 24 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_hash_multipart_finish_wrapper,
#else
    NULL,
#endif
/* Function Id: 25 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_hash_clone_wrapper,
#else
    NULL,
#endif
/* Function Id: 26 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_asymmetric_encrypt_decrypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 27 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_asymmetric_decrypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 28 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_asymmetric_encrypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 29 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_aead_encrypt_setup_wrapper,
#else
    NULL,
#endif
/* Function Id: 30 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST)
    test_aead_decrypt_setup_wrapper,
#else
    NULL,
#endif
/* Function Id: 31 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST) && defined(PSA_WANT_ALG_JPAKE)
    test_pake_operations_wrapper,
#else
    NULL,
#endif
/* Function Id: 32 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_DRIVERS) && defined(PSA_CRYPTO_DRIVER_TEST) && defined(PSA_WANT_ALG_JPAKE) && defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR) && defined(PSA_WANT_ECC_SECP_R1_256) && defined(PSA_WANT_ALG_SHA_256)
    test_ecjpake_rounds_wrapper,
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
    const char *default_filename = "./test_suite_psa_crypto_driver_wrappers.datax";
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
