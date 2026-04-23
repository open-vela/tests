#line 2 "suites/main_test.function"
/*
 * *** THIS FILE HAS BEEN MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py
 *
 * Test file      : ./test_suite_bignum_mod.generated.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_bignum_mod.function
 *      Test suite data     : suites/test_suite_bignum_mod.generated.data
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
#line 2 "suites/test_suite_bignum_mod.function"
#include "mbedtls/bignum.h"
#include "mbedtls/entropy.h"
#include "bignum_mod.h"
#include "bignum_mod_raw.h"
#include "constant_time_internal.h"
#include "test/constant_flow.h"

#define TEST_COMPARE_MPI_RESIDUES(a, b) \
    ASSERT_COMPARE((a).p, (a).limbs * sizeof(mbedtls_mpi_uint), \
                   (b).p, (b).limbs * sizeof(mbedtls_mpi_uint))

static int test_read_modulus(mbedtls_mpi_mod_modulus *m,
                             mbedtls_mpi_mod_rep_selector int_rep,
                             char *input)
{
    mbedtls_mpi_uint *p = NULL;
    size_t limbs;

    int ret = mbedtls_test_read_mpi_core(&p, &limbs, input);
    if (ret != 0) {
        return ret;
    }

    return mbedtls_mpi_mod_modulus_setup(m, p, limbs, int_rep);
}

static int test_read_residue(mbedtls_mpi_mod_residue *r,
                             const mbedtls_mpi_mod_modulus *m,
                             char *input,
                             int skip_limbs_and_value_checks)
{
    mbedtls_mpi_uint *p = NULL;
    size_t limbs;

    int ret = mbedtls_test_read_mpi_core(&p, &limbs, input);
    if (ret != 0) {
        return ret;
    }

    if (skip_limbs_and_value_checks) {
        r->p = p;
        r->limbs = limbs;
        return 0;
    }

    /* mbedtls_mpi_mod_residue_setup() checks limbs, and that value < m */
    return mbedtls_mpi_mod_residue_setup(r, m, p, limbs);
}
#line 58 "suites/test_suite_bignum_mod.function"
static void test_mpi_mod_setup(int int_rep, int iret)
{
    #define MLIMBS 8
    mbedtls_mpi_uint mp[MLIMBS];
    mbedtls_mpi_mod_modulus m;
    int ret;

    memset(mp, 0xFF, sizeof(mp));

    mbedtls_mpi_mod_modulus_init(&m);
    ret = mbedtls_mpi_mod_modulus_setup(&m, mp, MLIMBS, int_rep);
    TEST_EQUAL(ret, iret);

    /* Only test if the constants have been set-up  */
    if (ret == 0 && int_rep == MBEDTLS_MPI_MOD_REP_MONTGOMERY) {
        /* Test that the consts have been calculated */
        TEST_ASSERT(m.rep.mont.rr != NULL);
        TEST_ASSERT(m.rep.mont.mm != 0);

    }

    /* Address sanitiser should catch if we try to free mp */
    mbedtls_mpi_mod_modulus_free(&m);

    /* Make sure that the modulus doesn't have reference to mp anymore */
    TEST_ASSERT(m.p != mp);

    /* Only test if the constants have been set-up  */
    if (ret == 0 && int_rep == MBEDTLS_MPI_MOD_REP_MONTGOMERY) {
        /* Verify the data and pointers allocated have been properly wiped */
        TEST_ASSERT(m.rep.mont.rr == NULL);
        TEST_ASSERT(m.rep.mont.mm == 0);
    }
exit:
    /* It should be safe to call an mbedtls free several times */
    mbedtls_mpi_mod_modulus_free(&m);

    #undef MLIMBS
}

