/*
 * i2c_slave.h
 *
 *  Created on: Jul 1, 2024
 *      Author: jojo
 */

#ifndef INC_I2C_SLAVE_H_
#define INC_I2C_SLAVE_H_

#include <stm32f1xx_hal.h>

void i2c_interrupt_answer();
void i2c_start_slave();
void JOJO_InterruptHandler(I2C_HandleTypeDef *hi2c);

#endif /* INC_I2C_SLAVE_H_ */
