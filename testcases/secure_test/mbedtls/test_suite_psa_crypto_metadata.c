#line 2 "suites/main_test.function"
/*
 * *** THIS FILE HAS BEEN MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py
 *
 * Test file      : ./test_suite_psa_crypto_metadata.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_psa_crypto_metadata.function
 *      Test suite data     : suites/test_suite_psa_crypto_metadata.data
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

#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)
#line 2 "suites/test_suite_psa_crypto_metadata.function"
/* Test macros that provide metadata about algorithms and key types.
 * This test suite only contains tests that don't require executing
 * code. Other test suites validate macros that require creating a key
 * and using it. */

#if defined(MBEDTLS_PSA_CRYPTO_SPM)
#include "spm/psa_defs.h"
#endif

#include "psa/crypto.h"
#include "psa_crypto_invasive.h"

/* Flags for algorithm classification macros. There is a flag for every
 * algorithm classification macro PSA_ALG_IS_xxx except for the
 * category test macros, which are hard-coded in each
 * category-specific function. The name of the flag is the name of the
 * classification macro without the PSA_ prefix. */
#define ALG_IS_VENDOR_DEFINED           (1u << 0)
#define ALG_IS_HMAC                     (1u << 1)
#define ALG_IS_BLOCK_CIPHER_MAC         (1u << 2)
#define ALG_IS_STREAM_CIPHER            (1u << 3)
#define ALG_IS_RSA_PKCS1V15_SIGN        (1u << 4)
#define ALG_IS_RSA_PSS                  (1u << 5)
#define ALG_IS_RSA_PSS_ANY_SALT         (1u << 6)
#define ALG_IS_RSA_PSS_STANDARD_SALT    (1u << 7)
#define ALG_IS_DSA                      (1u << 8)
#define ALG_DSA_IS_DETERMINISTIC        (1u << 9)
#define ALG_IS_DETERMINISTIC_DSA        (1u << 10)
#define ALG_IS_RANDOMIZED_DSA           (1u << 11)
#define ALG_IS_ECDSA                    (1u << 12)
#define ALG_ECDSA_IS_DETERMINISTIC      (1u << 13)
#define ALG_IS_DETERMINISTIC_ECDSA      (1u << 14)
#define ALG_IS_RANDOMIZED_ECDSA         (1u << 15)
#define ALG_IS_HASH_EDDSA               (1u << 16)
#define ALG_IS_SIGN_HASH                (1u << 17)
#define ALG_IS_HASH_AND_SIGN            (1u << 18)
#define ALG_IS_RSA_OAEP                 (1u << 19)
#define ALG_IS_HKDF                     (1u << 20)
#define ALG_IS_HKDF_EXTRACT             (1u << 21)
#define ALG_IS_HKDF_EXPAND              (1u << 22)
#define ALG_IS_FFDH                     (1u << 23)
#define ALG_IS_ECDH                     (1u << 24)
#define ALG_IS_WILDCARD                 (1u << 25)
#define ALG_IS_RAW_KEY_AGREEMENT        (1u << 26)
#define ALG_IS_AEAD_ON_BLOCK_CIPHER     (1u << 27)
#define ALG_IS_TLS12_PRF                (1u << 28)
#define ALG_IS_TLS12_PSK_TO_MS          (1u << 29)
#define ALG_FLAG_MASK_PLUS_ONE          (1u << 30)   /* must be last! */

/* Flags for key type classification macros. There is a flag for every
 * key type classification macro PSA_KEY_TYPE_IS_xxx except for some that
 * are tested as derived from other macros. The name of the flag is
 * the name of the classification macro without the PSA_ prefix. */
#define KEY_TYPE_IS_VENDOR_DEFINED      (1u << 0)
#define KEY_TYPE_IS_UNSTRUCTURED        (1u << 1)
#define KEY_TYPE_IS_PUBLIC_KEY          (1u << 2)
#define KEY_TYPE_IS_KEY_PAIR            (1u << 3)
#define KEY_TYPE_IS_RSA                 (1u << 4)
#define KEY_TYPE_IS_DSA                 (1u << 5)
#define KEY_TYPE_IS_ECC                 (1u << 6)
#define KEY_TYPE_IS_DH                  (1u << 7)
#define KEY_TYPE_FLAG_MASK_PLUS_ONE     (1u << 8)   /* must be last! */

/* Flags for lifetime classification macros. There is a flag for every
 * lifetime classification macro PSA_KEY_LIFETIME_IS_xxx. The name of the
 * flag is the name of the classification macro without the PSA_ prefix. */
#define KEY_LIFETIME_IS_VOLATILE        (1u << 0)
#define KEY_LIFETIME_IS_READ_ONLY       (1u << 1)
#define KEY_LIFETIME_FLAG_MASK_PLUS_ONE (1u << 2)   /* must be last! */

/* Check that in the value of flags, the bit flag (which should be a macro
 * expanding to a number of the form 1 << k) is set if and only if
 * PSA_##flag(alg) is true.
 *
 * Only perform this check if cond is true. Typically cond is 1, but it can
 * be different if the value of the flag bit is only specified under specific
 * conditions.
 *
 * Unconditionally mask flag into the ambient variable
 * classification_flags_tested.
 */
