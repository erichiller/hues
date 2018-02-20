#ifdef __cplusplus
extern "C" {
#endif
#ifndef WIFI_H
#define WIFI_H

#include "freertos/event_groups.h"


int net_wifi_connect(void);

void net_wifi_status_print(void);

void log_wifi_init();

uint32_t wifi_get_local_ip( void );

/* FreeRTOS event group to signal when we are connected & ready to make a request */
extern EventGroupHandle_t wifi_event_group;
/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
extern int CONNECTED_BIT;

#endif
#ifdef __cplusplus
}
#endif