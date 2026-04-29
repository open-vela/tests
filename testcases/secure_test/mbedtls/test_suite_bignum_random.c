#line 2 "suites/main_test.function"
/*
 * *** THIS FILE HAS BEEN MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py
 *
 * Test file      : ./test_suite_bignum_random.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_bignum_random.function
 *      Test suite data     : suites/test_suite_bignum_random.data
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
#line 2 "suites/test_suite_bignum_random.function"
/* Dedicated test suite for mbedtls_mpi_core_random() and the upper-layer
 * functions. Due to the complexity of how these functions are tested,
 * we test all the layers in a single test suite, unlike the way other
 * functions are tested with each layer in its own test suite.
 *
 * Test strategy
 * =============
 *
 * There are three main goals for testing random() functions:
 * - Parameter validation.
 * - Correctness of outputs (well-formed, in range).
 * - Distribution of outputs.
 *
 * We test parameter validation in a standard way, with unit tests with
 * positive and negative cases:
 * - mbedtls_mpi_core_random(): negative cases for mpi_core_random_basic.
 * - mbedtls_mpi_mod_raw_random(),  mbedtls_mpi_mod_random(): negative
 *   cases for mpi_mod_random_validation.
 * - mbedtls_mpi_random(): mpi_random_fail.
 *
 * We test the correctness of outputs in positive tests:
 * - mbedtls_mpi_core_random(): positive cases for mpi_core_random_basic,
 *   and mpi_random_many.
 * - mbedtls_mpi_mod_raw_random(), mbedtls_mpi_mod_random(): tested indirectly
 *   via mpi_mod_random_values.
 * - mbedtls_mpi_random(): mpi_random_sizes, plus indirectly via
 *   mpi_random_values.
 *
 * We test the distribution of outputs only for mbedtls_mpi_core_random(),
 * in mpi_random_many, which runs the function multiple times. This also
 * helps in validating the output range, through test cases with a small
 * range where any output out of range would be very likely to lead to a
 * test failure. For the other functions, we validate the distribution
 * indirectly by testing that these functions consume the random generator
 * in the same way as mbedtls_mpi_core_random(). This is done in
 * mpi_mod_random_values and mpi_legacy_random_values.
 */

#include "mbedtls/bignum.h"
#include "mbedtls/entropy.h"
#include "bignum_core.h"
#include "bignum_mod_raw.h"
#include "constant_time_internal.h"

/* This test suite only manipulates non-negative bignums. */
static int sign_is_valid(const mbedtls_mpi *X)
{
    return X->s == 1;
}

/* A common initializer for test functions that should generate the same
 * sequences for reproducibility and good coverage. */
const mbedtls_test_rnd_pseudo_info rnd_pseudo_seed = {
    /* 16-word key */
    { 'T', 'h', 'i', 's', ' ', 'i', 's', ' ',
      'a', ' ', 's', 'e', 'e', 'd', '!', 0 },
    /* 2-word initial state, should be zero */
    0, 0
};

/* Test whether bytes represents (in big-endian base 256) a number b that
 * is significantly above a power of 2. That is, b must not have a long run
 * of unset bits after the most significant bit.
 *
 * Let n be the bit-size of b, i.e. the integer such that 2^n <= b < 2^{n+1}.
 * This function returns 1 if, when drawing a number between 0 and b,
 * the probability that this number is at least 2^n is not negligible.
 * This probability is (b - 2^n) / b and this function checks that this
 * number is above some threshold A. The threshold value is heuristic and
 * based on the needs of mpi_random_many().
 */
static int is_significantly_above_a_power_of_2(data_t *bytes)
{
    const uint8_t *p = bytes->x;
    size_t len = bytes->len;
    unsigned x;

    /* Skip leading null bytes */
    while (len > 0 && p[0] == 0) {
        ++p;
        --len;
    }
    /* 0 is not significantly above a power of 2 */
    if (len == 0) {
        return 0;
    }
    /* Extract the (up to) 2 most significant bytes */
    if (len == 1) {
        x = p[0];
    } else {
        x = (p[0] << 8) | p[1];
    }

    /* Shift the most significant bit of x to position 8 and mask it out */
    while ((x & 0xfe00) != 0) {
        x >>= 1;
    }
    x &= 0x00ff;

    /* At this point, x = floor((b - 2^n) / 2^(n-8)). b is significantly above
     * a power of 2 iff x is significantly above 0 compared to 2^8.
     * Testing x >= 2^4 amounts to picking A = 1/16 in the function
     * description above. */
    return x >= 0x10;
}

