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

#include "globals.h"
#include "mbed.h"
#include "timer.h"
#include "wifi_conn.h"
#include "spectrum.h"


const int MAX_CHARS = 65; // Max size of the input command buffer
char      commandBuffer[MAX_CHARS];

/* Command Functions */

int command_hue_start( void ) {
  int i = hue_mbed_open_dtls( );
  println( "hue_mbed_open_dtls() has been called" );
  return 0;
}


/* Main Functions for Parsing */


// Macro used in parseCommand function to simplify parsing get and set commands for a variable
#define GET_AND_SET( variableName )                                            \
  else if( strcmp( command, "GET " #variableName ) == 0 ) {                    \
    println( variableName );                                            \
  }                                                                            \
  else if( strstr( command, "SET " #variableName " " ) != NULL ) {             \
    variableName = (typeof( variableName ))atof(                               \
        command + ( sizeof( "SET " #variableName " " ) - 1 ) );                \
  }

void parseCommand( char *command ) {
#ifdef DEBUG
  printf( "Command received:" );
  println( command );
#endif

  if( strcmp( command, "GET samples" ) == 0 ) {
    for( int i = 0; i < FFT_SIZE; ++i ) {
      println( samples[i] );
    }
  } else if( strcmp( command, "GET SAMPLES" ) == 0 ) {
    for( int i = 0; i < FFT_SIZE * 2; i += 2 ) {
      println( samples[i] );
    }
  } else if( strcmp( command, "GET FFT_SIZE" ) == 0 ) {
    println( FFT_SIZE );
  } else if( strcmp( command, "PAUSE" ) == 0 ) {
    timerAlarmDisable( timer );
    println( "timer stopped." );
  } else if( strcmp( command, "RESUME" ) == 0 ) {
    timerAlarmEnable( timer );
    println( "timer resumed." );
  } else if( strcmp( command, "GET FFT_VERSION" ) == 0 ) {
    timerAlarmEnable( timer );
    printf( "ArduinoFFT library is Revision " );
    println( FFT.Revision( ) );
  } else if( strcmp( command, "GET IP" ) == 0 ) {
    net_wifi_status_print( );
  } else if( strcmp( command, "HUE START" ) == 0 ) {
    command_hue_start( );
  }
  GET_AND_SET( SAMPLE_RATE_HZ )
  GET_AND_SET( LEDS_ENABLED )
  GET_AND_SET( SPECTRUM_MIN_DB )
  GET_AND_SET( SPECTRUM_MAX_DB )

  // Update spectrum display values if sample rate was changed.
  if( strstr( command, "SET SAMPLE_RATE_HZ " ) != NULL ) { spectrumSetup( ); }
}


void parserLoop( ) {
  // Process any incoming characters from the serial port
  while( Serial.available( ) > 0 ) {
    char c = Serial.read( );
    // Add any characters that aren't the end of a command (semicolon) to the input buffer.
    if( c != ';' ) {
      c = toupper( c );
      strncat( commandBuffer, &c, 1 );
    } else {
      // Parse the command because an end of command token was encountered.
      parseCommand( commandBuffer );
      // Clear the input buffer
      memset( commandBuffer, 0, sizeof( commandBuffer ) );
    }
  }
}

#endif