static void test_mpi_mod_setup_wrapper( void ** params )
{

    test_mpi_mod_setup( *( (int *) params[0] ), *( (int *) params[1] ) );
}
#line 106 "suites/test_suite_bignum_mod.function"
static void test_mpi_mod_mul(char *input_A,
                 char *input_B,
                 char *input_N,
                 char *result)
{
    mbedtls_mpi_uint *X = NULL;

    mbedtls_mpi_mod_residue rA = { NULL, 0 };
    mbedtls_mpi_mod_residue rB = { NULL, 0 };
    mbedtls_mpi_mod_residue rR = { NULL, 0 };
    mbedtls_mpi_mod_residue rX = { NULL, 0 };

    mbedtls_mpi_mod_modulus m;
    mbedtls_mpi_mod_modulus_init(&m);

    TEST_EQUAL(test_read_modulus(&m, MBEDTLS_MPI_MOD_REP_MONTGOMERY, input_N),
               0);

    TEST_EQUAL(test_read_residue(&rA, &m, input_A, 0), 0);
    TEST_EQUAL(test_read_residue(&rB, &m, input_B, 0), 0);
    TEST_EQUAL(test_read_residue(&rR, &m, result,  0), 0);

    const size_t limbs = m.limbs;
    const size_t bytes = limbs * sizeof(mbedtls_mpi_uint);

    TEST_EQUAL(rA.limbs, limbs);
    TEST_EQUAL(rB.limbs, limbs);
    TEST_EQUAL(rR.limbs, limbs);

    ASSERT_ALLOC(X, limbs);

    TEST_EQUAL(mbedtls_mpi_mod_residue_setup(&rX, &m, X, limbs), 0);

    TEST_EQUAL(mbedtls_mpi_mod_mul(&rX, &rA, &rB, &m), 0);
    ASSERT_COMPARE(rX.p, bytes, rR.p, bytes);

    /* alias X to A */
    memcpy(rX.p, rA.p, bytes);
    TEST_EQUAL(mbedtls_mpi_mod_mul(&rX, &rX, &rB, &m), 0);
    ASSERT_COMPARE(rX.p, bytes, rR.p, bytes);

    /* alias X to B */
    memcpy(rX.p, rB.p, bytes);
    TEST_EQUAL(mbedtls_mpi_mod_mul(&rX, &rA, &rX, &m), 0);
    ASSERT_COMPARE(rX.p, bytes, rR.p, bytes);

    /* A == B: alias A and B */
    if (memcmp(rA.p, rB.p, bytes) == 0) {
        TEST_EQUAL(mbedtls_mpi_mod_mul(&rX, &rA, &rA, &m), 0);
        ASSERT_COMPARE(rX.p, bytes, rR.p, bytes);

        /* X, A, B all aliased together */
        memcpy(rX.p, rA.p, bytes);
        TEST_EQUAL(mbedtls_mpi_mod_mul(&rX, &rX, &rX, &m), 0);
        ASSERT_COMPARE(rX.p, bytes, rR.p, bytes);
    }
    /* A != B: test B * A */
    else {
        TEST_EQUAL(mbedtls_mpi_mod_mul(&rX, &rB, &rA, &m), 0);
        ASSERT_COMPARE(rX.p, bytes, rR.p, bytes);

        /* B * A: alias X to A */
        memcpy(rX.p, rA.p, bytes);
        TEST_EQUAL(mbedtls_mpi_mod_mul(&rX, &rB, &rX, &m), 0);
        ASSERT_COMPARE(rX.p, bytes, rR.p, bytes);

        /* B + A: alias X to B */
        memcpy(rX.p, rB.p, bytes);
        TEST_EQUAL(mbedtls_mpi_mod_mul(&rX, &rX, &rA, &m), 0);
        ASSERT_COMPARE(rX.p, bytes, rR.p, bytes);
    }

exit:
    mbedtls_free(rA.p);
    mbedtls_free(rB.p);
    mbedtls_free(rR.p);
    mbedtls_free(X);
    mbedtls_free((mbedtls_mpi_uint *) m.p);

    mbedtls_mpi_mod_modulus_free(&m);
}

