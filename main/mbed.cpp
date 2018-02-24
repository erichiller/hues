/*
 *  Hue DTLS adopted from mbed simples DTLS client
 **/


#define MBEDTLS_NET_C
#define MBEDTLS_TIMING_ALT

#include <unistd.h>

#include "config.h"
#include <stdint.h>
#include <string.h>
#include "mbedtls/ssl.h"
#include "esp_log.h"
#include "sys/time.h"

#include "hue.h"

#define LOGT "MBEDeh"


#if !defined( MBEDTLS_CONFIG_FILE )
#	include "mbedtls/config.h"
#else
// #	pragma message "Using config from preprocessor macro MBEDTLS_CONFIG_FILE -> " MBEDTLS_CONFIG_FILE
// #	include MBEDTLS_CONFIG_FILE
#endif

#if defined( MBEDTLS_PLATFORM_C )
#	include "mbedtls/platform.h"
#else
#	include <stdio.h>
#	define mbedtls_printf printf
#	define mbedtls_fprintf fprintf
#endif

#if !defined( MBEDTLS_SSL_CLI_C )
#	pragma message( "!! ERROR !! not defined: MBEDTLS_SSL_CLI_C " )
#	define MBED_MISSING_DEFINE
#endif
#if !defined( MBEDTLS_SSL_PROTO_DTLS )
#	pragma message( "!! ERROR !! not defined: MBEDTLS_SSL_PROTO_DTLS " )
#	define MBED_MISSING_DEFINE
#endif
#if !defined( MBEDTLS_NET_C )
#	pragma message( "!! ERROR !! not defined: MBEDTLS_NET_C " )
#	define MBED_MISSING_DEFINE
#endif
#if !defined( MBEDTLS_TIMING_C ) && !defined( MBEDTLS_TIMING_ALT )
#	pragma message( "!! ERROR !! not defined: MBEDTLS_TIMING_C " )
#	define MBED_MISSING_DEFINE
#endif
#if !defined( MBEDTLS_ENTROPY_C )
#	pragma message( "!! ERROR !! not defined: MBEDTLS_ENTROPY_C " )
#	define MBED_MISSING_DEFINE
#endif
#if !defined( MBEDTLS_CTR_DRBG_C )
#	pragma message( "!! ERROR !! not defined: MBEDTLS_CTR_DRBG_C " )
#	define MBED_MISSING_DEFINE
#endif
#if !defined( MBEDTLS_X509_CRT_PARSE_C )
#	pragma message( "!! ERROR !! not defined: MBEDTLS_X509_CRT_PARSE_C " )
#	define MBED_MISSING_DEFINE
#endif
#if !defined( MBEDTLS_RSA_C )
#	pragma message( "!! ERROR !! not defined: MBEDTLS_RSA_C " )
#	define MBED_MISSING_DEFINE
#endif
#if !defined( MBEDTLS_CERTS_C )
#	pragma message( "!! ERROR !! not defined: MBEDTLS_CERTS_C " )
#	define MBED_MISSING_DEFINE
#endif
#if !defined( MBEDTLS_PEM_PARSE_C )
#	pragma message( "!! ERROR !! not defined: MBEDTLS_PEM_PARSE_C " )
#	define MBED_MISSING_DEFINE
#endif
#if !defined( MBEDTLS_TLS_PSK_WITH_AES_128_GCM_SHA256 )
#	pragma message( "!! ERROR !! not defined: MBEDTLS_PEM_PARSE_C " )
#	define MBED_MISSING_DEFINE
#endif
/******
 *  SUM OF ABOVE ERRORS -- missing includes 
 *****/
#if defined( MBED_MISSING_DEFINE )
#	pragma message( "!! FATAL !! mbed missing critical" )
#else
#	pragma message( " BUILDING MBED " )


#	include "mbedtls/net_sockets.h"
#	include "mbedtls/debug.h"
#	include "mbedtls/entropy.h"
#	include "mbedtls/ctr_drbg.h"
#	include "mbedtls/error.h"
#	include "mbedtls/certs.h"
#	include "timing_alt.h"

	/* Last order of business: CHECK CONFIG VALIDITY! */
