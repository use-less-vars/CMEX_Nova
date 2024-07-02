
#ifndef GRM_H_
#define GRM_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"

// Handle GPIO interrupt
void GRM_new_pulse(void);

// returns the number of captured impulses
void GRM_write_counter_into_ringbuffer(void);

#endif /* GRM_H_ */
