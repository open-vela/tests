#line 2 "suites/main_test.function"
/*
 * *** THIS FILE HAS BEEN MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py
 *
 * Test file      : ./test_suite_x509write.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_x509write.function
 *      Test suite data     : suites/test_suite_x509write.data
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
#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_PK_PARSE_C)
#line 2 "suites/test_suite_x509write.function"
#include "mbedtls/bignum.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/x509_csr.h"
#include "mbedtls/pem.h"
#include "mbedtls/oid.h"
#include "mbedtls/rsa.h"
#include "mbedtls/asn1write.h"
#include "mbedtls/pk.h"

#include "hash_info.h"
#include "mbedtls/legacy_or_psa.h"

#if defined(MBEDTLS_RSA_C)
static int mbedtls_rsa_decrypt_func(void *ctx, size_t *olen,
                             const unsigned char *input, unsigned char *output,
                             size_t output_max_len)
{
    return mbedtls_rsa_pkcs1_decrypt((mbedtls_rsa_context *) ctx, NULL, NULL,
                                     olen, input, output, output_max_len);
}
static int mbedtls_rsa_sign_func(void *ctx,
                          int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
                          mbedtls_md_type_t md_alg, unsigned int hashlen,
                          const unsigned char *hash, unsigned char *sig)
{
    return mbedtls_rsa_pkcs1_sign((mbedtls_rsa_context *) ctx, f_rng, p_rng,
                                  md_alg, hashlen, hash, sig);
}
static size_t mbedtls_rsa_key_len_func(void *ctx)
{
    return ((const mbedtls_rsa_context *) ctx)->len;
}
#endif /* MBEDTLS_RSA_C */

#if defined(MBEDTLS_USE_PSA_CRYPTO) && \
    defined(MBEDTLS_PEM_WRITE_C) && defined(MBEDTLS_X509_CSR_WRITE_C)
static int x509_crt_verifycsr(const unsigned char *buf, size_t buflen)
{
    unsigned char hash[PSA_HASH_MAX_SIZE];
    mbedtls_x509_csr csr;
    int ret = 0;

    mbedtls_x509_csr_init(&csr);

    if (mbedtls_x509_csr_parse(&csr, buf, buflen) != 0) {
        ret = MBEDTLS_ERR_X509_BAD_INPUT_DATA;
        goto cleanup;
    }

    psa_algorithm_t psa_alg = mbedtls_hash_info_psa_from_md(csr.sig_md);
    size_t hash_size = 0;
    psa_status_t status = psa_hash_compute(psa_alg, csr.cri.p, csr.cri.len,
                                           hash, PSA_HASH_MAX_SIZE, &hash_size);

    if (status != PSA_SUCCESS) {
        /* Note: this can't happen except after an internal error */
        ret = MBEDTLS_ERR_X509_BAD_INPUT_DATA;
        goto cleanup;
    }

    if (mbedtls_pk_verify_ext(csr.sig_pk, csr.sig_opts, &csr.pk,
                              csr.sig_md, hash, mbedtls_hash_info_get_size(csr.sig_md),
                              csr.sig.p, csr.sig.len) != 0) {
        ret = MBEDTLS_ERR_X509_CERT_VERIFY_FAILED;
        goto cleanup;
    }

cleanup:

    mbedtls_x509_csr_free(&csr);
    return ret;
}
#endif /* MBEDTLS_USE_PSA_CRYPTO && MBEDTLS_PEM_WRITE_C && MBEDTLS_X509_CSR_WRITE_C */

#if defined(MBEDTLS_X509_CSR_WRITE_C)

/*
 * The size of this temporary buffer is given by the sequence of functions
 * called hereinafter:
 * - mbedtls_asn1_write_oid()
 *     - 8 bytes for MBEDTLS_OID_EXTENDED_KEY_USAGE raw value
 *     - 1 byte for MBEDTLS_OID_EXTENDED_KEY_USAGE length
 *     - 1 byte for MBEDTLS_ASN1_OID tag
 * - mbedtls_asn1_write_len()
 *     - 1 byte since we're dealing with sizes which are less than 0x80
 * - mbedtls_asn1_write_tag()
 *     - 1 byte
 *
 * This length is fine as long as this function is called using the
 * MBEDTLS_OID_SERVER_AUTH OID. If this is changed in the future, then this
 * buffer's length should be adjusted accordingly.
 * Unfortunately there's no predefined max size for OIDs which can be used
 * to set an overall upper boundary which is always guaranteed.
 */
#define EXT_KEY_USAGE_TMP_BUF_MAX_LENGTH    12