static void test_mpi_mod_mul_wrapper( void ** params )
{

    test_mpi_mod_mul( (char *) params[0], (char *) params[1], (char *) params[2], (char *) params[3] );
}
#line 190 "suites/test_suite_bignum_mod.function"
static void test_mpi_mod_mul_neg(char *input_A,
                     char *input_B,
                     char *input_N,
                     char *result,
                     int exp_ret)
{
    mbedtls_mpi_uint *X = NULL;

    mbedtls_mpi_mod_residue rA = { NULL, 0 };
    mbedtls_mpi_mod_residue rB = { NULL, 0 };
    mbedtls_mpi_mod_residue rR = { NULL, 0 };
    mbedtls_mpi_mod_residue rX = { NULL, 0 };

    mbedtls_mpi_mod_modulus m;
    mbedtls_mpi_mod_modulus_init(&m);

    mbedtls_mpi_mod_modulus fake_m;
    mbedtls_mpi_mod_modulus_init(&fake_m);

    TEST_EQUAL(test_read_modulus(&m, MBEDTLS_MPI_MOD_REP_MONTGOMERY, input_N),
               0);

    TEST_EQUAL(test_read_residue(&rA, &m, input_A, 1), 0);
    TEST_EQUAL(test_read_residue(&rB, &m, input_B, 1), 0);
    TEST_EQUAL(test_read_residue(&rR, &m, result,  1), 0);

    const size_t limbs = m.limbs;

    ASSERT_ALLOC(X, limbs);

    TEST_EQUAL(mbedtls_mpi_mod_residue_setup(&rX, &m, X, limbs), 0);
    rX.limbs = rR.limbs;

    TEST_EQUAL(mbedtls_mpi_mod_mul(&rX, &rA, &rB, &m), exp_ret);

    /* Check when m is not initialized */
    TEST_EQUAL(mbedtls_mpi_mod_mul(&rX, &rA, &rB, &fake_m),
               MBEDTLS_ERR_MPI_BAD_INPUT_DATA);

exit:
    mbedtls_free(rA.p);
    mbedtls_free(rB.p);
    mbedtls_free(rR.p);
    mbedtls_free(X);
    mbedtls_free((mbedtls_mpi_uint *) m.p);

    mbedtls_mpi_mod_modulus_free(&m);
    mbedtls_mpi_mod_modulus_free(&fake_m);
}

static void test_mpi_mod_mul_neg_wrapper( void ** params )
{

    test_mpi_mod_mul_neg( (char *) params[0], (char *) params[1], (char *) params[2], (char *) params[3], *( (int *) params[4] ) );
}
#line 245 "suites/test_suite_bignum_mod.function"
static void test_mpi_mod_sub(char *input_N,
                 char *input_A, char *input_B,
                 char *input_D, int expected_ret)
{
    mbedtls_mpi_mod_residue a = { NULL, 0 };
    mbedtls_mpi_mod_residue b = { NULL, 0 };
    mbedtls_mpi_mod_residue d = { NULL, 0 };
    mbedtls_mpi_mod_residue x = { NULL, 0 };
    mbedtls_mpi_uint *X_raw = NULL;

    mbedtls_mpi_mod_modulus m;
    mbedtls_mpi_mod_modulus_init(&m);

    TEST_EQUAL(0,
               test_read_modulus(&m, MBEDTLS_MPI_MOD_REP_MONTGOMERY, input_N));

    /* test_read_residue() normally checks that inputs have the same number of
     * limbs as the modulus. For negative testing we can ask it to skip this
     * with a non-zero final parameter. */
    TEST_EQUAL(0, test_read_residue(&a, &m, input_A, expected_ret != 0));
    TEST_EQUAL(0, test_read_residue(&b, &m, input_B, expected_ret != 0));
    TEST_EQUAL(0, test_read_residue(&d, &m, input_D, expected_ret != 0));

    size_t limbs = m.limbs;
    size_t bytes = limbs * sizeof(*X_raw);

    if (expected_ret == 0) {
        /* Negative test with too many limbs in output */
        ASSERT_ALLOC(X_raw, limbs + 1);

        x.p = X_raw;
        x.limbs = limbs + 1;
        TEST_EQUAL(MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                   mbedtls_mpi_mod_sub(&x, &a, &b, &m));

        mbedtls_free(X_raw);
        X_raw = NULL;

        /* Negative test with too few limbs in output */
        if (limbs > 1) {
            ASSERT_ALLOC(X_raw, limbs - 1);

            x.p = X_raw;
            x.limbs = limbs - 1;
            TEST_EQUAL(MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                       mbedtls_mpi_mod_sub(&x, &a, &b, &m));

            mbedtls_free(X_raw);
            X_raw = NULL;
        }

        /* Negative testing with too many/too few limbs in a and b is covered by
         * manually-written test cases with expected_ret != 0. */
    }

    ASSERT_ALLOC(X_raw, limbs);

    TEST_EQUAL(0, mbedtls_mpi_mod_residue_setup(&x, &m, X_raw, limbs));

    /* a - b => Correct result, or expected error */
    TEST_EQUAL(expected_ret, mbedtls_mpi_mod_sub(&x, &a, &b, &m));
    if (expected_ret != 0) {
        goto exit;
    }

    TEST_COMPARE_MPI_RESIDUES(x, d);

    /* a - b: alias x to a => Correct result */
    memcpy(x.p, a.p, bytes);
    TEST_EQUAL(0, mbedtls_mpi_mod_sub(&x, &x, &b, &m));
    TEST_COMPARE_MPI_RESIDUES(x, d);

    /* a - b: alias x to b => Correct result */
    memcpy(x.p, b.p, bytes);
    TEST_EQUAL(0, mbedtls_mpi_mod_sub(&x, &a, &x, &m));
    TEST_COMPARE_MPI_RESIDUES(x, d);

    if (memcmp(a.p, b.p, bytes) == 0) {
        /* a == b: alias a and b */

        /* a - a => Correct result */
        TEST_EQUAL(0, mbedtls_mpi_mod_sub(&x, &a, &a, &m));
        TEST_COMPARE_MPI_RESIDUES(x, d);

        /* a - a: x, a, b all aliased together => Correct result */
        memcpy(x.p, a.p, bytes);
        TEST_EQUAL(0, mbedtls_mpi_mod_sub(&x, &x, &x, &m));
        TEST_COMPARE_MPI_RESIDUES(x, d);
    }

exit:
    mbedtls_free((void *) m.p);  /* mbedtls_mpi_mod_modulus_free() sets m.p = NULL */
    mbedtls_mpi_mod_modulus_free(&m);

    mbedtls_free(a.p);
    mbedtls_free(b.p);
    mbedtls_free(d.p);
    mbedtls_free(X_raw);
}

