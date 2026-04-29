#line 2 "suites/main_test.function"
/*
 * *** THIS FILE HAS BEEN MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py
 *
 * Test file      : ./test_suite_psa_crypto_pake.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_psa_crypto_pake.function
 *      Test suite data     : suites/test_suite_psa_crypto_pake.data
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
#line 2 "suites/test_suite_psa_crypto_pake.function"
#include <stdint.h>

#include "psa/crypto.h"

typedef enum {
    ERR_NONE = 0,
    /* errors forced internally in the code */
    ERR_INJECT_UNINITIALIZED_ACCESS,
    ERR_INJECT_DUPLICATE_SETUP,
    ERR_INJECT_SET_USER,
    ERR_INJECT_SET_PEER,
    ERR_INJECT_SET_ROLE,
    ERR_DUPLICATE_SET_USER,
    ERR_DUPLICATE_SET_PEER,
    ERR_INJECT_EMPTY_IO_BUFFER,
    ERR_INJECT_UNKNOWN_STEP,
    ERR_INJECT_INVALID_FIRST_STEP,
    ERR_INJECT_WRONG_BUFFER_SIZE,
    ERR_INJECT_VALID_OPERATION_AFTER_FAILURE,
    ERR_INJECT_ANTICIPATE_KEY_DERIVATION_1,
    ERR_INJECT_ANTICIPATE_KEY_DERIVATION_2,
    ERR_INJECT_ROUND1_CLIENT_KEY_SHARE_PART1,
    ERR_INJECT_ROUND1_CLIENT_ZK_PUBLIC_PART1,
    ERR_INJECT_ROUND1_CLIENT_ZK_PROOF_PART1,
    ERR_INJECT_ROUND1_CLIENT_KEY_SHARE_PART2,
    ERR_INJECT_ROUND1_CLIENT_ZK_PUBLIC_PART2,
    ERR_INJECT_ROUND1_CLIENT_ZK_PROOF_PART2,
    ERR_INJECT_ROUND2_CLIENT_KEY_SHARE,
    ERR_INJECT_ROUND2_CLIENT_ZK_PUBLIC,
    ERR_INJECT_ROUND2_CLIENT_ZK_PROOF,
    ERR_INJECT_ROUND1_SERVER_KEY_SHARE_PART1,
    ERR_INJECT_ROUND1_SERVER_ZK_PUBLIC_PART1,
    ERR_INJECT_ROUND1_SERVER_ZK_PROOF_PART1,
    ERR_INJECT_ROUND1_SERVER_KEY_SHARE_PART2,
    ERR_INJECT_ROUND1_SERVER_ZK_PUBLIC_PART2,
    ERR_INJECT_ROUND1_SERVER_ZK_PROOF_PART2,
    ERR_INJECT_ROUND2_SERVER_KEY_SHARE,
    ERR_INJECT_ROUND2_SERVER_ZK_PUBLIC,
    ERR_INJECT_ROUND2_SERVER_ZK_PROOF,
    /* erros issued from the .data file */
    ERR_IN_SETUP,
    ERR_IN_SET_USER,
    ERR_IN_SET_PEER,
    ERR_IN_SET_ROLE,
    ERR_IN_SET_PASSWORD_KEY,
    ERR_IN_INPUT,
    ERR_IN_OUTPUT,
} ecjpake_error_stage_t;

typedef enum {
    PAKE_ROUND_ONE,
    PAKE_ROUND_TWO
} pake_round_t;

/* The only two JPAKE user/peer identifiers supported for the time being. */
static const uint8_t jpake_server_id[] = { 's', 'e', 'r', 'v', 'e', 'r' };
static const uint8_t jpake_client_id[] = { 'c', 'l', 'i', 'e', 'n', 't' };

/*
 * Inject an error on the specified buffer ONLY it this is the correct stage.
 * Offset 7 is arbitrary, but chosen because it's "in the middle" of the part
 * we're corrupting.
 */
#define DO_ROUND_CONDITIONAL_INJECT(this_stage, buf) \
    if (this_stage == err_stage)                  \
    {                                               \
        *(buf + 7) ^= 1;                           \
    }

#define DO_ROUND_UPDATE_OFFSETS(main_buf_offset, step_offset, step_size) \
    {                                       \
        step_offset = main_buf_offset;      \
        main_buf_offset += step_size;        \
    }

#define DO_ROUND_CHECK_FAILURE()                                  \
    if (err_stage != ERR_NONE && status != PSA_SUCCESS)            \
    {                                                               \
        TEST_EQUAL(status, expected_error_arg);                   \
        break;                                                      \
    }                                                               \
    else                                                            \
    {                                                               \
        TEST_EQUAL(status, PSA_SUCCESS);                          \
    }

