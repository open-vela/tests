#line 2 "suites/main_test.function"
/*
 * *** THIS FILE HAS BEEN MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py
 *
 * Test file      : ./test_suite_bignum_mod_raw.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_bignum_mod_raw.function
 *      Test suite data     : suites/test_suite_bignum_mod_raw.data
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

#if defined(MBEDTLS_BIGNUM_C)
#line 2 "suites/test_suite_bignum_mod_raw.function"
#include "mbedtls/bignum.h"
#include "mbedtls/entropy.h"
#include "bignum_core.h"
#include "bignum_mod_raw.h"
#include "constant_time_internal.h"
#include "test/constant_flow.h"

#include "bignum_mod_raw_invasive.h"

#line 19 "suites/test_suite_bignum_mod_raw.function"
static void test_mpi_mod_raw_io(data_t *input, int nb_int, int nx_32_int,
                    int iendian, int iret, int oret)
{
    mbedtls_mpi_mod_modulus m;
    mbedtls_mpi_mod_modulus_init(&m);

    if (iret != 0) {
        TEST_ASSERT(oret == 0);
    }

    TEST_LE_S(0, nb_int);
    size_t nb = nb_int;

    unsigned char buf[1024];
    TEST_LE_U(nb, sizeof(buf));

    /* nx_32_int is the number of 32 bit limbs, if we have 64 bit limbs we need
     * to halve the number of limbs to have the same size. */
    size_t nx;
    TEST_LE_S(0, nx_32_int);
    if (sizeof(mbedtls_mpi_uint) == 8) {
        nx = nx_32_int / 2 + nx_32_int % 2;
    } else {
        nx = nx_32_int;
    }

    mbedtls_mpi_uint X[sizeof(buf) / sizeof(mbedtls_mpi_uint)];
    TEST_LE_U(nx, sizeof(X) / sizeof(X[0]));

    int endian;
    if (iendian == MBEDTLS_MPI_MOD_EXT_REP_INVALID) {
        endian = MBEDTLS_MPI_MOD_EXT_REP_LE;
    } else {
        endian = iendian;
    }

    mbedtls_mpi_uint init[sizeof(X) / sizeof(X[0])];
    memset(init, 0xFF, sizeof(init));
    int ret = mbedtls_mpi_mod_modulus_setup(&m, init, nx,
                                            MBEDTLS_MPI_MOD_REP_MONTGOMERY);
    TEST_EQUAL(ret, 0);

    if (iendian == MBEDTLS_MPI_MOD_EXT_REP_INVALID && iret != 0) {
        endian = MBEDTLS_MPI_MOD_EXT_REP_INVALID;
    }

    ret = mbedtls_mpi_mod_raw_read(X, &m, input->x, input->len, endian);
    TEST_EQUAL(ret, iret);

    if (iret == 0) {
        if (iendian == MBEDTLS_MPI_MOD_EXT_REP_INVALID && oret != 0) {
            endian = MBEDTLS_MPI_MOD_EXT_REP_INVALID;
        }

        ret = mbedtls_mpi_mod_raw_write(X, &m, buf, nb, endian);
        TEST_EQUAL(ret, oret);
    }

    if ((iret == 0) && (oret == 0)) {
        if (nb > input->len) {
            if (endian == MBEDTLS_MPI_MOD_EXT_REP_BE) {
                size_t leading_zeroes = nb - input->len;
                TEST_ASSERT(memcmp(buf + nb - input->len, input->x, input->len) == 0);
                for (size_t i = 0; i < leading_zeroes; i++) {
                    TEST_EQUAL(buf[i], 0);
                }
            } else {
                TEST_ASSERT(memcmp(buf, input->x, input->len) == 0);
                for (size_t i = input->len; i < nb; i++) {
                    TEST_EQUAL(buf[i], 0);
                }
            }
        } else {
            if (endian == MBEDTLS_MPI_MOD_EXT_REP_BE) {
                size_t leading_zeroes = input->len - nb;
                TEST_ASSERT(memcmp(input->x + input->len - nb, buf, nb) == 0);
                for (size_t i = 0; i < leading_zeroes; i++) {
                    TEST_EQUAL(input->x[i], 0);
                }
            } else {
                TEST_ASSERT(memcmp(input->x, buf, nb) == 0);
                for (size_t i = nb; i < input->len; i++) {
                    TEST_EQUAL(input->x[i], 0);
                }
            }
        }
    }

exit:
    mbedtls_mpi_mod_modulus_free(&m);
}

