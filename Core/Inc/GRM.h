
#ifndef GRM_H_
#define GRM_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"

void GRM_read_timer(TIM_HandleTypeDef *htim);

#endif /* GRM_H_ */