static int csr_set_extended_key_usage(mbedtls_x509write_csr *ctx,
                                      const char *oid, size_t oid_len)
{
    unsigned char buf[EXT_KEY_USAGE_TMP_BUF_MAX_LENGTH] = { 0 };
    unsigned char *p = buf + sizeof(buf);
    int ret;
    size_t len = 0;

    /*
     * Following functions fail anyway if the temporary buffer is not large,
     * but we set an extra check here to emphasize a possible source of errors
     */
    if (oid_len > EXT_KEY_USAGE_TMP_BUF_MAX_LENGTH) {
        return MBEDTLS_ERR_X509_BAD_INPUT_DATA;
    }

    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_oid(&p, buf, oid, oid_len));
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_len(&p, buf, ret));
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_tag(&p, buf,
                                                     MBEDTLS_ASN1_CONSTRUCTED |
                                                     MBEDTLS_ASN1_SEQUENCE));

    ret = mbedtls_x509write_csr_set_extension(ctx,
                                              MBEDTLS_OID_EXTENDED_KEY_USAGE,
                                              MBEDTLS_OID_SIZE(MBEDTLS_OID_EXTENDED_KEY_USAGE),
                                              0,
                                              p,
                                              len);

    return ret;
}
#endif  /* MBEDTLS_X509_CSR_WRITE_C */
#if defined(MBEDTLS_PEM_WRITE_C)
#if defined(MBEDTLS_X509_CSR_WRITE_C)
#line 138 "suites/test_suite_x509write.function"
static void test_x509_csr_check(char *key_file, char *cert_req_check_file, int md_type,
                    int key_usage, int set_key_usage, int cert_type,
                    int set_cert_type, int set_extension)
{
    mbedtls_pk_context key;
    mbedtls_x509write_csr req;
    unsigned char buf[4096];
    int ret;
#if !defined(MBEDTLS_USE_PSA_CRYPTO)
    unsigned char check_buf[4000];
    FILE *f;
    size_t olen = 0;
#endif /* !MBEDTLS_USE_PSA_CRYPTO */
    size_t pem_len = 0, buf_index;
    int der_len = -1;
    const char *subject_name = "C=NL,O=PolarSSL,CN=PolarSSL Server 1";
    mbedtls_test_rnd_pseudo_info rnd_info;
    mbedtls_x509_san_list san_ip;
    mbedtls_x509_san_list san_dns;
    mbedtls_x509_san_list san_uri;
    mbedtls_x509_san_list *san_list = NULL;
    const char san_ip_name[] = { 0x7f, 0x01, 0x01, 0x00 }; // 127.1.1.0
    const char *san_dns_name = "example.com";
    const char *san_uri_name = "http://pki.example.com/";

    san_uri.node.type = MBEDTLS_X509_SAN_UNIFORM_RESOURCE_IDENTIFIER;
    san_uri.node.san.unstructured_name.p = (unsigned char *) san_uri_name;
    san_uri.node.san.unstructured_name.len = strlen(san_uri_name);
    san_uri.next = NULL;
    san_ip.node.type = MBEDTLS_X509_SAN_IP_ADDRESS;
    san_ip.node.san.unstructured_name.p = (unsigned char *) san_ip_name;
    san_ip.node.san.unstructured_name.len = sizeof(san_ip_name);
    san_ip.next = &san_uri;
    san_dns.node.type = MBEDTLS_X509_SAN_DNS_NAME;
    san_dns.node.san.unstructured_name.p = (unsigned char *) san_dns_name;
    san_dns.node.san.unstructured_name.len = strlen(san_dns_name);
    san_dns.next = &san_ip;
    san_list = &san_dns;

    memset(&rnd_info, 0x2a, sizeof(mbedtls_test_rnd_pseudo_info));

    mbedtls_x509write_csr_init(&req);

    USE_PSA_INIT();

    mbedtls_pk_init(&key);
    TEST_ASSERT(mbedtls_pk_parse_keyfile(&key, key_file, NULL,
                                         mbedtls_test_rnd_std_rand, NULL) == 0);

    mbedtls_x509write_csr_set_md_alg(&req, md_type);
    mbedtls_x509write_csr_set_key(&req, &key);
    TEST_ASSERT(mbedtls_x509write_csr_set_subject_name(&req, subject_name) == 0);
    if (set_key_usage != 0) {
        TEST_ASSERT(mbedtls_x509write_csr_set_key_usage(&req, key_usage) == 0);
    }
    if (set_cert_type != 0) {
        TEST_ASSERT(mbedtls_x509write_csr_set_ns_cert_type(&req, cert_type) == 0);
    }
    if (set_extension != 0) {
        TEST_ASSERT(csr_set_extended_key_usage(&req, MBEDTLS_OID_SERVER_AUTH,
                                               MBEDTLS_OID_SIZE(MBEDTLS_OID_SERVER_AUTH)) == 0);

        TEST_ASSERT(mbedtls_x509write_csr_set_subject_alternative_name(&req, san_list) == 0);
    }

    ret = mbedtls_x509write_csr_pem(&req, buf, sizeof(buf),
                                    mbedtls_test_rnd_pseudo_rand, &rnd_info);
    TEST_ASSERT(ret == 0);

    pem_len = strlen((char *) buf);

    for (buf_index = pem_len; buf_index < sizeof(buf); ++buf_index) {
        TEST_ASSERT(buf[buf_index] == 0);
    }

#if defined(MBEDTLS_USE_PSA_CRYPTO)
    // When using PSA crypto, RNG isn't controllable, so cert_req_check_file can't be used
    (void) cert_req_check_file;
    buf[pem_len] = '\0';
    TEST_ASSERT(x509_crt_verifycsr(buf, pem_len + 1) == 0);
#else
    f = fopen(cert_req_check_file, "r");
    TEST_ASSERT(f != NULL);
    olen = fread(check_buf, 1, sizeof(check_buf), f);
    fclose(f);

    TEST_ASSERT(olen >= pem_len - 1);
    TEST_ASSERT(memcmp(buf, check_buf, pem_len - 1) == 0);
#endif /* MBEDTLS_USE_PSA_CRYPTO */

    der_len = mbedtls_x509write_csr_der(&req, buf, sizeof(buf),
                                        mbedtls_test_rnd_pseudo_rand,
                                        &rnd_info);
    TEST_ASSERT(der_len >= 0);

    if (der_len == 0) {
        goto exit;
    }

#if defined(MBEDTLS_USE_PSA_CRYPTO)
    // When using PSA crypto, RNG isn't controllable, result length isn't
    // deterministic over multiple runs, removing a single byte isn't enough to
    // go into the MBEDTLS_ERR_ASN1_BUF_TOO_SMALL error case
    der_len /= 2;
#else
    der_len -= 1;
#endif
    ret = mbedtls_x509write_csr_der(&req, buf, (size_t) (der_len),
                                    mbedtls_test_rnd_pseudo_rand, &rnd_info);
    TEST_ASSERT(ret == MBEDTLS_ERR_ASN1_BUF_TOO_SMALL);

exit:
    mbedtls_x509write_csr_free(&req);
    mbedtls_pk_free(&key);
    USE_PSA_DONE();
}