static void test_mpi_mod_raw_io_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_mpi_mod_raw_io( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), *( (int *) params[6] ) );
}
#line 113 "suites/test_suite_bignum_mod_raw.function"
static void test_mpi_mod_raw_cond_assign(char *input_X,
                             char *input_Y,
                             int input_bytes)
{
    mbedtls_mpi_uint *X = NULL;
    mbedtls_mpi_uint *Y = NULL;
    mbedtls_mpi_uint *buff_m = NULL;
    size_t limbs_X;
    size_t limbs_Y;

    mbedtls_mpi_mod_modulus m;
    mbedtls_mpi_mod_modulus_init(&m);

    TEST_EQUAL(mbedtls_test_read_mpi_core(&X, &limbs_X, input_X), 0);
    TEST_EQUAL(mbedtls_test_read_mpi_core(&Y, &limbs_Y, input_Y), 0);

    size_t limbs = limbs_X;
    size_t copy_limbs = CHARS_TO_LIMBS(input_bytes);
    size_t bytes = limbs * sizeof(mbedtls_mpi_uint);
    size_t copy_bytes = copy_limbs * sizeof(mbedtls_mpi_uint);

    TEST_EQUAL(limbs_X, limbs_Y);
    TEST_ASSERT(copy_limbs <= limbs);

    ASSERT_ALLOC(buff_m, copy_limbs);
    memset(buff_m, 0xFF, copy_limbs);
    TEST_EQUAL(mbedtls_mpi_mod_modulus_setup(
                   &m, buff_m, copy_limbs,
                   MBEDTLS_MPI_MOD_REP_MONTGOMERY), 0);

    /* condition is false */
    TEST_CF_SECRET(X, bytes);
    TEST_CF_SECRET(Y, bytes);

    mbedtls_mpi_mod_raw_cond_assign(X, Y, &m, 0);

    TEST_CF_PUBLIC(X, bytes);
    TEST_CF_PUBLIC(Y, bytes);

    TEST_ASSERT(memcmp(X, Y, bytes) != 0);

    /* condition is true */
    TEST_CF_SECRET(X, bytes);
    TEST_CF_SECRET(Y, bytes);

    mbedtls_mpi_mod_raw_cond_assign(X, Y, &m, 1);

    TEST_CF_PUBLIC(X, bytes);
    TEST_CF_PUBLIC(Y, bytes);

    /* Check if the given length is copied even it is smaller
       than the length of the given MPIs. */
    if (copy_limbs < limbs) {
        ASSERT_COMPARE(X, copy_bytes, Y, copy_bytes);
        TEST_ASSERT(memcmp(X, Y, bytes) != 0);
    } else {
        ASSERT_COMPARE(X, bytes, Y, bytes);
    }

exit:
    mbedtls_free(X);
    mbedtls_free(Y);

    mbedtls_mpi_mod_modulus_free(&m);
    mbedtls_free(buff_m);
}

static void test_mpi_mod_raw_cond_assign_wrapper( void ** params )
{

    test_mpi_mod_raw_cond_assign( (char *) params[0], (char *) params[1], *( (int *) params[2] ) );
}
#line 182 "suites/test_suite_bignum_mod_raw.function"
static void test_mpi_mod_raw_cond_swap(char *input_X,
                           char *input_Y,
                           int input_bytes)
{
    mbedtls_mpi_uint *tmp_X = NULL;
    mbedtls_mpi_uint *tmp_Y = NULL;
    mbedtls_mpi_uint *X = NULL;
    mbedtls_mpi_uint *Y = NULL;
    mbedtls_mpi_uint *buff_m = NULL;
    size_t limbs_X;
    size_t limbs_Y;

    mbedtls_mpi_mod_modulus m;
    mbedtls_mpi_mod_modulus_init(&m);

    TEST_EQUAL(mbedtls_test_read_mpi_core(&tmp_X, &limbs_X, input_X), 0);
    TEST_EQUAL(mbedtls_test_read_mpi_core(&tmp_Y, &limbs_Y, input_Y), 0);

    size_t limbs = limbs_X;
    size_t copy_limbs = CHARS_TO_LIMBS(input_bytes);
    size_t bytes = limbs * sizeof(mbedtls_mpi_uint);
    size_t copy_bytes = copy_limbs * sizeof(mbedtls_mpi_uint);

    TEST_EQUAL(limbs_X, limbs_Y);
    TEST_ASSERT(copy_limbs <= limbs);

    ASSERT_ALLOC(buff_m, copy_limbs);
    memset(buff_m, 0xFF, copy_limbs);
    TEST_EQUAL(mbedtls_mpi_mod_modulus_setup(
                   &m, buff_m, copy_limbs,
                   MBEDTLS_MPI_MOD_REP_MONTGOMERY), 0);

    ASSERT_ALLOC(X, limbs);
    memcpy(X, tmp_X, bytes);

    ASSERT_ALLOC(Y, bytes);
    memcpy(Y, tmp_Y, bytes);

    /* condition is false */
    TEST_CF_SECRET(X, bytes);
    TEST_CF_SECRET(Y, bytes);

    mbedtls_mpi_mod_raw_cond_swap(X, Y, &m, 0);

    TEST_CF_PUBLIC(X, bytes);
    TEST_CF_PUBLIC(Y, bytes);

    ASSERT_COMPARE(X, bytes, tmp_X, bytes);
    ASSERT_COMPARE(Y, bytes, tmp_Y, bytes);

    /* condition is true */
    TEST_CF_SECRET(X, bytes);
    TEST_CF_SECRET(Y, bytes);

    mbedtls_mpi_mod_raw_cond_swap(X, Y, &m, 1);

    TEST_CF_PUBLIC(X, bytes);
    TEST_CF_PUBLIC(Y, bytes);

    /* Check if the given length is copied even it is smaller
       than the length of the given MPIs. */
    if (copy_limbs < limbs) {
        ASSERT_COMPARE(X, copy_bytes, tmp_Y, copy_bytes);
        ASSERT_COMPARE(Y, copy_bytes, tmp_X, copy_bytes);
        TEST_ASSERT(memcmp(X, tmp_X, bytes) != 0);
        TEST_ASSERT(memcmp(X, tmp_Y, bytes) != 0);
        TEST_ASSERT(memcmp(Y, tmp_X, bytes) != 0);
        TEST_ASSERT(memcmp(Y, tmp_Y, bytes) != 0);
    } else {
        ASSERT_COMPARE(X, bytes, tmp_Y, bytes);
        ASSERT_COMPARE(Y, bytes, tmp_X, bytes);
    }

exit:
    mbedtls_free(tmp_X);
    mbedtls_free(tmp_Y);
    mbedtls_free(X);
    mbedtls_free(Y);

    mbedtls_mpi_mod_modulus_free(&m);
    mbedtls_free(buff_m);
}

