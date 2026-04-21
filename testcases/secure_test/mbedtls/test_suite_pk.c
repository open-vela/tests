#line 2 "suites/main_test.function"
/*
 * *** THIS FILE HAS BEEN MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py
 *
 * Test file      : ./test_suite_pk.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_pk.function
 *      Test suite data     : suites/test_suite_pk.data
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

#if defined(MBEDTLS_PK_C)
#line 2 "suites/test_suite_pk.function"
#include "mbedtls/pk.h"

/* For error codes */
#include "mbedtls/asn1.h"
#include "mbedtls/base64.h"
#include "mbedtls/ecp.h"
#include "mbedtls/rsa.h"

#include "hash_info.h"
#include "mbedtls/legacy_or_psa.h"

#include <limits.h>
#include <stdint.h>

/* Needed only for test case data under #if defined(MBEDTLS_USE_PSA_CRYPTO),
 * but the test code generator requires test case data to be valid C code
 * unconditionally (https://github.com/Mbed-TLS/mbedtls/issues/2023). */
#include "psa/crypto.h"

#define RSA_KEY_SIZE 512
#define RSA_KEY_LEN   64

/** Generate a key of the desired type.
 *
 * \param pk        The PK object to fill. It must have been initialized
 *                  with mbedtls_pk_setup().
 * \param parameter - For RSA keys, the key size in bits.
 *                  - For EC keys, the curve (\c MBEDTLS_ECP_DP_xxx).
 *
 * \return          The status from the underlying type-specific key
 *                  generation function.
 * \return          -1 if the key type is not recognized.
 */
static int pk_genkey(mbedtls_pk_context *pk, int parameter)
{
    ((void) pk);
    (void) parameter;

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_GENPRIME)
    if (mbedtls_pk_get_type(pk) == MBEDTLS_PK_RSA) {
        return mbedtls_rsa_gen_key(mbedtls_pk_rsa(*pk),
                                   mbedtls_test_rnd_std_rand, NULL,
                                   parameter, 3);
    }
#endif
#if defined(MBEDTLS_ECP_C)
    if (mbedtls_pk_get_type(pk) == MBEDTLS_PK_ECKEY ||
        mbedtls_pk_get_type(pk) == MBEDTLS_PK_ECKEY_DH ||
        mbedtls_pk_get_type(pk) == MBEDTLS_PK_ECDSA) {
        int ret;
        if ((ret = mbedtls_ecp_group_load(&mbedtls_pk_ec(*pk)->grp,
                                          parameter)) != 0) {
            return ret;
        }

        return mbedtls_ecp_gen_keypair(&mbedtls_pk_ec(*pk)->grp,
                                       &mbedtls_pk_ec(*pk)->d,
                                       &mbedtls_pk_ec(*pk)->Q,
                                       mbedtls_test_rnd_std_rand, NULL);
    }
#endif
    return -1;
}

#if defined(MBEDTLS_RSA_C)
static int mbedtls_rsa_decrypt_func(void *ctx, size_t *olen,
                             const unsigned char *input, unsigned char *output,
                             size_t output_max_len)
{
    return mbedtls_rsa_pkcs1_decrypt((mbedtls_rsa_context *) ctx,
                                     mbedtls_test_rnd_std_rand, NULL,
                                     olen, input, output, output_max_len);
}
static int mbedtls_rsa_sign_func(void *ctx,
                          int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
                          mbedtls_md_type_t md_alg, unsigned int hashlen,
                          const unsigned char *hash, unsigned char *sig)
{
    ((void) f_rng);
    ((void) p_rng);
    return mbedtls_rsa_pkcs1_sign((mbedtls_rsa_context *) ctx,
                                  mbedtls_test_rnd_std_rand, NULL,
                                  md_alg, hashlen, hash, sig);
}
static size_t mbedtls_rsa_key_len_func(void *ctx)
{
    return ((const mbedtls_rsa_context *) ctx)->len;
}
#endif /* MBEDTLS_RSA_C */

#if defined(MBEDTLS_USE_PSA_CRYPTO)

/*
 * Generate an ECC key using PSA and return the key identifier of that key,
 * or 0 if the key generation failed.
 * The key uses NIST P-256 and is usable for signing with SHA-256.
 */
mbedtls_svc_key_id_t pk_psa_genkey_ecc(void)
{
    mbedtls_svc_key_id_t key;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    const psa_key_type_t type =
        PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1);
    const size_t bits = 256;

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    psa_set_key_type(&attributes, type);
    psa_set_key_bits(&attributes, bits);
    PSA_ASSERT(psa_generate_key(&attributes, &key));

exit:
    return key;
}

/*
 * Generate an RSA key using PSA and return the key identifier of that key,
 * or 0 if the key generation failed.
 */
mbedtls_svc_key_id_t pk_psa_genkey_rsa(void)
{
    mbedtls_svc_key_id_t key;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    const psa_key_type_t type = PSA_KEY_TYPE_RSA_KEY_PAIR;
    const size_t bits = 1024;

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&attributes, PSA_ALG_RSA_PKCS1V15_SIGN_RAW);
    psa_set_key_type(&attributes, type);
    psa_set_key_bits(&attributes, bits);
    PSA_ASSERT(psa_generate_key(&attributes, &key));

exit:
    return key;
}
#endif /* MBEDTLS_USE_PSA_CRYPTO */
#if defined(MBEDTLS_USE_PSA_CRYPTO)
#line 146 "suites/test_suite_pk.function"
static void test_pk_psa_utils(int key_is_rsa)
{
    mbedtls_pk_context pk, pk2;
    mbedtls_svc_key_id_t key;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    const char * const name = "Opaque";
    size_t bitlen;

    mbedtls_md_type_t md_alg = MBEDTLS_MD_NONE;
    unsigned char b1[1], b2[1];
    size_t len;
    mbedtls_pk_debug_item dbg;

    PSA_ASSERT(psa_crypto_init());

    mbedtls_pk_init(&pk);
    mbedtls_pk_init(&pk2);

    TEST_ASSERT(psa_crypto_init() == PSA_SUCCESS);

    TEST_ASSERT(mbedtls_pk_setup_opaque(&pk, MBEDTLS_SVC_KEY_ID_INIT) ==
                MBEDTLS_ERR_PK_BAD_INPUT_DATA);

    mbedtls_pk_free(&pk);
    mbedtls_pk_init(&pk);

    if (key_is_rsa) {
        bitlen = 1024; /* hardcoded in genkey() */
        key = pk_psa_genkey_rsa();
    } else {
        bitlen = 256; /* hardcoded in genkey() */
        key = pk_psa_genkey_ecc();
    }
    if (mbedtls_svc_key_id_is_null(key)) {
        goto exit;
    }

    TEST_ASSERT(mbedtls_pk_setup_opaque(&pk, key) == 0);

    TEST_ASSERT(mbedtls_pk_get_type(&pk) == MBEDTLS_PK_OPAQUE);
    TEST_ASSERT(strcmp(mbedtls_pk_get_name(&pk), name) == 0);

    TEST_ASSERT(mbedtls_pk_get_bitlen(&pk) == bitlen);
    TEST_ASSERT(mbedtls_pk_get_len(&pk) == bitlen / 8);

    if (key_is_rsa) {
        TEST_ASSERT(mbedtls_pk_can_do(&pk, MBEDTLS_PK_ECKEY) == 0);
        TEST_ASSERT(mbedtls_pk_can_do(&pk, MBEDTLS_PK_ECDSA) == 0);
        TEST_ASSERT(mbedtls_pk_can_do(&pk, MBEDTLS_PK_RSA) == 1);
    } else {
        TEST_ASSERT(mbedtls_pk_can_do(&pk, MBEDTLS_PK_ECKEY) == 1);
        TEST_ASSERT(mbedtls_pk_can_do(&pk, MBEDTLS_PK_ECDSA) == 1);
        TEST_ASSERT(mbedtls_pk_can_do(&pk, MBEDTLS_PK_RSA) == 0);
    }

    /* unsupported operations: verify, decrypt, encrypt */
    TEST_ASSERT(mbedtls_pk_verify(&pk, md_alg,
                                  b1, sizeof(b1), b2, sizeof(b2))
                == MBEDTLS_ERR_PK_TYPE_MISMATCH);
    if (key_is_rsa == 0) {
        TEST_ASSERT(mbedtls_pk_decrypt(&pk, b1, sizeof(b1),
                                       b2, &len, sizeof(b2),
                                       NULL, NULL)
                    == MBEDTLS_ERR_PK_TYPE_MISMATCH);
    }
    TEST_ASSERT(mbedtls_pk_encrypt(&pk, b1, sizeof(b1),
                                   b2, &len, sizeof(b2),
                                   NULL, NULL)
                == MBEDTLS_ERR_PK_TYPE_MISMATCH);

    /* unsupported functions: check_pair, debug */
    if (key_is_rsa) {
        TEST_ASSERT(mbedtls_pk_setup(&pk2,
                                     mbedtls_pk_info_from_type(MBEDTLS_PK_RSA)) == 0);
    } else {
        TEST_ASSERT(mbedtls_pk_setup(&pk2,
                                     mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY)) == 0);
    }
    TEST_ASSERT(mbedtls_pk_check_pair(&pk, &pk2,
                                      mbedtls_test_rnd_std_rand, NULL)
                == MBEDTLS_ERR_PK_TYPE_MISMATCH);
    TEST_ASSERT(mbedtls_pk_debug(&pk, &dbg)
                == MBEDTLS_ERR_PK_TYPE_MISMATCH);

    /* test that freeing the context does not destroy the key */
    mbedtls_pk_free(&pk);
    TEST_ASSERT(PSA_SUCCESS == psa_get_key_attributes(key, &attributes));
    TEST_ASSERT(PSA_SUCCESS == psa_destroy_key(key));

exit:
    /*
     * Key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes(&attributes);

    mbedtls_pk_free(&pk);   /* redundant except upon error */
    mbedtls_pk_free(&pk2);
    USE_PSA_DONE();
}

