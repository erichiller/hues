

#include <stdio.h>
#include <stdlib.h>



char *create_message(int time) {
	// number of lights in message
	int lights = 1;
	/*
	* 16 Base bytes
	* + (per light) {
	*     3 bytes for light id
	*     2 bytes per color, RGB
	* }
	* @ 1 = 25 bytes
	*/

	int message_length = 16 + (lights * 9);
	mbedtls_printf("\n  . ERIC ----> message_length=%i", message_length);
	char *msg = malloc( (sizeof(char) * message_length ) + 1);


	char template_msg[] = {
		'H', 'u', 'e', 'S', 't', 'r', 'e', 'a', 'm', //protocol
		0x01, 0x00, //version 1.0
		0x01, //sequence number 1 (not observed)
		0x00, 0x00, //reserved
		0x00, //color mode RGB
		0x00, //reserved
			  //// light command #1 (up to 10)
			  0x00, 0x00, 0x0d, //light 13
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // color
	};
	mbedtls_printf("\n  . ERIC ----> template_msg sizeof=%i", sizeof(template_msg));

	/* create message */

	//size_t destination_size = sizeof(char) * message_length;

	//strncpy_s(msg, destination_size, template_msg, destination_size);
	//array2[destination_size - 1] = '\0';
	//strcpy()

	int COLOR_LENGTH = 19; // start of color data
	
	for (int i = 0; i < COLOR_LENGTH; i++) {
		msg[i] = template_msg[i];
	}

	//msg[19] = rand(0xff);

	msg[19] = rand(0xee);
	msg[20] = 0x00;
	msg[21] = rand(0xca);
	msg[22] = 0x00;
	msg[23] = rand(0xfa);
	msg[24] = 0x00;
	
	// red
	if (time % 3 == 0) {
		msg[19] = rand(25) * 10;
		//msg[20] = 0xff;
	}
	// green
	if (time % 2 == 0) {
		msg[21] = rand(25) * 10;
		//msg[22] = 0xff;
	}
	// blue
	if (time % 4 == 0) {
		msg[23] = rand(25) * 10;
		//msg[24] = 0xff;
	}
	

	
	
	msg[message_length] = '\0';


	mbedtls_printf("\n  . ERIC ----> sizeof(msg)=%i ;; msg=%s (%p)", sizeof(msg), msg, msg);
	mbedtls_printf("\n  . ERIC ----> sizeof(*msg)=%i ;; msg@%c (%X)", sizeof(*msg), *msg, *msg);
	mbedtls_printf("\n  . ERIC ----> sizeof(char) *message_length + 1=%i", (sizeof(char) * message_length) + 1);


	return msg;
}