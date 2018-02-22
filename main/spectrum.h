#ifndef SPECTRUM_H
#define SPECTRUM_H
#include "arduinoFFT.h"	// Standard Arduino FFT library https://github.com/kosme/arduinoFFT
#include "driver/adc.h"


/** 
 * Color output
 */
typedef struct
{
	uint16_t red;
	uint16_t green;
	uint16_t blue;
} color16_t;


extern const int FFT_SIZE;	// Size of the FFT.  Realistically can only be at most 256
							  // without running out of memory for buffers and other state.
extern const adc1_channel_t AUDIO_INPUT_PIN;
;	// Input ADC pin for audio data. See board specfic pins header file. **A6**
extern const adc_bits_width_t ANALOG_READ_RESOLUTION;
;	// Bits of resolution for the ADC.
extern adc_atten_t ANALOG_ATTENUATION;
;	// Number of samples to average with each ADC reading.
// const int POWER_LED_PIN = 13;       // Output pin for power LED (pin 13 to use Teensy 3.0's onboard LED).
extern const int FREQUENCY_BINS;	// Number of neo pixels.  You should be able to increase this without
									// any other changes to the program.

extern double	samples[];
extern double	imaginary_complex[];
extern double	frequencyWindow[];
extern double	hues[];
extern color16_t outcolors[];

extern int sampleCounter;

extern int SAMPLE_RATE_HZ;

extern float	  SPECTRUM_MIN_DB;
extern float	  SPECTRUM_MAX_DB;
extern arduinoFFT FFT;


void	  spectrumSetup( void );
void	  log_spectrum_init( );
bool	  samplingIsDone( );
int		  samplingCallback( );
void	  spectrumLoop( );
color16_t pixelHSVtoRGBColor( float hue, float saturation, float value );
int		  frequencyToBin( float frequency );
void	  windowMean( double *samples,
					  int	 lowBin,
					  int	 highBin,
					  float * windowMean,
					  float * otherMean,
					  int *   windowMax );

#endif