static void test_pk_psa_utils_wrapper( void ** params )
{

    test_pk_psa_utils( *( (int *) params[0] ) );
}
#endif /* MBEDTLS_USE_PSA_CRYPTO */
#if defined(MBEDTLS_USE_PSA_CRYPTO)
#line 250 "suites/test_suite_pk.function"
static void test_pk_can_do_ext(int opaque_key, int key_type, int key_usage, int key_alg,
                   int key_alg2, int parameter, int alg_check, int usage_check,
                   int result)
{
    mbedtls_pk_context pk;
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    PSA_ASSERT(psa_crypto_init());

    mbedtls_pk_init(&pk);

    if (opaque_key == 1) {
        psa_set_key_usage_flags(&attributes, key_usage);
        psa_set_key_algorithm(&attributes, key_alg);
        if (key_alg2 != 0) {
            psa_set_key_enrollment_algorithm(&attributes, key_alg2);
        }
        psa_set_key_type(&attributes, key_type);
        psa_set_key_bits(&attributes, parameter);

        PSA_ASSERT(psa_generate_key(&attributes, &key));

        if (mbedtls_svc_key_id_is_null(key)) {
            goto exit;
        }

        TEST_EQUAL(mbedtls_pk_setup_opaque(&pk, key), 0);

        TEST_EQUAL(mbedtls_pk_get_type(&pk), MBEDTLS_PK_OPAQUE);
    } else {
        TEST_EQUAL(mbedtls_pk_setup(&pk,
                                    mbedtls_pk_info_from_type(key_type)), 0);
        TEST_EQUAL(pk_genkey(&pk, parameter), 0);
        TEST_EQUAL(mbedtls_pk_get_type(&pk), key_type);
    }

    TEST_EQUAL(mbedtls_pk_can_do_ext(&pk, alg_check, usage_check), result);

exit:
    psa_reset_key_attributes(&attributes);
    PSA_ASSERT(psa_destroy_key(key));
    mbedtls_pk_free(&pk);
    USE_PSA_DONE();
}

static void test_pk_can_do_ext_wrapper( void ** params )
{

    test_pk_can_do_ext( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), *( (int *) params[6] ), *( (int *) params[7] ), *( (int *) params[8] ) );
}
#endif /* MBEDTLS_USE_PSA_CRYPTO */
#line 298 "suites/test_suite_pk.function"
static void test_pk_invalid_param(void)
{
    mbedtls_pk_context ctx;
    mbedtls_pk_type_t pk_type = 0;
    unsigned char buf[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
    size_t buf_size = sizeof(buf);

    mbedtls_pk_init(&ctx);

    TEST_EQUAL(MBEDTLS_ERR_PK_BAD_INPUT_DATA,
               mbedtls_pk_verify_restartable(&ctx, MBEDTLS_MD_NONE,
                                             NULL, buf_size,
                                             buf, buf_size,
                                             NULL));
    TEST_EQUAL(MBEDTLS_ERR_PK_BAD_INPUT_DATA,
               mbedtls_pk_verify_restartable(&ctx, MBEDTLS_MD_SHA256,
                                             NULL, 0,
                                             buf, buf_size,
                                             NULL));
    TEST_EQUAL(MBEDTLS_ERR_PK_BAD_INPUT_DATA,
               mbedtls_pk_verify_ext(pk_type, NULL,
                                     &ctx, MBEDTLS_MD_NONE,
                                     NULL, buf_size,
                                     buf, buf_size));
    TEST_EQUAL(MBEDTLS_ERR_PK_BAD_INPUT_DATA,
               mbedtls_pk_verify_ext(pk_type, NULL,
                                     &ctx, MBEDTLS_MD_SHA256,
                                     NULL, 0,
                                     buf, buf_size));
    TEST_EQUAL(MBEDTLS_ERR_PK_BAD_INPUT_DATA,
               mbedtls_pk_sign_restartable(&ctx, MBEDTLS_MD_NONE,
                                           NULL, buf_size,
                                           buf, buf_size, &buf_size,
                                           NULL, NULL,
                                           NULL));
    TEST_EQUAL(MBEDTLS_ERR_PK_BAD_INPUT_DATA,
               mbedtls_pk_sign_restartable(&ctx, MBEDTLS_MD_SHA256,
                                           NULL, 0,
                                           buf, buf_size, &buf_size,
                                           NULL, NULL,
                                           NULL));
exit:
    mbedtls_pk_free(&ctx);
}

static void test_pk_invalid_param_wrapper( void ** params )
{
    (void)params;

    test_pk_invalid_param(  );
}
#line 345 "suites/test_suite_pk.function"
static void test_valid_parameters(void)
{
    mbedtls_pk_context pk;
    unsigned char buf[1];
    size_t len;
    void *options = NULL;

    mbedtls_pk_init(&pk);

    TEST_ASSERT(mbedtls_pk_setup(&pk, NULL) ==
                MBEDTLS_ERR_PK_BAD_INPUT_DATA);

    /* In informational functions, we accept NULL where a context pointer
     * is expected because that's what the library has done forever.
     * We do not document that NULL is accepted, so we may wish to change
     * the behavior in a future version. */
    TEST_ASSERT(mbedtls_pk_get_bitlen(NULL) == 0);
    TEST_ASSERT(mbedtls_pk_get_len(NULL) == 0);
    TEST_ASSERT(mbedtls_pk_can_do(NULL, MBEDTLS_PK_NONE) == 0);

    TEST_ASSERT(mbedtls_pk_sign_restartable(&pk,
                                            MBEDTLS_MD_NONE,
                                            NULL, 0,
                                            buf, sizeof(buf), &len,
                                            mbedtls_test_rnd_std_rand, NULL,
                                            NULL) ==
                MBEDTLS_ERR_PK_BAD_INPUT_DATA);

    TEST_ASSERT(mbedtls_pk_sign_restartable(&pk,
                                            MBEDTLS_MD_NONE,
                                            NULL, 0,
                                            buf, sizeof(buf), &len,
                                            mbedtls_test_rnd_std_rand, NULL,
                                            NULL) ==
                MBEDTLS_ERR_PK_BAD_INPUT_DATA);

    TEST_ASSERT(mbedtls_pk_sign(&pk,
                                MBEDTLS_MD_NONE,
                                NULL, 0,
                                buf, sizeof(buf), &len,
                                mbedtls_test_rnd_std_rand, NULL) ==
                MBEDTLS_ERR_PK_BAD_INPUT_DATA);

    TEST_ASSERT(mbedtls_pk_verify_restartable(&pk,
                                              MBEDTLS_MD_NONE,
                                              NULL, 0,
                                              buf, sizeof(buf),
                                              NULL) ==
                MBEDTLS_ERR_PK_BAD_INPUT_DATA);

    TEST_ASSERT(mbedtls_pk_verify(&pk,
                                  MBEDTLS_MD_NONE,
                                  NULL, 0,
                                  buf, sizeof(buf)) ==
                MBEDTLS_ERR_PK_BAD_INPUT_DATA);

    TEST_ASSERT(mbedtls_pk_verify_ext(MBEDTLS_PK_NONE, options,
                                      &pk,
                                      MBEDTLS_MD_NONE,
                                      NULL, 0,
                                      buf, sizeof(buf)) ==
                MBEDTLS_ERR_PK_BAD_INPUT_DATA);

    TEST_ASSERT(mbedtls_pk_encrypt(&pk,
                                   NULL, 0,
                                   NULL, &len, 0,
                                   mbedtls_test_rnd_std_rand, NULL) ==
                MBEDTLS_ERR_PK_BAD_INPUT_DATA);

    TEST_ASSERT(mbedtls_pk_decrypt(&pk,
                                   NULL, 0,
                                   NULL, &len, 0,
                                   mbedtls_test_rnd_std_rand, NULL) ==
                MBEDTLS_ERR_PK_BAD_INPUT_DATA);

#if defined(MBEDTLS_PK_PARSE_C)
    TEST_ASSERT(mbedtls_pk_parse_key(&pk, NULL, 0, NULL, 1,
                                     mbedtls_test_rnd_std_rand, NULL) ==
                MBEDTLS_ERR_PK_KEY_INVALID_FORMAT);

    TEST_ASSERT(mbedtls_pk_parse_public_key(&pk, NULL, 0) ==
                MBEDTLS_ERR_PK_KEY_INVALID_FORMAT);
#endif /* MBEDTLS_PK_PARSE_C */
exit:
    ;
}

static void test_valid_parameters_wrapper( void ** params )
{
    (void)params;

    test_valid_parameters(  );
}
#if defined(MBEDTLS_PK_WRITE_C)
#line 432 "suites/test_suite_pk.function"
static void test_valid_parameters_pkwrite(data_t *key_data)
{
    mbedtls_pk_context pk;

    /* For the write tests to be effective, we need a valid key pair. */
    mbedtls_pk_init(&pk);
    TEST_ASSERT(mbedtls_pk_parse_key(&pk,
                                     key_data->x, key_data->len, NULL, 0,
                                     mbedtls_test_rnd_std_rand, NULL) == 0);

    TEST_ASSERT(mbedtls_pk_write_key_der(&pk, NULL, 0) ==
                MBEDTLS_ERR_ASN1_BUF_TOO_SMALL);

    TEST_ASSERT(mbedtls_pk_write_pubkey_der(&pk, NULL, 0) ==
                MBEDTLS_ERR_ASN1_BUF_TOO_SMALL);

#if defined(MBEDTLS_PEM_WRITE_C)
    TEST_ASSERT(mbedtls_pk_write_key_pem(&pk, NULL, 0) ==
                MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL);

    TEST_ASSERT(mbedtls_pk_write_pubkey_pem(&pk, NULL, 0) ==
                MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL);
#endif /* MBEDTLS_PEM_WRITE_C */

exit:
    mbedtls_pk_free(&pk);
}

static void test_valid_parameters_pkwrite_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_valid_parameters_pkwrite( &data0 );
}
#endif /* MBEDTLS_PK_WRITE_C */
#line 462 "suites/test_suite_pk.function"
static void test_pk_utils(int type, int parameter, int bitlen, int len, char *name)
{
    mbedtls_pk_context pk;

    mbedtls_pk_init(&pk);

    TEST_ASSERT(mbedtls_pk_setup(&pk, mbedtls_pk_info_from_type(type)) == 0);
    TEST_ASSERT(pk_genkey(&pk, parameter) == 0);

    TEST_ASSERT((int) mbedtls_pk_get_type(&pk) == type);
    TEST_ASSERT(mbedtls_pk_can_do(&pk, type));
    TEST_ASSERT(mbedtls_pk_get_bitlen(&pk) == (unsigned) bitlen);
    TEST_ASSERT(mbedtls_pk_get_len(&pk) == (unsigned) len);
    TEST_ASSERT(strcmp(mbedtls_pk_get_name(&pk), name) == 0);

exit:
    mbedtls_pk_free(&pk);
}

