
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"

#include "net_wifi.h"
#include "secrets.h"
#include "config.h"

// see: http://esp-idf.readthedocs.io/en/latest/api-reference/wifi/esp_wifi.html



static const char *log_tag_wifi = LOG_TAG_WIFI;

EventGroupHandle_t wifi_event_group = NULL;

int CONNECTED_BIT = BIT0;

void log_wifi_init( ) {
	esp_log_level_set( log_tag_wifi, LOG_TAG_WIFI_LEVEL );
}

/**
I (3110) WiFi: SYSTEM_EVENT_STA_CONNECTED
D (3860) tcpip_adapter: if0 dhcpc cb
D (3860) tcpip_adapter: if0 ip changed=1
D (3860) event: SYSTEM_EVENT_STA_GOT_IP, ip:192.168.10.186, mask:255.255.255.0, gw:192.168.10.1
I (3860) event: sta ip: 192.168.10.186, mask: 255.255.255.0, gw: 192.168.10.1
[WiFi-event] event: 7
Guru Meditation Error: Core  0 panic'ed (Interrupt wdt timeout on CPU0)
Core 0 register dump:
PC      : 0x4008977b  PS      : 0x00060034  A0      : 0x8008ad5f  A1      : 0x3ffb0590
0x4008977b: vTaskExitCritical at C:/Users/ehiller/AppData/local/omega/system/msys/opt/esp-idf/components/freertos/tasks.c:3529

A2      : 0x3ffb33e4  A3      : 0x00000000  A4      : 0x00060021  A5      : 0x3ffbea10
A6      : 0x00000001  A7      : 0x00000000  A8      : 0xb33fffff  A9      : 0x000000ff
A10     : 0x00000001  A11     : 0x00000000  A12     : 0x00060023  A13     : 0x00000001
A14     : 0x00060023  A15     : 0x00000000  SAR     : 0x00000013  EXCCAUSE: 0x00000005
EXCVADDR: 0x00000000  LBEG    : 0x4000c2e0  LEND    : 0x40000c2f6  LCOUNT  : 0x00000000
Core 0 was running in ISR context:
EPC1    : 0x4000bff0  EPC2    : 0x00000000  EPC3    : 0x00000000  EPC4    : 0x4008977b
0x4008977b: vTaskExitCritical at C:/Users/ehiller/AppData/local/omega/system/msys/opt/esp-idf/components/freertos/tasks.c:3529
**/

