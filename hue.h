#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "query_color.h"



int create_message(char *msg, char light_id, uint16_t red, uint16_t green, uint16_t blue){

	// int lights = 1;

	// int message_length = 16 + (lights * 9);

	int COLOR_LENGTH = 19; // start of color data

	if ( colorme(&red, &green, &blue) ){
		// if colorme returns an error, so should this
		return 1;
	}

	msg[COLOR_LENGTH - 1] = light_id;
	msg[COLOR_LENGTH + 0] = red >> 8;
	msg[COLOR_LENGTH + 1] = red & 0x00ff;
	msg[COLOR_LENGTH + 2] = green >> 8;
	msg[COLOR_LENGTH + 3] = green & 0x00ff;
	msg[COLOR_LENGTH + 4] = blue >> 8;
	msg[COLOR_LENGTH + 5] = blue & 0x00ff;

	return 0;
}

int hue_begin_stream(){
	char url[1024];
	mbedtls_snprintf(url, 1023, "curl -X PUT -H \"Content-Type: application/json\" -d \"{\\\"stream\\\": {\\\"active\\\": true}}\" \"http://%s/api/%s/groups/%s\"", SERVER_ADDR, HUB_USER, ENTERTAINMENT_GROUP);
	mbedtls_printf("\n  . Setting Entertainment mode on the hub for url %s", url);
	return system(url);
}