static void test_pk_utils_wrapper( void ** params )
{

    test_pk_utils( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4] );
}
#if defined(MBEDTLS_PK_PARSE_C)
#if defined(MBEDTLS_FS_IO)
#line 483 "suites/test_suite_pk.function"
static void test_mbedtls_pk_check_pair(char *pub_file, char *prv_file, int ret)
{
    mbedtls_pk_context pub, prv, alt;

    USE_PSA_INIT();

    mbedtls_pk_init(&pub);
    mbedtls_pk_init(&prv);
    mbedtls_pk_init(&alt);

    TEST_ASSERT(mbedtls_pk_parse_public_keyfile(&pub, pub_file) == 0);
    TEST_ASSERT(mbedtls_pk_parse_keyfile(&prv, prv_file, NULL,
                                         mbedtls_test_rnd_std_rand, NULL)
                == 0);

    TEST_ASSERT(mbedtls_pk_check_pair(&pub, &prv,
                                      mbedtls_test_rnd_std_rand, NULL)
                == ret);

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_PK_RSA_ALT_SUPPORT)
    if (mbedtls_pk_get_type(&prv) == MBEDTLS_PK_RSA) {
        TEST_ASSERT(mbedtls_pk_setup_rsa_alt(&alt, mbedtls_pk_rsa(prv),
                                             mbedtls_rsa_decrypt_func, mbedtls_rsa_sign_func,
                                             mbedtls_rsa_key_len_func) == 0);
        TEST_ASSERT(mbedtls_pk_check_pair(&pub, &alt,
                                          mbedtls_test_rnd_std_rand, NULL)
                    == ret);
    }
#endif

    mbedtls_pk_free(&pub);
    mbedtls_pk_free(&prv);
    mbedtls_pk_free(&alt);
    USE_PSA_DONE();
exit:
    ;
}

static void test_mbedtls_pk_check_pair_wrapper( void ** params )
{

    test_mbedtls_pk_check_pair( (char *) params[0], (char *) params[1], *( (int *) params[2] ) );
}
#endif /* MBEDTLS_FS_IO */
#endif /* MBEDTLS_PK_PARSE_C */
#if defined(MBEDTLS_RSA_C)
#line 521 "suites/test_suite_pk.function"
static void test_pk_rsa_verify_test_vec(data_t *message_str, int digest, int mod,
                            char *input_N, char *input_E,
                            data_t *result_str, int result)
{
    mbedtls_rsa_context *rsa;
    mbedtls_pk_context pk;
    mbedtls_pk_restart_ctx *rs_ctx = NULL;
#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECP_RESTARTABLE)
    mbedtls_pk_restart_ctx ctx;

    rs_ctx = &ctx;
    mbedtls_pk_restart_init(rs_ctx);
    // this setting would ensure restart would happen if ECC was used
    mbedtls_ecp_set_max_ops(1);
#endif

    USE_PSA_INIT();

    mbedtls_pk_init(&pk);


    TEST_ASSERT(mbedtls_pk_setup(&pk, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA)) == 0);
    rsa = mbedtls_pk_rsa(pk);

    rsa->len = mod / 8;
    TEST_ASSERT(mbedtls_test_read_mpi(&rsa->N, input_N) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&rsa->E, input_E) == 0);

    TEST_ASSERT(mbedtls_pk_verify(&pk, digest, message_str->x, 0,
                                  result_str->x, mbedtls_pk_get_len(&pk)) == result);

    TEST_ASSERT(mbedtls_pk_verify_restartable(&pk, digest, message_str->x, 0,
                                              result_str->x, mbedtls_pk_get_len(
                                                  &pk), rs_ctx) == result);

exit:
#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECP_RESTARTABLE)
    mbedtls_pk_restart_free(rs_ctx);
#endif
    mbedtls_pk_free(&pk);
    USE_PSA_DONE();
}

static void test_pk_rsa_verify_test_vec_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_pk_rsa_verify_test_vec( &data0, *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4], (char *) params[5], &data6, *( (int *) params[8] ) );
}
#endif /* MBEDTLS_RSA_C */
#if defined(MBEDTLS_RSA_C)
#line 566 "suites/test_suite_pk.function"
static void test_pk_rsa_verify_ext_test_vec(data_t *message_str, int digest,
                                int mod, char *input_N,
                                char *input_E, data_t *result_str,
                                int pk_type, int mgf1_hash_id,
                                int salt_len, int sig_len,
                                int result)
{
    mbedtls_rsa_context *rsa;
    mbedtls_pk_context pk;
    mbedtls_pk_rsassa_pss_options pss_opts;
    void *options;
    int ret;

    USE_PSA_INIT();
    mbedtls_pk_init(&pk);

    TEST_ASSERT(mbedtls_pk_setup(&pk, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA)) == 0);
    rsa = mbedtls_pk_rsa(pk);

    rsa->len = mod / 8;
    TEST_ASSERT(mbedtls_test_read_mpi(&rsa->N, input_N) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&rsa->E, input_E) == 0);


    if (mgf1_hash_id < 0) {
        options = NULL;
    } else {
        options = &pss_opts;

        pss_opts.mgf1_hash_id = mgf1_hash_id;
        pss_opts.expected_salt_len = salt_len;
    }

    ret = mbedtls_pk_verify_ext(pk_type, options, &pk,
                                digest, message_str->x, message_str->len,
                                result_str->x, sig_len);

#if defined(MBEDTLS_USE_PSA_CRYPTO)
    if (result == MBEDTLS_ERR_RSA_INVALID_PADDING) {
        /* Mbed TLS distinguishes "invalid padding" from "valid padding but
         * the rest of the signature is invalid". This has little use in
         * practice and PSA doesn't report this distinction.
         * In this case, PSA returns PSA_ERROR_INVALID_SIGNATURE translated
         * to MBEDTLS_ERR_RSA_VERIFY_FAILED.
         * However, currently `mbedtls_pk_verify_ext()` may use either the
         * PSA or the Mbed TLS API, depending on the PSS options used.
         * So, it may return either INVALID_PADDING or INVALID_SIGNATURE.
         */
        TEST_ASSERT(ret == result || ret == MBEDTLS_ERR_RSA_VERIFY_FAILED);
    } else
#endif
    {
        TEST_EQUAL(ret, result);
    }

exit:
    mbedtls_pk_free(&pk);
    USE_PSA_DONE();
}

static void test_pk_rsa_verify_ext_test_vec_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_pk_rsa_verify_ext_test_vec( &data0, *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4], (char *) params[5], &data6, *( (int *) params[8] ), *( (int *) params[9] ), *( (int *) params[10] ), *( (int *) params[11] ), *( (int *) params[12] ) );
}
#endif /* MBEDTLS_RSA_C */
#if defined(MBEDTLS_PK_CAN_ECDSA_VERIFY)
#line 628 "suites/test_suite_pk.function"
static void test_pk_ec_test_vec(int type, int id, data_t *key, data_t *hash,
                    data_t *sig, int ret)
{
    mbedtls_pk_context pk;
    mbedtls_ecp_keypair *eckey;

    mbedtls_pk_init(&pk);
    USE_PSA_INIT();

    TEST_ASSERT(mbedtls_pk_setup(&pk, mbedtls_pk_info_from_type(type)) == 0);

    TEST_ASSERT(mbedtls_pk_can_do(&pk, MBEDTLS_PK_ECDSA));
    eckey = mbedtls_pk_ec(pk);

    TEST_ASSERT(mbedtls_ecp_group_load(&eckey->grp, id) == 0);
    TEST_ASSERT(mbedtls_ecp_point_read_binary(&eckey->grp, &eckey->Q,
                                              key->x, key->len) == 0);

    // MBEDTLS_MD_NONE is used since it will be ignored.
    TEST_ASSERT(mbedtls_pk_verify(&pk, MBEDTLS_MD_NONE,
                                  hash->x, hash->len, sig->x, sig->len) == ret);

exit:
    mbedtls_pk_free(&pk);
    USE_PSA_DONE();
}

