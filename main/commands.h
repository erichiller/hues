////////////////////////////////////////////////////////////////////////////////
// COMMAND PARSING FUNCTIONS
// These functions allow parsing simple commands input on the serial port.
// Commands allow reading and writing variables that control the device.
//
// All commands must end with a semicolon character.
//
// Example commands are:
// GET SAMPLE_RATE_HZ;
// - Get the sample rate of the device.
// SET SAMPLE_RATE_HZ 400;
// - Set the sample rate of the device to 400 hertz.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef COMMANDS_H
#define COMMANDS_H

#define LOGT "Commands"

#include "ctype.h"
#include "time_intr.h"
#include "config.h"
#include "mbed.h"
#include "net_wifi.h"
#include "spectrum.h"
#include "esp_log.h"

// #include "esp_console.h"
// #include "esp_vfs_dev.h"
// #include "esp_log.h"
// #include "driver/uart.h"

// void console_init( ) {
// 	/* Disable buffering on stdin and stdout */
// 	setvbuf( stdin, NULL, _IONBF, 0 );
// 	setvbuf( stdout, NULL, _IONBF, 0 );

// 	/* Minicom, screen, idf_monitor send CR when ENTER key is pressed */
// 	esp_vfs_dev_uart_set_rx_line_endings( ESP_LINE_ENDINGS_CR );
// 	/* Move the caret to the beginning of the next line on '\n' */
// 	esp_vfs_dev_uart_set_tx_line_endings( ESP_LINE_ENDINGS_CRLF );

// 	/* Install UART driver for interrupt-driven reads and writes */
// 	ESP_ERROR_CHECK( uart_driver_install( (uart_port_t)CONFIG_CONSOLE_UART_NUM,
// 										  256,
// 										  0,
// 										  0,
// 										  NULL,
// 										  0 ) );

// 	/* Tell VFS to use UART driver */
// 	esp_vfs_dev_uart_use_driver( CONFIG_CONSOLE_UART_NUM );

// 	/* Initialize the console */
// 	esp_console_config_t console_config = {
// 		.max_cmdline_args   = 8,
// 		.max_cmdline_length = 256,
// #if CONFIG_LOG_COLORS
// 		.hint_color = atoi( LOG_COLOR_CYAN )
// #endif
// 	};
// 	ESP_ERROR_CHECK( esp_console_init( &console_config ) );
// }


const int MAX_CHARS = 65; // Max size of the input command buffer
char	  commandBuffer[MAX_CHARS];

/* Command Functions */

int command_hue_start( void ) {
	hue_mbed_open_dtls( );
	printf( "hue_mbed_open_dtls() has been called\n" );
	return 0;
}


/* Main Functions for Parsing */


// Macro used in parseCommand function to simplify parsing get and set commands for a variable
#define SET( variableName )                                  \
	else if( strstr( command, "SET " #variableName " " ) != NULL ) { \
		variableName = (typeof( variableName ))atof(                 \
			command + ( sizeof( "SET " #variableName " " ) - 1 ) );  \
	}

void parseCommand( char *command ) {
	ESP_LOGD(LOGT, "Command received: %s\n", command );

	if( strcmp( command, "GET SAMPLES" ) == 0 ) {
		for( int i = 0; i < FFT_SIZE; ++i ) {
			printf( "samples[%i]=%f", i, samples[i] );
		}
	} else if( strcmp( command, "GET FFT_SIZE" ) == 0 ) {
		printf( "FFT size=%i\n", FFT_SIZE );
	} else if( strcmp( command, "PAUSE" ) == 0 ) {
		timer_pause( timer_hz.timer_group, timer_hz.timer_idx );
		printf( "timer stopped.\n" );
	} else if( strcmp( command, "RESUME" ) == 0 ) {
		timer_start( timer_hz.timer_group, timer_hz.timer_idx );
		printf( "timer resumed.\n" );
	} else if( strcmp( command, "GET FFT_VERSION" ) == 0 ) {
		printf( "ArduinoFFT library is Revision: %i \n",  FFT.Revision( ) );
	} else if( strcmp( command, "GET IP" ) == 0 ) {
		net_wifi_status_print( );
	} else if( strcmp( command, "HUE START" ) == 0 ) {
		command_hue_start( );
	} 
	else if( strcmp( command, "GET SAMPLE_RATE_HZ" ) == 0 ) {
		printf( "SAMPLE_RATE_HZ=%i", SAMPLE_RATE_HZ );
	}
	SET( SAMPLE_RATE_HZ )
	else if( strcmp( command, "GET SPECTRUM_MIN_DB" ) == 0 ) {
		printf( "SPECTRUM_MIN_DB=%f", SPECTRUM_MIN_DB );
	}
	SET( SPECTRUM_MIN_DB )
	else if( strcmp( command, "GET SAMPLE_RATE_HZ" ) == 0 ) {
		printf( "SPECTRUM_MAX_DB=%f", SPECTRUM_MAX_DB );
	}
	SET( SPECTRUM_MAX_DB )

	// Update spectrum display values if sample rate was changed.
	if( strstr( command, "SET SAMPLE_RATE_HZ " ) != NULL ) {
		spectrumSetup( );
	}
}


void parserLoop( ) {
	// Process any incoming characters from the serial port

	// while( !feof(stdin) ) {
	// 	char c = getc(stdin);
	// 	// Add any characters that aren't the end of a command (semicolon) to the input buffer.
	// 	if( c != ';' ) {
	// 		c = toupper( c );
	// 		strncat( commandBuffer, &c, 1 );
	// 	} else {
	// 		// Parse the command because an end of command token was encountered.
	// 		parseCommand( commandBuffer );
	// 		// Clear the input buffer
	// 		memset( commandBuffer, 0, sizeof( commandBuffer ) );
	// 	}
	// }
}

#undef LOGT

#endif