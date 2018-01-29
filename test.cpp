#include "Adafruit_TCS34725.h"
#include <stdio.h>
#include <math.h>
#include "config.h"
/*

Values to adjust ?/


Adafruit_TCS34725::setIntegrationTime(tcs34725IntegrationTime_t it)
    Sets the integration time for color samples from the sensor. Longer integration times can be used for increased sensitivity at low light levels. Valid integration times are:


Adafruit_TCS34725::setGain(tcs34725Gain_t gain)
    Sets the gain of the ADC to control the sensitivity of the sensor. Valid gain settings are:
      TCS34725_GAIN_1X = 0x00,  //  No gain
      TCS34725_GAIN_4X = 0x01,  // < 2x gain
      TCS34725_GAIN_16X = 0x02, // < 16x gain
      TCS34725_GAIN_60X = 0x03  // < 60x gain

https://learn.adafruit.com/adafruit-color-sensors/library-reference#gain-and-integration-time


WiringPi i2c library
http://wiringpi.com/reference/i2c-library/


Configuring i2c on the raspberry pi
https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup/the-gpio-connector

Arduino libraries
https://learn.adafruit.com/adafruit-all-about-arduino-libraries-install-use

mbedtls in python:
https://pypi.python.org/pypi/python-mbedtls/0.6

*/

void println(char const * str){
	printf("%s\n",str);
}


int main(){
  Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_1X);  
	printf("starting - colors are fun!");
	if( tcs.begin() ){
		println("Found Sensor");
		println("Began successful transmission to TCS34725");
	} else {
		println("No TCS34725 found ... check your connections");
		return 1;
	}


  // make bytearray for gammatable
     // a /= b   ...is the same as...    a = a/b


  // thanks PhilB for this gamma table!
  // it helps convert RGB colors to what humans see
  char gammatable[256];
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;
      
    gammatable[i] = x;      
  }


  uint16_t clear, red, green, blue;
	while(true){

    delay(60);  // takes 50ms to read 

    delay(1000);

    tcs.getRawData(&red, &green, &blue, &clear);
    uint32_t sum = clear;  


    // Figure out some basic hex code for visualization
    float r, g, b;
    r = red;    r /= sum;   r *= 255;
    g = green;  g /= sum;   g *= 255;
    b = blue;   b /= sum;   b *= 255;

    int redi = red >> 8;
    int blui = blue >> 8;
    int grni = green >> 8;

    printf("Clr:\t%u | %X\n", clear, clear);
    printf("\tRed:\t%u | %X //clrd= %X (%i)=(%f) // 8shift=%X(%i)\n", red, red, (int)r, (int)r, r, redi, redi);
    printf("\tGrn:\t%u | %X //clrd= %X (%i)=(%f) // 8shift=%X(%i)\n", green, green, (int)g, (int)g, g, grni, grni);
    printf("\tBlu:\t%u | %X //clrd= %X (%i)=(%f) // 8shift=%X(%i)\n", blue, blue, (int)b, (int)b, b,blui,blui);
    printf("  -->Color Temperature (K):\t%i", tcs.calculateColorTemperature(red,green,blue));
    printf("\n");



  /**
    printf("Clr:\t%u | %X\n", clear, clear);
    printf("\tRed:\t%u | %X //clrd= %X (%i)=(%f) // gamma=%i // 8shift=%X(%i)\n", red, red, (int)r, (int)r, r, gammatable[(int)r],redi,redi);
    printf("\tGrn:\t%u | %X //clrd= %X (%i)=(%f) // gamma=%i // 8shift=%X(%i)\n", green, green, (int)g, (int)g, g, gammatable[(int)g],grni, grni);
    printf("\tBlu:\t%u | %X //clrd= %X (%i)=(%f) // gamma=%i // 8shift=%X(%i)\n", blue, blue, (int)b, (int)b, b, gammatable[(int)b],blui,blui);
    printf("  -->Color Temperature (K):\t%i", tcs.calculateColorTemperature(red,green,blue));
    printf("\n");
    */

  }
}