static void test_pk_ec_test_vec_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_pk_ec_test_vec( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6, *( (int *) params[8] ) );
}
#endif /* MBEDTLS_PK_CAN_ECDSA_VERIFY */
#if defined(MBEDTLS_ECP_RESTARTABLE)
#if defined(MBEDTLS_ECDSA_C)
#if defined(MBEDTLS_ECDSA_DETERMINISTIC)
#line 657 "suites/test_suite_pk.function"
static void test_pk_sign_verify_restart(int pk_type, int grp_id, char *d_str,
                            char *QX_str, char *QY_str,
                            int md_alg, data_t *hash, data_t *sig_check,
                            int max_ops, int min_restart, int max_restart)
{
    int ret, cnt_restart;
    mbedtls_pk_restart_ctx rs_ctx;
    mbedtls_pk_context prv, pub;
    unsigned char sig[MBEDTLS_ECDSA_MAX_LEN];
    size_t slen;

    USE_PSA_INIT();

    mbedtls_pk_restart_init(&rs_ctx);
    mbedtls_pk_init(&prv);
    mbedtls_pk_init(&pub);
    memset(sig, 0, sizeof(sig));

    TEST_ASSERT(mbedtls_pk_setup(&prv, mbedtls_pk_info_from_type(pk_type)) == 0);
    TEST_ASSERT(mbedtls_ecp_group_load(&mbedtls_pk_ec(prv)->grp, grp_id) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&mbedtls_pk_ec(prv)->d, d_str) == 0);

    TEST_ASSERT(mbedtls_pk_setup(&pub, mbedtls_pk_info_from_type(pk_type)) == 0);
    TEST_ASSERT(mbedtls_ecp_group_load(&mbedtls_pk_ec(pub)->grp, grp_id) == 0);
    TEST_ASSERT(mbedtls_ecp_point_read_string(&mbedtls_pk_ec(pub)->Q, 16, QX_str, QY_str) == 0);

    mbedtls_ecp_set_max_ops(max_ops);

    slen = sizeof(sig);
    cnt_restart = 0;
    do {
        ret = mbedtls_pk_sign_restartable(&prv, md_alg, hash->x, hash->len,
                                          sig, sizeof(sig), &slen,
                                          mbedtls_test_rnd_std_rand, NULL,
                                          &rs_ctx);
    } while (ret == MBEDTLS_ERR_ECP_IN_PROGRESS && ++cnt_restart);

    TEST_ASSERT(ret == 0);
    TEST_ASSERT(slen == sig_check->len);
    TEST_ASSERT(memcmp(sig, sig_check->x, slen) == 0);

    TEST_ASSERT(cnt_restart >= min_restart);
    TEST_ASSERT(cnt_restart <= max_restart);

    cnt_restart = 0;
    do {
        ret = mbedtls_pk_verify_restartable(&pub, md_alg,
                                            hash->x, hash->len, sig, slen, &rs_ctx);
    } while (ret == MBEDTLS_ERR_ECP_IN_PROGRESS && ++cnt_restart);

    TEST_ASSERT(ret == 0);
    TEST_ASSERT(cnt_restart >= min_restart);
    TEST_ASSERT(cnt_restart <= max_restart);

    sig[0]++;
    do {
        ret = mbedtls_pk_verify_restartable(&pub, md_alg,
                                            hash->x, hash->len, sig, slen, &rs_ctx);
    } while (ret == MBEDTLS_ERR_ECP_IN_PROGRESS);
    TEST_ASSERT(ret != 0);
    sig[0]--;

    /* Do we leak memory when aborting? try verify then sign
     * This test only makes sense when we actually restart */
    if (min_restart > 0) {
        ret = mbedtls_pk_verify_restartable(&pub, md_alg,
                                            hash->x, hash->len, sig, slen, &rs_ctx);
        TEST_ASSERT(ret == MBEDTLS_ERR_ECP_IN_PROGRESS);
        mbedtls_pk_restart_free(&rs_ctx);

        slen = sizeof(sig);
        ret = mbedtls_pk_sign_restartable(&prv, md_alg, hash->x, hash->len,
                                          sig, sizeof(sig), &slen,
                                          mbedtls_test_rnd_std_rand, NULL,
                                          &rs_ctx);
        TEST_ASSERT(ret == MBEDTLS_ERR_ECP_IN_PROGRESS);
    }

exit:
    mbedtls_pk_restart_free(&rs_ctx);
    mbedtls_pk_free(&prv);
    mbedtls_pk_free(&pub);
    USE_PSA_DONE();
}

static void test_pk_sign_verify_restart_wrapper( void ** params )
{
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_pk_sign_verify_restart( *( (int *) params[0] ), *( (int *) params[1] ), (char *) params[2], (char *) params[3], (char *) params[4], *( (int *) params[5] ), &data6, &data8, *( (int *) params[10] ), *( (int *) params[11] ), *( (int *) params[12] ) );
}
#endif /* MBEDTLS_ECDSA_DETERMINISTIC */
#endif /* MBEDTLS_ECDSA_C */
#endif /* MBEDTLS_ECP_RESTARTABLE */
#if defined(MBEDTLS_HAS_ALG_SHA_256_VIA_MD_OR_PSA_BASED_ON_USE_PSA)
#line 744 "suites/test_suite_pk.function"
static void test_pk_sign_verify(int type, int parameter, int sign_ret, int verify_ret)
{
    mbedtls_pk_context pk;
    size_t sig_len;
    unsigned char hash[32]; // Hard-coded for SHA256
    size_t hash_len = sizeof(hash);
    unsigned char sig[MBEDTLS_PK_SIGNATURE_MAX_SIZE];
    void *rs_ctx = NULL;
#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECP_RESTARTABLE)
    mbedtls_pk_restart_ctx ctx;

    rs_ctx = &ctx;
    mbedtls_pk_restart_init(rs_ctx);
    /* This value is large enough that the operation will complete in one run.
     * See comments at the top of ecp_test_vect_restart in
     * test_suite_ecp.function for estimates of operation counts. */
    mbedtls_ecp_set_max_ops(42000);
#endif

    mbedtls_pk_init(&pk);
    USE_PSA_INIT();

    memset(hash, 0x2a, sizeof(hash));
    memset(sig, 0, sizeof(sig));

    TEST_ASSERT(mbedtls_pk_setup(&pk, mbedtls_pk_info_from_type(type)) == 0);
    TEST_ASSERT(pk_genkey(&pk, parameter) == 0);

    TEST_ASSERT(mbedtls_pk_sign_restartable(&pk, MBEDTLS_MD_SHA256,
                                            hash, hash_len,
                                            sig, sizeof(sig), &sig_len,
                                            mbedtls_test_rnd_std_rand, NULL,
                                            rs_ctx) == sign_ret);
    if (sign_ret == 0) {
        TEST_ASSERT(sig_len <= MBEDTLS_PK_SIGNATURE_MAX_SIZE);
    } else {
        sig_len = MBEDTLS_PK_SIGNATURE_MAX_SIZE;
    }

    TEST_ASSERT(mbedtls_pk_verify(&pk, MBEDTLS_MD_SHA256,
                                  hash, hash_len, sig, sig_len) == verify_ret);

    if (verify_ret == 0) {
        hash[0]++;
        TEST_ASSERT(mbedtls_pk_verify(&pk, MBEDTLS_MD_SHA256,
                                      hash, hash_len, sig, sig_len) != 0);
        hash[0]--;

        sig[0]++;
        TEST_ASSERT(mbedtls_pk_verify(&pk, MBEDTLS_MD_SHA256,
                                      hash, hash_len, sig, sig_len) != 0);
        sig[0]--;
    }

    TEST_ASSERT(mbedtls_pk_sign(&pk, MBEDTLS_MD_SHA256, hash, hash_len,
                                sig, sizeof(sig), &sig_len,
                                mbedtls_test_rnd_std_rand,
                                NULL) == sign_ret);
    if (sign_ret == 0) {
        TEST_ASSERT(sig_len <= MBEDTLS_PK_SIGNATURE_MAX_SIZE);
    } else {
        sig_len = MBEDTLS_PK_SIGNATURE_MAX_SIZE;
    }

    TEST_ASSERT(mbedtls_pk_verify_restartable(&pk, MBEDTLS_MD_SHA256,
                                              hash, hash_len, sig, sig_len, rs_ctx) == verify_ret);

    if (verify_ret == 0) {
        hash[0]++;
        TEST_ASSERT(mbedtls_pk_verify_restartable(&pk, MBEDTLS_MD_SHA256,
                                                  hash, sizeof(hash), sig, sig_len, rs_ctx) != 0);
        hash[0]--;

        sig[0]++;
        TEST_ASSERT(mbedtls_pk_verify_restartable(&pk, MBEDTLS_MD_SHA256,
                                                  hash, sizeof(hash), sig, sig_len, rs_ctx) != 0);
        sig[0]--;
    }

exit:
#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECP_RESTARTABLE)
    mbedtls_pk_restart_free(rs_ctx);
#endif
    mbedtls_pk_free(&pk);
    USE_PSA_DONE();
}

static void test_pk_sign_verify_wrapper( void ** params )
{

    test_pk_sign_verify( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ) );
}
#endif /* MBEDTLS_HAS_ALG_SHA_256_VIA_MD_OR_PSA_BASED_ON_USE_PSA */
#if defined(MBEDTLS_RSA_C)
#line 833 "suites/test_suite_pk.function"
static void test_pk_rsa_encrypt_decrypt_test(data_t *message, int mod,
                                 char *input_P, char *input_Q,
                                 char *input_N, char *input_E,
                                 int ret)
{
    unsigned char output[300], result[300];
    mbedtls_test_rnd_pseudo_info rnd_info;
    mbedtls_mpi N, P, Q, E;
    mbedtls_rsa_context *rsa;
    mbedtls_pk_context pk;
    size_t olen, rlen;

    mbedtls_pk_init(&pk);
    mbedtls_mpi_init(&N); mbedtls_mpi_init(&P);
    mbedtls_mpi_init(&Q); mbedtls_mpi_init(&E);

    memset(&rnd_info,  0, sizeof(mbedtls_test_rnd_pseudo_info));
    memset(output,     0, sizeof(output));

    USE_PSA_INIT();

    /* encryption test */

    /* init pk-rsa context */
    TEST_ASSERT(mbedtls_pk_setup(&pk, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA)) == 0);
    rsa = mbedtls_pk_rsa(pk);

    /* load public key */
    rsa->len = mod / 8;
    TEST_ASSERT(mbedtls_test_read_mpi(&rsa->N, input_N) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&rsa->E, input_E) == 0);

    TEST_ASSERT(mbedtls_pk_encrypt(&pk, message->x, message->len,
                                   output, &olen, sizeof(output),
                                   mbedtls_test_rnd_pseudo_rand, &rnd_info) == ret);

    /* decryption test */
    mbedtls_mpi_init(&N); mbedtls_mpi_init(&P);
    mbedtls_mpi_init(&Q); mbedtls_mpi_init(&E);

    /* init pk-rsa context */
    mbedtls_pk_free(&pk);
    TEST_ASSERT(mbedtls_pk_setup(&pk,
                                 mbedtls_pk_info_from_type(MBEDTLS_PK_RSA)) == 0);
    rsa = mbedtls_pk_rsa(pk);

    /* load public key */
    TEST_ASSERT(mbedtls_test_read_mpi(&N, input_N) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&E, input_E) == 0);

    /* load private key */
    TEST_ASSERT(mbedtls_test_read_mpi(&P, input_P) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&Q, input_Q) == 0);
    TEST_ASSERT(mbedtls_rsa_import(rsa, &N, &P, &Q, NULL, &E) == 0);
    TEST_ASSERT(mbedtls_rsa_get_len(rsa) == (size_t) (mod / 8));
    TEST_ASSERT(mbedtls_rsa_complete(rsa) == 0);

    memset(result, 0, sizeof(result));
    rlen = 0;
    TEST_ASSERT(mbedtls_pk_decrypt(&pk, output, olen,
                                   result, &rlen, sizeof(result),
                                   mbedtls_test_rnd_pseudo_rand, &rnd_info) == ret);
    if (ret == 0) {
        TEST_ASSERT(rlen == message->len);
        TEST_ASSERT(memcmp(result, message->x, rlen) == 0);
    }

