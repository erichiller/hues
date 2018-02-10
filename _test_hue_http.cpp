#include "http.c"
#include "config.h"


void main()
{
	char data[] = "{\"stream\": {\"active\": true}}";
	char url[1024];
	mbedtls_snprintf(url, 1023, "/api/%s/groups/%s", HUB_USER, ENTERTAINMENT_GROUP);	
	http_request(SERVER_ADDR, 80, url, HTTP_PUT, "application/json", (char*)data , sizeof(data));
}