#define TEST_CLASSIFICATION_MACRO(cond, flag, alg, flags)     \
    do                                                          \
    {                                                           \
        if (cond)                                              \
        {                                                       \
            if ((flags) & (flag))                          \
            TEST_ASSERT(PSA_##flag(alg));               \
            else                                                \
            TEST_ASSERT(!PSA_##flag(alg));             \
        }                                                       \
        classification_flags_tested |= (flag);                \
    }                                                           \
    while (0)

/* Check the parity of value.
 *
 * There are several numerical encodings for which the PSA Cryptography API
 * specification deliberately defines encodings that all have the same
 * parity. This way, a data glitch that flips one bit in the data cannot
 * possibly turn a valid encoding into another valid encoding. Here in
 * the tests, we check that the values (including Mbed TLS vendor-specific
 * values) have the expected parity.
 *
 * The expected parity is even so that 0 is considered a valid encoding.
 *
 * Return a nonzero value if value has even parity and 0 otherwise. */
int has_even_parity(uint32_t value)
{
    value ^= value >> 16;
    value ^= value >> 8;
    value ^= value >> 4;
    return 0x9669 & 1 << (value & 0xf);
}
#define TEST_PARITY(value)                    \
    TEST_ASSERT(has_even_parity(value))

static void algorithm_classification(psa_algorithm_t alg, unsigned flags)
{
    unsigned classification_flags_tested = 0;
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_VENDOR_DEFINED, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_HMAC, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_BLOCK_CIPHER_MAC, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_STREAM_CIPHER, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_RSA_PKCS1V15_SIGN, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_RSA_PSS, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_RSA_PSS_ANY_SALT, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_RSA_PSS_STANDARD_SALT, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_DSA, alg, flags);
    TEST_CLASSIFICATION_MACRO(PSA_ALG_IS_DSA(alg),
                              ALG_DSA_IS_DETERMINISTIC, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_DETERMINISTIC_DSA, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_RANDOMIZED_DSA, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_ECDSA, alg, flags);
    TEST_CLASSIFICATION_MACRO(PSA_ALG_IS_ECDSA(alg),
                              ALG_ECDSA_IS_DETERMINISTIC, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_DETERMINISTIC_ECDSA, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_RANDOMIZED_ECDSA, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_HASH_EDDSA, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_SIGN_HASH, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_HASH_AND_SIGN, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_RSA_OAEP, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_HKDF, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_HKDF_EXTRACT, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_HKDF_EXPAND, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_WILDCARD, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_ECDH, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_FFDH, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_RAW_KEY_AGREEMENT, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_AEAD_ON_BLOCK_CIPHER, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_TLS12_PRF, alg, flags);
    TEST_CLASSIFICATION_MACRO(1, ALG_IS_TLS12_PSK_TO_MS, alg, flags);
    TEST_EQUAL(classification_flags_tested, ALG_FLAG_MASK_PLUS_ONE - 1);
exit:;
}

static void key_type_classification(psa_key_type_t type, unsigned flags)
{
    unsigned classification_flags_tested = 0;

    /* Macros tested based on the test case parameter */
    TEST_CLASSIFICATION_MACRO(1, KEY_TYPE_IS_VENDOR_DEFINED, type, flags);
    TEST_CLASSIFICATION_MACRO(1, KEY_TYPE_IS_UNSTRUCTURED, type, flags);
    TEST_CLASSIFICATION_MACRO(1, KEY_TYPE_IS_PUBLIC_KEY, type, flags);
    TEST_CLASSIFICATION_MACRO(1, KEY_TYPE_IS_KEY_PAIR, type, flags);
    TEST_CLASSIFICATION_MACRO(1, KEY_TYPE_IS_RSA, type, flags);
    TEST_CLASSIFICATION_MACRO(1, KEY_TYPE_IS_DSA, type, flags);
    TEST_CLASSIFICATION_MACRO(1, KEY_TYPE_IS_ECC, type, flags);
    TEST_CLASSIFICATION_MACRO(1, KEY_TYPE_IS_DH, type, flags);
    TEST_EQUAL(classification_flags_tested, KEY_TYPE_FLAG_MASK_PLUS_ONE - 1);

    /* Macros with derived semantics */
    TEST_EQUAL(PSA_KEY_TYPE_IS_ASYMMETRIC(type),
               (PSA_KEY_TYPE_IS_PUBLIC_KEY(type) ||
                PSA_KEY_TYPE_IS_KEY_PAIR(type)));
    TEST_EQUAL(PSA_KEY_TYPE_IS_ECC_KEY_PAIR(type),
               (PSA_KEY_TYPE_IS_ECC(type) &&
                PSA_KEY_TYPE_IS_KEY_PAIR(type)));
    TEST_EQUAL(PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY(type),
               (PSA_KEY_TYPE_IS_ECC(type) &&
                PSA_KEY_TYPE_IS_PUBLIC_KEY(type)));
    TEST_EQUAL(PSA_KEY_TYPE_IS_DH_KEY_PAIR(type),
               (PSA_KEY_TYPE_IS_DH(type) &&
                PSA_KEY_TYPE_IS_KEY_PAIR(type)));
    TEST_EQUAL(PSA_KEY_TYPE_IS_DH_PUBLIC_KEY(type),
               (PSA_KEY_TYPE_IS_DH(type) &&
                PSA_KEY_TYPE_IS_PUBLIC_KEY(type)));

    TEST_PARITY(type);

exit:;
}

static void mac_algorithm_core(psa_algorithm_t alg, int classification_flags,
                        psa_key_type_t key_type, size_t key_bits,
                        size_t length)
{
    /* Algorithm classification */
    TEST_ASSERT(!PSA_ALG_IS_HASH(alg));
    TEST_ASSERT(PSA_ALG_IS_MAC(alg));
    TEST_ASSERT(!PSA_ALG_IS_CIPHER(alg));
    TEST_ASSERT(!PSA_ALG_IS_AEAD(alg));
    TEST_ASSERT(!PSA_ALG_IS_SIGN(alg));
    TEST_ASSERT(!PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg));
    TEST_ASSERT(!PSA_ALG_IS_KEY_AGREEMENT(alg));
    TEST_ASSERT(!PSA_ALG_IS_KEY_DERIVATION(alg));
    TEST_ASSERT(!PSA_ALG_IS_PAKE(alg));
    algorithm_classification(alg, classification_flags);

    /* Length */
    TEST_EQUAL(length, PSA_MAC_LENGTH(key_type, key_bits, alg));

#if defined(MBEDTLS_TEST_HOOKS) && defined(MBEDTLS_PSA_CRYPTO_C)
    PSA_ASSERT(psa_mac_key_can_do(alg, key_type));
#endif

exit:;
}

static void aead_algorithm_core(psa_algorithm_t alg, int classification_flags,
                         psa_key_type_t key_type, size_t key_bits,
                         size_t tag_length)
{
    /* Algorithm classification */
    TEST_ASSERT(!PSA_ALG_IS_HASH(alg));
    TEST_ASSERT(!PSA_ALG_IS_MAC(alg));
    TEST_ASSERT(!PSA_ALG_IS_CIPHER(alg));
    TEST_ASSERT(PSA_ALG_IS_AEAD(alg));
    TEST_ASSERT(!PSA_ALG_IS_SIGN(alg));
    TEST_ASSERT(!PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg));
    TEST_ASSERT(!PSA_ALG_IS_KEY_AGREEMENT(alg));
    TEST_ASSERT(!PSA_ALG_IS_KEY_DERIVATION(alg));
    TEST_ASSERT(!PSA_ALG_IS_PAKE(alg));
    algorithm_classification(alg, classification_flags);

    /* Tag length */
    TEST_EQUAL(tag_length, PSA_AEAD_TAG_LENGTH(key_type, key_bits, alg));

exit:;
}

#line 251 "suites/test_suite_psa_crypto_metadata.function"
static void test_hash_algorithm(int alg_arg, int length_arg)
{
    psa_algorithm_t alg = alg_arg;
    size_t length = length_arg;
    psa_algorithm_t hmac_alg = PSA_ALG_HMAC(alg);
    psa_algorithm_t rsa_pkcs1v15_sign_alg = PSA_ALG_RSA_PKCS1V15_SIGN(alg);
    psa_algorithm_t rsa_pss_alg = PSA_ALG_RSA_PSS(alg);
    psa_algorithm_t dsa_alg = PSA_ALG_DSA(alg);
    psa_algorithm_t deterministic_dsa_alg = PSA_ALG_DETERMINISTIC_DSA(alg);
    psa_algorithm_t ecdsa_alg = PSA_ALG_ECDSA(alg);
    psa_algorithm_t deterministic_ecdsa_alg = PSA_ALG_DETERMINISTIC_ECDSA(alg);
    psa_algorithm_t rsa_oaep_alg = PSA_ALG_RSA_OAEP(alg);
    psa_algorithm_t hkdf_alg = PSA_ALG_HKDF(alg);

    /* Algorithm classification */
    TEST_ASSERT(PSA_ALG_IS_HASH(alg));
    TEST_ASSERT(!PSA_ALG_IS_MAC(alg));
    TEST_ASSERT(!PSA_ALG_IS_CIPHER(alg));
    TEST_ASSERT(!PSA_ALG_IS_AEAD(alg));
    TEST_ASSERT(!PSA_ALG_IS_SIGN(alg));
    TEST_ASSERT(!PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg));
    TEST_ASSERT(!PSA_ALG_IS_KEY_AGREEMENT(alg));
    TEST_ASSERT(!PSA_ALG_IS_KEY_DERIVATION(alg));
    TEST_ASSERT(!PSA_ALG_IS_PAKE(alg));
    algorithm_classification(alg, 0);

    /* Dependent algorithms */
    TEST_EQUAL(PSA_ALG_HMAC_GET_HASH(hmac_alg), alg);
    TEST_EQUAL(PSA_ALG_SIGN_GET_HASH(rsa_pkcs1v15_sign_alg), alg);
    TEST_EQUAL(PSA_ALG_SIGN_GET_HASH(rsa_pss_alg), alg);
    TEST_EQUAL(PSA_ALG_SIGN_GET_HASH(dsa_alg), alg);
    TEST_EQUAL(PSA_ALG_SIGN_GET_HASH(deterministic_dsa_alg), alg);
    TEST_EQUAL(PSA_ALG_SIGN_GET_HASH(ecdsa_alg), alg);
    TEST_EQUAL(PSA_ALG_SIGN_GET_HASH(deterministic_ecdsa_alg), alg);
    TEST_EQUAL(PSA_ALG_RSA_OAEP_GET_HASH(rsa_oaep_alg), alg);
    TEST_EQUAL(PSA_ALG_HKDF_GET_HASH(hkdf_alg), alg);

    /* Hash length */
    TEST_EQUAL(length, PSA_HASH_LENGTH(alg));
    TEST_ASSERT(length <= PSA_HASH_MAX_SIZE);
