#include "string.h"
#include "mbedtls/timing.h"

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
#include "mbed.h"
#include "hue.h"
#include <time.h>


#define LOGT LOG_TAG_MAIN

////////////////////////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////////////////////////

void log_main_init( ) {
	esp_log_level_set( LOG_TAG_MAIN, LOG_TAG_MAIN_LEVEL );
	esp_log_level_set( LOG_TAG_MAIN_COLOR_LOOP, LOG_TAG_MAIN_COLOR_LOOP_LEVEL );
}

void log_init( ) {
	ESP_LOGD( LOGT, "notice; debug    log levels are enabled" );
	ESP_LOGI( LOGT, "notice; info     log levels are enabled" );
	ESP_LOGV( LOGT, "notice; verbose  log levels are enabled" );
	printf( " **************************************** \n" );
	esp_log_level_set( "*", LOG_DEFAULT_LEVEL );
	log_wifi_init( );
	log_main_init( );
	log_spectrum_init( );
	log_intr_init( );
	log_http_init( );
	esp_log_level_set( LOG_TAG_MBED_TIME, LOG_TAG_MBED_TIME_LEVEL );
	esp_log_level_set( LOG_TAG_MBED, LOG_TAG_MBED_LEVEL );
	esp_log_level_set( LOG_TAG_HUE_MSG, LOG_TAG_HUE_MSG_LEVEL );
}

#undef LOGT
#define LOGT LOG_TAG_MAIN_COLOR_LOOP


int counter = 50;
void demo( ) {
	// complete startup time
	unsigned long time_start_millis = mbedtls_timing_hardclock();
	// counter--;
	// if(counter - 1 < 0){
	// 	ESP_LOGI( LOGT, "counter[%i] loop complete, pausing timer", counter)
	// 	timer_pause( TIMER_GROUP_0, TIMER_0 );
	// }
	outcolors[0] = pixelHSVtoRGBColor( 
		( (float) (time_start_millis % 360) ),
		1.0, 
		( (time_start_millis % ( HUE_TX_RATE_HZ * 2 ) ) )
		);
}


static void main_loop( void *arg ) {

	// while (1){
		
	// 	printf("analogRead=%f\n", (double) adc1_get_raw( AUDIO_INPUT_PIN ));
	// }

	while( 1 ) {


		timer_event_t evt;
		xEventGroupWaitBits( wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY );
		xQueueReceive( timer_queue, &evt, portMAX_DELAY );

		// ESP_LOGI(LOGT, "Group[%d], timer[%d] alarm event\n", evt.timer_group, evt.timer_idx );

		/* Print the timer values passed by event */
		// printf( "------- EVENT TIME --------\n" );
		// print_timer_counter( evt.timer_counter_value );

		// /* Print the timer values as visible by this task */
		// printf( "-------- TASK TIME --------\n" );
		// uint64_t task_counter_value;
		// timer_get_counter_value( evt.timer_group, evt.timer_idx, &task_counter_value );
		// print_timer_counter( task_counter_value );

/** demo block **/
		// demo();

		// if( hue_hub_connected != true ) {
		// 	if( hue_mbed_open_dtls( ) ) {
		// 		ESP_LOGI( LOGT, "hue_mbed_open_dtls( ... ) successful ; hue_hub_connected is now TRUE" )
		// 		hue_hub_connected = true;
		// 	}
		// }
		// if( hue_hub_connected == true ) {
		// 	hue_mbed_tx( outcolors[0].red, outcolors[0].green, outcolors[0].blue );
		// }
/** end demo block **/

		if( samplingIsDone( ) ) {
			sampleCounter = 0;
			// Run FFT on sample data.
			ESP_LOGD( LOGT, "FFT.Windowing..." );
			FFT.Windowing( samples, FFT_SIZE, FFT_WIN_TYP_HAMMING, FFT_FORWARD );
			ESP_LOGD( LOGT, "FFT.Compute..." );
			FFT.Compute( samples, imaginary_complex, FFT_SIZE, FFT_FORWARD );
			// Calculate magnitude of complex numbers output by the FFT.
			ESP_LOGD( LOGT, "FFT.ComplexToMagnitude..." );
			FFT.ComplexToMagnitude( samples, imaginary_complex, FFT_SIZE );
			ESP_LOGD( LOGT, "FFT -> DONE" );

			spectrumLoop( );

			ESP_LOGV( LOGT, "=============== fft processed ===============\n" );
			for( int i = 0; i < FFT_SIZE; i++ ) {
				ESP_LOGV( LOGT, "\tsample[%i]: %f\n", i, samples[i] );
			}

			ESP_LOGV( LOGT, "=============== colors processed ===============\n" );
			for( int i = 0; i < FREQUENCY_BINS; i++ ) {
				ESP_LOGV( LOGT, "\toutcolors[%i]: \tred: %i\t green: %i\t blue: %i\n", i, outcolors[i].red, outcolors[i].green, outcolors[i].blue );
			}

			if( hue_hub_connected != true ) {
				if( hue_mbed_open_dtls( ) ) {
					ESP_LOGI( LOGT, "hue_mbed_open_dtls( ... ) successful ; hue_hub_connected is now TRUE" );
					hue_hub_connected = true;
				} else {
					ESP_LOGE( LOGT , "hue_mbed_open_dtls( ... ) successful ; hue_hub_connected is now FALSE" );
					hue_hub_connected = false;
				}
			}
			if( hue_hub_connected == true ) {
				hue_mbed_tx( outcolors[0].red, outcolors[0].green, outcolors[0].blue );
			}

			// counter--;
			// if(counter - 1 < 0){
			// 	ESP_LOGI( LOGT, "counter[%i] loop complete, pausing timer", counter)
			// 	timer_pause( TIMER_GROUP_0, TIMER_0 );
			// }
		}
		samplingCallback( );

		// Parse any pending commands.
		// parserLoop( );

		// ESP_LOGD(LOGT, "end of main_loop, restarting timer");
		// timer_start( TIMER_GROUP_0, TIMER_0 );
	}
}

#undef LOGT
#define LOGT LOG_TAG_MAIN

// NOTE: Cpp might not be the best idea
// http://bbs.esp32.com/viewtopic.php?t=362
extern "C" void app_main( void ) {
	nvs_flash_init( );
	// logging init
	log_init( );
	ESP_LOGI( LOGT, "flash initialized" );

	time_t rawtime;
	struct tm *now;
	char time_string[80];

   	time( &rawtime );
	now = localtime(&rawtime );
	strftime(time_string, 80, "%c", now);
	ESP_LOGI(LOGT, "Starting up at %s", time_string);

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
	timer_hz.timer_idx     = TIMER_HZ_IDX;
	timer_setup( 10 );
	ESP_LOGD( LOGT, "\nTIMER_SCALE:\t %i\nTIMER_SCALED:\t %i\nSAMPLE_RATE_HZ:\t %i\n TIMER/SAMPLE: \t %f\n", TIMER_SCALE, TIMER_SCALED, SAMPLE_RATE_HZ, (double)( TIMER_SCALED / SAMPLE_RATE_HZ ) );

	//                  index              reload?     value in microseconds
	timer_init_group_0( timer_hz.timer_idx, true, ( TIMER_SCALED / SAMPLE_RATE_HZ ) );

	xTaskCreate( &main_loop, "main_loop", 8192, NULL, 1, NULL );
}

#undef LOGT
