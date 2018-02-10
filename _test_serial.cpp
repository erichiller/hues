
#include <stdio.h>
#include <math.h>
#include "config.h"
#include "query_serial.h"
#include <stdint.h>




int main(){
	



	// for (int i = 0; i < sizeof(test); i++) {
	//   test[i] >>= 1;
	//   printf("%i:%i:%s\n",i,sizeof(test),test);
	// }

	// char *token = strtok(test,";");
	// while( token != NULL){
	//   printf("token->%s\n",token);
	//   token = strtok(token, ";");
	// }

	char command[] = "GET FFT_SIZE;";
	serial_send_data(command, sizeof(command));
	// serial_send_data("SET LEDS_ENABLED 1;\0", MAX_DATA_LENGTH);
	// serial_send_data("GET LEDS_ENABLED;\0", MAX_DATA_LENGTH);
	// serial_send_data("SET FREQUNCY_BINS 1;", 21);
	serial_receive_data();
}