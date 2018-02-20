#include "globals.h"
#include "config.h"
#include "spectrum.h"
// #include "gpio.h"
#include "driver/adc.h"
#include "esp_log.h"

#define LOGT LOG_TAG_SPECTRUM


/******
 * NOTE:
 * THESE VALUES ARE DECLARED IN globals.h
 * THEY ARE DEFINED HERE
 */
int   SAMPLE_RATE_HZ  = 9000;	// Sample rate of the audio in hertz.
float SPECTRUM_MIN_DB = 30.0;	// Audio intensity (in decibels) that maps to low LED brightness.
float SPECTRUM_MAX_DB = 60.0;	// Audio intensity (in decibels) that maps to high LED brightness.
int   LEDS_ENABLED	= 1;		 // Control if the LED's should display the spectrum or not.  1 is true, 0 is false.
								 // Useful for turning the LED display on and off with commands from the serial port.

const int FFT_SIZE = 256;	// Size of the FFT.  Realistically can only be at most 256
							 // without running out of memory for buffers and other state.

const adc1_channel_t   AUDIO_INPUT_PIN		  = ADC1_GPIO34_CHANNEL;	// Input ADC pin for audio data. See board specfic pins header file. **A6**
const adc_bits_width_t ANALOG_READ_RESOLUTION = ADC_WIDTH_BIT_10;		// Bits of resolution for the ADC.
const int			   ANALOG_ATTENUATION	 = ADC_ATTEN_DB_0;
const int			   ANALOG_READ_AVERAGING  = 16;	// Number of samples to average with each ADC reading.
// const int POWER_LED_PIN = 13;       // Output pin for power LED (pin 13 to use Teensy 3.0's onboard LED).
const int FREQUENCY_BINS = 1;	// Number of neo pixels.  You should be able to increase this without
								 // any other changes to the program.

int sampleCounter = 0;


double samples[FFT_SIZE];

double frequencyWindow[FREQUENCY_BINS + 1];
double hues[FREQUENCY_BINS];

arduinoFFT FFT = arduinoFFT( );

double imaginary_complex[FFT_SIZE];


void log_spectrum_init(){
	esp_log_level_set( LOG_TAG_SPECTRUM, LOG_TAG_SPECTRUM_LEVEL );
}

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
		printf( "**** DEBUG_STARTUP **** hues[%i]=%d", i, hues[i] );
#endif
	}
	debug( "**** DEBUG_STARTUP **** \\end// hues[0]=" );
	debug( hues[0] );
}


////////////////////////////////////////////////////////////////////////////////
// UTILITY FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

// Compute the average magnitude of a target frequency window vs. all other frequencies.
void windowMean( double *samples,
				 int	 lowBin,
				 int	 highBin,
				 float * windowMean,
				 float * otherMean,
				 int *   windowMax ) {
	*windowMean = 0;
	*otherMean  = 0;
	*windowMax  = 1;
	// Notice the first magnitude bin is skipped because it represents the
	// average power of the signal.
	for( int i = 1; i < FFT_SIZE / 2; ++i ) {
		if( i >= lowBin && i <= highBin ) {
			*windowMean += samples[i];
		} else {
			*otherMean += samples[i];
		}
		if( samples[i] > samples[*windowMax] ) {
			*windowMax = i;
		}
	}
	*windowMean /= ( highBin - lowBin ) + 1;
	*otherMean /= ( FFT_SIZE / 2 - ( highBin - lowBin ) );
}

// Convert a frequency to the appropriate FFT bin it will fall within.
int frequencyToBin( float frequency ) {
	float binFrequency = float( SAMPLE_RATE_HZ ) / float( FFT_SIZE );
	return int( frequency / binFrequency );
}