static void test_mpi_mod_sub_wrapper( void ** params )
{

    test_mpi_mod_sub( (char *) params[0], (char *) params[1], (char *) params[2], (char *) params[3], *( (int *) params[4] ) );
}
#line 347 "suites/test_suite_bignum_mod.function"
static void test_mpi_mod_inv_mont(char *input_N,
                      char *input_A, char *input_I,
                      int expected_ret)
{
    mbedtls_mpi_mod_residue a = { NULL, 0 };    /* argument */
    mbedtls_mpi_mod_residue i = { NULL, 0 };    /* expected inverse wrt N */
    mbedtls_mpi_mod_residue x = { NULL, 0 };    /* output */
    mbedtls_mpi_uint *X_raw = NULL;

    mbedtls_mpi_mod_modulus N;
    mbedtls_mpi_mod_modulus_init(&N);

    TEST_EQUAL(0,
               test_read_modulus(&N, MBEDTLS_MPI_MOD_REP_MONTGOMERY, input_N));

    /* test_read_residue() normally checks that inputs have the same number of
     * limbs as the modulus. For negative testing we can ask it to skip this
     * with a non-zero final parameter. */
    TEST_EQUAL(0, test_read_residue(&a, &N, input_A, expected_ret != 0));
    TEST_EQUAL(0, test_read_residue(&i, &N, input_I, expected_ret != 0));

    size_t limbs = N.limbs;
    size_t bytes = limbs * sizeof(*X_raw);

    ASSERT_ALLOC(X_raw, limbs);

    TEST_EQUAL(0, mbedtls_mpi_mod_residue_setup(&x, &N, X_raw, limbs));

    TEST_EQUAL(expected_ret, mbedtls_mpi_mod_inv(&x, &a, &N));
    if (expected_ret == 0) {
        TEST_COMPARE_MPI_RESIDUES(x, i);

        /* a^-1: alias x to a => Correct result */
        memcpy(x.p, a.p, bytes);
        TEST_EQUAL(0, mbedtls_mpi_mod_inv(&x, &x, &N));
        TEST_COMPARE_MPI_RESIDUES(x, i);
    }

exit:
    mbedtls_free((void *) N.p);  /* mbedtls_mpi_mod_modulus_free() sets N.p = NULL */
    mbedtls_mpi_mod_modulus_free(&N);

    mbedtls_free(a.p);
    mbedtls_free(i.p);
    mbedtls_free(X_raw);
}

