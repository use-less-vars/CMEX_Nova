
#include "GRM.h"

static uint32_t overflow_counter = 0;
uint32_t time = 0;

/*
Trigger on rising Edge
--> save actual timing
--> resets timer + extra variable

10kHz timer -> 
max value will trigger 3333/sec
min value will trigger once every 2 min --> 0.00833/sec

to catch the minimum values we add an extra counter
that increment a variable the timer reaches its limit (65536)

the full time can be calculated by (CNT + 65536 * counter)
*/

void GRM_new_pulse(uint16_t captured_ticks) {
    time = captured_ticks + 65535 * overflow_counter;
	__HAL_TIM_SET_COUNTER(htim,0);
}

void GRM_handle_timer_overflow(TIM_HandleTypeDef *htim) 
{
    __HAL_TIM_SET_COUNTER(htim,0);
    overflow_counter ++;
    
}

uint32_t GRM_get_last_time(void) {
	return time;
}

