#line 2 "suites/main_test.function"
/*
 * *** THIS FILE HAS BEEN MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py
 *
 * Test file      : ./test_suite_psa_crypto_slot_management.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_psa_crypto_slot_management.function
 *      Test suite data     : suites/test_suite_psa_crypto_slot_management.data
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
#line 2 "suites/test_suite_psa_crypto_slot_management.function"
#include <stdint.h>

#include "psa_crypto_slot_management.h"
#include "psa_crypto_storage.h"

typedef enum {
    /**< Close key(s) */
    INVALIDATE_BY_CLOSING,

    /**< Destroy key(s) */
    INVALIDATE_BY_DESTROYING,

    /**< Purge key(s) */
    INVALIDATE_BY_PURGING,

    /**< Terminate and reinitialize without closing/destroying keys */
    INVALIDATE_BY_SHUTDOWN,

    /**< Close key(s) then terminate and re-initialize */
    INVALIDATE_BY_CLOSING_WITH_SHUTDOWN,

    /**< Destroy key(s) then terminate and re-initialize */
    INVALIDATE_BY_DESTROYING_WITH_SHUTDOWN,

    /**< Purge key(s) then terminate and re-initialize */
    INVALIDATE_BY_PURGING_WITH_SHUTDOWN,
} invalidate_method_t;

typedef enum {
    KEEP_OPEN,
    CLOSE_BEFORE,
    CLOSE_AFTER,
} reopen_policy_t;

typedef enum {
    INVALID_HANDLE_0,
    INVALID_HANDLE_UNOPENED,
    INVALID_HANDLE_CLOSED,
    INVALID_HANDLE_HUGE,
} invalid_handle_construction_t;

/** Apply \p invalidate_method to invalidate the specified key:
 * close it, destroy it, or do nothing;
 */
static int invalidate_key(invalidate_method_t invalidate_method,
                          mbedtls_svc_key_id_t key)
{
    switch (invalidate_method) {
        /* Closing the key invalidate only volatile keys, not persistent ones. */
        case INVALIDATE_BY_CLOSING:
        case INVALIDATE_BY_CLOSING_WITH_SHUTDOWN:
            PSA_ASSERT(psa_close_key(key));
            break;
        case INVALIDATE_BY_DESTROYING:
        case INVALIDATE_BY_DESTROYING_WITH_SHUTDOWN:
            PSA_ASSERT(psa_destroy_key(key));
            break;
        /* Purging the key just purges RAM data of persistent keys. */
        case INVALIDATE_BY_PURGING:
        case INVALIDATE_BY_PURGING_WITH_SHUTDOWN:
            PSA_ASSERT(psa_purge_key(key));
            break;
        case INVALIDATE_BY_SHUTDOWN:
            break;
    }
    return 1;
exit:
    return 0;
}

/** Restart the PSA subsystem if \p invalidate_method says so. */
static int invalidate_psa(invalidate_method_t invalidate_method)
{
    switch (invalidate_method) {
        case INVALIDATE_BY_CLOSING:
        case INVALIDATE_BY_DESTROYING:
        case INVALIDATE_BY_PURGING:
            return 1;
        case INVALIDATE_BY_CLOSING_WITH_SHUTDOWN:
        case INVALIDATE_BY_DESTROYING_WITH_SHUTDOWN:
        case INVALIDATE_BY_PURGING_WITH_SHUTDOWN:
            /* All keys must have been closed. */
            PSA_SESSION_DONE();
            break;
        case INVALIDATE_BY_SHUTDOWN:
            /* Some keys may remain behind, and we're testing that this
             * properly closes them. */
            mbedtls_psa_crypto_free();
            break;
    }

    PSA_ASSERT(psa_crypto_init());
    ASSERT_PSA_PRISTINE();
    return 1;

exit:
    return 0;
}

#line 109 "suites/test_suite_psa_crypto_slot_management.function"
static void test_transient_slot_lifecycle(int owner_id_arg,
                              int usage_arg, int alg_arg,
                              int type_arg, data_t *key_data,
                              int invalidate_method_arg)
{
    psa_algorithm_t alg = alg_arg;
    psa_key_usage_t usage_flags = usage_arg;
    psa_key_type_t type = type_arg;
    invalidate_method_t invalidate_method = invalidate_method_arg;
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    mbedtls_test_set_step(1);
    PSA_ASSERT(psa_crypto_init());

    /* Import a key. */
#if defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)
    mbedtls_key_owner_id_t owner_id = owner_id_arg;

    mbedtls_set_key_owner_id(&attributes, owner_id);
#else
    (void) owner_id_arg;
#endif

    psa_set_key_usage_flags(&attributes, usage_flags);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, type);
    PSA_ASSERT(psa_import_key(&attributes, key_data->x, key_data->len,
                              &key));
    TEST_ASSERT(!mbedtls_svc_key_id_is_null(key));
    PSA_ASSERT(psa_get_key_attributes(key, &attributes));
    TEST_EQUAL(psa_get_key_type(&attributes), type);
    psa_reset_key_attributes(&attributes);

#if defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)
    {
        psa_key_handle_t handle;
        mbedtls_svc_key_id_t key_with_invalid_owner =
            mbedtls_svc_key_id_make(owner_id + 1,
                                    MBEDTLS_SVC_KEY_ID_GET_KEY_ID(key));

        TEST_ASSERT(mbedtls_key_owner_id_equal(
                        owner_id,
                        MBEDTLS_SVC_KEY_ID_GET_OWNER_ID(key)));
        TEST_EQUAL(psa_open_key(key_with_invalid_owner, &handle),
                   PSA_ERROR_DOES_NOT_EXIST);
    }
#endif

    /*
     * Purge the key and make sure that it is still valid, as purging a
     * volatile key shouldn't invalidate/destroy it.
     */
    PSA_ASSERT(psa_purge_key(key));
    PSA_ASSERT(psa_get_key_attributes(key, &attributes));
    TEST_EQUAL(psa_get_key_type(&attributes), type);
    psa_reset_key_attributes(&attributes);

    /* Do something that invalidates the key. */
    mbedtls_test_set_step(2);
    if (!invalidate_key(invalidate_method, key)) {
        goto exit;
    }
    if (!invalidate_psa(invalidate_method)) {
        goto exit;
    }

    /* Test that the key is now invalid. */
    TEST_EQUAL(psa_get_key_attributes(key, &attributes),
               PSA_ERROR_INVALID_HANDLE);
    TEST_EQUAL(psa_close_key(key), PSA_ERROR_INVALID_HANDLE);