exit:
    ;
}

static void test_hash_algorithm_wrapper( void ** params )
{

    test_hash_algorithm( *( (int *) params[0] ), *( (int *) params[1] ) );
}
#line 295 "suites/test_suite_psa_crypto_metadata.function"
static void test_mac_algorithm(int alg_arg, int classification_flags,
                   int length_arg,
                   int key_type_arg, int key_bits_arg)
{
    psa_algorithm_t alg = alg_arg;
    size_t length = length_arg;
    size_t n;
    size_t key_type = key_type_arg;
    size_t key_bits = key_bits_arg;

    mac_algorithm_core(alg, classification_flags,
                       key_type, key_bits, length);
    TEST_EQUAL(PSA_ALG_FULL_LENGTH_MAC(alg), alg);
    TEST_ASSERT(length <= PSA_MAC_MAX_SIZE);

    /* Truncated versions */
    for (n = 1; n <= length; n++) {
        psa_algorithm_t truncated_alg = PSA_ALG_TRUNCATED_MAC(alg, n);
        mac_algorithm_core(truncated_alg, classification_flags,
                           key_type, key_bits, n);
        TEST_EQUAL(PSA_ALG_FULL_LENGTH_MAC(truncated_alg), alg);
        /* Check that calling PSA_ALG_TRUNCATED_MAC twice gives the length
         * of the outer truncation (even if the outer length is smaller than
         * the inner length). */
        TEST_EQUAL(PSA_ALG_TRUNCATED_MAC(truncated_alg, 1),
                   PSA_ALG_TRUNCATED_MAC(alg, 1));
        TEST_EQUAL(PSA_ALG_TRUNCATED_MAC(truncated_alg, length - 1),
                   PSA_ALG_TRUNCATED_MAC(alg, length - 1));
        TEST_EQUAL(PSA_ALG_TRUNCATED_MAC(truncated_alg, length),
                   PSA_ALG_TRUNCATED_MAC(alg, length));

        /* Check that calling PSA_ALG_TRUNCATED_MAC on an algorithm
         * earlier constructed with PSA_ALG_AT_LEAST_THIS_LENGTH_MAC gives the
         * length of the outer truncation (even if the outer length is smaller
         * than the inner length). */
        TEST_EQUAL(PSA_ALG_TRUNCATED_MAC(
                       PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(truncated_alg, n), 1),
                   PSA_ALG_TRUNCATED_MAC(alg, 1));
        TEST_EQUAL(PSA_ALG_TRUNCATED_MAC(
                       PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(truncated_alg, n), length - 1),
                   PSA_ALG_TRUNCATED_MAC(alg, length - 1));
        TEST_EQUAL(PSA_ALG_TRUNCATED_MAC(
                       PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(truncated_alg, n), length),
                   PSA_ALG_TRUNCATED_MAC(alg, length));
    }

    /* At-leat-this-length versions */
    for (n = 1; n <= length; n++) {
        psa_algorithm_t policy_alg = PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(alg, n);
        mac_algorithm_core(policy_alg, classification_flags | ALG_IS_WILDCARD,
                           key_type, key_bits, n);
        TEST_EQUAL(PSA_ALG_FULL_LENGTH_MAC(policy_alg), alg);
        /* Check that calling PSA_ALG_AT_LEAST_THIS_LENGTH_MAC twice gives the
         * length of the outer truncation (even if the outer length is smaller
         * than the inner length). */
        TEST_EQUAL(PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(policy_alg, 1),
                   PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(alg, 1));
        TEST_EQUAL(PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(policy_alg, length - 1),
                   PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(alg, length - 1));
        TEST_EQUAL(PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(policy_alg, length),
                   PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(alg, length));

        /* Check that calling PSA_ALG_AT_LEAST_THIS_LENGTH_MAC on an algorithm
         * earlier constructed with PSA_ALG_TRUNCATED_MAC gives the length of
         * the outer truncation (even if the outer length is smaller than the
         * inner length). */
        TEST_EQUAL(PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(
                       PSA_ALG_TRUNCATED_MAC(policy_alg, n), 1),
                   PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(alg, 1));
        TEST_EQUAL(PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(
                       PSA_ALG_TRUNCATED_MAC(policy_alg, n), length - 1),
                   PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(alg, length - 1));
        TEST_EQUAL(PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(
                       PSA_ALG_TRUNCATED_MAC(policy_alg, n), length),
                   PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(alg, length));
    }
exit:
    ;
}

static void test_mac_algorithm_wrapper( void ** params )
{

    test_mac_algorithm( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ) );
}
#line 375 "suites/test_suite_psa_crypto_metadata.function"
static void test_hmac_algorithm(int alg_arg,
                    int length_arg,
                    int block_size_arg)
{
    psa_algorithm_t alg = alg_arg;
    psa_algorithm_t hash_alg = PSA_ALG_HMAC_GET_HASH(alg);
    size_t block_size = block_size_arg;
    size_t length = length_arg;
    size_t n;

    TEST_ASSERT(PSA_ALG_IS_HASH(hash_alg));
    TEST_EQUAL(PSA_ALG_HMAC(hash_alg), alg);

    TEST_ASSERT(block_size == PSA_HASH_BLOCK_LENGTH(alg));
    TEST_ASSERT(block_size <= PSA_HMAC_MAX_HASH_BLOCK_SIZE);

    test_mac_algorithm(alg_arg, ALG_IS_HMAC, length,
                       PSA_KEY_TYPE_HMAC, PSA_BYTES_TO_BITS(length));

    for (n = 1; n <= length; n++) {
        psa_algorithm_t truncated_alg = PSA_ALG_TRUNCATED_MAC(alg, n);
        TEST_EQUAL(PSA_ALG_HMAC_GET_HASH(truncated_alg), hash_alg);
    }
exit:
    ;
}

