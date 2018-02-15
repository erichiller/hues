
#include "Arduino.h"
#include <WiFi.h>
#include "secrets.h"
#include "config.h"






void WiFiEvent(WiFiEvent_t event)
{
    Serial.printf("[WiFi-event] event: %d\n", event);

    switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("WiFi lost connection");
        break;
    }
}



int net_wifi_connect(){

	WiFi.disconnect(true);

	delay(1000);

	WiFi.onEvent(WiFiEvent);



	WiFi.begin(ssid, password);
    Serial.print("Connecting to Wireless SSID: ");
    Serial.println(ssid);
	int t_start = millis();
	while (WiFi.status() != WL_CONNECTED && (millis() - t_start) < WIFI_MAX_WAIT ) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}



void net_wifi_status_print(void){
	Serial.print("SSID   : ");
	Serial.println(ssid);
	Serial.print("IP ADDR: ");
	Serial.println(WiFi.localIP());
}