static void test_x509_csr_check_wrapper( void ** params )
{

    test_x509_csr_check( (char *) params[0], (char *) params[1], *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), *( (int *) params[6] ), *( (int *) params[7] ) );
}
#endif /* MBEDTLS_X509_CSR_WRITE_C */
#endif /* MBEDTLS_PEM_WRITE_C */
#if defined(MBEDTLS_PEM_WRITE_C)
#if defined(MBEDTLS_X509_CSR_WRITE_C)
#if defined(MBEDTLS_USE_PSA_CRYPTO)
#line 257 "suites/test_suite_x509write.function"
static void test_x509_csr_check_opaque(char *key_file, int md_type, int key_usage,
                           int cert_type)
{
    mbedtls_pk_context key;
    mbedtls_svc_key_id_t key_id = MBEDTLS_SVC_KEY_ID_INIT;
    psa_algorithm_t md_alg_psa, alg_psa;
    mbedtls_x509write_csr req;
    unsigned char buf[4096];
    int ret;
    size_t pem_len = 0;
    const char *subject_name = "C=NL,O=PolarSSL,CN=PolarSSL Server 1";
    mbedtls_test_rnd_pseudo_info rnd_info;

    memset(&rnd_info, 0x2a, sizeof(mbedtls_test_rnd_pseudo_info));

    mbedtls_x509write_csr_init(&req);

    USE_PSA_INIT();

    md_alg_psa = mbedtls_hash_info_psa_from_md((mbedtls_md_type_t) md_type);
    TEST_ASSERT(md_alg_psa != MBEDTLS_MD_NONE);

    mbedtls_pk_init(&key);
    TEST_ASSERT(mbedtls_pk_parse_keyfile(&key, key_file, NULL,
                                         mbedtls_test_rnd_std_rand, NULL) == 0);

    if (mbedtls_pk_get_type(&key) == MBEDTLS_PK_ECKEY) {
        alg_psa = PSA_ALG_ECDSA(md_alg_psa);
    } else if (mbedtls_pk_get_type(&key) == MBEDTLS_PK_RSA) {
        alg_psa = PSA_ALG_RSA_PKCS1V15_SIGN(md_alg_psa);
    } else {
        TEST_ASSUME(!"PK key type not supported in this configuration");
    }

    TEST_ASSERT(mbedtls_pk_wrap_as_opaque(&key, &key_id, alg_psa,
                                          PSA_KEY_USAGE_SIGN_HASH,
                                          PSA_ALG_NONE) == 0);

    mbedtls_x509write_csr_set_md_alg(&req, md_type);
    mbedtls_x509write_csr_set_key(&req, &key);
    TEST_ASSERT(mbedtls_x509write_csr_set_subject_name(&req, subject_name) == 0);
    if (key_usage != 0) {
        TEST_ASSERT(mbedtls_x509write_csr_set_key_usage(&req, key_usage) == 0);
    }
    if (cert_type != 0) {
        TEST_ASSERT(mbedtls_x509write_csr_set_ns_cert_type(&req, cert_type) == 0);
    }

    ret = mbedtls_x509write_csr_pem(&req, buf, sizeof(buf) - 1,
                                    mbedtls_test_rnd_pseudo_rand, &rnd_info);

    TEST_ASSERT(ret == 0);

    pem_len = strlen((char *) buf);
    buf[pem_len] = '\0';
    TEST_ASSERT(x509_crt_verifycsr(buf, pem_len + 1) == 0);


exit:
    mbedtls_x509write_csr_free(&req);
    mbedtls_pk_free(&key);
    psa_destroy_key(key_id);
    PSA_DONE();
}

