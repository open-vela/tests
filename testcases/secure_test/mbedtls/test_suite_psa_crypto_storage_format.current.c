#line 2 "suites/main_test.function"
/*
 * *** THIS FILE HAS BEEN MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py
 *
 * Test file      : ./test_suite_psa_crypto_storage_format.current.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_psa_crypto_storage_format.function
 *      Test suite data     : suites/test_suite_psa_crypto_storage_format.current.data
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
#if defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)
#line 2 "suites/test_suite_psa_crypto_storage_format.function"

#include <psa/crypto.h>

#include <test/psa_crypto_helpers.h>
#include <test/psa_exercise_key.h>

#include <psa_crypto_its.h>

#define TEST_FLAG_EXERCISE      0x00000001
#define TEST_FLAG_READ_ONLY     0x00000002

/** Write a key with the given attributes and key material to storage.
 * Test that it has the expected representation.
 *
 * On error, including if the key representation in storage differs,
 * mark the test case as failed and return 0. On success, return 1.
 */
static int test_written_key(const psa_key_attributes_t *attributes,
                            const data_t *material,
                            psa_storage_uid_t uid,
                            const data_t *expected_representation)
{
    mbedtls_svc_key_id_t created_key_id = MBEDTLS_SVC_KEY_ID_INIT;
    uint8_t *actual_representation = NULL;
    size_t length;
    struct psa_storage_info_t storage_info;
    int ok = 0;

    /* Create a key with the given parameters. */
    PSA_ASSERT(psa_import_key(attributes, material->x, material->len,
                              &created_key_id));
    TEST_ASSERT(mbedtls_svc_key_id_equal(psa_get_key_id(attributes),
                                         created_key_id));

    /* Check that the key is represented as expected. */
    PSA_ASSERT(psa_its_get_info(uid, &storage_info));
    TEST_EQUAL(storage_info.size, expected_representation->len);
    ASSERT_ALLOC(actual_representation, storage_info.size);
    PSA_ASSERT(psa_its_get(uid, 0, storage_info.size,
                           actual_representation, &length));
    ASSERT_COMPARE(expected_representation->x, expected_representation->len,
                   actual_representation, length);

    ok = 1;

exit:
    mbedtls_free(actual_representation);
    return ok;
}

/** Check if a key is exportable. */
static int can_export(const psa_key_attributes_t *attributes)
{
    if (psa_get_key_usage_flags(attributes) & PSA_KEY_USAGE_EXPORT) {
        return 1;
    } else if (PSA_KEY_TYPE_IS_PUBLIC_KEY(psa_get_key_type(attributes))) {
        return 1;
    } else {
        return 0;
    }
}

#if defined(MBEDTLS_TEST_LIBTESTDRIVER1)
static int is_accelerated_rsa(psa_algorithm_t alg)
{
#if defined(MBEDTLS_PSA_ACCEL_ALG_RSA_PKCS1V15_SIGN)
    if (PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg)) {
        return 1;
    }
#endif
#if defined(MBEDTLS_PSA_ACCEL_ALG_RSA_PSS)
    if (PSA_ALG_IS_RSA_PSS(alg)) {
        return 1;
    }
#endif
#if defined(MBEDTLS_PSA_ACCEL_ALG_RSA_OAEP)
    if (PSA_ALG_IS_RSA_OAEP(alg)) {
        return 1;
    }
#endif
    (void) alg;
    return 0;
}

/* Whether the algorithm is implemented as a builtin, i.e. not accelerated,
 * and calls mbedtls_md() functions that require the hash algorithm to
 * also be built-in. */
static int is_builtin_calling_md(psa_algorithm_t alg)
{
#if defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PSS)
    if (PSA_ALG_IS_RSA_PSS(alg))
#if defined(MBEDTLS_MD_C)
    { return 1; }
#else
    { return 0; }
#endif
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_OAEP)
    if (PSA_ALG_IS_RSA_OAEP(alg))
#if defined(MBEDTLS_MD_C)
    { return 1; }
#else
    { return 0; }
#endif
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_DETERMINISTIC_ECDSA)
    if (PSA_ALG_IS_DETERMINISTIC_ECDSA(alg)) {
        return 1;
    }
#endif
    (void) alg;
    return 0;
}

static int has_builtin_hash(psa_algorithm_t alg)
{
#if !defined(MBEDTLS_MD5_C)
    if (alg == PSA_ALG_MD5) {
        return 0;
    }
#endif
#if !defined(MBEDTLS_RIPEMD160_C)
    if (alg == PSA_ALG_RIPEMD160) {
        return 0;
    }
#endif
#if !defined(MBEDTLS_SHA1_C)
    if (alg == PSA_ALG_SHA_1) {
        return 0;
    }
#endif
#if !defined(MBEDTLS_SHA224_C)
    if (alg == PSA_ALG_SHA_224) {
        return 0;
    }
#endif
#if !defined(MBEDTLS_SHA256_C)
    if (alg == PSA_ALG_SHA_256) {
        return 0;
    }
#endif
#if !defined(MBEDTLS_SHA384_C)
    if (alg == PSA_ALG_SHA_384) {
        return 0;
    }
#endif
#if !defined(MBEDTLS_SHA512_C)
    if (alg == PSA_ALG_SHA_512) {
        return 0;
    }
#endif
    (void) alg;
    return 1;
}
#endif

/* Mbed TLS doesn't support certain combinations of key type and algorithm
 * in certain configurations. */
