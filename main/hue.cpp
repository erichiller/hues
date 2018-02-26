#include "hue.h"
#include "esp_log.h"
#include "config.h"

bool hue_hub_connected = false;


#define LOGT LOG_TAG_HUE_MSG


/* returns the received number of colors */
int create_message( unsigned char *msg, char light_id, uint16_t red, uint16_t green, uint16_t blue ) {
	// int lights = 1;
	// int message_length = 16 + (lights * 9);

	int COLOR_LENGTH = 19;    // start of color data

	msg[COLOR_LENGTH - 1] = (unsigned char)light_id;
	msg[COLOR_LENGTH + 0] = (unsigned char)( red >> 8 );
	msg[COLOR_LENGTH + 1] = (unsigned char)( red & 0x00ff );
	msg[COLOR_LENGTH + 2] = (unsigned char)( green >> 8 );
	msg[COLOR_LENGTH + 3] = (unsigned char)( green & 0x00ff );
	msg[COLOR_LENGTH + 4] = (unsigned char)( blue >> 8 );
	msg[COLOR_LENGTH + 5] = (unsigned char)( blue & 0x00ff );

	return true;
}

int hue_begin_stream( ) {
	char data[] = "{\"stream\": {\"active\": true}}";
	char url[1024];
	// char *success = "success";
	snprintf( url, 1023, "/api/%s/groups/%s", HUB_USER, ENTERTAINMENT_GROUP );
	ESP_LOGD( LOGT, "\n  . Setting Entertainment mode on the hub for url %s", url );
	if( http_request( SERVER_ADDR, SERVER_API_PORT, url, HTTP_PUT, "application/json", (char *)data, sizeof( data ), "success" ) ) {
		ESP_LOGI( LOGT, "entertainment mode is on" );
		hue_hub_connected = true;
		return 1;
	}
	ESP_LOGE( LOGT, "FAILURE changing entertainment mode in %s", __FILE__ );
	return 0;
}

int hue_end_stream( ) {
	const char data[] = "{\"stream\": {\"active\": false}}";
	char       url[1024];
	// char *success = "success";
	snprintf( url, 1023, "/api/%s/groups/%s", HUB_USER, ENTERTAINMENT_GROUP );
	ESP_LOGD( LOGT, "\n  . Setting Entertainment mode off the hub for url %s", url );
	if( http_request( SERVER_ADDR, SERVER_API_PORT, url, HTTP_PUT, "application/json", (char *)data, sizeof( data ), "success" ) ) {
		ESP_LOGI( LOGT, "entertainment mode is off" );
		return 1;
	}
	ESP_LOGE( LOGT, "FAILURE changing entertainment mode in %s", __FILE__ );
	return 0;
}


#undef LOGT