exit:
    mbedtls_mpi_free(&N); mbedtls_mpi_free(&P);
    mbedtls_mpi_free(&Q); mbedtls_mpi_free(&E);
    mbedtls_pk_free(&pk);
    USE_PSA_DONE();
}

static void test_pk_rsa_encrypt_decrypt_test_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_pk_rsa_encrypt_decrypt_test( &data0, *( (int *) params[2] ), (char *) params[3], (char *) params[4], (char *) params[5], (char *) params[6], *( (int *) params[7] ) );
}
#endif /* MBEDTLS_RSA_C */
#if defined(MBEDTLS_RSA_C)
#line 909 "suites/test_suite_pk.function"
static void test_pk_rsa_decrypt_test_vec(data_t *cipher, int mod,
                             char *input_P, char *input_Q,
                             char *input_N, char *input_E,
                             data_t *clear, int ret)
{
    unsigned char output[256];
    mbedtls_test_rnd_pseudo_info rnd_info;
    mbedtls_mpi N, P, Q, E;
    mbedtls_rsa_context *rsa;
    mbedtls_pk_context pk;
    size_t olen;

    USE_PSA_INIT();

    mbedtls_pk_init(&pk);
    mbedtls_mpi_init(&N); mbedtls_mpi_init(&P);
    mbedtls_mpi_init(&Q); mbedtls_mpi_init(&E);

    memset(&rnd_info,  0, sizeof(mbedtls_test_rnd_pseudo_info));


    /* init pk-rsa context */
    TEST_ASSERT(mbedtls_pk_setup(&pk, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA)) == 0);
    rsa = mbedtls_pk_rsa(pk);

    /* load public key */
    TEST_ASSERT(mbedtls_test_read_mpi(&N, input_N) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&E, input_E) == 0);

    /* load private key */
    TEST_ASSERT(mbedtls_test_read_mpi(&P, input_P) == 0);
    TEST_ASSERT(mbedtls_test_read_mpi(&Q, input_Q) == 0);
    TEST_ASSERT(mbedtls_rsa_import(rsa, &N, &P, &Q, NULL, &E) == 0);
    TEST_ASSERT(mbedtls_rsa_get_len(rsa) == (size_t) (mod / 8));
    TEST_ASSERT(mbedtls_rsa_complete(rsa) == 0);

    /* decryption test */
    memset(output, 0, sizeof(output));
    olen = 0;
    TEST_ASSERT(mbedtls_pk_decrypt(&pk, cipher->x, cipher->len,
                                   output, &olen, sizeof(output),
                                   mbedtls_test_rnd_pseudo_rand, &rnd_info) == ret);
    if (ret == 0) {
        TEST_ASSERT(olen == clear->len);
        TEST_ASSERT(memcmp(output, clear->x, olen) == 0);
    }

exit:
    mbedtls_mpi_free(&N); mbedtls_mpi_free(&P);
    mbedtls_mpi_free(&Q); mbedtls_mpi_free(&E);
    mbedtls_pk_free(&pk);
    USE_PSA_DONE();
}

static void test_pk_rsa_decrypt_test_vec_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};

    test_pk_rsa_decrypt_test_vec( &data0, *( (int *) params[2] ), (char *) params[3], (char *) params[4], (char *) params[5], (char *) params[6], &data7, *( (int *) params[9] ) );
}
#endif /* MBEDTLS_RSA_C */
#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_USE_PSA_CRYPTO)
#line 965 "suites/test_suite_pk.function"
static void test_pk_wrap_rsa_decrypt_test_vec(data_t *cipher, int mod,
                                  char *input_P, char *input_Q,
                                  char *input_N, char *input_E,
                                  data_t *clear, int ret)
{
    unsigned char output[256];
    mbedtls_test_rnd_pseudo_info rnd_info;
    mbedtls_mpi N, P, Q, E;
    mbedtls_rsa_context *rsa;
    mbedtls_pk_context pk;
    mbedtls_svc_key_id_t key_id;
    size_t olen;

    USE_PSA_INIT();

    mbedtls_pk_init(&pk);
    mbedtls_mpi_init(&N); mbedtls_mpi_init(&P);
    mbedtls_mpi_init(&Q); mbedtls_mpi_init(&E);

    memset(&rnd_info,  0, sizeof(mbedtls_test_rnd_pseudo_info));

    /* init pk-rsa context */
    TEST_EQUAL(mbedtls_pk_setup(&pk,
                                mbedtls_pk_info_from_type(MBEDTLS_PK_RSA)), 0);
    rsa = mbedtls_pk_rsa(pk);

    /* load public key */
    TEST_EQUAL(mbedtls_test_read_mpi(&N, input_N), 0);
    TEST_EQUAL(mbedtls_test_read_mpi(&E, input_E), 0);

    /* load private key */
    TEST_EQUAL(mbedtls_test_read_mpi(&P, input_P), 0);
    TEST_EQUAL(mbedtls_test_read_mpi(&Q, input_Q), 0);
    TEST_EQUAL(mbedtls_rsa_import(rsa, &N, &P, &Q, NULL, &E), 0);
    TEST_EQUAL(mbedtls_rsa_get_len(rsa), (size_t) (mod / 8));
    TEST_EQUAL(mbedtls_rsa_complete(rsa), 0);

    /* Turn PK context into an opaque one. */
    TEST_EQUAL(mbedtls_pk_wrap_as_opaque(&pk, &key_id,
                                         PSA_ALG_RSA_PKCS1V15_CRYPT,
                                         PSA_KEY_USAGE_DECRYPT,
                                         PSA_ALG_NONE), 0);

    /* decryption test */
    memset(output, 0, sizeof(output));
    olen = 0;
    TEST_EQUAL(mbedtls_pk_decrypt(&pk, cipher->x, cipher->len,
                                  output, &olen, sizeof(output),
                                  mbedtls_test_rnd_pseudo_rand, &rnd_info), ret);
    if (ret == 0) {
        TEST_EQUAL(olen, clear->len);
        TEST_EQUAL(memcmp(output, clear->x, olen), 0);
    }

    TEST_EQUAL(PSA_SUCCESS, psa_destroy_key(key_id));

exit:
    mbedtls_mpi_free(&N); mbedtls_mpi_free(&P);
    mbedtls_mpi_free(&Q); mbedtls_mpi_free(&E);
    mbedtls_pk_free(&pk);
    USE_PSA_DONE();
}

static void test_pk_wrap_rsa_decrypt_test_vec_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};

    test_pk_wrap_rsa_decrypt_test_vec( &data0, *( (int *) params[2] ), (char *) params[3], (char *) params[4], (char *) params[5], (char *) params[6], &data7, *( (int *) params[9] ) );
}
#endif /* MBEDTLS_USE_PSA_CRYPTO */
#endif /* MBEDTLS_RSA_C */
#line 1030 "suites/test_suite_pk.function"
static void test_pk_ec_nocrypt(int type)
{
    mbedtls_pk_context pk;
    unsigned char output[100];
    unsigned char input[100];
    mbedtls_test_rnd_pseudo_info rnd_info;
    size_t olen = 0;
    int ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;

    mbedtls_pk_init(&pk);

    memset(&rnd_info,  0, sizeof(mbedtls_test_rnd_pseudo_info));
    memset(output,     0, sizeof(output));
    memset(input,      0, sizeof(input));

    TEST_ASSERT(mbedtls_pk_setup(&pk, mbedtls_pk_info_from_type(type)) == 0);

    TEST_ASSERT(mbedtls_pk_encrypt(&pk, input, sizeof(input),
                                   output, &olen, sizeof(output),
                                   mbedtls_test_rnd_pseudo_rand, &rnd_info) == ret);

    TEST_ASSERT(mbedtls_pk_decrypt(&pk, input, sizeof(input),
                                   output, &olen, sizeof(output),
                                   mbedtls_test_rnd_pseudo_rand, &rnd_info) == ret);

exit:
    mbedtls_pk_free(&pk);
}

static void test_pk_ec_nocrypt_wrapper( void ** params )
{

    test_pk_ec_nocrypt( *( (int *) params[0] ) );
}
#if defined(MBEDTLS_RSA_C)
#line 1061 "suites/test_suite_pk.function"
static void test_pk_rsa_overflow(void)
{
    mbedtls_pk_context pk;
    size_t hash_len = SIZE_MAX, sig_len = SIZE_MAX;
    unsigned char hash[50], sig[100];

    memset(hash, 0x2a, sizeof(hash));
    memset(sig, 0, sizeof(sig));

    mbedtls_pk_init(&pk);

    TEST_ASSERT(mbedtls_pk_setup(&pk,
                                 mbedtls_pk_info_from_type(MBEDTLS_PK_RSA)) == 0);

#if defined(MBEDTLS_PKCS1_V21)
    TEST_ASSERT(mbedtls_pk_verify_ext(MBEDTLS_PK_RSASSA_PSS, NULL, &pk,
                                      MBEDTLS_MD_NONE, hash, hash_len, sig, sig_len) ==
                MBEDTLS_ERR_PK_BAD_INPUT_DATA);
#endif /* MBEDTLS_PKCS1_V21 */

    TEST_ASSERT(mbedtls_pk_verify(&pk, MBEDTLS_MD_NONE, hash, hash_len,
                                  sig, sig_len) == MBEDTLS_ERR_PK_BAD_INPUT_DATA);

    TEST_ASSERT(mbedtls_pk_sign(&pk, MBEDTLS_MD_NONE, hash, hash_len,
                                sig, sizeof(sig), &sig_len,
                                mbedtls_test_rnd_std_rand, NULL)
                == MBEDTLS_ERR_PK_BAD_INPUT_DATA);

exit:
    mbedtls_pk_free(&pk);
}