#line 116 "suites/test_suite_bignum_random.function"
static void test_mpi_core_random_basic(int min, char *bound_bytes, int expected_ret)
{
    /* Same RNG as in mpi_random_values */
    mbedtls_test_rnd_pseudo_info rnd = rnd_pseudo_seed;
    size_t limbs;
    mbedtls_mpi_uint *lower_bound = NULL;
    mbedtls_mpi_uint *upper_bound = NULL;
    mbedtls_mpi_uint *result = NULL;

    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&upper_bound, &limbs,
                                             bound_bytes));
    ASSERT_ALLOC(lower_bound, limbs);
    lower_bound[0] = min;
    ASSERT_ALLOC(result, limbs);

    TEST_EQUAL(expected_ret,
               mbedtls_mpi_core_random(result, min, upper_bound, limbs,
                                       mbedtls_test_rnd_pseudo_rand, &rnd));

    if (expected_ret == 0) {
        TEST_EQUAL(0, mbedtls_mpi_core_lt_ct(result, lower_bound, limbs));
        TEST_EQUAL(1, mbedtls_mpi_core_lt_ct(result, upper_bound, limbs));
    }

exit:
    mbedtls_free(lower_bound);
    mbedtls_free(upper_bound);
    mbedtls_free(result);
}

static void test_mpi_core_random_basic_wrapper( void ** params )
{

    test_mpi_core_random_basic( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ) );
}
#line 148 "suites/test_suite_bignum_random.function"
static void test_mpi_legacy_random_values(int min, char *max_hex)
{
    /* Same RNG as in mpi_core_random_basic */
    mbedtls_test_rnd_pseudo_info rnd_core = rnd_pseudo_seed;
    mbedtls_test_rnd_pseudo_info rnd_legacy;
    memcpy(&rnd_legacy, &rnd_core, sizeof(rnd_core));
    mbedtls_mpi max_legacy;
    mbedtls_mpi_init(&max_legacy);
    mbedtls_mpi_uint *R_core = NULL;
    mbedtls_mpi R_legacy;
    mbedtls_mpi_init(&R_legacy);

    TEST_EQUAL(0, mbedtls_test_read_mpi(&max_legacy, max_hex));
    size_t limbs = max_legacy.n;
    ASSERT_ALLOC(R_core, limbs);

    /* Call the legacy function and the core function with the same random
     * stream. */
    int core_ret = mbedtls_mpi_core_random(R_core, min, max_legacy.p, limbs,
                                           mbedtls_test_rnd_pseudo_rand,
                                           &rnd_core);
    int legacy_ret = mbedtls_mpi_random(&R_legacy, min, &max_legacy,
                                        mbedtls_test_rnd_pseudo_rand,
                                        &rnd_legacy);

    /* They must return the same status, and, on success, output the
     * same number, with the same limb count. */
    TEST_EQUAL(core_ret, legacy_ret);
    if (core_ret == 0) {
        ASSERT_COMPARE(R_core, limbs * ciL,
                       R_legacy.p, R_legacy.n * ciL);
    }

    /* Also check that they have consumed the RNG in the same way. */
    /* This may theoretically fail on rare platforms with padding in
     * the structure! If this is a problem in practice, change to a
     * field-by-field comparison. */
    ASSERT_COMPARE(&rnd_core, sizeof(rnd_core),
                   &rnd_legacy, sizeof(rnd_legacy));

exit:
    mbedtls_mpi_free(&max_legacy);
    mbedtls_free(R_core);
    mbedtls_mpi_free(&R_legacy);
}

