#include "string.h"
#include "sys/time.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/adc.h"
#include "esp_log.h"

#include "globals.h"
#include "commands.h"
#include "config.h"
#include "net_wifi.h"
#include "time_intr.h"
#include "spectrum.h"


#define DEBUG
#define DEBUG_STARTUP
#define HUE_DIRECT_FROM_FREQUENCY

#define LTAG LOG_TAG_MAIN

////////////////////////////////////////////////////////////////////////////////
// INTERNAL TIME
////////////////////////////////////////////////////////////////////////////////

struct timeval time_start_tv;
unsigned int   time_start_millis;


////////////////////////////////////////////////////////////////////////////////
// MAIN SKETCH FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


void log_main_init(){
	esp_log_level_set( LOG_TAG_MAIN, LOG_TAG_MAIN_LEVEL );
}

void log_init( ) {
	esp_log_level_set( "*", LOG_DEFAULT_LEVEL );
	log_wifi_init( );
	log_main_init( );
	log_spectrum_init( );
}


static void main_loop( void *arg ) {
	// Calculate FFT if a full sample is available.
	while( 1 ) {
		timer_event_t evt;
		xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                            false, true, portMAX_DELAY);
		xQueueReceive( timer_queue, &evt, portMAX_DELAY );

		/* Print information that the timer reported an event */
		if( evt.type == EVENT_ONE_HZ_COLLECTED ) {
			ESP_LOGI(LTAG, "EVENT TYPE = EVENT_ONE_HZ_COLLECTED\n" );
			// } else if (evt.type == TEST_WITH_RELOAD) {
			//     printf("\n    Example timer with auto reload\n");
		} else {
			ESP_LOGE(LTAG, "UNKNOWN EVENT TYPE\n" );
		}
		ESP_LOGI(LTAG, "Group[%d], timer[%d] alarm event\n", evt.timer_group, evt.timer_idx );

		/* Print the timer values passed by event */
		printf( "------- EVENT TIME --------\n" );
		print_timer_counter( evt.timer_counter_value );

		/* Print the timer values as visible by this task */
		printf( "-------- TASK TIME --------\n" );
		uint64_t task_counter_value;
		timer_get_counter_value( evt.timer_group, evt.timer_idx, &task_counter_value );
		print_timer_counter( task_counter_value );

		ESP_LOGD(LTAG,  "taking samples" );

		// samplingCallback( );
	}

	// if( samplingIsDone( ) ) {
	// 	sampleCounter = 0;
	// 	// Run FFT on sample data.
	// 	ESP_LOGD(LTAG, "FFT.Windowing..." );
	// 	FFT.Windowing( samples, FFT_SIZE, FFT_WIN_TYP_HAMMING, FFT_FORWARD );
	// 	ESP_LOGD(LTAG, "FFT.Compute..." );
	// 	FFT.Compute( samples, imaginary_complex, FFT_SIZE, FFT_FORWARD );
	// 	// Calculate magnitude of complex numbers output by the FFT.
	// 	ESP_LOGD(LTAG, "FFT.ComplexToMagnitude..." );
	// 	FFT.ComplexToMagnitude( samples, imaginary_complex, FFT_SIZE );
	// 	ESP_LOGD(LTAG, "FFT -> DONE" );

	// 	if( LEDS_ENABLED == 1 ) {
	// 		spectrumLoop( );
	// 	}

	// 	printf( "========= fft processed =========" );
	// 	for( int i = 0; i < FFT_SIZE; i++ ) {
	// 		printf( "\tsample[%i]: %f\n", i, samples[i] );
	// 	}
	// }

	// // Parse any pending commands.
	// // parserLoop( );

	// ESP_LOGD(LTAG, "end of main_loop, restarting timer");
	// timer_start( TIMER_GROUP_0, TIMER_0 );
}


// NOTE: Cpp might not be the best idea
// http://bbs.esp32.com/viewtopic.php?t=362
extern "C" void app_main( void ) {
	// logging init
	log_init( );
	ESP_LOGD(LTAG, "notice; debug log levels are enabled");	
	ESP_LOGI(LTAG, "notice; info  log levels are enabled");	

	// complete startup time
	gettimeofday( &time_start_tv, NULL );
	time_start_millis = time_start_tv.tv_usec * 1000;

	// Set up ADC and audio input.
	adc1_config_width( ANALOG_READ_RESOLUTION );
	adc1_config_channel_atten( AUDIO_INPUT_PIN, ADC_ATTEN_DB_0 );

	// Turn on the power indicator LED.
	// pinMode(POWER_LED_PIN, OUTPUT);
	// digitalWrite(POWER_LED_PIN, HIGH);

	// Clear the input command buffer
	memset( commandBuffer, 0, sizeof( commandBuffer ) );

	ESP_LOGD(LTAG, "amidst setup()\n" );

	// Initialize spectrum display
	spectrumSetup( );

	// connect to WiFi
	net_wifi_connect( );

	// setup timer for periodic
	timer_event_t timer_hz = {};
	timer_hz.timer_group = TIMER_HZ_GROUP;
	timer_hz.timer_idx   = TIMER_HZ_IDX;
	timer_setup( 10 );
	timer_init_group_0( timer_hz.timer_idx, EVENT_ONE_HZ_COLLECTED, TIMER_INTERVAL_HZ );

	if ( wifi_get_local_ip() ) {
		// no Errors? Let's begin!
		// NOTE: should actually check if I have an IP ADDRESS
		// begin main loop
		xTaskCreate( &main_loop, "main_loop", 4096, NULL, 5, NULL );
	}
}

#undef LTAG