static void test_hmac_algorithm_wrapper( void ** params )
{

    test_hmac_algorithm( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
#line 402 "suites/test_suite_psa_crypto_metadata.function"
static void test_cipher_algorithm(int alg_arg, int classification_flags)
{
    psa_algorithm_t alg = alg_arg;

    /* Algorithm classification */
    TEST_ASSERT(!PSA_ALG_IS_HASH(alg));
    TEST_ASSERT(!PSA_ALG_IS_MAC(alg));
    TEST_ASSERT(PSA_ALG_IS_CIPHER(alg));
    TEST_ASSERT(!PSA_ALG_IS_AEAD(alg));
    TEST_ASSERT(!PSA_ALG_IS_SIGN(alg));
    TEST_ASSERT(!PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg));
    TEST_ASSERT(!PSA_ALG_IS_KEY_AGREEMENT(alg));
    TEST_ASSERT(!PSA_ALG_IS_KEY_DERIVATION(alg));
    TEST_ASSERT(!PSA_ALG_IS_PAKE(alg));
    algorithm_classification(alg, classification_flags);
exit:
    ;
}

static void test_cipher_algorithm_wrapper( void ** params )
{

    test_cipher_algorithm( *( (int *) params[0] ), *( (int *) params[1] ) );
}
#line 421 "suites/test_suite_psa_crypto_metadata.function"
static void test_aead_algorithm(int alg_arg, int classification_flags,
                    int tag_length_arg,
                    int key_type_arg, int key_bits_arg)
{
    psa_algorithm_t alg = alg_arg;
    size_t tag_length = tag_length_arg;
    size_t n;
    psa_key_type_t key_type = key_type_arg;
    size_t key_bits = key_bits_arg;

    aead_algorithm_core(alg, classification_flags,
                        key_type, key_bits, tag_length);

    /* Truncated versions */
    for (n = 1; n <= tag_length; n++) {
        psa_algorithm_t truncated_alg = PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, n);
        aead_algorithm_core(truncated_alg, classification_flags,
                            key_type, key_bits, n);
        TEST_EQUAL(PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(truncated_alg),
                   alg);
        /* Check that calling PSA_ALG_AEAD_WITH_SHORTENED_TAG twice gives
         * the length of the outer truncation (even if the outer length is
         * smaller than the inner length). */
        TEST_EQUAL(PSA_ALG_AEAD_WITH_SHORTENED_TAG(truncated_alg, 1),
                   PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, 1));
        TEST_EQUAL(PSA_ALG_AEAD_WITH_SHORTENED_TAG(truncated_alg, tag_length - 1),
                   PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, tag_length - 1));
        TEST_EQUAL(PSA_ALG_AEAD_WITH_SHORTENED_TAG(truncated_alg, tag_length),
                   PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, tag_length));

        /* Check that calling PSA_ALG_AEAD_WITH_SHORTENED_TAG on an algorithm
         * earlier constructed with PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG
         * gives the length of the outer truncation (even if the outer length is
         * smaller than the inner length). */
        TEST_EQUAL(PSA_ALG_AEAD_WITH_SHORTENED_TAG(
                       PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(truncated_alg, n), 1),
                   PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, 1));
        TEST_EQUAL(PSA_ALG_AEAD_WITH_SHORTENED_TAG(
                       PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(truncated_alg,
                                                                  n), tag_length - 1),
                   PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, tag_length - 1));
        TEST_EQUAL(PSA_ALG_AEAD_WITH_SHORTENED_TAG(
                       PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(truncated_alg, n), tag_length),
                   PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, tag_length));
    }

    /* At-leat-this-length versions */
    for (n = 1; n <= tag_length; n++) {
        psa_algorithm_t policy_alg = PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(alg, n);
        aead_algorithm_core(policy_alg, classification_flags | ALG_IS_WILDCARD,
                            key_type, key_bits, n);
        TEST_EQUAL(PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(policy_alg),
                   alg);
        /* Check that calling PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG twice
         * gives the length of the outer truncation (even if the outer length is
         * smaller than the inner length). */
        TEST_EQUAL(PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(policy_alg, 1),
                   PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(alg, 1));
        TEST_EQUAL(PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(policy_alg, tag_length - 1),
                   PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(alg, tag_length - 1));
        TEST_EQUAL(PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(policy_alg, tag_length),
                   PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(alg, tag_length));

        /* Check that calling PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG on an
         * algorithm earlier constructed with PSA_ALG_AEAD_WITH_SHORTENED_TAG
         * gives the length of the outer truncation (even if the outer length is
         * smaller than the inner length). */
        TEST_EQUAL(PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(
                       PSA_ALG_AEAD_WITH_SHORTENED_TAG(policy_alg, n), 1),
                   PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(alg, 1));
        TEST_EQUAL(PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(
                       PSA_ALG_AEAD_WITH_SHORTENED_TAG(policy_alg, n), tag_length - 1),
                   PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(alg, tag_length - 1));
        TEST_EQUAL(PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(
                       PSA_ALG_AEAD_WITH_SHORTENED_TAG(policy_alg, n), tag_length),
                   PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(alg, tag_length));
    }
exit:
    ;
}

static void test_aead_algorithm_wrapper( void ** params )
{

    test_aead_algorithm( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ) );
}
#line 502 "suites/test_suite_psa_crypto_metadata.function"
static void test_asymmetric_signature_algorithm(int alg_arg, int classification_flags)
{
    psa_algorithm_t alg = alg_arg;

    /* Algorithm classification */
    TEST_ASSERT(!PSA_ALG_IS_HASH(alg));
    TEST_ASSERT(!PSA_ALG_IS_MAC(alg));
    TEST_ASSERT(!PSA_ALG_IS_CIPHER(alg));
    TEST_ASSERT(!PSA_ALG_IS_AEAD(alg));
    TEST_ASSERT(PSA_ALG_IS_SIGN(alg));
    TEST_ASSERT(!PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg));
    TEST_ASSERT(!PSA_ALG_IS_KEY_AGREEMENT(alg));
    TEST_ASSERT(!PSA_ALG_IS_KEY_DERIVATION(alg));
    TEST_ASSERT(!PSA_ALG_IS_PAKE(alg));
    algorithm_classification(alg, classification_flags);
exit:
    ;
}

static void test_asymmetric_signature_algorithm_wrapper( void ** params )
{

    test_asymmetric_signature_algorithm( *( (int *) params[0] ), *( (int *) params[1] ) );
}
#line 521 "suites/test_suite_psa_crypto_metadata.function"
static void test_asymmetric_signature_wildcard(int alg_arg, int classification_flags)
{
    classification_flags |= ALG_IS_WILDCARD;
    classification_flags |= ALG_IS_SIGN_HASH;
    classification_flags |= ALG_IS_HASH_AND_SIGN;
    test_asymmetric_signature_algorithm(alg_arg, classification_flags);
    /* Any failure of this test function comes from
     * asymmetric_signature_algorithm. Pacify -Werror=unused-label. */
    goto exit;
exit:
    ;
}

static void test_asymmetric_signature_wildcard_wrapper( void ** params )
{

    test_asymmetric_signature_wildcard( *( (int *) params[0] ), *( (int *) params[1] ) );
}
#line 534 "suites/test_suite_psa_crypto_metadata.function"
static void test_asymmetric_encryption_algorithm(int alg_arg, int classification_flags)
{
    psa_algorithm_t alg = alg_arg;

    /* Algorithm classification */
    TEST_ASSERT(!PSA_ALG_IS_HASH(alg));
    TEST_ASSERT(!PSA_ALG_IS_MAC(alg));
    TEST_ASSERT(!PSA_ALG_IS_CIPHER(alg));
    TEST_ASSERT(!PSA_ALG_IS_AEAD(alg));
    TEST_ASSERT(!PSA_ALG_IS_SIGN(alg));
    TEST_ASSERT(PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg));
    TEST_ASSERT(!PSA_ALG_IS_KEY_AGREEMENT(alg));
    TEST_ASSERT(!PSA_ALG_IS_KEY_DERIVATION(alg));
    TEST_ASSERT(!PSA_ALG_IS_PAKE(alg));
    algorithm_classification(alg, classification_flags);
exit:
    ;
}

static void test_asymmetric_encryption_algorithm_wrapper( void ** params )
{

    test_asymmetric_encryption_algorithm( *( (int *) params[0] ), *( (int *) params[1] ) );
}
#line 553 "suites/test_suite_psa_crypto_metadata.function"
static void test_key_derivation_algorithm(int alg_arg, int classification_flags)
{
    psa_algorithm_t alg = alg_arg;
    psa_algorithm_t ecdh_alg = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, alg);
    psa_algorithm_t ffdh_alg = PSA_ALG_KEY_AGREEMENT(PSA_ALG_FFDH, alg);

    /* Algorithm classification */
    TEST_ASSERT(!PSA_ALG_IS_HASH(alg));
    TEST_ASSERT(!PSA_ALG_IS_MAC(alg));
    TEST_ASSERT(!PSA_ALG_IS_CIPHER(alg));
    TEST_ASSERT(!PSA_ALG_IS_AEAD(alg));
    TEST_ASSERT(!PSA_ALG_IS_SIGN(alg));
    TEST_ASSERT(!PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg));
    TEST_ASSERT(!PSA_ALG_IS_KEY_AGREEMENT(alg));
    TEST_ASSERT(PSA_ALG_IS_KEY_DERIVATION(alg));
    TEST_ASSERT(!PSA_ALG_IS_PAKE(alg));
    algorithm_classification(alg, classification_flags);

    /* Check combinations with key agreements */
    TEST_ASSERT(PSA_ALG_IS_KEY_AGREEMENT(ecdh_alg));
    TEST_ASSERT(PSA_ALG_IS_KEY_AGREEMENT(ffdh_alg));
    TEST_EQUAL(PSA_ALG_KEY_AGREEMENT_GET_KDF(ecdh_alg), alg);
    TEST_EQUAL(PSA_ALG_KEY_AGREEMENT_GET_KDF(ffdh_alg), alg);