static void test_pk_rsa_overflow_wrapper( void ** params )
{
    (void)params;

    test_pk_rsa_overflow(  );
}
#endif /* MBEDTLS_RSA_C */
#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_PK_RSA_ALT_SUPPORT)
#line 1095 "suites/test_suite_pk.function"
static void test_pk_rsa_alt(void)
{
    /*
     * An rsa_alt context can only do private operations (decrypt, sign).
     * Test it against the public operations (encrypt, verify) of a
     * corresponding rsa context.
     */
    mbedtls_rsa_context raw;
    mbedtls_pk_context rsa, alt;
    mbedtls_pk_debug_item dbg_items[10];
    unsigned char hash[50], sig[64];
    unsigned char msg[50], ciph[64], test[50];
    size_t sig_len, ciph_len, test_len;
    int ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;

    USE_PSA_INIT();

    mbedtls_rsa_init(&raw);
    mbedtls_pk_init(&rsa); mbedtls_pk_init(&alt);

    memset(hash, 0x2a, sizeof(hash));
    memset(sig, 0, sizeof(sig));
    memset(msg, 0x2a, sizeof(msg));
    memset(ciph, 0, sizeof(ciph));
    memset(test, 0, sizeof(test));

    /* Initialize PK RSA context with random key */
    TEST_ASSERT(mbedtls_pk_setup(&rsa,
                                 mbedtls_pk_info_from_type(MBEDTLS_PK_RSA)) == 0);
    TEST_ASSERT(pk_genkey(&rsa, RSA_KEY_SIZE) == 0);

    /* Extract key to the raw rsa context */
    TEST_ASSERT(mbedtls_rsa_copy(&raw, mbedtls_pk_rsa(rsa)) == 0);

    /* Initialize PK RSA_ALT context */
    TEST_ASSERT(mbedtls_pk_setup_rsa_alt(&alt, (void *) &raw,
                                         mbedtls_rsa_decrypt_func, mbedtls_rsa_sign_func,
                                         mbedtls_rsa_key_len_func) == 0);

    /* Test administrative functions */
    TEST_ASSERT(mbedtls_pk_can_do(&alt, MBEDTLS_PK_RSA));
    TEST_ASSERT(mbedtls_pk_get_bitlen(&alt) == RSA_KEY_SIZE);
    TEST_ASSERT(mbedtls_pk_get_len(&alt) == RSA_KEY_LEN);
    TEST_ASSERT(mbedtls_pk_get_type(&alt) == MBEDTLS_PK_RSA_ALT);
    TEST_ASSERT(strcmp(mbedtls_pk_get_name(&alt), "RSA-alt") == 0);

    /* Test signature */
#if SIZE_MAX > UINT_MAX
    TEST_ASSERT(mbedtls_pk_sign(&alt, MBEDTLS_MD_NONE, hash, SIZE_MAX,
                                sig, sizeof(sig), &sig_len,
                                mbedtls_test_rnd_std_rand, NULL)
                == MBEDTLS_ERR_PK_BAD_INPUT_DATA);
#endif /* SIZE_MAX > UINT_MAX */
    TEST_ASSERT(mbedtls_pk_sign(&alt, MBEDTLS_MD_NONE, hash, sizeof(hash),
                                sig, sizeof(sig), &sig_len,
                                mbedtls_test_rnd_std_rand, NULL)
                == 0);
    TEST_ASSERT(sig_len == RSA_KEY_LEN);
    TEST_ASSERT(mbedtls_pk_verify(&rsa, MBEDTLS_MD_NONE,
                                  hash, sizeof(hash), sig, sig_len) == 0);

    /* Test decrypt */
    TEST_ASSERT(mbedtls_pk_encrypt(&rsa, msg, sizeof(msg),
                                   ciph, &ciph_len, sizeof(ciph),
                                   mbedtls_test_rnd_std_rand, NULL) == 0);
    TEST_ASSERT(mbedtls_pk_decrypt(&alt, ciph, ciph_len,
                                   test, &test_len, sizeof(test),
                                   mbedtls_test_rnd_std_rand, NULL) == 0);
    TEST_ASSERT(test_len == sizeof(msg));
    TEST_ASSERT(memcmp(test, msg, test_len) == 0);

    /* Test forbidden operations */
    TEST_ASSERT(mbedtls_pk_encrypt(&alt, msg, sizeof(msg),
                                   ciph, &ciph_len, sizeof(ciph),
                                   mbedtls_test_rnd_std_rand, NULL) == ret);
    TEST_ASSERT(mbedtls_pk_verify(&alt, MBEDTLS_MD_NONE,
                                  hash, sizeof(hash), sig, sig_len) == ret);
    TEST_ASSERT(mbedtls_pk_debug(&alt, dbg_items) == ret);

exit:
    mbedtls_rsa_free(&raw);
    mbedtls_pk_free(&rsa); mbedtls_pk_free(&alt);
    USE_PSA_DONE();
}

static void test_pk_rsa_alt_wrapper( void ** params )
{
    (void)params;

    test_pk_rsa_alt(  );
}
#endif /* MBEDTLS_PK_RSA_ALT_SUPPORT */
#endif /* MBEDTLS_RSA_C */
#if defined(MBEDTLS_HAS_ALG_SHA_256_VIA_MD_OR_PSA_BASED_ON_USE_PSA)
#if defined(MBEDTLS_USE_PSA_CRYPTO)
#line 1182 "suites/test_suite_pk.function"
static void test_pk_psa_sign(int parameter_arg,
                 int psa_type_arg, int expected_bits_arg)
{
    mbedtls_pk_context pk;
    unsigned char hash[32];
    unsigned char sig[MBEDTLS_PK_SIGNATURE_MAX_SIZE];
    unsigned char pkey_legacy[200];
    unsigned char pkey_psa[200];
    unsigned char *pkey_legacy_start, *pkey_psa_start;
    psa_algorithm_t alg_psa;
    size_t sig_len, klen_legacy, klen_psa;
    int ret;
    mbedtls_svc_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_type_t expected_type = psa_type_arg;
    size_t expected_bits = expected_bits_arg;

    /*
     * This tests making signatures with a wrapped PSA key:
     * - generate a fresh ECP/RSA legacy PK context
     * - wrap it in a PK context and make a signature this way
     * - extract the public key
     * - parse it to a PK context and verify the signature this way
     */

    PSA_ASSERT(psa_crypto_init());

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_GENPRIME)
    if (PSA_KEY_TYPE_IS_RSA(psa_type_arg)) {
        /* Create legacy RSA public/private key in PK context. */
        mbedtls_pk_init(&pk);
        TEST_ASSERT(mbedtls_pk_setup(&pk,
                                     mbedtls_pk_info_from_type(MBEDTLS_PK_RSA)) == 0);
        TEST_ASSERT(mbedtls_rsa_gen_key(mbedtls_pk_rsa(pk),
                                        mbedtls_test_rnd_std_rand, NULL,
                                        parameter_arg, 3) == 0);
        alg_psa = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256);
    } else
#endif /* MBEDTLS_RSA_C && MBEDTLS_GENPRIME */
#if defined(MBEDTLS_PK_CAN_ECDSA_SIGN)
    if (PSA_KEY_TYPE_IS_ECC_KEY_PAIR(psa_type_arg)) {
        mbedtls_ecp_group_id grpid = parameter_arg;

        /* Create legacy EC public/private key in PK context. */
        mbedtls_pk_init(&pk);
        TEST_ASSERT(mbedtls_pk_setup(&pk,
                                     mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY)) == 0);
        TEST_ASSERT(mbedtls_ecp_gen_key(grpid,
                                        (mbedtls_ecp_keypair *) pk.pk_ctx,
                                        mbedtls_test_rnd_std_rand, NULL) == 0);
        alg_psa = PSA_ALG_ECDSA(PSA_ALG_SHA_256);
    } else
#endif /* MBEDTLS_PK_CAN_ECDSA_SIGN */
    {
        (void) parameter_arg;
        TEST_ASSUME(!"Opaque PK key not supported in this configuration");
    }

    /* Export underlying public key for re-importing in a legacy context. */
#if defined(MBEDTLS_PK_WRITE_C)
    ret = mbedtls_pk_write_pubkey_der(&pk, pkey_legacy,
                                      sizeof(pkey_legacy));
    TEST_ASSERT(ret >= 0);
    klen_legacy = (size_t) ret;
    /* mbedtls_pk_write_pubkey_der() writes backwards in the data buffer. */
    pkey_legacy_start = pkey_legacy + sizeof(pkey_legacy) - klen_legacy;
#else
    ret = mbedtls_ecp_point_write_binary(&(mbedtls_pk_ec(pk)->grp),
                                         &(mbedtls_pk_ec(pk)->Q),
                                         MBEDTLS_ECP_PF_UNCOMPRESSED,
                                         &klen_legacy, pkey_legacy,
                                         sizeof(pkey_legacy));
    TEST_EQUAL(ret, 0);
    pkey_legacy_start = pkey_legacy;
#endif /* MBEDTLS_PK_WRITE_C */

    /* Turn PK context into an opaque one. */
    TEST_ASSERT(mbedtls_pk_wrap_as_opaque(&pk, &key_id, alg_psa,
                                          PSA_KEY_USAGE_SIGN_HASH,
                                          PSA_ALG_NONE) == 0);

    PSA_ASSERT(psa_get_key_attributes(key_id, &attributes));
    TEST_EQUAL(psa_get_key_type(&attributes), expected_type);
    TEST_EQUAL(psa_get_key_bits(&attributes), expected_bits);
    TEST_EQUAL(psa_get_key_lifetime(&attributes),
               PSA_KEY_LIFETIME_VOLATILE);

    memset(hash, 0x2a, sizeof(hash));
    memset(sig, 0, sizeof(sig));

    TEST_ASSERT(mbedtls_pk_sign(&pk, MBEDTLS_MD_SHA256,
                                hash, sizeof(hash), sig, sizeof(sig), &sig_len,
                                NULL, NULL) == 0);

    /* Export underlying public key for re-importing in a psa context. */
