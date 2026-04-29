#line 2 "suites/main_test.function"
/*
 * *** THIS FILE HAS BEEN MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py
 *
 * Test file      : ./test_suite_bignum_core.misc.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_bignum_core.function
 *      Test suite data     : suites/test_suite_bignum_core.misc.data
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
#line 2 "suites/test_suite_bignum_core.function"
#include "mbedtls/bignum.h"
#include "mbedtls/entropy.h"
#include "bignum_core.h"
#include "constant_time_internal.h"
#include "test/constant_flow.h"

/** Verifies mbedtls_mpi_core_add().
 *
 * \param[in] A       Little-endian presentation of the left operand.
 * \param[in] B       Little-endian presentation of the right operand.
 * \param limbs       Number of limbs in each MPI (\p A, \p B, \p S and \p X).
 * \param[in] S       Little-endian presentation of the expected sum.
 * \param carry       Expected carry from the addition.
 * \param[in,out] X   Temporary storage to be used for results.
 *
 * \return  1 if mbedtls_mpi_core_add() passes this test, otherwise 0.
 */
static int mpi_core_verify_add(mbedtls_mpi_uint *A,
                               mbedtls_mpi_uint *B,
                               size_t limbs,
                               mbedtls_mpi_uint *S,
                               int carry,
                               mbedtls_mpi_uint *X)
{
    int ret = 0;

    size_t bytes = limbs * sizeof(*A);

    /* The test cases have A <= B to avoid repetition, so we test A + B then,
     * if A != B, B + A. If A == B, we can test when A and B are aliased */

    /* A + B */

    /* A + B => correct result and carry */
    TEST_EQUAL(carry, mbedtls_mpi_core_add(X, A, B, limbs));
    ASSERT_COMPARE(X, bytes, S, bytes);

    /* A + B; alias output and first operand => correct result and carry */
    memcpy(X, A, bytes);
    TEST_EQUAL(carry, mbedtls_mpi_core_add(X, X, B, limbs));
    ASSERT_COMPARE(X, bytes, S, bytes);

    /* A + B; alias output and second operand => correct result and carry */
    memcpy(X, B, bytes);
    TEST_EQUAL(carry, mbedtls_mpi_core_add(X, A, X, limbs));
    ASSERT_COMPARE(X, bytes, S, bytes);

    if (memcmp(A, B, bytes) == 0) {
        /* A == B, so test where A and B are aliased */

        /* A + A => correct result and carry */
        TEST_EQUAL(carry, mbedtls_mpi_core_add(X, A, A, limbs));
        ASSERT_COMPARE(X, bytes, S, bytes);

        /* A + A, output aliased to both operands => correct result and carry */
        memcpy(X, A, bytes);
        TEST_EQUAL(carry, mbedtls_mpi_core_add(X, X, X, limbs));
        ASSERT_COMPARE(X, bytes, S, bytes);
    } else {
        /* A != B, so test B + A */

        /* B + A => correct result and carry */
        TEST_EQUAL(carry, mbedtls_mpi_core_add(X, B, A, limbs));
        ASSERT_COMPARE(X, bytes, S, bytes);

        /* B + A; alias output and first operand => correct result and carry */
        memcpy(X, B, bytes);
        TEST_EQUAL(carry, mbedtls_mpi_core_add(X, X, A, limbs));
        ASSERT_COMPARE(X, bytes, S, bytes);

        /* B + A; alias output and second operand => correct result and carry */
        memcpy(X, A, bytes);
        TEST_EQUAL(carry, mbedtls_mpi_core_add(X, B, X, limbs));
        ASSERT_COMPARE(X, bytes, S, bytes);
    }

    ret = 1;

exit:
    return ret;
}

/** Verifies mbedtls_mpi_core_add_if().
 *
 * \param[in] A       Little-endian presentation of the left operand.
 * \param[in] B       Little-endian presentation of the right operand.
 * \param limbs       Number of limbs in each MPI (\p A, \p B, \p S and \p X).
 * \param[in] S       Little-endian presentation of the expected sum.
 * \param carry       Expected carry from the addition.
 * \param[in,out] X   Temporary storage to be used for results.
 *
 * \return  1 if mbedtls_mpi_core_add_if() passes this test, otherwise 0.
 */
static int mpi_core_verify_add_if(mbedtls_mpi_uint *A,
                                  mbedtls_mpi_uint *B,
                                  size_t limbs,
                                  mbedtls_mpi_uint *S,
                                  int carry,
                                  mbedtls_mpi_uint *X)
{
    int ret = 0;

    size_t bytes = limbs * sizeof(*A);

    /* The test cases have A <= B to avoid repetition, so we test A + B then,
     * if A != B, B + A. If A == B, we can test when A and B are aliased */

    /* A + B */

    /* cond = 0 => X unchanged, no carry */
    memcpy(X, A, bytes);
    TEST_EQUAL(0, mbedtls_mpi_core_add_if(X, B, limbs, 0));
    ASSERT_COMPARE(X, bytes, A, bytes);

    /* cond = 1 => correct result and carry */
    TEST_EQUAL(carry, mbedtls_mpi_core_add_if(X, B, limbs, 1));
    ASSERT_COMPARE(X, bytes, S, bytes);

    if (memcmp(A, B, bytes) == 0) {
        /* A == B, so test where A and B are aliased */

        /* cond = 0 => X unchanged, no carry */
        memcpy(X, B, bytes);
        TEST_EQUAL(0, mbedtls_mpi_core_add_if(X, X, limbs, 0));
        ASSERT_COMPARE(X, bytes, B, bytes);

        /* cond = 1 => correct result and carry */
        TEST_EQUAL(carry, mbedtls_mpi_core_add_if(X, X, limbs, 1));
        ASSERT_COMPARE(X, bytes, S, bytes);
    } else {
        /* A != B, so test B + A */

        /* cond = 0 => d unchanged, no carry */
        memcpy(X, B, bytes);
        TEST_EQUAL(0, mbedtls_mpi_core_add_if(X, A, limbs, 0));
        ASSERT_COMPARE(X, bytes, B, bytes);

        /* cond = 1 => correct result and carry */
        TEST_EQUAL(carry, mbedtls_mpi_core_add_if(X, A, limbs, 1));
        ASSERT_COMPARE(X, bytes, S, bytes);
    }

    ret = 1;

exit:
    return ret;
}

