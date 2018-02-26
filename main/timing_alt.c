

#if !defined( MBEDTLS_CONFIG_FILE )
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined( MBEDTLS_SELF_TEST ) && defined( MBEDTLS_PLATFORM_C )
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf printf
#endif



#include <stdint.h>
// #include <sys/types.h>
#include <sys/time.h>
// #include <signal.h>
// #include <time.h>
#include "esp_log.h"
#include "config.h"

struct _hr_time
{
	struct timeval start;
};



#if !defined( HAVE_HARDCLOCK )

#define HAVE_HARDCLOCK


#define LOGT LOG_TAG_MBED_TIME

static int            hardclock_init = 0;
static struct timeval tv_init;



// returns microseconds since program start
unsigned long mbedtls_timing_hardclock( void ) {
	struct timeval tv_cur;

	ESP_LOGV( LOGT, "mbedtls_timing_hardclock called" );

	if( hardclock_init == 0 ) {
		gettimeofday( &tv_init, NULL );
		hardclock_init = 1;
	}

	gettimeofday( &tv_cur, NULL );
	return ( ( tv_cur.tv_sec - tv_init.tv_sec ) * 1000000 + ( tv_cur.tv_usec - tv_init.tv_usec ) );
}
#endif /* !HAVE_HARDCLOCK */

// volatile int mbedtls_timing_alarmed = 0;

// static void sighandler( int signum )
// {
//     mbedtls_timing_alarmed = 1;
//     signal( signum, sighandler );
// }

// void mbedtls_set_alarm( int seconds )
// {
//     mbedtls_timing_alarmed = 0;
//     signal( SIGALRM, sighandler );
//     alarm( seconds );
// }



/**
 * \brief          timer structure
 */
struct mbedtls_timing_hr_time
{
	unsigned char opaque[32];
};
/**
 * \brief          Context for mbedtls_timing_set/get_delay()
 */
typedef struct
{
	struct mbedtls_timing_hr_time timer;
	uint32_t                      int_ms;
	uint32_t                      fin_ms;
} mbedtls_timing_delay_context;


unsigned long mbedtls_timing_get_timer( struct mbedtls_timing_hr_time *val, int reset ) {
	unsigned long    delta;
	struct timeval   offset;
	struct _hr_time *t = (struct _hr_time *)val;


	gettimeofday( &offset, NULL );

	if( reset ) {
		t->start.tv_sec  = offset.tv_sec;
		t->start.tv_usec = offset.tv_usec;
		ESP_LOGD( LOGT, "\t GET_T \t mbedtls_timing_get_timer [RETURN 0] =============> RESET" );

		return ( 0 );
	}

	delta = ( offset.tv_sec - t->start.tv_sec ) * 1000 + ( offset.tv_usec - t->start.tv_usec ) / 1000;

	ESP_LOGD( LOGT, "\t GET_T \t mbedtls_timing_get_timer [RETURN delta] =========> DELTA=%lu", delta );

	return ( delta );
}


/**
 * Set delays to watch
 * 
 * \brief          Set a pair of delays to watch
 *                 (See \c mbedtls_timing_get_delay().)
 *
 * \param data     Pointer to timing data
 *                 Must point to a valid \c mbedtls_timing_delay_context struct.
 * \param int_ms   First (intermediate) delay in milliseconds.
 * \param fin_ms   Second (final) delay in milliseconds.
 *                 Pass 0 to cancel the current delay.
 */
void mbedtls_timing_set_delay( void *data, uint32_t int_ms, uint32_t fin_ms ) {
	mbedtls_timing_delay_context *ctx = (mbedtls_timing_delay_context *)data;

	ESP_LOGD( LOGT, "SET \t\t mbedtls_timing_set_delay =============>\n"
	                "\t int_ms=%u \n"
	                "\t fin_ms=%u \n",
	          int_ms,
	          fin_ms );

	ctx->int_ms = int_ms;
	ctx->fin_ms = fin_ms;

	if( fin_ms != 0 ) {
		ESP_LOGD( LOGT, "SET \t\t mbedtls_timing_set_delay =============> CALLING mbedtls_timing_get_timer to RESET" );
		(void)mbedtls_timing_get_timer( &ctx->timer, 1 );
	}
}

/**
 * Get number of delays expired
 * 
 * \brief          Get the status of delays
 *                 (Memory helper: number of delays passed.)
 *
 * \param data     Pointer to timing data
 *                 Must point to a valid \c mbedtls_timing_delay_context struct.
 *
 * \return         -1 if cancelled (fin_ms = 0)
 *                  0 if none of the delays are passed,
 *                  1 if only the intermediate delay is passed,
 *                  2 if the final delay is passed.
 */
int mbedtls_timing_get_delay( void *data ) {
	mbedtls_timing_delay_context *ctx = (mbedtls_timing_delay_context *)data;
	unsigned long                 elapsed_ms;

	if( ctx->fin_ms == 0 ) {
		ESP_LOGW( LOGT, "\t GET \t mbedtls_timing_get_delay \t !! CANCELLED [RETURN -1] ===> ctx->fin_ms is 0 ( %u )", ctx->fin_ms );
		return ( -1 );
	}

	elapsed_ms = mbedtls_timing_get_timer( &ctx->timer, 0 );


	if( elapsed_ms >= ctx->fin_ms ) {
		ESP_LOGW( LOGT, "\t GET \t mbedtls_timing_get_delay \t !! final_delay_passed [RETURN 2]  ===> elapsed_ms( %lu ) >= ctx->fin_ms ( %u )", elapsed_ms, ctx->fin_ms );
		return ( 2 );
	}
	if( elapsed_ms >= ctx->int_ms ) {
		ESP_LOGW( LOGT, "\t GET \t mbedtls_timing_get_delay \t !! intermediate delay passed [RETURN 1]  ===> elapsed_ms( %lu ) >= ctx->int_ms ( %u )", elapsed_ms, ctx->int_ms );
		return ( 1 );
	}

	ESP_LOGD( LOGT, "\t GET \t mbedtls_timing_get_delay [RETURN 0]  =============> elapsed_ms=%lu", elapsed_ms );
	return ( 0 );
}

#undef LOGT
