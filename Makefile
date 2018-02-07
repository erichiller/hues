

CFLAGS	?= -O2 -fdiagnostics-color=always -g 
WARNING_CFLAGS ?= -Wall -W
# -Wdeclaration-after-statement
LDFLAGS ?=
DEBUG_CFLAGS ?=

LOCAL_CFLAGS = $(WARNING_CFLAGS) -I . -D_FILE_OFFSET_BITS=64 -o gohuesgocs
# -std=c99
LOCAL_LDFLAGS = -L./library 		\
		-lmbedtls$(SHARED_SUFFIX)	\
		-lmbedx509$(SHARED_SUFFIX)	\
		-lmbedcrypto$(SHARED_SUFFIX)\
		-lwiringPi -lm



main:
	$(CC) $(LOCAL_CFLAGS) $(DEBUG_CFLAGS) $(CFLAGS) _hues.cpp WireShim.cpp Adafruit_TCS34725/Adafruit_TCS34725.cpp $(LOCAL_LDFLAGS) $(LDFLAGS)
