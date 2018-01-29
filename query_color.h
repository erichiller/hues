#include <stdint.h>
#include "Adafruit_TCS34725.h"
#include <math.h>


Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_16X);
int tcs_status = false;
char gammatable[256];

void logm(char const * str){
	printf("%s\n",str);
}

int colorme(uint16_t *red, uint16_t *green, uint16_t *blue){
	if( !tcs_status ){
		if( tcs.begin() ){
			logm("Found Sensor");
			logm("Began successful transmission to TCS34725");
		} else {
			logm("No TCS34725 found ... check your connections");
			return 1;
		}
		tcs_status = true;
		for (int i=0; i<256; i++) {
			float x = i;
			x /= 255;
			x = pow(x, 2.5);
			x *= 255;
				
			gammatable[i] = x;
		}
	}

	uint16_t clear;
	float r, g, b;
	tcs.getRawData(red, green, blue, &clear);

	r = *red;    r /= clear;   r *= 0xffff;
    g = *green;  g /= clear;   g *= 0xffff;
    b = *blue;   b /= clear;   b *= 0xffff;



	// r = *red;    r /= clear;   r = gammatable[(int)(r * 0xff)];	r *= 0xff;
    // g = *green;  g /= clear;   g = gammatable[(int)(g * 0xff)];	g *= 0xff;
    // b = *blue;   b /= clear;   b = gammatable[(int)(b * 0xff)];	b *= 0xff;

	*red = r;
	*green = g;
	*blue = b;



	// /* low byte high byte ?? */
	// uint16_t red_R = ((*red & 0xff00) >>8 ) | ((*red & 0x00ff) << 8);
	// uint16_t green_R = ((*green & 0xff00) >>8 ) | ((*green & 0x00ff) << 8);
	// uint16_t blue_R = ((*blue & 0xff00) >>8 ) | ((*blue & 0x00ff) << 8);

	// printf("R: %X \t G: %X \t B: %X \n", red_R, green_R, blue_R);


	// *red = red_R;
	// *green = green_R;
	// *blue = blue_R;

	return 0;
}