static void test_mpi_mod_inv_mont_wrapper( void ** params )
{

    test_mpi_mod_inv_mont( (char *) params[0], (char *) params[1], (char *) params[2], *( (int *) params[3] ) );
}
#line 396 "suites/test_suite_bignum_mod.function"
static void test_mpi_mod_inv_non_mont(char *input_N,
                          char *input_A, char *input_I,
                          int expected_ret)
{
    mbedtls_mpi_mod_residue a = { NULL, 0 };    /* argument */
    mbedtls_mpi_mod_residue i = { NULL, 0 };    /* expected inverse wrt N */
    mbedtls_mpi_mod_residue x = { NULL, 0 };    /* output */
    mbedtls_mpi_uint *X_raw = NULL;

    mbedtls_mpi_mod_modulus N;
    mbedtls_mpi_mod_modulus_init(&N);

    TEST_EQUAL(0,
               test_read_modulus(&N, MBEDTLS_MPI_MOD_REP_OPT_RED, input_N));

    /* test_read_residue() normally checks that inputs have the same number of
     * limbs as the modulus. For negative testing we can ask it to skip this
     * with a non-zero final parameter. */
    TEST_EQUAL(0, test_read_residue(&a, &N, input_A, expected_ret != 0));
    TEST_EQUAL(0, test_read_residue(&i, &N, input_I, expected_ret != 0));

    size_t limbs = N.limbs;
    size_t bytes = limbs * sizeof(*X_raw);

    ASSERT_ALLOC(X_raw, limbs);

    TEST_EQUAL(0, mbedtls_mpi_mod_residue_setup(&x, &N, X_raw, limbs));

    TEST_EQUAL(expected_ret, mbedtls_mpi_mod_inv(&x, &a, &N));
    if (expected_ret == 0) {
        TEST_COMPARE_MPI_RESIDUES(x, i);

        /* a^-1: alias x to a => Correct result */
        memcpy(x.p, a.p, bytes);
        TEST_EQUAL(0, mbedtls_mpi_mod_inv(&x, &x, &N));
        TEST_COMPARE_MPI_RESIDUES(x, i);
    }

exit:
    mbedtls_free((void *) N.p);  /* mbedtls_mpi_mod_modulus_free() sets N.p = NULL */
    mbedtls_mpi_mod_modulus_free(&N);

    mbedtls_free(a.p);
    mbedtls_free(i.p);
    mbedtls_free(X_raw);
}