#line 158 "suites/test_suite_bignum_core.function"
static void test_mpi_core_io_null(void)
{
    mbedtls_mpi_uint X = 0;
    int ret;

    ret = mbedtls_mpi_core_read_be(&X, 1, NULL, 0);
    TEST_EQUAL(ret, 0);
    ret = mbedtls_mpi_core_write_be(&X, 1, NULL, 0);
    TEST_EQUAL(ret, 0);

    ret = mbedtls_mpi_core_read_be(NULL, 0, NULL, 0);
    TEST_EQUAL(ret, 0);
    ret = mbedtls_mpi_core_write_be(NULL, 0, NULL, 0);
    TEST_EQUAL(ret, 0);

    ret = mbedtls_mpi_core_read_le(&X, 1, NULL, 0);
    TEST_EQUAL(ret, 0);
    ret = mbedtls_mpi_core_write_le(&X, 1, NULL, 0);
    TEST_EQUAL(ret, 0);

    ret = mbedtls_mpi_core_read_le(NULL, 0, NULL, 0);
    TEST_EQUAL(ret, 0);
    ret = mbedtls_mpi_core_write_le(NULL, 0, NULL, 0);
    TEST_EQUAL(ret, 0);

exit:
    ;
}

static void test_mpi_core_io_null_wrapper( void ** params )
{
    (void)params;

    test_mpi_core_io_null(  );
}
#line 189 "suites/test_suite_bignum_core.function"
static void test_mpi_core_io_be(data_t *input, int nb_int, int nx_32_int, int iret,
                    int oret)
{
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

    int ret = mbedtls_mpi_core_read_be(X, nx, input->x, input->len);
    TEST_EQUAL(ret, iret);

    if (iret == 0) {
        ret =  mbedtls_mpi_core_write_be(X, nx, buf, nb);
        TEST_EQUAL(ret, oret);
    }

    if ((iret == 0) && (oret == 0)) {
        if (nb > input->len) {
            size_t leading_zeroes = nb - input->len;
            TEST_ASSERT(memcmp(buf + nb - input->len, input->x, input->len) == 0);
            for (size_t i = 0; i < leading_zeroes; i++) {
                TEST_EQUAL(buf[i], 0);
            }
        } else {
            size_t leading_zeroes = input->len - nb;
            TEST_ASSERT(memcmp(input->x + input->len - nb, buf, nb) == 0);
            for (size_t i = 0; i < leading_zeroes; i++) {
                TEST_EQUAL(input->x[i], 0);
            }
        }
    }

exit:
    ;
}

static void test_mpi_core_io_be_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_mpi_core_io_be( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ) );
}
#line 245 "suites/test_suite_bignum_core.function"
static void test_mpi_core_io_le(data_t *input, int nb_int, int nx_32_int, int iret,
                    int oret)
{
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

    int ret =  mbedtls_mpi_core_read_le(X, nx, input->x, input->len);
    TEST_EQUAL(ret, iret);

    if (iret == 0) {
        ret =  mbedtls_mpi_core_write_le(X, nx, buf, nb);
        TEST_EQUAL(ret, oret);
    }

    if ((iret == 0) && (oret == 0)) {
        if (nb > input->len) {
            TEST_ASSERT(memcmp(buf, input->x, input->len) == 0);
            for (size_t i = input->len; i < nb; i++) {
                TEST_EQUAL(buf[i], 0);
            }
        } else {
            TEST_ASSERT(memcmp(input->x, buf, nb) == 0);
            for (size_t i = nb; i < input->len; i++) {
                TEST_EQUAL(input->x[i], 0);
            }
        }
    }

exit:
    ;
}

static void test_mpi_core_io_le_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_mpi_core_io_le( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ) );
}
#line 299 "suites/test_suite_bignum_core.function"
static void test_mpi_core_bitlen(char *input_X, int nr_bits)
{
    mbedtls_mpi_uint *X = NULL;
    size_t limbs;

    TEST_EQUAL(mbedtls_test_read_mpi_core(&X, &limbs, input_X), 0);
    TEST_EQUAL(mbedtls_mpi_core_bitlen(X, limbs), nr_bits);

exit:
    mbedtls_free(X);
}

static void test_mpi_core_bitlen_wrapper( void ** params )
{

    test_mpi_core_bitlen( (char *) params[0], *( (int *) params[1] ) );
}
#line 313 "suites/test_suite_bignum_core.function"
static void test_mpi_core_lt_ct(char *input_X, char *input_Y, int exp_ret)
{
    mbedtls_mpi_uint *X = NULL;
    size_t X_limbs;
    mbedtls_mpi_uint *Y = NULL;
    size_t Y_limbs;
    int ret;

    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&X, &X_limbs, input_X));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&Y, &Y_limbs, input_Y));

    /* We need two same-length limb arrays */
    TEST_EQUAL(X_limbs, Y_limbs);

    TEST_CF_SECRET(X, X_limbs * sizeof(mbedtls_mpi_uint));
    TEST_CF_SECRET(Y, X_limbs * sizeof(mbedtls_mpi_uint));

    ret = mbedtls_mpi_core_lt_ct(X, Y, X_limbs);
    TEST_EQUAL(ret, exp_ret);

exit:
    mbedtls_free(X);
    mbedtls_free(Y);
}

static void test_mpi_core_lt_ct_wrapper( void ** params )
{

    test_mpi_core_lt_ct( (char *) params[0], (char *) params[1], *( (int *) params[2] ) );
}
#line 340 "suites/test_suite_bignum_core.function"
static void test_mpi_core_uint_le_mpi(char *input_A)
{
    mbedtls_mpi_uint *A = NULL;
    size_t A_limbs = 0;

    TEST_EQUAL(mbedtls_test_read_mpi_core(&A, &A_limbs, input_A), 0);

    int is_large = 0; /* nonzero limbs beyond the lowest-order one? */
    for (size_t i = 1; i < A_limbs; i++) {
        if (A[i] != 0) {
            is_large = 1;
            break;
        }
    }

    TEST_CF_SECRET(A, A_limbs * sizeof(*A));

    TEST_EQUAL(mbedtls_mpi_core_uint_le_mpi(0, A, A_limbs), 1);
    TEST_EQUAL(mbedtls_mpi_core_uint_le_mpi(A[0], A, A_limbs), 1);

    if (is_large) {
        TEST_EQUAL(mbedtls_mpi_core_uint_le_mpi(A[0] + 1,
                                                A, A_limbs), 1);
        TEST_EQUAL(mbedtls_mpi_core_uint_le_mpi((mbedtls_mpi_uint) (-1) >> 1,
                                                A, A_limbs), 1);
        TEST_EQUAL(mbedtls_mpi_core_uint_le_mpi((mbedtls_mpi_uint) (-1),
                                                A, A_limbs), 1);
    } else {
        TEST_EQUAL(mbedtls_mpi_core_uint_le_mpi(A[0] + 1,
                                                A, A_limbs),
                   A[0] + 1 <= A[0]);
        TEST_EQUAL(mbedtls_mpi_core_uint_le_mpi((mbedtls_mpi_uint) (-1) >> 1,
                                                A, A_limbs),
                   (mbedtls_mpi_uint) (-1) >> 1 <= A[0]);
        TEST_EQUAL(mbedtls_mpi_core_uint_le_mpi((mbedtls_mpi_uint) (-1),
                                                A, A_limbs),
                   (mbedtls_mpi_uint) (-1) <= A[0]);
    }

exit:
    mbedtls_free(A);
}