exit:
    /*
     * Key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes(&attributes);

    PSA_DONE();
}

static void test_transient_slot_lifecycle_wrapper( void ** params )
{
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};

    test_transient_slot_lifecycle( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), &data4, *( (int *) params[6] ) );
}
#if defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)
#line 193 "suites/test_suite_psa_crypto_slot_management.function"
static void test_persistent_slot_lifecycle(int lifetime_arg, int owner_id_arg, int id_arg,
                               int usage_arg, int alg_arg, int alg2_arg,
                               int type_arg, data_t *key_data,
                               int invalidate_method_arg)
{
    psa_key_lifetime_t lifetime = lifetime_arg;
    mbedtls_svc_key_id_t id = mbedtls_svc_key_id_make(owner_id_arg, id_arg);
    psa_algorithm_t alg = alg_arg;
    psa_algorithm_t alg2 = alg2_arg;
    psa_key_usage_t usage_flags = usage_arg;
    psa_key_type_t type = type_arg;
    invalidate_method_t invalidate_method = invalidate_method_arg;
    mbedtls_svc_key_id_t returned_id = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_handle_t handle = PSA_KEY_HANDLE_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t read_attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t *reexported = NULL;
    size_t reexported_length = -1;

#if defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)
    mbedtls_svc_key_id_t wrong_owner_id =
        mbedtls_svc_key_id_make(owner_id_arg + 1, id_arg);
    mbedtls_svc_key_id_t invalid_svc_key_id = MBEDTLS_SVC_KEY_ID_INIT;
#endif

    TEST_USES_KEY_ID(id);

    mbedtls_test_set_step(1);
    PSA_ASSERT(psa_crypto_init());

    psa_set_key_id(&attributes, id);
    psa_set_key_lifetime(&attributes, lifetime);
    psa_set_key_type(&attributes, type);
    psa_set_key_usage_flags(&attributes, usage_flags);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_enrollment_algorithm(&attributes, alg2);
    PSA_ASSERT(psa_import_key(&attributes, key_data->x, key_data->len,
                              &returned_id));
    TEST_ASSERT(mbedtls_svc_key_id_equal(id, returned_id));

#if defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)
    TEST_EQUAL(psa_open_key(wrong_owner_id, &invalid_svc_key_id),
               PSA_ERROR_DOES_NOT_EXIST);
#endif

    PSA_ASSERT(psa_get_key_attributes(id, &attributes));
    TEST_EQUAL(psa_get_key_lifetime(&attributes), lifetime);
    TEST_ASSERT(mbedtls_svc_key_id_equal(
                    psa_get_key_id(&attributes), id));
    TEST_EQUAL(psa_get_key_usage_flags(&attributes),
               mbedtls_test_update_key_usage_flags(usage_flags));
    TEST_EQUAL(psa_get_key_algorithm(&attributes), alg);
    TEST_EQUAL(psa_get_key_enrollment_algorithm(&attributes), alg2);
    TEST_EQUAL(psa_get_key_type(&attributes), type);

    /* Close the key and then open it. */
    PSA_ASSERT(psa_close_key(id));

#if defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)
    TEST_EQUAL(psa_open_key(wrong_owner_id, &invalid_svc_key_id),
               PSA_ERROR_DOES_NOT_EXIST);
#endif

    PSA_ASSERT(psa_open_key(id, &handle));
    TEST_ASSERT(!psa_key_handle_is_null(handle));
    PSA_ASSERT(psa_get_key_attributes(handle, &attributes));
    TEST_EQUAL(psa_get_key_lifetime(&attributes), lifetime);
    TEST_ASSERT(mbedtls_svc_key_id_equal(
                    psa_get_key_id(&attributes), id));
    TEST_EQUAL(psa_get_key_usage_flags(&attributes),
               mbedtls_test_update_key_usage_flags(usage_flags));
    TEST_EQUAL(psa_get_key_algorithm(&attributes), alg);
    TEST_EQUAL(psa_get_key_enrollment_algorithm(&attributes), alg2);
    TEST_EQUAL(psa_get_key_type(&attributes), type);

    /*
     * Do something that wipes key data in volatile memory or destroy the
     * key.
     */
    mbedtls_test_set_step(2);
    if (!invalidate_key(invalidate_method, id)) {
        goto exit;
    }
    if (!invalidate_psa(invalidate_method)) {
        goto exit;
    }

    /* Try to reaccess the key. If we destroyed it, check that it doesn't
     * exist. Otherwise check that it still exists and has the expected
     * content. */
    switch (invalidate_method) {
        case INVALIDATE_BY_CLOSING:
        case INVALIDATE_BY_CLOSING_WITH_SHUTDOWN:
        case INVALIDATE_BY_PURGING:
        case INVALIDATE_BY_PURGING_WITH_SHUTDOWN:
        case INVALIDATE_BY_SHUTDOWN:
            PSA_ASSERT(psa_open_key(id, &handle));
            PSA_ASSERT(psa_get_key_attributes(id, &read_attributes));
            TEST_EQUAL(psa_get_key_lifetime(&attributes),
                       psa_get_key_lifetime(&read_attributes));
            TEST_ASSERT(mbedtls_svc_key_id_equal(
                            psa_get_key_id(&attributes),
                            psa_get_key_id(&read_attributes)));
            TEST_EQUAL(psa_get_key_usage_flags(&attributes),
                       mbedtls_test_update_key_usage_flags(usage_flags));
            TEST_EQUAL(psa_get_key_algorithm(&attributes),
                       psa_get_key_algorithm(&read_attributes));
            TEST_EQUAL(psa_get_key_enrollment_algorithm(&attributes),
                       psa_get_key_enrollment_algorithm(&read_attributes));
            TEST_EQUAL(psa_get_key_type(&attributes),
                       psa_get_key_type(&read_attributes));
            TEST_EQUAL(psa_get_key_bits(&attributes),
                       psa_get_key_bits(&read_attributes));
            ASSERT_ALLOC(reexported, key_data->len);
            if (usage_flags & PSA_KEY_USAGE_EXPORT) {
                PSA_ASSERT(psa_export_key(id, reexported, key_data->len,
                                          &reexported_length));
                ASSERT_COMPARE(key_data->x, key_data->len,
                               reexported, reexported_length);
            } else {
                TEST_EQUAL(psa_export_key(id, reexported,
                                          key_data->len, &reexported_length),
                           PSA_ERROR_NOT_PERMITTED);
            }
            PSA_ASSERT(psa_close_key(handle));
            break;

        case INVALIDATE_BY_DESTROYING:
        case INVALIDATE_BY_DESTROYING_WITH_SHUTDOWN:
            /*
             * Test that the key handle and identifier are now not referring to an
             * existing key.
             */
            TEST_EQUAL(psa_get_key_attributes(handle, &read_attributes),
                       PSA_ERROR_INVALID_HANDLE);
            TEST_EQUAL(psa_close_key(handle), PSA_ERROR_INVALID_HANDLE);
            TEST_EQUAL(psa_get_key_attributes(id, &read_attributes),
                       PSA_ERROR_INVALID_HANDLE);
            break;
    }