#if defined(PSA_WANT_ALG_JPAKE)
static void ecjpake_do_round(psa_algorithm_t alg, unsigned int primitive,
                             psa_pake_operation_t *server,
                             psa_pake_operation_t *client,
                             int client_input_first,
                             pake_round_t round,
                             ecjpake_error_stage_t err_stage,
                             int expected_error_arg)
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
        case PAKE_ROUND_ONE:
            /* Server first round Output */
            PSA_ASSERT(psa_pake_output(server, PSA_PAKE_STEP_KEY_SHARE,
                                       buffer0 + buffer0_off,
                                       512 - buffer0_off, &s_g1_len));
            TEST_EQUAL(s_g1_len, expected_size_key_share);
            DO_ROUND_CONDITIONAL_INJECT(
                ERR_INJECT_ROUND1_SERVER_KEY_SHARE_PART1,
                buffer0 + buffer0_off);
            DO_ROUND_UPDATE_OFFSETS(buffer0_off, s_g1_off, s_g1_len);

            PSA_ASSERT(psa_pake_output(server, PSA_PAKE_STEP_ZK_PUBLIC,
                                       buffer0 + buffer0_off,
                                       512 - buffer0_off, &s_x1_pk_len));
            TEST_EQUAL(s_x1_pk_len, expected_size_zk_public);
            DO_ROUND_CONDITIONAL_INJECT(
                ERR_INJECT_ROUND1_SERVER_ZK_PUBLIC_PART1,
                buffer0 + buffer0_off);
            DO_ROUND_UPDATE_OFFSETS(buffer0_off, s_x1_pk_off, s_x1_pk_len);

            PSA_ASSERT(psa_pake_output(server, PSA_PAKE_STEP_ZK_PROOF,
                                       buffer0 + buffer0_off,
                                       512 - buffer0_off, &s_x1_pr_len));
            TEST_LE_U(s_x1_pr_len, max_expected_size_zk_proof);
            DO_ROUND_CONDITIONAL_INJECT(
                ERR_INJECT_ROUND1_SERVER_ZK_PROOF_PART1,
                buffer0 + buffer0_off);
            DO_ROUND_UPDATE_OFFSETS(buffer0_off, s_x1_pr_off, s_x1_pr_len);

            PSA_ASSERT(psa_pake_output(server, PSA_PAKE_STEP_KEY_SHARE,
                                       buffer0 + buffer0_off,
                                       512 - buffer0_off, &s_g2_len));
            TEST_EQUAL(s_g2_len, expected_size_key_share);
            DO_ROUND_CONDITIONAL_INJECT(
                ERR_INJECT_ROUND1_SERVER_KEY_SHARE_PART2,
                buffer0 + buffer0_off);
            DO_ROUND_UPDATE_OFFSETS(buffer0_off, s_g2_off, s_g2_len);

            PSA_ASSERT(psa_pake_output(server, PSA_PAKE_STEP_ZK_PUBLIC,
                                       buffer0 + buffer0_off,
                                       512 - buffer0_off, &s_x2_pk_len));
            TEST_EQUAL(s_x2_pk_len, expected_size_zk_public);
            DO_ROUND_CONDITIONAL_INJECT(
                ERR_INJECT_ROUND1_SERVER_ZK_PUBLIC_PART2,
                buffer0 + buffer0_off);
            DO_ROUND_UPDATE_OFFSETS(buffer0_off, s_x2_pk_off, s_x2_pk_len);

            PSA_ASSERT(psa_pake_output(server, PSA_PAKE_STEP_ZK_PROOF,
                                       buffer0 + buffer0_off,
                                       512 - buffer0_off, &s_x2_pr_len));
            TEST_LE_U(s_x2_pr_len, max_expected_size_zk_proof);
            DO_ROUND_CONDITIONAL_INJECT(
                ERR_INJECT_ROUND1_SERVER_ZK_PROOF_PART2,
                buffer0 + buffer0_off);
            DO_ROUND_UPDATE_OFFSETS(buffer0_off, s_x2_pr_off, s_x2_pr_len);

            /*
             * When injecting errors in inputs, the implementation is
             * free to detect it right away of with a delay.
             * This permits delaying the error until the end of the input
             * sequence, if no error appears then, this will be treated
             * as an error.
             */
            if (client_input_first == 1) {
                /* Client first round Input */
                status = psa_pake_input(client, PSA_PAKE_STEP_KEY_SHARE,
                                        buffer0 + s_g1_off, s_g1_len);
                DO_ROUND_CHECK_FAILURE();

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PUBLIC,
                                        buffer0 + s_x1_pk_off,
                                        s_x1_pk_len);
                DO_ROUND_CHECK_FAILURE();

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PROOF,
                                        buffer0 + s_x1_pr_off,
                                        s_x1_pr_len);
                DO_ROUND_CHECK_FAILURE();

                status = psa_pake_input(client, PSA_PAKE_STEP_KEY_SHARE,
                                        buffer0 + s_g2_off,
                                        s_g2_len);
                DO_ROUND_CHECK_FAILURE();

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PUBLIC,
                                        buffer0 + s_x2_pk_off,
                                        s_x2_pk_len);
                DO_ROUND_CHECK_FAILURE();

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PROOF,
                                        buffer0 + s_x2_pr_off,
                                        s_x2_pr_len);
                DO_ROUND_CHECK_FAILURE();

                /* Error didn't trigger, make test fail */
                if ((err_stage >= ERR_INJECT_ROUND1_SERVER_KEY_SHARE_PART1) &&
                    (err_stage <= ERR_INJECT_ROUND1_SERVER_ZK_PROOF_PART2)) {
                    TEST_ASSERT(
                        !"One of the last psa_pake_input() calls should have returned the expected error.");
                }
            }

            /* Client first round Output */
            PSA_ASSERT(psa_pake_output(client, PSA_PAKE_STEP_KEY_SHARE,
                                       buffer1 + buffer1_off,
                                       512 - buffer1_off, &c_g1_len));
            TEST_EQUAL(c_g1_len, expected_size_key_share);
            DO_ROUND_CONDITIONAL_INJECT(
                ERR_INJECT_ROUND1_CLIENT_KEY_SHARE_PART1,
                buffer1 + buffer1_off);
            DO_ROUND_UPDATE_OFFSETS(buffer1_off, c_g1_off, c_g1_len);

            PSA_ASSERT(psa_pake_output(client, PSA_PAKE_STEP_ZK_PUBLIC,
                                       buffer1 + buffer1_off,
                                       512 - buffer1_off, &c_x1_pk_len));
            TEST_EQUAL(c_x1_pk_len, expected_size_zk_public);
            DO_ROUND_CONDITIONAL_INJECT(
                ERR_INJECT_ROUND1_CLIENT_ZK_PUBLIC_PART1,
                buffer1 + buffer1_off);
            DO_ROUND_UPDATE_OFFSETS(buffer1_off, c_x1_pk_off, c_x1_pk_len);

            PSA_ASSERT(psa_pake_output(client, PSA_PAKE_STEP_ZK_PROOF,
                                       buffer1 + buffer1_off,
                                       512 - buffer1_off, &c_x1_pr_len));
            TEST_LE_U(c_x1_pr_len, max_expected_size_zk_proof);
            DO_ROUND_CONDITIONAL_INJECT(
                ERR_INJECT_ROUND1_CLIENT_ZK_PROOF_PART1,
                buffer1 + buffer1_off);
            DO_ROUND_UPDATE_OFFSETS(buffer1_off, c_x1_pr_off, c_x1_pr_len);

            PSA_ASSERT(psa_pake_output(client, PSA_PAKE_STEP_KEY_SHARE,
                                       buffer1 + buffer1_off,
                                       512 - buffer1_off, &c_g2_len));
            TEST_EQUAL(c_g2_len, expected_size_key_share);
            DO_ROUND_CONDITIONAL_INJECT(
                ERR_INJECT_ROUND1_CLIENT_KEY_SHARE_PART2,
                buffer1 + buffer1_off);
            DO_ROUND_UPDATE_OFFSETS(buffer1_off, c_g2_off, c_g2_len);

            PSA_ASSERT(psa_pake_output(client, PSA_PAKE_STEP_ZK_PUBLIC,
                                       buffer1 + buffer1_off,
                                       512 - buffer1_off, &c_x2_pk_len));
            TEST_EQUAL(c_x2_pk_len, expected_size_zk_public);
            DO_ROUND_CONDITIONAL_INJECT(
                ERR_INJECT_ROUND1_CLIENT_ZK_PUBLIC_PART2,
                buffer1 + buffer1_off);
            DO_ROUND_UPDATE_OFFSETS(buffer1_off, c_x2_pk_off, c_x2_pk_len);

            PSA_ASSERT(psa_pake_output(client, PSA_PAKE_STEP_ZK_PROOF,
                                       buffer1 + buffer1_off,
                                       512 - buffer1_off, &c_x2_pr_len));
            TEST_LE_U(c_x2_pr_len, max_expected_size_zk_proof);
            DO_ROUND_CONDITIONAL_INJECT(
                ERR_INJECT_ROUND1_CLIENT_ZK_PROOF_PART2,
                buffer1 + buffer1_off);
            DO_ROUND_UPDATE_OFFSETS(buffer1_off, c_x2_pr_off, buffer1_off);

            if (client_input_first == 0) {
                /* Client first round Input */
                status = psa_pake_input(client, PSA_PAKE_STEP_KEY_SHARE,
                                        buffer0 + s_g1_off, s_g1_len);
                DO_ROUND_CHECK_FAILURE();

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PUBLIC,
                                        buffer0 + s_x1_pk_off,
                                        s_x1_pk_len);
                DO_ROUND_CHECK_FAILURE();

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PROOF,
                                        buffer0 + s_x1_pr_off,
                                        s_x1_pr_len);
                DO_ROUND_CHECK_FAILURE();

                status = psa_pake_input(client, PSA_PAKE_STEP_KEY_SHARE,
                                        buffer0 + s_g2_off,
                                        s_g2_len);
                DO_ROUND_CHECK_FAILURE();

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PUBLIC,
                                        buffer0 + s_x2_pk_off,
                                        s_x2_pk_len);
                DO_ROUND_CHECK_FAILURE();

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PROOF,
                                        buffer0 + s_x2_pr_off,
                                        s_x2_pr_len);
                DO_ROUND_CHECK_FAILURE();

                /* Error didn't trigger, make test fail */
                if ((err_stage >= ERR_INJECT_ROUND1_SERVER_KEY_SHARE_PART1) &&
                    (err_stage <= ERR_INJECT_ROUND1_SERVER_ZK_PROOF_PART2)) {
                    TEST_ASSERT(
                        !"One of the last psa_pake_input() calls should have returned the expected error.");
                }
            }

            /* Server first round Input */
            status = psa_pake_input(server, PSA_PAKE_STEP_KEY_SHARE,
                                    buffer1 + c_g1_off, c_g1_len);
            DO_ROUND_CHECK_FAILURE();

            status = psa_pake_input(server, PSA_PAKE_STEP_ZK_PUBLIC,
                                    buffer1 + c_x1_pk_off, c_x1_pk_len);
            DO_ROUND_CHECK_FAILURE();

            status = psa_pake_input(server, PSA_PAKE_STEP_ZK_PROOF,
                                    buffer1 + c_x1_pr_off, c_x1_pr_len);
            DO_ROUND_CHECK_FAILURE();

            status = psa_pake_input(server, PSA_PAKE_STEP_KEY_SHARE,
                                    buffer1 + c_g2_off, c_g2_len);
            DO_ROUND_CHECK_FAILURE();

            status = psa_pake_input(server, PSA_PAKE_STEP_ZK_PUBLIC,
                                    buffer1 + c_x2_pk_off, c_x2_pk_len);
            DO_ROUND_CHECK_FAILURE();

            status = psa_pake_input(server, PSA_PAKE_STEP_ZK_PROOF,
                                    buffer1 + c_x2_pr_off, c_x2_pr_len);
            DO_ROUND_CHECK_FAILURE();

            /* Error didn't trigger, make test fail */
            if ((err_stage >= ERR_INJECT_ROUND1_CLIENT_KEY_SHARE_PART1) &&
                (err_stage <= ERR_INJECT_ROUND1_CLIENT_ZK_PROOF_PART2)) {
                TEST_ASSERT(
                    !"One of the last psa_pake_input() calls should have returned the expected error.");
            }

            break;

        case PAKE_ROUND_TWO:
            /* Server second round Output */
            buffer0_off = 0;

            PSA_ASSERT(psa_pake_output(server, PSA_PAKE_STEP_KEY_SHARE,
                                       buffer0 + buffer0_off,
                                       512 - buffer0_off, &s_a_len));
            TEST_EQUAL(s_a_len, expected_size_key_share);
            DO_ROUND_CONDITIONAL_INJECT(
                ERR_INJECT_ROUND2_SERVER_KEY_SHARE,
                buffer0 + buffer0_off);
            DO_ROUND_UPDATE_OFFSETS(buffer0_off, s_a_off, s_a_len);

            PSA_ASSERT(psa_pake_output(server, PSA_PAKE_STEP_ZK_PUBLIC,
                                       buffer0 + buffer0_off,
                                       512 - buffer0_off, &s_x2s_pk_len));
            TEST_EQUAL(s_x2s_pk_len, expected_size_zk_public);
            DO_ROUND_CONDITIONAL_INJECT(
                ERR_INJECT_ROUND2_SERVER_ZK_PUBLIC,
                buffer0 + buffer0_off);
            DO_ROUND_UPDATE_OFFSETS(buffer0_off, s_x2s_pk_off, s_x2s_pk_len);

            PSA_ASSERT(psa_pake_output(server, PSA_PAKE_STEP_ZK_PROOF,
                                       buffer0 + buffer0_off,
                                       512 - buffer0_off, &s_x2s_pr_len));
            TEST_LE_U(s_x2s_pr_len, max_expected_size_zk_proof);
            DO_ROUND_CONDITIONAL_INJECT(
                ERR_INJECT_ROUND2_SERVER_ZK_PROOF,
                buffer0 + buffer0_off);
            DO_ROUND_UPDATE_OFFSETS(buffer0_off, s_x2s_pr_off, s_x2s_pr_len);

            if (client_input_first == 1) {
                /* Client second round Input */
                status = psa_pake_input(client, PSA_PAKE_STEP_KEY_SHARE,
                                        buffer0 + s_a_off, s_a_len);
                DO_ROUND_CHECK_FAILURE();

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PUBLIC,
                                        buffer0 + s_x2s_pk_off,
                                        s_x2s_pk_len);
                DO_ROUND_CHECK_FAILURE();

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PROOF,
                                        buffer0 + s_x2s_pr_off,
                                        s_x2s_pr_len);
                DO_ROUND_CHECK_FAILURE();

                /* Error didn't trigger, make test fail */
                if ((err_stage >= ERR_INJECT_ROUND2_SERVER_KEY_SHARE) &&
                    (err_stage <= ERR_INJECT_ROUND2_SERVER_ZK_PROOF)) {
                    TEST_ASSERT(
                        !"One of the last psa_pake_input() calls should have returned the expected error.");
                }
            }

            /* Client second round Output */
            buffer1_off = 0;

            PSA_ASSERT(psa_pake_output(client, PSA_PAKE_STEP_KEY_SHARE,
                                       buffer1 + buffer1_off,
                                       512 - buffer1_off, &c_a_len));
            TEST_EQUAL(c_a_len, expected_size_key_share);
            DO_ROUND_CONDITIONAL_INJECT(
                ERR_INJECT_ROUND2_CLIENT_KEY_SHARE,
                buffer1 + buffer1_off);
            DO_ROUND_UPDATE_OFFSETS(buffer1_off, c_a_off, c_a_len);

            PSA_ASSERT(psa_pake_output(client, PSA_PAKE_STEP_ZK_PUBLIC,
                                       buffer1 + buffer1_off,
                                       512 - buffer1_off, &c_x2s_pk_len));
            TEST_EQUAL(c_x2s_pk_len, expected_size_zk_public);
            DO_ROUND_CONDITIONAL_INJECT(
                ERR_INJECT_ROUND2_CLIENT_ZK_PUBLIC,
                buffer1 + buffer1_off);
            DO_ROUND_UPDATE_OFFSETS(buffer1_off, c_x2s_pk_off, c_x2s_pk_len);

            PSA_ASSERT(psa_pake_output(client, PSA_PAKE_STEP_ZK_PROOF,
                                       buffer1 + buffer1_off,
                                       512 - buffer1_off, &c_x2s_pr_len));
            TEST_LE_U(c_x2s_pr_len, max_expected_size_zk_proof);
            DO_ROUND_CONDITIONAL_INJECT(
                ERR_INJECT_ROUND2_CLIENT_ZK_PROOF,
                buffer1 + buffer1_off);
            DO_ROUND_UPDATE_OFFSETS(buffer1_off, c_x2s_pr_off, c_x2s_pr_len);

            if (client_input_first == 0) {
                /* Client second round Input */
                status = psa_pake_input(client, PSA_PAKE_STEP_KEY_SHARE,
                                        buffer0 + s_a_off, s_a_len);
                DO_ROUND_CHECK_FAILURE();

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PUBLIC,
                                        buffer0 + s_x2s_pk_off,
                                        s_x2s_pk_len);
                DO_ROUND_CHECK_FAILURE();

                status = psa_pake_input(client, PSA_PAKE_STEP_ZK_PROOF,
                                        buffer0 + s_x2s_pr_off,
                                        s_x2s_pr_len);
                DO_ROUND_CHECK_FAILURE();

                /* Error didn't trigger, make test fail */
                if ((err_stage >= ERR_INJECT_ROUND2_SERVER_KEY_SHARE) &&
                    (err_stage <= ERR_INJECT_ROUND2_SERVER_ZK_PROOF)) {
                    TEST_ASSERT(
                        !"One of the last psa_pake_input() calls should have returned the expected error.");
                }
            }

            /* Server second round Input */
            status = psa_pake_input(server, PSA_PAKE_STEP_KEY_SHARE,
                                    buffer1 + c_a_off, c_a_len);
            DO_ROUND_CHECK_FAILURE();

            status = psa_pake_input(server, PSA_PAKE_STEP_ZK_PUBLIC,
                                    buffer1 + c_x2s_pk_off, c_x2s_pk_len);
            DO_ROUND_CHECK_FAILURE();

            status = psa_pake_input(server, PSA_PAKE_STEP_ZK_PROOF,
                                    buffer1 + c_x2s_pr_off, c_x2s_pr_len);
            DO_ROUND_CHECK_FAILURE();

            /* Error didn't trigger, make test fail */
            if ((err_stage >= ERR_INJECT_ROUND2_CLIENT_KEY_SHARE) &&
                (err_stage <= ERR_INJECT_ROUND2_CLIENT_ZK_PROOF)) {
                TEST_ASSERT(
                    !"One of the last psa_pake_input() calls should have returned the expected error.");
            }

            break;

    }