#	include "mbedtls/check_config.h"



int hue_mbed_tx( mbedtls_ssl_context *ssl ) {

	int			  ret, len, rx;

	unsigned char message_template[] = {
		'H', 'u', 'e', 'S', 't', 'r', 'e', 'a', 'm',	// protocol (9)
		0x01,
		0x00,	// version 1.0 (2)
		0x01,	// sequence number 1 (not observed) (1)
		0x00,
		0x00,	// reserved (2)
		0x00,	// color mode RGB (1)
		0x00,	// reserved (1)
		//// light command #1 (up to 10)
		0x00,
		0x00,
		0x0d,	// light 13
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00	// color
	};
	unsigned char *message = (unsigned char *)&message_template;
	while( 1 ) {
		// message_time(i, &message);
		uint16_t red   = 0xaaff;
		uint16_t green = 0x1234;
		uint16_t blue  = 0x5678;

		rx = create_message( message, LIGHT_ID, red, green, blue );
		if( rx <= 0 ) {
			// create message returned an error
			ESP_LOGE( LOGT, " create message returned an error\n Exiting\n\n" );
			exit( 1 );
		}
		// len = sizeof(message);
		len = 25;

		for( int j = 0; j < len; j++ ) {
			mbedtls_printf( "%02X ", message[j] );
		}

		do {
			ret = mbedtls_ssl_write( ssl, (unsigned char *)message, len );
		} while( ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE );

		if( ret < 0 ) {
			ESP_LOGE( LOGT, " failed\n  ! mbedtls_ssl_write returned %d\n\n", ret );
			return ret;
		}
		mbedtls_printf( "<----MESSAGE-- (hex) \n" );

		usleep( SLEEP_PERIOD );


	}
	return 0;
}

void exit_close_hue_stream( void ) {
	if( hue_end_stream( ) ) {
		ESP_LOGD(LOGT, "closed hue stream successfully" );
	} else {
		ESP_LOGW(LOGT, "failed to close hue stream" )
	}
}

static void my_debug( void *ctx, int level, const char *file, int line, const char *str ) {
	( (void)level );

	mbedtls_fprintf( (FILE *)ctx, "%s:%04d: %s", file, line, str );
	//fflush( (FILE *)ctx );
}