static void test_x509_csr_check_opaque_wrapper( void ** params )
{

    test_x509_csr_check_opaque( (char *) params[0], *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ) );
}
#endif /* MBEDTLS_USE_PSA_CRYPTO */
#endif /* MBEDTLS_X509_CSR_WRITE_C */
#endif /* MBEDTLS_PEM_WRITE_C */
#if defined(MBEDTLS_PEM_WRITE_C)
#if defined(MBEDTLS_X509_CRT_WRITE_C)
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(MBEDTLS_HAS_ALG_SHA_1_VIA_MD_OR_PSA_BASED_ON_USE_PSA)
#line 324 "suites/test_suite_x509write.function"
static void test_x509_crt_check(char *subject_key_file, char *subject_pwd,
                    char *subject_name, char *issuer_key_file,
                    char *issuer_pwd, char *issuer_name,
                    data_t *serial_arg, char *not_before, char *not_after,
                    int md_type, int key_usage, int set_key_usage,
                    char *ext_key_usage,
                    int cert_type, int set_cert_type, int auth_ident,
                    int ver, char *cert_check_file, int pk_wrap, int is_ca,
                    char *cert_verify_file)
{
    mbedtls_pk_context subject_key, issuer_key, issuer_key_alt;
    mbedtls_pk_context *key = &issuer_key;

    mbedtls_x509write_cert crt;
    unsigned char buf[4096];
    unsigned char check_buf[5000];
    unsigned char *p, *end;
    unsigned char tag, sz;
#if defined(MBEDTLS_TEST_DEPRECATED) && defined(MBEDTLS_BIGNUM_C)
    mbedtls_mpi serial_mpi;
#endif
    int ret, before_tag, after_tag;
    size_t olen = 0, pem_len = 0, buf_index = 0;
    int der_len = -1;
    FILE *f;
    mbedtls_test_rnd_pseudo_info rnd_info;
#if defined(MBEDTLS_USE_PSA_CRYPTO)
    mbedtls_svc_key_id_t key_id = MBEDTLS_SVC_KEY_ID_INIT;
#endif
    mbedtls_pk_type_t issuer_key_type;

    memset(&rnd_info, 0x2a, sizeof(mbedtls_test_rnd_pseudo_info));
#if defined(MBEDTLS_TEST_DEPRECATED) && defined(MBEDTLS_BIGNUM_C)
    mbedtls_mpi_init(&serial_mpi);
#endif

    USE_PSA_INIT();

    mbedtls_pk_init(&subject_key);
    mbedtls_pk_init(&issuer_key);
    mbedtls_pk_init(&issuer_key_alt);

    mbedtls_x509write_crt_init(&crt);

    TEST_ASSERT(mbedtls_pk_parse_keyfile(&subject_key, subject_key_file,
                                         subject_pwd, mbedtls_test_rnd_std_rand, NULL) == 0);

    TEST_ASSERT(mbedtls_pk_parse_keyfile(&issuer_key, issuer_key_file,
                                         issuer_pwd, mbedtls_test_rnd_std_rand, NULL) == 0);

    issuer_key_type = mbedtls_pk_get_type(&issuer_key);

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_PK_RSA_ALT_SUPPORT)
    /* For RSA PK contexts, create a copy as an alternative RSA context. */
    if (pk_wrap == 1 && issuer_key_type == MBEDTLS_PK_RSA) {
        TEST_ASSERT(mbedtls_pk_setup_rsa_alt(&issuer_key_alt,
                                             mbedtls_pk_rsa(issuer_key),
                                             mbedtls_rsa_decrypt_func,
                                             mbedtls_rsa_sign_func,
                                             mbedtls_rsa_key_len_func) == 0);

        key = &issuer_key_alt;
    }