static void test_mpi_core_uint_le_mpi_wrapper( void ** params )
{

    test_mpi_core_uint_le_mpi( (char *) params[0] );
}
#line 385 "suites/test_suite_bignum_core.function"
static void test_mpi_core_cond_assign(char *input_X,
                          char *input_Y,
                          int input_bytes)
{
    mbedtls_mpi_uint *X = NULL;
    mbedtls_mpi_uint *Y = NULL;
    size_t limbs_X;
    size_t limbs_Y;

    TEST_EQUAL(mbedtls_test_read_mpi_core(&X, &limbs_X, input_X), 0);
    TEST_EQUAL(mbedtls_test_read_mpi_core(&Y, &limbs_Y, input_Y), 0);

    size_t limbs = limbs_X;
    size_t copy_limbs = CHARS_TO_LIMBS(input_bytes);
    size_t bytes = limbs * sizeof(mbedtls_mpi_uint);
    size_t copy_bytes = copy_limbs * sizeof(mbedtls_mpi_uint);

    TEST_EQUAL(limbs_X, limbs_Y);
    TEST_ASSERT(copy_limbs <= limbs);

    /* condition is false */
    TEST_CF_SECRET(X, bytes);
    TEST_CF_SECRET(Y, bytes);

    mbedtls_mpi_core_cond_assign(X, Y, copy_limbs, 0);

    TEST_CF_PUBLIC(X, bytes);
    TEST_CF_PUBLIC(Y, bytes);

    TEST_ASSERT(memcmp(X, Y, bytes) != 0);

    /* condition is true */
    TEST_CF_SECRET(X, bytes);
    TEST_CF_SECRET(Y, bytes);

    mbedtls_mpi_core_cond_assign(X, Y, copy_limbs, 1);

    TEST_CF_PUBLIC(X, bytes);
    TEST_CF_PUBLIC(Y, bytes);

    /* Check if the given length is copied even it is smaller
       than the length of the given MPIs. */
    if (copy_limbs < limbs) {
        TEST_CF_PUBLIC(X, bytes);
        TEST_CF_PUBLIC(Y, bytes);

        ASSERT_COMPARE(X, copy_bytes, Y, copy_bytes);
        TEST_ASSERT(memcmp(X, Y, bytes) != 0);
    } else {
        ASSERT_COMPARE(X, bytes, Y, bytes);
    }

exit:
    mbedtls_free(X);
    mbedtls_free(Y);
}

static void test_mpi_core_cond_assign_wrapper( void ** params )
{

    test_mpi_core_cond_assign( (char *) params[0], (char *) params[1], *( (int *) params[2] ) );
}
#line 444 "suites/test_suite_bignum_core.function"
static void test_mpi_core_cond_swap(char *input_X,
                        char *input_Y,
                        int input_bytes)
{
    mbedtls_mpi_uint *tmp_X = NULL;
    mbedtls_mpi_uint *tmp_Y = NULL;
    mbedtls_mpi_uint *X = NULL;
    mbedtls_mpi_uint *Y = NULL;
    size_t limbs_X;
    size_t limbs_Y;

    TEST_EQUAL(mbedtls_test_read_mpi_core(&tmp_X, &limbs_X, input_X), 0);
    TEST_EQUAL(mbedtls_test_read_mpi_core(&tmp_Y, &limbs_Y, input_Y), 0);

    size_t limbs = limbs_X;
    size_t copy_limbs = CHARS_TO_LIMBS(input_bytes);
    size_t bytes = limbs * sizeof(mbedtls_mpi_uint);
    size_t copy_bytes = copy_limbs * sizeof(mbedtls_mpi_uint);

    TEST_EQUAL(limbs_X, limbs_Y);
    TEST_ASSERT(copy_limbs <= limbs);

    ASSERT_ALLOC(X, limbs);
    memcpy(X, tmp_X, bytes);

    ASSERT_ALLOC(Y, limbs);
    memcpy(Y, tmp_Y, bytes);

    /* condition is false */
    TEST_CF_SECRET(X, bytes);
    TEST_CF_SECRET(Y, bytes);

    mbedtls_mpi_core_cond_swap(X, Y, copy_limbs, 0);

    TEST_CF_PUBLIC(X, bytes);
    TEST_CF_PUBLIC(Y, bytes);

    ASSERT_COMPARE(X, bytes, tmp_X, bytes);
    ASSERT_COMPARE(Y, bytes, tmp_Y, bytes);

    /* condition is true */
    TEST_CF_SECRET(X, bytes);
    TEST_CF_SECRET(Y, bytes);

    mbedtls_mpi_core_cond_swap(X, Y, copy_limbs, 1);

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
}

static void test_mpi_core_cond_swap_wrapper( void ** params )
{

    test_mpi_core_cond_swap( (char *) params[0], (char *) params[1], *( (int *) params[2] ) );
}
#line 516 "suites/test_suite_bignum_core.function"
static void test_mpi_core_shift_r(char *input, int count, char *result)
{
    mbedtls_mpi_uint *X = NULL;
    mbedtls_mpi_uint *Y = NULL;
    size_t limbs, n;

    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&X, &limbs, input));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&Y, &n, result));
    TEST_EQUAL(limbs, n);

    mbedtls_mpi_core_shift_r(X, limbs, count);
    ASSERT_COMPARE(X, limbs * ciL, Y, limbs * ciL);

exit:
    mbedtls_free(X);
    mbedtls_free(Y);
}