static int can_exercise(const psa_key_attributes_t *attributes)
{
    psa_key_type_t key_type = psa_get_key_type(attributes);
    psa_algorithm_t alg = psa_get_key_algorithm(attributes);
    psa_algorithm_t hash_alg =
        PSA_ALG_IS_HASH_AND_SIGN(alg) ? PSA_ALG_SIGN_GET_HASH(alg) :
        PSA_ALG_IS_RSA_OAEP(alg) ? PSA_ALG_RSA_OAEP_GET_HASH(alg) :
        PSA_ALG_NONE;
    psa_key_usage_t usage = psa_get_key_usage_flags(attributes);

#if defined(MBEDTLS_TEST_LIBTESTDRIVER1)
    /* We test some configurations using drivers where the driver doesn't
     * support certain hash algorithms, but declares that it supports
     * compound algorithms that use those hashes. Until this is fixed,
     * in those configurations, don't try to actually perform operations.
     *
     * Hash-and-sign algorithms where the asymmetric part doesn't use
     * a hash operation are ok. So randomized ECDSA signature is fine,
     * ECDSA verification is fine, but deterministic ECDSA signature is
     * affected. All RSA signatures are affected except raw PKCS#1v1.5.
     * OAEP is also affected.
     */
    if (PSA_ALG_IS_DETERMINISTIC_ECDSA(alg) &&
        !(usage & (PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_SIGN_MESSAGE))) {
        /* Verification only. Verification doesn't use the hash algorithm. */
        return 1;
    }

#if defined(MBEDTLS_PSA_ACCEL_ALG_DETERMINISTIC_ECDSA)
    if (PSA_ALG_IS_DETERMINISTIC_ECDSA(alg) &&
        (hash_alg == PSA_ALG_MD5 ||
         hash_alg == PSA_ALG_RIPEMD160 ||
         hash_alg == PSA_ALG_SHA_1)) {
        return 0;
    }
#endif
    if (is_accelerated_rsa(alg) &&
        (hash_alg == PSA_ALG_RIPEMD160 || hash_alg == PSA_ALG_SHA_384)) {
        return 0;
    }
#if defined(MBEDTLS_PSA_ACCEL_ALG_RSA_OAEP)
    if (PSA_ALG_IS_RSA_OAEP(alg) &&
        (hash_alg == PSA_ALG_RIPEMD160 || hash_alg == PSA_ALG_SHA_384)) {
        return 0;
    }
#endif

    /* The built-in implementation of asymmetric algorithms that use a
     * hash internally only dispatch to the internal md module, not to
     * PSA. Until this is supported, don't try to actually perform
     * operations when the operation is built-in and the hash isn't.  */
    if (is_builtin_calling_md(alg) && !has_builtin_hash(hash_alg)) {
        return 0;
    }
#endif /* MBEDTLS_TEST_LIBTESTDRIVER1 */

    (void) key_type;
    (void) alg;
    (void) hash_alg;
    (void) usage;
    return 1;
}

/** Write a key with the given representation to storage, then check
 * that it has the given attributes and (if exportable) key material.
 *
 * On error, including if the key representation in storage differs,
 * mark the test case as failed and return 0. On success, return 1.
 */
static int test_read_key(const psa_key_attributes_t *expected_attributes,
                         const data_t *expected_material,
                         psa_storage_uid_t uid,
                         const data_t *representation,
                         int flags)
{
    psa_key_attributes_t actual_attributes = PSA_KEY_ATTRIBUTES_INIT;
    mbedtls_svc_key_id_t key_id = psa_get_key_id(expected_attributes);
    struct psa_storage_info_t storage_info;
    int ok = 0;
    uint8_t *exported_material = NULL;
    size_t length;

    /* Prime the storage with a key file. */
    PSA_ASSERT(psa_its_set(uid, representation->len, representation->x, 0));

    /* Check that the injected key exists and looks as expected. */
    PSA_ASSERT(psa_get_key_attributes(key_id, &actual_attributes));
    TEST_ASSERT(mbedtls_svc_key_id_equal(key_id,
                                         psa_get_key_id(&actual_attributes)));
    TEST_EQUAL(psa_get_key_lifetime(expected_attributes),
               psa_get_key_lifetime(&actual_attributes));
    TEST_EQUAL(psa_get_key_type(expected_attributes),
               psa_get_key_type(&actual_attributes));
    TEST_EQUAL(psa_get_key_bits(expected_attributes),
               psa_get_key_bits(&actual_attributes));
    TEST_EQUAL(psa_get_key_usage_flags(expected_attributes),
               psa_get_key_usage_flags(&actual_attributes));
    TEST_EQUAL(psa_get_key_algorithm(expected_attributes),
               psa_get_key_algorithm(&actual_attributes));
    TEST_EQUAL(psa_get_key_enrollment_algorithm(expected_attributes),
               psa_get_key_enrollment_algorithm(&actual_attributes));
    if (can_export(expected_attributes)) {
        ASSERT_ALLOC(exported_material, expected_material->len);
        PSA_ASSERT(psa_export_key(key_id,
                                  exported_material, expected_material->len,
                                  &length));
        ASSERT_COMPARE(expected_material->x, expected_material->len,
                       exported_material, length);
    }

    if ((flags & TEST_FLAG_EXERCISE) && can_exercise(&actual_attributes)) {
        TEST_ASSERT(mbedtls_test_psa_exercise_key(
                        key_id,
                        psa_get_key_usage_flags(expected_attributes),
                        psa_get_key_algorithm(expected_attributes)));
    }


    if (flags & TEST_FLAG_READ_ONLY) {
        /* Read-only keys cannot be removed through the API.
         * The key will be removed through ITS in the cleanup code below. */
        TEST_EQUAL(PSA_ERROR_NOT_PERMITTED, psa_destroy_key(key_id));
    } else {
        /* Destroy the key. Confirm through direct access to the storage. */
        PSA_ASSERT(psa_destroy_key(key_id));
        TEST_EQUAL(PSA_ERROR_DOES_NOT_EXIST,
                   psa_its_get_info(uid, &storage_info));
    }

    ok = 1;

exit:
    psa_reset_key_attributes(&actual_attributes);
    psa_its_remove(uid);
    mbedtls_free(exported_material);
    return ok;
}

#line 306 "suites/test_suite_psa_crypto_storage_format.function"
static void test_key_storage_save(int lifetime_arg, int type_arg, int bits_arg,
                      int usage_arg, int alg_arg, int alg2_arg,
                      data_t *material,
                      data_t *representation)
{
    /* Forward compatibility: save a key in the current format and
     * check that it has the expected format so that future versions
     * will still be able to read it. */

    psa_key_lifetime_t lifetime = lifetime_arg;
    psa_key_type_t type = type_arg;
    size_t bits = bits_arg;
    psa_key_usage_t usage = usage_arg;
    psa_algorithm_t alg = alg_arg;
    psa_algorithm_t alg2 = alg2_arg;
    mbedtls_svc_key_id_t key_id = mbedtls_svc_key_id_make(0, 1);
    psa_storage_uid_t uid = 1;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    PSA_INIT();
    TEST_USES_KEY_ID(key_id);

    psa_set_key_lifetime(&attributes, lifetime);
    psa_set_key_id(&attributes, key_id);
    psa_set_key_type(&attributes, type);
    psa_set_key_bits(&attributes, bits);
    psa_set_key_usage_flags(&attributes, usage);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_enrollment_algorithm(&attributes, alg2);

    /* This is the current storage format. Test that we know exactly how
     * the key is stored. The stability of the test data in future
     * versions of Mbed TLS will guarantee that future versions
     * can read back what this version wrote. */
    TEST_ASSERT(test_written_key(&attributes, material,
                                 uid, representation));

exit:
    psa_reset_key_attributes(&attributes);
    psa_destroy_key(key_id);
    PSA_DONE();
}