#endif

#if defined(MBEDTLS_USE_PSA_CRYPTO)
    /* For Opaque PK contexts, wrap key as an Opaque RSA context. */
    if (pk_wrap == 2) {
        psa_algorithm_t alg_psa, md_alg_psa;

        md_alg_psa = mbedtls_hash_info_psa_from_md((mbedtls_md_type_t) md_type);
        TEST_ASSERT(md_alg_psa != MBEDTLS_MD_NONE);

        if (mbedtls_pk_get_type(&issuer_key) == MBEDTLS_PK_ECKEY) {
            alg_psa = PSA_ALG_ECDSA(md_alg_psa);
        } else if (mbedtls_pk_get_type(&issuer_key) == MBEDTLS_PK_RSA) {
            alg_psa = PSA_ALG_RSA_PKCS1V15_SIGN(md_alg_psa);
        } else {
            TEST_ASSUME(!"PK key type not supported in this configuration");
        }

        TEST_ASSERT(mbedtls_pk_wrap_as_opaque(&issuer_key, &key_id, alg_psa,
                                              PSA_KEY_USAGE_SIGN_HASH,
                                              PSA_ALG_NONE) == 0);
    }
#endif /* MBEDTLS_USE_PSA_CRYPTO */

    if (pk_wrap == 2) {
        TEST_ASSERT(mbedtls_pk_get_type(&issuer_key) == MBEDTLS_PK_OPAQUE);
    }

    if (ver != -1) {
        mbedtls_x509write_crt_set_version(&crt, ver);
    }

#if defined(MBEDTLS_TEST_DEPRECATED) && defined(MBEDTLS_BIGNUM_C)
    TEST_ASSERT(mbedtls_mpi_read_binary(&serial_mpi, serial_arg->x,
                                        serial_arg->len) == 0);
    TEST_ASSERT(mbedtls_x509write_crt_set_serial(&crt, &serial_mpi) == 0);
#else
    TEST_ASSERT(mbedtls_x509write_crt_set_serial_raw(&crt, serial_arg->x,
                                                     serial_arg->len) == 0);