exit:
    ;
}

static void test_key_derivation_algorithm_wrapper( void ** params )
{

    test_key_derivation_algorithm( *( (int *) params[0] ), *( (int *) params[1] ) );
}
#line 580 "suites/test_suite_psa_crypto_metadata.function"
static void test_key_agreement_algorithm(int alg_arg, int classification_flags,
                             int ka_alg_arg, int kdf_alg_arg)
{
    psa_algorithm_t alg = alg_arg;
    psa_algorithm_t actual_ka_alg = PSA_ALG_KEY_AGREEMENT_GET_BASE(alg);
    psa_algorithm_t expected_ka_alg = ka_alg_arg;
    psa_algorithm_t actual_kdf_alg = PSA_ALG_KEY_AGREEMENT_GET_KDF(alg);
    psa_algorithm_t expected_kdf_alg = kdf_alg_arg;

    /* Algorithm classification */
    TEST_ASSERT(!PSA_ALG_IS_HASH(alg));
    TEST_ASSERT(!PSA_ALG_IS_MAC(alg));
    TEST_ASSERT(!PSA_ALG_IS_CIPHER(alg));
    TEST_ASSERT(!PSA_ALG_IS_AEAD(alg));
    TEST_ASSERT(!PSA_ALG_IS_SIGN(alg));
    TEST_ASSERT(!PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg));
    TEST_ASSERT(PSA_ALG_IS_KEY_AGREEMENT(alg));
    TEST_ASSERT(!PSA_ALG_IS_KEY_DERIVATION(alg));
    TEST_ASSERT(!PSA_ALG_IS_PAKE(alg));
    algorithm_classification(alg, classification_flags);

    /* Shared secret derivation properties */
    TEST_EQUAL(actual_ka_alg, expected_ka_alg);
    TEST_EQUAL(actual_kdf_alg, expected_kdf_alg);
exit:
    ;
}

static void test_key_agreement_algorithm_wrapper( void ** params )
{

    test_key_agreement_algorithm( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ) );
}
#line 608 "suites/test_suite_psa_crypto_metadata.function"
static void test_pake_algorithm(int alg_arg)
{
    psa_algorithm_t alg = alg_arg;

    /* Algorithm classification */
    TEST_ASSERT(!PSA_ALG_IS_HASH(alg));
    TEST_ASSERT(!PSA_ALG_IS_MAC(alg));
    TEST_ASSERT(!PSA_ALG_IS_CIPHER(alg));
    TEST_ASSERT(!PSA_ALG_IS_AEAD(alg));
    TEST_ASSERT(!PSA_ALG_IS_SIGN(alg));
    TEST_ASSERT(!PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg));
    TEST_ASSERT(!PSA_ALG_IS_KEY_AGREEMENT(alg));
    TEST_ASSERT(!PSA_ALG_IS_KEY_DERIVATION(alg));
    TEST_ASSERT(PSA_ALG_IS_PAKE(alg));
exit:
    ;
}


static void test_pake_algorithm_wrapper( void ** params )
{

    test_pake_algorithm( *( (int *) params[0] ) );
}
#line 626 "suites/test_suite_psa_crypto_metadata.function"
static void test_key_type(int type_arg, int classification_flags)
{
    psa_key_type_t type = type_arg;

    key_type_classification(type, classification_flags);

    /* For asymmetric types, check the corresponding pair/public type */
    if (classification_flags & KEY_TYPE_IS_PUBLIC_KEY) {
        psa_key_type_t pair_type = PSA_KEY_TYPE_KEY_PAIR_OF_PUBLIC_KEY(type);
        TEST_EQUAL(PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(pair_type), type);
        key_type_classification(pair_type,
                                (classification_flags
                                 & ~KEY_TYPE_IS_PUBLIC_KEY)
                                | KEY_TYPE_IS_KEY_PAIR);
        TEST_EQUAL(PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type), type);
    }
    if (classification_flags & KEY_TYPE_IS_KEY_PAIR) {
        psa_key_type_t public_type = PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type);
        TEST_EQUAL(PSA_KEY_TYPE_KEY_PAIR_OF_PUBLIC_KEY(public_type), type);
        key_type_classification(public_type,
                                (classification_flags
                                 & ~KEY_TYPE_IS_KEY_PAIR)
                                | KEY_TYPE_IS_PUBLIC_KEY);
        TEST_EQUAL(PSA_KEY_TYPE_KEY_PAIR_OF_PUBLIC_KEY(type), type);
    }
exit:
    ;
}

static void test_key_type_wrapper( void ** params )
{

    test_key_type( *( (int *) params[0] ), *( (int *) params[1] ) );
}
#line 655 "suites/test_suite_psa_crypto_metadata.function"
static void test_block_cipher_key_type(int type_arg, int block_size_arg)
{
    psa_key_type_t type = type_arg;
    size_t block_size = block_size_arg;

    test_key_type(type_arg, KEY_TYPE_IS_UNSTRUCTURED);

    TEST_EQUAL(type & PSA_KEY_TYPE_CATEGORY_MASK,
               PSA_KEY_TYPE_CATEGORY_SYMMETRIC);
    TEST_EQUAL(PSA_BLOCK_CIPHER_BLOCK_LENGTH(type), block_size);

    /* Check that the block size is a power of 2. This is required, at least,
       for PSA_ROUND_UP_TO_MULTIPLE(block_size, length) in crypto_sizes.h. */
    TEST_ASSERT(((block_size - 1) & block_size) == 0);
exit:
    ;
}

static void test_block_cipher_key_type_wrapper( void ** params )
{

    test_block_cipher_key_type( *( (int *) params[0] ), *( (int *) params[1] ) );
}
#line 673 "suites/test_suite_psa_crypto_metadata.function"
static void test_stream_cipher_key_type(int type_arg)
{
    psa_key_type_t type = type_arg;

    test_key_type(type_arg, KEY_TYPE_IS_UNSTRUCTURED);

    TEST_EQUAL(type & PSA_KEY_TYPE_CATEGORY_MASK,
               PSA_KEY_TYPE_CATEGORY_SYMMETRIC);
    TEST_EQUAL(PSA_BLOCK_CIPHER_BLOCK_LENGTH(type), 1);
exit:
    ;
}

static void test_stream_cipher_key_type_wrapper( void ** params )
{

    test_stream_cipher_key_type( *( (int *) params[0] ) );
}
#if defined(PSA_KEY_TYPE_ECC_PUBLIC_KEY)
#if defined(PSA_KEY_TYPE_ECC_KEY_PAIR)
#line 686 "suites/test_suite_psa_crypto_metadata.function"
static void test_ecc_key_family(int curve_arg)
{
    psa_ecc_family_t curve = curve_arg;
    psa_key_type_t public_type = PSA_KEY_TYPE_ECC_PUBLIC_KEY(curve);
    psa_key_type_t pair_type = PSA_KEY_TYPE_ECC_KEY_PAIR(curve);

    TEST_PARITY(curve);

    test_key_type(public_type, KEY_TYPE_IS_ECC | KEY_TYPE_IS_PUBLIC_KEY);
    test_key_type(pair_type, KEY_TYPE_IS_ECC | KEY_TYPE_IS_KEY_PAIR);

    TEST_EQUAL(PSA_KEY_TYPE_ECC_GET_FAMILY(public_type), curve);
    TEST_EQUAL(PSA_KEY_TYPE_ECC_GET_FAMILY(pair_type), curve);
exit:
    ;
}

