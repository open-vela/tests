ifneq ($(CONFIG_CA_TEST),)
PROGNAME += secure_ca_alipay_test
MAINSRC += $(CURDIR)/secure_test/api/AlipayApiTest.c

PROGNAME += secure_ca_triad_test
MAINSRC += $(CURDIR)/secure_test/api/TriadApiTest.c

PROGNAME += secure_ca_wxcodepay_test
MAINSRC += $(CURDIR)/secure_test/api/WxcodepayApiTest.c

PROGNAME += secure_ca_pin_test
MAINSRC += $(CURDIR)/secure_test/api/PinApiTest.c

endif

ifneq ($(CONFIG_KEYSTORE_TEST),)
PROGNAME += keystore_test_insert
MAINSRC += $(CURDIR)/secure_test/keystore/keystore_insert_test.c

PROGNAME += keystore_test_get
MAINSRC += $(CURDIR)/secure_test/keystore/keystore_get_test.c

PROGNAME += keystore_test_multi
MAINSRC += $(CURDIR)/secure_test/keystore/keystore_insert_multi.c

PROGNAME += keystore_test_block
MAINSRC += $(CURDIR)//secure_test/keystore/keystore_block_test.c
CFLAGS += $(APPDIR)/../external/android/system/security/keystore/include

endif

ifneq ($(CONFIG_MBEDTLS_TEST),)

ORIGS_M  := $(wildcard $(CURDIR)/secure_test/mbedtls/*.c)
MAINWORDS = "main("
$(foreach word, $(MAINWORDS), $(eval MAINSRC_M+=$(shell grep -lr $(word) $(ORIGS_M))))
MAINSRC += $(MAINSRC_M)
PROGNAME += $(basename $(shell echo $(MAINSRC_M) | xargs -n 1 | awk -F "[/]" '{print "mbedtls_"$$(NF)}' | sed 's/\./_/g'))

STACKSIZE += 10240
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/crypto/mbedtls/mbedtls/tests/include
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/crypto/mbedtls/mbedtls/library
CSRCS += $(APPDIR)/crypto/mbedtls/mbedtls/tests/src/test_helpers/ssl_helpers.c
CSRCS += $(wildcard $(APPDIR)/crypto/mbedtls/mbedtls/tests/src/*.c)
endif
