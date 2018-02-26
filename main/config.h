#ifndef CONFIG
#define CONFIG

#include "esp_log.h"
#include "driver/timer.h"


/*
 ***************************************
 * LOGGING
 * 
 * LOG_TAG_* must be unique
 **************************************
 */
// net_wifi
#define LOG_DEFAULT_LEVEL ESP_LOG_INFO
#define LOG_TAG_WIFI "WiFi"
// main
#define LOG_TAG_WIFI_LEVEL ESP_LOG_VERBOSE
#define LOG_TAG_MAIN "Main"
// main -> color_loop , main_loop()
#define LOG_TAG_MAIN_LEVEL ESP_LOG_DEBUG
#define LOG_TAG_MAIN_COLOR_LOOP "Color_Loop"
// #define LOG_TAG_MAIN_COLOR_LOOP_LEVEL ESP_LOG_VERBOSE
#define LOG_TAG_MAIN_COLOR_LOOP_LEVEL ESP_LOG_INFO
// spectrum
#define LOG_TAG_SPECTRUM "Spectrum"
// #define LOG_TAG_SPECTRUM_LEVEL ESP_LOG_VERBOSE
#define LOG_TAG_SPECTRUM_LEVEL ESP_LOG_INFO
// time_intr
#define LOG_TAG_INTR "time_intr"
#define LOG_TAG_INTR_LEVEL ESP_LOG_VERBOSE
// http
#define LOG_TAG_HTTP "Http"
#define LOG_TAG_HTTP_LEVEL ESP_LOG_VERBOSE
// hue
#define LOG_TAG_HUE_MSG "hue_msg"
#define LOG_TAG_HUE_MSG_LEVEL ESP_LOG_VERBOSE
// timing_alt
#define LOG_TAG_MBED_TIME "EH_mbed_timing"
#define LOG_TAG_MBED_TIME_LEVEL ESP_LOG_INFO
// mbed (local)
#define LOG_TAG_MBED "dtls-erics-mbed"
#define LOG_TAG_MBED_LEVEL ESP_LOG_INFO
/** see esp-idf\components\mbedtls\port\include\mbedtls\esp_debug.h
 * - 1 - Warning
 * - 2 - Info
 * - 3 - Debug
 * - 4 - Verbose
 */
#define LOG_MBED_LEVEL 1


/*****
 * Log Colors
 * #define LOG_COLOR_BLACK   "30"
 * #define LOG_COLOR_RED     "31"
 * #define LOG_COLOR_GREEN   "32"
 * #define LOG_COLOR_BROWN   "33"
 * #define LOG_COLOR_BLUE    "34"
 * #define LOG_COLOR_PURPLE  "35"
 * #define LOG_COLOR_CYAN    "36"
 ***/
// add white for bolding
#define LOG_COLOR_WHITE "37"
// defaults
// #define LOG_COLOR_E       LOG_COLOR(LOG_COLOR_RED)
// #define LOG_COLOR_W       LOG_COLOR(LOG_COLOR_BROWN)
// edits
#undef LOG_COLOR_I
#define LOG_COLOR_I LOG_COLOR( LOG_COLOR_BLUE )
#undef LOG_COLOR_D
#define LOG_COLOR_D LOG_COLOR( LOG_COLOR_GREEN )
#undef LOG_COLOR_V
#define LOG_COLOR_V LOG_BOLD( LOG_COLOR_WHITE )





/* ESP32 */
#define BAUD_RATE 115200


/*** REQUEST TYPES ***/
#define HTTP_GET "GET"
#define HTTP_POST "POST"
#define HTTP_PUT "PUT"


/***************************************
 * HUE
 ***************************************/

#define HUE_DIRECT_FROM_FREQUENCY

/////////////////////////////////
// CONFIGURE COLOR DATA SOURCE //
//  ONLY __1__ CAN BE ACTIVE!  //
/////////////////////////////////
// #define SOURCE_TCS34725
#define SOURCE_LOCAL_MIC


/* hue stream , mbedtls */
#define SERVER_PORT "2100"
#define SERVER_NAME "Hue"
#define SERVER_ADDR "192.168.10.46" /* forces IPv4 */
// #define SERVER_ADDR "192.168.10.175" /* forces IPv4 */
#define SERVER_API_PORT 80
#define MESSAGE "Echo this"

#define DFL_PSK_IDENTITY "PKSaPQW6j-vRpcn9UPoNZ3Olm-Dd0EU0q-K9m-f7"
#define HUB_USER DFL_PSK_IDENTITY
// #define ENTERTAINMENT_GROUP "2"  // tv entertainment group
#define ENTERTAINMENT_GROUP "5" //OFFICE's entertainment group

// #define LIGHT_ID 0x0d // tv illumination
#define LIGHT_ID 0x0b // idealamp

#define READ_TIMEOUT_MS 1000
#define MAX_RETRY 5

#define HUE_TX_RATE_HZ 50

/* mbedtls */
//#define SLEEP_PERIOD 1000000
// #define SLEEP_PERIOD 100000
// #define SLEEP_PERIOD 50000

// #define MBED_CONFIG_FILE "mbed_config.h"
// #define CONFIG_MBEDTLS_SSL_PROTO_TLS1_2
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

/**
 *  timer 
 **/
#define TIMER_HZ_GROUP TIMER_GROUP_0
#define TIMER_HZ_IDX TIMER_0

#endif


/*
 this is a cool _hot pink_
 aa ff 12 34 56 78
 */