static void test_ecc_key_family_wrapper( void ** params )
{

    test_ecc_key_family( *( (int *) params[0] ) );
}
#endif /* PSA_KEY_TYPE_ECC_KEY_PAIR */
#endif /* PSA_KEY_TYPE_ECC_PUBLIC_KEY */
#if defined(MBEDTLS_DHM_C)
#line 703 "suites/test_suite_psa_crypto_metadata.function"
static void test_dh_key_family(int group_arg)
{
    psa_dh_family_t group = group_arg;
    psa_key_type_t public_type = PSA_KEY_TYPE_DH_PUBLIC_KEY(group);
    psa_key_type_t pair_type = PSA_KEY_TYPE_DH_KEY_PAIR(group);

    TEST_PARITY(group);

    test_key_type(public_type, KEY_TYPE_IS_DH | KEY_TYPE_IS_PUBLIC_KEY);
    test_key_type(pair_type, KEY_TYPE_IS_DH | KEY_TYPE_IS_KEY_PAIR);

    TEST_EQUAL(PSA_KEY_TYPE_DH_GET_FAMILY(public_type), group);
    TEST_EQUAL(PSA_KEY_TYPE_DH_GET_FAMILY(pair_type), group);
exit:
    ;
}

static void test_dh_key_family_wrapper( void ** params )
{

    test_dh_key_family( *( (int *) params[0] ) );
}
#endif /* MBEDTLS_DHM_C */
#line 720 "suites/test_suite_psa_crypto_metadata.function"
static void test_lifetime(int lifetime_arg, int classification_flags,
              int persistence_arg, int location_arg)
{
    psa_key_lifetime_t lifetime = lifetime_arg;
    psa_key_persistence_t persistence = persistence_arg;
    psa_key_location_t location = location_arg;
    unsigned flags = classification_flags;
    unsigned classification_flags_tested = 0;

    TEST_CLASSIFICATION_MACRO(1, KEY_LIFETIME_IS_VOLATILE, lifetime, flags);
    TEST_CLASSIFICATION_MACRO(1, KEY_LIFETIME_IS_READ_ONLY, lifetime, flags);
    TEST_EQUAL(classification_flags_tested,
               KEY_LIFETIME_FLAG_MASK_PLUS_ONE - 1);

    TEST_EQUAL(PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime), persistence);
    TEST_EQUAL(PSA_KEY_LIFETIME_GET_LOCATION(lifetime), location);
exit:
    ;
}

static void test_lifetime_wrapper( void ** params )
{

    test_lifetime( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ) );
}
#endif /* MBEDTLS_PSA_CRYPTO_CLIENT */


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
    