exit:
    mbedtls_free(buffer0);
    mbedtls_free(buffer1);
}
#endif /* PSA_WANT_ALG_JPAKE */

/*
 * This check is used for functions that might either succeed or fail depending
 * on the parameters that are passed in from the *.data file:
 * - in case of success following functions depend on the current one
 * - in case of failure the test is always terminated. There are two options
 *   here
 *     - terminated successfully if this exact error was expected at this stage
 *     - terminated with failure otherwise (either no error was expected at this
 *       stage or a different error code was expected)
 */
#define SETUP_ALWAYS_CHECK_STEP(test_function, this_check_err_stage)      \
    status = test_function;                                                 \
    if (err_stage != this_check_err_stage)                                 \
    {                                                                       \
        PSA_ASSERT(status);                                               \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        TEST_EQUAL(status, expected_error);                               \
        goto exit;                                                          \
    }

/*
 * This check is used for failures that are injected at code level. There's only
 * 1 input parameter that is relevant in this case and it's the stage at which
 * the error should be injected.
 * The check is conditional in this case because, once the error is triggered,
 * the pake's context structure is compromised and the setup function cannot
 * proceed further. As a consequence the test is terminated.
 * The test succeeds if the returned error is exactly the expected one,
 * otherwise it fails.
 */
#define SETUP_CONDITIONAL_CHECK_STEP(test_function, this_check_err_stage) \
    if (err_stage == this_check_err_stage)                                 \
    {                                                                       \
        TEST_EQUAL(test_function, expected_error);                        \
        goto exit;                                                          \
    }