#endif
    TEST_ASSERT(mbedtls_x509write_crt_set_validity(&crt, not_before,
                                                   not_after) == 0);
    mbedtls_x509write_crt_set_md_alg(&crt, md_type);
    TEST_ASSERT(mbedtls_x509write_crt_set_issuer_name(&crt, issuer_name) == 0);
    TEST_ASSERT(mbedtls_x509write_crt_set_subject_name(&crt, subject_name) == 0);
    mbedtls_x509write_crt_set_subject_key(&crt, &subject_key);

    mbedtls_x509write_crt_set_issuer_key(&crt, key);

    if (crt.version >= MBEDTLS_X509_CRT_VERSION_3) {
        /* For the CA case, a path length of -1 means unlimited. */
        TEST_ASSERT(mbedtls_x509write_crt_set_basic_constraints(&crt, is_ca,
                                                                (is_ca ? -1 : 0)) == 0);
        TEST_ASSERT(mbedtls_x509write_crt_set_subject_key_identifier(&crt) == 0);
        if (auth_ident) {
            TEST_ASSERT(mbedtls_x509write_crt_set_authority_key_identifier(&crt) == 0);
        }
        if (set_key_usage != 0) {
            TEST_ASSERT(mbedtls_x509write_crt_set_key_usage(&crt, key_usage) == 0);
        }
        if (set_cert_type != 0) {
            TEST_ASSERT(mbedtls_x509write_crt_set_ns_cert_type(&crt, cert_type) == 0);
        }
        if (strcmp(ext_key_usage, "NULL") != 0) {
            mbedtls_asn1_sequence exts[2];
            memset(exts, 0, sizeof(exts));

#define SET_OID(x, oid)                \
    do {                               \
        x.len = MBEDTLS_OID_SIZE(oid); \
        x.p   = (unsigned char *) oid;   \
        x.tag = MBEDTLS_ASN1_OID;      \
    }                                  \
    while (0)

            if (strcmp(ext_key_usage, "serverAuth") == 0) {
                SET_OID(exts[0].buf, MBEDTLS_OID_SERVER_AUTH);
            } else if (strcmp(ext_key_usage, "codeSigning,timeStamping") == 0) {
                SET_OID(exts[0].buf, MBEDTLS_OID_CODE_SIGNING);
                exts[0].next = &exts[1];
                SET_OID(exts[1].buf, MBEDTLS_OID_TIME_STAMPING);
            }
            TEST_ASSERT(mbedtls_x509write_crt_set_ext_key_usage(&crt, exts) == 0);
        }
    }

    ret = mbedtls_x509write_crt_pem(&crt, buf, sizeof(buf),
                                    mbedtls_test_rnd_pseudo_rand, &rnd_info);
    TEST_ASSERT(ret == 0);

    pem_len = strlen((char *) buf);

    // check that the rest of the buffer remains clear
    for (buf_index = pem_len; buf_index < sizeof(buf); ++buf_index) {
        TEST_ASSERT(buf[buf_index] == 0);
    }

    if (issuer_key_type != MBEDTLS_PK_RSA) {
        mbedtls_x509_crt crt_parse, trusted;
        uint32_t flags;

        mbedtls_x509_crt_init(&crt_parse);
        mbedtls_x509_crt_init(&trusted);

        TEST_ASSERT(mbedtls_x509_crt_parse_file(&trusted,
                                                cert_verify_file) == 0);
        TEST_ASSERT(mbedtls_x509_crt_parse(&crt_parse,
                                           buf, sizeof(buf)) == 0);

        ret = mbedtls_x509_crt_verify(&crt_parse, &trusted, NULL, NULL, &flags,
                                      NULL, NULL);

        mbedtls_x509_crt_free(&crt_parse);
        mbedtls_x509_crt_free(&trusted);

        TEST_EQUAL(flags, 0);
        TEST_EQUAL(ret, 0);
    } else if (*cert_check_file != '\0') {
        f = fopen(cert_check_file, "r");
        TEST_ASSERT(f != NULL);
        olen = fread(check_buf, 1, sizeof(check_buf), f);
        fclose(f);
        TEST_ASSERT(olen < sizeof(check_buf));

        TEST_EQUAL(olen, pem_len);
        TEST_ASSERT(olen >= pem_len - 1);
        TEST_ASSERT(memcmp(buf, check_buf, pem_len - 1) == 0);
    }

    der_len = mbedtls_x509write_crt_der(&crt, buf, sizeof(buf),
                                        mbedtls_test_rnd_pseudo_rand,
                                        &rnd_info);
    TEST_ASSERT(der_len >= 0);

    if (der_len == 0) {
        goto exit;
    }

    // Not testing against file, check date format
    if (*cert_check_file == '\0') {
        // UTC tag if before 2050, 2 digits less for year
        if (not_before[0] == '2' && (not_before[1] > '0' || not_before[2] > '4')) {
            before_tag = MBEDTLS_ASN1_GENERALIZED_TIME;
        } else {
            before_tag = MBEDTLS_ASN1_UTC_TIME;
            not_before += 2;
        }
        if (not_after[0] == '2' && (not_after[1] > '0' || not_after[2] > '4')) {
            after_tag = MBEDTLS_ASN1_GENERALIZED_TIME;
        } else {
            after_tag = MBEDTLS_ASN1_UTC_TIME;
            not_after += 2;
        }
        end = buf + sizeof(buf);
        for (p = end - der_len; p < end;) {
            tag = *p++;
            sz = *p++;
            if (tag == MBEDTLS_ASN1_UTC_TIME || tag == MBEDTLS_ASN1_GENERALIZED_TIME) {
                // Check correct tag and time written
                TEST_ASSERT(before_tag == tag);
                TEST_ASSERT(memcmp(p, not_before, sz - 1) == 0);
                p += sz;
                tag = *p++;
                sz = *p++;
                TEST_ASSERT(after_tag == tag);
                TEST_ASSERT(memcmp(p, not_after, sz - 1) == 0);
                break;
            }
            // Increment if long form ASN1 length
            if (sz & 0x80) {
                p += sz & 0x0F;
            }
            if (tag != (MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE)) {
                p += sz;
            }
        }
        TEST_ASSERT(p < end);
    }

#if defined(MBEDTLS_USE_PSA_CRYPTO)
    // When using PSA crypto, RNG isn't controllable, result length isn't
    // deterministic over multiple runs, removing a single byte isn't enough to
    // go into the MBEDTLS_ERR_ASN1_BUF_TOO_SMALL error case
    if (issuer_key_type != MBEDTLS_PK_RSA) {
        der_len /= 2;
    } else