int hue_mbed_open_dtls( ) {
	ESP_LOGD( LOGT, "\n  . Beginning Mbed..." );
	ESP_LOGD( LOGT, "\n  . ERIC ----> check on MBEDTLS_NET_PROTO_UDP: %i",
					MBEDTLS_NET_PROTO_UDP );
	ESP_LOGD( LOGT,
		"\n  . ERIC ----> check on MBEDTLS_TLS_PSK_WITH_AES_128_GCM_SHA256: %X",
		MBEDTLS_TLS_PSK_WITH_AES_128_GCM_SHA256 );

	// close hue at exit
	atexit( exit_close_hue_stream );

	int					ret, len;
	mbedtls_net_context server_fd;
	uint32_t			flags;
	unsigned char		buf[1024];
	const char *		pers	   = "dtls_client";
	int					retry_left = MAX_RETRY;

	/* DIFFERENCE HERE */
	const char *psk_identity;
	/* 16 length */
	/*  BA    2A    27    18    85    63    10    B1    BF    F8    46    A2    0D
    * 56    3C    8C */
	const unsigned char psk[] = {0xba, 0x2a, 0x27, 0x18, 0x85, 0x63, 0x10, 0xb1, 0xbf, 0xf8, 0x46, 0xa2, 0x0d, 0x56, 0x3c, 0x8c};
	int					force_ciphersuite[2];

	mbedtls_entropy_context		 entropy;
	mbedtls_ctr_drbg_context	 ctr_drbg;
	mbedtls_ssl_context			 ssl;
	mbedtls_ssl_config			 conf;
	mbedtls_x509_crt			 cacert;
	mbedtls_timing_delay_context delay_timer;

#	if defined( MBEDTLS_DEBUG_C )
	mbedtls_debug_set_threshold( 0 );
#	endif

	psk_identity		 = DFL_PSK_IDENTITY;
	force_ciphersuite[0] = MBEDTLS_TLS_PSK_WITH_AES_128_GCM_SHA256;
	force_ciphersuite[1] = 0;

	/*
    * set hue in the right mode
    */

	hue_begin_stream( );

	/*
    * 0. Initialize the RNG and the session data
    */
	mbedtls_net_init( &server_fd );
	mbedtls_ssl_init( &ssl );
	mbedtls_ssl_config_init( &conf );
	mbedtls_x509_crt_init( &cacert );
	mbedtls_ctr_drbg_init( &ctr_drbg );

	mbedtls_printf( "\n  . Seeding the random number generator..." );
	//fflush( stdout );

	mbedtls_entropy_init( &entropy );
	if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)pers, strlen( pers ) ) ) != 0 ) {
		ESP_LOGE( LOGT, " failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret );
		goto exit;
	}

	mbedtls_printf( " ok\n" );

	/*
    * 0. Load certificates
    */

	mbedtls_printf( "  . Loading the CA root certificate ..." );
	//fflush( stdout );

	ret = mbedtls_x509_crt_parse( &cacert,
								  (const unsigned char *)mbedtls_test_cas_pem,
								  mbedtls_test_cas_pem_len );
	if( ret < 0 ) {
		mbedtls_printf( " failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n",
						-ret );
		goto exit;
	}

	mbedtls_printf( " ok (%d skipped)\n", ret );

	/*
    * 1. Start the connection
    */
	mbedtls_printf( "  . Connecting to udp/%s/%s...", SERVER_NAME, SERVER_PORT );
	//fflush( stdout );

	if( ( ret = mbedtls_net_connect( &server_fd, SERVER_ADDR, SERVER_PORT, MBEDTLS_NET_PROTO_UDP ) ) != 0 ) {
		mbedtls_printf( " failed\n  ! mbedtls_net_connect returned %d\n\n", ret );
		goto exit;
	}

	mbedtls_printf( " ok\n" );

	/*
    * 2. Setup stuff
    */
	mbedtls_printf( "  . Setting up the DTLS structure..." );
	//fflush( stdout );

	if( ( ret = mbedtls_ssl_config_defaults( &conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_DATAGRAM, MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 ) {
		mbedtls_printf( " failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n",
						ret );
		goto exit;
	}

	mbedtls_ssl_conf_transport( &conf, MBEDTLS_SSL_TRANSPORT_DATAGRAM );

	/* OPTIONAL is usually a bad choice for security, but makes interop easier
    * in this simplified example, in which the ca chain is hardcoded.
    * Production code should set a proper ca chain and use REQUIRED. */
	mbedtls_ssl_conf_authmode( &conf, MBEDTLS_SSL_VERIFY_OPTIONAL );
	mbedtls_ssl_conf_ca_chain( &conf, &cacert, NULL );
	mbedtls_ssl_conf_rng( &conf, mbedtls_ctr_drbg_random, &ctr_drbg );
	mbedtls_ssl_conf_dbg( &conf, my_debug, stdout );

	if( ( ret = mbedtls_ssl_setup( &ssl, &conf ) ) != 0 ) {
		mbedtls_printf( " failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret );
		goto exit;
	}

	if( ( ret = mbedtls_ssl_conf_psk( &conf, psk, sizeof( psk ), (const unsigned char *)psk_identity, strlen( psk_identity ) ) ) != 0 ) {
		mbedtls_printf( " failed\n  ! mbedtls_ssl_conf_psk returned %d\n\n", ret );
		goto exit;
	}

	mbedtls_ssl_conf_ciphersuites( &conf, force_ciphersuite );

	if( ( ret = mbedtls_ssl_set_hostname( &ssl, SERVER_NAME ) ) != 0 ) {
		mbedtls_printf( " failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n",
						ret );
		goto exit;
	}

	mbedtls_ssl_set_bio( &ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout );

	mbedtls_ssl_set_timer_cb( &ssl, &delay_timer, mbedtls_timing_set_delay, &mbedtls_timing_get_delay );

	mbedtls_ssl_conf_handshake_timeout( &conf, 100, 60000 );

	mbedtls_printf( " ok\n" );

	/*
    * 4. Handshake
    */

	mbedtls_printf( "  . Performing the SSL/TLS handshake..." );
	//fflush( stdout );

	do
		ret = mbedtls_ssl_handshake( &ssl );
	while( ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE );

	if( ret != 0 ) {
		mbedtls_printf( " failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n",
						-ret );
		goto exit;
	}

	mbedtls_printf( " ok\n" );

	/*
    * 5. Verify the server certificate
    */
	mbedtls_printf( "  . Verifying peer X.509 certificate..." );

	/* In real life, we would have used MBEDTLS_SSL_VERIFY_REQUIRED so that the
    * handshake would not succeed if the peer's cert is bad.  Even if we used
    * MBEDTLS_SSL_VERIFY_OPTIONAL, we would bail out here if ret != 0 */
	if( ( flags = mbedtls_ssl_get_verify_result( &ssl ) ) != 0 ) {
		char vrfy_buf[512];

		ESP_LOGE( LOGT, " failed\n" );

		mbedtls_x509_crt_verify_info( vrfy_buf, sizeof( vrfy_buf ), "  ! ", flags );

		mbedtls_printf( "%s\n", vrfy_buf );
	} else
		mbedtls_printf( " ok\n" );

	/*
    * 6. Write the echo request
    */
send_request:
	mbedtls_printf( "  > Write to server:" );
	//fflush( stdout );

	// create message space
	// char *mmessage = malloc(25 * sizeof(char));

	// if( !loop( &ssl ) ) {
	// 	goto exit;
	// }

	/*
    * 7. Read the echo response
    */
	mbedtls_printf( "  < Read from server:" );
	//fflush( stdout );

	len = sizeof( buf ) - 1;
	memset( buf, 0, sizeof( buf ) );

	do
		ret = mbedtls_ssl_read( &ssl, buf, len );
	while( ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE );

	if( ret <= 0 ) {
		switch( ret ) {
			case MBEDTLS_ERR_SSL_TIMEOUT:
				mbedtls_printf( " timeout\n\n" );
				if( retry_left-- > 0 )
					goto send_request;
				goto exit;

			case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
				mbedtls_printf( " connection was closed gracefully\n" );
				ret = 0;
				goto close_notify;

			default:
				mbedtls_printf( " mbedtls_ssl_read returned -0x%x\n\n", -ret );
				goto exit;
		}
	}

	len = ret;
	mbedtls_printf( " %d bytes read\n\n%s\n\n", len, buf );

	/*
    * 8. Done, cleanly close the connection
    */
close_notify:
	mbedtls_printf( "  . Closing the connection..." );

	/* No error checking, the connection might be closed already */
	do
		ret = mbedtls_ssl_close_notify( &ssl );
	while( ret == MBEDTLS_ERR_SSL_WANT_WRITE );
	ret = 0;

	mbedtls_printf( " done\n" );

	/*
    * 9. Final clean-ups and exit
    */
exit:

#	ifdef MBEDTLS_ERROR_C
	if( ret != 0 ) {
		char error_buf[100];
		mbedtls_strerror( ret, error_buf, 100 );
		mbedtls_printf( "Last error was: %d - %s\n\n", ret, error_buf );
	}
#	endif

	mbedtls_net_free( &server_fd );

	mbedtls_x509_crt_free( &cacert );
	mbedtls_ssl_free( &ssl );
	mbedtls_ssl_config_free( &conf );
	mbedtls_ctr_drbg_free( &ctr_drbg );
	mbedtls_entropy_free( &entropy );

#	if defined( _WIN32 )
	mbedtls_printf( "  + Press Enter to exit this program.\n" );
	//fflush( stdout );
	getchar( );
#	endif

	/* Shell can not handle large exit numbers -> 1 for errors */
	if( ret < 0 )
		ret = 1;

	return ( ret );
}

#undef LOGT

#endif