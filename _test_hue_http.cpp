#include "http.c"



void main()
{
	http_request("/", HTTP_GET, "application/text", "hello");

}