#endif
    der_len -= 1;

    ret = mbedtls_x509write_crt_der(&crt, buf, (size_t) (der_len),
                                    mbedtls_test_rnd_pseudo_rand, &rnd_info);
    TEST_ASSERT(ret == MBEDTLS_ERR_ASN1_BUF_TOO_SMALL);

exit:
    mbedtls_x509write_crt_free(&crt);
    mbedtls_pk_free(&issuer_key_alt);
    mbedtls_pk_free(&subject_key);
    mbedtls_pk_free(&issuer_key);
#if defined(MBEDTLS_TEST_DEPRECATED) && defined(MBEDTLS_BIGNUM_C)
    mbedtls_mpi_free(&serial_mpi);
#endif
#if defined(MBEDTLS_USE_PSA_CRYPTO)
    psa_destroy_key(key_id);
#endif
    USE_PSA_DONE();
}

static void test_x509_crt_check_wrapper( void ** params )
{
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_x509_crt_check( (char *) params[0], (char *) params[1], (char *) params[2], (char *) params[3], (char *) params[4], (char *) params[5], &data6, (char *) params[8], (char *) params[9], *( (int *) params[10] ), *( (int *) params[11] ), *( (int *) params[12] ), (char *) params[13], *( (int *) params[14] ), *( (int *) params[15] ), *( (int *) params[16] ), *( (int *) params[17] ), (char *) params[18], *( (int *) params[19] ), *( (int *) params[20] ), (char *) params[21] );
}
#endif /* MBEDTLS_HAS_ALG_SHA_1_VIA_MD_OR_PSA_BASED_ON_USE_PSA */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#endif /* MBEDTLS_X509_CRT_WRITE_C */
#endif /* MBEDTLS_PEM_WRITE_C */
#if defined(MBEDTLS_X509_CRT_WRITE_C)
#line 596 "suites/test_suite_x509write.function"
static void test_x509_set_serial_check(void)
{
    mbedtls_x509write_cert ctx;
    uint8_t invalid_serial[MBEDTLS_X509_RFC5280_MAX_SERIAL_LEN + 1];

    memset(invalid_serial, 0x01, sizeof(invalid_serial));

#if defined(MBEDTLS_TEST_DEPRECATED) && defined(MBEDTLS_BIGNUM_C)
    mbedtls_mpi serial_mpi;

    mbedtls_mpi_init(&serial_mpi);
    TEST_EQUAL(mbedtls_mpi_read_binary(&serial_mpi, invalid_serial,
                                       sizeof(invalid_serial)), 0);
    TEST_EQUAL(mbedtls_x509write_crt_set_serial(&ctx, &serial_mpi),
               MBEDTLS_ERR_X509_BAD_INPUT_DATA);
#endif

    TEST_EQUAL(mbedtls_x509write_crt_set_serial_raw(&ctx, invalid_serial,
                                                    sizeof(invalid_serial)),
               MBEDTLS_ERR_X509_BAD_INPUT_DATA);

exit:
#if defined(MBEDTLS_TEST_DEPRECATED) && defined(MBEDTLS_BIGNUM_C)
    mbedtls_mpi_free(&serial_mpi);
#else
    ;
#endif
}

static void test_x509_set_serial_check_wrapper( void ** params )
{
    (void)params;

    test_x509_set_serial_check(  );
}
#endif /* MBEDTLS_X509_CRT_WRITE_C */
#if defined(MBEDTLS_X509_CREATE_C)
#if defined(MBEDTLS_X509_USE_C)
#line 627 "suites/test_suite_x509write.function"
static void test_mbedtls_x509_string_to_names(char *name, char *parsed_name, int result
                                  )
{
    int ret;
    size_t len = 0;
    mbedtls_asn1_named_data *names = NULL;
    mbedtls_x509_name parsed, *parsed_cur, *parsed_prv;
    unsigned char buf[1024], out[1024], *c;

    memset(&parsed, 0, sizeof(parsed));
    memset(out, 0, sizeof(out));
    memset(buf, 0, sizeof(buf));
    c = buf + sizeof(buf);

    ret = mbedtls_x509_string_to_names(&names, name);
    TEST_ASSERT(ret == result);

    if (ret != 0) {
        goto exit;
    }

    ret = mbedtls_x509_write_names(&c, buf, names);
    TEST_ASSERT(ret > 0);

    TEST_ASSERT(mbedtls_asn1_get_tag(&c, buf + sizeof(buf), &len,
                                     MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE) == 0);
    TEST_ASSERT(mbedtls_x509_get_name(&c, buf + sizeof(buf), &parsed) == 0);

    ret = mbedtls_x509_dn_gets((char *) out, sizeof(out), &parsed);
    TEST_ASSERT(ret > 0);

    TEST_ASSERT(strcmp((char *) out, parsed_name) == 0);

exit:
    mbedtls_asn1_free_named_data_list(&names);

    parsed_cur = parsed.next;
    while (parsed_cur != 0) {
        parsed_prv = parsed_cur;
        parsed_cur = parsed_cur->next;
        mbedtls_free(parsed_prv);
    }
}