static void test_key_storage_save_wrapper( void ** params )
{
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_key_storage_save( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), &data6, &data8 );
}
#line 351 "suites/test_suite_psa_crypto_storage_format.function"
static void test_key_storage_read(int lifetime_arg, int type_arg, int bits_arg,
                      int usage_arg, int alg_arg, int alg2_arg,
                      data_t *material,
                      data_t *representation, int flags)
{
    /* Backward compatibility: read a key in the format of a past version
     * and check that this version can use it. */

    psa_key_lifetime_t lifetime = lifetime_arg;
    psa_key_type_t type = type_arg;
    size_t bits = bits_arg;
    psa_key_usage_t usage = usage_arg;
    psa_algorithm_t alg = alg_arg;
    psa_algorithm_t alg2 = alg2_arg;
    mbedtls_svc_key_id_t key_id = mbedtls_svc_key_id_make(0, 1);
    psa_storage_uid_t uid = 1;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    PSA_INIT();
    TEST_USES_KEY_ID(key_id);

    psa_set_key_lifetime(&attributes, lifetime);
    psa_set_key_id(&attributes, key_id);
    psa_set_key_type(&attributes, type);
    psa_set_key_bits(&attributes, bits);
    psa_set_key_usage_flags(&attributes, usage);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_enrollment_algorithm(&attributes, alg2);

    /* Test that we can use a key with the given representation. This
     * guarantees backward compatibility with keys that were stored by
     * past versions of Mbed TLS. */
    TEST_ASSERT(test_read_key(&attributes, material,
                              uid, representation, flags));

exit:
    psa_reset_key_attributes(&attributes);
    PSA_DONE();
}

static void test_key_storage_read_wrapper( void ** params )
{
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_key_storage_read( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), &data6, &data8, *( (int *) params[10] ) );
}
#endif /* MBEDTLS_PSA_CRYPTO_STORAGE_C */
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
    