static void test_mpi_core_shift_r_wrapper( void ** params )
{

    test_mpi_core_shift_r( (char *) params[0], *( (int *) params[1] ), (char *) params[2] );
}
#line 536 "suites/test_suite_bignum_core.function"
static void test_mpi_core_add_and_add_if(char *input_A, char *input_B,
                             char *input_S, int carry)
{
    mbedtls_mpi_uint *A = NULL; /* first value to add */
    mbedtls_mpi_uint *B = NULL; /* second value to add */
    mbedtls_mpi_uint *S = NULL; /* expected result */
    mbedtls_mpi_uint *X = NULL; /* destination - the in/out first operand */
    size_t A_limbs, B_limbs, S_limbs;

    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&A, &A_limbs, input_A));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&B, &B_limbs, input_B));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&S, &S_limbs, input_S));

    /* add and add_if expect all operands to be the same length */
    TEST_EQUAL(A_limbs, B_limbs);
    TEST_EQUAL(A_limbs, S_limbs);

    size_t limbs = A_limbs;
    ASSERT_ALLOC(X, limbs);

    TEST_ASSERT(mpi_core_verify_add(A, B, limbs, S, carry, X));
    TEST_ASSERT(mpi_core_verify_add_if(A, B, limbs, S, carry, X));

exit:
    mbedtls_free(A);
    mbedtls_free(B);
    mbedtls_free(S);
    mbedtls_free(X);
}

static void test_mpi_core_add_and_add_if_wrapper( void ** params )
{

    test_mpi_core_add_and_add_if( (char *) params[0], (char *) params[1], (char *) params[2], *( (int *) params[3] ) );
}
#line 568 "suites/test_suite_bignum_core.function"
static void test_mpi_core_sub(char *input_A, char *input_B,
                  char *input_X, int carry)
{
    mbedtls_mpi A, B, X;
    mbedtls_mpi_uint *a = NULL;
    mbedtls_mpi_uint *b = NULL;
    mbedtls_mpi_uint *x = NULL; /* expected */
    mbedtls_mpi_uint *r = NULL; /* result */

    mbedtls_mpi_init(&A);
    mbedtls_mpi_init(&B);
    mbedtls_mpi_init(&X);

    TEST_EQUAL(0, mbedtls_test_read_mpi(&A, input_A));
    TEST_EQUAL(0, mbedtls_test_read_mpi(&B, input_B));
    TEST_EQUAL(0, mbedtls_test_read_mpi(&X, input_X));

    /* All of the inputs are +ve (or zero) */
    TEST_EQUAL(1, A.s);
    TEST_EQUAL(1, B.s);
    TEST_EQUAL(1, X.s);

    /* Get the number of limbs we will need */
    size_t limbs = MAX(A.n, B.n);
    size_t bytes = limbs * sizeof(mbedtls_mpi_uint);

    /* The result shouldn't have more limbs than the longest input */
    TEST_LE_U(X.n, limbs);

    /* Now let's get arrays of mbedtls_mpi_uints, rather than MPI structures */

    /* ASSERT_ALLOC() uses calloc() under the hood, so these do get zeroed */
    ASSERT_ALLOC(a, bytes);
    ASSERT_ALLOC(b, bytes);
    ASSERT_ALLOC(x, bytes);
    ASSERT_ALLOC(r, bytes);

    /* Populate the arrays. As the mbedtls_mpi_uint[]s in mbedtls_mpis (and as
     * processed by mbedtls_mpi_core_sub()) are little endian, we can just
     * copy what we have as long as MSBs are 0 (which they are from ASSERT_ALLOC())
     */
    memcpy(a, A.p, A.n * sizeof(mbedtls_mpi_uint));
    memcpy(b, B.p, B.n * sizeof(mbedtls_mpi_uint));
    memcpy(x, X.p, X.n * sizeof(mbedtls_mpi_uint));

    /* 1a) r = a - b => we should get the correct carry */
    TEST_EQUAL(carry, mbedtls_mpi_core_sub(r, a, b, limbs));

    /* 1b) r = a - b => we should get the correct result */
    ASSERT_COMPARE(r, bytes, x, bytes);

    /* 2 and 3 test "r may be aliased to a or b" */
    /* 2a) r = a; r -= b => we should get the correct carry (use r to avoid clobbering a) */
    memcpy(r, a, bytes);
    TEST_EQUAL(carry, mbedtls_mpi_core_sub(r, r, b, limbs));

    /* 2b) r -= b => we should get the correct result */
    ASSERT_COMPARE(r, bytes, x, bytes);

    /* 3a) r = b; r = a - r => we should get the correct carry (use r to avoid clobbering b) */
    memcpy(r, b, bytes);
    TEST_EQUAL(carry, mbedtls_mpi_core_sub(r, a, r, limbs));

    /* 3b) r = a - b => we should get the correct result */
    ASSERT_COMPARE(r, bytes, x, bytes);

    /* 4 tests "r may be aliased to [...] both" */
    if (A.n == B.n && memcmp(A.p, B.p, bytes) == 0) {
        memcpy(r, b, bytes);
        TEST_EQUAL(carry, mbedtls_mpi_core_sub(r, r, r, limbs));
        ASSERT_COMPARE(r, bytes, x, bytes);
    }

exit:
    mbedtls_free(a);
    mbedtls_free(b);
    mbedtls_free(x);
    mbedtls_free(r);

    mbedtls_mpi_free(&A);
    mbedtls_mpi_free(&B);
    mbedtls_mpi_free(&X);
}

