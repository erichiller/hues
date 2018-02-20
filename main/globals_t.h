#ifndef GLOBAL_T_H
#define GLOBAL_T_H

#include "driver/timer.h"



/*
 * Timer Event
 *
 * structure to pass events
 * from the timer interrupt handler to the main program.
 */
typedef struct
{
	int			  type;			  // the type of timer's event
	timer_group_t timer_group;	// enum , int
	timer_idx_t   timer_idx;	  // enum , int
	uint64_t	  timer_counter_value;
} timer_event_t;


#endif