static void test_mpi_mod_raw_cond_swap_wrapper( void ** params )
{

    test_mpi_mod_raw_cond_swap( (char *) params[0], (char *) params[1], *( (int *) params[2] ) );
}
#line 273 "suites/test_suite_bignum_mod_raw.function"
static void test_mpi_mod_raw_sub(char *input_A,
                     char *input_B,
                     char *input_N,
                     char *result)
{
    mbedtls_mpi_uint *A = NULL;
    mbedtls_mpi_uint *B = NULL;
    mbedtls_mpi_uint *N = NULL;
    mbedtls_mpi_uint *X = NULL;
    mbedtls_mpi_uint *res = NULL;
    size_t limbs_A;
    size_t limbs_B;
    size_t limbs_N;
    size_t limbs_res;

    mbedtls_mpi_mod_modulus m;
    mbedtls_mpi_mod_modulus_init(&m);

    TEST_EQUAL(mbedtls_test_read_mpi_core(&A,   &limbs_A,   input_A), 0);
    TEST_EQUAL(mbedtls_test_read_mpi_core(&B,   &limbs_B,   input_B), 0);
    TEST_EQUAL(mbedtls_test_read_mpi_core(&N,   &limbs_N,   input_N), 0);
    TEST_EQUAL(mbedtls_test_read_mpi_core(&res, &limbs_res, result), 0);

    size_t limbs = limbs_N;
    size_t bytes = limbs * sizeof(mbedtls_mpi_uint);

    TEST_EQUAL(limbs_A,   limbs);
    TEST_EQUAL(limbs_B,   limbs);
    TEST_EQUAL(limbs_res, limbs);

    ASSERT_ALLOC(X, limbs);

    TEST_EQUAL(mbedtls_mpi_mod_modulus_setup(
                   &m, N, limbs,
                   MBEDTLS_MPI_MOD_REP_MONTGOMERY), 0);

    mbedtls_mpi_mod_raw_sub(X, A, B, &m);
    ASSERT_COMPARE(X, bytes, res, bytes);

    /* alias X to A */
    memcpy(X, A, bytes);
    mbedtls_mpi_mod_raw_sub(X, X, B, &m);
    ASSERT_COMPARE(X, bytes, res, bytes);

    /* alias X to B */
    memcpy(X, B, bytes);
    mbedtls_mpi_mod_raw_sub(X, A, X, &m);
    ASSERT_COMPARE(X, bytes, res, bytes);

    /* A == B: alias A and B */
    if (memcmp(A, B, bytes) == 0) {
        mbedtls_mpi_mod_raw_sub(X, A, A, &m);
        ASSERT_COMPARE(X, bytes, res, bytes);

        /* X, A, B all aliased together */
        memcpy(X, A, bytes);
        mbedtls_mpi_mod_raw_sub(X, X, X, &m);
        ASSERT_COMPARE(X, bytes, res, bytes);
    }
exit:
    mbedtls_free(A);
    mbedtls_free(B);
    mbedtls_free(X);
    mbedtls_free(res);

    mbedtls_mpi_mod_modulus_free(&m);
    mbedtls_free(N);
}

