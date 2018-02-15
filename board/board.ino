#include <Wire.h>
#include <arduinoFFT.h> // Standard Arduino FFT library https://github.com/kosme/arduinoFFT
#include "commands.h"
#include "config.h"
#include "wifi_conn.h"


#define DEBUG
#define DEBUG_STARTUP
#define HUE_DIRECT_FROM_FREQUENCY


////////////////////////////////////////////////////////////////////////////////
// INTERNAL STATE
// These shouldn't be modified unless you know what you're doing.
////////////////////////////////////////////////////////////////////////////////

hw_timer_t *			   timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE			   timerMux = portMUX_INITIALIZER_UNLOCKED;
//timer counters
volatile uint32_t isrCounter	 = 0;
volatile uint32_t isr_time		 = 0;
volatile uint32_t prior_isr_time = 0;

double imaginary_complex[FFT_SIZE];

int sampleCounter = 0;

unsigned int time_start_millis = millis( );

double frequencyWindow[FREQUENCY_BINS + 1];
double hues[FREQUENCY_BINS];

////////////////////////////////////////////////////////////////////////////////
// MAIN SKETCH FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void setup( ) {
	// Set up serial port.
	Serial.begin( BAUD_RATE );
	timerSemaphore = xSemaphoreCreateBinary( );

	// Set up ADC and audio input.
	pinMode( AUDIO_INPUT_PIN, INPUT );
	analogReadResolution( ANALOG_READ_RESOLUTION );
	// analogReadAveraging(ANALOG_READ_AVERAGING);

	// Turn on the power indicator LED.
	// pinMode(POWER_LED_PIN, OUTPUT);
	// digitalWrite(POWER_LED_PIN, HIGH);

	// Clear the input command buffer
	memset( commandBuffer, 0, sizeof( commandBuffer ) );

#ifdef DEBUG_STARTUP
	Serial.print( "amidst setup()" );
#endif

	// Initialize spectrum display
	spectrumSetup( );

	// Begin sampling audio
	timerSetup( );

	// connect to WiFi
	net_wifi_connect( );
}


void loop( ) {
	// Calculate FFT if a full sample is available.

	if( xSemaphoreTake( timerSemaphore, 0 ) == pdTRUE ) {
		uint32_t isrCount = 0, l_isr_time = 0, l_prior_isr_time;
		// Read the interrupt count and time
		portENTER_CRITICAL( &timerMux );
		isrCount		 = isrCounter;
		l_isr_time		 = isr_time;
		l_prior_isr_time = prior_isr_time;
		portEXIT_CRITICAL( &timerMux );
		// Print it
		Serial.print( "onTimer no. " );
		Serial.print( isrCount );
		Serial.print( " took " );
		Serial.print( ( l_isr_time - l_prior_isr_time ), DEC );
		Serial.println( " microseconds " );
		samplingCallback( );
	}

	if( samplingIsDone( ) ) {
		sampleCounter = 0;
		// Run FFT on sample data.
#ifdef DEBUG
		Serial.println( "FFT.Windowing..." );
#endif
		FFT.Windowing( samples, FFT_SIZE, FFT_WIN_TYP_HAMMING, FFT_FORWARD );
#ifdef DEBUG
		Serial.println( "FFT.Compute..." );
#endif
		FFT.Compute( samples, imaginary_complex, FFT_SIZE, FFT_FORWARD );
		// Calculate magnitude of complex numbers output by the FFT.
#ifdef DEBUG
		Serial.println( "FFT.ComplexToMagnitude..." );
#endif
		FFT.ComplexToMagnitude( samples, imaginary_complex, FFT_SIZE );
#ifdef DEBUG
		Serial.println( "FFT -> DONE" );
#endif

		if( LEDS_ENABLED == 1 ) {
			spectrumLoop( );
		}

		// Restart audio sampling.
		// samplingBegin();

		Serial.println( "========= fft processed -->" );
		for( int i = 0; i < FFT_SIZE; i++ ) {
			Serial.println( samples[i], DEC );
		}
	}

	// Parse any pending commands.
	parserLoop( );

	if( ( millis( ) - time_start_millis ) < 10 ) {
		timerStart( timer );
	}
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
uint32_t pixelHSVtoRGBColor( float hue, float saturation, float value ) {
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
	} else // h1 <= 6.0
	{
		r = chroma;
		b = x;
	}
	float m = value - chroma;
	r += m;
	g += m;
	b += m;
#ifdef DEBUG
	Serial.print( "FREQUENCY_BINS=" );
	Serial.println( FREQUENCY_BINS );
	Serial.print( "Hue=" );
	Serial.print( hue, DEC );
	Serial.print( "Saturation=" );
	Serial.print( saturation, DEC );
	Serial.print( "Luminosity=" );
	Serial.println( value, DEC );
	Serial.print( "red=" );
	Serial.print( r, DEC );
	Serial.print( "x255=" );
	Serial.print( ( r * 255 ), DEC );
	Serial.print( "hex=" );
	Serial.println( int( 255 * r ), HEX );
	Serial.print( "grn=" );
	Serial.print( g, DEC );
	Serial.print( "x255=" );
	Serial.print( ( g * 255 ), DEC );
	Serial.print( "hex=" );
	Serial.println( int( 255 * g ), HEX );
	Serial.print( "blu=" );
	Serial.print( b, DEC );
	Serial.print( "x255=" );
	Serial.print( ( b * 255 ), DEC );
	Serial.print( "hex=" );
	Serial.println( int( 255 * b ), HEX );
#endif

	//  Serial.print(int(255*r));  Serial.print(",");  Serial.print(int(255*g));  Serial.print(",");  Serial.print(int(255*b));  Serial.println(";");
	Serial.print( ";" );
	Serial.print( uint16_t( 0xFFFF * r ) );
	Serial.print( "," );
	Serial.print( uint16_t( 0xFFFF * g ) );
	Serial.print( "," );
	Serial.print( uint16_t( 0xFFFF * b ) );
	Serial.println( ";" );
	return ( ( ( int( 255 * r ) ) << 16 ) + ( ( int( 255 * g ) ) << 8 ) +
			 int( 255 * b ) );
}

