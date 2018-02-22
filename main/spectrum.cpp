#include "config.h"
#include "spectrum.h"
// #include "gpio.h"
#include "driver/adc.h"
#include "esp_log.h"

#define LOGT LOG_TAG_SPECTRUM


/***
 * converts inputs of noise to ouputs of colors of size FREQUENCY BINS
 * (samplingCallback)--samples[FFT_SIZE]--> (spectrumLoop)--outcolors[FREQUENCY_BINS]
 **/


/******
 * NOTE:
 * THESE VALUES ARE DECLARED IN globals.h
 * THEY ARE DEFINED HERE
 */
int   SAMPLE_RATE_HZ  = 9000;	// Sample rate of the audio in hertz.
float SPECTRUM_MIN_DB = 30.0;	// Audio intensity (in decibels) that maps to low LED brightness.
float SPECTRUM_MAX_DB = 60.0;	// Audio intensity (in decibels) that maps to high LED brightness.

const int FFT_SIZE = 256;	// Size of the FFT.  Realistically can only be at most 256
							 // without running out of memory for buffers and other state.

const adc1_channel_t   AUDIO_INPUT_PIN		  = ADC1_GPIO34_CHANNEL;	// Input ADC pin for audio data. See board specfic pins header file. **A6**
const adc_bits_width_t ANALOG_READ_RESOLUTION = ADC_WIDTH_BIT_12;		// Bits of resolution for the ADC.
adc_atten_t			   ANALOG_ATTENUATION	  = ADC_ATTEN_DB_11;
const int			   ANALOG_READ_AVERAGING  = 16;	// Number of samples to average with each ADC reading.
const int			   FREQUENCY_BINS		  = 1;	 // Number of neo pixels.  You should be able to increase this without
													   // any other changes to the program.

int sampleCounter = 0;


double samples[FFT_SIZE];

double frequencyWindow[FREQUENCY_BINS + 1];
double hues[FREQUENCY_BINS];

arduinoFFT FFT = arduinoFFT( );

double imaginary_complex[FFT_SIZE];

color16_t outcolors[FREQUENCY_BINS];


void log_spectrum_init( ) {
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
		ESP_LOGD( LOGT, "**** DEBUG_STARTUP **** hues[%i]=%f", i, hues[i] );
	}
	ESP_LOGD( LOGT, "**** DEBUG_STARTUP **** \\end// hues[0]=%f", hues[0] );
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
color16_t pixelHSVtoRGBColor( float hue, float saturation, float value ) {
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
	ESP_LOGD( LOGT, "\tFREQUENCY_BINS=%i\n", FREQUENCY_BINS );
	ESP_LOGD( LOGT, "\tHue=%f\n", hue );
	ESP_LOGD( LOGT, "\tSaturation=%f\n", saturation );
	ESP_LOGD( LOGT, "\tLuminosity=%f\n", value );
	ESP_LOGD( LOGT, "\tred  =%f\tx255=%f\thex=%04X\n", r, ( r * 255 ), int( 255 * r ) );
	ESP_LOGD( LOGT, "\tgreen=%f\tx255=%f\thex=%04X\n", g, ( g * 255 ), int( 255 * g ) );
	ESP_LOGD( LOGT, "\tblue =%f\tx255=%f\thex=%04X\n", b, ( b * 255 ), int( 255 * b ) );
	// return ( ( uint64_t )( uint16_t( 0xFFFF * r ) << 32 ) +
	// 		 ( ( uint16_t( 0xFFFF * g ) ) << 16 ) +
	// 		 uint16_t( 0xFFFF * b ) );
	return ( color16_t ){
		.red   = ( uint16_t )( 0xFFFF * r ),
		.green = ( uint16_t )( 0xFFFF * b ),
		.blue  = ( uint16_t )( 0xFFFF * g ),
	};
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
		ESP_LOGV( LOGT, "**** HUE_DIRECT_FROM_FREQUENCY **** {windowMax=%i} {FFT_SIZE=%i} {hue=%f} {intensity=%f} ****", windowMax, FFT_SIZE, hue, intensity );
#else
		float hue = hues[i];
		ESP_LOGV( LOGT, "**** [i=%i] {hue=%f} {intensity=%f} ****", i, hue, intensity );
#endif
		outcolors[i] = pixelHSVtoRGBColor( hue, 1.0, intensity );
	}
}

////////////////////////////////////////////////////////////////////////////////
// SAMPLING FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

int samplingCallback( ) {
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
		ESP_LOGV( LOGT, "**** sampleCounter{%i} has exceeded FFT_SIZE{%i}, timer_pausing ****", sampleCounter, FFT_SIZE );
		// timer.end();
		// timer_pause( TIMER_GROUP_0, TIMER_0 );
	}
	return sampleCounter - 1;
}

bool samplingIsDone( ) {
	return sampleCounter >= FFT_SIZE;
}

#undef LOGT