#if defined(MBEDTLS_PK_WRITE_C)
    ret = mbedtls_pk_write_pubkey_der(&pk, pkey_psa,
                                      sizeof(pkey_psa));
    TEST_ASSERT(ret >= 0);
    klen_psa = (size_t) ret;
    /* mbedtls_pk_write_pubkey_der() writes backwards in the data buffer. */
    pkey_psa_start = pkey_psa + sizeof(pkey_psa) - klen_psa;
#else
    psa_status_t status;

    status = psa_export_public_key(key_id, pkey_psa, sizeof(pkey_psa),
                                   &klen_psa);
    TEST_EQUAL(status, PSA_SUCCESS);
    pkey_psa_start = pkey_psa;
#endif /* MBEDTLS_PK_WRITE_C */

    TEST_ASSERT(klen_psa == klen_legacy);
    TEST_ASSERT(memcmp(pkey_psa_start, pkey_legacy_start, klen_psa) == 0);

    mbedtls_pk_free(&pk);
    TEST_ASSERT(PSA_SUCCESS == psa_destroy_key(key_id));

    mbedtls_pk_init(&pk);

    /* If we used "pk_write" previously, then we go for a "pk_parse" here;
     * otherwise if we went for "ecp_point_write_binary" then we'll go
     * for a "ecp_point_read_binary" here. This allows to drop dependencies
     * on "PK_WRITE" and "PK_PARSE" if required */
#if defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_PK_PARSE_C)
    TEST_EQUAL(mbedtls_pk_parse_public_key(&pk, pkey_legacy_start,
                                           klen_legacy), 0);
#else
    TEST_EQUAL(mbedtls_pk_setup(&pk,
                                mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY)), 0);
    TEST_EQUAL(mbedtls_ecp_group_load(
                   &(mbedtls_pk_ec(pk)->grp),
                   (mbedtls_ecp_group_id) parameter_arg), 0);
    TEST_EQUAL(mbedtls_ecp_point_read_binary(&(mbedtls_pk_ec(pk)->grp),
                                             &(mbedtls_pk_ec(pk)->Q),
                                             pkey_legacy_start, klen_legacy), 0);
#endif
    TEST_ASSERT(mbedtls_pk_verify(&pk, MBEDTLS_MD_SHA256,
                                  hash, sizeof(hash), sig, sig_len) == 0);

exit:
    /*
     * Key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes(&attributes);

    mbedtls_pk_free(&pk);
    USE_PSA_DONE();
}

static void test_pk_psa_sign_wrapper( void ** params )
{

    test_pk_psa_sign( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
#endif /* MBEDTLS_USE_PSA_CRYPTO */
#endif /* MBEDTLS_HAS_ALG_SHA_256_VIA_MD_OR_PSA_BASED_ON_USE_PSA */
#if defined(MBEDTLS_PSA_CRYPTO_C)
#if defined(MBEDTLS_GENPRIME)
#line 1334 "suites/test_suite_pk.function"
static void test_pk_psa_sign_ext(int pk_type, int parameter, int key_pk_type, int md_alg)
{
    /* See the description of pk_genkey() for the description of the `parameter` argument. */
    mbedtls_pk_context pk;
    size_t sig_len;
    unsigned char sig[MBEDTLS_PK_SIGNATURE_MAX_SIZE];
    unsigned char hash[PSA_HASH_MAX_SIZE];
    size_t hash_len = mbedtls_hash_info_get_size(md_alg);
    void const *options = NULL;
    mbedtls_pk_rsassa_pss_options rsassa_pss_options;
    memset(hash, 0x2a, sizeof(hash));
    memset(sig, 0, sizeof(sig));

    mbedtls_pk_init(&pk);
    PSA_INIT();

    TEST_ASSERT(mbedtls_pk_setup(&pk,
                                 mbedtls_pk_info_from_type(pk_type)) == 0);

    TEST_ASSERT(pk_genkey(&pk, parameter) == 0);

    TEST_ASSERT(mbedtls_pk_sign_ext(key_pk_type, &pk, md_alg, hash, hash_len,
                                    sig, sizeof(sig), &sig_len,
                                    mbedtls_test_rnd_std_rand, NULL) == 0);

    if (key_pk_type == MBEDTLS_PK_RSASSA_PSS) {
        rsassa_pss_options.mgf1_hash_id = md_alg;
        TEST_ASSERT(hash_len != 0);
        rsassa_pss_options.expected_salt_len = hash_len;
        options = (const void *) &rsassa_pss_options;
    }
    TEST_ASSERT(mbedtls_pk_verify_ext(key_pk_type, options, &pk, md_alg,
                                      hash, hash_len, sig, sig_len) == 0);
exit:
    PSA_DONE();
    mbedtls_pk_free(&pk);
}

static void test_pk_psa_sign_ext_wrapper( void ** params )
{

    test_pk_psa_sign_ext( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ) );
}
#endif /* MBEDTLS_GENPRIME */
#endif /* MBEDTLS_PSA_CRYPTO_C */
#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_GENPRIME)
#if defined(MBEDTLS_USE_PSA_CRYPTO)
#line 1374 "suites/test_suite_pk.function"
static void test_pk_psa_wrap_sign_ext(int pk_type, int parameter, int key_pk_type, int md_alg)
{
    /* See the description of mbedtls_rsa_gen_key() for the description of the `parameter` argument. */
    mbedtls_pk_context pk;
    size_t sig_len, pkey_len;
    mbedtls_svc_key_id_t key_id;
    unsigned char sig[MBEDTLS_PK_SIGNATURE_MAX_SIZE];
    unsigned char pkey[PSA_EXPORT_PUBLIC_KEY_MAX_SIZE];
    unsigned char *pkey_start;
    unsigned char hash[PSA_HASH_MAX_SIZE];
    psa_algorithm_t psa_md_alg = mbedtls_hash_info_psa_from_md(md_alg);
    psa_algorithm_t psa_alg;
    size_t hash_len = PSA_HASH_LENGTH(psa_md_alg);
    void const *options = NULL;
    mbedtls_pk_rsassa_pss_options rsassa_pss_options;
    int ret;

    mbedtls_pk_init(&pk);
    PSA_INIT();

    /* Create legacy RSA public/private key in PK context. */
    mbedtls_pk_init(&pk);
    TEST_EQUAL(mbedtls_pk_setup(&pk,
                                mbedtls_pk_info_from_type(pk_type)), 0);
    TEST_EQUAL(mbedtls_rsa_gen_key(mbedtls_pk_rsa(pk),
                                   mbedtls_test_rnd_std_rand, NULL,
                                   parameter, 3), 0);

    /* Export underlying public key for re-importing in a legacy context. */
    ret = mbedtls_pk_write_pubkey_der(&pk, pkey, sizeof(pkey));
    TEST_ASSERT(ret >= 0);

    pkey_len = (size_t) ret;
    /* mbedtls_pk_write_pubkey_der() writes backwards in the data buffer. */
    pkey_start = pkey + sizeof(pkey) - pkey_len;

    if (key_pk_type == MBEDTLS_PK_RSA) {
        psa_alg = PSA_ALG_RSA_PKCS1V15_SIGN(psa_md_alg);
    } else if (key_pk_type == MBEDTLS_PK_RSASSA_PSS) {
        psa_alg = PSA_ALG_RSA_PSS(psa_md_alg);
    } else {
        TEST_ASSUME(!"PK key type not supported in this configuration");
    }

    /* Turn PK context into an opaque one. */
    TEST_EQUAL(mbedtls_pk_wrap_as_opaque(&pk, &key_id, psa_alg,
                                         PSA_KEY_USAGE_SIGN_HASH,
                                         PSA_ALG_NONE), 0);

    memset(hash, 0x2a, sizeof(hash));
    memset(sig, 0, sizeof(sig));

    TEST_EQUAL(mbedtls_pk_sign_ext(key_pk_type, &pk, md_alg, hash, hash_len,
                                   sig, sizeof(sig), &sig_len,
                                   mbedtls_test_rnd_std_rand, NULL), 0);

    mbedtls_pk_free(&pk);
    TEST_EQUAL(PSA_SUCCESS, psa_destroy_key(key_id));

    mbedtls_pk_init(&pk);
    TEST_EQUAL(mbedtls_pk_parse_public_key(&pk, pkey_start, pkey_len), 0);

    if (key_pk_type == MBEDTLS_PK_RSASSA_PSS) {
        rsassa_pss_options.mgf1_hash_id = md_alg;
        TEST_ASSERT(hash_len != 0);
        rsassa_pss_options.expected_salt_len = hash_len;
        options = (const void *) &rsassa_pss_options;
    }
    TEST_EQUAL(mbedtls_pk_verify_ext(key_pk_type, options, &pk, md_alg,
                                     hash, hash_len, sig, sig_len), 0);

exit:
    mbedtls_pk_free(&pk);
    PSA_DONE();
}

static void test_pk_psa_wrap_sign_ext_wrapper( void ** params )
{

    test_pk_psa_wrap_sign_ext( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ) );
}
#endif /* MBEDTLS_USE_PSA_CRYPTO */
#endif /* MBEDTLS_GENPRIME */
#endif /* MBEDTLS_RSA_C */
#endif /* MBEDTLS_PK_C */


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
    
