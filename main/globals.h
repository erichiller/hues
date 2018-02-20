
/* Globally accessible variables here */
#ifndef GLOBALS_H
#define GLOBALS_H
#include "arduinoFFT.h" // Standard Arduino FFT library https://github.com/kosme/arduinoFFT
#include "globals_t.h"
#include "driver/adc.h"

////////////////////////////////////////////////////////////////////////////////
// CONIFIGURATION
// These values can be changed to alter the behavior of the spectrum display.
////////////////////////////////////////////////////////////////////////////////

extern const int FFT_SIZE; // Size of the FFT.  Realistically can only be at most 256
						   // without running out of memory for buffers and other state.
extern const adc1_channel_t AUDIO_INPUT_PIN;
; // Input ADC pin for audio data. See board specfic pins header file. **A6**
extern const adc_bits_width_t ANALOG_READ_RESOLUTION;
; // Bits of resolution for the ADC.
extern const int ANALOG_READ_AVERAGING;
; // Number of samples to average with each ADC reading.
// const int POWER_LED_PIN = 13;       // Output pin for power LED (pin 13 to use Teensy 3.0's onboard LED).
extern const int FREQUENCY_BINS; // Number of neo pixels.  You should be able to increase this without
								 // any other changes to the program.

extern double samples[];
extern double imaginary_complex[];
extern double frequencyWindow[];
extern double hues[];

extern int sampleCounter;

extern int SAMPLE_RATE_HZ;

extern float	  SPECTRUM_MIN_DB;
extern float	  SPECTRUM_MAX_DB;
extern int		  LEDS_ENABLED;
extern arduinoFFT FFT;

// struct for timer to monitor frequency
extern timer_event_t timer_hz;


#endif