#if defined(PSA_WANT_ALG_JPAKE)
#line 546 "suites/test_suite_psa_crypto_pake.function"
static void test_ecjpake_setup(int alg_arg, int key_type_pw_arg, int key_usage_pw_arg,
                   int primitive_arg, int hash_arg, char *user_arg, char *peer_arg,
                   int test_input,
                   int err_stage_arg,
                   int expected_error_arg)
{
    psa_pake_cipher_suite_t cipher_suite = psa_pake_cipher_suite_init();
    psa_pake_operation_t operation = psa_pake_operation_init();
    psa_algorithm_t alg = alg_arg;
    psa_pake_primitive_t primitive = primitive_arg;
    psa_key_type_t key_type_pw = key_type_pw_arg;
    psa_key_usage_t key_usage_pw = key_usage_pw_arg;
    psa_algorithm_t hash_alg = hash_arg;
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    ecjpake_error_stage_t err_stage = err_stage_arg;
    psa_status_t expected_error = expected_error_arg;
    psa_status_t status;
    unsigned char *output_buffer = NULL;
    size_t output_len = 0;
    const uint8_t password[] = "abcd";
    uint8_t *user = (uint8_t *) user_arg;
    uint8_t *peer = (uint8_t *) peer_arg;
    size_t user_len = strlen(user_arg);
    size_t peer_len = strlen(peer_arg);

    psa_key_derivation_operation_t key_derivation =
        PSA_KEY_DERIVATION_OPERATION_INIT;

    PSA_INIT();

    size_t buf_size = PSA_PAKE_OUTPUT_SIZE(alg, primitive_arg,
                                           PSA_PAKE_STEP_KEY_SHARE);
    ASSERT_ALLOC(output_buffer, buf_size);

    psa_set_key_usage_flags(&attributes, key_usage_pw);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, key_type_pw);
    PSA_ASSERT(psa_import_key(&attributes, password, sizeof(password),
                              &key));

    psa_pake_cs_set_algorithm(&cipher_suite, alg);
    psa_pake_cs_set_primitive(&cipher_suite, primitive);
    psa_pake_cs_set_hash(&cipher_suite, hash_alg);

    PSA_ASSERT(psa_pake_abort(&operation));

    if (err_stage == ERR_INJECT_UNINITIALIZED_ACCESS) {
        TEST_EQUAL(psa_pake_set_user(&operation, user, user_len),
                   expected_error);
        TEST_EQUAL(psa_pake_set_peer(&operation, peer, peer_len),
                   expected_error);
        TEST_EQUAL(psa_pake_set_password_key(&operation, key),
                   expected_error);
        TEST_EQUAL(psa_pake_set_role(&operation, PSA_PAKE_ROLE_SERVER),
                   expected_error);
        TEST_EQUAL(psa_pake_output(&operation, PSA_PAKE_STEP_KEY_SHARE,
                                   output_buffer, 0, &output_len),
                   expected_error);
        TEST_EQUAL(psa_pake_input(&operation, PSA_PAKE_STEP_KEY_SHARE,
                                  output_buffer, 0),
                   expected_error);
        TEST_EQUAL(psa_pake_get_implicit_key(&operation, &key_derivation),
                   expected_error);
        goto exit;
    }

    SETUP_ALWAYS_CHECK_STEP(psa_pake_setup(&operation, &cipher_suite),
                            ERR_IN_SETUP);

    SETUP_CONDITIONAL_CHECK_STEP(psa_pake_setup(&operation, &cipher_suite),
                                 ERR_INJECT_DUPLICATE_SETUP);

    SETUP_CONDITIONAL_CHECK_STEP(psa_pake_set_role(&operation, PSA_PAKE_ROLE_SERVER),
                                 ERR_INJECT_SET_ROLE);

    SETUP_ALWAYS_CHECK_STEP(psa_pake_set_role(&operation, PSA_PAKE_ROLE_NONE),
                            ERR_IN_SET_ROLE);

    SETUP_ALWAYS_CHECK_STEP(psa_pake_set_user(&operation, user, user_len),
                            ERR_IN_SET_USER);

    SETUP_ALWAYS_CHECK_STEP(psa_pake_set_peer(&operation, peer, peer_len),
                            ERR_IN_SET_PEER);

    SETUP_CONDITIONAL_CHECK_STEP(psa_pake_set_user(&operation, user, user_len),
                                 ERR_DUPLICATE_SET_USER);

    SETUP_CONDITIONAL_CHECK_STEP(psa_pake_set_peer(&operation, peer, peer_len),
                                 ERR_DUPLICATE_SET_PEER);

    SETUP_ALWAYS_CHECK_STEP(psa_pake_set_password_key(&operation, key),
                            ERR_IN_SET_PASSWORD_KEY);

    const size_t size_key_share = PSA_PAKE_INPUT_SIZE(alg, primitive,
                                                      PSA_PAKE_STEP_KEY_SHARE);
    const size_t size_zk_public = PSA_PAKE_INPUT_SIZE(alg, primitive,
                                                      PSA_PAKE_STEP_ZK_PUBLIC);
    const size_t size_zk_proof = PSA_PAKE_INPUT_SIZE(alg, primitive,
                                                     PSA_PAKE_STEP_ZK_PROOF);

    if (test_input) {
        SETUP_CONDITIONAL_CHECK_STEP(psa_pake_input(&operation,
                                                    PSA_PAKE_STEP_ZK_PROOF,
                                                    output_buffer, 0),
                                     ERR_INJECT_EMPTY_IO_BUFFER);

        SETUP_CONDITIONAL_CHECK_STEP(psa_pake_input(&operation,
                                                    PSA_PAKE_STEP_ZK_PROOF + 10,
                                                    output_buffer, size_zk_proof),
                                     ERR_INJECT_UNKNOWN_STEP);

        SETUP_CONDITIONAL_CHECK_STEP(psa_pake_input(&operation,
                                                    PSA_PAKE_STEP_ZK_PROOF,
                                                    output_buffer, size_zk_proof),
                                     ERR_INJECT_INVALID_FIRST_STEP)

        SETUP_ALWAYS_CHECK_STEP(psa_pake_input(&operation,
                                               PSA_PAKE_STEP_KEY_SHARE,
                                               output_buffer, size_key_share),
                                ERR_IN_INPUT);

        SETUP_CONDITIONAL_CHECK_STEP(psa_pake_input(&operation,
                                                    PSA_PAKE_STEP_ZK_PUBLIC,
                                                    output_buffer, size_zk_public + 1),
                                     ERR_INJECT_WRONG_BUFFER_SIZE);

        SETUP_CONDITIONAL_CHECK_STEP(
            (psa_pake_input(&operation, PSA_PAKE_STEP_ZK_PUBLIC,
                            output_buffer, size_zk_public + 1),
             psa_pake_input(&operation, PSA_PAKE_STEP_ZK_PUBLIC,
                            output_buffer, size_zk_public)),
            ERR_INJECT_VALID_OPERATION_AFTER_FAILURE);
    } else {
        SETUP_CONDITIONAL_CHECK_STEP(psa_pake_output(&operation,
                                                     PSA_PAKE_STEP_ZK_PROOF,
                                                     output_buffer, 0,
                                                     &output_len),
                                     ERR_INJECT_EMPTY_IO_BUFFER);

        SETUP_CONDITIONAL_CHECK_STEP(psa_pake_output(&operation,
                                                     PSA_PAKE_STEP_ZK_PROOF + 10,
                                                     output_buffer, buf_size, &output_len),
                                     ERR_INJECT_UNKNOWN_STEP);

        SETUP_CONDITIONAL_CHECK_STEP(psa_pake_output(&operation,
                                                     PSA_PAKE_STEP_ZK_PROOF,
                                                     output_buffer, buf_size, &output_len),
                                     ERR_INJECT_INVALID_FIRST_STEP);

        SETUP_ALWAYS_CHECK_STEP(psa_pake_output(&operation,
                                                PSA_PAKE_STEP_KEY_SHARE,
                                                output_buffer, buf_size, &output_len),
                                ERR_IN_OUTPUT);

        TEST_ASSERT(output_len > 0);

        SETUP_CONDITIONAL_CHECK_STEP(psa_pake_output(&operation,
                                                     PSA_PAKE_STEP_ZK_PUBLIC,
                                                     output_buffer, size_zk_public - 1,
                                                     &output_len),
                                     ERR_INJECT_WRONG_BUFFER_SIZE);

        SETUP_CONDITIONAL_CHECK_STEP(
            (psa_pake_output(&operation, PSA_PAKE_STEP_ZK_PUBLIC,
                             output_buffer, size_zk_public - 1, &output_len),
             psa_pake_output(&operation, PSA_PAKE_STEP_ZK_PUBLIC,
                             output_buffer, buf_size, &output_len)),
            ERR_INJECT_VALID_OPERATION_AFTER_FAILURE);
    }