exit:
    /*
     * Key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes(&attributes);
    psa_reset_key_attributes(&read_attributes);

    PSA_DONE();
    mbedtls_free(reexported);
}

static void test_persistent_slot_lifecycle_wrapper( void ** params )
{
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};

    test_persistent_slot_lifecycle( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), *( (int *) params[6] ), &data7, *( (int *) params[9] ) );
}
#endif /* MBEDTLS_PSA_CRYPTO_STORAGE_C */
#if defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)
#line 348 "suites/test_suite_psa_crypto_slot_management.function"
static void test_create_existent(int lifetime_arg, int owner_id_arg, int id_arg,
                     int reopen_policy_arg)
{
    psa_key_lifetime_t lifetime = lifetime_arg;
    mbedtls_svc_key_id_t id = mbedtls_svc_key_id_make(owner_id_arg, id_arg);
    mbedtls_svc_key_id_t returned_id = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_type_t type1 = PSA_KEY_TYPE_RAW_DATA;
    const uint8_t material1[5] = "a key";
    const uint8_t material2[5] = "b key";
    size_t bits1 = PSA_BYTES_TO_BITS(sizeof(material1));
    uint8_t reexported[sizeof(material1)];
    size_t reexported_length;
    reopen_policy_t reopen_policy = reopen_policy_arg;

    TEST_USES_KEY_ID(id);

    PSA_ASSERT(psa_crypto_init());

    /* Create a key. */
    psa_set_key_id(&attributes, id);
    psa_set_key_lifetime(&attributes, lifetime);
    psa_set_key_type(&attributes, type1);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_EXPORT);
    psa_set_key_algorithm(&attributes, 0);
    PSA_ASSERT(psa_import_key(&attributes, material1, sizeof(material1),
                              &returned_id));
    TEST_ASSERT(mbedtls_svc_key_id_equal(id, returned_id));

    if (reopen_policy == CLOSE_BEFORE) {
        PSA_ASSERT(psa_close_key(id));
    }

    /* Attempt to create a new key in the same slot. */
    TEST_EQUAL(psa_import_key(&attributes, material2, sizeof(material2),
                              &returned_id),
               PSA_ERROR_ALREADY_EXISTS);
    TEST_ASSERT(mbedtls_svc_key_id_is_null(returned_id));

    if (reopen_policy == CLOSE_AFTER) {
        PSA_ASSERT(psa_close_key(id));
    }

    /* Check that the original key hasn't changed. */
    psa_reset_key_attributes(&attributes);
    PSA_ASSERT(psa_get_key_attributes(id, &attributes));
    TEST_ASSERT(mbedtls_svc_key_id_equal(
                    psa_get_key_id(&attributes), id));
    TEST_EQUAL(psa_get_key_lifetime(&attributes), lifetime);
    TEST_EQUAL(psa_get_key_type(&attributes), type1);
    TEST_EQUAL(psa_get_key_bits(&attributes), bits1);
    TEST_EQUAL(psa_get_key_usage_flags(&attributes), PSA_KEY_USAGE_EXPORT);
    TEST_EQUAL(psa_get_key_algorithm(&attributes), 0);

    PSA_ASSERT(psa_export_key(id,
                              reexported, sizeof(reexported),
                              &reexported_length));
    ASSERT_COMPARE(material1, sizeof(material1),
                   reexported, reexported_length);

    PSA_ASSERT(psa_close_key(id));

exit:
    /*
     * Key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes(&attributes);

    PSA_DONE();
}

static void test_create_existent_wrapper( void ** params )
{

    test_create_existent( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ) );
}
#endif /* MBEDTLS_PSA_CRYPTO_STORAGE_C */
#line 422 "suites/test_suite_psa_crypto_slot_management.function"
static void test_open_fail(int id_arg,
               int expected_status_arg)
{
    mbedtls_svc_key_id_t id = mbedtls_svc_key_id_make(1, id_arg);
    psa_status_t expected_status = expected_status_arg;
    psa_key_handle_t handle = mbedtls_svc_key_id_make(0xdead, 0xdead);

    PSA_ASSERT(psa_crypto_init());

    TEST_EQUAL(psa_open_key(id, &handle), expected_status);
    TEST_ASSERT(psa_key_handle_is_null(handle));

exit:
    PSA_DONE();
}

static void test_open_fail_wrapper( void ** params )
{

    test_open_fail( *( (int *) params[0] ), *( (int *) params[1] ) );
}
#line 440 "suites/test_suite_psa_crypto_slot_management.function"
static void test_create_fail(int lifetime_arg, int id_arg,
                 int expected_status_arg)
{
    psa_key_lifetime_t lifetime = lifetime_arg;
    mbedtls_svc_key_id_t id = mbedtls_svc_key_id_make(1, id_arg);
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t expected_status = expected_status_arg;
    mbedtls_svc_key_id_t returned_id =
        mbedtls_svc_key_id_make(0xdead, 0xdead);
    uint8_t material[1] = { 'k' };

    TEST_USES_KEY_ID(id);

    PSA_ASSERT(psa_crypto_init());

    psa_set_key_lifetime(&attributes, lifetime);
    if (PSA_KEY_LIFETIME_IS_VOLATILE(lifetime)) {
        /*
         * Not possible to set a key identifier different from 0 through
         * PSA key attributes APIs thus accessing to the attributes
         * directly.
         */
        attributes.core.id = id;
    } else {
        psa_set_key_id(&attributes, id);
    }

    psa_set_key_type(&attributes, PSA_KEY_TYPE_RAW_DATA);
    TEST_EQUAL(psa_import_key(&attributes, material, sizeof(material),
                              &returned_id),
               expected_status);
    TEST_ASSERT(mbedtls_svc_key_id_is_null(returned_id));

exit:
    PSA_DONE();
}

