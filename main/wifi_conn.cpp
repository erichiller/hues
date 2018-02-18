
#include "Arduino.h"
#include <WiFi.h>
#include "secrets.h"
#include "config.h"






void WiFiEvent(WiFiEvent_t event)
{
    printff("[WiFi-event] event: %d\n", event);

    switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
        println("WiFi connected");
        println("IP address: ");
        println(WiFi.localIP());
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        println("WiFi lost connection");
        break;
    }
}



int net_wifi_connect(){

	WiFi.disconnect(true);

	delay(1000);

	WiFi.onEvent(WiFiEvent);



	WiFi.begin(ssid, password);
    printf("Connecting to Wireless SSID: ");
    println(ssid);
	int t_start = millis();
	while (WiFi.status() != WL_CONNECTED && (millis() - t_start) < WIFI_MAX_WAIT ) {
		delay(500);
		printf(".");
	}
	println("");
	println("WiFi connected");
	println("IP address: ");
	println(WiFi.localIP());
}



void net_wifi_status_print(void){
	printf("SSID   : ");
	println(ssid);
	printf("IP ADDR: ");
	println(WiFi.localIP());
}