exit:
    PSA_ASSERT(psa_destroy_key(key));
    PSA_ASSERT(psa_pake_abort(&operation));
    mbedtls_free(output_buffer);
    PSA_DONE();
}

static void test_ecjpake_setup_wrapper( void ** params )
{

    test_ecjpake_setup( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), (char *) params[5], (char *) params[6], *( (int *) params[7] ), *( (int *) params[8] ), *( (int *) params[9] ) );
}
#endif /* PSA_WANT_ALG_JPAKE */
#if defined(PSA_WANT_ALG_JPAKE)
#line 726 "suites/test_suite_psa_crypto_pake.function"
static void test_ecjpake_rounds_inject(int alg_arg, int primitive_arg, int hash_arg,
                           int client_input_first,
                           data_t *pw_data,
                           int err_stage_arg,
                           int expected_error_arg)
{
    psa_pake_cipher_suite_t cipher_suite = psa_pake_cipher_suite_init();
    psa_pake_operation_t server = psa_pake_operation_init();
    psa_pake_operation_t client = psa_pake_operation_init();
    psa_algorithm_t alg = alg_arg;
    psa_algorithm_t hash_alg = hash_arg;
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    ecjpake_error_stage_t err_stage = err_stage_arg;

    PSA_INIT();

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DERIVE);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_PASSWORD);

    PSA_ASSERT(psa_import_key(&attributes, pw_data->x, pw_data->len,
                              &key));

    psa_pake_cs_set_algorithm(&cipher_suite, alg);
    psa_pake_cs_set_primitive(&cipher_suite, primitive_arg);
    psa_pake_cs_set_hash(&cipher_suite, hash_alg);

    PSA_ASSERT(psa_pake_setup(&server, &cipher_suite));
    PSA_ASSERT(psa_pake_setup(&client, &cipher_suite));

    PSA_ASSERT(psa_pake_set_user(&server, jpake_server_id, sizeof(jpake_server_id)));
    PSA_ASSERT(psa_pake_set_peer(&server, jpake_client_id, sizeof(jpake_client_id)));
    PSA_ASSERT(psa_pake_set_user(&client, jpake_client_id, sizeof(jpake_client_id)));
    PSA_ASSERT(psa_pake_set_peer(&client, jpake_server_id, sizeof(jpake_server_id)));

    PSA_ASSERT(psa_pake_set_password_key(&server, key));
    PSA_ASSERT(psa_pake_set_password_key(&client, key));

    ecjpake_do_round(alg, primitive_arg, &server, &client,
                     client_input_first, PAKE_ROUND_ONE,
                     err_stage, expected_error_arg);

    if (err_stage != ERR_NONE) {
        goto exit;
    }

    ecjpake_do_round(alg, primitive_arg, &server, &client,
                     client_input_first, PAKE_ROUND_TWO,
                     err_stage, expected_error_arg);

exit:
    psa_destroy_key(key);
    psa_pake_abort(&server);
    psa_pake_abort(&client);
    PSA_DONE();
}

