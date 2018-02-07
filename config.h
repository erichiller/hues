#ifndef CONFIG
#define CONFIG
#include "upstream.h"

/* TCS34725 */
#define RASPBERRY_PI_2
#define RASPBERRY_PI
#define LOG_LEVEL 2

#define logm(message) \
	printf("LOG ---- %s : %i ----\n", __FILE__, __LINE__); \
	printf("LOG:\t%s\n", message);


/////////////////////////////////
// CONFIGURE COLOR DATA SOURCE //
//  ONLY __1__ CAN BE ACTIVE!  //
/////////////////////////////////
// #define SOURCE_TCS34725
#define SOURCE_SERIAL_SOUND

/* hue stream , mbedtls */

#define LIGHT_ID 0x0d

#define SERVER_PORT "2100"
#define SERVER_NAME "Hue"
#define SERVER_ADDR "192.168.10.46" /* forces IPv4 */
#define MESSAGE     "Echo this"

#define DFL_PSK_IDENTITY "PKSaPQW6j-vRpcn9UPoNZ3Olm-Dd0EU0q-K9m-f7"
#define HUB_USER DFL_PSK_IDENTITY
#define ENTERTAINMENT_GROUP "2"

#define READ_TIMEOUT_MS 1000
#define MAX_RETRY       5

#define DEBUG_LEVEL 0

/* mbedtls */
//#define SLEEP_PERIOD 1000000
#define SLEEP_PERIOD 100000

// #define ENABLE_GAMMATABLE /* enabling gammatable will attempt to adjust for how the human eye sees **/




/* SerialPort */

#define ARDUINO_WAIT_TIME 0
// #define MAX_DATA_LENGTH 255
#define MAX_DATA_LENGTH 255


// void logm(const char* file, const char* line, const char* func, char const * str){
// 	printf("LOG ---- %s : %s , %s ----", file, line, func);
// 	printf("LOG:\t%s\n",str);
// } 


#endif


/*
 this is a cool _hot pink_
 aa ff 12 34 56 78
 */