static void test_mbedtls_x509_string_to_names_wrapper( void ** params )
{

    test_mbedtls_x509_string_to_names( (char *) params[0], (char *) params[1], *( (int *) params[2] ) );
}
#endif /* MBEDTLS_X509_USE_C */
#endif /* MBEDTLS_X509_CREATE_C */
#endif /* MBEDTLS_PK_PARSE_C */
#endif /* MBEDTLS_FS_IO */
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
    
#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_PK_PARSE_C)

        case 0:
            {
                *out_value = MBEDTLS_MD_SHA1;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_MD_SHA224;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_MD_SHA256;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_MD_SHA384;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_MD_SHA512;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_MD_MD5;
            }
            break;
        case 6:
            {
                *out_value = MBEDTLS_X509_KU_DIGITAL_SIGNATURE | MBEDTLS_X509_KU_NON_REPUDIATION | MBEDTLS_X509_KU_KEY_ENCIPHERMENT;
            }
            break;
        case 7:
            {
                *out_value = MBEDTLS_X509_KU_DIGITAL_SIGNATURE | MBEDTLS_X509_KU_NON_REPUDIATION;
            }
            break;
        case 8:
            {
                *out_value = MBEDTLS_X509_NS_CERT_TYPE_SSL_SERVER;
            }
            break;
        case 9:
            {
                *out_value = -1;
            }
            break;
        case 10:
            {
                *out_value = MBEDTLS_X509_CRT_VERSION_1;
            }
            break;
        case 11:
            {
                *out_value = MBEDTLS_ERR_X509_UNKNOWN_OID;
            }
            break;
        case 12:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_NAME;
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
    
#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_PK_PARSE_C)

        case 0:
            {
#if defined(MBEDTLS_HAS_ALG_SHA_1_VIA_MD_OR_PSA_BASED_ON_USE_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(MBEDTLS_RSA_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(MBEDTLS_PKCS1_V15)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 3:
            {
#if defined(MBEDTLS_HAS_ALG_SHA_224_VIA_MD_OR_PSA_BASED_ON_USE_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 4:
            {
#if defined(MBEDTLS_HAS_ALG_SHA_256_VIA_MD_OR_PSA_BASED_ON_USE_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 5:
            {
#if defined(MBEDTLS_HAS_ALG_SHA_384_VIA_MD_OR_PSA_BASED_ON_USE_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 6:
            {
#if defined(MBEDTLS_HAS_ALG_SHA_512_VIA_MD_OR_PSA_BASED_ON_USE_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 7:
            {
#if defined(MBEDTLS_HAS_ALG_MD5_VIA_MD_OR_PSA_BASED_ON_USE_PSA)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 8:
            {
#if defined(MBEDTLS_PK_CAN_ECDSA_SIGN)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 9:
            {
#if defined(MBEDTLS_ECDSA_DETERMINISTIC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 10:
            {
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 11:
            {
#if defined(MBEDTLS_DES_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 12:
            {
#if defined(MBEDTLS_CIPHER_MODE_CBC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 13:
            {
#if defined(MBEDTLS_USE_PSA_CRYPTO)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 14:
            {
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
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

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_PK_PARSE_C) && defined(MBEDTLS_PEM_WRITE_C) && defined(MBEDTLS_X509_CSR_WRITE_C)
    test_x509_csr_check_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_PK_PARSE_C) && defined(MBEDTLS_PEM_WRITE_C) && defined(MBEDTLS_X509_CSR_WRITE_C) && defined(MBEDTLS_USE_PSA_CRYPTO)
    test_x509_csr_check_opaque_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_PK_PARSE_C) && defined(MBEDTLS_PEM_WRITE_C) && defined(MBEDTLS_X509_CRT_WRITE_C) && defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_HAS_ALG_SHA_1_VIA_MD_OR_PSA_BASED_ON_USE_PSA)
    test_x509_crt_check_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_PK_PARSE_C) && defined(MBEDTLS_X509_CRT_WRITE_C)
    test_x509_set_serial_check_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_PK_PARSE_C) && defined(MBEDTLS_X509_CREATE_C) && defined(MBEDTLS_X509_USE_C)
    test_mbedtls_x509_string_to_names_wrapper,
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
    const char *default_filename = "./test_suite_x509write.datax";
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