static void test_mpi_core_sub_wrapper( void ** params )
{

    test_mpi_core_sub( (char *) params[0], (char *) params[1], (char *) params[2], *( (int *) params[3] ) );
}
#line 654 "suites/test_suite_bignum_core.function"
static void test_mpi_core_mla(char *input_A, char *input_B, char *input_S,
                  char *input_X4, char *input_cy4,
                  char *input_X8, char *input_cy8)
{
    /* We are testing A += B * s; A, B are MPIs, s is a scalar.
     *
     * However, we encode s as an MPI in the .data file as the test framework
     * currently only supports `int`-typed scalars, and that doesn't cover the
     * full range of `mbedtls_mpi_uint`.
     *
     * We also have the different results for sizeof(mbedtls_mpi_uint) == 4 or 8.
     */
    mbedtls_mpi A, B, S, X4, X8, cy4, cy8;
    mbedtls_mpi_uint *a = NULL;
    mbedtls_mpi_uint *x = NULL;

    mbedtls_mpi_init(&A);
    mbedtls_mpi_init(&B);
    mbedtls_mpi_init(&S);
    mbedtls_mpi_init(&X4);
    mbedtls_mpi_init(&X8);
    mbedtls_mpi_init(&cy4);
    mbedtls_mpi_init(&cy8);

    TEST_EQUAL(0, mbedtls_test_read_mpi(&A, input_A));
    TEST_EQUAL(0, mbedtls_test_read_mpi(&B, input_B));
    TEST_EQUAL(0, mbedtls_test_read_mpi(&S, input_S));
    TEST_EQUAL(0, mbedtls_test_read_mpi(&X4, input_X4));
    TEST_EQUAL(0, mbedtls_test_read_mpi(&cy4, input_cy4));
    TEST_EQUAL(0, mbedtls_test_read_mpi(&X8, input_X8));
    TEST_EQUAL(0, mbedtls_test_read_mpi(&cy8, input_cy8));

    /* The MPI encoding of scalar s must be only 1 limb */
    TEST_EQUAL(1, S.n);

    /* We only need to work with X4 or X8, and cy4 or cy8, depending on sizeof(mbedtls_mpi_uint) */
    mbedtls_mpi *X = (sizeof(mbedtls_mpi_uint) == 4) ? &X4 : &X8;
    mbedtls_mpi *cy = (sizeof(mbedtls_mpi_uint) == 4) ? &cy4 : &cy8;

    /* The carry should only have one limb */
    TEST_EQUAL(1, cy->n);

    /* All of the inputs are +ve (or zero) */
    TEST_EQUAL(1, A.s);
    TEST_EQUAL(1, B.s);
    TEST_EQUAL(1, S.s);
    TEST_EQUAL(1, X->s);
    TEST_EQUAL(1, cy->s);

    /* Get the (max) number of limbs we will need */
    size_t limbs = MAX(A.n, B.n);
    size_t bytes = limbs * sizeof(mbedtls_mpi_uint);

    /* The result shouldn't have more limbs than the longest input */
    TEST_LE_U(X->n, limbs);

    /* Now let's get arrays of mbedtls_mpi_uints, rather than MPI structures */

    /* ASSERT_ALLOC() uses calloc() under the hood, so these do get zeroed */
    ASSERT_ALLOC(a, bytes);
    ASSERT_ALLOC(x, bytes);

    /* Populate the arrays. As the mbedtls_mpi_uint[]s in mbedtls_mpis (and as
     * processed by mbedtls_mpi_core_mla()) are little endian, we can just
     * copy what we have as long as MSBs are 0 (which they are from ASSERT_ALLOC()).
     */
    memcpy(a, A.p, A.n * sizeof(mbedtls_mpi_uint));
    memcpy(x, X->p, X->n * sizeof(mbedtls_mpi_uint));

    /* 1a) A += B * s => we should get the correct carry */
    TEST_EQUAL(mbedtls_mpi_core_mla(a, limbs, B.p, B.n, *S.p), *cy->p);

    /* 1b) A += B * s => we should get the correct result */
    ASSERT_COMPARE(a, bytes, x, bytes);

    if (A.n == B.n && memcmp(A.p, B.p, bytes) == 0) {
        /* Check when A and B are aliased */
        memcpy(a, A.p, A.n * sizeof(mbedtls_mpi_uint));
        TEST_EQUAL(mbedtls_mpi_core_mla(a, limbs, a, limbs, *S.p), *cy->p);
        ASSERT_COMPARE(a, bytes, x, bytes);
    }

exit:
    mbedtls_free(a);
    mbedtls_free(x);

    mbedtls_mpi_free(&A);
    mbedtls_mpi_free(&B);
    mbedtls_mpi_free(&S);
    mbedtls_mpi_free(&X4);
    mbedtls_mpi_free(&X8);
    mbedtls_mpi_free(&cy4);
    mbedtls_mpi_free(&cy8);
}

static void test_mpi_core_mla_wrapper( void ** params )
{

    test_mpi_core_mla( (char *) params[0], (char *) params[1], (char *) params[2], (char *) params[3], (char *) params[4], (char *) params[5], (char *) params[6] );
}
#line 752 "suites/test_suite_bignum_core.function"
static void test_mpi_montg_init(char *input_N, char *input_mm)
{
    mbedtls_mpi N, mm;

    mbedtls_mpi_init(&N);
    mbedtls_mpi_init(&mm);

    TEST_EQUAL(0, mbedtls_test_read_mpi(&N, input_N));
    TEST_EQUAL(0, mbedtls_test_read_mpi(&mm, input_mm));

    /* The MPI encoding of mm should be 1 limb (sizeof(mbedtls_mpi_uint) == 8) or
     * 2 limbs (sizeof(mbedtls_mpi_uint) == 4).
     *
     * The data file contains the expected result for sizeof(mbedtls_mpi_uint) == 8;
     * for sizeof(mbedtls_mpi_uint) == 4 it's just the LSW of this.
     */
    TEST_ASSERT(mm.n == 1  || mm.n == 2);

    /* All of the inputs are +ve (or zero) */
    TEST_EQUAL(1, N.s);
    TEST_EQUAL(1, mm.s);

    /* mbedtls_mpi_core_montmul_init() only returns a result, no error possible */
    mbedtls_mpi_uint result = mbedtls_mpi_core_montmul_init(N.p);

    /* Check we got the correct result */
    TEST_EQUAL(result, mm.p[0]);

exit:
    mbedtls_mpi_free(&N);
    mbedtls_mpi_free(&mm);
}

