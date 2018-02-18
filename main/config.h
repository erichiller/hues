#ifndef CONFIG
#define CONFIG

/* ESP32 */
#define BAUD_RATE 115200


#define logm(message) \
	printf("\nLOG:\t (src) %s : %i ----\n", __FILE__, __LINE__); \
	printf("LOG:\t(msg) %s\n", message);

#ifdef DEBUG
#define debug(message) \
	printf("\nLOG:\t (src) "); \
	printf(__FILE__); \
	printf(" : "); \
	printf(__LINE__); \
	println("----"); \
	printf("LOG:\t(msg) ");
	println(message);
#else
	#define debug(message) 
#endif


/////////////////////////////////
// CONFIGURE COLOR DATA SOURCE //
//  ONLY __1__ CAN BE ACTIVE!  //
/////////////////////////////////
// #define SOURCE_TCS34725
#define SOURCE_LOCAL_MIC

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
// #define SLEEP_PERIOD 100000
#define SLEEP_PERIOD 50000

// #define MBED_CONFIG_FILE "mbed_config.h"
// #define CONFIG_MBEDTLS_SSL_PROTO_TLS1_1
// #define CONFIG_MBEDTLS_SSL_PROTO_DTLS
// #define MBEDTLS_NET_C
// #define MBEDTLS_TIMING_C
// #define MBEDTLS_KEY_EXCHANGE_PSK_ENABLED

/*******************************************************************************
 ******************************** mbedtls / end ********************************
 ******************************************************************************/


// #define ENABLE_GAMMATABLE /* enabling gammatable will attempt to adjust for how the human eye sees **/


/* wifi max wait time (miliseconds) */
#define WIFI_MAX_WAIT 30000



/* SerialPort */

#define ARDUINO_WAIT_TIME 0
// #define MAX_DATA_LENGTH 255
#define MAX_DATA_LENGTH 64

// #define SERIAL_PORT_SLEEP 1000
#define SERIAL_PORT_SLEEP 500


// void logm(const char* file, const char* line, const char* func, char const * str){
// 	printf("LOG ---- %s : %s , %s ----", file, line, func);
// 	printf("LOG:\t%s\n",str);
// } 

/* replacement for println */

#ifdef ARDUINO
#define println(message) \
	println(message);
#else
#define println(message) \
	printf("%s\n", message);
#endif


/** POST CONFIG PROCESSING 
 * DO NOT EDIT BENEATH HERE**/
#if DEBUG_LEVEL > 0
#pragma message("NOTE ---> DEBUG MESSAGES ENABLED")
#define DEBUG
#endif

#endif


/*
 this is a cool _hot pink_
 aa ff 12 34 56 78
 */

#include "upstream.h"
