#include "string.h"
#include "sys/time.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/adc.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "commands.h"
#include "config.h"
#include "net_wifi.h"
#include "time_intr.h"
#include "spectrum.h"
#include "http.h"



#define LTAG LOG_TAG_MAIN

////////////////////////////////////////////////////////////////////////////////
// INTERNAL TIME
////////////////////////////////////////////////////////////////////////////////

timeval time_start_tv;
unsigned int   time_start_millis;

////////////////////////////////////////////////////////////////////////////////
// MAIN SKETCH FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void log_main_init( ) {
	esp_log_level_set( LOG_TAG_MAIN, LOG_TAG_MAIN_LEVEL );
}

void log_init( ) {
	esp_log_level_set( "*", LOG_DEFAULT_LEVEL );
	log_wifi_init( );
	log_main_init( );
	log_spectrum_init( );
	log_intr_init( );
	log_http_init( );
}


static void main_loop( void *arg ) {
	// Calculate FFT if a full sample is available.
	while( 1 ) {
		timer_event_t evt;
		xEventGroupWaitBits( wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY );
		xQueueReceive( timer_queue, &evt, portMAX_DELAY );

		// ESP_LOGI(LTAG, "Group[%d], timer[%d] alarm event\n", evt.timer_group, evt.timer_idx );

		/* Print the timer values passed by event */
		// printf( "------- EVENT TIME --------\n" );
		// print_timer_counter( evt.timer_counter_value );

		// /* Print the timer values as visible by this task */
		// printf( "-------- TASK TIME --------\n" );
		// uint64_t task_counter_value;
		// timer_get_counter_value( evt.timer_group, evt.timer_idx, &task_counter_value );
		// print_timer_counter( task_counter_value );

		int i = samplingCallback( );
		ESP_LOGD( LTAG, "taking samples, sampleCounter=%i , value=samples[%i]=%f", i , i, samples[i] );

		if( samplingIsDone( ) ) {
			sampleCounter = 0;
			// Run FFT on sample data.
			ESP_LOGD( LTAG, "FFT.Windowing..." );
			FFT.Windowing( samples, FFT_SIZE, FFT_WIN_TYP_HAMMING, FFT_FORWARD );
			ESP_LOGD( LTAG, "FFT.Compute..." );
			FFT.Compute( samples, imaginary_complex, FFT_SIZE, FFT_FORWARD );
			// Calculate magnitude of complex numbers output by the FFT.
			ESP_LOGD( LTAG, "FFT.ComplexToMagnitude..." );
			FFT.ComplexToMagnitude( samples, imaginary_complex, FFT_SIZE );
			ESP_LOGD( LTAG, "FFT -> DONE" );

			spectrumLoop( );

			printf( "=============== fft processed ===============\n" );
			for( int i = 0; i < FFT_SIZE; i++ ) {
				printf( "\tsample[%i]: %f\n", i, samples[i] );
			}
		
			printf( "=============== colors processed ===============\n" );
			for( int i = 0; i < FREQUENCY_BINS; i++ ) {
				printf( "\toutcolors %i: \tred: %i\t green: %i\t blue: %i\n", i, outcolors[i].red, outcolors[i].green, outcolors[i].blue );
			}
		
		/** FOR TESTING **/
		timer_pause( TIMER_GROUP_0, TIMER_0 );
		hue_mbed_open_dtls();
		}

		// Parse any pending commands.
		// parserLoop( );

		// ESP_LOGD(LTAG, "end of main_loop, restarting timer");
		// timer_start( TIMER_GROUP_0, TIMER_0 );
	}
}


// NOTE: Cpp might not be the best idea
// http://bbs.esp32.com/viewtopic.php?t=362
extern "C" void app_main( void ) {
	nvs_flash_init( );
	// logging init
	log_init( );
	ESP_LOGD( LTAG, "notice; debug log levels are enabled" );
	ESP_LOGI( LTAG, "notice; info  log levels are enabled" );
	ESP_LOGI( LTAG, "flash initialized" );

	// complete startup time
	gettimeofday( &time_start_tv, NULL );
	time_start_millis = time_start_tv.tv_usec * 1000;

	// Set up ADC and audio input.
	adc1_config_width( ANALOG_READ_RESOLUTION );
	adc1_config_channel_atten( AUDIO_INPUT_PIN, ANALOG_ATTENUATION );

	// Clear the input command buffer
	memset( commandBuffer, 0, sizeof( commandBuffer ) );

	// Initialize spectrum display
	spectrumSetup( );

	// connect to WiFi
	net_wifi_connect( );

	// setup timer for periodic
	timer_event_t timer_hz = {};
	timer_hz.timer_group   = TIMER_HZ_GROUP;
	timer_hz.timer_idx	 = TIMER_HZ_IDX;
	timer_setup( 10 );
	ESP_LOGD( LTAG, "\nTIMER_SCALE:\t %i\nTIMER_SCALED:\t %i\nSAMPLE_RATE_HZ:\t %i\n TIMER/SAMPLE: \t %f\n", TIMER_SCALE, TIMER_SCALED, SAMPLE_RATE_HZ, (double)( TIMER_SCALED / SAMPLE_RATE_HZ ) );

	//                  index              reload?     value in microseconds
	timer_init_group_0( timer_hz.timer_idx, true , (TIMER_SCALED / SAMPLE_RATE_HZ) );

	xTaskCreate( &main_loop, "main_loop", 8192, NULL, 1, NULL );
}

#undef LTAG