static void test_mpi_montg_init_wrapper( void ** params )
{

    test_mpi_montg_init( (char *) params[0], (char *) params[1] );
}
#line 787 "suites/test_suite_bignum_core.function"
static void test_mpi_core_montmul(int limbs_AN4, int limbs_B4,
                      int limbs_AN8, int limbs_B8,
                      char *input_A,
                      char *input_B,
                      char *input_N,
                      char *input_X4,
                      char *input_X8)
{
    mbedtls_mpi A, B, N, X4, X8, T, R;

    mbedtls_mpi_init(&A);
    mbedtls_mpi_init(&B);
    mbedtls_mpi_init(&N);
    mbedtls_mpi_init(&X4);      /* expected result, sizeof(mbedtls_mpi_uint) == 4 */
    mbedtls_mpi_init(&X8);      /* expected result, sizeof(mbedtls_mpi_uint) == 8 */
    mbedtls_mpi_init(&T);
    mbedtls_mpi_init(&R);       /* for the result */

    TEST_EQUAL(0, mbedtls_test_read_mpi(&A, input_A));
    TEST_EQUAL(0, mbedtls_test_read_mpi(&B, input_B));
    TEST_EQUAL(0, mbedtls_test_read_mpi(&N, input_N));
    TEST_EQUAL(0, mbedtls_test_read_mpi(&X4, input_X4));
    TEST_EQUAL(0, mbedtls_test_read_mpi(&X8, input_X8));

    mbedtls_mpi *X = (sizeof(mbedtls_mpi_uint) == 4) ? &X4 : &X8;

    int limbs_AN = (sizeof(mbedtls_mpi_uint) == 4) ? limbs_AN4 : limbs_AN8;
    int limbs_B = (sizeof(mbedtls_mpi_uint) == 4) ? limbs_B4 : limbs_B8;

    TEST_LE_U(A.n, (size_t) limbs_AN);
    TEST_LE_U(X->n, (size_t) limbs_AN);
    TEST_LE_U(B.n, (size_t) limbs_B);
    TEST_LE_U(limbs_B, limbs_AN);

    /* All of the inputs are +ve (or zero) */
    TEST_EQUAL(1, A.s);
    TEST_EQUAL(1, B.s);
    TEST_EQUAL(1, N.s);
    TEST_EQUAL(1, X->s);

    TEST_EQUAL(0, mbedtls_mpi_grow(&A, limbs_AN));
    TEST_EQUAL(0, mbedtls_mpi_grow(&N, limbs_AN));
    TEST_EQUAL(0, mbedtls_mpi_grow(X, limbs_AN));
    TEST_EQUAL(0, mbedtls_mpi_grow(&B, limbs_B));

    size_t working_limbs = mbedtls_mpi_core_montmul_working_limbs(limbs_AN);
    TEST_EQUAL(working_limbs, limbs_AN * 2 + 1);
    TEST_EQUAL(0, mbedtls_mpi_grow(&T, working_limbs));

    /* Calculate the Montgomery constant (this is unit tested separately) */
    mbedtls_mpi_uint mm = mbedtls_mpi_core_montmul_init(N.p);

    TEST_EQUAL(0, mbedtls_mpi_grow(&R, limbs_AN));     /* ensure it's got the right number of limbs */

    mbedtls_mpi_core_montmul(R.p, A.p, B.p, B.n, N.p, N.n, mm, T.p);
    size_t bytes = N.n * sizeof(mbedtls_mpi_uint);
    ASSERT_COMPARE(R.p, bytes, X->p, bytes);

    /* The output (R, above) may be aliased to A - use R to save the value of A */

    memcpy(R.p, A.p, bytes);

    mbedtls_mpi_core_montmul(A.p, A.p, B.p, B.n, N.p, N.n, mm, T.p);
    ASSERT_COMPARE(A.p, bytes, X->p, bytes);

    memcpy(A.p, R.p, bytes);    /* restore A */

    /* The output may be aliased to N - use R to save the value of N */

    memcpy(R.p, N.p, bytes);

    mbedtls_mpi_core_montmul(N.p, A.p, B.p, B.n, N.p, N.n, mm, T.p);
    ASSERT_COMPARE(N.p, bytes, X->p, bytes);

    memcpy(N.p, R.p, bytes);

    if (limbs_AN == limbs_B) {
        /* Test when A aliased to B (requires A == B on input values) */
        if (memcmp(A.p, B.p, bytes) == 0) {
            /* Test with A aliased to B and output, since this is permitted -
             * don't bother with yet another test with only A and B aliased */

            mbedtls_mpi_core_montmul(B.p, B.p, B.p, B.n, N.p, N.n, mm, T.p);
            ASSERT_COMPARE(B.p, bytes, X->p, bytes);

            memcpy(B.p, A.p, bytes);    /* restore B from equal value A */
        }

        /* The output may be aliased to B - last test, so we don't save B */

        mbedtls_mpi_core_montmul(B.p, A.p, B.p, B.n, N.p, N.n, mm, T.p);
        ASSERT_COMPARE(B.p, bytes, X->p, bytes);
    }

exit:
    mbedtls_mpi_free(&A);
    mbedtls_mpi_free(&B);
    mbedtls_mpi_free(&N);
    mbedtls_mpi_free(&X4);
    mbedtls_mpi_free(&X8);
    mbedtls_mpi_free(&T);
    mbedtls_mpi_free(&R);
}

static void test_mpi_core_montmul_wrapper( void ** params )
{

    test_mpi_core_montmul( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4], (char *) params[5], (char *) params[6], (char *) params[7], (char *) params[8] );
}
#line 893 "suites/test_suite_bignum_core.function"
static void test_mpi_core_get_mont_r2_unsafe_neg(void)
{
    mbedtls_mpi N, RR;
    mbedtls_mpi_init(&N);
    mbedtls_mpi_init(&RR);
    const char *n = "7ffffffffffffff1";

    /* Test for zero divisor */
    TEST_EQUAL(MBEDTLS_ERR_MPI_DIVISION_BY_ZERO,
               mbedtls_mpi_core_get_mont_r2_unsafe(&RR, &N));

    /* Test for negative input */
    TEST_EQUAL(0, mbedtls_test_read_mpi(&N, n));
    N.s = -1;
    TEST_EQUAL(MBEDTLS_ERR_MPI_NEGATIVE_VALUE,
               mbedtls_mpi_core_get_mont_r2_unsafe(&RR, &N));
    N.s = 1;

exit:
    mbedtls_mpi_free(&N);
    mbedtls_mpi_free(&RR);
}

static void test_mpi_core_get_mont_r2_unsafe_neg_wrapper( void ** params )
{
    (void)params;

    test_mpi_core_get_mont_r2_unsafe_neg(  );
}
#line 918 "suites/test_suite_bignum_core.function"
static void test_mpi_core_get_mont_r2_unsafe(char *input_N,
                                 char *input_RR_X4,
                                 char *input_RR_X8)
{
    mbedtls_mpi N, RR, RR_REF;

    /* Select the appropriate output */
    char *input_rr = (sizeof(mbedtls_mpi_uint) == 4) ? input_RR_X4 : input_RR_X8;

    mbedtls_mpi_init(&N);
    mbedtls_mpi_init(&RR);
    mbedtls_mpi_init(&RR_REF);

    /* Read inputs */
    TEST_EQUAL(0, mbedtls_test_read_mpi(&N, input_N));
    TEST_EQUAL(0, mbedtls_test_read_mpi(&RR_REF, input_rr));

    /* All of the inputs are +ve (or zero) */
    TEST_EQUAL(1, N.s);
    TEST_EQUAL(1, RR_REF.s);

    /* Test valid input */
    TEST_EQUAL(0, mbedtls_mpi_core_get_mont_r2_unsafe(&RR, &N));

    /* Test that the moduli is odd */
    TEST_EQUAL(N.p[0] ^ 1, N.p[0] - 1);

    /* Output is +ve (or zero) */
    TEST_EQUAL(1, RR_REF.s);

    /* rr is updated to a valid pointer */
    TEST_ASSERT(RR.p != NULL);

    /* Calculated rr matches expected value */
    TEST_ASSERT(mbedtls_mpi_cmp_mpi(&RR, &RR_REF) == 0);

exit:
    mbedtls_mpi_free(&N);
    mbedtls_mpi_free(&RR);
    mbedtls_mpi_free(&RR_REF);
}