////////////////////////////////////////////////////////////////////////////////
// SPECTRUM DISPLAY FUNCTIONS
///////////////////////////////////////////////////////////////////////////////

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
		Serial.print( "**** DEBUG_STARTUP **** hues[" );
		Serial.print( i );
		Serial.print( "]=" );
		Serial.println( hues[i] );
#endif
	}
#ifdef DEBUG_STARTUP
	Serial.print( "**** DEBUG_STARTUP **** \\end// hues[0]=" );
	Serial.println( hues[0] );
#endif
}

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
//    Serial.print(pixelHSVtoRGBColor(hues[i], 1.0, intensity), HEX);
#ifdef HUE_DIRECT_FROM_FREQUENCY
		float hue = ( float( windowMax ) / ( FFT_SIZE / 2.0 ) ) * 360.0;
#	ifdef DEBUG
		Serial.print( "**** HUE_DIRECT_FROM_FREQUENCY" );
		Serial.print( " {windowMax=" );
		Serial.print( windowMax );
		Serial.print( "} {FFT_SIZE=" );
		Serial.print( FFT_SIZE );
		Serial.print( "} {hue=" );
		Serial.print( hue );
		Serial.print( "} {intensity=}" );
		Serial.print( intensity, DEC );
		Serial.println( "} ****" );
#	endif
#else
#	ifdef DEBUG
		Serial.print( "**** [i=" );
		Serial.print( i, DEC );
		Serial.print( "] {hues=" );
		Serial.print( hues[i], DEC );
		Serial.print( "} {intensity=" );
		Serial.print( intensity, DEC );
		Serial.println( "} ****" );
#	endif
		float hue = hues[i];
#endif
		pixelHSVtoRGBColor( hue, 1.0, intensity );
	}
}

////////////////////////////////////////////////////////////////////////////////
// SAMPLING FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void samplingCallback( ) {
	// #ifdef DEBUG
	//   Serial.println("samplingCallback()");
	// #endif
	// Read from the ADC and store the sample data
	samples[sampleCounter] = analogRead( AUDIO_INPUT_PIN );
	Serial.println( samples[sampleCounter], DEC );

	imaginary_complex[sampleCounter] = 0.0;
	// samples[sampleCounter+1] = 0.0;
	// Update sample buffer position and stop after the buffer is filled
	sampleCounter++;
	if( sampleCounter >= FFT_SIZE ) {
		// timer.end();
		timerAlarmDisable( timer );
	}
}

boolean samplingIsDone( ) {
	return sampleCounter >= FFT_SIZE;
}

////////////////////////////////////////
// Timer Management
////////////////////////////////////////

void IRAM_ATTR timerCall( ) {
	// Increment the counter and set the time of ISR
	portENTER_CRITICAL_ISR( &timerMux );
	isrCounter++;
	// lastIsrAt = millis();
	prior_isr_time = isr_time;
	isr_time	   = micros( );
	portEXIT_CRITICAL_ISR( &timerMux );
	// Give a semaphore that we can check in the loop
	xSemaphoreGiveFromISR( timerSemaphore, NULL );
	// It is safe to use digitalRead/Write here if you want to toggle an output
}

void timerSetup( ) {
#ifdef DEBUG
	Serial.println( "timerSetup()" );
#endif
	timer = timerBegin( 0, 80, true ); // divide by 80, gives us 1,000,000/s
	// third value in timerAttachInterrupt is edge/level triggering
	// see https://electronics.stackexchange.com/questions/21886/what-does-edge-triggered-and-level-triggered-mean
	timerAttachInterrupt( timer, &timerCall, true ); //attach callback
	// Set alarm to call onTimer function every second / SAMPLE_RATE_HZ ;; Thus, 10 kHz would be ever .1ms
	// Repeat the alarm is the third parameter
	timerAlarmWrite( timer, 1000000 / SAMPLE_RATE_HZ, true );
	// Start an alarm
	timerAlarmEnable( timer );
}