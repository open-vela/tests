#line 2 "suites/main_test.function"
/*
 * *** THIS FILE HAS BEEN MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py
 *
 * Test file      : ./test_suite_psa_crypto_se_driver_hal.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_psa_crypto_se_driver_hal.function
 *      Test suite data     : suites/test_suite_psa_crypto_se_driver_hal.data
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

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
#line 2 "suites/test_suite_psa_crypto_se_driver_hal.function"
#include "psa/crypto_se_driver.h"

#include "psa_crypto_se.h"
#include "psa_crypto_slot_management.h"
#include "psa_crypto_storage.h"

/* Invasive peeking: check the persistent data */
#if defined(MBEDTLS_PSA_ITS_FILE_C)
#include "psa_crypto_its.h"
#else /* Native ITS implementation */
#include "psa/error.h"
#include "psa/internal_trusted_storage.h"
#endif


/****************************************************************/
/* Test driver helpers */
/****************************************************************/

/** The minimum valid location value for a secure element driver. */
#define MIN_DRIVER_LOCATION 1

/** The location and lifetime used for tests that use a single driver. */
#define TEST_DRIVER_LOCATION 1
#define TEST_SE_PERSISTENT_LIFETIME                             \
    (PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(           \
         PSA_KEY_PERSISTENCE_DEFAULT, TEST_DRIVER_LOCATION))

#define TEST_SE_VOLATILE_LIFETIME                               \
    (PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(           \
         PSA_KEY_PERSISTENCE_VOLATILE, TEST_DRIVER_LOCATION))

/** The driver detected a condition that shouldn't happen.
 * This is probably a bug in the library. */
#define PSA_ERROR_DETECTED_BY_DRIVER ((psa_status_t) (-500))

/** Like #TEST_ASSERT for use in a driver method, with no cleanup.
 *
 * If an error happens, this macro returns from the calling function.
 *
 * Use this macro to assert on guarantees provided by the core.
 */