static void test_create_fail_wrapper( void ** params )
{

    test_create_fail( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
#line 479 "suites/test_suite_psa_crypto_slot_management.function"
static void test_copy_across_lifetimes(int source_lifetime_arg, int source_owner_id_arg,
                           int source_id_arg, int source_usage_arg,
                           int source_alg_arg, int source_alg2_arg,
                           int type_arg, data_t *material,
                           int target_lifetime_arg, int target_owner_id_arg,
                           int target_id_arg, int target_usage_arg,
                           int target_alg_arg, int target_alg2_arg,
                           int expected_usage_arg,
                           int expected_alg_arg, int expected_alg2_arg)
{
    psa_key_lifetime_t source_lifetime = source_lifetime_arg;
    mbedtls_svc_key_id_t source_id =
        mbedtls_svc_key_id_make(source_owner_id_arg, source_id_arg);
    psa_key_usage_t source_usage = source_usage_arg;
    psa_algorithm_t source_alg = source_alg_arg;
    psa_key_attributes_t source_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_type_t source_type = type_arg;
    mbedtls_svc_key_id_t returned_source_id = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_lifetime_t target_lifetime = target_lifetime_arg;
    mbedtls_svc_key_id_t target_id =
        mbedtls_svc_key_id_make(target_owner_id_arg, target_id_arg);
    psa_key_usage_t target_usage = target_usage_arg;
    psa_algorithm_t target_alg = target_alg_arg;
    psa_key_attributes_t target_attributes = PSA_KEY_ATTRIBUTES_INIT;
    mbedtls_svc_key_id_t returned_target_id = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_handle_t target_handle = PSA_KEY_HANDLE_INIT;
    psa_key_usage_t expected_usage = expected_usage_arg;
    psa_algorithm_t expected_alg = expected_alg_arg;
    psa_algorithm_t expected_alg2 = expected_alg2_arg;
    uint8_t *export_buffer = NULL;

    TEST_USES_KEY_ID(source_id);
    TEST_USES_KEY_ID(target_id);

    PSA_ASSERT(psa_crypto_init());

    /* Populate the source slot. */
    psa_set_key_id(&source_attributes, source_id);
    psa_set_key_lifetime(&source_attributes, source_lifetime);

    psa_set_key_type(&source_attributes, source_type);
    psa_set_key_usage_flags(&source_attributes, source_usage);
    psa_set_key_algorithm(&source_attributes, source_alg);
    psa_set_key_enrollment_algorithm(&source_attributes, source_alg2_arg);
    PSA_ASSERT(psa_import_key(&source_attributes,
                              material->x, material->len,
                              &returned_source_id));
    /* Update the attributes with the bit size. */
    PSA_ASSERT(psa_get_key_attributes(returned_source_id,
                                      &source_attributes));

    /* Prepare the target slot. */
    psa_set_key_id(&target_attributes, target_id);
    psa_set_key_lifetime(&target_attributes, target_lifetime);

    psa_set_key_usage_flags(&target_attributes, target_usage);
    psa_set_key_algorithm(&target_attributes, target_alg);
    psa_set_key_enrollment_algorithm(&target_attributes, target_alg2_arg);

    /* Copy the key. */
    PSA_ASSERT(psa_copy_key(returned_source_id,
                            &target_attributes, &returned_target_id));

    /* Destroy the source to ensure that this doesn't affect the target. */
    PSA_ASSERT(psa_destroy_key(returned_source_id));

    /* If the target key is persistent, restart the system to make
     * sure that the material is still alive. */
    if (!PSA_KEY_LIFETIME_IS_VOLATILE(target_lifetime)) {
        mbedtls_psa_crypto_free();
        PSA_ASSERT(psa_crypto_init());
        PSA_ASSERT(psa_open_key(target_id, &target_handle));
    }

    /* Test that the target slot has the expected content. */
    psa_reset_key_attributes(&target_attributes);
    PSA_ASSERT(psa_get_key_attributes(returned_target_id,
                                      &target_attributes));

    if (!PSA_KEY_LIFETIME_IS_VOLATILE(target_lifetime)) {
        TEST_ASSERT(mbedtls_svc_key_id_equal(
                        target_id, psa_get_key_id(&target_attributes)));
    } else {
#if defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)
        TEST_EQUAL(MBEDTLS_SVC_KEY_ID_GET_OWNER_ID(returned_target_id),
                   target_owner_id_arg);
#endif
    }

    TEST_EQUAL(target_lifetime, psa_get_key_lifetime(&target_attributes));
    TEST_EQUAL(source_type, psa_get_key_type(&target_attributes));
    TEST_EQUAL(psa_get_key_bits(&source_attributes),
               psa_get_key_bits(&target_attributes));
    TEST_EQUAL(expected_usage, psa_get_key_usage_flags(&target_attributes));
    TEST_EQUAL(expected_alg, psa_get_key_algorithm(&target_attributes));
    TEST_EQUAL(expected_alg2,
               psa_get_key_enrollment_algorithm(&target_attributes));
    if (expected_usage & PSA_KEY_USAGE_EXPORT) {
        size_t length;
        ASSERT_ALLOC(export_buffer, material->len);
        PSA_ASSERT(psa_export_key(returned_target_id, export_buffer,
                                  material->len, &length));
        ASSERT_COMPARE(material->x, material->len,
                       export_buffer, length);
    } else {
        size_t length;
        /* Check that the key is actually non-exportable. */
        TEST_EQUAL(psa_export_key(returned_target_id, export_buffer,
                                  material->len, &length),
                   PSA_ERROR_NOT_PERMITTED);
    }

    PSA_ASSERT(psa_destroy_key(returned_target_id));

exit:
    /*
     * Source and target key attributes may have been returned by
     * psa_get_key_attributes() thus reset them as required.
     */
    psa_reset_key_attributes(&source_attributes);
    psa_reset_key_attributes(&target_attributes);

    PSA_DONE();
    mbedtls_free(export_buffer);
}

static void test_copy_across_lifetimes_wrapper( void ** params )
{
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};

    test_copy_across_lifetimes( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), *( (int *) params[6] ), &data7, *( (int *) params[9] ), *( (int *) params[10] ), *( (int *) params[11] ), *( (int *) params[12] ), *( (int *) params[13] ), *( (int *) params[14] ), *( (int *) params[15] ), *( (int *) params[16] ), *( (int *) params[17] ) );
}
#line 607 "suites/test_suite_psa_crypto_slot_management.function"
static void test_copy_to_occupied(int source_lifetime_arg, int source_id_arg,
                      int source_usage_arg, int source_alg_arg,
                      int source_type_arg, data_t *source_material,
                      int target_lifetime_arg, int target_id_arg,
                      int target_usage_arg, int target_alg_arg,
                      int target_type_arg, data_t *target_material)
{
    psa_key_lifetime_t source_lifetime = source_lifetime_arg;
    mbedtls_svc_key_id_t source_id =
        mbedtls_svc_key_id_make(1, source_id_arg);
    psa_key_usage_t source_usage = source_usage_arg;
    psa_algorithm_t source_alg = source_alg_arg;
    psa_key_type_t source_type = source_type_arg;
    mbedtls_svc_key_id_t returned_source_id = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_lifetime_t target_lifetime = target_lifetime_arg;
    mbedtls_svc_key_id_t target_id =
        mbedtls_svc_key_id_make(1, target_id_arg);
    psa_key_usage_t target_usage = target_usage_arg;
    psa_algorithm_t target_alg = target_alg_arg;
    psa_key_type_t target_type = target_type_arg;
    mbedtls_svc_key_id_t returned_target_id = MBEDTLS_SVC_KEY_ID_INIT;
    mbedtls_svc_key_id_t new_key = MBEDTLS_SVC_KEY_ID_INIT;
    uint8_t *export_buffer = NULL;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t attributes1 = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t attributes2 = PSA_KEY_ATTRIBUTES_INIT;

    TEST_USES_KEY_ID(source_id);
    TEST_USES_KEY_ID(target_id);

    PSA_ASSERT(psa_crypto_init());

    /* Populate the source slot. */
    if (!PSA_KEY_LIFETIME_IS_VOLATILE(source_lifetime)) {
        psa_set_key_id(&attributes, source_id);
        psa_set_key_lifetime(&attributes, source_lifetime);
    }
    psa_set_key_type(&attributes, source_type);
    psa_set_key_usage_flags(&attributes, source_usage);
    psa_set_key_algorithm(&attributes, source_alg);
    PSA_ASSERT(psa_import_key(&attributes,
                              source_material->x, source_material->len,
                              &returned_source_id));

    /* Populate the target slot. */
    if (mbedtls_svc_key_id_equal(target_id, source_id)) {
        returned_target_id = returned_source_id;
    } else {
        psa_set_key_id(&attributes1, target_id);
        psa_set_key_lifetime(&attributes1, target_lifetime);
        psa_set_key_type(&attributes1, target_type);
        psa_set_key_usage_flags(&attributes1, target_usage);
        psa_set_key_algorithm(&attributes1, target_alg);
        PSA_ASSERT(psa_import_key(&attributes1,
                                  target_material->x, target_material->len,
                                  &returned_target_id));
    }

    PSA_ASSERT(psa_get_key_attributes(returned_target_id, &attributes1));

    /* Make a copy attempt. */
    psa_set_key_id(&attributes, target_id);
    psa_set_key_lifetime(&attributes, target_lifetime);
    TEST_EQUAL(psa_copy_key(returned_source_id,
                            &attributes, &new_key),
               PSA_ERROR_ALREADY_EXISTS);
    TEST_ASSERT(mbedtls_svc_key_id_is_null(new_key));

    /* Test that the target slot is unaffected. */
    PSA_ASSERT(psa_get_key_attributes(returned_target_id, &attributes2));
    TEST_ASSERT(mbedtls_svc_key_id_equal(
                    psa_get_key_id(&attributes1),
                    psa_get_key_id(&attributes2)));
    TEST_EQUAL(psa_get_key_lifetime(&attributes1),
               psa_get_key_lifetime(&attributes2));
    TEST_EQUAL(psa_get_key_type(&attributes1),
               psa_get_key_type(&attributes2));
    TEST_EQUAL(psa_get_key_bits(&attributes1),
               psa_get_key_bits(&attributes2));
    TEST_EQUAL(psa_get_key_usage_flags(&attributes1),
               psa_get_key_usage_flags(&attributes2));
    TEST_EQUAL(psa_get_key_algorithm(&attributes1),
               psa_get_key_algorithm(&attributes2));
    if (target_usage & PSA_KEY_USAGE_EXPORT) {
        size_t length;
        ASSERT_ALLOC(export_buffer, target_material->len);
        PSA_ASSERT(psa_export_key(returned_target_id, export_buffer,
                                  target_material->len, &length));
        ASSERT_COMPARE(target_material->x, target_material->len,
                       export_buffer, length);
    }

    PSA_ASSERT(psa_destroy_key(returned_source_id));
    if (!mbedtls_svc_key_id_equal(target_id, source_id)) {
        PSA_ASSERT(psa_destroy_key(returned_target_id));
    }

exit:
    /*
     * Key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes(&attributes1);
    psa_reset_key_attributes(&attributes2);

    PSA_DONE();
    mbedtls_free(export_buffer);
}

static void test_copy_to_occupied_wrapper( void ** params )
{
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data12 = {(uint8_t *) params[12], *( (uint32_t *) params[13] )};

    test_copy_to_occupied( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), &data5, *( (int *) params[7] ), *( (int *) params[8] ), *( (int *) params[9] ), *( (int *) params[10] ), *( (int *) params[11] ), &data12 );
}
#line 718 "suites/test_suite_psa_crypto_slot_management.function"
static void test_invalid_handle(int handle_construction,
                    int close_status_arg)
{
    psa_key_handle_t valid_handle = PSA_KEY_HANDLE_INIT;
    psa_key_handle_t invalid_handle = PSA_KEY_HANDLE_INIT;
    psa_key_id_t key_id;
    psa_status_t close_status = close_status_arg;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t material[1] = "a";

    PSA_ASSERT(psa_crypto_init());

    /* Allocate a handle and store a key in it. */
    psa_set_key_type(&attributes, PSA_KEY_TYPE_RAW_DATA);
    psa_set_key_usage_flags(&attributes, 0);
    psa_set_key_algorithm(&attributes, 0);
    PSA_ASSERT(psa_import_key(&attributes,
                              material, sizeof(material),
                              &valid_handle));
    TEST_ASSERT(!psa_key_handle_is_null(valid_handle));

    /* Construct an invalid handle as specified in the test case data. */
    switch (handle_construction) {
        case INVALID_HANDLE_0:
            invalid_handle = PSA_KEY_HANDLE_INIT;
            break;
        case INVALID_HANDLE_UNOPENED:

            /*
             * MBEDTLS_SVC_KEY_ID_GET_KEY_ID( valid_handle ) is a volatile
             * key identifier as the imported key is a volatile key. Volatile
             * key identifiers are in the range from PSA_KEY_ID_VOLATILE_MIN
             * to PSA_KEY_ID_VOLATILE_MAX included. Thus pick a key identifier
             * in the range from PSA_KEY_ID_VOLATILE_MIN to
             * PSA_KEY_ID_VOLATILE_MAX different from
             * MBEDTLS_SVC_KEY_ID_GET_KEY_ID( valid_handle ) to build an
             * unopened and thus invalid identifier.
             */

            if (MBEDTLS_SVC_KEY_ID_GET_KEY_ID(valid_handle) ==
                PSA_KEY_ID_VOLATILE_MIN) {
                key_id = PSA_KEY_ID_VOLATILE_MIN + 1;
            } else {
                key_id = MBEDTLS_SVC_KEY_ID_GET_KEY_ID(valid_handle) - 1;
            }

            invalid_handle =
                mbedtls_svc_key_id_make(0, key_id);
            break;
        case INVALID_HANDLE_CLOSED:
            PSA_ASSERT(psa_import_key(&attributes,
                                      material, sizeof(material),
                                      &invalid_handle));
            PSA_ASSERT(psa_destroy_key(invalid_handle));
            break;
        case INVALID_HANDLE_HUGE:
            invalid_handle =
                mbedtls_svc_key_id_make(0, PSA_KEY_ID_VENDOR_MAX + 1);
            break;
        default:
            TEST_ASSERT(!"unknown handle construction");
    }

    /* Attempt to use the invalid handle. */
    TEST_EQUAL(psa_get_key_attributes(invalid_handle, &attributes),
               PSA_ERROR_INVALID_HANDLE);
    TEST_EQUAL(psa_close_key(invalid_handle), close_status);
    TEST_EQUAL(psa_destroy_key(invalid_handle), close_status);

    /* After all this, check that the original handle is intact. */
    PSA_ASSERT(psa_get_key_attributes(valid_handle, &attributes));
    TEST_EQUAL(psa_get_key_type(&attributes), PSA_KEY_TYPE_RAW_DATA);
    TEST_EQUAL(psa_get_key_bits(&attributes),
               PSA_BYTES_TO_BITS(sizeof(material)));
    PSA_ASSERT(psa_close_key(valid_handle));

exit:
    /*
     * Key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes(&attributes);

    PSA_DONE();
}

static void test_invalid_handle_wrapper( void ** params )
{

    test_invalid_handle( *( (int *) params[0] ), *( (int *) params[1] ) );
}
#line 806 "suites/test_suite_psa_crypto_slot_management.function"
static void test_many_transient_keys(int max_keys_arg)
{
    mbedtls_svc_key_id_t *keys = NULL;
    size_t max_keys = max_keys_arg;
    size_t i, j;
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t exported[sizeof(size_t)];
    size_t exported_length;

    ASSERT_ALLOC(keys, max_keys);
    PSA_ASSERT(psa_crypto_init());

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_EXPORT);
    psa_set_key_algorithm(&attributes, 0);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_RAW_DATA);

    for (i = 0; i < max_keys; i++) {
        status = psa_import_key(&attributes,
                                (uint8_t *) &i, sizeof(i),
                                &keys[i]);
        if (status == PSA_ERROR_INSUFFICIENT_MEMORY) {
            break;
        }
        PSA_ASSERT(status);
        TEST_ASSERT(!mbedtls_svc_key_id_is_null(keys[i]));
        for (j = 0; j < i; j++) {
            TEST_ASSERT(!mbedtls_svc_key_id_equal(keys[i], keys[j]));
        }
    }
    max_keys = i;

    for (i = 1; i < max_keys; i++) {
        PSA_ASSERT(psa_close_key(keys[i - 1]));
        PSA_ASSERT(psa_export_key(keys[i],
                                  exported, sizeof(exported),
                                  &exported_length));
        ASSERT_COMPARE(exported, exported_length,
                       (uint8_t *) &i, sizeof(i));
    }
    PSA_ASSERT(psa_close_key(keys[i - 1]));

exit:
    PSA_DONE();
    mbedtls_free(keys);
}

static void test_many_transient_keys_wrapper( void ** params )
{

    test_many_transient_keys( *( (int *) params[0] ) );
}
#if defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)
#line 855 "suites/test_suite_psa_crypto_slot_management.function"
static void test_key_slot_eviction_to_import_new_key(int lifetime_arg)
{
    psa_key_lifetime_t lifetime = (psa_key_lifetime_t) lifetime_arg;
    size_t i;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t exported[sizeof(size_t)];
    size_t exported_length;
    mbedtls_svc_key_id_t key, returned_key_id;

    PSA_ASSERT(psa_crypto_init());

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_EXPORT);
    psa_set_key_algorithm(&attributes, 0);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_RAW_DATA);

    /*
     * Create MBEDTLS_PSA_KEY_SLOT_COUNT persistent keys.
     */
    for (i = 0; i < MBEDTLS_PSA_KEY_SLOT_COUNT; i++) {
        key = mbedtls_svc_key_id_make(i, i + 1);
        psa_set_key_id(&attributes, key);
        PSA_ASSERT(psa_import_key(&attributes,
                                  (uint8_t *) &i, sizeof(i),
                                  &returned_key_id));
        TEST_ASSERT(mbedtls_svc_key_id_equal(returned_key_id, key));
    }

    /*
     * Create a new persistent or volatile key. When creating the key,
     * one of the descriptions of the previously created persistent keys
     * is removed from the RAM key slots. This makes room to store its
     * description in RAM.
     */
    i = MBEDTLS_PSA_KEY_SLOT_COUNT;
    key = mbedtls_svc_key_id_make(i, i + 1);
    psa_set_key_id(&attributes, key);
    psa_set_key_lifetime(&attributes, lifetime);

    PSA_ASSERT(psa_import_key(&attributes,
                              (uint8_t *) &i, sizeof(i),
                              &returned_key_id));
    if (lifetime != PSA_KEY_LIFETIME_VOLATILE) {
        TEST_ASSERT(mbedtls_svc_key_id_equal(returned_key_id, key));
    } else {
        TEST_ASSERT(psa_key_id_is_volatile(
                        MBEDTLS_SVC_KEY_ID_GET_KEY_ID(returned_key_id)));
    }

    /*
     * Check that we can export all ( MBEDTLS_PSA_KEY_SLOT_COUNT + 1 ) keys,
     * that they have the expected value and destroy them. In that process,
     * the description of the persistent key that was evicted from the RAM
     * slots when creating the last key is restored in a RAM slot to export
     * its value.
     */
    for (i = 0; i <= MBEDTLS_PSA_KEY_SLOT_COUNT; i++) {
        if (i < MBEDTLS_PSA_KEY_SLOT_COUNT) {
            key = mbedtls_svc_key_id_make(i, i + 1);
        } else {
            key = returned_key_id;
        }

        PSA_ASSERT(psa_export_key(key,
                                  exported, sizeof(exported),
                                  &exported_length));
        ASSERT_COMPARE(exported, exported_length,
                       (uint8_t *) &i, sizeof(i));
        PSA_ASSERT(psa_destroy_key(key));
    }

exit:
    PSA_DONE();
}