#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)

        case 0:
            {
                *out_value = PSA_ALG_MD5;
            }
            break;
        case 1:
            {
                *out_value = PSA_ALG_RIPEMD160;
            }
            break;
        case 2:
            {
                *out_value = PSA_ALG_SHA_1;
            }
            break;
        case 3:
            {
                *out_value = PSA_ALG_SHA_224;
            }
            break;
        case 4:
            {
                *out_value = PSA_ALG_SHA_256;
            }
            break;
        case 5:
            {
                *out_value = PSA_ALG_SHA_384;
            }
            break;
        case 6:
            {
                *out_value = PSA_ALG_SHA_512;
            }
            break;
        case 7:
            {
                *out_value = PSA_ALG_HMAC( PSA_ALG_MD5 );
            }
            break;
        case 8:
            {
                *out_value = PSA_ALG_HMAC( PSA_ALG_RIPEMD160 );
            }
            break;
        case 9:
            {
                *out_value = PSA_ALG_HMAC( PSA_ALG_SHA_1 );
            }
            break;
        case 10:
            {
                *out_value = PSA_ALG_HMAC( PSA_ALG_SHA_224 );
            }
            break;
        case 11:
            {
                *out_value = PSA_ALG_HMAC( PSA_ALG_SHA_256 );
            }
            break;
        case 12:
            {
                *out_value = PSA_ALG_HMAC( PSA_ALG_SHA_384 );
            }
            break;
        case 13:
            {
                *out_value = PSA_ALG_HMAC( PSA_ALG_SHA_512 );
            }
            break;
        case 14:
            {
                *out_value = PSA_ALG_CBC_MAC;
            }
            break;
        case 15:
            {
                *out_value = ALG_IS_BLOCK_CIPHER_MAC;
            }
            break;
        case 16:
            {
                *out_value = PSA_KEY_TYPE_AES;
            }
            break;
        case 17:
            {
                *out_value = PSA_KEY_TYPE_DES;
            }
            break;
        case 18:
            {
                *out_value = PSA_ALG_CMAC;
            }
            break;
        case 19:
            {
                *out_value = PSA_ALG_STREAM_CIPHER;
            }
            break;
        case 20:
            {
                *out_value = ALG_IS_STREAM_CIPHER;
            }
            break;
        case 21:
            {
                *out_value = PSA_ALG_CTR;
            }
            break;
        case 22:
            {
                *out_value = PSA_ALG_CFB;
            }
            break;
        case 23:
            {
                *out_value = PSA_ALG_OFB;
            }
            break;
        case 24:
            {
                *out_value = PSA_ALG_ECB_NO_PADDING;
            }
            break;
        case 25:
            {
                *out_value = PSA_ALG_CBC_NO_PADDING;
            }
            break;
        case 26:
            {
                *out_value = PSA_ALG_CBC_PKCS7;
            }
            break;
        case 27:
            {
                *out_value = PSA_ALG_XTS;
            }
            break;
        case 28:
            {
                *out_value = PSA_ALG_CCM_STAR_NO_TAG;
            }
            break;
        case 29:
            {
                *out_value = PSA_ALG_CCM;
            }
            break;
        case 30:
            {
                *out_value = ALG_IS_AEAD_ON_BLOCK_CIPHER;
            }
            break;
        case 31:
            {
                *out_value = PSA_KEY_TYPE_ARIA;
            }
            break;
        case 32:
            {
                *out_value = PSA_KEY_TYPE_CAMELLIA;
            }
            break;
        case 33:
            {
                *out_value = PSA_ALG_GCM;
            }
            break;
        case 34:
            {
                *out_value = PSA_ALG_CHACHA20_POLY1305;
            }
            break;
        case 35:
            {
                *out_value = PSA_KEY_TYPE_CHACHA20;
            }
            break;
        case 36:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_SIGN_RAW;
            }
            break;
        case 37:
            {
                *out_value = ALG_IS_RSA_PKCS1V15_SIGN | ALG_IS_SIGN_HASH;
            }
            break;
        case 38:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_SIGN( PSA_ALG_SHA_256 );
            }
            break;
        case 39:
            {
                *out_value = ALG_IS_RSA_PKCS1V15_SIGN | ALG_IS_SIGN_HASH | ALG_IS_HASH_AND_SIGN;
            }
            break;
        case 40:
            {
                *out_value = PSA_ALG_RSA_PSS( PSA_ALG_SHA_256 );
            }
            break;
        case 41:
            {
                *out_value = ALG_IS_RSA_PSS | ALG_IS_RSA_PSS_STANDARD_SALT | ALG_IS_SIGN_HASH | ALG_IS_HASH_AND_SIGN;
            }
            break;
        case 42:
            {
                *out_value = PSA_ALG_RSA_PSS_ANY_SALT( PSA_ALG_SHA_256 );
            }
            break;
        case 43:
            {
                *out_value = ALG_IS_RSA_PSS | ALG_IS_RSA_PSS_ANY_SALT | ALG_IS_SIGN_HASH | ALG_IS_HASH_AND_SIGN;
            }
            break;
        case 44:
            {
                *out_value = PSA_ALG_ECDSA_ANY;
            }
            break;
        case 45:
            {
                *out_value = ALG_IS_ECDSA | ALG_IS_RANDOMIZED_ECDSA | ALG_IS_SIGN_HASH;
            }
            break;
        case 46:
            {
                *out_value = PSA_ALG_ECDSA( PSA_ALG_SHA_256 );
            }
            break;
        case 47:
            {
                *out_value = ALG_IS_ECDSA | ALG_IS_RANDOMIZED_ECDSA | ALG_IS_SIGN_HASH | ALG_IS_HASH_AND_SIGN;
            }
            break;
        case 48:
            {
                *out_value = PSA_ALG_DETERMINISTIC_ECDSA( PSA_ALG_SHA_256 );
            }
            break;
        case 49:
            {
                *out_value = ALG_IS_ECDSA | ALG_IS_DETERMINISTIC_ECDSA | ALG_ECDSA_IS_DETERMINISTIC | ALG_IS_SIGN_HASH | ALG_IS_HASH_AND_SIGN;
            }
            break;
        case 50:
            {
                *out_value = PSA_ALG_PURE_EDDSA;
            }
            break;
        case 51:
            {
                *out_value = PSA_ALG_ED25519PH;
            }
            break;
        case 52:
            {
                *out_value = ALG_IS_HASH_EDDSA | ALG_IS_SIGN_HASH | ALG_IS_HASH_AND_SIGN;
            }
            break;
        case 53:
            {
                *out_value = PSA_ALG_ED448PH;
            }
            break;
        case 54:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_SIGN( PSA_ALG_ANY_HASH );
            }
            break;
        case 55:
            {
                *out_value = ALG_IS_RSA_PKCS1V15_SIGN;
            }
            break;
        case 56:
            {
                *out_value = PSA_ALG_RSA_PSS( PSA_ALG_ANY_HASH );
            }
            break;
        case 57:
            {
                *out_value = ALG_IS_RSA_PSS | ALG_IS_RSA_PSS_STANDARD_SALT;
            }
            break;
        case 58:
            {
                *out_value = PSA_ALG_RSA_PSS_ANY_SALT( PSA_ALG_ANY_HASH );
            }
            break;
        case 59:
            {
                *out_value = ALG_IS_RSA_PSS | ALG_IS_RSA_PSS_ANY_SALT;
            }
            break;
        case 60:
            {
                *out_value = PSA_ALG_ECDSA( PSA_ALG_ANY_HASH );
            }
            break;
        case 61:
            {
                *out_value = ALG_IS_ECDSA | ALG_IS_RANDOMIZED_ECDSA;
            }
            break;
        case 62:
            {
                *out_value = PSA_ALG_DETERMINISTIC_ECDSA( PSA_ALG_ANY_HASH );
            }
            break;
        case 63:
            {
                *out_value = ALG_IS_ECDSA | ALG_IS_DETERMINISTIC_ECDSA | ALG_ECDSA_IS_DETERMINISTIC;
            }
            break;
        case 64:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_CRYPT;
            }
            break;
        case 65:
            {
                *out_value = PSA_ALG_RSA_OAEP( PSA_ALG_SHA_256 );
            }
            break;
        case 66:
            {
                *out_value = ALG_IS_RSA_OAEP;
            }
            break;
        case 67:
            {
                *out_value = PSA_ALG_HKDF( PSA_ALG_SHA_256 );
            }
            break;
        case 68:
            {
                *out_value = ALG_IS_HKDF;
            }
            break;
        case 69:
            {
                *out_value = PSA_ALG_HKDF( PSA_ALG_SHA_384 );
            }
            break;
        case 70:
            {
                *out_value = PSA_ALG_HKDF_EXTRACT( PSA_ALG_SHA_256 );
            }
            break;
        case 71:
            {
                *out_value = ALG_IS_HKDF_EXTRACT;
            }
            break;
        case 72:
            {
                *out_value = PSA_ALG_HKDF_EXTRACT( PSA_ALG_SHA_384 );
            }
            break;
        case 73:
            {
                *out_value = PSA_ALG_HKDF_EXPAND( PSA_ALG_SHA_256 );
            }
            break;
        case 74:
            {
                *out_value = ALG_IS_HKDF_EXPAND;
            }
            break;
        case 75:
            {
                *out_value = PSA_ALG_HKDF_EXPAND( PSA_ALG_SHA_384 );
            }
            break;
        case 76:
            {
                *out_value = PSA_ALG_TLS12_ECJPAKE_TO_PMS;
            }
            break;
        case 77:
            {
                *out_value = PSA_ALG_TLS12_PRF( PSA_ALG_SHA_256 );
            }
            break;
        case 78:
            {
                *out_value = ALG_IS_TLS12_PRF;
            }
            break;
        case 79:
            {
                *out_value = PSA_ALG_TLS12_PRF( PSA_ALG_SHA_384 );
            }
            break;
        case 80:
            {
                *out_value = PSA_ALG_TLS12_PSK_TO_MS( PSA_ALG_SHA_256 );
            }
            break;
        case 81:
            {
                *out_value = ALG_IS_TLS12_PSK_TO_MS;
            }
            break;
        case 82:
            {
                *out_value = PSA_ALG_TLS12_PSK_TO_MS( PSA_ALG_SHA_384 );
            }
            break;
        case 83:
            {
                *out_value = PSA_ALG_FFDH;
            }
            break;
        case 84:
            {
                *out_value = ALG_IS_FFDH | ALG_IS_RAW_KEY_AGREEMENT;
            }
            break;
        case 85:
            {
                *out_value = PSA_ALG_CATEGORY_KEY_DERIVATION;
            }
            break;
        case 86:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT( PSA_ALG_FFDH, PSA_ALG_HKDF( PSA_ALG_SHA_256 ) );
            }
            break;
        case 87:
            {
                *out_value = ALG_IS_FFDH;
            }
            break;
        case 88:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT( PSA_ALG_FFDH, PSA_ALG_HKDF( PSA_ALG_SHA_384 ) );
            }
            break;
        case 89:
            {
                *out_value = PSA_ALG_ECDH;
            }
            break;
        case 90:
            {
                *out_value = ALG_IS_ECDH | ALG_IS_RAW_KEY_AGREEMENT;
            }
            break;
        case 91:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT( PSA_ALG_ECDH, PSA_ALG_HKDF( PSA_ALG_SHA_256 ) );
            }
            break;
        case 92:
            {
                *out_value = ALG_IS_ECDH;
            }
            break;
        case 93:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT( PSA_ALG_ECDH, PSA_ALG_HKDF( PSA_ALG_SHA_384 ) );
            }
            break;
        case 94:
            {
                *out_value = PSA_ALG_JPAKE;
            }
            break;
        case 95:
            {
                *out_value = PSA_KEY_TYPE_RAW_DATA;
            }
            break;
        case 96:
            {
                *out_value = KEY_TYPE_IS_UNSTRUCTURED;
            }
            break;
        case 97:
            {
                *out_value = PSA_KEY_TYPE_HMAC;
            }
            break;
        case 98:
            {
                *out_value = PSA_KEY_TYPE_DERIVE;
            }
            break;
        case 99:
            {
                *out_value = PSA_KEY_TYPE_PASSWORD;
            }
            break;
        case 100:
            {
                *out_value = PSA_KEY_TYPE_PASSWORD_HASH;
            }
            break;
        case 101:
            {
                *out_value = PSA_KEY_TYPE_RSA_PUBLIC_KEY;
            }
            break;
        case 102:
            {
                *out_value = KEY_TYPE_IS_PUBLIC_KEY | KEY_TYPE_IS_RSA;
            }
            break;
        case 103:
            {
                *out_value = PSA_KEY_TYPE_RSA_KEY_PAIR;
            }
            break;
        case 104:
            {
                *out_value = KEY_TYPE_IS_KEY_PAIR | KEY_TYPE_IS_RSA;
            }
            break;
        case 105:
            {
                *out_value = PSA_ECC_FAMILY_SECP_K1;
            }
            break;
        case 106:
            {
                *out_value = PSA_ECC_FAMILY_SECP_R1;
            }
            break;
        case 107:
            {
                *out_value = PSA_ECC_FAMILY_SECP_R2;
            }
            break;
        case 108:
            {
                *out_value = PSA_ECC_FAMILY_SECT_K1;
            }
            break;
        case 109:
            {
                *out_value = PSA_ECC_FAMILY_SECT_R1;
            }
            break;
        case 110:
            {
                *out_value = PSA_ECC_FAMILY_SECT_R2;
            }
            break;
        case 111:
            {
                *out_value = PSA_ECC_FAMILY_BRAINPOOL_P_R1;
            }
            break;
        case 112:
            {
                *out_value = PSA_ECC_FAMILY_MONTGOMERY;
            }
            break;
        case 113:
            {
                *out_value = PSA_ECC_FAMILY_TWISTED_EDWARDS;
            }
            break;
        case 114:
            {
                *out_value = PSA_DH_FAMILY_RFC7919;
            }
            break;
        case 115:
            {
                *out_value = PSA_KEY_LIFETIME_VOLATILE;
            }
            break;
        case 116:
            {
                *out_value = KEY_LIFETIME_IS_VOLATILE;
            }
            break;
        case 117:
            {
                *out_value = PSA_KEY_PERSISTENCE_VOLATILE;
            }
            break;
        case 118:
            {
                *out_value = PSA_KEY_LOCATION_LOCAL_STORAGE;
            }
            break;
        case 119:
            {
                *out_value = PSA_KEY_LIFETIME_PERSISTENT;
            }
            break;
        case 120:
            {
                *out_value = PSA_KEY_PERSISTENCE_DEFAULT;
            }
            break;
        case 121:
            {
                *out_value = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_VOLATILE, PSA_KEY_LOCATION_LOCAL_STORAGE);
            }
            break;
        case 122:
            {
                *out_value = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_DEFAULT, PSA_KEY_LOCATION_LOCAL_STORAGE);
            }
            break;
        case 123:
            {
                *out_value = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(2, PSA_KEY_LOCATION_LOCAL_STORAGE);
            }
            break;
        case 124:
            {
                *out_value = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(254, PSA_KEY_LOCATION_LOCAL_STORAGE);
            }
            break;
        case 125:
            {
                *out_value = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_READ_ONLY, PSA_KEY_LOCATION_LOCAL_STORAGE);
            }
            break;
        case 126:
            {
                *out_value = KEY_LIFETIME_IS_READ_ONLY;
            }
            break;
        case 127:
            {
                *out_value = PSA_KEY_PERSISTENCE_READ_ONLY;
            }
            break;
        case 128:
            {
                *out_value = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_VOLATILE, 0x123456);
            }
            break;
        case 129:
            {
                *out_value = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_DEFAULT, 0x123456);
            }
            break;
        case 130:
            {
                *out_value = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(2, 0x123456);
            }
            break;
        case 131:
            {
                *out_value = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(254, 0x123456);
            }
            break;
        case 132:
            {
                *out_value = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_READ_ONLY, 0x123456);
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
    
