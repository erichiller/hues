#ifndef TIMER_H
#define TIMER_H

#include <stdio.h>
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "soc/timer_group_struct.h"

#include "driver/periph_ctrl.h"
#include "driver/timer.h"

#include "config.h"
#include "globals.h"

#define LTAG "time_intr"

#define TIMER_SCALE 80							   //  Hardware timer clock divider
#define TIMER_INTERVAL_HZ ( TIMER_SCALE / SAMPLE_RATE_HZ ) // sample test interval for the first timer
#define EVENT_ONE_HZ_COLLECTED 1					   // testing will be done without auto reload

xQueueHandle timer_queue;

/*
 * A simple helper function to print the raw timer counter value
 * and the counter value converted to seconds
 */
static void inline print_timer_counter( uint64_t counter_value ) {
	printf( "Counter: 0x%08x%08x\n", ( uint32_t )( counter_value >> 32 ), ( uint32_t )( counter_value ) );
	printf( "Time   : %.8f µs\n", (double)counter_value / TIMER_SCALE );
}



/*
 * Timer group0 ISR handler
 *
 * Note:
 * We don't call the timer API here because they are not declared with IRAM_ATTR.
 * If we're okay with the timer irq not being serviced while SPI flash cache is disabled,
 * we can allocate this interrupt without the ESP_INTR_FLAG_IRAM flag and use the normal API.
 */
void IRAM_ATTR timer_group0_isr( void *para ) {
	int timer_idx = (int) para;

	/* Retrieve the interrupt status and the counter value
       from the timer that reported the interrupt */
	uint32_t intr_status			   = TIMERG0.int_st_timers.val;
	TIMERG0.hw_timer[timer_idx].update = 1;
	uint64_t timer_counter_value =
		( (uint64_t)TIMERG0.hw_timer[timer_idx].cnt_high ) << 32 | TIMERG0.hw_timer[timer_idx].cnt_low;

	/* Prepare basic event data
       that will be then sent back to the main program task */
	timer_event_t evt;
	evt.timer_group			= (timer_group_t) 0;
	evt.timer_idx			= (timer_idx_t) timer_idx;
	evt.timer_counter_value = timer_counter_value;

	/* Clear the interrupt
       and update the alarm time for the timer with without reload */
	if( ( intr_status & BIT( timer_idx ) ) && timer_idx == TIMER_0 ) {
		evt.type				  = EVENT_ONE_HZ_COLLECTED;
		TIMERG0.int_clr_timers.t0 = 1;

	/* After the alarm has been triggered
      we need enable it again, so it is triggered the next time */
	TIMERG0.hw_timer[timer_idx].config.alarm_en = TIMER_ALARM_EN;


		// } else if( ( intr_status & BIT( timer_idx ) ) && timer_idx == TIMER_1 ) {
		// 	evt.type				  = TEST_WITH_RELOAD;
		// 	TIMERG0.int_clr_timers.t1 = 1;
	} else {
		evt.type = -1; // not supported event type
	}

	/* After the alarm has been triggered
      we need enable it again, so it is triggered the next time */
	// TIMERG0.hw_timer[timer_idx].config.alarm_en = TIMER_ALARM_EN;

	/* Now just send the event data back to the main program task */
	xQueueSendFromISR( timer_queue, &evt, NULL );
}





void timer_setup( int queue_size ) {
	ESP_LOGI(LTAG, "timer_setup" );
	/** timer init **/
	timer_queue = xQueueCreate( queue_size, sizeof( timer_event_t ) );
}

/*
 * Initialize selected timer of the timer group 0
 *
 * timer_idx - the timer number to initialize
 * auto_reload - should the timer auto reload on alarm?
 * timer_interval_microseconds - the interval of alarm to set
 */
static void timer_init_group_0( timer_idx_t timer_idx,
								bool		auto_reload,
								double		timer_interval_microseconds ) {
	/* Select and initialize basic parameters of the timer */
	timer_config_t config;
	config.divider	 = TIMER_SCALE;
	config.counter_dir = TIMER_COUNT_UP;
	config.counter_en  = TIMER_PAUSE;
	config.alarm_en	= TIMER_ALARM_EN;
	config.intr_type   = TIMER_INTR_LEVEL;
	config.auto_reload = auto_reload;
	timer_init( TIMER_GROUP_0, timer_idx, &config );

	/* Timer's counter will initially start from value below.
       Also, if auto_reload is set, this value will be automatically reload on alarm */
	timer_set_counter_value( TIMER_GROUP_0, timer_idx, 0x00000000ULL );

	/* Configure the alarm value and the interrupt on alarm. */
	timer_set_alarm_value( TIMER_GROUP_0, timer_idx, timer_interval_microseconds);
	timer_enable_intr( TIMER_GROUP_0, timer_idx );
	timer_isr_register( TIMER_GROUP_0, timer_idx, timer_group0_isr, (void *)timer_idx, ESP_INTR_FLAG_IRAM, NULL );

}




#undef LTAG

#endif