static void test_mpi_legacy_random_values_wrapper( void ** params )
{

    test_mpi_legacy_random_values( *( (int *) params[0] ), (char *) params[1] );
}
#line 196 "suites/test_suite_bignum_random.function"
static void test_mpi_mod_random_values(int min, char *max_hex, int rep)
{
    /* Same RNG as in mpi_core_random_basic */
    mbedtls_test_rnd_pseudo_info rnd_core = rnd_pseudo_seed;
    mbedtls_test_rnd_pseudo_info rnd_mod_raw;
    memcpy(&rnd_mod_raw, &rnd_core, sizeof(rnd_core));
    mbedtls_test_rnd_pseudo_info rnd_mod;
    memcpy(&rnd_mod, &rnd_core, sizeof(rnd_core));
    mbedtls_mpi_uint *R_core = NULL;
    mbedtls_mpi_uint *R_mod_raw = NULL;
    mbedtls_mpi_uint *R_mod_digits = NULL;
    mbedtls_mpi_mod_residue R_mod;
    mbedtls_mpi_mod_modulus N;
    mbedtls_mpi_mod_modulus_init(&N);

    TEST_EQUAL(mbedtls_test_read_mpi_modulus(&N, max_hex, rep), 0);
    ASSERT_ALLOC(R_core, N.limbs);
    ASSERT_ALLOC(R_mod_raw, N.limbs);
    ASSERT_ALLOC(R_mod_digits, N.limbs);
    TEST_EQUAL(mbedtls_mpi_mod_residue_setup(&R_mod, &N,
                                             R_mod_digits, N.limbs),
               0);

    /* Call the core and mod random() functions with the same random stream. */
    int core_ret = mbedtls_mpi_core_random(R_core,
                                           min, N.p, N.limbs,
                                           mbedtls_test_rnd_pseudo_rand,
                                           &rnd_core);
    int mod_raw_ret = mbedtls_mpi_mod_raw_random(R_mod_raw,
                                                 min, &N,
                                                 mbedtls_test_rnd_pseudo_rand,
                                                 &rnd_mod_raw);
    int mod_ret = mbedtls_mpi_mod_random(&R_mod,
                                         min, &N,
                                         mbedtls_test_rnd_pseudo_rand,
                                         &rnd_mod);

    /* They must return the same status, and, on success, output the
     * same number, with the same limb count. */
    TEST_EQUAL(core_ret, mod_raw_ret);
    TEST_EQUAL(core_ret, mod_ret);
    if (core_ret == 0) {
        TEST_EQUAL(mbedtls_mpi_mod_raw_modulus_to_canonical_rep(R_mod_raw, &N),
                   0);
        ASSERT_COMPARE(R_core, N.limbs * ciL,
                       R_mod_raw, N.limbs * ciL);
        TEST_EQUAL(mbedtls_mpi_mod_raw_modulus_to_canonical_rep(R_mod_digits, &N),
                   0);
        ASSERT_COMPARE(R_core, N.limbs * ciL,
                       R_mod_digits, N.limbs * ciL);
    }

    /* Also check that they have consumed the RNG in the same way. */
    /* This may theoretically fail on rare platforms with padding in
     * the structure! If this is a problem in practice, change to a
     * field-by-field comparison. */
    ASSERT_COMPARE(&rnd_core, sizeof(rnd_core),
                   &rnd_mod_raw, sizeof(rnd_mod_raw));
    ASSERT_COMPARE(&rnd_core, sizeof(rnd_core),
                   &rnd_mod, sizeof(rnd_mod));

exit:
    mbedtls_test_mpi_mod_modulus_free_with_limbs(&N);
    mbedtls_free(R_core);
    mbedtls_free(R_mod_raw);
    mbedtls_free(R_mod_digits);
}