#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)

        case 0:
            {
#if defined(PSA_WANT_ALG_MD5)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(PSA_WANT_ALG_RIPEMD160)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(PSA_WANT_ALG_SHA_1)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 3:
            {
#if defined(PSA_WANT_ALG_SHA_224)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 4:
            {
#if defined(PSA_WANT_ALG_SHA_256)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 5:
            {
#if defined(PSA_WANT_ALG_SHA_384)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 6:
            {
#if defined(PSA_WANT_ALG_SHA_512)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 7:
            {
#if defined(PSA_WANT_ALG_HMAC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 8:
            {
#if defined(PSA_WANT_ALG_CBC_MAC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 9:
            {
#if defined(PSA_WANT_KEY_TYPE_AES)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 10:
            {
#if defined(MBEDTLS_CIPHER_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 11:
            {
#if defined(PSA_WANT_KEY_TYPE_DES)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 12:
            {
#if defined(PSA_WANT_ALG_CMAC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 13:
            {
#if defined(PSA_WANT_ALG_STREAM_CIPHER)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 14:
            {
#if defined(PSA_WANT_ALG_CTR)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 15:
            {
#if defined(PSA_WANT_ALG_CFB)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 16:
            {
#if defined(PSA_WANT_ALG_OFB)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 17:
            {
#if defined(PSA_WANT_ALG_ECB_NO_PADDING)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 18:
            {
#if defined(PSA_WANT_ALG_CBC_NO_PADDING)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 19:
            {
#if defined(PSA_WANT_ALG_CBC_PKCS7)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 20:
            {
#if defined(PSA_WANT_ALG_XTS)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 21:
            {
#if defined(PSA_WANT_ALG_CCM_STAR_NO_TAG)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 22:
            {
#if defined(PSA_WANT_ALG_CCM)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 23:
            {
#if defined(PSA_WANT_KEY_TYPE_ARIA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 24:
            {
#if defined(PSA_WANT_KEY_TYPE_CAMELLIA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 25:
            {
#if defined(PSA_WANT_ALG_GCM)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 26:
            {
#if defined(PSA_WANT_ALG_CHACHA20_POLY1305)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 27:
            {
#if defined(PSA_WANT_ALG_RSA_PKCS1V15_SIGN)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 28:
            {
#if defined(PSA_WANT_ALG_RSA_PSS)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 29:
            {
#if defined(PSA_WANT_ALG_ECDSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 30:
            {
#if defined(PSA_WANT_ALG_DETERMINISTIC_ECDSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 31:
            {
#if defined(PSA_WANT_ALG_EDDSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 32:
            {
#if defined(PSA_WANT_ALG_RSA_PKCS1V15_CRYPT)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 33:
            {
#if defined(PSA_WANT_ALG_RSA_OAEP)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 34:
            {
#if defined(PSA_WANT_ALG_HKDF)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 35:
            {
#if defined(PSA_WANT_ALG_HKDF_EXTRACT)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 36:
            {
#if defined(PSA_WANT_ALG_HKDF_EXPAND)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 37:
            {
#if defined(PSA_WANT_ALG_TLS12_ECJPAKE_TO_PMS)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 38:
            {
#if defined(PSA_WANT_ALG_TLS12_PRF)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 39:
            {
#if defined(PSA_WANT_ALG_TLS12_PSK_TO_MS)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 40:
            {
#if defined(PSA_WANT_ALG_FFDH)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 41:
            {
#if defined(PSA_WANT_ALG_ECDH)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 42:
            {
#if defined(PSA_WANT_KEY_TYPE_HMAC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 43:
            {
#if defined(PSA_WANT_KEY_TYPE_CHACHA20)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 44:
            {
#if defined(PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 45:
            {
#if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR)
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

#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)
    test_hash_algorithm_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)
    test_mac_algorithm_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)
    test_hmac_algorithm_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)
    test_cipher_algorithm_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)
    test_aead_algorithm_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)
    test_asymmetric_signature_algorithm_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)
    test_asymmetric_signature_wildcard_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)
    test_asymmetric_encryption_algorithm_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)
    test_key_derivation_algorithm_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)
    test_key_agreement_algorithm_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)
    test_pake_algorithm_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)
    test_key_type_wrapper,
#else
    NULL,
#endif
/* Function Id: 12 */

#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)
    test_block_cipher_key_type_wrapper,
#else
    NULL,
#endif
/* Function Id: 13 */

#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)
    test_stream_cipher_key_type_wrapper,
#else
    NULL,
#endif
/* Function Id: 14 */

#if defined(MBEDTLS_PSA_CRYPTO_CLIENT) && defined(PSA_KEY_TYPE_ECC_PUBLIC_KEY) && defined(PSA_KEY_TYPE_ECC_KEY_PAIR)
    test_ecc_key_family_wrapper,
#else
    NULL,
#endif
/* Function Id: 15 */

#if defined(MBEDTLS_PSA_CRYPTO_CLIENT) && defined(MBEDTLS_DHM_C)
    test_dh_key_family_wrapper,
#else
    NULL,
#endif
/* Function Id: 16 */

#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)
    test_lifetime_wrapper,
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
    const char *default_filename = "./test_suite_psa_crypto_metadata.datax";
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
