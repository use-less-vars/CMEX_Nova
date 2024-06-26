
#ifndef GRM_H_
#define GRM_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"

// both will be called in the Timer Interrupt
void GRM_handle_timer_overflow(TIM_HandleTypeDef *htim);
void GRM_new_pulse(TIM_HandleTypeDef *htim, uint16_t captured_ticks);

// returns the last captured time between 2 pulses
uint32_t GRM_get_last_time(void);

#endif /* GRM_H_ */