static void test_mpi_mod_raw_sub_wrapper( void ** params )
{

    test_mpi_mod_raw_sub( (char *) params[0], (char *) params[1], (char *) params[2], (char *) params[3] );
}
#if defined(MBEDTLS_TEST_HOOKS)
#line 344 "suites/test_suite_bignum_mod_raw.function"
static void test_mpi_mod_raw_fix_quasi_reduction(char *input_N,
                                     char *input_X,
                                     char *result)
{
    mbedtls_mpi_uint *X = NULL;
    mbedtls_mpi_uint *N = NULL;
    mbedtls_mpi_uint *res = NULL;
    mbedtls_mpi_uint *tmp = NULL;
    size_t limbs_X;
    size_t limbs_N;
    size_t limbs_res;

    mbedtls_mpi_mod_modulus m;
    mbedtls_mpi_mod_modulus_init(&m);

    TEST_EQUAL(mbedtls_test_read_mpi_core(&X,   &limbs_X,   input_X), 0);
    TEST_EQUAL(mbedtls_test_read_mpi_core(&N,   &limbs_N,   input_N), 0);
    TEST_EQUAL(mbedtls_test_read_mpi_core(&res, &limbs_res, result),  0);

    size_t limbs = limbs_N;
    size_t bytes = limbs * sizeof(mbedtls_mpi_uint);

    TEST_EQUAL(limbs_X,   limbs);
    TEST_EQUAL(limbs_res, limbs);

    ASSERT_ALLOC(tmp, limbs);
    memcpy(tmp, X, bytes);

    /* Check that 0 <= X < 2N */
    mbedtls_mpi_uint c = mbedtls_mpi_core_sub(tmp, X, N, limbs);
    TEST_ASSERT(c || mbedtls_mpi_core_lt_ct(tmp, N, limbs));

    TEST_EQUAL(mbedtls_mpi_mod_modulus_setup(
                   &m, N, limbs,
                   MBEDTLS_MPI_MOD_REP_MONTGOMERY), 0);

    mbedtls_mpi_mod_raw_fix_quasi_reduction(X, &m);
    ASSERT_COMPARE(X, bytes, res, bytes);

exit:
    mbedtls_free(X);
    mbedtls_free(res);
    mbedtls_free(tmp);

    mbedtls_mpi_mod_modulus_free(&m);
    mbedtls_free(N);
}

static void test_mpi_mod_raw_fix_quasi_reduction_wrapper( void ** params )
{

    test_mpi_mod_raw_fix_quasi_reduction( (char *) params[0], (char *) params[1], (char *) params[2] );
}
#endif /* MBEDTLS_TEST_HOOKS */
#line 394 "suites/test_suite_bignum_mod_raw.function"
static void test_mpi_mod_raw_mul(char *input_A,
                     char *input_B,
                     char *input_N,
                     char *result)
{
    mbedtls_mpi_uint *A = NULL;
    mbedtls_mpi_uint *B = NULL;
    mbedtls_mpi_uint *N = NULL;
    mbedtls_mpi_uint *X = NULL;
    mbedtls_mpi_uint *R = NULL;
    mbedtls_mpi_uint *T = NULL;
    size_t limbs_A;
    size_t limbs_B;
    size_t limbs_N;
    size_t limbs_R;

    mbedtls_mpi_mod_modulus m;
    mbedtls_mpi_mod_modulus_init(&m);

    TEST_EQUAL(mbedtls_test_read_mpi_core(&A, &limbs_A, input_A), 0);
    TEST_EQUAL(mbedtls_test_read_mpi_core(&B, &limbs_B, input_B), 0);
    TEST_EQUAL(mbedtls_test_read_mpi_core(&N, &limbs_N, input_N), 0);
    TEST_EQUAL(mbedtls_test_read_mpi_core(&R, &limbs_R, result), 0);

    const size_t limbs = limbs_N;
    const size_t bytes = limbs * sizeof(mbedtls_mpi_uint);

    TEST_EQUAL(limbs_A, limbs);
    TEST_EQUAL(limbs_B, limbs);
    TEST_EQUAL(limbs_R, limbs);

    ASSERT_ALLOC(X, limbs);

    TEST_EQUAL(mbedtls_mpi_mod_modulus_setup(
                   &m, N, limbs,
                   MBEDTLS_MPI_MOD_REP_MONTGOMERY), 0);

    const size_t limbs_T = limbs * 2 + 1;
    ASSERT_ALLOC(T, limbs_T);

    mbedtls_mpi_mod_raw_mul(X, A, B, &m, T);
    ASSERT_COMPARE(X, bytes, R, bytes);

    /* alias X to A */
    memcpy(X, A, bytes);
    mbedtls_mpi_mod_raw_mul(X, X, B, &m, T);
    ASSERT_COMPARE(X, bytes, R, bytes);

    /* alias X to B */
    memcpy(X, B, bytes);
    mbedtls_mpi_mod_raw_mul(X, A, X, &m, T);
    ASSERT_COMPARE(X, bytes, R, bytes);

    /* A == B: alias A and B */
    if (memcmp(A, B, bytes) == 0) {
        mbedtls_mpi_mod_raw_mul(X, A, A, &m, T);
        ASSERT_COMPARE(X, bytes, R, bytes);

        /* X, A, B all aliased together */
        memcpy(X, A, bytes);
        mbedtls_mpi_mod_raw_mul(X, X, X, &m, T);
        ASSERT_COMPARE(X, bytes, R, bytes);
    }
    /* A != B: test B * A */
    else {
        mbedtls_mpi_mod_raw_mul(X, B, A, &m, T);
        ASSERT_COMPARE(X, bytes, R, bytes);

        /* B * A: alias X to A */
        memcpy(X, A, bytes);
        mbedtls_mpi_mod_raw_mul(X, B, X, &m, T);
        ASSERT_COMPARE(X, bytes, R, bytes);

        /* B + A: alias X to B */
        memcpy(X, B, bytes);
        mbedtls_mpi_mod_raw_mul(X, X, A, &m, T);
        ASSERT_COMPARE(X, bytes, R, bytes);
    }

exit:
    mbedtls_free(A);
    mbedtls_free(B);
    mbedtls_free(X);
    mbedtls_free(R);
    mbedtls_free(T);

    mbedtls_mpi_mod_modulus_free(&m);
    mbedtls_free(N);
}