esp_err_t WiFiEvent( void *ctx, system_event_t *event ) {
	printf( "[WiFi-event] event: %i\n", (int)event->event_id );

	switch( (int)event->event_id ) {
		case SYSTEM_EVENT_WIFI_READY:	// ESP32 WiFi ready
			ESP_LOGI( log_tag_wifi, "SYSTEM_EVENT_WIFI_READY" );
			return ESP_OK;
			break;
		case SYSTEM_EVENT_SCAN_DONE:	// ESP32 finish scanning AP
			ESP_LOGI( log_tag_wifi, "SYSTEM_EVENT_SCAN_DONE" );
			return ESP_OK;
			break;
		case SYSTEM_EVENT_STA_START:	// ESP32 station start
			ESP_LOGI( log_tag_wifi, "SYSTEM_EVENT_STA_START" );
			return ESP_OK;
			break;
		case SYSTEM_EVENT_STA_STOP:	// ESP32 station stop
			ESP_LOGI( log_tag_wifi, "SYSTEM_EVENT_STA_STOP" );
			return ESP_OK;
			break;
		case SYSTEM_EVENT_STA_CONNECTED:	// ESP32 station connected to AP
			ESP_LOGI( log_tag_wifi, "SYSTEM_EVENT_STA_CONNECTED" );
			return ESP_OK;
			break;
		case SYSTEM_EVENT_STA_DISCONNECTED:	// ESP32 station disconnected from AP
			ESP_LOGI( log_tag_wifi, "SYSTEM_EVENT_STA_DISCONNECTED" );
			
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);	
			ESP_ERROR_CHECK( esp_wifi_connect( ) );
			//return ESP_FAIL;
			return ESP_OK;
			break;
		case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:	// the auth mode of AP connected by ESP32 station changed
			ESP_LOGW( log_tag_wifi, "SYSTEM_EVENT_STA_AUTHMODE_CHANGE" );
			return event->event_id;
		case SYSTEM_EVENT_STA_GOT_IP:	// ESP32 station got IP from connected AP
			timer_start( TIMER_GROUP_0, TIMER_0 );
			
			printf( "SYSTEM_EVENT_STA_GOT_IP\n" );
			printf( "got ip:%s\n",
					ip4addr_ntoa( &event->event_info.got_ip.ip_info.ip ) );
			xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);

			ESP_LOGI( log_tag_wifi, "SYSTEM_EVENT_STA_GOT_IP" );
			return ESP_OK;
			break;
		case SYSTEM_EVENT_STA_LOST_IP:	// ESP32 station lost IP and the IP is reset to 0
			ESP_LOGE( log_tag_wifi, "SYSTEM_EVENT_STA_LOST_IP" );
			//return ESP_FAIL;
			return ESP_OK;
			break;
		case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:	// ESP32 station wps succeeds in enrollee mode	// ESP32 station wps fails in enrollee mode
			ESP_LOGI( log_tag_wifi, "SYSTEM_EVENT_STA_WPS_ER_SUCCESS" );
			return ESP_OK;
			break;
		case SYSTEM_EVENT_STA_WPS_ER_FAILED:	// ESP32 station wps fails in enrollee mode
			ESP_LOGE( log_tag_wifi, "SYSTEM_EVENT_STA_WPS_ER_FAILED" );
			//return ESP_FAIL;
			return ESP_OK;
			break;
		case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:	// ESP32 station wps timeout in enrollee mode
			ESP_LOGE( log_tag_wifi, "SYSTEM_EVENT_STA_WPS_ER_TIMEOUT" );
			//return ESP_FAIL;
			return ESP_OK;
			break;
		case SYSTEM_EVENT_STA_WPS_ER_PIN:	// ESP32 station wps pin code in enrollee mode
			ESP_LOGE( log_tag_wifi, "SYSTEM_EVENT_ETH_DISCONNECTED" );
			//return ESP_FAIL;
			return ESP_OK;
			break;
		case SYSTEM_EVENT_AP_START:	// ESP32 soft-AP start
			ESP_LOGI( log_tag_wifi, "SYSTEM_EVENT_AP_START" );
			return ESP_OK;
			break;
		case SYSTEM_EVENT_AP_STOP:	// ESP32 soft-AP stop
			ESP_LOGI( log_tag_wifi, "SYSTEM_EVENT_AP_STOP" );
			return ESP_OK;
			break;
		case SYSTEM_EVENT_AP_STACONNECTED:	// a station connected to ESP32 soft-AP
			ESP_LOGI( log_tag_wifi, "SYSTEM_EVENT_AP_STACONNECTED" );
			return ESP_OK;
			break;
		case SYSTEM_EVENT_AP_STADISCONNECTED:	// a station disconnected from ESP32 soft-AP
			ESP_LOGI( log_tag_wifi, "SYSTEM_EVENT_AP_STADISCONNECTED" );
			return ESP_OK;
			break;
		case SYSTEM_EVENT_AP_PROBEREQRECVED:	// Receive probe request packet in soft-AP interface
			ESP_LOGI( log_tag_wifi, "SYSTEM_EVENT_AP_PROBEREQRECVED" );
			return ESP_OK;
			break;
		case SYSTEM_EVENT_GOT_IP6:	// ESP32 station or ap or ethernet interface v6IP addr is preferred
			ESP_LOGI( log_tag_wifi, "SYSTEM_EVENT_GOT_IP6" );
			return ESP_OK;
			break;
		case SYSTEM_EVENT_ETH_START:	// ESP32 ethernet start
			ESP_LOGI( log_tag_wifi, "SYSTEM_EVENT_ETH_START" );
			return ESP_OK;
			break;
		case SYSTEM_EVENT_ETH_STOP:	// ESP32 ethernet stop
			ESP_LOGI( log_tag_wifi, "SYSTEM_EVENT_ETH_STOP" );
			return ESP_OK;
			break;
		case SYSTEM_EVENT_ETH_CONNECTED:	// ESP32 ethernet phy link up
			ESP_LOGI( log_tag_wifi, "SYSTEM_EVENT_ETH_CONNECTED" );
			return ESP_OK;
			break;
		case SYSTEM_EVENT_ETH_DISCONNECTED:	// ESP32 ethernet phy link down
			ESP_LOGE( log_tag_wifi, "SYSTEM_EVENT_ETH_DISCONNECTED" );
			//return ESP_FAIL;
			return ESP_OK;
			break;
		case SYSTEM_EVENT_ETH_GOT_IP:	// ESP32 ethernet got IP from connected AP
			ESP_LOGI( log_tag_wifi, "SYSTEM_EVENT_ETH_GOT_IP" );
			return ESP_OK;
			break;
		case SYSTEM_EVENT_MAX:
			ESP_LOGE( log_tag_wifi, "SYSTEM_EVENT_MAX" );
			//return ESP_FAIL;
			return ESP_OK;
			break;
		default:
			ESP_LOGE( log_tag_wifi, "UNKNOWN WiFi EVENT HAS OCCURRED" );
			//return ESP_FAIL;
			return ESP_OK;
			break;
	}
}