static void test_mpi_core_get_mont_r2_unsafe_wrapper( void ** params )
{

    test_mpi_core_get_mont_r2_unsafe( (char *) params[0], (char *) params[1], (char *) params[2] );
}
#if defined(MBEDTLS_TEST_HOOKS)
#line 962 "suites/test_suite_bignum_core.function"
static void test_mpi_core_ct_uint_table_lookup(int bitlen, int window_size)
{
    size_t limbs = BITS_TO_LIMBS(bitlen);
    size_t count = ((size_t) 1) << window_size;

    mbedtls_mpi_uint *table = NULL;
    mbedtls_mpi_uint *dest = NULL;

    ASSERT_ALLOC(table, limbs * count);
    ASSERT_ALLOC(dest, limbs);

    /*
     * Fill the table with a unique counter so that differences are easily
     * detected. (And have their relationship to the index relatively non-trivial just
     * to be sure.)
     */
    for (size_t i = 0; i < count * limbs; i++) {
        table[i] = ~i - 1;
    }

    for (size_t i = 0; i < count; i++) {
        mbedtls_mpi_uint *current = table + i * limbs;
        memset(dest, 0x00, limbs * sizeof(*dest));

        /*
         * We shouldn't leak anything through timing.
         * We need to set these in every loop as we need to make the loop
         * variable public for the loop head and the buffers for comparison.
         */
        TEST_CF_SECRET(&i, sizeof(i));
        TEST_CF_SECRET(dest, limbs * sizeof(*dest));
        TEST_CF_SECRET(table, count * limbs * sizeof(*table));

        mbedtls_mpi_core_ct_uint_table_lookup(dest, table, limbs, count, i);

        TEST_CF_PUBLIC(dest, limbs * sizeof(*dest));
        TEST_CF_PUBLIC(table, count * limbs * sizeof(*table));
        ASSERT_COMPARE(dest, limbs * sizeof(*dest),
                       current, limbs * sizeof(*current));
        TEST_CF_PUBLIC(&i, sizeof(i));
    }

exit:
    mbedtls_free(table);
    mbedtls_free(dest);
}

static void test_mpi_core_ct_uint_table_lookup_wrapper( void ** params )
{

    test_mpi_core_ct_uint_table_lookup( *( (int *) params[0] ), *( (int *) params[1] ) );
}
#endif /* MBEDTLS_TEST_HOOKS */
#line 1011 "suites/test_suite_bignum_core.function"
static void test_mpi_core_fill_random(int wanted_bytes_arg, int extra_rng_bytes,
                          int extra_limbs, int before, int expected_ret)
{
    size_t wanted_bytes = wanted_bytes_arg;
    mbedtls_mpi_uint *X = NULL;
    size_t X_limbs = CHARS_TO_LIMBS(wanted_bytes) + extra_limbs;
    size_t rng_bytes = wanted_bytes + extra_rng_bytes;
    unsigned char *rnd_data = NULL;
    mbedtls_test_rnd_buf_info rnd_info = { NULL, rng_bytes, NULL, NULL };
    int ret;

    /* Prepare an RNG with known output, limited to rng_bytes. */
    ASSERT_ALLOC(rnd_data, rng_bytes);
    TEST_EQUAL(0, mbedtls_test_rnd_std_rand(NULL, rnd_data, rng_bytes));
    rnd_info.buf = rnd_data;

    /* Allocate an MPI with room for wanted_bytes plus extra_limbs.
     * extra_limbs may be negative but the total limb count must be positive.
     * Fill the MPI with the byte value in before. */
    TEST_LE_U(1, X_limbs);
    ASSERT_ALLOC(X, X_limbs);
    memset(X, before, X_limbs * sizeof(*X));

    ret = mbedtls_mpi_core_fill_random(X, X_limbs, wanted_bytes,
                                       mbedtls_test_rnd_buffer_rand,
                                       &rnd_info);
    TEST_EQUAL(expected_ret, ret);

    if (expected_ret == 0) {
        /* mbedtls_mpi_core_fill_random is documented to use bytes from the
         * RNG as a big-endian representation of the number. We used an RNG
         * with known output, so check that the output contains the
         * expected value. Bytes above wanted_bytes must be zero. */
        for (size_t i = 0; i < wanted_bytes; i++) {
            mbedtls_test_set_step(i);
            TEST_EQUAL(GET_BYTE(X, i), rnd_data[wanted_bytes - 1 - i]);
        }
        for (size_t i = wanted_bytes; i < X_limbs * ciL; i++) {
            mbedtls_test_set_step(i);
            TEST_EQUAL(GET_BYTE(X, i), 0);
        }
    }

exit:
    mbedtls_free(rnd_data);
    mbedtls_free(X);
}

static void test_mpi_core_fill_random_wrapper( void ** params )
{

    test_mpi_core_fill_random( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ) );
}
#line 1063 "suites/test_suite_bignum_core.function"
static void test_mpi_core_exp_mod(char *input_N, char *input_A,
                      char *input_E, char *input_X)
{
    mbedtls_mpi_uint *A = NULL;
    mbedtls_mpi_uint *E = NULL;
    mbedtls_mpi_uint *N = NULL;
    mbedtls_mpi_uint *X = NULL;
    size_t A_limbs, E_limbs, N_limbs, X_limbs;
    const mbedtls_mpi_uint *R2 = NULL;
    mbedtls_mpi_uint *Y = NULL;
    mbedtls_mpi_uint *T = NULL;
    /* Legacy MPIs for computing R2 */
    mbedtls_mpi N_mpi;
    mbedtls_mpi_init(&N_mpi);
    mbedtls_mpi R2_mpi;
    mbedtls_mpi_init(&R2_mpi);

    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&A, &A_limbs, input_A));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&E, &E_limbs, input_E));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&N, &N_limbs, input_N));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&X, &X_limbs, input_X));
    ASSERT_ALLOC(Y, N_limbs);

    TEST_EQUAL(A_limbs, N_limbs);
    TEST_EQUAL(X_limbs, N_limbs);

    TEST_EQUAL(0, mbedtls_mpi_grow(&N_mpi, N_limbs));
    memcpy(N_mpi.p, N, N_limbs * sizeof(*N));
    N_mpi.n = N_limbs;
    TEST_EQUAL(0,
               mbedtls_mpi_core_get_mont_r2_unsafe(&R2_mpi, &N_mpi));
    TEST_EQUAL(0, mbedtls_mpi_grow(&R2_mpi, N_limbs));
    R2 = R2_mpi.p;

    size_t working_limbs = mbedtls_mpi_core_exp_mod_working_limbs(N_limbs,
                                                                  E_limbs);

    /* No point exactly duplicating the code in mbedtls_mpi_core_exp_mod_working_limbs()
     * to see if the output is correct, but we can check that it's in a
     * reasonable range.  The current calculation works out as
     * `1 + N_limbs * (welem + 3)`, where welem is the number of elements in
     * the window (1 << 1 up to 1 << 6).
     */
    size_t min_expected_working_limbs = 1 + N_limbs * 4;
    size_t max_expected_working_limbs = 1 + N_limbs * 67;

    TEST_LE_U(min_expected_working_limbs, working_limbs);
    TEST_LE_U(working_limbs, max_expected_working_limbs);

    /* Should also be at least mbedtls_mpi_core_montmul_working_limbs() */
    TEST_LE_U(mbedtls_mpi_core_montmul_working_limbs(N_limbs),
              working_limbs);

    ASSERT_ALLOC(T, working_limbs);

    mbedtls_mpi_core_exp_mod(Y, A, N, N_limbs, E, E_limbs, R2, T);

    TEST_EQUAL(0, memcmp(X, Y, N_limbs * sizeof(mbedtls_mpi_uint)));

    /* Check when output aliased to input */

    mbedtls_mpi_core_exp_mod(A, A, N, N_limbs, E, E_limbs, R2, T);

    TEST_EQUAL(0, memcmp(X, A, N_limbs * sizeof(mbedtls_mpi_uint)));

