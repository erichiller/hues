#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#pragma message "Using config from preprocessor macro MBEDTLS_CONFIG_FILE -> " MBEDTLS_CONFIG_FILE
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf     printf
#define mbedtls_fprintf    fprintf
#endif