#define DRIVER_ASSERT_RETURN(TEST)                        \
    do {                                                    \
        if (!(TEST))                                       \
        {                                                    \
            mbedtls_test_fail( #TEST, __LINE__, __FILE__);   \
            return PSA_ERROR_DETECTED_BY_DRIVER;           \
        }                                                    \
    } while (0)

/** Like #TEST_ASSERT for use in a driver method, with cleanup.
 *
 * In case of error, this macro sets `status` and jumps to the
 * label `exit`.
 *
 * Use this macro to assert on guarantees provided by the core.
 */
#define DRIVER_ASSERT(TEST)                               \
    do {                                                    \
        if (!(TEST))                                       \
        {                                                    \
            mbedtls_test_fail( #TEST, __LINE__, __FILE__);   \
            status = PSA_ERROR_DETECTED_BY_DRIVER;            \
            goto exit;                                        \
        }                                                    \
    } while (0)

/** Like #PSA_ASSERT for a PSA API call that calls a driver underneath.
 *
 * Run the code \p expr. If this returns \p expected_status,
 * do nothing. If this returns #PSA_ERROR_DETECTED_BY_DRIVER,
 * jump directly to the `exit` label. If this returns any other
 * status, call mbedtls_test_fail() then jump to `exit`.
 *
 * The special case for #PSA_ERROR_DETECTED_BY_DRIVER is because in this
 * case, the test driver code is expected to have called mbedtls_test_fail()
 * already, so we make sure not to overwrite the failure information.
 */
#define PSA_ASSERT_VIA_DRIVER(expr, expected_status)                  \
    do {                                                                \
        psa_status_t PSA_ASSERT_VIA_DRIVER_status = (expr);           \
        if (PSA_ASSERT_VIA_DRIVER_status == PSA_ERROR_DETECTED_BY_DRIVER) \
        goto exit;                                                  \
        if (PSA_ASSERT_VIA_DRIVER_status != (expected_status))       \
        {                                                               \
            mbedtls_test_fail( #expr, __LINE__, __FILE__);                     \
            goto exit;                                                  \
        }                                                               \
    } while (0)



/****************************************************************/
/* Domain support functions */
/****************************************************************/

/* Return the exact bit size given a curve family and a byte length. */
static size_t ecc_curve_bits(psa_ecc_family_t curve, size_t data_length)
{
    switch (curve) {
        case PSA_ECC_FAMILY_SECP_R1:
            if (data_length == PSA_BYTES_TO_BITS(521)) {
                return 521;
            }
            break;
        case PSA_ECC_FAMILY_MONTGOMERY:
            if (data_length == PSA_BYTES_TO_BITS(255)) {
                return 255;
            }
    }
    /* If not listed above, assume a multiple of 8 bits. */
    return PSA_BYTES_TO_BITS(data_length);
}


/****************************************************************/
/* Miscellaneous driver methods */
/****************************************************************/

typedef struct {
    psa_key_slot_number_t slot_number;
    psa_key_creation_method_t method;
    psa_status_t status;
} validate_slot_number_directions_t;
static validate_slot_number_directions_t validate_slot_number_directions;

/* Validate a choice of slot number as directed. */
static psa_status_t validate_slot_number_as_directed(
    psa_drv_se_context_t *context,
    void *persistent_data,
    const psa_key_attributes_t *attributes,
    psa_key_creation_method_t method,
    psa_key_slot_number_t slot_number)
{
    (void) context;
    (void) persistent_data;
    (void) attributes;
    DRIVER_ASSERT_RETURN(slot_number ==
                         validate_slot_number_directions.slot_number);
    DRIVER_ASSERT_RETURN(method ==
                         validate_slot_number_directions.method);
    return validate_slot_number_directions.status;
}

/* Allocate slot numbers with a monotonic counter. */
static psa_key_slot_number_t shadow_counter;
static void counter_reset(void)
{
    shadow_counter = 0;
}
static psa_status_t counter_allocate(psa_drv_se_context_t *context,
                                     void *persistent_data,
                                     const psa_key_attributes_t *attributes,
                                     psa_key_creation_method_t method,
                                     psa_key_slot_number_t *slot_number)
{
    psa_key_slot_number_t *p_counter = persistent_data;
    (void) attributes;
    (void) method;
    if (context->persistent_data_size != sizeof(psa_key_slot_number_t)) {
        return PSA_ERROR_DETECTED_BY_DRIVER;
    }
    ++*p_counter;
    if (*p_counter == 0) {
        return PSA_ERROR_INSUFFICIENT_STORAGE;
    }
    shadow_counter = *p_counter;
    *slot_number = *p_counter;
    return PSA_SUCCESS;
}

/* Null import: do nothing, but pretend it worked. */
#if defined(AT_LEAST_ONE_BUILTIN_KDF)
static psa_status_t null_import(psa_drv_se_context_t *context,
                                psa_key_slot_number_t slot_number,
                                const psa_key_attributes_t *attributes,
                                const uint8_t *data,
                                size_t data_length,
                                size_t *bits)
{
    (void) context;
    (void) slot_number;
    (void) attributes;
    (void) data;
    /* We're supposed to return a key size. Return one that's correct for
     * plain data keys. */
    *bits = PSA_BYTES_TO_BITS(data_length);
    return PSA_SUCCESS;
}
#endif /* AT_LEAST_ONE_BUILTIN_KDF */

/* Null generate: do nothing, but pretend it worked. */
#if defined(AT_LEAST_ONE_BUILTIN_KDF)
static psa_status_t null_generate(psa_drv_se_context_t *context,
                                  psa_key_slot_number_t slot_number,
                                  const psa_key_attributes_t *attributes,
                                  uint8_t *pubkey,
                                  size_t pubkey_size,
                                  size_t *pubkey_length)
{
    (void) context;
    (void) slot_number;
    (void) attributes;

    DRIVER_ASSERT_RETURN(*pubkey_length == 0);
    if (!PSA_KEY_TYPE_IS_KEY_PAIR(psa_get_key_type(attributes))) {
        DRIVER_ASSERT_RETURN(pubkey == NULL);
        DRIVER_ASSERT_RETURN(pubkey_size == 0);
    }

    return PSA_SUCCESS;
}
#endif /* AT_LEAST_ONE_BUILTIN_KDF */

/* Null destroy: do nothing, but pretend it worked. */
static psa_status_t null_destroy(psa_drv_se_context_t *context,
                                 void *persistent_data,
                                 psa_key_slot_number_t slot_number)
{
    (void) context;
    (void) persistent_data;
    (void) slot_number;
    return PSA_SUCCESS;
}



/****************************************************************/
/* RAM-based test driver */
/****************************************************************/

#define RAM_MAX_KEY_SIZE 64
typedef struct {
    psa_key_lifetime_t lifetime;
    psa_key_type_t type;
    size_t bits;
    uint8_t content[RAM_MAX_KEY_SIZE];
} ram_slot_t;
static ram_slot_t ram_slots[16];

/* A type with at least ARRAY_LENGTH(ram_slots) bits, containing a
 * bit vector indicating which slots are in use. */
typedef uint16_t ram_slot_usage_t;

static ram_slot_usage_t ram_shadow_slot_usage;

static uint8_t ram_min_slot = 0;

static void ram_slots_reset(void)
{
    memset(ram_slots, 0, sizeof(ram_slots));
    ram_min_slot = 0;
    ram_shadow_slot_usage = 0;
}

/* Common parts of key creation.
 *
 * In case of error, zero out ram_slots[slot_number]. But don't
 * do that if the error is PSA_ERROR_DETECTED_BY_DRIVER: in this case
 * you don't need to clean up (ram_slot_reset() will take care of it
 * in the test case function's cleanup code) and it might be wrong
 * (if slot_number is invalid).
 */
static psa_status_t ram_create_common(psa_drv_se_context_t *context,
                                      psa_key_slot_number_t slot_number,
                                      const psa_key_attributes_t *attributes,
                                      size_t required_storage)
{
    (void) context;
    DRIVER_ASSERT_RETURN(slot_number < ARRAY_LENGTH(ram_slots));

    ram_slots[slot_number].lifetime = psa_get_key_lifetime(attributes);
    ram_slots[slot_number].type = psa_get_key_type(attributes);
    ram_slots[slot_number].bits = psa_get_key_bits(attributes);

    if (required_storage > sizeof(ram_slots[slot_number].content)) {
        memset(&ram_slots[slot_number], 0, sizeof(ram_slots[slot_number]));
        return PSA_ERROR_INSUFFICIENT_STORAGE;
    }

    return PSA_SUCCESS;
}

/* This function does everything except actually generating key material.
 * After calling it, you must copy the desired key material to
 * ram_slots[slot_number].content. */
static psa_status_t ram_fake_generate(psa_drv_se_context_t *context,
                                      psa_key_slot_number_t slot_number,
                                      const psa_key_attributes_t *attributes,
                                      uint8_t *pubkey,
                                      size_t pubkey_size,
                                      size_t *pubkey_length)
{
    psa_status_t status;
    size_t required_storage =
        PSA_EXPORT_KEY_OUTPUT_SIZE(psa_get_key_type(attributes),
                                   psa_get_key_bits(attributes));

    DRIVER_ASSERT_RETURN(*pubkey_length == 0);
    if (!PSA_KEY_TYPE_IS_KEY_PAIR(psa_get_key_type(attributes))) {
        DRIVER_ASSERT_RETURN(pubkey == NULL);
        DRIVER_ASSERT_RETURN(pubkey_size == 0);
    }

    status = ram_create_common(context, slot_number, attributes,
                               required_storage);
    return status;
}

static psa_status_t ram_import(psa_drv_se_context_t *context,
                               psa_key_slot_number_t slot_number,
                               const psa_key_attributes_t *attributes,
                               const uint8_t *data,
                               size_t data_length,
                               size_t *bits)
{
    psa_key_type_t type = psa_get_key_type(attributes);
    psa_status_t status = ram_create_common(context, slot_number, attributes,
                                            data_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* The RAM driver only works for certain key types: raw keys,
     * and ECC key pairs. This is true in particular of the bit-size
     * calculation here. */
    if (PSA_KEY_TYPE_IS_UNSTRUCTURED(type)) {
        *bits = PSA_BYTES_TO_BITS(data_length);
    } else if (PSA_KEY_TYPE_IS_ECC_KEY_PAIR(type)) {
        *bits = ecc_curve_bits(PSA_KEY_TYPE_ECC_GET_FAMILY(type), data_length);
        if (*bits == 0) {
            return PSA_ERROR_DETECTED_BY_DRIVER;
        }
    } else {
        memset(&ram_slots[slot_number], 0, sizeof(ram_slots[slot_number]));
        return PSA_ERROR_NOT_SUPPORTED;
    }

    ram_slots[slot_number].bits = *bits;
    memcpy(ram_slots[slot_number].content, data, data_length);

    return PSA_SUCCESS;
}

static psa_status_t ram_export(psa_drv_se_context_t *context,
                               psa_key_slot_number_t slot_number,
                               uint8_t *data,
                               size_t data_size,
                               size_t *data_length)
{
    size_t actual_size;
    (void) context;
    DRIVER_ASSERT_RETURN(slot_number < ARRAY_LENGTH(ram_slots));
    actual_size = PSA_BITS_TO_BYTES(ram_slots[slot_number].bits);
    if (actual_size > data_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    *data_length = actual_size;
    memcpy(data, ram_slots[slot_number].content, actual_size);
    return PSA_SUCCESS;
}

static psa_status_t ram_export_public(psa_drv_se_context_t *context,
                                      psa_key_slot_number_t slot_number,
                                      uint8_t *data,
                                      size_t data_size,
                                      size_t *data_length)
{
    psa_status_t status;
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    (void) context;
    DRIVER_ASSERT_RETURN(slot_number < ARRAY_LENGTH(ram_slots));
    DRIVER_ASSERT_RETURN(
        PSA_KEY_TYPE_IS_KEY_PAIR(ram_slots[slot_number].type));

    psa_set_key_type(&attributes, ram_slots[slot_number].type);
    status = psa_import_key(&attributes,
                            ram_slots[slot_number].content,
                            PSA_BITS_TO_BYTES(ram_slots[slot_number].bits),
                            &key);
    if (status != PSA_SUCCESS) {
        return status;
    }
    status = psa_export_public_key(key, data, data_size, data_length);
    psa_destroy_key(key);
    return PSA_SUCCESS;
}

static psa_status_t ram_destroy(psa_drv_se_context_t *context,
                                void *persistent_data,
                                psa_key_slot_number_t slot_number)
{
    ram_slot_usage_t *slot_usage = persistent_data;
    DRIVER_ASSERT_RETURN(context->persistent_data_size == sizeof(ram_slot_usage_t));
    DRIVER_ASSERT_RETURN(slot_number < ARRAY_LENGTH(ram_slots));
    memset(&ram_slots[slot_number], 0, sizeof(ram_slots[slot_number]));
    *slot_usage &= ~(ram_slot_usage_t) (1 << slot_number);
    ram_shadow_slot_usage = *slot_usage;
    return PSA_SUCCESS;
}

static psa_status_t ram_allocate(psa_drv_se_context_t *context,
                                 void *persistent_data,
                                 const psa_key_attributes_t *attributes,
                                 psa_key_creation_method_t method,
                                 psa_key_slot_number_t *slot_number)
{
    ram_slot_usage_t *slot_usage = persistent_data;
    (void) attributes;
    (void) method;
    DRIVER_ASSERT_RETURN(context->persistent_data_size == sizeof(ram_slot_usage_t));
    for (*slot_number = ram_min_slot;
         *slot_number < ARRAY_LENGTH(ram_slots);
         ++(*slot_number)) {
        if (!(*slot_usage & 1 << *slot_number)) {
            ram_shadow_slot_usage = *slot_usage;
            return PSA_SUCCESS;
        }
    }
    return PSA_ERROR_INSUFFICIENT_STORAGE;
}

static psa_status_t ram_validate_slot_number(
    psa_drv_se_context_t *context,
    void *persistent_data,
    const psa_key_attributes_t *attributes,
    psa_key_creation_method_t method,
    psa_key_slot_number_t slot_number)
{
    (void) context;
    (void) persistent_data;
    (void) attributes;
    (void) method;
    if (slot_number >= ARRAY_LENGTH(ram_slots)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    return PSA_SUCCESS;
}

static psa_status_t ram_sign(psa_drv_se_context_t *context,
                             psa_key_slot_number_t slot_number,
                             psa_algorithm_t alg,
                             const uint8_t *hash,
                             size_t hash_length,
                             uint8_t *signature,
                             size_t signature_size,
                             size_t *signature_length)
{
    ram_slot_t *slot;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;

    (void) context;
    DRIVER_ASSERT_RETURN(slot_number < ARRAY_LENGTH(ram_slots));
    slot = &ram_slots[slot_number];

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, slot->type);
    DRIVER_ASSERT(psa_import_key(&attributes,
                                 slot->content,
                                 PSA_BITS_TO_BYTES(slot->bits),
                                 &key) == PSA_SUCCESS);
    status = psa_sign_hash(key, alg,
                           hash, hash_length,
                           signature, signature_size, signature_length);

exit:
    psa_destroy_key(key);
    return status;
}

static psa_status_t ram_verify(psa_drv_se_context_t *context,
                               psa_key_slot_number_t slot_number,
                               psa_algorithm_t alg,
                               const uint8_t *hash,
                               size_t hash_length,
                               const uint8_t *signature,
                               size_t signature_length)
{
    ram_slot_t *slot;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;

    (void) context;
    DRIVER_ASSERT_RETURN(slot_number < ARRAY_LENGTH(ram_slots));
    slot = &ram_slots[slot_number];

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, slot->type);
    DRIVER_ASSERT(psa_import_key(&attributes,
                                 slot->content,
                                 PSA_BITS_TO_BYTES(slot->bits),
                                 &key) ==
                  PSA_SUCCESS);
    status = psa_verify_hash(key, alg,
                             hash, hash_length,
                             signature, signature_length);

exit:
    psa_destroy_key(key);
    return status;
}


/****************************************************************/
/* Other test helper functions */
/****************************************************************/

typedef enum {
    SIGN_IN_SOFTWARE_AND_PARALLEL_CREATION,
    SIGN_IN_DRIVER_AND_PARALLEL_CREATION,
    SIGN_IN_DRIVER_THEN_EXPORT_PUBLIC,
} sign_verify_method_t;

/* Check that the attributes of a key reported by psa_get_key_attributes()
 * are consistent with the attributes used when creating the key. */
static int check_key_attributes(
    mbedtls_svc_key_id_t key,
    const psa_key_attributes_t *reference_attributes)
{
    int ok = 0;
    psa_key_attributes_t actual_attributes = PSA_KEY_ATTRIBUTES_INIT;

    PSA_ASSERT(psa_get_key_attributes(key, &actual_attributes));

    TEST_ASSERT(mbedtls_svc_key_id_equal(
                    psa_get_key_id(&actual_attributes),
                    psa_get_key_id(reference_attributes)));
    TEST_EQUAL(psa_get_key_lifetime(&actual_attributes),
               psa_get_key_lifetime(reference_attributes));
    TEST_EQUAL(psa_get_key_type(&actual_attributes),
               psa_get_key_type(reference_attributes));
    TEST_EQUAL(psa_get_key_usage_flags(&actual_attributes),
               psa_get_key_usage_flags(reference_attributes));
    TEST_EQUAL(psa_get_key_algorithm(&actual_attributes),
               psa_get_key_algorithm(reference_attributes));
    TEST_EQUAL(psa_get_key_enrollment_algorithm(&actual_attributes),
               psa_get_key_enrollment_algorithm(reference_attributes));
    if (psa_get_key_bits(reference_attributes) != 0) {
        TEST_EQUAL(psa_get_key_bits(&actual_attributes),
                   psa_get_key_bits(reference_attributes));
    }

    {
        psa_key_slot_number_t actual_slot_number = 0xdeadbeef;
        psa_key_slot_number_t desired_slot_number = 0xb90cc011;
        psa_key_lifetime_t lifetime =
            psa_get_key_lifetime(&actual_attributes);
        psa_status_t status = psa_get_key_slot_number(&actual_attributes,
                                                      &actual_slot_number);
        if (PSA_KEY_LIFETIME_GET_LOCATION(lifetime) < MIN_DRIVER_LOCATION) {
            /* The key is not in a secure element. */
            TEST_EQUAL(status, PSA_ERROR_INVALID_ARGUMENT);
        } else {
            /* The key is in a secure element. If it had been created
             * in a specific slot, check that it is reported there. */
            PSA_ASSERT(status);
            status = psa_get_key_slot_number(reference_attributes,
                                             &desired_slot_number);
            if (status == PSA_SUCCESS) {
                TEST_EQUAL(desired_slot_number, actual_slot_number);
            }
        }
    }
    ok = 1;

exit:
    /*
     * Actual key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes(&actual_attributes);

    return ok;
}

/* Get the file UID corresponding to the specified location.
 * If this changes, the storage format version must change.
 * See psa_get_se_driver_its_file_uid() in psa_crypto_se.c.
 */
psa_storage_uid_t file_uid_for_location(psa_key_location_t location)
{
    if (location > PSA_MAX_SE_LOCATION) {
        return 0;
    }
    return 0xfffffe00 + location;
}

/* Check that the persistent data of a driver has its expected content. */
static int check_persistent_data(psa_key_location_t location,
                                 const void *expected_data,
                                 size_t size)
{
    psa_storage_uid_t uid = file_uid_for_location(location);
    struct psa_storage_info_t info;
    uint8_t *loaded = NULL;
    int ok = 0;

    PSA_ASSERT(psa_its_get_info(uid, &info));
    ASSERT_ALLOC(loaded, info.size);
    PSA_ASSERT(psa_its_get(uid, 0, info.size, loaded, NULL));
    ASSERT_COMPARE(expected_data, size, loaded, info.size);
    ok = 1;

exit:
    mbedtls_free(loaded);
    return ok;
}

/* Check that no persistent data exists for the given location. */
static int check_no_persistent_data(psa_key_location_t location)
{
    psa_storage_uid_t uid = file_uid_for_location(location);
    struct psa_storage_info_t info;
    int ok = 0;

    TEST_EQUAL(psa_its_get_info(uid, &info), PSA_ERROR_DOES_NOT_EXIST);
    ok = 1;

exit:
    return ok;
}

/* Check that a function's return status is "smoke-free", i.e. that
 * it's an acceptable error code when calling an API function that operates
 * on a key with potentially bogus parameters. */
#if defined(AT_LEAST_ONE_BUILTIN_KDF)
static int is_status_smoke_free(psa_status_t status)
{
    switch (status) {
        case PSA_SUCCESS:
        case PSA_ERROR_NOT_SUPPORTED:
        case PSA_ERROR_NOT_PERMITTED:
        case PSA_ERROR_BUFFER_TOO_SMALL:
        case PSA_ERROR_INVALID_ARGUMENT:
        case PSA_ERROR_INVALID_SIGNATURE:
        case PSA_ERROR_INVALID_PADDING:
            return 1;
        default:
            return 0;
    }
}
#endif /* AT_LEAST_ONE_BUILTIN_KDF */

#define SMOKE_ASSERT(expr)                    \
    TEST_ASSERT(is_status_smoke_free(expr))

/* Smoke test a key. There are mostly no wrong answers here since we pass
 * mostly bogus parameters: the goal is to ensure that there is no memory
 * corruption or crash. This test function is most useful when run under
 * an environment with sanity checks such as ASan or MSan. */
#if defined(AT_LEAST_ONE_BUILTIN_KDF)
static int smoke_test_key(mbedtls_svc_key_id_t key)
{
    int ok = 0;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_mac_operation_t mac_operation = PSA_MAC_OPERATION_INIT;
    psa_cipher_operation_t cipher_operation = PSA_CIPHER_OPERATION_INIT;
    psa_key_derivation_operation_t derivation_operation =
        PSA_KEY_DERIVATION_OPERATION_INIT;
    uint8_t buffer[80]; /* large enough for a public key for ECDH */
    size_t length;
    mbedtls_svc_key_id_t key2 = MBEDTLS_SVC_KEY_ID_INIT;

    SMOKE_ASSERT(psa_get_key_attributes(key, &attributes));

    SMOKE_ASSERT(psa_export_key(key,
                                buffer, sizeof(buffer), &length));
    SMOKE_ASSERT(psa_export_public_key(key,
                                       buffer, sizeof(buffer), &length));

    SMOKE_ASSERT(psa_copy_key(key, &attributes, &key2));
    if (!mbedtls_svc_key_id_is_null(key2)) {
        PSA_ASSERT(psa_destroy_key(key2));
    }

    SMOKE_ASSERT(psa_mac_sign_setup(&mac_operation, key, PSA_ALG_CMAC));
    PSA_ASSERT(psa_mac_abort(&mac_operation));
    SMOKE_ASSERT(psa_mac_verify_setup(&mac_operation, key,
                                      PSA_ALG_HMAC(PSA_ALG_SHA_256)));
    PSA_ASSERT(psa_mac_abort(&mac_operation));

    SMOKE_ASSERT(psa_cipher_encrypt_setup(&cipher_operation, key,
                                          PSA_ALG_CTR));
    PSA_ASSERT(psa_cipher_abort(&cipher_operation));
    SMOKE_ASSERT(psa_cipher_decrypt_setup(&cipher_operation, key,
                                          PSA_ALG_CTR));
    PSA_ASSERT(psa_cipher_abort(&cipher_operation));

    SMOKE_ASSERT(psa_aead_encrypt(key, PSA_ALG_CCM,
                                  buffer, sizeof(buffer),
                                  NULL, 0,
                                  buffer, sizeof(buffer),
                                  buffer, sizeof(buffer), &length));
    SMOKE_ASSERT(psa_aead_decrypt(key, PSA_ALG_CCM,
                                  buffer, sizeof(buffer),
                                  NULL, 0,
                                  buffer, sizeof(buffer),
                                  buffer, sizeof(buffer), &length));

    SMOKE_ASSERT(psa_sign_hash(key, PSA_ALG_ECDSA_ANY,
                               buffer, 32,
                               buffer, sizeof(buffer), &length));
    SMOKE_ASSERT(psa_verify_hash(key, PSA_ALG_ECDSA_ANY,
                                 buffer, 32,
                                 buffer, sizeof(buffer)));

    SMOKE_ASSERT(psa_asymmetric_encrypt(key, PSA_ALG_RSA_PKCS1V15_CRYPT,
                                        buffer, 10, NULL, 0,
                                        buffer, sizeof(buffer), &length));
    SMOKE_ASSERT(psa_asymmetric_decrypt(key, PSA_ALG_RSA_PKCS1V15_CRYPT,
                                        buffer, sizeof(buffer), NULL, 0,
                                        buffer, sizeof(buffer), &length));

#if defined(MBEDTLS_SHA256_C)
    /* Try the key in a plain key derivation. */
    PSA_ASSERT(psa_key_derivation_setup(&derivation_operation,
                                        PSA_ALG_HKDF(PSA_ALG_SHA_256)));
    PSA_ASSERT(psa_key_derivation_input_bytes(&derivation_operation,
                                              PSA_KEY_DERIVATION_INPUT_SALT,
                                              NULL, 0));
    SMOKE_ASSERT(psa_key_derivation_input_key(&derivation_operation,
                                              PSA_KEY_DERIVATION_INPUT_SECRET,
                                              key));
    PSA_ASSERT(psa_key_derivation_abort(&derivation_operation));

    /* If the key is asymmetric, try it in a key agreement, both as
     * part of a derivation operation and standalone. */
    if (psa_export_public_key(key, buffer, sizeof(buffer), &length) ==
        PSA_SUCCESS) {
        psa_algorithm_t alg =
            PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH,
                                  PSA_ALG_HKDF(PSA_ALG_SHA_256));
        PSA_ASSERT(psa_key_derivation_setup(&derivation_operation, alg));
        PSA_ASSERT(psa_key_derivation_input_bytes(
                       &derivation_operation, PSA_KEY_DERIVATION_INPUT_SALT,
                       NULL, 0));
        SMOKE_ASSERT(psa_key_derivation_key_agreement(
                         &derivation_operation,
                         PSA_KEY_DERIVATION_INPUT_SECRET,
                         key, buffer, length));
        PSA_ASSERT(psa_key_derivation_abort(&derivation_operation));

        SMOKE_ASSERT(psa_raw_key_agreement(
                         alg, key, buffer, length,
                         buffer, sizeof(buffer), &length));
    }
#endif /* MBEDTLS_SHA256_C */

    ok = 1;

exit:
    /*
     * Key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes(&attributes);

    return ok;
}
#endif /* AT_LEAST_ONE_BUILTIN_KDF */

static void psa_purge_storage(void)
{
    /* The generic code in mbedtls_test_psa_purge_key_storage()
     * (which is called by PSA_DONE()) doesn't take care of things that are
     * specific to dynamic secure elements. */
    psa_key_location_t location;
    /* Purge the transaction file. */
    psa_crypto_stop_transaction();
    /* Purge driver persistent data. */
    for (location = 0; location < PSA_MAX_SE_LOCATION; location++) {
        psa_destroy_se_persistent_data(location);
    }
}

#line 793 "suites/test_suite_psa_crypto_se_driver_hal.function"
static void test_register_one(int location, int version, int expected_status_arg)
{
    psa_status_t expected_status = expected_status_arg;
    psa_drv_se_t driver;

    memset(&driver, 0, sizeof(driver));
    driver.hal_version = version;

    TEST_EQUAL(psa_register_se_driver(location, &driver),
               expected_status);

    PSA_ASSERT(psa_crypto_init());

exit:
    PSA_DONE();
}

static void test_register_one_wrapper( void ** params )
{

    test_register_one( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
#line 812 "suites/test_suite_psa_crypto_se_driver_hal.function"
static void test_register_twice(int count)
{
    psa_drv_se_t driver;
    psa_key_location_t location;
    psa_key_location_t max = MIN_DRIVER_LOCATION + count;

    memset(&driver, 0, sizeof(driver));
    driver.hal_version = PSA_DRV_SE_HAL_VERSION;

    for (location = MIN_DRIVER_LOCATION; location < max; location++) {
        PSA_ASSERT(psa_register_se_driver(location, &driver));
    }
    for (location = MIN_DRIVER_LOCATION; location < max; location++) {
        TEST_EQUAL(psa_register_se_driver(location, &driver),
                   PSA_ERROR_ALREADY_EXISTS);
    }

    PSA_ASSERT(psa_crypto_init());

exit:
    PSA_DONE();
}

static void test_register_twice_wrapper( void ** params )
{

    test_register_twice( *( (int *) params[0] ) );
}
#line 837 "suites/test_suite_psa_crypto_se_driver_hal.function"
static void test_register_max(void)
{
    psa_drv_se_t driver;
    psa_key_location_t location;
    psa_key_location_t max = MIN_DRIVER_LOCATION + PSA_MAX_SE_DRIVERS;

    memset(&driver, 0, sizeof(driver));
    driver.hal_version = PSA_DRV_SE_HAL_VERSION;

    for (location = MIN_DRIVER_LOCATION; location < max; location++) {
        PSA_ASSERT(psa_register_se_driver(location, &driver));
    }

    TEST_EQUAL(psa_register_se_driver(location, &driver),
               PSA_ERROR_INSUFFICIENT_MEMORY);

    PSA_ASSERT(psa_crypto_init());

exit:
    PSA_DONE();
}

static void test_register_max_wrapper( void ** params )
{
    (void)params;

    test_register_max(  );
}
#line 861 "suites/test_suite_psa_crypto_se_driver_hal.function"
static void test_key_creation_import_export(int lifetime_arg, int min_slot, int restart)
{
    psa_drv_se_t driver;
    psa_drv_se_key_management_t key_management;
    psa_key_lifetime_t lifetime = (psa_key_lifetime_t) lifetime_arg;
    psa_key_location_t location = PSA_KEY_LIFETIME_GET_LOCATION(lifetime);
    mbedtls_svc_key_id_t id = mbedtls_svc_key_id_make(1, 1);
    mbedtls_svc_key_id_t returned_id = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_handle_t handle;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    const uint8_t key_material[3] = { 0xfa, 0xca, 0xde };
    uint8_t exported[sizeof(key_material)];
    size_t exported_length;

    TEST_USES_KEY_ID(id);

    memset(&driver, 0, sizeof(driver));
    memset(&key_management, 0, sizeof(key_management));
    driver.hal_version = PSA_DRV_SE_HAL_VERSION;
    driver.key_management = &key_management;
    driver.persistent_data_size = sizeof(ram_slot_usage_t);
    key_management.p_allocate = ram_allocate;
    key_management.p_import = ram_import;
    key_management.p_destroy = ram_destroy;
    key_management.p_export = ram_export;
    ram_min_slot = min_slot;

    PSA_ASSERT(psa_register_se_driver(location, &driver));
    PSA_ASSERT(psa_crypto_init());

    /* Create a key. */
    psa_set_key_id(&attributes, id);
    psa_set_key_lifetime(&attributes, lifetime);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_EXPORT);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_RAW_DATA);
    PSA_ASSERT(psa_import_key(&attributes,
                              key_material, sizeof(key_material),
                              &returned_id));

    if (PSA_KEY_LIFETIME_IS_VOLATILE(lifetime)) {
        /* For volatile keys, check no persistent data was created */
        if (!check_no_persistent_data(location)) {
            goto exit;
        }
    } else {
        /* For persistent keys, check persistent data */
        if (!check_persistent_data(location,
                                   &ram_shadow_slot_usage,
                                   sizeof(ram_shadow_slot_usage))) {
            goto exit;
        }
    }

    /* Test that the key was created in the expected slot. */
    TEST_EQUAL(ram_slots[min_slot].type, PSA_KEY_TYPE_RAW_DATA);

    /* Maybe restart, to check that the information is saved correctly. */
    if (restart) {
        mbedtls_psa_crypto_free();
        PSA_ASSERT(psa_register_se_driver(location, &driver));
        PSA_ASSERT(psa_crypto_init());

        if (PSA_KEY_LIFETIME_IS_VOLATILE(lifetime)) {
            /* Check that the PSA core has no knowledge of the volatile key */
            TEST_ASSERT(psa_open_key(returned_id, &handle) ==
                        PSA_ERROR_DOES_NOT_EXIST);

            /* Drop data from our mockup driver */
            ram_slots_reset();
            ram_min_slot = min_slot;

            /* Re-import key */
            PSA_ASSERT(psa_import_key(&attributes,
                                      key_material, sizeof(key_material),
                                      &returned_id));
        } else {
            /* Check the persistent key file */
            if (!check_persistent_data(location,
                                       &ram_shadow_slot_usage,
                                       sizeof(ram_shadow_slot_usage))) {
                goto exit;
            }
        }
    }

    /* Test that the key was created in the expected slot. */
    TEST_EQUAL(ram_slots[min_slot].type, PSA_KEY_TYPE_RAW_DATA);

    /* Test the key attributes, including the reported slot number. */
    psa_set_key_bits(&attributes,
                     PSA_BYTES_TO_BITS(sizeof(key_material)));
    psa_set_key_slot_number(&attributes, min_slot);

    if (PSA_KEY_LIFETIME_IS_VOLATILE(lifetime)) {
        attributes.core.id = returned_id;
    } else {
        psa_set_key_id(&attributes, returned_id);
    }

    if (!check_key_attributes(returned_id, &attributes)) {
        goto exit;
    }

    /* Test the key data. */
    PSA_ASSERT(psa_export_key(returned_id,
                              exported, sizeof(exported),
                              &exported_length));
    ASSERT_COMPARE(key_material, sizeof(key_material),
                   exported, exported_length);

    PSA_ASSERT(psa_destroy_key(returned_id));
    if (!check_persistent_data(location,
                               &ram_shadow_slot_usage,
                               sizeof(ram_shadow_slot_usage))) {
        goto exit;
    }
    TEST_EQUAL(psa_open_key(returned_id, &handle),
               PSA_ERROR_DOES_NOT_EXIST);

    /* Test that the key has been erased from the designated slot. */
    TEST_EQUAL(ram_slots[min_slot].type, 0);

exit:
    PSA_DONE();
    ram_slots_reset();
    psa_purge_storage();
}

static void test_key_creation_import_export_wrapper( void ** params )
{

    test_key_creation_import_export( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
#line 991 "suites/test_suite_psa_crypto_se_driver_hal.function"
static void test_key_creation_in_chosen_slot(int slot_arg,
                                 int restart,
                                 int expected_status_arg)
{
    psa_key_slot_number_t wanted_slot = slot_arg;
    psa_status_t expected_status = expected_status_arg;
    psa_status_t status;
    psa_drv_se_t driver;
    psa_drv_se_key_management_t key_management;
    psa_key_lifetime_t lifetime = TEST_SE_PERSISTENT_LIFETIME;
    psa_key_location_t location = PSA_KEY_LIFETIME_GET_LOCATION(lifetime);
    mbedtls_svc_key_id_t id = mbedtls_svc_key_id_make(1, 1);
    mbedtls_svc_key_id_t returned_id;
    psa_key_handle_t handle;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    const uint8_t key_material[3] = { 0xfa, 0xca, 0xde };

    TEST_USES_KEY_ID(id);

    memset(&driver, 0, sizeof(driver));
    memset(&key_management, 0, sizeof(key_management));
    driver.hal_version = PSA_DRV_SE_HAL_VERSION;
    driver.key_management = &key_management;
    driver.persistent_data_size = sizeof(ram_slot_usage_t);
    key_management.p_validate_slot_number = ram_validate_slot_number;
    key_management.p_import = ram_import;
    key_management.p_destroy = ram_destroy;
    key_management.p_export = ram_export;

    PSA_ASSERT(psa_register_se_driver(location, &driver));
    PSA_ASSERT(psa_crypto_init());

    /* Create a key. */
    psa_set_key_id(&attributes, id);
    psa_set_key_lifetime(&attributes, lifetime);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_EXPORT);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_RAW_DATA);
    psa_set_key_slot_number(&attributes, wanted_slot);
    status = psa_import_key(&attributes,
                            key_material, sizeof(key_material),
                            &returned_id);
    TEST_EQUAL(status, expected_status);

    if (status != PSA_SUCCESS) {
        goto exit;
    }
    if (!check_persistent_data(location,
                               &ram_shadow_slot_usage,
                               sizeof(ram_shadow_slot_usage))) {
        goto exit;
    }

    /* Maybe restart, to check that the information is saved correctly. */
    if (restart) {
        mbedtls_psa_crypto_free();
        PSA_ASSERT(psa_register_se_driver(location, &driver));
        PSA_ASSERT(psa_crypto_init());
        if (!check_persistent_data(location,
                                   &ram_shadow_slot_usage,
                                   sizeof(ram_shadow_slot_usage))) {
            goto exit;
        }
    }

    /* Test that the key was created in the expected slot. */
    TEST_EQUAL(ram_slots[wanted_slot].type, PSA_KEY_TYPE_RAW_DATA);

    /* Test that the key is reported with the correct attributes,
     * including the expected slot. */
    PSA_ASSERT(psa_get_key_attributes(id, &attributes));

    PSA_ASSERT(psa_destroy_key(id));
    if (!check_persistent_data(location,
                               &ram_shadow_slot_usage,
                               sizeof(ram_shadow_slot_usage))) {
        goto exit;
    }
    TEST_EQUAL(psa_open_key(id, &handle), PSA_ERROR_DOES_NOT_EXIST);

exit:
    /*
     * Key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes(&attributes);

    PSA_DONE();
    ram_slots_reset();
    psa_purge_storage();
}

static void test_key_creation_in_chosen_slot_wrapper( void ** params )
{

    test_key_creation_in_chosen_slot( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
#if defined(AT_LEAST_ONE_BUILTIN_KDF)
#line 1084 "suites/test_suite_psa_crypto_se_driver_hal.function"
static void test_import_key_smoke(int type_arg, int alg_arg,
                      data_t *key_material)
{
    psa_key_type_t type = type_arg;
    psa_algorithm_t alg = alg_arg;
    psa_drv_se_t driver;
    psa_drv_se_key_management_t key_management;
    psa_key_lifetime_t lifetime = TEST_SE_PERSISTENT_LIFETIME;
    psa_key_location_t location = PSA_KEY_LIFETIME_GET_LOCATION(lifetime);
    mbedtls_svc_key_id_t id = mbedtls_svc_key_id_make(1, 1);
    mbedtls_svc_key_id_t returned_id;
    psa_key_handle_t handle;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    TEST_USES_KEY_ID(id);

    memset(&driver, 0, sizeof(driver));
    memset(&key_management, 0, sizeof(key_management));
    driver.hal_version = PSA_DRV_SE_HAL_VERSION;
    driver.key_management = &key_management;
    driver.persistent_data_size = sizeof(psa_key_slot_number_t);
    key_management.p_allocate = counter_allocate;
    key_management.p_import = null_import;
    key_management.p_destroy = null_destroy;

    PSA_ASSERT(psa_register_se_driver(location, &driver));
    PSA_ASSERT(psa_crypto_init());

    /* Create a key. */
    psa_set_key_id(&attributes, id);
    psa_set_key_lifetime(&attributes, lifetime);
    psa_set_key_usage_flags(&attributes,
                            PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH |
                            PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT |
                            PSA_KEY_USAGE_EXPORT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, type);
    PSA_ASSERT(psa_import_key(&attributes,
                              key_material->x, key_material->len,
                              &returned_id));
    if (!check_persistent_data(location,
                               &shadow_counter, sizeof(shadow_counter))) {
        goto exit;
    }

    /* Do stuff with the key. */
    if (!smoke_test_key(id)) {
        goto exit;
    }

    /* Restart and try again. */
    mbedtls_psa_crypto_free();
    PSA_ASSERT(psa_register_se_driver(location, &driver));
    PSA_ASSERT(psa_crypto_init());
    if (!check_persistent_data(location,
                               &shadow_counter, sizeof(shadow_counter))) {
        goto exit;
    }
    if (!smoke_test_key(id)) {
        goto exit;
    }

    /* We're done. */
    PSA_ASSERT(psa_destroy_key(id));
    if (!check_persistent_data(location,
                               &shadow_counter, sizeof(shadow_counter))) {
        goto exit;
    }
    TEST_EQUAL(psa_open_key(id, &handle), PSA_ERROR_DOES_NOT_EXIST);

exit:
    PSA_DONE();
    counter_reset();
    psa_purge_storage();
}

static void test_import_key_smoke_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};

    test_import_key_smoke( *( (int *) params[0] ), *( (int *) params[1] ), &data2 );
}
#endif /* AT_LEAST_ONE_BUILTIN_KDF */
#line 1162 "suites/test_suite_psa_crypto_se_driver_hal.function"
static void test_generate_key_not_supported(int type_arg, int bits_arg)
{
    psa_key_type_t type = type_arg;
    size_t bits = bits_arg;
    psa_drv_se_t driver;
    psa_drv_se_key_management_t key_management;
    psa_key_lifetime_t lifetime = TEST_SE_PERSISTENT_LIFETIME;
    psa_key_location_t location = PSA_KEY_LIFETIME_GET_LOCATION(lifetime);
    mbedtls_svc_key_id_t id = mbedtls_svc_key_id_make(1, 1);
    mbedtls_svc_key_id_t returned_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    TEST_USES_KEY_ID(id);

    memset(&driver, 0, sizeof(driver));
    memset(&key_management, 0, sizeof(key_management));
    driver.hal_version = PSA_DRV_SE_HAL_VERSION;
    driver.key_management = &key_management;
    driver.persistent_data_size = sizeof(psa_key_slot_number_t);
    key_management.p_allocate = counter_allocate;
    /* No p_generate method */

    PSA_ASSERT(psa_register_se_driver(location, &driver));
    PSA_ASSERT(psa_crypto_init());

    psa_set_key_id(&attributes, id);
    psa_set_key_lifetime(&attributes, lifetime);
    psa_set_key_type(&attributes, type);
    psa_set_key_bits(&attributes, bits);
    TEST_EQUAL(psa_generate_key(&attributes, &returned_id),
               PSA_ERROR_NOT_SUPPORTED);

exit:
    PSA_DONE();
    counter_reset();
    psa_purge_storage();
}

static void test_generate_key_not_supported_wrapper( void ** params )
{

    test_generate_key_not_supported( *( (int *) params[0] ), *( (int *) params[1] ) );
}
#if defined(AT_LEAST_ONE_BUILTIN_KDF)
#line 1202 "suites/test_suite_psa_crypto_se_driver_hal.function"
static void test_generate_key_smoke(int type_arg, int bits_arg, int alg_arg)
{
    psa_key_type_t type = type_arg;
    psa_key_bits_t bits = bits_arg;
    psa_algorithm_t alg = alg_arg;
    psa_drv_se_t driver;
    psa_drv_se_key_management_t key_management;
    psa_key_lifetime_t lifetime = TEST_SE_PERSISTENT_LIFETIME;
    psa_key_location_t location = PSA_KEY_LIFETIME_GET_LOCATION(lifetime);
    mbedtls_svc_key_id_t id = mbedtls_svc_key_id_make(1, 1);
    mbedtls_svc_key_id_t returned_id;
    psa_key_handle_t handle;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    TEST_USES_KEY_ID(id);

    memset(&driver, 0, sizeof(driver));
    memset(&key_management, 0, sizeof(key_management));
    driver.hal_version = PSA_DRV_SE_HAL_VERSION;
    driver.key_management = &key_management;
    driver.persistent_data_size = sizeof(psa_key_slot_number_t);
    key_management.p_allocate = counter_allocate;
    key_management.p_generate = null_generate;
    key_management.p_destroy = null_destroy;

    PSA_ASSERT(psa_register_se_driver(location, &driver));
    PSA_ASSERT(psa_crypto_init());

    /* Create a key. */
    psa_set_key_id(&attributes, id);
    psa_set_key_lifetime(&attributes, lifetime);
    psa_set_key_usage_flags(&attributes,
                            PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH |
                            PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT |
                            PSA_KEY_USAGE_EXPORT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, type);
    psa_set_key_bits(&attributes, bits);
    PSA_ASSERT(psa_generate_key(&attributes, &returned_id));
    if (!check_persistent_data(location,
                               &shadow_counter, sizeof(shadow_counter))) {
        goto exit;
    }

    /* Do stuff with the key. */
    if (!smoke_test_key(id)) {
        goto exit;
    }

    /* Restart and try again. */
    mbedtls_psa_crypto_free();
    PSA_ASSERT(psa_register_se_driver(location, &driver));
    PSA_ASSERT(psa_crypto_init());
    if (!check_persistent_data(location,
                               &shadow_counter, sizeof(shadow_counter))) {
        goto exit;
    }
    if (!smoke_test_key(id)) {
        goto exit;
    }

    /* We're done. */
    PSA_ASSERT(psa_destroy_key(id));
    if (!check_persistent_data(location,
                               &shadow_counter, sizeof(shadow_counter))) {
        goto exit;
    }
    TEST_EQUAL(psa_open_key(id, &handle), PSA_ERROR_DOES_NOT_EXIST);

exit:
    PSA_DONE();
    counter_reset();
    psa_purge_storage();
}

static void test_generate_key_smoke_wrapper( void ** params )
{

    test_generate_key_smoke( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
#endif /* AT_LEAST_ONE_BUILTIN_KDF */
#line 1279 "suites/test_suite_psa_crypto_se_driver_hal.function"
static void test_sign_verify(int flow,
                 int type_arg, int alg_arg,
                 int bits_arg, data_t *key_material,
                 data_t *input)
{
    psa_key_type_t type = type_arg;
    psa_algorithm_t alg = alg_arg;
    size_t bits = bits_arg;
    /* Pass bits=0 to import, bits>0 to fake-generate */
    int generating = (bits != 0);

    psa_drv_se_t driver;
    psa_drv_se_key_management_t key_management;
    psa_drv_se_asymmetric_t asymmetric;

    psa_key_lifetime_t lifetime = TEST_SE_PERSISTENT_LIFETIME;
    psa_key_location_t location = PSA_KEY_LIFETIME_GET_LOCATION(lifetime);
    mbedtls_svc_key_id_t id = mbedtls_svc_key_id_make(1, 1);
    mbedtls_svc_key_id_t returned_id;
    mbedtls_svc_key_id_t sw_key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_attributes_t sw_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t drv_attributes;
    uint8_t signature[PSA_SIGNATURE_MAX_SIZE];
    size_t signature_length;

    TEST_USES_KEY_ID(id);

    memset(&driver, 0, sizeof(driver));
    memset(&key_management, 0, sizeof(key_management));
    memset(&asymmetric, 0, sizeof(asymmetric));
    driver.hal_version = PSA_DRV_SE_HAL_VERSION;
    driver.key_management = &key_management;
    driver.asymmetric = &asymmetric;
    driver.persistent_data_size = sizeof(ram_slot_usage_t);
    key_management.p_allocate = ram_allocate;
    key_management.p_destroy = ram_destroy;
    if (generating) {
        key_management.p_generate = ram_fake_generate;
    } else {
        key_management.p_import = ram_import;
    }
    switch (flow) {
        case SIGN_IN_SOFTWARE_AND_PARALLEL_CREATION:
            break;
        case SIGN_IN_DRIVER_AND_PARALLEL_CREATION:
            asymmetric.p_sign = ram_sign;
            break;
        case SIGN_IN_DRIVER_THEN_EXPORT_PUBLIC:
            asymmetric.p_sign = ram_sign;
            key_management.p_export_public = ram_export_public;
            break;
        default:
            TEST_ASSERT(!"unsupported flow (should be SIGN_IN_xxx)");
            break;
    }
    asymmetric.p_verify = ram_verify;

    PSA_ASSERT(psa_register_se_driver(location, &driver));
    PSA_ASSERT(psa_crypto_init());

    /* Prepare to create two keys with the same key material: a transparent
     * key, and one that goes through the driver. */
    psa_set_key_usage_flags(&sw_attributes,
                            PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&sw_attributes, alg);
    psa_set_key_type(&sw_attributes, type);
    drv_attributes = sw_attributes;
    psa_set_key_id(&drv_attributes, id);
    psa_set_key_lifetime(&drv_attributes, lifetime);

    /* Create the key in the driver. */
    if (generating) {
        psa_set_key_bits(&drv_attributes, bits);
        PSA_ASSERT(psa_generate_key(&drv_attributes, &returned_id));
        /* Since we called a generate method that does not actually
         * generate material, store the desired result of generation in
         * the mock secure element storage. */
        PSA_ASSERT(psa_get_key_attributes(id, &drv_attributes));
        TEST_EQUAL(key_material->len, PSA_BITS_TO_BYTES(bits));
        memcpy(ram_slots[ram_min_slot].content, key_material->x,
               key_material->len);
    } else {
        PSA_ASSERT(psa_import_key(&drv_attributes,
                                  key_material->x, key_material->len,
                                  &returned_id));
    }

    /* Either import the same key in software, or export the driver's
     * public key and import that. */
    switch (flow) {
        case SIGN_IN_SOFTWARE_AND_PARALLEL_CREATION:
        case SIGN_IN_DRIVER_AND_PARALLEL_CREATION:
            PSA_ASSERT(psa_import_key(&sw_attributes,
                                      key_material->x, key_material->len,
                                      &sw_key));
            break;
        case SIGN_IN_DRIVER_THEN_EXPORT_PUBLIC:
        {
            uint8_t public_key[PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(PSA_VENDOR_ECC_MAX_CURVE_BITS)
            ];
            size_t public_key_length;
            PSA_ASSERT(psa_export_public_key(id,
                                             public_key, sizeof(public_key),
                                             &public_key_length));
            psa_set_key_type(&sw_attributes,
                             PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type));
            PSA_ASSERT(psa_import_key(&sw_attributes,
                                      public_key, public_key_length,
                                      &sw_key));
            break;
        }
    }

    /* Sign with the chosen key. */
    switch (flow) {
        case SIGN_IN_DRIVER_AND_PARALLEL_CREATION:
        case SIGN_IN_DRIVER_THEN_EXPORT_PUBLIC:
            PSA_ASSERT_VIA_DRIVER(
                psa_sign_hash(id, alg,
                              input->x, input->len,
                              signature, sizeof(signature),
                              &signature_length),
                PSA_SUCCESS);
            break;
        case SIGN_IN_SOFTWARE_AND_PARALLEL_CREATION:
            PSA_ASSERT(psa_sign_hash(sw_key, alg,
                                     input->x, input->len,
                                     signature, sizeof(signature),
                                     &signature_length));
            break;
    }

    /* Verify with both keys. */
    PSA_ASSERT(psa_verify_hash(sw_key, alg,
                               input->x, input->len,
                               signature, signature_length));
    PSA_ASSERT_VIA_DRIVER(
        psa_verify_hash(id, alg,
                        input->x, input->len,
                        signature, signature_length),
        PSA_SUCCESS);

    /* Change the signature and verify again. */
    signature[0] ^= 1;
    TEST_EQUAL(psa_verify_hash(sw_key, alg,
                               input->x, input->len,
                               signature, signature_length),
               PSA_ERROR_INVALID_SIGNATURE);
    PSA_ASSERT_VIA_DRIVER(
        psa_verify_hash(id, alg,
                        input->x, input->len,
                        signature, signature_length),
        PSA_ERROR_INVALID_SIGNATURE);

exit:
    /*
     * Driver key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes(&drv_attributes);

    psa_destroy_key(id);
    psa_destroy_key(sw_key);
    PSA_DONE();
    ram_slots_reset();
    psa_purge_storage();
}

static void test_sign_verify_wrapper( void ** params )
{
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_sign_verify( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), &data4, &data6 );
}
#line 1449 "suites/test_suite_psa_crypto_se_driver_hal.function"
static void test_register_key_smoke_test(int lifetime_arg,
                             int owner_id_arg,
                             int id_arg,
                             int validate,
                             int expected_status_arg)
{
    psa_key_lifetime_t lifetime = lifetime_arg;
    psa_key_location_t location = PSA_KEY_LIFETIME_GET_LOCATION(lifetime);
    psa_status_t expected_status = expected_status_arg;
    psa_drv_se_t driver;
    psa_drv_se_key_management_t key_management;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    mbedtls_svc_key_id_t id = mbedtls_svc_key_id_make(owner_id_arg, id_arg);
    psa_key_handle_t handle;
    size_t bit_size = 48;
    psa_key_slot_number_t wanted_slot = 0x123456789;
    psa_status_t status;

    TEST_USES_KEY_ID(id);

    memset(&driver, 0, sizeof(driver));
    driver.hal_version = PSA_DRV_SE_HAL_VERSION;
    memset(&key_management, 0, sizeof(key_management));
    driver.key_management = &key_management;
    key_management.p_destroy = null_destroy;
    if (validate >= 0) {
        key_management.p_validate_slot_number = validate_slot_number_as_directed;
        validate_slot_number_directions.slot_number = wanted_slot;
        validate_slot_number_directions.method = PSA_KEY_CREATION_REGISTER;
        validate_slot_number_directions.status =
            (validate > 0 ? PSA_SUCCESS : PSA_ERROR_NOT_PERMITTED);
    }

    mbedtls_test_set_step(1);
    PSA_ASSERT(psa_register_se_driver(MIN_DRIVER_LOCATION, &driver));
    PSA_ASSERT(psa_crypto_init());

    psa_set_key_id(&attributes, id);
    psa_set_key_lifetime(&attributes, lifetime);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_EXPORT);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_RAW_DATA);
    psa_set_key_bits(&attributes, bit_size);
    psa_set_key_slot_number(&attributes, wanted_slot);

    status = mbedtls_psa_register_se_key(&attributes);
    TEST_EQUAL(status, expected_status);

    if (status != PSA_SUCCESS) {
        goto exit;
    }

    /* Test that the key exists and has the expected attributes. */
    if (!check_key_attributes(id, &attributes)) {
        goto exit;
    }

#if defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)
    mbedtls_svc_key_id_t invalid_id =
        mbedtls_svc_key_id_make(owner_id_arg + 1, id_arg);
    TEST_EQUAL(psa_open_key(invalid_id, &handle), PSA_ERROR_DOES_NOT_EXIST);
#endif

    PSA_ASSERT(psa_purge_key(id));

    /* Restart and try again. */
    mbedtls_test_set_step(2);
    PSA_SESSION_DONE();
    PSA_ASSERT(psa_register_se_driver(location, &driver));
    PSA_ASSERT(psa_crypto_init());
    if (!check_key_attributes(id, &attributes)) {
        goto exit;
    }
    /* This time, destroy the key. */
    PSA_ASSERT(psa_destroy_key(id));
    TEST_EQUAL(psa_open_key(id, &handle), PSA_ERROR_DOES_NOT_EXIST);

exit:
    psa_reset_key_attributes(&attributes);
    psa_destroy_key(id);
    PSA_DONE();
    psa_purge_storage();
    memset(&validate_slot_number_directions, 0,
           sizeof(validate_slot_number_directions));
}

static void test_register_key_smoke_test_wrapper( void ** params )
{

    test_register_key_smoke_test( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ) );
}
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */


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
    
#if defined(MBEDTLS_PSA_CRYPTO_SE_C)

        case 0:
            {
                *out_value = PSA_DRV_SE_HAL_VERSION;
            }
            break;
        case 1:
            {
                *out_value = PSA_SUCCESS;
            }
            break;
        case 2:
            {
                *out_value = PSA_ERROR_INVALID_ARGUMENT;
            }
            break;
        case 3:
            {
                *out_value = PSA_KEY_LOCATION_LOCAL_STORAGE;
            }
            break;
        case 4:
            {
                *out_value = PSA_ERROR_NOT_SUPPORTED;
            }
            break;
        case 5:
            {
                *out_value = PSA_DRV_SE_HAL_VERSION + 1;
            }
            break;
        case 6:
            {
                *out_value = TEST_SE_PERSISTENT_LIFETIME;
            }
            break;
        case 7:
            {
                *out_value = ARRAY_LENGTH( ram_slots ) - 1;
            }
            break;
        case 8:
            {
                *out_value = TEST_SE_VOLATILE_LIFETIME;
            }
            break;
        case 9:
            {
                *out_value = ARRAY_LENGTH( ram_slots );
            }
            break;
        case 10:
            {
                *out_value = PSA_KEY_TYPE_AES;
            }
            break;
        case 11:
            {
                *out_value = PSA_ALG_CTR;
            }
            break;
        case 12:
            {
                *out_value = PSA_ALG_CBC_NO_PADDING;
            }
            break;
        case 13:
            {
                *out_value = PSA_ALG_CMAC;
            }
            break;
        case 14:
            {
                *out_value = PSA_ALG_CCM;
            }
            break;
        case 15:
            {
                *out_value = PSA_ALG_GCM;
            }
            break;
        case 16:
            {
                *out_value = PSA_KEY_TYPE_ARIA;
            }
            break;
        case 17:
            {
                *out_value = PSA_KEY_TYPE_CAMELLIA;
            }
            break;
        case 18:
            {
                *out_value = PSA_KEY_TYPE_HMAC;
            }
            break;
        case 19:
            {
                *out_value = PSA_ALG_HMAC( PSA_ALG_SHA_256 );
            }
            break;
        case 20:
            {
                *out_value = PSA_KEY_TYPE_DERIVE;
            }
            break;
        case 21:
            {
                *out_value = PSA_ALG_HKDF( PSA_ALG_SHA_256 );
            }
            break;
        case 22:
            {
                *out_value = PSA_KEY_TYPE_RSA_KEY_PAIR;
            }
            break;
        case 23:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_SIGN_RAW;
            }
            break;
        case 24:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_CRYPT;
            }
            break;
        case 25:
            {
                *out_value = PSA_ALG_RSA_OAEP( PSA_ALG_SHA_256 );
            }
            break;
        case 26:
            {
                *out_value = PSA_KEY_TYPE_ECC_KEY_PAIR( PSA_ECC_FAMILY_SECP_R1 );
            }
            break;
        case 27:
            {
                *out_value = PSA_ALG_ECDSA_ANY;
            }
            break;
        case 28:
            {
                *out_value = PSA_ALG_ECDH;
            }
            break;
        case 29:
            {
                *out_value = PSA_ALG_KEY_AGREEMENT( PSA_ALG_ECDH, PSA_ALG_HKDF( PSA_ALG_SHA_256 ) );
            }
            break;
        case 30:
            {
                *out_value = PSA_KEY_LIFETIME_VOLATILE;
            }
            break;
        case 31:
            {
                *out_value = PSA_KEY_LIFETIME_PERSISTENT;
            }
            break;
        case 32:
            {
                *out_value = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION( PSA_KEY_PERSISTENCE_DEFAULT, TEST_DRIVER_LOCATION + 1 );
            }
            break;
        case 33:
            {
                *out_value = PSA_ERROR_NOT_PERMITTED;
            }
            break;
        case 34:
            {
                *out_value = -1;
            }
            break;
        case 35:
            {
                *out_value = PSA_KEY_ID_VENDOR_MAX+1;
            }
            break;
        case 36:
            {
                *out_value = PSA_KEY_ID_VENDOR_MIN;
            }
            break;
        case 37:
            {
                *out_value = PSA_KEY_ID_VOLATILE_MIN-1;
            }
            break;
        case 38:
            {
                *out_value = PSA_KEY_ID_VOLATILE_MIN;
            }
            break;
        case 39:
            {
                *out_value = PSA_KEY_ID_VOLATILE_MAX;
            }
            break;
        case 40:
            {
                *out_value = SIGN_IN_DRIVER_AND_PARALLEL_CREATION;
            }
            break;
        case 41:
            {
                *out_value = SIGN_IN_DRIVER_THEN_EXPORT_PUBLIC;
            }
            break;
        case 42:
            {
                *out_value = SIGN_IN_SOFTWARE_AND_PARALLEL_CREATION;
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
    
#if defined(MBEDTLS_PSA_CRYPTO_SE_C)

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
#if defined(PSA_WANT_ECC_SECP_R1_256)
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

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    test_register_one_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    test_register_twice_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    test_register_max_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    test_key_creation_import_export_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    test_key_creation_in_chosen_slot_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_PSA_CRYPTO_SE_C) && defined(AT_LEAST_ONE_BUILTIN_KDF)
    test_import_key_smoke_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    test_generate_key_not_supported_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_PSA_CRYPTO_SE_C) && defined(AT_LEAST_ONE_BUILTIN_KDF)
    test_generate_key_smoke_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    test_sign_verify_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    test_register_key_smoke_test_wrapper,
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
    const char *default_filename = "./test_suite_psa_crypto_se_driver_hal.datax";
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