static void test_key_slot_eviction_to_import_new_key_wrapper( void ** params )
{

    test_key_slot_eviction_to_import_new_key( *( (int *) params[0] ) );
}
#endif /* MBEDTLS_PSA_CRYPTO_STORAGE_C */
#if defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)
#line 931 "suites/test_suite_psa_crypto_slot_management.function"
static void test_non_reusable_key_slots_integrity_in_case_of_key_slot_starvation(void)
{
    psa_status_t status;
    size_t i;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t exported[sizeof(size_t)];
    size_t exported_length;
    mbedtls_svc_key_id_t persistent_key = MBEDTLS_SVC_KEY_ID_INIT;
    mbedtls_svc_key_id_t persistent_key2 = MBEDTLS_SVC_KEY_ID_INIT;
    mbedtls_svc_key_id_t returned_key_id = MBEDTLS_SVC_KEY_ID_INIT;
    mbedtls_svc_key_id_t *keys = NULL;

    TEST_ASSERT(MBEDTLS_PSA_KEY_SLOT_COUNT >= 1);

    ASSERT_ALLOC(keys, MBEDTLS_PSA_KEY_SLOT_COUNT);
    PSA_ASSERT(psa_crypto_init());

    psa_set_key_usage_flags(&attributes,
                            PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_COPY);
    psa_set_key_algorithm(&attributes, 0);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_RAW_DATA);

    /*
     * Create a persistent key
     */
    persistent_key = mbedtls_svc_key_id_make(0x100, 0x205);
    psa_set_key_id(&attributes, persistent_key);
    PSA_ASSERT(psa_import_key(&attributes,
                              (uint8_t *) &persistent_key,
                              sizeof(persistent_key),
                              &returned_key_id));
    TEST_ASSERT(mbedtls_svc_key_id_equal(returned_key_id, persistent_key));

    /*
     * Create MBEDTLS_PSA_KEY_SLOT_COUNT volatile keys
     */
    psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_VOLATILE);
    for (i = 0; i < MBEDTLS_PSA_KEY_SLOT_COUNT; i++) {
        PSA_ASSERT(psa_import_key(&attributes,
                                  (uint8_t *) &i, sizeof(i),
                                  &keys[i]));
    }
    psa_reset_key_attributes(&attributes);

    /*
     * Check that we cannot access the persistent key as all slots are
     * occupied by volatile keys and the implementation needs to load the
     * persistent key description in a slot to be able to access it.
     */
    status = psa_get_key_attributes(persistent_key, &attributes);
    TEST_EQUAL(status, PSA_ERROR_INSUFFICIENT_MEMORY);

    /*
     * Check we can export the volatile key created last and that it has the
     * expected value. Then, destroy it.
     */
    PSA_ASSERT(psa_export_key(keys[MBEDTLS_PSA_KEY_SLOT_COUNT - 1],
                              exported, sizeof(exported),
                              &exported_length));
    i = MBEDTLS_PSA_KEY_SLOT_COUNT - 1;
    ASSERT_COMPARE(exported, exported_length, (uint8_t *) &i, sizeof(i));
    PSA_ASSERT(psa_destroy_key(keys[MBEDTLS_PSA_KEY_SLOT_COUNT - 1]));

    /*
     * Check that we can now access the persistent key again.
     */
    PSA_ASSERT(psa_get_key_attributes(persistent_key, &attributes));
    TEST_ASSERT(mbedtls_svc_key_id_equal(attributes.core.id,
                                         persistent_key));

    /*
     * Check that we cannot copy the persistent key as all slots are occupied
     * by the persistent key and the volatile keys and the slot containing the
     * persistent key cannot be reclaimed as it contains the key to copy.
     */
    persistent_key2 = mbedtls_svc_key_id_make(0x100, 0x204);
    psa_set_key_id(&attributes, persistent_key2);
    status = psa_copy_key(persistent_key, &attributes, &returned_key_id);
    TEST_EQUAL(status, PSA_ERROR_INSUFFICIENT_MEMORY);

    /*
     * Check we can export the remaining volatile keys and that they have the
     * expected values.
     */
    for (i = 0; i < (MBEDTLS_PSA_KEY_SLOT_COUNT - 1); i++) {
        PSA_ASSERT(psa_export_key(keys[i],
                                  exported, sizeof(exported),
                                  &exported_length));
        ASSERT_COMPARE(exported, exported_length,
                       (uint8_t *) &i, sizeof(i));
        PSA_ASSERT(psa_destroy_key(keys[i]));
    }

    /*
     * Check we can export the persistent key and that it have the expected
     * value.
     */

    PSA_ASSERT(psa_export_key(persistent_key, exported, sizeof(exported),
                              &exported_length));
    ASSERT_COMPARE(exported, exported_length,
                   (uint8_t *) &persistent_key, sizeof(persistent_key));