exit:
    mbedtls_free(T);
    mbedtls_free(A);
    mbedtls_free(E);
    mbedtls_free(N);
    mbedtls_free(X);
    mbedtls_free(Y);
    mbedtls_mpi_free(&N_mpi);
    mbedtls_mpi_free(&R2_mpi);
    // R2 doesn't need to be freed as it is only aliasing R2_mpi
}

static void test_mpi_core_exp_mod_wrapper( void ** params )
{

    test_mpi_core_exp_mod( (char *) params[0], (char *) params[1], (char *) params[2], (char *) params[3] );
}
#line 1150 "suites/test_suite_bignum_core.function"
static void test_mpi_core_sub_int(char *input_A, char *input_B,
                      char *input_X, int borrow)
{
    /* We are testing A - b, where A is an MPI and b is a scalar, expecting
     * result X with borrow borrow.  However, for ease of handling we encode b
     * as a 1-limb MPI (B) in the .data file. */

    mbedtls_mpi_uint *A = NULL;
    mbedtls_mpi_uint *B = NULL;
    mbedtls_mpi_uint *X = NULL;
    mbedtls_mpi_uint *R = NULL;
    size_t A_limbs, B_limbs, X_limbs;

    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&A, &A_limbs, input_A));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&B, &B_limbs, input_B));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&X, &X_limbs, input_X));

    /* The MPI encoding of scalar b must be only 1 limb */
    TEST_EQUAL(B_limbs, 1);

    /* The subtraction is fixed-width, so A and X must have the same number of limbs */
    TEST_EQUAL(A_limbs, X_limbs);
    size_t limbs = A_limbs;

    ASSERT_ALLOC(R, limbs);

#define TEST_COMPARE_CORE_MPIS(A, B, limbs) \
    ASSERT_COMPARE(A, (limbs) * sizeof(mbedtls_mpi_uint), B, (limbs) * sizeof(mbedtls_mpi_uint))

    /* 1. R = A - b. Result and borrow should be correct */
    TEST_EQUAL(mbedtls_mpi_core_sub_int(R, A, B[0], limbs), borrow);
    TEST_COMPARE_CORE_MPIS(R, X, limbs);

    /* 2. A = A - b. Result and borrow should be correct */
    TEST_EQUAL(mbedtls_mpi_core_sub_int(A, A, B[0], limbs), borrow);
    TEST_COMPARE_CORE_MPIS(A, X, limbs);

exit:
    mbedtls_free(A);
    mbedtls_free(B);
    mbedtls_free(X);
    mbedtls_free(R);
}

static void test_mpi_core_sub_int_wrapper( void ** params )
{

    test_mpi_core_sub_int( (char *) params[0], (char *) params[1], (char *) params[2], *( (int *) params[3] ) );
}
#line 1196 "suites/test_suite_bignum_core.function"
static void test_mpi_core_check_zero_ct(char *input_X, int expected_is_zero)
{
    mbedtls_mpi_uint *X = NULL;
    size_t X_limbs;

    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&X, &X_limbs, input_X));

    TEST_CF_SECRET(X, X_limbs * sizeof(mbedtls_mpi_uint));

    mbedtls_mpi_uint check = mbedtls_mpi_core_check_zero_ct(X, X_limbs);
    int is_zero = (check == 0);
    TEST_EQUAL(is_zero, expected_is_zero);

exit:
    mbedtls_free(X);
}

static void test_mpi_core_check_zero_ct_wrapper( void ** params )
{

    test_mpi_core_check_zero_ct( (char *) params[0], *( (int *) params[1] ) );
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
                *out_value = MBEDTLS_ERR_MPI_BUFFER_TOO_SMALL;
            }
            break;
        case 1:
            {
                *out_value = -1;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
            }
            break;
        case 4:
            {
                *out_value = -5;
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
    test_mpi_core_io_null_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_core_io_be_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_core_io_le_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_core_bitlen_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_core_lt_ct_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_core_uint_le_mpi_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_core_cond_assign_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_core_cond_swap_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_core_shift_r_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_core_add_and_add_if_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_core_sub_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_core_mla_wrapper,
#else
    NULL,
#endif
/* Function Id: 12 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_montg_init_wrapper,
#else
    NULL,
#endif
/* Function Id: 13 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_core_montmul_wrapper,
#else
    NULL,
#endif
/* Function Id: 14 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_core_get_mont_r2_unsafe_neg_wrapper,
#else
    NULL,
#endif
/* Function Id: 15 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_core_get_mont_r2_unsafe_wrapper,
#else
    NULL,
#endif
/* Function Id: 16 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_TEST_HOOKS)
    test_mpi_core_ct_uint_table_lookup_wrapper,
#else
    NULL,
#endif
/* Function Id: 17 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_core_fill_random_wrapper,
#else
    NULL,
#endif
/* Function Id: 18 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_core_exp_mod_wrapper,
#else
    NULL,
#endif
/* Function Id: 19 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_core_sub_int_wrapper,
#else
    NULL,
#endif
/* Function Id: 20 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_core_check_zero_ct_wrapper,
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
    const char *default_filename = "./test_suite_bignum_core.misc.datax";
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