#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)

        case 0:
            {
                *out_value = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_DEFAULT, PSA_KEY_LOCATION_LOCAL_STORAGE);
            }
            break;
        case 1:
            {
                *out_value = PSA_KEY_TYPE_RAW_DATA;
            }
            break;
        case 2:
            {
                *out_value = PSA_KEY_USAGE_EXPORT;
            }
            break;
        case 3:
            {
                *out_value = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(2, PSA_KEY_LOCATION_LOCAL_STORAGE);
            }
            break;
        case 4:
            {
                *out_value = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(254, PSA_KEY_LOCATION_LOCAL_STORAGE);
            }
            break;
        case 5:
            {
                *out_value = PSA_KEY_LIFETIME_PERSISTENT;
            }
            break;
        case 6:
            {
                *out_value = PSA_KEY_USAGE_COPY;
            }
            break;
        case 7:
            {
                *out_value = PSA_KEY_USAGE_DECRYPT;
            }
            break;
        case 8:
            {
                *out_value = PSA_KEY_USAGE_DERIVE;
            }
            break;
        case 9:
            {
                *out_value = PSA_KEY_USAGE_ENCRYPT;
            }
            break;
        case 10:
            {
                *out_value = PSA_KEY_USAGE_SIGN_HASH;
            }
            break;
        case 11:
            {
                *out_value = PSA_KEY_USAGE_SIGN_MESSAGE;
            }
            break;
        case 12:
            {
                *out_value = PSA_KEY_USAGE_VERIFY_DERIVATION;
            }
            break;
        case 13:
            {
                *out_value = PSA_KEY_USAGE_VERIFY_HASH;
            }
            break;
        case 14:
            {
                *out_value = PSA_KEY_USAGE_VERIFY_MESSAGE;
            }
            break;
        case 15:
            {
                *out_value = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_DECRYPT;
            }
            break;
        case 16:
            {
                *out_value = PSA_KEY_USAGE_DECRYPT | PSA_KEY_USAGE_DERIVE;
            }
            break;
        case 17:
            {
                *out_value = PSA_KEY_USAGE_DERIVE | PSA_KEY_USAGE_ENCRYPT;
            }
            break;
        case 18:
            {
                *out_value = PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_EXPORT;
            }
            break;
        case 19:
            {
                *out_value = PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_SIGN_HASH;
            }
            break;
        case 20:
            {
                *out_value = PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_SIGN_MESSAGE;
            }
            break;
        case 21:
            {
                *out_value = PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_DERIVATION;
            }
            break;
        case 22:
            {
                *out_value = PSA_KEY_USAGE_VERIFY_DERIVATION | PSA_KEY_USAGE_VERIFY_HASH;
            }
            break;
        case 23:
            {
                *out_value = PSA_KEY_USAGE_VERIFY_HASH | PSA_KEY_USAGE_VERIFY_MESSAGE;
            }
            break;
        case 24:
            {
                *out_value = PSA_KEY_USAGE_VERIFY_MESSAGE | PSA_KEY_USAGE_COPY;
            }
            break;
        case 25:
            {
                *out_value = PSA_KEY_USAGE_COPY | PSA_KEY_USAGE_DECRYPT | PSA_KEY_USAGE_DERIVE | PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_DERIVATION | PSA_KEY_USAGE_VERIFY_HASH | PSA_KEY_USAGE_VERIFY_MESSAGE;
            }
            break;
        case 26:
            {
                *out_value = PSA_KEY_TYPE_AES;
            }
            break;
        case 27:
            {
                *out_value = PSA_KEY_USAGE_DECRYPT | PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_EXPORT;
            }
            break;
        case 28:
            {
                *out_value = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM,4);
            }
            break;
        case 29:
            {
                *out_value = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM,14);
            }
            break;
        case 30:
            {
                *out_value = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM,16);
            }
            break;
        case 31:
            {
                *out_value = PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_HASH | PSA_KEY_USAGE_VERIFY_MESSAGE;
            }
            break;
        case 32:
            {
                *out_value = PSA_ALG_CBC_MAC;
            }
            break;
        case 33:
            {
                *out_value = PSA_ALG_CBC_NO_PADDING;
            }
            break;
        case 34:
            {
                *out_value = PSA_ALG_CBC_PKCS7;
            }
            break;
        case 35:
            {
                *out_value = PSA_ALG_CCM;
            }
            break;
        case 36:
            {
                *out_value = PSA_ALG_CCM_STAR_NO_TAG;
            }
            break;
        case 37:
            {
                *out_value = PSA_ALG_CFB;
            }
            break;
        case 38:
            {
                *out_value = PSA_ALG_CMAC;
            }
            break;
        case 39:
            {
                *out_value = PSA_ALG_CTR;
            }
            break;
        case 40:
            {
                *out_value = PSA_ALG_ECB_NO_PADDING;
            }
            break;
        case 41:
            {
                *out_value = PSA_ALG_GCM;
            }
            break;
        case 42:
            {
                *out_value = PSA_ALG_OFB;
            }
            break;
        case 43:
            {
                *out_value = PSA_ALG_TRUNCATED_MAC(PSA_ALG_CBC_MAC,4);
            }
            break;
        case 44:
            {
                *out_value = PSA_ALG_TRUNCATED_MAC(PSA_ALG_CBC_MAC,13);
            }
            break;
        case 45:
            {
                *out_value = PSA_ALG_TRUNCATED_MAC(PSA_ALG_CBC_MAC,14);
            }
            break;
        case 46:
            {
                *out_value = PSA_ALG_TRUNCATED_MAC(PSA_ALG_CBC_MAC,16);
            }
            break;
        case 47:
            {
                *out_value = PSA_ALG_XTS;
            }
            break;
        case 48:
            {
                *out_value = PSA_KEY_TYPE_ARIA;
            }
            break;
        case 49:
            {
                *out_value = PSA_KEY_TYPE_CAMELLIA;
            }
            break;
        case 50:
            {
                *out_value = PSA_KEY_TYPE_CHACHA20;
            }
            break;
        case 51:
            {
                *out_value = PSA_ALG_CHACHA20_POLY1305;
            }
            break;
        case 52:
            {
                *out_value = PSA_ALG_STREAM_CIPHER;
            }
            break;
        case 53:
            {
                *out_value = PSA_KEY_TYPE_DERIVE;
            }
            break;
        case 54:
            {
                *out_value = PSA_KEY_TYPE_DES;
            }
            break;
        case 55:
            {
                *out_value = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_BRAINPOOL_P_R1);
            }
            break;
        case 56:
            {
                *out_value = PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_MD5);
            }
            break;
        case 57:
            {
                *out_value = PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_RIPEMD160);
            }
            break;
        case 58:
            {
                *out_value = PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_1);
            }
            break;
        case 59:
            {
                *out_value = PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_224);
            }
            break;
        case 60:
            {
                *out_value = PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_256);
            }
            break;
        case 61:
            {
                *out_value = PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_384);
            }
            break;
        case 62:
            {
                *out_value = PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_512);
            }
            break;
        case 63:
            {
                *out_value = PSA_KEY_USAGE_DERIVE | PSA_KEY_USAGE_EXPORT;
            }
            break;
        case 64:
            {
                *out_value = PSA_ALG_ECDH;
            }
            break;
        case 65:
            {
                *out_value = PSA_ALG_ECDSA(PSA_ALG_MD5);
            }
            break;
        case 66:
            {
                *out_value = PSA_ALG_ECDSA(PSA_ALG_RIPEMD160);
            }
            break;
        case 67:
            {
                *out_value = PSA_ALG_ECDSA(PSA_ALG_SHA_1);
            }
            break;
        case 68:
            {
                *out_value = PSA_ALG_ECDSA(PSA_ALG_SHA_224);
            }
            break;
        case 69:
            {
                *out_value = PSA_ALG_ECDSA(PSA_ALG_SHA_256);
            }
            break;
        case 70:
            {
                *out_value = PSA_ALG_ECDSA(PSA_ALG_SHA_384);
            }
            break;
        case 71:
            {
                *out_value = PSA_ALG_ECDSA(PSA_ALG_SHA_512);
            }
            break;
        case 72:
            {
                *out_value = PSA_ALG_ECDSA_ANY;
            }
            break;
        case 73:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH,PSA_ALG_HKDF(PSA_ALG_SHA_256));
            }
            break;
        case 74:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH,PSA_ALG_HKDF(PSA_ALG_SHA_384));
            }
            break;
        case 75:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH,PSA_ALG_HKDF_EXPAND(PSA_ALG_SHA_256));
            }
            break;
        case 76:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH,PSA_ALG_HKDF_EXPAND(PSA_ALG_SHA_384));
            }
            break;
        case 77:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH,PSA_ALG_HKDF_EXTRACT(PSA_ALG_SHA_256));
            }
            break;
        case 78:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH,PSA_ALG_HKDF_EXTRACT(PSA_ALG_SHA_384));
            }
            break;
        case 79:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH,PSA_ALG_TLS12_PRF(PSA_ALG_SHA_256));
            }
            break;
        case 80:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH,PSA_ALG_TLS12_PRF(PSA_ALG_SHA_384));
            }
            break;
        case 81:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH,PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_SHA_256));
            }
            break;
        case 82:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH,PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_SHA_384));
            }
            break;
        case 83:
            {
                *out_value = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_MONTGOMERY);
            }
            break;
        case 84:
            {
                *out_value = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_K1);
            }
            break;
        case 85:
            {
                *out_value = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1);
            }
            break;
        case 86:
            {
                *out_value = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R2);
            }
            break;
        case 87:
            {
                *out_value = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECT_K1);
            }
            break;
        case 88:
            {
                *out_value = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECT_R1);
            }
            break;
        case 89:
            {
                *out_value = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECT_R2);
            }
            break;
        case 90:
            {
                *out_value = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_TWISTED_EDWARDS);
            }
            break;
        case 91:
            {
                *out_value = PSA_ALG_ED25519PH;
            }
            break;
        case 92:
            {
                *out_value = PSA_ALG_ED448PH;
            }
            break;
        case 93:
            {
                *out_value = PSA_ALG_PURE_EDDSA;
            }
            break;
        case 94:
            {
                *out_value = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_BRAINPOOL_P_R1);
            }
            break;
        case 95:
            {
                *out_value = PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_VERIFY_HASH | PSA_KEY_USAGE_VERIFY_MESSAGE;
            }
            break;
        case 96:
            {
                *out_value = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_MONTGOMERY);
            }
            break;
        case 97:
            {
                *out_value = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_K1);
            }
            break;
        case 98:
            {
                *out_value = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1);
            }
            break;
        case 99:
            {
                *out_value = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R2);
            }
            break;
        case 100:
            {
                *out_value = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECT_K1);
            }
            break;
        case 101:
            {
                *out_value = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECT_R1);
            }
            break;
        case 102:
            {
                *out_value = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECT_R2);
            }
            break;
        case 103:
            {
                *out_value = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_TWISTED_EDWARDS);
            }
            break;
        case 104:
            {
                *out_value = PSA_KEY_TYPE_HMAC;
            }
            break;
        case 105:
            {
                *out_value = PSA_ALG_HMAC(PSA_ALG_MD5);
            }
            break;
        case 106:
            {
                *out_value = PSA_ALG_HMAC(PSA_ALG_RIPEMD160);
            }
            break;
        case 107:
            {
                *out_value = PSA_ALG_HMAC(PSA_ALG_SHA_1);
            }
            break;
        case 108:
            {
                *out_value = PSA_ALG_HMAC(PSA_ALG_SHA_224);
            }
            break;
        case 109:
            {
                *out_value = PSA_ALG_HMAC(PSA_ALG_SHA_256);
            }
            break;
        case 110:
            {
                *out_value = PSA_ALG_HMAC(PSA_ALG_SHA_384);
            }
            break;
        case 111:
            {
                *out_value = PSA_ALG_HMAC(PSA_ALG_SHA_512);
            }
            break;
        case 112:
            {
                *out_value = PSA_KEY_TYPE_PASSWORD;
            }
            break;
        case 113:
            {
                *out_value = PSA_KEY_TYPE_PASSWORD_HASH;
            }
            break;
        case 114:
            {
                *out_value = PSA_KEY_TYPE_PEPPER;
            }
            break;
        case 115:
            {
                *out_value = PSA_KEY_TYPE_RSA_KEY_PAIR;
            }
            break;
        case 116:
            {
                *out_value = PSA_ALG_RSA_OAEP(PSA_ALG_MD5);
            }
            break;
        case 117:
            {
                *out_value = PSA_ALG_RSA_OAEP(PSA_ALG_RIPEMD160);
            }
            break;
        case 118:
            {
                *out_value = PSA_ALG_RSA_OAEP(PSA_ALG_SHA_1);
            }
            break;
        case 119:
            {
                *out_value = PSA_ALG_RSA_OAEP(PSA_ALG_SHA_224);
            }
            break;
        case 120:
            {
                *out_value = PSA_ALG_RSA_OAEP(PSA_ALG_SHA_256);
            }
            break;
        case 121:
            {
                *out_value = PSA_ALG_RSA_OAEP(PSA_ALG_SHA_384);
            }
            break;
        case 122:
            {
                *out_value = PSA_ALG_RSA_OAEP(PSA_ALG_SHA_512);
            }
            break;
        case 123:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_CRYPT;
            }
            break;
        case 124:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_MD5);
            }
            break;
        case 125:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_RIPEMD160);
            }
            break;
        case 126:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_1);
            }
            break;
        case 127:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_224);
            }
            break;
        case 128:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256);
            }
            break;
        case 129:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_384);
            }
            break;
        case 130:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_512);
            }
            break;
        case 131:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_SIGN_RAW;
            }
            break;
        case 132:
            {
                *out_value = PSA_ALG_RSA_PSS(PSA_ALG_MD5);
            }
            break;
        case 133:
            {
                *out_value = PSA_ALG_RSA_PSS(PSA_ALG_RIPEMD160);
            }
            break;
        case 134:
            {
                *out_value = PSA_ALG_RSA_PSS(PSA_ALG_SHA_1);
            }
            break;
        case 135:
            {
                *out_value = PSA_ALG_RSA_PSS(PSA_ALG_SHA_224);
            }
            break;
        case 136:
            {
                *out_value = PSA_ALG_RSA_PSS(PSA_ALG_SHA_256);
            }
            break;
        case 137:
            {
                *out_value = PSA_ALG_RSA_PSS(PSA_ALG_SHA_384);
            }
            break;
        case 138:
            {
                *out_value = PSA_ALG_RSA_PSS(PSA_ALG_SHA_512);
            }
            break;
        case 139:
            {
                *out_value = PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_MD5);
            }
            break;
        case 140:
            {
                *out_value = PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_RIPEMD160);
            }
            break;
        case 141:
            {
                *out_value = PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_SHA_1);
            }
            break;
        case 142:
            {
                *out_value = PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_SHA_224);
            }
            break;
        case 143:
            {
                *out_value = PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_SHA_256);
            }
            break;
        case 144:
            {
                *out_value = PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_SHA_384);
            }
            break;
        case 145:
            {
                *out_value = PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_SHA_512);
            }
            break;
        case 146:
            {
                *out_value = PSA_KEY_TYPE_RSA_PUBLIC_KEY;
            }
            break;
        case 147:
            {
                *out_value = PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(PSA_ALG_CCM, 1);
            }
            break;
        case 148:
            {
                *out_value = PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(PSA_ALG_CHACHA20_POLY1305, 1);
            }
            break;
        case 149:
            {
                *out_value = PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(PSA_ALG_GCM, 1);
            }
            break;
        case 150:
            {
                *out_value = PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(PSA_ALG_CCM, 4);
            }
            break;
        case 151:
            {
                *out_value = PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(PSA_ALG_CCM, 13);
            }
            break;
        case 152:
            {
                *out_value = PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(PSA_ALG_CCM, 14);
            }
            break;
        case 153:
            {
                *out_value = PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(PSA_ALG_CCM, 16);
            }
            break;
        case 154:
            {
                *out_value = PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(PSA_ALG_CCM, 63);
            }
            break;
        case 155:
            {
                *out_value = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 1);
            }
            break;
        case 156:
            {
                *out_value = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CHACHA20_POLY1305, 1);
            }
            break;
        case 157:
            {
                *out_value = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 1);
            }
            break;
        case 158:
            {
                *out_value = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 4);
            }
            break;
        case 159:
            {
                *out_value = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 13);
            }
            break;
        case 160:
            {
                *out_value = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 14);
            }
            break;
        case 161:
            {
                *out_value = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 16);
            }
            break;
        case 162:
            {
                *out_value = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 63);
            }
            break;
        case 163:
            {
                *out_value = PSA_ALG_ANY_HASH;
            }
            break;
        case 164:
            {
                *out_value = PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(PSA_ALG_CBC_MAC, 1);
            }
            break;
        case 165:
            {
                *out_value = PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(PSA_ALG_CMAC, 1);
            }
            break;
        case 166:
            {
                *out_value = PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(PSA_ALG_HMAC(PSA_ALG_MD5), 1);
            }
            break;
        case 167:
            {
                *out_value = PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(PSA_ALG_HMAC(PSA_ALG_RIPEMD160), 1);
            }
            break;
        case 168:
            {
                *out_value = PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(PSA_ALG_HMAC(PSA_ALG_SHA_1), 1);
            }
            break;
        case 169:
            {
                *out_value = PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(PSA_ALG_HMAC(PSA_ALG_SHA_224), 1);
            }
            break;
        case 170:
            {
                *out_value = PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(PSA_ALG_HMAC(PSA_ALG_SHA_256), 1);
            }
            break;
        case 171:
            {
                *out_value = PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(PSA_ALG_HMAC(PSA_ALG_SHA_384), 1);
            }
            break;
        case 172:
            {
                *out_value = PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(PSA_ALG_HMAC(PSA_ALG_SHA_512), 1);
            }
            break;
        case 173:
            {
                *out_value = PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(PSA_ALG_CBC_MAC, 4);
            }
            break;
        case 174:
            {
                *out_value = PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(PSA_ALG_CBC_MAC, 13);
            }
            break;
        case 175:
            {
                *out_value = PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(PSA_ALG_CBC_MAC, 14);
            }
            break;
        case 176:
            {
                *out_value = PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(PSA_ALG_CBC_MAC, 16);
            }
            break;
        case 177:
            {
                *out_value = PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(PSA_ALG_CBC_MAC, 63);
            }
            break;
        case 178:
            {
                *out_value = PSA_ALG_DETERMINISTIC_DSA(PSA_ALG_MD5);
            }
            break;
        case 179:
            {
                *out_value = PSA_ALG_DETERMINISTIC_DSA(PSA_ALG_RIPEMD160);
            }
            break;
        case 180:
            {
                *out_value = PSA_ALG_DETERMINISTIC_DSA(PSA_ALG_SHA_1);
            }
            break;
        case 181:
            {
                *out_value = PSA_ALG_DETERMINISTIC_DSA(PSA_ALG_SHA_224);
            }
            break;
        case 182:
            {
                *out_value = PSA_ALG_DETERMINISTIC_DSA(PSA_ALG_SHA_256);
            }
            break;
        case 183:
            {
                *out_value = PSA_ALG_DETERMINISTIC_DSA(PSA_ALG_SHA_384);
            }
            break;
        case 184:
            {
                *out_value = PSA_ALG_DETERMINISTIC_DSA(PSA_ALG_SHA_512);
            }
            break;
        case 185:
            {
                *out_value = PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_ANY_HASH);
            }
            break;
        case 186:
            {
                *out_value = PSA_ALG_DSA(PSA_ALG_MD5);
            }
            break;
        case 187:
            {
                *out_value = PSA_ALG_DSA(PSA_ALG_RIPEMD160);
            }
            break;
        case 188:
            {
                *out_value = PSA_ALG_DSA(PSA_ALG_SHA_1);
            }
            break;
        case 189:
            {
                *out_value = PSA_ALG_DSA(PSA_ALG_SHA_224);
            }
            break;
        case 190:
            {
                *out_value = PSA_ALG_DSA(PSA_ALG_SHA_256);
            }
            break;
        case 191:
            {
                *out_value = PSA_ALG_DSA(PSA_ALG_SHA_384);
            }
            break;
        case 192:
            {
                *out_value = PSA_ALG_DSA(PSA_ALG_SHA_512);
            }
            break;
        case 193:
            {
                *out_value = PSA_ALG_ECDSA(PSA_ALG_ANY_HASH);
            }
            break;
        case 194:
            {
                *out_value = PSA_ALG_FFDH;
            }
            break;
        case 195:
            {
                *out_value = PSA_ALG_HKDF(PSA_ALG_MD5);
            }
            break;
        case 196:
            {
                *out_value = PSA_ALG_HKDF(PSA_ALG_RIPEMD160);
            }
            break;
        case 197:
            {
                *out_value = PSA_ALG_HKDF(PSA_ALG_SHA_1);
            }
            break;
        case 198:
            {
                *out_value = PSA_ALG_HKDF(PSA_ALG_SHA_224);
            }
            break;
        case 199:
            {
                *out_value = PSA_ALG_HKDF(PSA_ALG_SHA_256);
            }
            break;
        case 200:
            {
                *out_value = PSA_ALG_HKDF(PSA_ALG_SHA_384);
            }
            break;
        case 201:
            {
                *out_value = PSA_ALG_HKDF(PSA_ALG_SHA_512);
            }
            break;
        case 202:
            {
                *out_value = PSA_ALG_HKDF_EXPAND(PSA_ALG_MD5);
            }
            break;
        case 203:
            {
                *out_value = PSA_ALG_HKDF_EXPAND(PSA_ALG_RIPEMD160);
            }
            break;
        case 204:
            {
                *out_value = PSA_ALG_HKDF_EXPAND(PSA_ALG_SHA_1);
            }
            break;
        case 205:
            {
                *out_value = PSA_ALG_HKDF_EXPAND(PSA_ALG_SHA_224);
            }
            break;
        case 206:
            {
                *out_value = PSA_ALG_HKDF_EXPAND(PSA_ALG_SHA_256);
            }
            break;
        case 207:
            {
                *out_value = PSA_ALG_HKDF_EXPAND(PSA_ALG_SHA_384);
            }
            break;
        case 208:
            {
                *out_value = PSA_ALG_HKDF_EXPAND(PSA_ALG_SHA_512);
            }
            break;
        case 209:
            {
                *out_value = PSA_ALG_HKDF_EXTRACT(PSA_ALG_MD5);
            }
            break;
        case 210:
            {
                *out_value = PSA_ALG_HKDF_EXTRACT(PSA_ALG_RIPEMD160);
            }
            break;
        case 211:
            {
                *out_value = PSA_ALG_HKDF_EXTRACT(PSA_ALG_SHA_1);
            }
            break;
        case 212:
            {
                *out_value = PSA_ALG_HKDF_EXTRACT(PSA_ALG_SHA_224);
            }
            break;
        case 213:
            {
                *out_value = PSA_ALG_HKDF_EXTRACT(PSA_ALG_SHA_256);
            }
            break;
        case 214:
            {
                *out_value = PSA_ALG_HKDF_EXTRACT(PSA_ALG_SHA_384);
            }
            break;
        case 215:
            {
                *out_value = PSA_ALG_HKDF_EXTRACT(PSA_ALG_SHA_512);
            }
            break;
        case 216:
            {
                *out_value = PSA_ALG_JPAKE;
            }
            break;
        case 217:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256));
            }
            break;
        case 218:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_FFDH, PSA_ALG_HKDF(PSA_ALG_SHA_256));
            }
            break;
        case 219:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_384));
            }
            break;
        case 220:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF_EXPAND(PSA_ALG_SHA_256));
            }
            break;
        case 221:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF_EXPAND(PSA_ALG_SHA_384));
            }
            break;
        case 222:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF_EXTRACT(PSA_ALG_SHA_256));
            }
            break;
        case 223:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF_EXTRACT(PSA_ALG_SHA_384));
            }
            break;
        case 224:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_TLS12_ECJPAKE_TO_PMS);
            }
            break;
        case 225:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_TLS12_PRF(PSA_ALG_SHA_256));
            }
            break;
        case 226:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_TLS12_PRF(PSA_ALG_SHA_384));
            }
            break;
        case 227:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_SHA_256));
            }
            break;
        case 228:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_SHA_384));
            }
            break;
        case 229:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_FFDH, PSA_ALG_HKDF(PSA_ALG_SHA_384));
            }
            break;
        case 230:
            {
                *out_value = PSA_ALG_MD5;
            }
            break;
        case 231:
            {
                *out_value = PSA_ALG_PBKDF2_AES_CMAC_PRF_128;
            }
            break;
        case 232:
            {
                *out_value = PSA_ALG_PBKDF2_HMAC(PSA_ALG_MD5);
            }
            break;
        case 233:
            {
                *out_value = PSA_ALG_PBKDF2_HMAC(PSA_ALG_RIPEMD160);
            }
            break;
        case 234:
            {
                *out_value = PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_1);
            }
            break;
        case 235:
            {
                *out_value = PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_224);
            }
            break;
        case 236:
            {
                *out_value = PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_256);
            }
            break;
        case 237:
            {
                *out_value = PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_384);
            }
            break;
        case 238:
            {
                *out_value = PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_512);
            }
            break;
        case 239:
            {
                *out_value = PSA_ALG_RIPEMD160;
            }
            break;
        case 240:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH);
            }
            break;
        case 241:
            {
                *out_value = PSA_ALG_RSA_PSS(PSA_ALG_ANY_HASH);
            }
            break;
        case 242:
            {
                *out_value = PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_ANY_HASH);
            }
            break;
        case 243:
            {
                *out_value = PSA_ALG_SHA3_224;
            }
            break;
        case 244:
            {
                *out_value = PSA_ALG_SHA3_256;
            }
            break;
        case 245:
            {
                *out_value = PSA_ALG_SHA3_384;
            }
            break;
        case 246:
            {
                *out_value = PSA_ALG_SHA3_512;
            }
            break;
        case 247:
            {
                *out_value = PSA_ALG_SHAKE256_512;
            }
            break;
        case 248:
            {
                *out_value = PSA_ALG_SHA_1;
            }
            break;
        case 249:
            {
                *out_value = PSA_ALG_SHA_224;
            }
            break;
        case 250:
            {
                *out_value = PSA_ALG_SHA_256;
            }
            break;
        case 251:
            {
                *out_value = PSA_ALG_SHA_384;
            }
            break;
        case 252:
            {
                *out_value = PSA_ALG_SHA_512;
            }
            break;
        case 253:
            {
                *out_value = PSA_ALG_SHA_512_224;
            }
            break;
        case 254:
            {
                *out_value = PSA_ALG_SHA_512_256;
            }
            break;
        case 255:
            {
                *out_value = PSA_ALG_TLS12_ECJPAKE_TO_PMS;
            }
            break;
        case 256:
            {
                *out_value = PSA_ALG_TLS12_PRF(PSA_ALG_MD5);
            }
            break;
        case 257:
            {
                *out_value = PSA_ALG_TLS12_PRF(PSA_ALG_RIPEMD160);
            }
            break;
        case 258:
            {
                *out_value = PSA_ALG_TLS12_PRF(PSA_ALG_SHA_1);
            }
            break;
        case 259:
            {
                *out_value = PSA_ALG_TLS12_PRF(PSA_ALG_SHA_224);
            }
            break;
        case 260:
            {
                *out_value = PSA_ALG_TLS12_PRF(PSA_ALG_SHA_256);
            }
            break;
        case 261:
            {
                *out_value = PSA_ALG_TLS12_PRF(PSA_ALG_SHA_384);
            }
            break;
        case 262:
            {
                *out_value = PSA_ALG_TLS12_PRF(PSA_ALG_SHA_512);
            }
            break;
        case 263:
            {
                *out_value = PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_MD5);
            }
            break;
        case 264:
            {
                *out_value = PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_RIPEMD160);
            }
            break;
        case 265:
            {
                *out_value = PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_SHA_1);
            }
            break;
        case 266:
            {
                *out_value = PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_SHA_224);
            }
            break;
        case 267:
            {
                *out_value = PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_SHA_256);
            }
            break;
        case 268:
            {
                *out_value = PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_SHA_384);
            }
            break;
        case 269:
            {
                *out_value = PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_SHA_512);
            }
            break;
        case 270:
            {
                *out_value = PSA_ALG_TRUNCATED_MAC(PSA_ALG_CBC_MAC, 1);
            }
            break;
        case 271:
            {
                *out_value = PSA_ALG_TRUNCATED_MAC(PSA_ALG_CMAC, 1);
            }
            break;
        case 272:
            {
                *out_value = PSA_ALG_TRUNCATED_MAC(PSA_ALG_HMAC(PSA_ALG_MD5), 1);
            }
            break;
        case 273:
            {
                *out_value = PSA_ALG_TRUNCATED_MAC(PSA_ALG_HMAC(PSA_ALG_RIPEMD160), 1);
            }
            break;
        case 274:
            {
                *out_value = PSA_ALG_TRUNCATED_MAC(PSA_ALG_HMAC(PSA_ALG_SHA_1), 1);
            }
            break;
        case 275:
            {
                *out_value = PSA_ALG_TRUNCATED_MAC(PSA_ALG_HMAC(PSA_ALG_SHA_224), 1);
            }
            break;
        case 276:
            {
                *out_value = PSA_ALG_TRUNCATED_MAC(PSA_ALG_HMAC(PSA_ALG_SHA_256), 1);
            }
            break;
        case 277:
            {
                *out_value = PSA_ALG_TRUNCATED_MAC(PSA_ALG_HMAC(PSA_ALG_SHA_384), 1);
            }
            break;
        case 278:
            {
                *out_value = PSA_ALG_TRUNCATED_MAC(PSA_ALG_HMAC(PSA_ALG_SHA_512), 1);
            }
            break;
        case 279:
            {
                *out_value = PSA_ALG_TRUNCATED_MAC(PSA_ALG_CBC_MAC, 4);
            }
            break;
        case 280:
            {
                *out_value = PSA_ALG_TRUNCATED_MAC(PSA_ALG_CBC_MAC, 13);
            }
            break;
        case 281:
            {
                *out_value = PSA_ALG_TRUNCATED_MAC(PSA_ALG_CBC_MAC, 14);
            }
            break;
        case 282:
            {
                *out_value = PSA_ALG_TRUNCATED_MAC(PSA_ALG_CBC_MAC, 16);
            }
            break;
        case 283:
            {
                *out_value = PSA_ALG_TRUNCATED_MAC(PSA_ALG_CBC_MAC, 63);
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
    
#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)

        case 0:
            {
#if defined(PSA_WANT_KEY_TYPE_RAW_DATA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(PSA_WANT_KEY_TYPE_AES)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(PSA_WANT_ALG_CCM)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 3:
            {
#if defined(PSA_WANT_ALG_CBC_MAC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 4:
            {
#if defined(PSA_WANT_ALG_CBC_NO_PADDING)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 5:
            {
#if defined(PSA_WANT_ALG_CBC_PKCS7)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 6:
            {
#if defined(PSA_WANT_ALG_CCM_STAR_NO_TAG)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 7:
            {
#if defined(PSA_WANT_ALG_CFB)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 8:
            {
#if defined(PSA_WANT_ALG_CMAC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 9:
            {
#if defined(PSA_WANT_ALG_CTR)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 10:
            {
#if defined(PSA_WANT_ALG_ECB_NO_PADDING)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 11:
            {
#if defined(PSA_WANT_ALG_GCM)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 12:
            {
#if defined(PSA_WANT_ALG_OFB)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 13:
            {
#if defined(PSA_WANT_ALG_XTS)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 14:
            {
#if defined(PSA_WANT_KEY_TYPE_ARIA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 15:
            {
#if defined(PSA_WANT_KEY_TYPE_CAMELLIA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 16:
            {
#if defined(PSA_WANT_KEY_TYPE_CHACHA20)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 17:
            {
#if defined(PSA_WANT_ALG_CHACHA20_POLY1305)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 18:
            {
#if defined(PSA_WANT_ALG_STREAM_CIPHER)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 19:
            {
#if defined(PSA_WANT_KEY_TYPE_DERIVE)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 20:
            {
#if defined(PSA_WANT_KEY_TYPE_DES)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 21:
            {
#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_160)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 22:
            {
#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 23:
            {
#if defined(PSA_WANT_ALG_DETERMINISTIC_ECDSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 24:
            {
#if defined(PSA_WANT_ALG_MD5)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 25:
            {
#if defined(PSA_WANT_ALG_RIPEMD160)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 26:
            {
#if defined(PSA_WANT_ALG_SHA_1)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 27:
            {
#if defined(PSA_WANT_ALG_SHA_224)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 28:
            {
#if defined(PSA_WANT_ALG_SHA_256)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 29:
            {
#if defined(PSA_WANT_ALG_SHA_384)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 30:
            {
#if defined(PSA_WANT_ALG_SHA_512)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 31:
            {
#if defined(PSA_WANT_ALG_ECDH)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 32:
            {
#if defined(PSA_WANT_ALG_ECDSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 33:
            {
#if defined(PSA_WANT_ALG_ECDSA_ANY)
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
#if defined(PSA_WANT_ALG_HKDF_EXPAND)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 36:
            {
#if defined(PSA_WANT_ALG_HKDF_EXTRACT)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 37:
            {
#if defined(PSA_WANT_ALG_TLS12_PRF)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 38:
            {
#if defined(PSA_WANT_ALG_TLS12_PSK_TO_MS)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 39:
            {
#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_192)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 40:
            {
#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_224)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 41:
            {
#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_256)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 42:
            {
#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_320)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 43:
            {
#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_384)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 44:
            {
#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_512)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 45:
            {
#if defined(PSA_WANT_ECC_MONTGOMERY_255)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 46:
            {
#if defined(PSA_WANT_ECC_MONTGOMERY_448)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 47:
            {
#if defined(PSA_WANT_ECC_SECP_K1_192)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 48:
            {
#if defined(PSA_WANT_ECC_SECP_K1_224)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 49:
            {
#if defined(PSA_WANT_ECC_SECP_K1_256)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 50:
            {
#if defined(PSA_WANT_ECC_SECP_R1_225)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 51:
            {
#if defined(PSA_WANT_ECC_SECP_R1_256)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 52:
            {
#if defined(PSA_WANT_ECC_SECP_R1_384)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 53:
            {
#if defined(PSA_WANT_ECC_SECP_R1_521)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 54:
            {
#if defined(PSA_WANT_ECC_SECP_R2_160)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 55:
            {
#if defined(PSA_WANT_ECC_SECT_K1_163)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 56:
            {
#if defined(PSA_WANT_ECC_SECT_K1_233)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 57:
            {
#if defined(PSA_WANT_ECC_SECT_K1_239)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 58:
            {
#if defined(PSA_WANT_ECC_SECT_K1_283)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 59:
            {
#if defined(PSA_WANT_ECC_SECT_K1_409)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 60:
            {
#if defined(PSA_WANT_ECC_SECT_K1_571)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 61:
            {
#if defined(PSA_WANT_ECC_SECT_R1_163)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 62:
            {
#if defined(PSA_WANT_ECC_SECT_R1_233)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 63:
            {
#if defined(PSA_WANT_ECC_SECT_R1_283)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 64:
            {
#if defined(PSA_WANT_ECC_SECT_R1_409)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 65:
            {
#if defined(PSA_WANT_ECC_SECT_R1_571)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 66:
            {
#if defined(PSA_WANT_ECC_SECT_R2_163)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 67:
            {
#if defined(PSA_WANT_ECC_TWISTED_EDWARDS_255)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 68:
            {
#if defined(PSA_WANT_ALG_ED25519PH)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 69:
            {
#if defined(PSA_WANT_ALG_ED448PH)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 70:
            {
#if defined(PSA_WANT_ALG_PURE_EDDSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 71:
            {
#if defined(PSA_WANT_ECC_TWISTED_EDWARDS_448)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 72:
            {
#if defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 73:
            {
#if defined(PSA_WANT_KEY_TYPE_HMAC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 74:
            {
#if defined(PSA_WANT_ALG_HMAC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 75:
            {
#if defined(PSA_WANT_KEY_TYPE_PASSWORD)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 76:
            {
#if defined(PSA_WANT_KEY_TYPE_PASSWORD_HASH)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 77:
            {
#if defined(PSA_WANT_KEY_TYPE_PEPPER)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 78:
            {
#if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 79:
            {
#if defined(PSA_WANT_ALG_RSA_OAEP)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 80:
            {
#if defined(PSA_WANT_ALG_RSA_PKCS1V15_CRYPT)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 81:
            {
#if defined(PSA_WANT_ALG_RSA_PKCS1V15_SIGN)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 82:
            {
#if defined(PSA_WANT_ALG_RSA_PKCS1V15_SIGN_RAW)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 83:
            {
#if defined(PSA_WANT_ALG_RSA_PSS)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 84:
            {
#if defined(PSA_WANT_ALG_RSA_PSS_ANY_SALT)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 85:
            {
#if defined(PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 86:
            {
#if defined(PSA_WANT_ALG_DETERMINISTIC_DSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 87:
            {
#if defined(PSA_WANT_ALG_DSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 88:
            {
#if defined(PSA_WANT_ALG_FFDH)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 89:
            {
#if defined(PSA_WANT_ALG_JPAKE)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 90:
            {
#if defined(PSA_WANT_ALG_TLS12_ECJPAKE_TO_PMS)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 91:
            {
#if defined(PSA_WANT_ALG_PBKDF2_AES_CMAC_PRF_128)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 92:
            {
#if defined(PSA_WANT_ALG_PBKDF2_HMAC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 93:
            {
#if defined(PSA_WANT_ALG_SHA3_224)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 94:
            {
#if defined(PSA_WANT_ALG_SHA3_256)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 95:
            {
#if defined(PSA_WANT_ALG_SHA3_384)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 96:
            {
#if defined(PSA_WANT_ALG_SHA3_512)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 97:
            {
#if defined(PSA_WANT_ALG_SHAKE256_512)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 98:
            {
#if defined(PSA_WANT_ALG_SHA_512_224)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 99:
            {
#if defined(PSA_WANT_ALG_SHA_512_256)
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

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)
    test_key_storage_save_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)
    test_key_storage_read_wrapper,
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
    const char *default_filename = "./test_suite_psa_crypto_storage_format.current.datax";
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