static void test_ecjpake_rounds_inject_wrapper( void ** params )
{
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};

    test_ecjpake_rounds_inject( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), &data4, *( (int *) params[6] ), *( (int *) params[7] ) );
}
#endif /* PSA_WANT_ALG_JPAKE */
#if defined(PSA_WANT_ALG_JPAKE)
#line 786 "suites/test_suite_psa_crypto_pake.function"
static void test_ecjpake_rounds(int alg_arg, int primitive_arg, int hash_arg,
                    int derive_alg_arg, data_t *pw_data,
                    int client_input_first, int destroy_key,
                    int err_stage_arg)
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
    ecjpake_error_stage_t err_stage = err_stage_arg;

    PSA_INIT();

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

    if (PSA_ALG_IS_TLS12_PRF(derive_alg) ||
        PSA_ALG_IS_TLS12_PSK_TO_MS(derive_alg)) {
        PSA_ASSERT(psa_key_derivation_input_bytes(&server_derive,
                                                  PSA_KEY_DERIVATION_INPUT_SEED,
                                                  (const uint8_t *) "", 0));
        PSA_ASSERT(psa_key_derivation_input_bytes(&client_derive,
                                                  PSA_KEY_DERIVATION_INPUT_SEED,
                                                  (const uint8_t *) "", 0));
    }

    PSA_ASSERT(psa_pake_setup(&server, &cipher_suite));
    PSA_ASSERT(psa_pake_setup(&client, &cipher_suite));

    PSA_ASSERT(psa_pake_set_user(&server, jpake_server_id, sizeof(jpake_server_id)));
    PSA_ASSERT(psa_pake_set_peer(&server, jpake_client_id, sizeof(jpake_client_id)));
    PSA_ASSERT(psa_pake_set_user(&client, jpake_client_id, sizeof(jpake_client_id)));
    PSA_ASSERT(psa_pake_set_peer(&client, jpake_server_id, sizeof(jpake_server_id)));

    PSA_ASSERT(psa_pake_set_password_key(&server, key));
    PSA_ASSERT(psa_pake_set_password_key(&client, key));

    if (destroy_key == 1) {
        psa_destroy_key(key);
    }

    if (err_stage == ERR_INJECT_ANTICIPATE_KEY_DERIVATION_1) {
        TEST_EQUAL(psa_pake_get_implicit_key(&server, &server_derive),
                   PSA_ERROR_BAD_STATE);
        TEST_EQUAL(psa_pake_get_implicit_key(&client, &client_derive),
                   PSA_ERROR_BAD_STATE);
        goto exit;
    }

    /* First round */
    ecjpake_do_round(alg, primitive_arg, &server, &client,
                     client_input_first, PAKE_ROUND_ONE,
                     ERR_NONE, PSA_SUCCESS);

    if (err_stage == ERR_INJECT_ANTICIPATE_KEY_DERIVATION_2) {
        TEST_EQUAL(psa_pake_get_implicit_key(&server, &server_derive),
                   PSA_ERROR_BAD_STATE);
        TEST_EQUAL(psa_pake_get_implicit_key(&client, &client_derive),
                   PSA_ERROR_BAD_STATE);
        goto exit;
    }

    /* Second round */
    ecjpake_do_round(alg, primitive_arg, &server, &client,
                     client_input_first, PAKE_ROUND_TWO,
                     ERR_NONE, PSA_SUCCESS);

    PSA_ASSERT(psa_pake_get_implicit_key(&server, &server_derive));
    PSA_ASSERT(psa_pake_get_implicit_key(&client, &client_derive));

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

    test_ecjpake_rounds( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), &data4, *( (int *) params[6] ), *( (int *) params[7] ), *( (int *) params[8] ) );
}
#endif /* PSA_WANT_ALG_JPAKE */
#line 886 "suites/test_suite_psa_crypto_pake.function"
static void test_ecjpake_size_macros(void)
{
    const psa_algorithm_t alg = PSA_ALG_JPAKE;
    const size_t bits = 256;
    const psa_pake_primitive_t prim = PSA_PAKE_PRIMITIVE(
        PSA_PAKE_PRIMITIVE_TYPE_ECC, PSA_ECC_FAMILY_SECP_R1, bits);
    const psa_key_type_t key_type = PSA_KEY_TYPE_ECC_KEY_PAIR(
        PSA_ECC_FAMILY_SECP_R1);

    // https://armmbed.github.io/mbed-crypto/1.1_PAKE_Extension.0-bet.0/html/pake.html#pake-step-types
    /* The output for KEY_SHARE and ZK_PUBLIC is the same as a public key */
    TEST_EQUAL(PSA_PAKE_OUTPUT_SIZE(alg, prim, PSA_PAKE_STEP_KEY_SHARE),
               PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(key_type, bits));
    TEST_EQUAL(PSA_PAKE_OUTPUT_SIZE(alg, prim, PSA_PAKE_STEP_ZK_PUBLIC),
               PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(key_type, bits));
    /* The output for ZK_PROOF is the same bitsize as the curve */
    TEST_EQUAL(PSA_PAKE_OUTPUT_SIZE(alg, prim, PSA_PAKE_STEP_ZK_PROOF),
               PSA_BITS_TO_BYTES(bits));

    /* Input sizes are the same as output sizes */
    TEST_EQUAL(PSA_PAKE_OUTPUT_SIZE(alg, prim, PSA_PAKE_STEP_KEY_SHARE),
               PSA_PAKE_INPUT_SIZE(alg, prim, PSA_PAKE_STEP_KEY_SHARE));
    TEST_EQUAL(PSA_PAKE_OUTPUT_SIZE(alg, prim, PSA_PAKE_STEP_ZK_PUBLIC),
               PSA_PAKE_INPUT_SIZE(alg, prim, PSA_PAKE_STEP_ZK_PUBLIC));
    TEST_EQUAL(PSA_PAKE_OUTPUT_SIZE(alg, prim, PSA_PAKE_STEP_ZK_PROOF),
               PSA_PAKE_INPUT_SIZE(alg, prim, PSA_PAKE_STEP_ZK_PROOF));

    /* These inequalities will always hold even when other PAKEs are added */
    TEST_LE_U(PSA_PAKE_OUTPUT_SIZE(alg, prim, PSA_PAKE_STEP_KEY_SHARE),
              PSA_PAKE_OUTPUT_MAX_SIZE);
    TEST_LE_U(PSA_PAKE_OUTPUT_SIZE(alg, prim, PSA_PAKE_STEP_ZK_PUBLIC),
              PSA_PAKE_OUTPUT_MAX_SIZE);
    TEST_LE_U(PSA_PAKE_OUTPUT_SIZE(alg, prim, PSA_PAKE_STEP_ZK_PROOF),
              PSA_PAKE_OUTPUT_MAX_SIZE);
    TEST_LE_U(PSA_PAKE_INPUT_SIZE(alg, prim, PSA_PAKE_STEP_KEY_SHARE),
              PSA_PAKE_INPUT_MAX_SIZE);
    TEST_LE_U(PSA_PAKE_INPUT_SIZE(alg, prim, PSA_PAKE_STEP_ZK_PUBLIC),
              PSA_PAKE_INPUT_MAX_SIZE);
    TEST_LE_U(PSA_PAKE_INPUT_SIZE(alg, prim, PSA_PAKE_STEP_ZK_PROOF),
              PSA_PAKE_INPUT_MAX_SIZE);
exit:
    ;
}

static void test_ecjpake_size_macros_wrapper( void ** params )
{
    (void)params;

    test_ecjpake_size_macros(  );
}
#if defined(PSA_WANT_ALG_JPAKE)
#line 930 "suites/test_suite_psa_crypto_pake.function"
static void test_pake_input_getters_password(void)
{
    psa_pake_cipher_suite_t cipher_suite = psa_pake_cipher_suite_init();
    psa_pake_operation_t operation = psa_pake_operation_init();
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    const char *password = "password";
    uint8_t password_ret[20] = { 0 }; // max key length is 20 bytes
    size_t password_len_ret = 0;
    size_t buffer_len_ret = 0;

    psa_pake_primitive_t primitive = PSA_PAKE_PRIMITIVE(
        PSA_PAKE_PRIMITIVE_TYPE_ECC,
        PSA_ECC_FAMILY_SECP_R1, 256);

    PSA_INIT();

    psa_pake_cs_set_algorithm(&cipher_suite, PSA_ALG_JPAKE);
    psa_pake_cs_set_primitive(&cipher_suite, primitive);
    psa_pake_cs_set_hash(&cipher_suite, PSA_ALG_SHA_256);

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DERIVE);
    psa_set_key_algorithm(&attributes, PSA_ALG_JPAKE);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_PASSWORD);

    PSA_ASSERT(psa_pake_setup(&operation, &cipher_suite));

    PSA_ASSERT(psa_import_key(&attributes, (uint8_t *) password, strlen(password), &key));

    TEST_EQUAL(psa_crypto_driver_pake_get_password(&operation.data.inputs,
                                                   (uint8_t *) &password_ret,
                                                   10, &buffer_len_ret),
               PSA_ERROR_BAD_STATE);

    TEST_EQUAL(psa_crypto_driver_pake_get_password_len(&operation.data.inputs, &password_len_ret),
               PSA_ERROR_BAD_STATE);

    PSA_ASSERT(psa_pake_set_password_key(&operation, key));

    TEST_EQUAL(psa_crypto_driver_pake_get_password_len(&operation.data.inputs, &password_len_ret),
               PSA_SUCCESS);

    TEST_EQUAL(password_len_ret, strlen(password));

    TEST_EQUAL(psa_crypto_driver_pake_get_password(&operation.data.inputs,
                                                   (uint8_t *) &password_ret,
                                                   password_len_ret - 1,
                                                   &buffer_len_ret),
               PSA_ERROR_BUFFER_TOO_SMALL);

    TEST_EQUAL(psa_crypto_driver_pake_get_password(&operation.data.inputs,
                                                   (uint8_t *) &password_ret,
                                                   password_len_ret,
                                                   &buffer_len_ret),
               PSA_SUCCESS);

    TEST_EQUAL(buffer_len_ret, strlen(password));
    PSA_ASSERT(memcmp(password_ret, password, buffer_len_ret));
exit:
    PSA_ASSERT(psa_destroy_key(key));
    PSA_ASSERT(psa_pake_abort(&operation));
    PSA_DONE();
}

static void test_pake_input_getters_password_wrapper( void ** params )
{
    (void)params;

    test_pake_input_getters_password(  );
}
#endif /* PSA_WANT_ALG_JPAKE */
#if defined(PSA_WANT_ALG_JPAKE)
#line 996 "suites/test_suite_psa_crypto_pake.function"
static void test_pake_input_getters_cipher_suite(void)
{
    psa_pake_cipher_suite_t cipher_suite = psa_pake_cipher_suite_init();
    psa_pake_operation_t operation = psa_pake_operation_init();
    psa_pake_cipher_suite_t cipher_suite_ret = psa_pake_cipher_suite_init();

    psa_pake_primitive_t primitive = PSA_PAKE_PRIMITIVE(
        PSA_PAKE_PRIMITIVE_TYPE_ECC,
        PSA_ECC_FAMILY_SECP_R1, 256);

    PSA_INIT();

    psa_pake_cs_set_algorithm(&cipher_suite, PSA_ALG_JPAKE);
    psa_pake_cs_set_primitive(&cipher_suite, primitive);
    psa_pake_cs_set_hash(&cipher_suite, PSA_ALG_SHA_256);

    TEST_EQUAL(psa_crypto_driver_pake_get_cipher_suite(&operation.data.inputs, &cipher_suite_ret),
               PSA_ERROR_BAD_STATE);

    PSA_ASSERT(psa_pake_setup(&operation, &cipher_suite));

    TEST_EQUAL(psa_crypto_driver_pake_get_cipher_suite(&operation.data.inputs, &cipher_suite_ret),
               PSA_SUCCESS);

    PSA_ASSERT(memcmp(&cipher_suite_ret, &cipher_suite, sizeof(cipher_suite)));

exit:
    PSA_ASSERT(psa_pake_abort(&operation));
    PSA_DONE();
}

