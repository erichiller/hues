#include <Arduino.h>
#include "globals.h"
#include <arduinoFFT.h> // Standard Arduino FFT library https://github.com/kosme/arduinoFFT

int   SAMPLE_RATE_HZ  = 9000; // Sample rate of the audio in hertz.
float SPECTRUM_MIN_DB = 30.0; // Audio intensity (in decibels) that maps to low LED brightness.
float SPECTRUM_MAX_DB = 60.0; // Audio intensity (in decibels) that maps to high LED brightness.
int   LEDS_ENABLED	= 1;	// Control if the LED's should display the spectrum or not.  1 is true, 0 is false.
							  // Useful for turning the LED display on and off with commands from the serial port.

const int FFT_SIZE = 256;			   // Size of the FFT.  Realistically can only be at most 256
									   // without running out of memory for buffers and other state.
const int AUDIO_INPUT_PIN		 = A6; // Input ADC pin for audio data. See board specfic pins header file. **A6**
const int ANALOG_READ_RESOLUTION = 10; // Bits of resolution for the ADC.
const int ANALOG_READ_AVERAGING  = 16; // Number of samples to average with each ADC reading.
// const int POWER_LED_PIN = 13;       // Output pin for power LED (pin 13 to use Teensy 3.0's onboard LED).
const int FREQUENCY_BINS = 1; // Number of neo pixels.  You should be able to increase this without
							  // any other changes to the program.

int sampleCounter = 0;


double samples[FFT_SIZE];

double frequencyWindow[FREQUENCY_BINS + 1];
double hues[FREQUENCY_BINS];



arduinoFFT FFT = arduinoFFT( );

double imaginary_complex[FFT_SIZE];


void spectrumSetup( ) {
	// Set the frequency window values by evenly dividing the possible frequency
	// spectrum across the number of neo pixels.
	float windowSize = ( SAMPLE_RATE_HZ / 2.0 ) / float( FREQUENCY_BINS );
	for( int i = 0; i < FREQUENCY_BINS + 1; ++i ) {
		frequencyWindow[i] = i * windowSize;
	}
	// Evenly spread hues across all pixels.
	for( int i = 0; i < FREQUENCY_BINS; ++i ) {
		hues[i] = 360.0 * ( float( i ) / float( FREQUENCY_BINS - 1 ) );
#ifdef DEBUG_STARTUP
		printf( "**** DEBUG_STARTUP **** hues[" );
		printf( i );
		printf( "]=" );
		println( hues[i] );
#endif
	}
#ifdef DEBUG_STARTUP
	printf( "**** DEBUG_STARTUP **** \\end// hues[0]=" );
	println( hues[0] );
#endif
}