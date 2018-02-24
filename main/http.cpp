#include <sys/types.h>
/* was winsock */
#include <lwip/sockets.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
/* end of non-windows */

#include <string.h>
#include <stdio.h>
#include "config.h"
#include "esp_log.h"


#define LOGT LOG_TAG_HTTP


void log_http_init( ) {
	esp_log_level_set( LOG_TAG_HTTP, LOG_TAG_HTTP_LEVEL );
}



int http_request( const char *host, int port, char *url, const char request_type[], const char *content_type, char *data, size_t data_length, const char *find_success_str ) {
	int				   found			= 0;
	int				   count_len = 0, ret = 0, len = 0, server_fd = 0;
	int				   addr_len = 0;
	unsigned char	  buf[1024];
	struct sockaddr_in server_addr;
	struct hostent *   server_host;

	// the null at the end of the data should not count towards its length
	data_length -= 1;

	/*
     * Start the connection
     */
	printf( "\n  . Connecting to tcp/%s:%d...", host, port );
	//fflush( stdout );

	if( ( server_host = gethostbyname( host ) ) == NULL ) {
		printf( " failed\n  ! gethostbyname failed\n\n" );
		goto exit;
	}
	printf( "\n  . Found host..." );


	if( ( server_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_IP ) ) < 0 ) {
		printf( " failed\n  ! socket returned %d\n\n", server_fd );
		goto exit;
	}
	printf( "\n  . Created handle for host..." );


	if( server_host->h_addrtype == AF_INET ) {
		addr_len = 4;
	} else if( server_host->h_addrtype == AF_INET6 ) {
		//addr_len = 16
		// ipv6 unsupported for the moment
		ESP_LOGE( LOGT, "IPv6 not currently supported in httpc" )
	} else {
		ESP_LOGE( LOGT, "Unkown network type." )
	}

	ESP_LOGD( LOGT, " \nCOPY server ADDR (name=%s) (h_length=%i) (length=%i) (type=%i)from HOST ", server_host->h_name, server_host->h_length, addr_len, server_host->h_addrtype );

	ESP_LOG_BUFFER_HEX_LEVEL( LOGT, server_host->h_addr, addr_len, ESP_LOG_DEBUG );

	memcpy( (void *)&server_addr.sin_addr,
			(void *)server_host->h_addr,
			addr_len );

	ESP_LOGD( LOGT, "\n ...COPIED INTO server ADDR:" );

	ESP_LOG_BUFFER_HEX_LEVEL( LOGT, &server_addr.sin_addr, addr_len, ESP_LOG_DEBUG );

	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons( port );

	if( ( ret = connect( server_fd, (struct sockaddr *)&server_addr, sizeof( server_addr ) ) ) < 0 ) {
		printf( " failed\n  ! connect returned %d\n\n", ret );
		goto exit;
	}

	printf( " \n  . Connection ok" );

	/*
     * Write the GET request
     */
	printf( "  > Writing to server...\n" );
	//fflush( stdout );



	/*
	 * Write takes place here
	 */
	len = sprintf( (char *)buf, 
						"%s %s HTTP/1.1\r\n"
						"Content-Type: %s\r\n"
						"Content-Length: %i\r\n"
						"\r\n"
						"%s"
						, request_type, url, content_type, data_length, data );

	while( ( ret = write( server_fd, buf, len ) ) <= 0 ) {
		if( ret != 0 ) {
			ESP_LOGE( LOGT, " failed\n  ! write returned %d\n\n", ret );
			goto exit;
		}
		ESP_LOGV(LOGT, "write return=%i and errno=%i\n", ret, errno);
		count_len += ret;
	}
	ESP_LOGD(LOGT, "write(...); last return=%i and errno=%i\n", ret, errno);

	ESP_LOGD( LOGT, "**** START RESULT ****\n"
				"Result of sending string: \n"
				"\t Actual string length:\t %i long string\n"
				"\t Bytes written: %d \n"
				"\t Data String:\n"
					"\t\tlength:%i\n"
					"\t\tstring next line:\n"
				" ---------- DATA START ---------- \n"					
				"%s"
				"\n ----------- DATA END ----------- \n"									
				"\n"
				"\t String (full message, next line):\n"
				" ---------- MESSAGE START ---------- \n"
				"%s"
				"\n ----------- MESSAGE END ----------- \n"
				"**** END RESULT ****\n"
				"\n", len, count_len, data_length, data, (char *)buf );

	/*
     * Read the HTTP response
     */
	printf( "  << Reading from server:\n" );
	//fflush( stdout );


	struct timeval receiving_timeout;
	receiving_timeout.tv_sec  = 5;
	receiving_timeout.tv_usec = 0;
	if( setsockopt( server_fd, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout, sizeof( receiving_timeout ) ) < 0 ) {
		ESP_LOGE( LOGT, "\n !! FAILED to set socket receiving timeout !! \n" );
		goto exit;
	}
	ESP_LOGD( LOGT ," . set socket receiving timeout success\n" );


	/* Read HTTP response */
	// do {
	// 	ret = read( server_fd, buf, sizeof( buf ) - 1 );
	// 	for( int i = 0; i < ret; i++ ) {
	// 		putchar( buf[i] );
	// 	}
	// } while( ret > 0 );
	// printf( " . done reading from socket. Last read return=%d errno=%d\r\n", ret, errno );
	// 

	len = 0;

	do {
		memset( buf, 0, sizeof( buf ) );
		ret = read( server_fd, buf, sizeof( buf ) - 1 );
		len += ret;
		for( int i = 0; i < ret; i++ ) {
			putchar( buf[i] );
		}

		if( strstr( (char *)buf, find_success_str ) != NULL ) {
			found = 1;
		}

		if( ret == 0 ) {
			printf( "\n %d bytes read ; %d total bytes read ; page complete\n\n", ret, len );
			break;
		} else if ( ret == -1 ){
			ESP_LOGE( LOGT, "read() in httpc returned -1" );
			found = false;
		}

		printf( "\n %d bytes read\n"
				"%d total bytes read\n"
				"---------- MESSAGE READ ----------\n"
				"%s\n"
				"-------- MESSAGE READ END --------\n"
				, ret, len, (char *)buf );
	} while( ret > 0 );
	printf( " . done reading from socket. Last read return=%d errno=%d\r\n", ret, errno );

exit:
	close( server_fd );

	return ( found );
}

#undef LOGT