static void test_mpi_mod_inv_non_mont_wrapper( void ** params )
{

    test_mpi_mod_inv_non_mont( (char *) params[0], (char *) params[1], (char *) params[2], *( (int *) params[3] ) );
}
#line 451 "suites/test_suite_bignum_mod.function"
static void test_mpi_mod_add(char *input_N,
                 char *input_A, char *input_B,
                 char *input_S, int expected_ret)
{
    mbedtls_mpi_mod_residue a = { NULL, 0 };
    mbedtls_mpi_mod_residue b = { NULL, 0 };
    mbedtls_mpi_mod_residue s = { NULL, 0 };
    mbedtls_mpi_mod_residue x = { NULL, 0 };
    mbedtls_mpi_uint *X_raw = NULL;

    mbedtls_mpi_mod_modulus m;
    mbedtls_mpi_mod_modulus_init(&m);

    TEST_EQUAL(0,
               test_read_modulus(&m, MBEDTLS_MPI_MOD_REP_MONTGOMERY, input_N));

    /* test_read_residue() normally checks that inputs have the same number of
     * limbs as the modulus. For negative testing we can ask it to skip this
     * with a non-zero final parameter. */
    TEST_EQUAL(0, test_read_residue(&a, &m, input_A, expected_ret != 0));
    TEST_EQUAL(0, test_read_residue(&b, &m, input_B, expected_ret != 0));
    TEST_EQUAL(0, test_read_residue(&s, &m, input_S, expected_ret != 0));

    size_t limbs = m.limbs;
    size_t bytes = limbs * sizeof(*X_raw);

    if (expected_ret == 0) {
        /* Negative test with too many limbs in output */
        ASSERT_ALLOC(X_raw, limbs + 1);

        x.p = X_raw;
        x.limbs = limbs + 1;
        TEST_EQUAL(MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                   mbedtls_mpi_mod_add(&x, &a, &b, &m));

        mbedtls_free(X_raw);
        X_raw = NULL;

        /* Negative test with too few limbs in output */
        if (limbs > 1) {
            ASSERT_ALLOC(X_raw, limbs - 1);

            x.p = X_raw;
            x.limbs = limbs - 1;
            TEST_EQUAL(MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                       mbedtls_mpi_mod_add(&x, &a, &b, &m));

            mbedtls_free(X_raw);
            X_raw = NULL;
        }

        /* Negative testing with too many/too few limbs in a and b is covered by
         * manually-written test cases with oret != 0. */
    }

    /* Allocate correct number of limbs for X_raw */
    ASSERT_ALLOC(X_raw, limbs);

    TEST_EQUAL(0, mbedtls_mpi_mod_residue_setup(&x, &m, X_raw, limbs));

    /* A + B => Correct result or expected error */
    TEST_EQUAL(expected_ret, mbedtls_mpi_mod_add(&x, &a, &b, &m));
    if (expected_ret != 0) {
        goto exit;
    }

    TEST_COMPARE_MPI_RESIDUES(x, s);

    /* a + b: alias x to a => Correct result */
    memcpy(x.p, a.p, bytes);
    TEST_EQUAL(0, mbedtls_mpi_mod_add(&x, &x, &b, &m));
    TEST_COMPARE_MPI_RESIDUES(x, s);

    /* a + b: alias x to b => Correct result */
    memcpy(x.p, b.p, bytes);
    TEST_EQUAL(0, mbedtls_mpi_mod_add(&x, &a, &x, &m));
    TEST_COMPARE_MPI_RESIDUES(x, s);

    if (memcmp(a.p, b.p, bytes) == 0) {
        /* a == b: alias a and b */

        /* a + a => Correct result */
        TEST_EQUAL(0, mbedtls_mpi_mod_add(&x, &a, &a, &m));
        TEST_COMPARE_MPI_RESIDUES(x, s);

        /* a + a: x, a, b all aliased together => Correct result */
        memcpy(x.p, a.p, bytes);
        TEST_EQUAL(0, mbedtls_mpi_mod_add(&x, &x, &x, &m));
        TEST_COMPARE_MPI_RESIDUES(x, s);
    }

exit:
    mbedtls_free((void *) m.p);  /* mbedtls_mpi_mod_modulus_free() sets m.p = NULL */
    mbedtls_mpi_mod_modulus_free(&m);

    mbedtls_free(a.p);
    mbedtls_free(b.p);
    mbedtls_free(s.p);
    mbedtls_free(X_raw);
}

static void test_mpi_mod_add_wrapper( void ** params )
{

    test_mpi_mod_add( (char *) params[0], (char *) params[1], (char *) params[2], (char *) params[3], *( (int *) params[4] ) );
}
#line 560 "suites/test_suite_bignum_mod.function"
static void test_mpi_residue_setup(char *input_N, char *input_R, int ret)
{
    mbedtls_mpi_uint *N = NULL;
    mbedtls_mpi_uint *R = NULL;
    size_t n_limbs, r_limbs;
    mbedtls_mpi_mod_modulus m;
    mbedtls_mpi_mod_residue r;

    mbedtls_mpi_mod_modulus_init(&m);

    /* Allocate the memory for intermediate data structures */
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&N, &n_limbs, input_N));
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&R, &r_limbs, input_R));

    TEST_EQUAL(0, mbedtls_mpi_mod_modulus_setup(&m, N, n_limbs,
                                                MBEDTLS_MPI_MOD_REP_MONTGOMERY));

    TEST_EQUAL(ret, mbedtls_mpi_mod_residue_setup(&r, &m, R, r_limbs));

    if (ret == 0) {
        TEST_EQUAL(r.limbs, r_limbs);
        TEST_ASSERT(r.p == R);
    }