int net_wifi_connect( ) {
	nvs_flash_init( );
	ESP_LOGI( log_tag_wifi, "flash initialized" );

	tcpip_adapter_init( );
	ESP_LOGI( log_tag_wifi, "tcpip adapter initialized" );

	wifi_event_group = xEventGroupCreate();

	ESP_ERROR_CHECK( esp_event_loop_init( WiFiEvent, NULL ) );
	ESP_LOGI( log_tag_wifi, "WIFI event loop initialized" );

	wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT( );
	ESP_LOGI( log_tag_wifi, "WIFI config created" );

	ESP_ERROR_CHECK( esp_wifi_init( &wifi_config ) );
	ESP_LOGI( log_tag_wifi, "WIFI config initialized" );

	ESP_ERROR_CHECK( esp_wifi_set_storage( WIFI_STORAGE_RAM ) );
	ESP_LOGI( log_tag_wifi, "WIFI storage set to RAM" );

	ESP_LOGI(log_tag_wifi, "Setting WiFi configuration SSID %s...", sta_config.sta.ssid);
	ESP_ERROR_CHECK( esp_wifi_set_mode( WIFI_MODE_STA ) );
	ESP_LOGI( log_tag_wifi, "WIFI station mode set" );

	ESP_ERROR_CHECK( esp_wifi_set_config( WIFI_IF_STA, &sta_config ) );
	ESP_LOGI( log_tag_wifi, "WIFI config attached" );

	ESP_ERROR_CHECK( esp_wifi_start( ) );
	ESP_LOGI( log_tag_wifi, "WIFI interface started" );

	ESP_ERROR_CHECK( esp_wifi_connect( ) );
	ESP_LOGI( log_tag_wifi, "WIFI interface being connected" );

	return 0;
}



uint32_t wifi_get_local_ip( void ) {
	tcpip_adapter_if_t		ifx = TCPIP_ADAPTER_IF_AP;
	tcpip_adapter_ip_info_t ip_info;
	wifi_mode_t				mode;

	esp_wifi_get_mode( &mode );
	if( WIFI_MODE_STA == mode ) {
		tcpip_adapter_get_ip_info( ifx, &ip_info );
		return ip_info.ip.addr;
	}
	return 0;
}


// void net_wifi_status_print(void){
// 	printf("SSID   : ");
// 	println(ssid);
// 	printf("IP ADDR: ");
// 	println(WiFi.localIP());
// }
