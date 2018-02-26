

#ifndef HUE_H
#define HUE_H

#include <stdio.h>
#include <stdlib.h>

#include "http.h"

#include "config.h"

extern bool hue_hub_connected;


/* returns the received number of colors */
int create_message( unsigned char *msg, char light_id, uint16_t red, uint16_t green, uint16_t blue );

int hue_begin_stream( );

int hue_end_stream( );
#endif