exit:
    mbedtls_mpi_mod_modulus_free(&m);
    mbedtls_free(N);
    mbedtls_free(R);
}

static void test_mpi_residue_setup_wrapper( void ** params )
{

    test_mpi_residue_setup( (char *) params[0], (char *) params[1], *( (int *) params[2] ) );
}
#line 592 "suites/test_suite_bignum_mod.function"
static void test_mpi_mod_io_neg(char *input_N, data_t *buf, int ret)
{
    mbedtls_mpi_uint *N = NULL;
    mbedtls_mpi_uint *R = NULL;

    mbedtls_mpi_mod_modulus m;
    mbedtls_mpi_mod_residue r = { NULL, 0 };
    mbedtls_mpi_mod_ext_rep endian = MBEDTLS_MPI_MOD_EXT_REP_LE;

    mbedtls_mpi_mod_modulus_init(&m);

    size_t n_limbs;
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&N, &n_limbs, input_N));
    size_t r_limbs = n_limbs;
    ASSERT_ALLOC(R, r_limbs);

    /* modulus->p == NULL || residue->p == NULL ( m has not been set-up ) */
    TEST_EQUAL(MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
               mbedtls_mpi_mod_read(&r, &m, buf->x, buf->len, endian));

    TEST_EQUAL(MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
               mbedtls_mpi_mod_write(&r, &m, buf->x, buf->len, endian));

    /* Set up modulus and test with residue->p == NULL */
    TEST_EQUAL(0, mbedtls_mpi_mod_modulus_setup(&m, N, n_limbs,
                                                MBEDTLS_MPI_MOD_REP_MONTGOMERY));

    TEST_EQUAL(MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
               mbedtls_mpi_mod_read(&r, &m, buf->x, buf->len, endian));
    TEST_EQUAL(MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
               mbedtls_mpi_mod_write(&r, &m, buf->x, buf->len, endian));

    /* Do the rest of the tests with a residue set up with the input data */
    TEST_EQUAL(0, mbedtls_mpi_mod_residue_setup(&r, &m, R, r_limbs));

    /* Fail for r_limbs < m->limbs */
    r.limbs--;
    TEST_ASSERT(r.limbs < m.limbs);
    TEST_EQUAL(MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
               mbedtls_mpi_mod_read(&r, &m, buf->x, buf->len, endian));
    TEST_EQUAL(MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
               mbedtls_mpi_mod_write(&r, &m, buf->x, buf->len, endian));
    r.limbs++;

    /* Fail for r_limbs > m->limbs */
    m.limbs--;
    TEST_ASSERT(r.limbs > m.limbs);
    TEST_EQUAL(MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
               mbedtls_mpi_mod_read(&r, &m, buf->x, buf->len, endian));
    TEST_EQUAL(MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
               mbedtls_mpi_mod_write(&r, &m, buf->x, buf->len, endian));
    m.limbs++;

    /* Test the read */
    TEST_EQUAL(ret, mbedtls_mpi_mod_read(&r, &m, buf->x, buf->len, endian));

    /* Test write overflow only when the representation is large and read is successful  */
    if (r.limbs > 1 && ret == 0) {
        TEST_EQUAL(MBEDTLS_ERR_MPI_BUFFER_TOO_SMALL,
                   mbedtls_mpi_mod_write(&r, &m, buf->x, 1, endian));
    }

exit:
    mbedtls_mpi_mod_residue_release(&r);
    mbedtls_mpi_mod_modulus_free(&m);
    mbedtls_free(N);
    mbedtls_free(R);
}

