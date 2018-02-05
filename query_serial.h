//This code snippet will help you to read data from arduino

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// parses incomingData from serial received data and returns number of tokens found
int token_parser(){
	char rx[MAX_DATA_LENGTH];	

	strcpy( rx, incomingData );
	printf(incomingData);

	char *rx_str_state;
	char *color_state;
	int counter_tokens = 0;

	char *token = strtok_s(rx, ";", &rx_str_state);
	while( token != NULL){
		counter_tokens++;
		// printf("token->%s\n",token);

		char *color_component = strtok_s(token, ",", &color_state);
		int color = 0;
		int red = 0;
		int green = 0;
		int blue = 0;
		while( color_component != NULL ){
			color++;
			if ( color == 1 ) red = atoi(color_component);
			if ( color == 2 ) green = atoi(color_component);
			if ( color == 3 ) blue = atoi(color_component);

			//   printf("color_component->%s\n",color_component);      
			color_component = strtok_s(NULL, ",", &color_state);
		}
		if ( green != 0 || blue != 0 ) {
			printf(" ---> !! NOTICE !! found a color with component green [%i/%X] or blue[%i/%X]\n",green,green,blue,blue);
		}
		token = strtok_s(NULL, ";", &rx_str_state);    
	}
	printf("Parsed %i tokens\n",counter_tokens);
	*incomingData = NULL;

	return counter_tokens;
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
		// puts(incomingData);

		if (counter_rx < 5){
			printf("receved 5\n");
			exit(0);
		} else {
			printf("counter_rx=%i\n",counter_rx);
		}
		counter_rx++;

		// printf("sizeRxD=%i", sizeof(incomingData));
		token_parser();
		//wait a bit
#ifdef SERIAL_PORT_SLEEP
		Sleep(SERIAL_PORT_SLEEP);
#endif
	}
	return 0;
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


/**
 *  F:\Users\ehiller\dev\src\github.com\erichiller\hues\
 **/