static void test_mpi_mod_raw_mul_wrapper( void ** params )
{

    test_mpi_mod_raw_mul( (char *) params[0], (char *) params[1], (char *) params[2], (char *) params[3] );
}
#line 490 "suites/test_suite_bignum_mod_raw.function"
static void test_mpi_mod_raw_inv_prime(char *input_N, char *input_A, char *input_X)
{
    mbedtls_mpi_uint *A = NULL;
    mbedtls_mpi_uint *N = NULL;
    mbedtls_mpi_uint *X = NULL;
    size_t A_limbs, N_limbs, X_limbs;
    mbedtls_mpi_uint *Y = NULL;
    mbedtls_mpi_uint *T = NULL;
    const mbedtls_mpi_uint *R2 = NULL;

    /* Legacy MPIs for computing R2 */
    mbedtls_mpi N_mpi;  /* gets set up manually, aliasing N, so no need to free */
    mbedtls_mpi R2_mpi;
    mbedtls_mpi_init(&R2_mpi);

    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&A, &A_limbs, input_A));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&N, &N_limbs, input_N));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&X, &X_limbs, input_X));
    ASSERT_ALLOC(Y, N_limbs);

    TEST_EQUAL(A_limbs, N_limbs);
    TEST_EQUAL(X_limbs, N_limbs);

    N_mpi.s = 1;
    N_mpi.p = N;
    N_mpi.n = N_limbs;
    TEST_EQUAL(0, mbedtls_mpi_core_get_mont_r2_unsafe(&R2_mpi, &N_mpi));
    TEST_EQUAL(0, mbedtls_mpi_grow(&R2_mpi, N_limbs));
    R2 = R2_mpi.p;

    size_t working_limbs = mbedtls_mpi_mod_raw_inv_prime_working_limbs(N_limbs);

    /* No point exactly duplicating the code in mbedtls_mpi_mod_raw_inv_prime_working_limbs()
     * to see if the output is correct, but we can check that it's in a
     * reasonable range.  The current calculation works out as
     * `1 + N_limbs * (welem + 4)`, where welem is the number of elements in
     * the window (1 << 1 up to 1 << 6).
     */
    size_t min_expected_working_limbs = 1 + N_limbs * 5;
    size_t max_expected_working_limbs = 1 + N_limbs * 68;

    TEST_LE_U(min_expected_working_limbs, working_limbs);
    TEST_LE_U(working_limbs, max_expected_working_limbs);

    /* Should also be at least mbedtls_mpi_core_montmul_working_limbs() */
    TEST_LE_U(mbedtls_mpi_core_montmul_working_limbs(N_limbs),
              working_limbs);

    ASSERT_ALLOC(T, working_limbs);

    mbedtls_mpi_mod_raw_inv_prime(Y, A, N, N_limbs, R2, T);

    TEST_EQUAL(0, memcmp(X, Y, N_limbs * sizeof(mbedtls_mpi_uint)));

    /* Check when output aliased to input */

    mbedtls_mpi_mod_raw_inv_prime(A, A, N, N_limbs, R2, T);

    TEST_EQUAL(0, memcmp(X, A, N_limbs * sizeof(mbedtls_mpi_uint)));

exit:
    mbedtls_free(T);
    mbedtls_free(A);
    mbedtls_free(N);
    mbedtls_free(X);
    mbedtls_free(Y);
    mbedtls_mpi_free(&R2_mpi);
    // R2 doesn't need to be freed as it is only aliasing R2_mpi
    // N_mpi doesn't need to be freed as it is only aliasing N
}

