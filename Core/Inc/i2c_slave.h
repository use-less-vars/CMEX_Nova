/*
 * i2c_slave.h
 *
 *  Created on: Jul 1, 2024
 *      Author: jojo
 */

#ifndef INC_I2C_SLAVE_H_
#define INC_I2C_SLAVE_H_

#include <stm32f1xx_hal.h>

typedef enum{
	I2C_BUFFER_EMPTY = 0,
	I2C_BUFFER_NOT_EMPTY
}i2c_buffer_state_t;

void i2c_interrupt_answer();
void i2c_start_slave();
void JOJO_InterruptHandler(I2C_HandleTypeDef *hi2c);
uint8_t i2c_data_buffer_empty();
void i2c_set_data_buffer_state(i2c_buffer_state_t);
void i2c_load_data_to_buffer(uint8_t* data);

#endif/* INC_I2C_SLAVE_H_ */
