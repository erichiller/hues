

#ifndef HUE_H
#define HUE_H

#include <stdio.h>
#include <stdlib.h>

#include "http.h"

#include "config.h"
#ifdef SOURCE_TCS34725
#include "query_color.h"
#elif defined(SOURCE_SERIAL_SOUND)
#include "query_serial.h"
#endif

/* returns the received number of colors */
int create_message(unsigned char* msg, char light_id, uint16_t red, uint16_t green, uint16_t blue)
{
    // int lights = 1;
    // int message_length = 16 + (lights * 9);

    int rx = 0; // received colors
    int COLOR_LENGTH = 19; // start of color data

#ifdef SOURCE_TCS34725
    if (colorme(&red, &green, &blue)) {
        // if colorme returns an error, so should this
        return -1;
    }
#elif defined(SOURCE_LOCAL_MIC)
    rx = serial_receive_colors(&red, &green, &blue);
    if (rx == 0) {
        logm("error in serial_receive_colors");
    }
#ifdef DEBUG
    printf(" ----> create_message received->Colors:\n\t\n\tRed:\t%" PRIu16 "\n\tGreen:\t%" PRIu16 "\n\tblue:\t%" PRIu16 "\n", red, green, blue);
#endif
#else
    mbedtls_printf("\n  . NO MESSAGE SOURCE WAS DEFINED....\n EXITING...\n\n");
    exit(1);
#endif

    msg[COLOR_LENGTH - 1] = (unsigned char)light_id;
    msg[COLOR_LENGTH + 0] = (unsigned char)(red >> 8);
    msg[COLOR_LENGTH + 1] = (unsigned char)(red & 0x00ff);
    msg[COLOR_LENGTH + 2] = (unsigned char)(green >> 8);
    msg[COLOR_LENGTH + 3] = (unsigned char)(green & 0x00ff);
    msg[COLOR_LENGTH + 4] = (unsigned char)(blue >> 8);
    msg[COLOR_LENGTH + 5] = (unsigned char)(blue & 0x00ff);

    return rx;
}

int hue_begin_stream()
{
    char data[] = "{\"stream\": {\"active\": true}}";
    char url[1024];
    // char *success = "success";
    mbedtls_snprintf(url, 1023, "/api/%s/groups/%s", HUB_USER, ENTERTAINMENT_GROUP);
    mbedtls_printf("\n  . Setting Entertainment mode on the hub for url %s", url);
    if (http_request(SERVER_ADDR, 80, url, HTTP_PUT, "application/json", (char*)data, sizeof(data), "success")) {
        printf("entertainment mode is on");
        return 1;
    }
    return 0;
}

int hue_end_stream()
{
    char data[] = "{\"stream\": {\"active\": false}}";
    char url[1024];
    // char *success = "success";
    mbedtls_snprintf(url, 1023, "/api/%s/groups/%s", HUB_USER, ENTERTAINMENT_GROUP);
    if (http_request(SERVER_ADDR, 80, url, HTTP_PUT, "application/json", (char*)data, sizeof(data), "success")) {
        printf("entertainment mode is off");
        return 1;
    }

    return 0;
}

#endif