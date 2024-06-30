
#ifndef GRM_H_
#define GRM_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"

// Handle GPIO interrupt
void GRM_new_pulse(void);

// returns the number of captured impulses
uint32_t GRM_get_counter(void);

#endif /* GRM_H_ */