static void test_mpi_mod_raw_inv_prime_wrapper( void ** params )
{

    test_mpi_mod_raw_inv_prime( (char *) params[0], (char *) params[1], (char *) params[2] );
}
#line 571 "suites/test_suite_bignum_mod_raw.function"
static void test_mpi_mod_raw_add(char *input_N,
                     char *input_A, char *input_B,
                     char *input_S)
{
    mbedtls_mpi_uint *A = NULL;
    mbedtls_mpi_uint *B = NULL;
    mbedtls_mpi_uint *S = NULL;
    mbedtls_mpi_uint *N = NULL;
    mbedtls_mpi_uint *X = NULL;
    size_t A_limbs, B_limbs, N_limbs, S_limbs;

    mbedtls_mpi_mod_modulus m;
    mbedtls_mpi_mod_modulus_init(&m);

    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&A, &A_limbs, input_A));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&B, &B_limbs, input_B));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&N, &N_limbs, input_N));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&S, &S_limbs, input_S));

    /* Modulus gives the number of limbs; all inputs must have the same. */
    size_t limbs = N_limbs;
    size_t bytes = limbs * sizeof(*A);

    TEST_EQUAL(A_limbs, limbs);
    TEST_EQUAL(B_limbs, limbs);
    TEST_EQUAL(S_limbs, limbs);

    ASSERT_ALLOC(X, limbs);

    TEST_EQUAL(mbedtls_mpi_mod_modulus_setup(
                   &m, N, limbs,
                   MBEDTLS_MPI_MOD_REP_MONTGOMERY
                   ), 0);

    /* A + B => Correct result */
    mbedtls_mpi_mod_raw_add(X, A, B, &m);
    ASSERT_COMPARE(X, bytes, S, bytes);

    /* A + B: alias X to A => Correct result */
    memcpy(X, A, bytes);
    mbedtls_mpi_mod_raw_add(X, X, B, &m);
    ASSERT_COMPARE(X, bytes, S, bytes);

    /* A + B: alias X to B => Correct result */
    memcpy(X, B, bytes);
    mbedtls_mpi_mod_raw_add(X, A, X, &m);
    ASSERT_COMPARE(X, bytes, S, bytes);

    if (memcmp(A, B, bytes) == 0) {
        /* A == B: alias A and B */

        /* A + A => Correct result */
        mbedtls_mpi_mod_raw_add(X, A, A, &m);
        ASSERT_COMPARE(X, bytes, S, bytes);

        /* A + A: X, A, B all aliased together => Correct result */
        memcpy(X, A, bytes);
        mbedtls_mpi_mod_raw_add(X, X, X, &m);
        ASSERT_COMPARE(X, bytes, S, bytes);
    } else {
        /* A != B: test B + A */

        /* B + A => Correct result */
        mbedtls_mpi_mod_raw_add(X, B, A, &m);
        ASSERT_COMPARE(X, bytes, S, bytes);

        /* B + A: alias X to A => Correct result */
        memcpy(X, A, bytes);
        mbedtls_mpi_mod_raw_add(X, B, X, &m);
        ASSERT_COMPARE(X, bytes, S, bytes);

        /* B + A: alias X to B => Correct result */
        memcpy(X, B, bytes);
        mbedtls_mpi_mod_raw_add(X, X, A, &m);
        ASSERT_COMPARE(X, bytes, S, bytes);
    }

exit:
    mbedtls_mpi_mod_modulus_free(&m);

    mbedtls_free(A);
    mbedtls_free(B);
    mbedtls_free(S);
    mbedtls_free(N);
    mbedtls_free(X);
}

static void test_mpi_mod_raw_add_wrapper( void ** params )
{

    test_mpi_mod_raw_add( (char *) params[0], (char *) params[1], (char *) params[2], (char *) params[3] );
}
#line 662 "suites/test_suite_bignum_mod_raw.function"
static void test_mpi_mod_raw_canonical_to_modulus_rep(const char *input_N, int rep,
                                          const char *input_A,
                                          const char *input_X)
{
    mbedtls_mpi_mod_modulus N;
    mbedtls_mpi_mod_modulus_init(&N);
    mbedtls_mpi_uint *A = NULL;
    size_t A_limbs = 0;;
    mbedtls_mpi_uint *X = NULL;
    size_t X_limbs = 0;

    TEST_EQUAL(0, mbedtls_test_read_mpi_modulus(&N, input_N, rep));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&A, &A_limbs, input_A));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&X, &X_limbs, input_X));

    TEST_EQUAL(0, mbedtls_mpi_mod_raw_canonical_to_modulus_rep(A, &N));
    ASSERT_COMPARE(A, A_limbs * sizeof(mbedtls_mpi_uint),
                   X, X_limbs * sizeof(mbedtls_mpi_uint));

exit:
    mbedtls_test_mpi_mod_modulus_free_with_limbs(&N);
    mbedtls_free(A);
    mbedtls_free(X);
}

static void test_mpi_mod_raw_canonical_to_modulus_rep_wrapper( void ** params )
{

    test_mpi_mod_raw_canonical_to_modulus_rep( (char *) params[0], *( (int *) params[1] ), (char *) params[2], (char *) params[3] );
}
#line 689 "suites/test_suite_bignum_mod_raw.function"
static void test_mpi_mod_raw_modulus_to_canonical_rep(const char *input_N, int rep,
                                          const char *input_A,
                                          const char *input_X)
{
    mbedtls_mpi_mod_modulus N;
    mbedtls_mpi_mod_modulus_init(&N);
    mbedtls_mpi_uint *A = NULL;
    size_t A_limbs = 0;
    mbedtls_mpi_uint *X = NULL;
    size_t X_limbs = 0;

    TEST_EQUAL(0, mbedtls_test_read_mpi_modulus(&N, input_N, rep));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&A, &A_limbs, input_A));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&X, &X_limbs, input_X));

    TEST_EQUAL(0, mbedtls_mpi_mod_raw_modulus_to_canonical_rep(A, &N));
    ASSERT_COMPARE(A, A_limbs * sizeof(mbedtls_mpi_uint),
                   X, X_limbs * sizeof(mbedtls_mpi_uint));