// Convert from HSV values (in floating point 0 to 1.0) to RGB colors usable
// by neo pixel functions.
uint64_t pixelHSVtoRGBColor( float hue, float saturation, float value ) {
	// Implemented from algorithm at http://en.wikipedia.org/wiki/HSL_and_HSV#From_HSV
	float chroma = value * saturation;
	float h1	 = float( hue ) / 60.0;
	float x		 = chroma * ( 1.0 - fabs( fmod( h1, 2.0 ) - 1.0 ) );
	float r		 = 0;
	float g		 = 0;
	float b		 = 0;
	if( h1 < 1.0 ) {
		r = chroma;
		g = x;
	} else if( h1 < 2.0 ) {
		r = x;
		g = chroma;
	} else if( h1 < 3.0 ) {
		g = chroma;
		b = x;
	} else if( h1 < 4.0 ) {
		g = x;
		b = chroma;
	} else if( h1 < 5.0 ) {
		r = x;
		b = chroma;
	} else	// h1 <= 6.0
	{
		r = chroma;
		b = x;
	}
	float m = value - chroma;
	r += m;
	g += m;
	b += m;
#ifdef DEBUG
	printf( "\tFREQUENCY_BINS=%i\n", FREQUENCY_BINS );
	printf( "\tHue=%f\n", hue );
	printf( "\tSaturation=%f\n", saturation );
	printf( "\tLuminosity=%f\n", value );
	printf( "\tred  =%f\tx255=%f\thex=%04X\n", r, ( r * 255 ), int( 255 * r ) );
	printf( "\tgreen=%f\tx255=%f\thex=%04X\n", g, ( g * 255 ), int( 255 * g ) );
	printf( "\tblue =%f\tx255=%f\thex=%04X\n", b, ( b * 255 ), int( 255 * b ) );
#endif
	return (
		( uint16_t( 0xFFFF * r ) << 32 ) +
		( ( uint16_t( 0xFFFF * g ) ) << 16 ) +
		uint16_t( 0xFFFF * b ) );
}

////////////////////////////////////////////////////////////////////////////////
// SPECTRUM DISPLAY FUNCTIONS
///////////////////////////////////////////////////////////////////////////////


void spectrumLoop( ) {
	// Update each LED based on the intensity of the audio
	// in the associated frequency window.
	float intensity, otherMean;
	int   windowMax;

	for( int i = 0; i < FREQUENCY_BINS; ++i ) {
		windowMean( samples,
					frequencyToBin( frequencyWindow[i] ),
					frequencyToBin( frequencyWindow[i + 1] ),
					&intensity,
					&otherMean,
					&windowMax );
		// Convert intensity to decibels.
		intensity = 20.0 * log10( intensity );
		// Scale the intensity and clamp between 0 and 1.0.
		intensity -= SPECTRUM_MIN_DB;
		intensity = intensity < 0.0 ? 0.0 : intensity;
		intensity /= ( SPECTRUM_MAX_DB - SPECTRUM_MIN_DB );
		intensity = intensity > 1.0 ? 1.0 : intensity;
//    printf(pixelHSVtoRGBColor(hues[i], 1.0, intensity), HEX);
#ifdef HUE_DIRECT_FROM_FREQUENCY
		float hue = ( float( windowMax ) / ( FFT_SIZE / 2.0 ) ) * 360.0;
		ESP_LOGV(LOGT, "**** HUE_DIRECT_FROM_FREQUENCY **** {windowMax=%f} {FFT_SIZE=%i} {hue=%f} {intensity=%f} ****", windowMax, FFT_SIZE, hue, intensity );
#else
		float hue = hues[i];
		ESP_LOGV(LOGT, "**** [i=%i] {hue=%f} {intensity=%f} ****", i, hue, intensity );
#endif
		pixelHSVtoRGBColor( hue, 1.0, intensity );
	}
}

////////////////////////////////////////////////////////////////////////////////
// SAMPLING FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void samplingCallback( ) {
	// #ifdef DEBUG
	//   println("samplingCallback()");
	// #endif
	// Read from the ADC and store the sample data

	samples[sampleCounter] = adc1_get_raw( AUDIO_INPUT_PIN );
	printf( "analogRead=%f\n", samples[sampleCounter] );

	imaginary_complex[sampleCounter] = 0.0;
	// samples[sampleCounter+1] = 0.0;
	// Update sample buffer position and stop after the buffer is filled
	sampleCounter++;
	if( sampleCounter >= FFT_SIZE ) {
		ESP_LOGV(LOGT, "**** sampleCounter{%i} has exceeded FFT_SIZE{%i}, timer_pauseing ****", sampleCounter, FFT_SIZE );		
		// timer.end();
		timer_pause( TIMER_GROUP_0, TIMER_0 );
	}
}

bool samplingIsDone( ) {
	return sampleCounter >= FFT_SIZE;
}

#undef LOGT