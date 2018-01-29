

CFLAGS	?= -O2
WARNING_CFLAGS ?= -Wall -W -Wdeclaration-after-statement
LDFLAGS ?=

LOCAL_CFLAGS = $(WARNING_CFLAGS) -I . -D_FILE_OFFSET_BITS=64 -std=c99
LOCAL_LDFLAGS = -L./library 		\
		-lmbedtls$(SHARED_SUFFIX)	\
		-lmbedx509$(SHARED_SUFFIX)	\
		-lmbedcrypto$(SHARED_SUFFIX)


#gcc -lmbedtls -lmbedx509 -lmbedcrypto -B lib -I . -std=c11 main.c


#CC = gcc

main:
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) main.c $(LOCAL_LDFLAGS) $(LDFLAGS)
