/*
 * i2c_slave.c
 *
 *  Created on: Jul 1, 2024
 *      Author: jojo
 */


#include "i2c_slave.h"
#include "ringbuffer.h"
#include <stm32f1xx_hal_i2c.h>

uint8_t dummy_in;
RINGBUFFER_DataItem data;

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode){
	if (TransferDirection == I2C_DIRECTION_TRANSMIT)  // if the master wants to transmit the data
		{
			//HAL_I2C_Slave_Seq_Receive_IT(hi2c, &dummy_in, 1, I2C_FIRST_FRAME);
			RINGBUFFER_reset();
		}
	else{
		RINGBUFFER_dequeue(&data);
		//TODO: put into dataOutArr
		HAL_I2C_Slave_Seq_Transmit_IT(hi2c, (uint8_t*)&data, 9, I2C_FIRST_FRAME);
	}
}
