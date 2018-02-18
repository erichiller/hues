#include "driver/gpio.h"

#include "string.h"
#include "globals.h"
#include "commands.h"
#include "config.h"
#include "wifi_conn.h"
#include "timer.h"
#include "spectrum.h"


#define DEBUG
#define DEBUG_STARTUP
#define HUE_DIRECT_FROM_FREQUENCY



////////////////////////////////////////////////////////////////////////////////
// INTERNAL STATE
// These shouldn't be modified unless you know what you're doing.
////////////////////////////////////////////////////////////////////////////////


unsigned int time_start_millis = millis( );


////////////////////////////////////////////////////////////////////////////////
// MAIN SKETCH FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void app_main( ) {
	timerSemaphore = xSemaphoreCreateBinary( );

	// gpio_pad_select_gpio(AUDIO_INPUT_PIN);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(AUDIO_INPUT_PIN, GPIO_MODE_OUTPUT);
	// Set up ADC and audio input.
	analogReadResolution( ANALOG_READ_RESOLUTION );
	// analogReadAveraging(ANALOG_READ_AVERAGING);

	// Turn on the power indicator LED.
	// pinMode(POWER_LED_PIN, OUTPUT);
	// digitalWrite(POWER_LED_PIN, HIGH);

	// Clear the input command buffer
	memset( commandBuffer, 0, sizeof( commandBuffer ) );

#ifdef DEBUG_STARTUP
	printf( "amidst setup()\n" );
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
		printf( "onTimer no. " );
		printf( isrCount );
		printf( " took " );
		printf( ( l_isr_time - l_prior_isr_time ), DEC );
		println( " microseconds " );
		samplingCallback( );
	}

	if( samplingIsDone( ) ) {
		sampleCounter = 0;
		// Run FFT on sample data.
#ifdef DEBUG
		println( "FFT.Windowing..." );
#endif
		FFT.Windowing( samples, FFT_SIZE, FFT_WIN_TYP_HAMMING, FFT_FORWARD );
#ifdef DEBUG
		println( "FFT.Compute..." );
#endif
		FFT.Compute( samples, imaginary_complex, FFT_SIZE, FFT_FORWARD );
		// Calculate magnitude of complex numbers output by the FFT.
#ifdef DEBUG
		println( "FFT.ComplexToMagnitude..." );
#endif
		FFT.ComplexToMagnitude( samples, imaginary_complex, FFT_SIZE );
#ifdef DEBUG
		println( "FFT -> DONE" );
#endif

		if( LEDS_ENABLED == 1 ) {
			spectrumLoop( );
		}

		// Restart audio sampling.
		// samplingBegin();

		println( "========= fft processed -->" );
		for( int i = 0; i < FFT_SIZE; i++ ) {
			println( samples[i], DEC );
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
	printf( "FREQUENCY_BINS=" );
	println( FREQUENCY_BINS );
	printf( "Hue=" );
	printf( hue, DEC );
	printf( "Saturation=" );
	printf( saturation, DEC );
	printf( "Luminosity=" );
	println( value, DEC );
	printf( "red=" );
	printf( r, DEC );
	printf( "x255=" );
	printf( ( r * 255 ), DEC );
	printf( "hex=" );
	println( int( 255 * r ), HEX );
	printf( "grn=" );
	printf( g, DEC );
	printf( "x255=" );
	printf( ( g * 255 ), DEC );
	printf( "hex=" );
	println( int( 255 * g ), HEX );
	printf( "blu=" );
	printf( b, DEC );
	printf( "x255=" );
	printf( ( b * 255 ), DEC );
	printf( "hex=" );
	println( int( 255 * b ), HEX );
#endif

	//  printf(int(255*r));  printf(",");  printf(int(255*g));  printf(",");  printf(int(255*b));  println(";");
	printf( ";" );
	printf( uint16_t( 0xFFFF * r ) );
	printf( "," );
	printf( uint16_t( 0xFFFF * g ) );
	printf( "," );
	printf( uint16_t( 0xFFFF * b ) );
	println( ";" );
	return ( ( ( int( 255 * r ) ) << 16 ) + ( ( int( 255 * g ) ) << 8 ) +
			 int( 255 * b ) );
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
#	ifdef DEBUG
		printf( "**** HUE_DIRECT_FROM_FREQUENCY" );
		printf( " {windowMax=" );
		printf( windowMax );
		printf( "} {FFT_SIZE=" );
		printf( FFT_SIZE );
		printf( "} {hue=" );
		printf( hue );
		printf( "} {intensity=}" );
		printf( intensity, DEC );
		println( "} ****" );
#	endif
#else
#	ifdef DEBUG
		printf( "**** [i=" );
		printf( i, DEC );
		printf( "] {hues=" );
		printf( hues[i], DEC );
		printf( "} {intensity=" );
		printf( intensity, DEC );
		println( "} ****" );
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
	//   println("samplingCallback()");
	// #endif
	// Read from the ADC and store the sample data
	samples[sampleCounter] = analogRead( AUDIO_INPUT_PIN );
	println( samples[sampleCounter], DEC );

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
	debug( "timerSetup()" );
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