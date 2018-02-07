#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#ifdef SOURCE_TCS34725
#include "query_color.h"
#elif defined(SOURCE_SERIAL_SOUND)
#include "query_serial.h"
#endif




int create_message(unsigned char *msg, char light_id, uint16_t red, uint16_t green, uint16_t blue){

	// int lights = 1;

	// int message_length = 16 + (lights * 9);

	int COLOR_LENGTH = 19; // start of color data



#ifdef SOURCE_TCS34725
	if ( colorme(&red, &green, &blue) ){
		// if colorme returns an error, so should this
		return 1;
	}
#elif defined( SOURCE_SERIAL_SOUND)
	if ( serial_receive_colors(&red, &green, &blue) ){
		logm("error in serial_receive_colors");
		return 1;
	}
	printf(" ----> create_message received->Colors:\n\t\n\tRed:\t%" PRIu16 "\n\tGreen:\t%" PRIu16 "\n\tblue:\t%" PRIu16 "\n", red, green, blue);
#else
		mbedtls_printf("\n  . NO MESSAGE SOURCE WAS DEFINED....\n EXITING...\n\n");
		exit(1);
#endif

	msg[COLOR_LENGTH - 1] = (unsigned char)light_id;
	msg[COLOR_LENGTH + 0] = (unsigned char)(red >> 8);
	msg[COLOR_LENGTH + 1] = (unsigned char)(red & 0x00ff);
	msg[COLOR_LENGTH + 2] = (unsigned char)(green >> 8);
	msg[COLOR_LENGTH + 3] = (unsigned char)(green & 0x00ff);
	msg[COLOR_LENGTH + 4] = (unsigned char)(blue >> 8);
	msg[COLOR_LENGTH + 5] = (unsigned char)(blue & 0x00ff);

	return 0;
}

int hue_begin_stream(){
	char url[1024];
	mbedtls_snprintf(url, 1023, "curl -X PUT -H \"Content-Type: application/json\" -d \"{\\\"stream\\\": {\\\"active\\\": true}}\" \"http://%s/api/%s/groups/%s\"", SERVER_ADDR, HUB_USER, ENTERTAINMENT_GROUP);
	mbedtls_printf("\n  . Setting Entertainment mode on the hub for url %s", url);
	return system(url);
}