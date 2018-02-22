#ifndef MBED_H
#define MBED_H

#include "mbedtls/ssl.h"








int hue_mbed_open_dtls( );
void exit_close_hue_stream(void);
int hue_mbed_tx(mbedtls_ssl_context* ssl);



#endif