#if defined(MBEDTLS_PK_C)

        case 0:
            {
                *out_value = MBEDTLS_PK_RSA;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_PK_ECKEY;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_ECP_DP_SECP192R1;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_PK_ECKEY_DH;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_ECP_DP_CURVE25519;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_ECP_DP_CURVE448;
            }
            break;
        case 6:
            {
                *out_value = MBEDTLS_PK_ECDSA;
            }
            break;
        case 7:
            {
                *out_value = MBEDTLS_ECP_DP_SECP256R1;
            }
            break;
        case 8:
            {
                *out_value = MBEDTLS_ECP_DP_SECP384R1;
            }
            break;
        case 9:
            {
                *out_value = MBEDTLS_ECP_DP_SECP521R1;
            }
            break;
        case 10:
            {
                *out_value = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1);
            }
            break;
        case 11:
            {
                *out_value = PSA_KEY_USAGE_SIGN_HASH;
            }
            break;
        case 12:
            {
                *out_value = PSA_ALG_ECDSA(PSA_ALG_ANY_HASH);
            }
            break;
        case 13:
            {
                *out_value = PSA_ALG_NONE;
            }
            break;
        case 14:
            {
                *out_value = PSA_ALG_STREAM_CIPHER;
            }
            break;
        case 15:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256);
            }
            break;
        case 16:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_CRYPT;
            }
            break;
        case 17:
            {
                *out_value = PSA_ALG_RSA_PSS(PSA_ALG_SHA_256);
            }
            break;
        case 18:
            {
                *out_value = PSA_ALG_ECDH;
            }
            break;
        case 19:
            {
                *out_value = PSA_ALG_ECDSA(PSA_ALG_SHA_256);
            }
            break;
        case 20:
            {
                *out_value = PSA_ALG_ECDSA(PSA_ALG_SHA_1);
            }
            break;
        case 21:
            {
                *out_value = PSA_KEY_USAGE_DERIVE;
            }
            break;
        case 22:
            {
                *out_value = PSA_KEY_USAGE_DERIVE|PSA_KEY_USAGE_SIGN_HASH;
            }
            break;
        case 23:
            {
                *out_value = PSA_KEY_TYPE_RSA_KEY_PAIR;
            }
            break;
        case 24:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH);
            }
            break;
        case 25:
            {
                *out_value = PSA_KEY_USAGE_COPY;
            }
            break;
        case 26:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_1);
            }
            break;
        case 27:
            {
                *out_value = PSA_ALG_RSA_PSS(PSA_ALG_ANY_HASH);
            }
            break;
        case 28:
            {
                *out_value = PSA_ALG_RSA_PSS(PSA_ALG_SHA_1);
            }
            break;
        case 29:
            {
                *out_value = PSA_ALG_RSA_PKCS1V15_SIGN_RAW;
            }
            break;
        case 30:
            {
                *out_value = PSA_KEY_USAGE_ENCRYPT|PSA_KEY_USAGE_DECRYPT;
            }
            break;
        case 31:
            {
                *out_value = PSA_KEY_USAGE_DECRYPT;
            }
            break;
        case 32:
            {
                *out_value = PSA_KEY_USAGE_ENCRYPT|PSA_KEY_USAGE_DECRYPT|PSA_KEY_USAGE_SIGN_HASH;
            }
            break;
        case 33:
            {
                *out_value = PSA_KEY_USAGE_ENCRYPT;
            }
            break;
        case 34:
            {
                *out_value = MBEDTLS_MD_SHA1;
            }
            break;
        case 35:
            {
                *out_value = MBEDTLS_ERR_RSA_VERIFY_FAILED;
            }
            break;
        case 36:
            {
                *out_value = MBEDTLS_ERR_ECP_VERIFY_FAILED;
            }
            break;
        case 37:
            {
                *out_value = MBEDTLS_ECP_DP_BP256R1;
            }
            break;
        case 38:
            {
                *out_value = MBEDTLS_ECP_DP_BP512R1;
            }
            break;
        case 39:
            {
                *out_value = MBEDTLS_ERR_PK_TYPE_MISMATCH;
            }
            break;
        case 40:
            {
                *out_value = MBEDTLS_ERR_RSA_INVALID_PADDING;
            }
            break;
        case 41:
            {
                *out_value = MBEDTLS_MD_SHA256;
            }
            break;
        case 42:
            {
                *out_value = MBEDTLS_PK_RSASSA_PSS;
            }
            break;
        case 43:
            {
                *out_value = MBEDTLS_RSA_SALT_LEN_ANY;
            }
            break;
        case 44:
            {
                *out_value = MBEDTLS_MD_NONE;
            }
            break;
        case 45:
            {
                *out_value = -1;
            }
            break;
        case 46:
            {
                *out_value = MBEDTLS_ERR_PK_BAD_INPUT_DATA;
            }
            break;
        case 47:
            {
                *out_value = MBEDTLS_ERR_PK_SIG_LEN_MISMATCH;
            }
            break;
        case 48:
            {
                *out_value = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
            }
            break;
        case 49:
            {
                *out_value = MBEDTLS_ERR_RSA_KEY_CHECK_FAILED;
            }
            break;
        case 50:
            {
                *out_value = MBEDTLS_ECP_DP_SECP192K1;
            }
            break;
        case 51:
            {
                *out_value = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_K1);
            }
            break;
        case 52:
            {
                *out_value = MBEDTLS_ECP_DP_SECP256K1;
            }
            break;
        case 53:
            {
                *out_value = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_BRAINPOOL_P_R1);
            }
            break;
        case 54:
            {
                *out_value = MBEDTLS_ECP_DP_BP384R1;
            }
            break;
        case 55:
            {
                *out_value = MBEDTLS_MD_SHA384;
            }
            break;
        case 56:
            {
                *out_value = MBEDTLS_MD_SHA512;
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
    
#if defined(MBEDTLS_PK_C)

        case 0:
            {
#if defined(MBEDTLS_RSA_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(MBEDTLS_GENPRIME)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(MBEDTLS_ECP_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 3:
            {
#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 4:
            {
#if defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 5:
            {
#if defined(MBEDTLS_ECP_DP_CURVE448_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 6:
            {
#if defined(MBEDTLS_PK_CAN_ECDSA_VERIFY)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 7:
            {
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 8:
            {
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 9:
            {
#if defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 10:
            {
#if defined(MBEDTLS_PK_CAN_ECDSA_SIGN)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 11:
            {
#if defined(MBEDTLS_HAS_ALG_SHA_1_VIA_LOWLEVEL_OR_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 12:
            {
#if defined(MBEDTLS_PKCS1_V15)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 13:
            {
#if defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 14:
            {
#if defined(MBEDTLS_ECP_DP_BP512R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 15:
            {
#if defined(MBEDTLS_PKCS1_V21)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 16:
            {
#if defined(MBEDTLS_HAS_ALG_SHA_256_VIA_MD_OR_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 17:
            {
#if !defined(MBEDTLS_USE_PSA_CRYPTO)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 18:
            {
#if defined(MBEDTLS_USE_PSA_CRYPTO)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 19:
            {
#if defined(MBEDTLS_HAS_ALG_SHA_1_VIA_MD_OR_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 20:
            {
#if defined(MBEDTLS_HAS_ALG_SHA_256_VIA_LOWLEVEL_OR_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 21:
            {
#if defined(MBEDTLS_PEM_PARSE_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 22:
            {
#if defined(MBEDTLS_HAVE_INT64)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 23:
            {
#if defined(MBEDTLS_ECP_DP_SECP192K1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 24:
            {
#if defined(MBEDTLS_ECP_DP_SECP256K1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 25:
            {
#if defined(MBEDTLS_ECP_DP_BP384R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 26:
            {
#if defined(MBEDTLS_PK_WRITE_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 27:
            {
#if defined(MBEDTLS_HAS_ALG_SHA_384_VIA_LOWLEVEL_OR_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 28:
            {
#if defined(MBEDTLS_HAS_ALG_SHA_384_VIA_MD_OR_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 29:
            {
#if defined(MBEDTLS_HAS_ALG_SHA_512_VIA_LOWLEVEL_OR_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 30:
            {
#if defined(MBEDTLS_HAS_ALG_SHA_512_VIA_MD_OR_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 31:
            {
#if defined(MBEDTLS_HAS_ALG_SHA_256_VIA_MD_OR_PSA_BASED_ON_USE_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 32:
            {
#if defined(MBEDTLS_HAS_ALG_SHA_384_VIA_MD_OR_PSA_BASED_ON_USE_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 33:
            {
#if defined(MBEDTLS_HAS_ALG_SHA_512_VIA_MD_OR_PSA_BASED_ON_USE_PSA)
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

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_USE_PSA_CRYPTO)
    test_pk_psa_utils_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_USE_PSA_CRYPTO)
    test_pk_can_do_ext_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_PK_C)
    test_pk_invalid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_PK_C)
    test_valid_parameters_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_PK_WRITE_C)
    test_valid_parameters_pkwrite_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_PK_C)
    test_pk_utils_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_PK_PARSE_C) && defined(MBEDTLS_FS_IO)
    test_mbedtls_pk_check_pair_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_RSA_C)
    test_pk_rsa_verify_test_vec_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_RSA_C)
    test_pk_rsa_verify_ext_test_vec_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_PK_CAN_ECDSA_VERIFY)
    test_pk_ec_test_vec_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_ECP_RESTARTABLE) && defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECDSA_DETERMINISTIC)
    test_pk_sign_verify_restart_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_HAS_ALG_SHA_256_VIA_MD_OR_PSA_BASED_ON_USE_PSA)
    test_pk_sign_verify_wrapper,
#else
    NULL,
#endif
/* Function Id: 12 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_RSA_C)
    test_pk_rsa_encrypt_decrypt_test_wrapper,
#else
    NULL,
#endif
/* Function Id: 13 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_RSA_C)
    test_pk_rsa_decrypt_test_vec_wrapper,
#else
    NULL,
#endif
/* Function Id: 14 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_USE_PSA_CRYPTO)
    test_pk_wrap_rsa_decrypt_test_vec_wrapper,
#else
    NULL,
#endif
/* Function Id: 15 */

#if defined(MBEDTLS_PK_C)
    test_pk_ec_nocrypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 16 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_RSA_C)
    test_pk_rsa_overflow_wrapper,
#else
    NULL,
#endif
/* Function Id: 17 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_PK_RSA_ALT_SUPPORT)
    test_pk_rsa_alt_wrapper,
#else
    NULL,
#endif
/* Function Id: 18 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_HAS_ALG_SHA_256_VIA_MD_OR_PSA_BASED_ON_USE_PSA) && defined(MBEDTLS_USE_PSA_CRYPTO)
    test_pk_psa_sign_wrapper,
#else
    NULL,
#endif
/* Function Id: 19 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_GENPRIME)
    test_pk_psa_sign_ext_wrapper,
#else
    NULL,
#endif
/* Function Id: 20 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_GENPRIME) && defined(MBEDTLS_USE_PSA_CRYPTO)
    test_pk_psa_wrap_sign_ext_wrapper,
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
    const char *default_filename = "./test_suite_pk.datax";
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