exit:
    mbedtls_test_mpi_mod_modulus_free_with_limbs(&N);
    mbedtls_free(A);
    mbedtls_free(X);
}

static void test_mpi_mod_raw_modulus_to_canonical_rep_wrapper( void ** params )
{

    test_mpi_mod_raw_modulus_to_canonical_rep( (char *) params[0], *( (int *) params[1] ), (char *) params[2], (char *) params[3] );
}
#line 718 "suites/test_suite_bignum_mod_raw.function"
static void test_mpi_mod_raw_to_mont_rep(char *input_N, char *input_A, char *input_X)
{
    mbedtls_mpi_uint *N = NULL;
    mbedtls_mpi_uint *A = NULL;
    mbedtls_mpi_uint *R = NULL; /* for result of low-level conversion */
    mbedtls_mpi_uint *X = NULL;
    mbedtls_mpi_uint *T = NULL;
    size_t n_limbs, a_limbs, x_limbs;

    mbedtls_mpi_mod_modulus m;
    mbedtls_mpi_mod_modulus_init(&m);

    /* Read inputs */
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&N, &n_limbs, input_N));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&A, &a_limbs, input_A));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&X, &x_limbs, input_X));

    /* Number to convert must have same number of limbs as modulus */
    TEST_EQUAL(a_limbs, n_limbs);

    /* Higher-level conversion is in-place, so expected result must have the
     * same number of limbs too */
    TEST_EQUAL(x_limbs, n_limbs);

    size_t limbs = n_limbs;
    size_t bytes = limbs * sizeof(mbedtls_mpi_uint);

    TEST_EQUAL(0, mbedtls_mpi_mod_modulus_setup(&m, N, n_limbs,
                                                MBEDTLS_MPI_MOD_REP_MONTGOMERY));

    /* 1. Test low-level function first */

    /* It has separate output, and requires temporary working storage */
    size_t temp_limbs = mbedtls_mpi_core_montmul_working_limbs(limbs);
    ASSERT_ALLOC(T, temp_limbs);
    ASSERT_ALLOC(R, limbs);
    mbedtls_mpi_core_to_mont_rep(R, A, N, n_limbs,
                                 m.rep.mont.mm, m.rep.mont.rr, T);
    /* Test that the low-level function gives the required value */
    ASSERT_COMPARE(R, bytes, X, bytes);

    /* Test when output is aliased to input */
    memcpy(R, A, bytes);
    mbedtls_mpi_core_to_mont_rep(R, R, N, n_limbs,
                                 m.rep.mont.mm, m.rep.mont.rr, T);
    ASSERT_COMPARE(R, bytes, X, bytes);

    /* 2. Test higher-level cannonical to Montgomery conversion */

    TEST_EQUAL(0, mbedtls_mpi_mod_raw_to_mont_rep(A, &m));

    /* The result matches expected value */
    ASSERT_COMPARE(A, bytes, X, bytes);

exit:
    mbedtls_mpi_mod_modulus_free(&m);
    mbedtls_free(T);
    mbedtls_free(N);
    mbedtls_free(A);
    mbedtls_free(R);
    mbedtls_free(X);
}

static void test_mpi_mod_raw_to_mont_rep_wrapper( void ** params )
{

    test_mpi_mod_raw_to_mont_rep( (char *) params[0], (char *) params[1], (char *) params[2] );
}
#line 783 "suites/test_suite_bignum_mod_raw.function"
static void test_mpi_mod_raw_from_mont_rep(char *input_N, char *input_A, char *input_X)
{
    mbedtls_mpi_uint *N = NULL;
    mbedtls_mpi_uint *A = NULL;
    mbedtls_mpi_uint *R = NULL; /* for result of low-level conversion */
    mbedtls_mpi_uint *X = NULL;
    mbedtls_mpi_uint *T = NULL;
    size_t n_limbs, a_limbs, x_limbs;

    mbedtls_mpi_mod_modulus m;
    mbedtls_mpi_mod_modulus_init(&m);

    /* Read inputs */
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&N, &n_limbs, input_N));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&A, &a_limbs, input_A));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&X, &x_limbs, input_X));

    /* Number to convert must have same number of limbs as modulus */
    TEST_EQUAL(a_limbs, n_limbs);

    /* Higher-level conversion is in-place, so expected result must have the
     * same number of limbs too */
    TEST_EQUAL(x_limbs, n_limbs);

    size_t limbs = n_limbs;
    size_t bytes = limbs * sizeof(mbedtls_mpi_uint);

    TEST_EQUAL(0, mbedtls_mpi_mod_modulus_setup(&m, N, n_limbs,
                                                MBEDTLS_MPI_MOD_REP_MONTGOMERY));

    /* 1. Test low-level function first */

    /* It has separate output, and requires temporary working storage */
    size_t temp_limbs = mbedtls_mpi_core_montmul_working_limbs(limbs);
    ASSERT_ALLOC(T, temp_limbs);
    ASSERT_ALLOC(R, limbs);
    mbedtls_mpi_core_from_mont_rep(R, A, N, n_limbs,
                                   m.rep.mont.mm, T);
    /* Test that the low-level function gives the required value */
    ASSERT_COMPARE(R, bytes, X, bytes);

    /* Test when output is aliased to input */
    memcpy(R, A, bytes);
    mbedtls_mpi_core_from_mont_rep(R, R, N, n_limbs,
                                   m.rep.mont.mm, T);
    ASSERT_COMPARE(R, bytes, X, bytes);

    /* 2. Test higher-level Montgomery to cannonical conversion */

    TEST_EQUAL(0, mbedtls_mpi_mod_raw_from_mont_rep(A, &m));

    /* The result matches expected value */
    ASSERT_COMPARE(A, bytes, X, bytes);