static void test_mpi_mod_random_values_wrapper( void ** params )
{

    test_mpi_mod_random_values( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ) );
}
#line 266 "suites/test_suite_bignum_random.function"
static void test_mpi_random_many(int min, char *bound_hex, int iterations)
{
    /* Generate numbers in the range 1..bound-1. Do it iterations times.
     * This function assumes that the value of bound is at least 2 and
     * that iterations is large enough that a one-in-2^iterations chance
     * effectively never occurs.
     */

    data_t bound_bytes = { NULL, 0 };
    mbedtls_mpi_uint *upper_bound = NULL;
    size_t limbs;
    size_t n_bits;
    mbedtls_mpi_uint *result = NULL;
    size_t b;
    /* If upper_bound is small, stats[b] is the number of times the value b
     * has been generated. Otherwise stats[b] is the number of times a
     * value with bit b set has been generated. */
    size_t *stats = NULL;
    size_t stats_len;
    int full_stats;
    size_t i;

    TEST_EQUAL(0, mbedtls_test_read_mpi_core(&upper_bound, &limbs,
                                             bound_hex));
    ASSERT_ALLOC(result, limbs);

    n_bits = mbedtls_mpi_core_bitlen(upper_bound, limbs);
    /* Consider a bound "small" if it's less than 2^5. This value is chosen
     * to be small enough that the probability of missing one value is
     * negligible given the number of iterations. It must be less than
     * 256 because some of the code below assumes that "small" values
     * fit in a byte. */
    if (n_bits <= 5) {
        full_stats = 1;
        stats_len = (uint8_t) upper_bound[0];
    } else {
        full_stats = 0;
        stats_len = n_bits;
    }
    ASSERT_ALLOC(stats, stats_len);

    for (i = 0; i < (size_t) iterations; i++) {
        mbedtls_test_set_step(i);
        TEST_EQUAL(0, mbedtls_mpi_core_random(result,
                                              min, upper_bound, limbs,
                                              mbedtls_test_rnd_std_rand, NULL));

        /* Temporarily use a legacy MPI for analysis, because the
         * necessary auxiliary functions don't exist yet in core. */
        mbedtls_mpi B = { 1, limbs, upper_bound };
        mbedtls_mpi R = { 1, limbs, result };

        TEST_ASSERT(mbedtls_mpi_cmp_mpi(&R, &B) < 0);
        TEST_ASSERT(mbedtls_mpi_cmp_int(&R, min) >= 0);
        if (full_stats) {
            uint8_t value;
            TEST_EQUAL(0, mbedtls_mpi_write_binary(&R, &value, 1));
            TEST_ASSERT(value < stats_len);
            ++stats[value];
        } else {
            for (b = 0; b < n_bits; b++) {
                stats[b] += mbedtls_mpi_get_bit(&R, b);
            }
        }
    }

    if (full_stats) {
        for (b = min; b < stats_len; b++) {
            mbedtls_test_set_step(1000000 + b);
            /* Assert that each value has been reached at least once.
             * This is almost guaranteed if the iteration count is large
             * enough. This is a very crude way of checking the distribution.
             */
            TEST_ASSERT(stats[b] > 0);
        }
    } else {
        bound_bytes.len = limbs * sizeof(mbedtls_mpi_uint);
        ASSERT_ALLOC(bound_bytes.x, bound_bytes.len);
        mbedtls_mpi_core_write_be(upper_bound, limbs,
                                  bound_bytes.x, bound_bytes.len);
        int statistically_safe_all_the_way =
            is_significantly_above_a_power_of_2(&bound_bytes);
        for (b = 0; b < n_bits; b++) {
            mbedtls_test_set_step(1000000 + b);
            /* Assert that each bit has been set in at least one result and
             * clear in at least one result. Provided that iterations is not
             * too small, it would be extremely unlikely for this not to be
             * the case if the results are uniformly distributed.
             *
             * As an exception, the top bit may legitimately never be set
             * if bound is a power of 2 or only slightly above.
             */
            if (statistically_safe_all_the_way || b != n_bits - 1) {
                TEST_ASSERT(stats[b] > 0);
            }
            TEST_ASSERT(stats[b] < (size_t) iterations);
        }
    }

exit:
    mbedtls_free(bound_bytes.x);
    mbedtls_free(upper_bound);
    mbedtls_free(result);
    mbedtls_free(stats);
}

static void test_mpi_random_many_wrapper( void ** params )
{

    test_mpi_random_many( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ) );
}
#line 374 "suites/test_suite_bignum_random.function"
static void test_mpi_random_sizes(int min, data_t *bound_bytes, int nlimbs, int before)
{
    mbedtls_mpi upper_bound;
    mbedtls_mpi result;

    mbedtls_mpi_init(&upper_bound);
    mbedtls_mpi_init(&result);

    if (before != 0) {
        /* Set result to sign(before) * 2^(|before|-1) */
        TEST_ASSERT(mbedtls_mpi_lset(&result, before > 0 ? 1 : -1) == 0);
        if (before < 0) {
            before = -before;
        }
        TEST_ASSERT(mbedtls_mpi_shift_l(&result, before - 1) == 0);
    }

    TEST_EQUAL(0, mbedtls_mpi_grow(&result, nlimbs));
    TEST_EQUAL(0, mbedtls_mpi_read_binary(&upper_bound,
                                          bound_bytes->x, bound_bytes->len));
    TEST_EQUAL(0, mbedtls_mpi_random(&result, min, &upper_bound,
                                     mbedtls_test_rnd_std_rand, NULL));
    TEST_ASSERT(sign_is_valid(&result));
    TEST_ASSERT(mbedtls_mpi_cmp_mpi(&result, &upper_bound) < 0);
    TEST_ASSERT(mbedtls_mpi_cmp_int(&result, min) >= 0);

exit:
    mbedtls_mpi_free(&upper_bound);
    mbedtls_mpi_free(&result);
}