static void test_mpi_mod_io_neg_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_mpi_mod_io_neg( (char *) params[0], &data1, *( (int *) params[3] ) );
}
#line 663 "suites/test_suite_bignum_mod.function"
static void test_mpi_mod_io(char *input_N, data_t *input_A, int endian)
{
    mbedtls_mpi_uint *N = NULL;
    mbedtls_mpi_uint *R = NULL;
    mbedtls_mpi_uint *R_COPY = NULL;
    unsigned char *obuf = NULL;
    unsigned char *ref_buf = NULL;
    mbedtls_mpi_mod_modulus m;
    mbedtls_mpi_mod_residue r;
    mbedtls_mpi_mod_residue r_copy;
    size_t n_limbs, n_bytes, a_bytes;

    mbedtls_mpi_mod_modulus_init(&m);

    /* Read inputs */
    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&N, &n_limbs, input_N));
    n_bytes = n_limbs * sizeof(mbedtls_mpi_uint);
    a_bytes = input_A->len;

    /* Allocate the memory for intermediate data structures */
    ASSERT_ALLOC(R, n_bytes);
    ASSERT_ALLOC(R_COPY, n_bytes);

    /* Test that input's size is not greater to modulo's */
    TEST_LE_U(a_bytes, n_bytes);

    /* Init Structures */
    TEST_EQUAL(0, mbedtls_mpi_mod_modulus_setup(&m, N, n_limbs,
                                                MBEDTLS_MPI_MOD_REP_MONTGOMERY));

    /* Enforcing p_limbs >= m->limbs */
    TEST_EQUAL(0, mbedtls_mpi_mod_residue_setup(&r, &m, R, n_limbs));

    TEST_EQUAL(0, mbedtls_mpi_mod_read(&r, &m, input_A->x, input_A->len,
                                       endian));

    /* Read a copy for checking that writing didn't change the value of r */
    TEST_EQUAL(0, mbedtls_mpi_mod_residue_setup(&r_copy, &m,
                                                R_COPY, n_limbs));
    TEST_EQUAL(0, mbedtls_mpi_mod_read(&r_copy, &m, input_A->x, input_A->len,
                                       endian));

    /* Get number of bytes without leading zeroes */
    size_t a_bytes_trimmed = a_bytes;
    while (a_bytes_trimmed > 0) {
        unsigned char *r_byte_array = (unsigned char *) r.p;
        if (r_byte_array[--a_bytes_trimmed] != 0) {
            break;
        }
    }
    a_bytes_trimmed++;

    /* Test write with three output buffer sizes: tight, same as input and
     * longer than the input */
    size_t obuf_sizes[3];
    const size_t obuf_sizes_len = sizeof(obuf_sizes) / sizeof(obuf_sizes[0]);
    obuf_sizes[0] = a_bytes_trimmed;
    obuf_sizes[1] = a_bytes;
    obuf_sizes[2] = a_bytes + 8;

    for (size_t i = 0; i < obuf_sizes_len; i++) {
        ASSERT_ALLOC(obuf, obuf_sizes[i]);
        TEST_EQUAL(0, mbedtls_mpi_mod_write(&r, &m, obuf, obuf_sizes[i], endian));

        /* Make sure that writing didn't corrupt the value of r */
        ASSERT_COMPARE(r.p, r.limbs, r_copy.p, r_copy.limbs);

        /* Set up reference output for checking the result */
        ASSERT_ALLOC(ref_buf, obuf_sizes[i]);
        switch (endian) {
            case MBEDTLS_MPI_MOD_EXT_REP_LE:
                memcpy(ref_buf, input_A->x, a_bytes_trimmed);
                break;
            case MBEDTLS_MPI_MOD_EXT_REP_BE:
            {
                size_t a_offset = input_A->len - a_bytes_trimmed;
                size_t ref_offset = obuf_sizes[i] - a_bytes_trimmed;
                memcpy(ref_buf + ref_offset, input_A->x + a_offset,
                       a_bytes_trimmed);
            }
            break;
            default:
                TEST_ASSERT(0);
        }

        /* Check the result */
        ASSERT_COMPARE(obuf, obuf_sizes[i], ref_buf, obuf_sizes[i]);

        mbedtls_free(ref_buf);
        ref_buf = NULL;
        mbedtls_free(obuf);
        obuf = NULL;
    }

exit:
    mbedtls_mpi_mod_modulus_free(&m);
    mbedtls_free(N);
    mbedtls_free(R);
    mbedtls_free(R_COPY);
    mbedtls_free(obuf);
}

static void test_mpi_mod_io_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_mpi_mod_io( (char *) params[0], &data1, *( (int *) params[3] ) );
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
    test_mpi_mod_setup_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_mul_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_mul_neg_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_sub_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_inv_mont_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_inv_non_mont_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_add_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_residue_setup_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_io_neg_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_io_wrapper,
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
    const char *default_filename = "./test_suite_bignum_mod.generated.datax";
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