exit:
    mbedtls_mpi_mod_modulus_free(&m);
    mbedtls_free(T);
    mbedtls_free(N);
    mbedtls_free(A);
    mbedtls_free(R);
    mbedtls_free(X);
}

static void test_mpi_mod_raw_from_mont_rep_wrapper( void ** params )
{

    test_mpi_mod_raw_from_mont_rep( (char *) params[0], (char *) params[1], (char *) params[2] );
}
#line 848 "suites/test_suite_bignum_mod_raw.function"
static void test_mpi_mod_raw_neg(char *input_N, char *input_A, char *input_X)
{
    mbedtls_mpi_uint *N = NULL;
    mbedtls_mpi_uint *A = NULL;
    mbedtls_mpi_uint *X = NULL;
    mbedtls_mpi_uint *R = NULL;
    mbedtls_mpi_uint *Z = NULL;
    size_t n_limbs, a_limbs, x_limbs, bytes;

    mbedtls_mpi_mod_modulus m;
    mbedtls_mpi_mod_modulus_init(&m);

    /* Read inputs */
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&N, &n_limbs, input_N));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&A, &a_limbs, input_A));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&X, &x_limbs, input_X));

    TEST_EQUAL(a_limbs, n_limbs);
    TEST_EQUAL(x_limbs, n_limbs);
    bytes = n_limbs * sizeof(mbedtls_mpi_uint);

    ASSERT_ALLOC(R, n_limbs);
    ASSERT_ALLOC(Z, n_limbs);

    TEST_EQUAL(0, mbedtls_mpi_mod_modulus_setup(&m, N, n_limbs,
                                                MBEDTLS_MPI_MOD_REP_MONTGOMERY));

    /* Neg( A == 0 ) => Zero result */
    mbedtls_mpi_mod_raw_neg(R, Z, &m);
    ASSERT_COMPARE(R, bytes, Z, bytes);

    /* Neg( A == N ) => Zero result */
    mbedtls_mpi_mod_raw_neg(R, N, &m);
    ASSERT_COMPARE(R, bytes, Z, bytes);

    /* Neg( A ) => Correct result */
    mbedtls_mpi_mod_raw_neg(R, A, &m);
    ASSERT_COMPARE(R, bytes, X, bytes);

    /* Neg( A ): alias A to R => Correct result */
    mbedtls_mpi_mod_raw_neg(A, A, &m);
    ASSERT_COMPARE(A, bytes, X, bytes);
exit:
    mbedtls_mpi_mod_modulus_free(&m);
    mbedtls_free(N);
    mbedtls_free(A);
    mbedtls_free(X);
    mbedtls_free(R);
    mbedtls_free(Z);
}

static void test_mpi_mod_raw_neg_wrapper( void ** params )
{

    test_mpi_mod_raw_neg( (char *) params[0], (char *) params[1], (char *) params[2] );
}
#endif /* MBEDTLS_BIGNUM_C */


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
    
#if defined(MBEDTLS_BIGNUM_C)

        case 0:
            {
                *out_value = MBEDTLS_MPI_MOD_EXT_REP_BE;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_MPI_MOD_EXT_REP_LE;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_ERR_MPI_BUFFER_TOO_SMALL;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_MPI_MOD_EXT_REP_INVALID;
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
    
#if defined(MBEDTLS_BIGNUM_C)

        case 0:
            {
#if defined(MBEDTLS_HAVE_INT32)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(MBEDTLS_HAVE_INT64)
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

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_raw_io_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_raw_cond_assign_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_raw_cond_swap_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_raw_sub_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_TEST_HOOKS)
    test_mpi_mod_raw_fix_quasi_reduction_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_raw_mul_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_raw_inv_prime_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_raw_add_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_raw_canonical_to_modulus_rep_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_raw_modulus_to_canonical_rep_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_raw_to_mont_rep_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_raw_from_mont_rep_wrapper,
#else
    NULL,
#endif
/* Function Id: 12 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_raw_neg_wrapper,
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
    const char *default_filename = "./test_suite_bignum_mod_raw.datax";
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
