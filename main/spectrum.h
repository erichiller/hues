#ifndef SPECTRUM_H
#define SPECTRUM_H
#include "arduinoFFT.h" // Standard Arduino FFT library https://github.com/kosme/arduinoFFT


void spectrumSetup( void );
void log_spectrum_init( );
bool samplingIsDone( );
void samplingCallback( );
void spectrumLoop( );
uint64_t pixelHSVtoRGBColor( float hue, float saturation, float value );
int frequencyToBin( float frequency );
void windowMean( double *samples,
				 int	 lowBin,
				 int	 highBin,
				 float * windowMean,
				 float * otherMean,
				 int *   windowMax );

#endif