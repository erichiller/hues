// #ifdef __cplusplus
// extern "C" {
// #endif

#ifndef HTTP_H
#define HTTP_H


void log_http_init( );


int http_request( const char *host, int port, char *url, const char request_type[], const char *content_type, char *data, size_t data_length, const char *find_success_str );

#endif


// #ifdef __cplusplus
// }
// #endif