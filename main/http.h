

#ifndef HTTP_H
#define HTTP_H


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

// REQUEST TYPES
#define HTTP_GET "GET"
#define HTTP_POST "POST"
#define HTTP_PUT "PUT"

int http_request( char *host, int port, char *url, const char request_type[], char *content_type, char *data, size_t data_length, const char *find_success_str ) {
	int				   found = 0;
	int				   ret = 0, len, server_fd = 0;
	unsigned char	  buf[1024];
	struct sockaddr_in server_addr;
	struct hostent *   server_host;

	/*
     * Start the connection
     */
	printf( "\n  . Connecting to tcp/%s/%4d...", SERVER_NAME, port );
	fflush( stdout );

	if( ( server_host = gethostbyname( host ) ) == NULL ) {
		printf( " failed\n  ! gethostbyname failed\n\n" );
		goto exit;
	}

	if( ( server_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_IP ) ) < 0 ) {
		printf( " failed\n  ! socket returned %d\n\n", server_fd );
		goto exit;
	}

	memcpy( (void *)&server_addr.sin_addr,
			(void *)server_host->h_addr,
			server_host->h_length );

	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons( port );

	if( ( ret = connect( server_fd, (struct sockaddr *)&server_addr, sizeof( server_addr ) ) ) < 0 ) {
		printf( " failed\n  ! connect returned %d\n\n", ret );
		goto exit;
	}

	printf( " ok\n" );

	/*
     * Write the GET request
     */
	printf( "  > Write to server:" );
	fflush( stdout );


	/*
	 * Write takes place here
	 */
	len = sprintf( (char *)buf, "%s %s HTTP/1.0\r\nContent-Type: %s\r\nContent-Length: %i\r\n\r\n%s\r\n", request_type, url, content_type, data_length, data );

	while( ( ret = write( server_fd, buf, len ) ) <= 0 ) {
		if( ret != 0 ) {
			printf( " failed\n  ! write returned %d\n\n", ret );
			goto exit;
		}
	}

	len = ret;
	printf( " %d bytes written\n\n%s", len, (char *)buf );

	/*
     * Read the HTTP response
     */
	printf( "  < Read from server:" );
	fflush( stdout );
	do {
		len = sizeof( buf ) - 1;
		memset( buf, 0, sizeof( buf ) );
		ret = read( server_fd, buf, len );
		if( strstr( (char *)buf, find_success_str ) != NULL ) {
			found = 1;
		}

		if( ret <= 0 ) {
			printf( "\n %d bytes read ; page complete\n\n", ret );
			break;
		}

		len = ret;
		printf( "\n %d bytes read\n\n%s", len, (char *)buf );
	} while( 1 );

exit:
	close( server_fd );

	return ( found );
}
#endif