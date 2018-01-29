

#include <stdio.h>
#include <stdlib.h>

#include "query_color.h"



int create_message(char *msg, char light_id, uint16_t red, uint16_t green, uint16_t blue){

	// int lights = 1;

	// int message_length = 16 + (lights * 9);

	int COLOR_LENGTH = 19; // start of color data

	colorme(&red, &green, &blue);

	msg[COLOR_LENGTH - 1] = light_id;
	msg[COLOR_LENGTH + 0] = red >> 8;
	msg[COLOR_LENGTH + 1] = red & 0x00ff;
	msg[COLOR_LENGTH + 2] = green >> 8;
	msg[COLOR_LENGTH + 3] = green & 0x00ff;
	msg[COLOR_LENGTH + 4] = blue >> 8;
	msg[COLOR_LENGTH + 5] = blue & 0x00ff;

	return 0;
}