static void test_mpi_random_sizes_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_mpi_random_sizes( *( (int *) params[0] ), &data1, *( (int *) params[3] ), *( (int *) params[4] ) );
}
#line 407 "suites/test_suite_bignum_random.function"
static void test_mpi_mod_random_validation(int min, char *bound_hex,
                               int result_limbs_delta,
                               int expected_ret)
{
    mbedtls_mpi_uint *result_digits = NULL;
    mbedtls_mpi_mod_modulus N;
    mbedtls_mpi_mod_modulus_init(&N);

    TEST_EQUAL(mbedtls_test_read_mpi_modulus(&N, bound_hex,
                                             MBEDTLS_MPI_MOD_REP_OPT_RED),
               0);
    size_t result_limbs = N.limbs + result_limbs_delta;
    ASSERT_ALLOC(result_digits, result_limbs);
    /* Build a reside that might not match the modulus, to test that
     * the library function rejects that as expected. */
    mbedtls_mpi_mod_residue result = { result_digits, result_limbs };

    TEST_EQUAL(mbedtls_mpi_mod_random(&result, min, &N,
                                      mbedtls_test_rnd_std_rand, NULL),
               expected_ret);
    if (expected_ret == 0) {
        /* Success should only be expected when the result has the same
         * size as the modulus, otherwise it's a mistake in the test data. */
        TEST_EQUAL(result_limbs, N.limbs);
        /* Sanity check: check that the result is in range */
        TEST_EQUAL(mbedtls_mpi_core_lt_ct(result_digits, N.p, N.limbs),
                   1);
        /* Check result >= min (changes result) */
        TEST_EQUAL(mbedtls_mpi_core_sub_int(result_digits, result_digits, min,
                                            result_limbs),
                   0);
    }

    /* When the result has the right number of limbs, also test mod_raw
     * (for which this is an unchecked precondition). */
    if (result_limbs_delta == 0) {
        TEST_EQUAL(mbedtls_mpi_mod_raw_random(result_digits, min, &N,
                                              mbedtls_test_rnd_std_rand, NULL),
                   expected_ret);
        if (expected_ret == 0) {
            TEST_EQUAL(mbedtls_mpi_core_lt_ct(result_digits, N.p, N.limbs),
                       1);
            TEST_EQUAL(mbedtls_mpi_core_sub_int(result_digits, result.p, min,
                                                result_limbs),
                       0);
        }
    }

exit:
    mbedtls_test_mpi_mod_modulus_free_with_limbs(&N);
    mbedtls_free(result_digits);
}

static void test_mpi_mod_random_validation_wrapper( void ** params )
{

    test_mpi_mod_random_validation( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), *( (int *) params[3] ) );
}
#line 462 "suites/test_suite_bignum_random.function"
static void test_mpi_random_fail(int min, data_t *bound_bytes, int expected_ret)
{
    mbedtls_mpi upper_bound;
    mbedtls_mpi result;
    int actual_ret;

    mbedtls_mpi_init(&upper_bound);
    mbedtls_mpi_init(&result);

    TEST_EQUAL(0, mbedtls_mpi_read_binary(&upper_bound,
                                          bound_bytes->x, bound_bytes->len));
    actual_ret = mbedtls_mpi_random(&result, min, &upper_bound,
                                    mbedtls_test_rnd_std_rand, NULL);
    TEST_EQUAL(expected_ret, actual_ret);

exit:
    mbedtls_mpi_free(&upper_bound);
    mbedtls_mpi_free(&result);
}

static void test_mpi_random_fail_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_mpi_random_fail( *( (int *) params[0] ), &data1, *( (int *) params[3] ) );
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
                *out_value = MBEDTLS_ERR_MPI_NOT_ACCEPTABLE;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_MPI_MOD_REP_MONTGOMERY;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_MPI_MOD_REP_OPT_RED;
            }
            break;
        case 3:
            {
                *out_value = -1;
            }
            break;
        case 4:
            {
                *out_value = -65;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
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
    test_mpi_core_random_basic_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_legacy_random_values_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_random_values_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_random_many_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_random_sizes_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_mod_random_validation_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_random_fail_wrapper,
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
    const char *default_filename = "./test_suite_bignum_random.datax";
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