static void test_pake_input_getters_cipher_suite_wrapper( void ** params )
{
    (void)params;

    test_pake_input_getters_cipher_suite(  );
}
#endif /* PSA_WANT_ALG_JPAKE */
#if defined(PSA_WANT_ALG_JPAKE)
#line 1029 "suites/test_suite_psa_crypto_pake.function"
static void test_pake_input_getters_role(void)
{
    psa_pake_cipher_suite_t cipher_suite = psa_pake_cipher_suite_init();
    psa_pake_operation_t operation = psa_pake_operation_init();
    psa_pake_role_t role_ret = PSA_PAKE_ROLE_NONE;

    psa_pake_primitive_t primitive = PSA_PAKE_PRIMITIVE(
        PSA_PAKE_PRIMITIVE_TYPE_ECC,
        PSA_ECC_FAMILY_SECP_R1, 256);

    PSA_INIT();

    psa_pake_cs_set_algorithm(&cipher_suite, PSA_ALG_JPAKE);
    psa_pake_cs_set_primitive(&cipher_suite, primitive);
    psa_pake_cs_set_hash(&cipher_suite, PSA_ALG_SHA_256);

    PSA_ASSERT(psa_pake_setup(&operation, &cipher_suite));

    TEST_EQUAL(psa_crypto_driver_pake_get_role(&operation.data.inputs, &role_ret),
               PSA_ERROR_BAD_STATE);

    /* Role can not be set directly using psa_pake_set_role(). It is set by the core
       based on given user/peer identifiers. Simulate that Role is already set. */
    operation.data.inputs.role = PSA_PAKE_ROLE_SERVER;
    TEST_EQUAL(psa_crypto_driver_pake_get_role(&operation.data.inputs, &role_ret),
               PSA_SUCCESS);

    TEST_EQUAL(role_ret, PSA_PAKE_ROLE_SERVER);
exit:
    PSA_ASSERT(psa_pake_abort(&operation));
    PSA_DONE();
}

static void test_pake_input_getters_role_wrapper( void ** params )
{
    (void)params;

    test_pake_input_getters_role(  );
}
#endif /* PSA_WANT_ALG_JPAKE */
#if defined(PSA_WANT_ALG_JPAKE)
#line 1064 "suites/test_suite_psa_crypto_pake.function"
static void test_pake_input_getters_user(void)
{
    psa_pake_cipher_suite_t cipher_suite = psa_pake_cipher_suite_init();
    psa_pake_operation_t operation = psa_pake_operation_init();
    const uint8_t user[] = { 's', 'e', 'r', 'v', 'e', 'r' };
    const size_t user_len = sizeof(user);
    uint8_t user_ret[20] = { 0 }; // max user length is 20 bytes
    size_t user_len_ret = 0;
    size_t buffer_len_ret = 0;

    psa_pake_primitive_t primitive = PSA_PAKE_PRIMITIVE(
        PSA_PAKE_PRIMITIVE_TYPE_ECC,
        PSA_ECC_FAMILY_SECP_R1, 256);

    PSA_INIT();

    psa_pake_cs_set_algorithm(&cipher_suite, PSA_ALG_JPAKE);
    psa_pake_cs_set_primitive(&cipher_suite, primitive);
    psa_pake_cs_set_hash(&cipher_suite, PSA_ALG_SHA_256);

    PSA_ASSERT(psa_pake_setup(&operation, &cipher_suite));

    TEST_EQUAL(psa_crypto_driver_pake_get_user(&operation.data.inputs,
                                               (uint8_t *) &user_ret,
                                               10, &buffer_len_ret),
               PSA_ERROR_BAD_STATE);

    TEST_EQUAL(psa_crypto_driver_pake_get_user_len(&operation.data.inputs, &user_len_ret),
               PSA_ERROR_BAD_STATE);

    PSA_ASSERT(psa_pake_set_user(&operation, user, user_len));

    TEST_EQUAL(psa_crypto_driver_pake_get_user_len(&operation.data.inputs, &user_len_ret),
               PSA_SUCCESS);

    TEST_EQUAL(user_len_ret, user_len);

    TEST_EQUAL(psa_crypto_driver_pake_get_user(&operation.data.inputs,
                                               (uint8_t *) &user_ret,
                                               user_len_ret - 1,
                                               &buffer_len_ret),
               PSA_ERROR_BUFFER_TOO_SMALL);

    TEST_EQUAL(psa_crypto_driver_pake_get_user(&operation.data.inputs,
                                               (uint8_t *) &user_ret,
                                               user_len_ret,
                                               &buffer_len_ret),
               PSA_SUCCESS);

    TEST_EQUAL(buffer_len_ret, user_len);
    PSA_ASSERT(memcmp(user_ret, user, buffer_len_ret));
exit:
    PSA_ASSERT(psa_pake_abort(&operation));
    PSA_DONE();
}

static void test_pake_input_getters_user_wrapper( void ** params )
{
    (void)params;

    test_pake_input_getters_user(  );
}
#endif /* PSA_WANT_ALG_JPAKE */
#if defined(PSA_WANT_ALG_JPAKE)
#line 1122 "suites/test_suite_psa_crypto_pake.function"
static void test_pake_input_getters_peer(void)
{
    psa_pake_cipher_suite_t cipher_suite = psa_pake_cipher_suite_init();
    psa_pake_operation_t operation = psa_pake_operation_init();
    const uint8_t peer[] = { 's', 'e', 'r', 'v', 'e', 'r' };
    const size_t peer_len = sizeof(peer);
    uint8_t peer_ret[20] = { 0 }; // max peer length is 20 bytes
    size_t peer_len_ret = 0;
    size_t buffer_len_ret = 0;

    psa_pake_primitive_t primitive = PSA_PAKE_PRIMITIVE(
        PSA_PAKE_PRIMITIVE_TYPE_ECC,
        PSA_ECC_FAMILY_SECP_R1, 256);

    PSA_INIT();

    psa_pake_cs_set_algorithm(&cipher_suite, PSA_ALG_JPAKE);
    psa_pake_cs_set_primitive(&cipher_suite, primitive);
    psa_pake_cs_set_hash(&cipher_suite, PSA_ALG_SHA_256);

    PSA_ASSERT(psa_pake_setup(&operation, &cipher_suite));

    TEST_EQUAL(psa_crypto_driver_pake_get_peer(&operation.data.inputs,
                                               (uint8_t *) &peer_ret,
                                               10, &buffer_len_ret),
               PSA_ERROR_BAD_STATE);

    TEST_EQUAL(psa_crypto_driver_pake_get_peer_len(&operation.data.inputs, &peer_len_ret),
               PSA_ERROR_BAD_STATE);

    PSA_ASSERT(psa_pake_set_peer(&operation, peer, peer_len));

    TEST_EQUAL(psa_crypto_driver_pake_get_peer_len(&operation.data.inputs, &peer_len_ret),
               PSA_SUCCESS);

    TEST_EQUAL(peer_len_ret, peer_len);

    TEST_EQUAL(psa_crypto_driver_pake_get_peer(&operation.data.inputs,
                                               (uint8_t *) &peer_ret,
                                               peer_len_ret - 1,
                                               &buffer_len_ret),
               PSA_ERROR_BUFFER_TOO_SMALL);

    TEST_EQUAL(psa_crypto_driver_pake_get_peer(&operation.data.inputs,
                                               (uint8_t *) &peer_ret,
                                               peer_len_ret,
                                               &buffer_len_ret),
               PSA_SUCCESS);

    TEST_EQUAL(buffer_len_ret, peer_len);
    PSA_ASSERT(memcmp(peer_ret, peer, buffer_len_ret));
exit:
    PSA_ASSERT(psa_pake_abort(&operation));
    PSA_DONE();
}

