//This code snippet will help you to read data from arduino

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "config.h"
#include "SerialPort.h"

#define SERIAL_PORT_SLEEP 1000

using std::cout;
using std::endl;

/*Portname must contain these backslashes, and remember to
replace the following com port*/
char *port_name = "\\\\.\\COM7";

//String for incoming data
char incomingData[MAX_DATA_LENGTH];

SerialPort arduino(port_name);

static bool
str_to_uint16(const char *str, uint16_t *res)
{
  char *end;
  errno = 0;
unsigned long val = strtoul(str, &end, 10);

//   intmax_t val = strtoimax(str, &end, 10);
  if (errno == ERANGE || val < 0 || val > UINT16_MAX || end == str || *end != '\0') { return false; }
  *res = (uint16_t) val;

  return true;
}

// parses incomingData from serial received data and returns number of tokens found
int token_parser(uint16_t *red, uint16_t *green, uint16_t *blue ){
	char rx[MAX_DATA_LENGTH];	

	strcpy( rx, incomingData );
#ifdef DEBUG
	printf(incomingData);
#endif

	char *rx_str_state;
	char *color_state;
	int counter_tokens = 0;
	int counter_errors_non_numeric = 0;
	int counter_errors_incomplete_colors = 0;

	char *token = strtok_s(rx, ";", &rx_str_state);
	while( token != NULL){
		// printf("token->%s\n",token);
		if ( !isdigit(token[0] ) ) {
			counter_errors_non_numeric++; 
			// if the token isn't a digit, log an error and continue
			// printf("non digit found in token: ");
			// puts(token);
		} else {
#ifdef DEBUG
			printf("current working token=<%s>\n", token);
#endif
			char *color_component = strtok_s(token, ",", &color_state);
			int color = 0;
			while( color_component != NULL ){
				color++;
				if ( color == 1 ) { 
					str_to_uint16(color_component, red);
#ifdef DEBUG
					printf("<<color_component [%s] (  red  ) is now int=%" PRIu16 ">>\n", color_component, *red);
#endif
				}
				if ( color == 2 ) {
					str_to_uint16(color_component, green);
#ifdef DEBUG
					printf("<<color_component [%s] ( green ) is now int=%" PRIu16 ">>\n", color_component, *green);
#endif
				}
				if ( color == 3 ) {
					str_to_uint16(color_component, blue);
#ifdef DEBUG
					printf("<<color_component [%s] (  blue ) is now int=%" PRIu16 ">>\n", color_component, *blue);
#endif
				}

				color_component = strtok_s(NULL, ",", &color_state);
			}
			// less then 3 colors were seen - error! probably the message stopped halfway through, it could be reconstructed by concat the next one, but meh.
			if ( color == 3 ) { counter_tokens++; } else { counter_errors_incomplete_colors++; }
#ifdef DEBUG
			printf("Colors:\n\t\n\tRed:\t%" PRIu16 "\n\tGreen:\t%" PRIu16 "\n\tblue:\t%" PRIu16 "\n", *red, *green, *blue);
#endif
		}
		token = strtok_s(NULL, ";", &rx_str_state);    
	}
#ifdef DEBUG
	printf("Parsed %i tokens\n",counter_tokens);
#endif
	*incomingData = NULL;

	return counter_tokens;
}

// return the number of colors received
uint32_t serial_receive_colors(uint16_t *red, uint16_t *green, uint16_t *blue ){
	// SerialPort arduino(port_name);
	if (arduino.isConnected()) cout << "Connection Established" << endl;
	else cout << "ERROR, check port name";
	
	int counter_rx = 0;
	while (arduino.isConnected()){
		//Check if data has been read or not
		int read_result = arduino.readSerialPort(incomingData, MAX_DATA_LENGTH);
		//prints out data
		// puts(incomingData);

/** 
// DEBUG HERE ,,, UNCOMMENT THIS
		if (counter_rx >= 5){
			printf("receved 5\n");
			exit(0);
		} else {
			printf("counter_rx=%i\n",counter_rx);
		}
 **/
		int rx = token_parser(red, green, blue);
		counter_rx += rx;
		if( rx ) return 0;

#ifdef SERIAL_PORT_SLEEP
		Sleep(SERIAL_PORT_SLEEP);
#endif
	}
	return 1;
}

int serial_send_data(char *buffer,  unsigned int buf_size){
	if (arduino.isConnected()) cout << "Connection Ok, Data Sending..." << endl;
	else cout << "ERROR, check port name";

	int bytesSent = arduino.writeSerialPort(buffer, buf_size);
	// check if writing was successfull
	if( bytesSent != 0){
		printf("Data Sent. (%i bytes)\n", bytesSent);
		return 0;
	}
	else cout << "Error while sending data" << endl; 	return 1;

}

uint32_t serial_receive_data(){
	// SerialPort arduino(port_name);
	if (arduino.isConnected()) cout << "Connection Established" << endl;
	else cout << "ERROR, check port name";
	
	int counter_rx = 0;
	while (arduino.isConnected()){
		//Check if data has been read or not
		int read_result = arduino.readSerialPort(incomingData, MAX_DATA_LENGTH);
		//prints out data
		puts(incomingData);

		if (counter_rx > 5){
			printf("receved 5\n");
			exit(0);
		} else {
			printf("counter_rx=%i\n",counter_rx);
		}
		counter_rx++;
		// uint16_t *red = NULL;
		// uint16_t *green = NULL;
		// uint16_t *blue = NULL;
		// // printf("sizeRxD=%i", sizeof(incomingData));
		// if( token_parser(red, green, blue) ){
		// 	return 0;
		// }
		//wait a bit
#ifdef SERIAL_PORT_SLEEP
		Sleep(SERIAL_PORT_SLEEP);
#endif
	}
	return 1;
}

/**
 *  F:\Users\ehiller\dev\src\github.com\erichiller\hues\
 **/