exit:
    /*
     * Key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes(&attributes);

    psa_destroy_key(persistent_key);
    PSA_DONE();
    mbedtls_free(keys);
}

static void test_non_reusable_key_slots_integrity_in_case_of_key_slot_starvation_wrapper( void ** params )
{
    (void)params;

    test_non_reusable_key_slots_integrity_in_case_of_key_slot_starvation(  );
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
    
#if defined(MBEDTLS_PSA_CRYPTO_C)

        case 0:
            {
                *out_value = PSA_KEY_TYPE_RAW_DATA;
            }
            break;
        case 1:
            {
                *out_value = INVALIDATE_BY_CLOSING;
            }
            break;
        case 2:
            {
                *out_value = INVALIDATE_BY_CLOSING_WITH_SHUTDOWN;
            }
            break;
        case 3:
            {
                *out_value = INVALIDATE_BY_DESTROYING;
            }
            break;
        case 4:
            {
                *out_value = INVALIDATE_BY_DESTROYING_WITH_SHUTDOWN;
            }
            break;
        case 5:
            {
                *out_value = INVALIDATE_BY_SHUTDOWN;
            }
            break;
        case 6:
            {
                *out_value = PSA_KEY_LIFETIME_PERSISTENT;
            }
            break;
        case 7:
            {
                *out_value = PSA_KEY_ID_USER_MIN;
            }
            break;
        case 8:
            {
                *out_value = INVALIDATE_BY_PURGING;
            }
            break;
        case 9:
            {
                *out_value = INVALIDATE_BY_PURGING_WITH_SHUTDOWN;
            }
            break;
        case 10:
            {
                *out_value = PSA_KEY_ID_USER_MAX;
            }
            break;
        case 11:
            {
                *out_value = PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH;
            }
            break;
        case 12:
            {
                *out_value = PSA_ALG_ECDSA_ANY;
            }
            break;
        case 13:
            {
                *out_value = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1);
            }
            break;
        case 14:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256));
            }
            break;
        case 15:
            {
                *out_value = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(2, PSA_KEY_LOCATION_LOCAL_STORAGE);
            }
            break;
        case 16:
            {
                *out_value = CLOSE_BEFORE;
            }
            break;
        case 17:
            {
                *out_value = CLOSE_AFTER;
            }
            break;
        case 18:
            {
                *out_value = KEEP_OPEN;
            }
            break;
        case 19:
            {
                *out_value = PSA_ERROR_DOES_NOT_EXIST;
            }
            break;
        case 20:
            {
                *out_value = PSA_CRYPTO_ITS_RANDOM_SEED_UID;
            }
            break;
        case 21:
            {
                *out_value = PSA_KEY_ID_VENDOR_MAX + 1;
            }
            break;
        case 22:
            {
                *out_value = PSA_KEY_ID_USER_MAX + 1;
            }
            break;
        case 23:
            {
                *out_value = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_READ_ONLY, PSA_KEY_LOCATION_LOCAL_STORAGE);
            }
            break;
        case 24:
            {
                *out_value = PSA_ERROR_INVALID_ARGUMENT;
            }
            break;
        case 25:
            {
                *out_value = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_DEFAULT, 0xbad10cU);
            }
            break;
        case 26:
            {
                *out_value = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_VOLATILE, 0xbad10cU);
            }
            break;
        case 27:
            {
                *out_value = PSA_KEY_LIFETIME_VOLATILE;
            }
            break;
        case 28:
            {
                *out_value = PSA_ERROR_NOT_SUPPORTED;
            }
            break;
        case 29:
            {
                *out_value = PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_COPY;
            }
            break;
        case 30:
            {
                *out_value = PSA_KEY_USAGE_EXPORT;
            }
            break;
        case 31:
            {
                *out_value = PSA_ALG_CTR;
            }
            break;
        case 32:
            {
                *out_value = PSA_ALG_CBC_NO_PADDING;
            }
            break;
        case 33:
            {
                *out_value = PSA_KEY_TYPE_AES;
            }
            break;
        case 34:
            {
                *out_value = INVALID_HANDLE_0;
            }
            break;
        case 35:
            {
                *out_value = PSA_SUCCESS;
            }
            break;
        case 36:
            {
                *out_value = INVALID_HANDLE_UNOPENED;
            }
            break;
        case 37:
            {
                *out_value = PSA_ERROR_INVALID_HANDLE;
            }
            break;
        case 38:
            {
                *out_value = INVALID_HANDLE_CLOSED;
            }
            break;
        case 39:
            {
                *out_value = INVALID_HANDLE_HUGE;
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
#if defined(PSA_WANT_ALG_ECDSA)
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
#if defined(PSA_WANT_ALG_ECDH)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 4:
            {
#if defined(PSA_WANT_ALG_HKDF)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 5:
            {
#if defined(PSA_WANT_ALG_SHA_256)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 6:
            {
#if defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 7:
            {
#if !defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 8:
            {
#if !defined(MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 9:
            {
#if defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 10:
            {
#if defined(PSA_WANT_ALG_CBC_NO_PADDING)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 11:
            {
#if defined(PSA_WANT_ALG_CTR)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 12:
            {
#if defined(PSA_WANT_KEY_TYPE_AES)
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

#if defined(MBEDTLS_PSA_CRYPTO_C)
    test_transient_slot_lifecycle_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)
    test_persistent_slot_lifecycle_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)
    test_create_existent_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_PSA_CRYPTO_C)
    test_open_fail_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_PSA_CRYPTO_C)
    test_create_fail_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_PSA_CRYPTO_C)
    test_copy_across_lifetimes_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_PSA_CRYPTO_C)
    test_copy_to_occupied_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_PSA_CRYPTO_C)
    test_invalid_handle_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_PSA_CRYPTO_C)
    test_many_transient_keys_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)
    test_key_slot_eviction_to_import_new_key_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)
    test_non_reusable_key_slots_integrity_in_case_of_key_slot_starvation_wrapper,
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
    const char *default_filename = "./test_suite_psa_crypto_slot_management.datax";
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