static void test_pake_input_getters_peer_wrapper( void ** params )
{
    (void)params;

    test_pake_input_getters_peer(  );
}
#endif /* PSA_WANT_ALG_JPAKE */
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
    
#if defined(MBEDTLS_PSA_CRYPTO_C)

        case 0:
            {
                *out_value = PSA_ALG_JPAKE;
            }
            break;
        case 1:
            {
                *out_value = PSA_KEY_TYPE_PASSWORD;
            }
            break;
        case 2:
            {
                *out_value = PSA_KEY_USAGE_DERIVE;
            }
            break;
        case 3:
            {
                *out_value = PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC, PSA_ECC_FAMILY_SECP_R1, 256);
            }
            break;
        case 4:
            {
                *out_value = PSA_ALG_SHA_256;
            }
            break;
        case 5:
            {
                *out_value = ERR_INJECT_UNINITIALIZED_ACCESS;
            }
            break;
        case 6:
            {
                *out_value = PSA_ERROR_BAD_STATE;
            }
            break;
        case 7:
            {
                *out_value = ERR_IN_SETUP;
            }
            break;
        case 8:
            {
                *out_value = PSA_ERROR_INVALID_ARGUMENT;
            }
            break;
        case 9:
            {
                *out_value = PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_DH, PSA_ECC_FAMILY_SECP_R1, 256);
            }
            break;
        case 10:
            {
                *out_value = ERR_IN_OUTPUT;
            }
            break;
        case 11:
            {
                *out_value = PSA_ERROR_NOT_SUPPORTED;
            }
            break;
        case 12:
            {
                *out_value = PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC, PSA_ECC_FAMILY_SECP_K1, 256);
            }
            break;
        case 13:
            {
                *out_value = PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC, PSA_ECC_FAMILY_SECP_R1, 128);
            }
            break;
        case 14:
            {
                *out_value = PSA_ALG_SHA_1;
            }
            break;
        case 15:
            {
                *out_value = ERR_INJECT_DUPLICATE_SETUP;
            }
            break;
        case 16:
            {
                *out_value = ERR_INJECT_SET_ROLE;
            }
            break;
        case 17:
            {
                *out_value = PSA_KEY_TYPE_HMAC;
            }
            break;
        case 18:
            {
                *out_value = ERR_IN_SET_PASSWORD_KEY;
            }
            break;
        case 19:
            {
                *out_value = PSA_KEY_USAGE_ENCRYPT;
            }
            break;
        case 20:
            {
                *out_value = PSA_ERROR_NOT_PERMITTED;
            }
            break;
        case 21:
            {
                *out_value = ERR_IN_SET_USER;
            }
            break;
        case 22:
            {
                *out_value = ERR_IN_SET_PEER;
            }
            break;
        case 23:
            {
                *out_value = ERR_DUPLICATE_SET_USER;
            }
            break;
        case 24:
            {
                *out_value = ERR_DUPLICATE_SET_PEER;
            }
            break;
        case 25:
            {
                *out_value = ERR_INJECT_EMPTY_IO_BUFFER;
            }
            break;
        case 26:
            {
                *out_value = ERR_INJECT_UNKNOWN_STEP;
            }
            break;
        case 27:
            {
                *out_value = ERR_INJECT_INVALID_FIRST_STEP;
            }
            break;
        case 28:
            {
                *out_value = ERR_INJECT_WRONG_BUFFER_SIZE;
            }
            break;
        case 29:
            {
                *out_value = PSA_ERROR_BUFFER_TOO_SMALL;
            }
            break;
        case 30:
            {
                *out_value = PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_SHA_256);
            }
            break;
        case 31:
            {
                *out_value = ERR_NONE;
            }
            break;
        case 32:
            {
                *out_value = PSA_ALG_TLS12_PRF(PSA_ALG_SHA_256);
            }
            break;
        case 33:
            {
                *out_value = ERR_INJECT_ANTICIPATE_KEY_DERIVATION_1;
            }
            break;
        case 34:
            {
                *out_value = ERR_INJECT_ANTICIPATE_KEY_DERIVATION_2;
            }
            break;
        case 35:
            {
                *out_value = PSA_SUCCESS;
            }
            break;
        case 36:
            {
                *out_value = ERR_INJECT_ROUND1_CLIENT_KEY_SHARE_PART1;
            }
            break;
        case 37:
            {
                *out_value = PSA_ERROR_DATA_INVALID;
            }
            break;
        case 38:
            {
                *out_value = ERR_INJECT_ROUND1_CLIENT_ZK_PUBLIC_PART1;
            }
            break;
        case 39:
            {
                *out_value = ERR_INJECT_ROUND1_CLIENT_ZK_PROOF_PART1;
            }
            break;
        case 40:
            {
                *out_value = ERR_INJECT_ROUND1_CLIENT_KEY_SHARE_PART2;
            }
            break;
        case 41:
            {
                *out_value = ERR_INJECT_ROUND1_CLIENT_ZK_PUBLIC_PART2;
            }
            break;
        case 42:
            {
                *out_value = ERR_INJECT_ROUND1_CLIENT_ZK_PROOF_PART2;
            }
            break;
        case 43:
            {
                *out_value = ERR_INJECT_ROUND1_SERVER_KEY_SHARE_PART1;
            }
            break;
        case 44:
            {
                *out_value = ERR_INJECT_ROUND1_SERVER_ZK_PUBLIC_PART1;
            }
            break;
        case 45:
            {
                *out_value = ERR_INJECT_ROUND1_SERVER_ZK_PROOF_PART1;
            }
            break;
        case 46:
            {
                *out_value = ERR_INJECT_ROUND1_SERVER_KEY_SHARE_PART2;
            }
            break;
        case 47:
            {
                *out_value = ERR_INJECT_ROUND1_SERVER_ZK_PUBLIC_PART2;
            }
            break;
        case 48:
            {
                *out_value = ERR_INJECT_ROUND1_SERVER_ZK_PROOF_PART2;
            }
            break;
        case 49:
            {
                *out_value = ERR_INJECT_ROUND2_CLIENT_KEY_SHARE;
            }
            break;
        case 50:
            {
                *out_value = ERR_INJECT_ROUND2_CLIENT_ZK_PUBLIC;
            }
            break;
        case 51:
            {
                *out_value = ERR_INJECT_ROUND2_CLIENT_ZK_PROOF;
            }
            break;
        case 52:
            {
                *out_value = ERR_INJECT_ROUND2_SERVER_KEY_SHARE;
            }
            break;
        case 53:
            {
                *out_value = ERR_INJECT_ROUND2_SERVER_ZK_PUBLIC;
            }
            break;
        case 54:
            {
                *out_value = ERR_INJECT_ROUND2_SERVER_ZK_PROOF;
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
    
#if defined(MBEDTLS_PSA_CRYPTO_C)

        case 0:
            {
#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(PSA_WANT_ECC_SECP_R1_256)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(PSA_WANT_ALG_SHA_256)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 3:
            {
#if defined(PSA_WANT_ALG_TLS12_PSK_TO_MS)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 4:
            {
#if defined(PSA_WANT_ALG_TLS12_PRF)
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

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(PSA_WANT_ALG_JPAKE)
    test_ecjpake_setup_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(PSA_WANT_ALG_JPAKE)
    test_ecjpake_rounds_inject_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(PSA_WANT_ALG_JPAKE)
    test_ecjpake_rounds_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_PSA_CRYPTO_C)
    test_ecjpake_size_macros_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(PSA_WANT_ALG_JPAKE)
    test_pake_input_getters_password_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(PSA_WANT_ALG_JPAKE)
    test_pake_input_getters_cipher_suite_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(PSA_WANT_ALG_JPAKE)
    test_pake_input_getters_role_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(PSA_WANT_ALG_JPAKE)
    test_pake_input_getters_user_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(PSA_WANT_ALG_JPAKE)
    test_pake_input_getters_peer_wrapper,
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
    const char *default_filename = "./test_suite_psa_crypto_pake.datax";
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
