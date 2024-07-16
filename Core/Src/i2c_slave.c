/*
 * i2c_slave.c
 *
 *  Created on: Jul 1, 2024
 *      Author: jojo
 */


#include "i2c_slave.h"
#include "ringbuffer.h"
#include <stm32f1xx_hal.h>

uint8_t dummy_in;
RINGBUFFER_DataItem data;
uint8_t data_test[9] = {255};
uint8_t count = 0;
uint8_t call_count = 0;
uint8_t debug_arr[260];

#define IS_BIT_SET(byte, bit) ((byte) & (1 << (bit)))

#define ADDR_FLAG  1
#define BTF_FLAG   2
#define TXE_FLAG   7
#define AF_FLAG   10
#define OVF_FLAG  11

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c){
	//hi2c->Instance->DR = 0b10101010;
	HAL_I2C_EnableListen_IT(hi2c); // Start listening again
}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode){
	static uint8_t a= 0;
//count++;
	if(count >10){
		//count++;
	}
	data_test[5] = 0b11001010;
	if (TransferDirection == I2C_DIRECTION_TRANSMIT)  // if the master wants to transmit the data
		{
			//HAL_I2C_Slave_Seq_Receive_IT(hi2c, &dummy_in, 1, I2C_FIRST_FRAME);
			RINGBUFFER_reset();
		}
	else{
		//RINGBUFFER_dequeue(&data);

		data.timestamp = a;
		a++;
		data.type = 1;
		data_test[4] = a;
		data_test[0] = a;
		HAL_I2C_Slave_Seq_Transmit_IT(hi2c, data_test, 9,I2C_FIRST_FRAME );
	}
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c){
	SET_BIT(hi2c->Instance->CR1 , I2C_CR1_SWRST);
	HAL_I2C_EnableListen_IT(hi2c);
}

void JOJO_InterruptHandler(I2C_HandleTypeDef *hi2c)
{
  uint32_t sr1itflags;
  volatile uint32_t sr2itflags;
  sr1itflags = hi2c->Instance->SR1;
  sr2itflags = hi2c->Instance->SR2;

//  if(sr1itflags = )
//  sr1itflags = hi2c->Instance->SR1;
//  sr1itflags = hi2c->Instance->SR1;
//  sr1itflags = hi2c->Instance->SR1;
  call_count++;
  count++;
  debug_arr[count]=0xFF;
  uint8_t already_sent = 0;
  if(count > 22){
	  //sr1itflags = 5;
  }

  if(IS_BIT_SET(sr1itflags,ADDR_FLAG)){
	  //Slave has just been addressed. Load Data to the DR reg.

	  //sr2itflags = hi2c->Instance->SR2; //read SR2 after new Data has been written to DR to clear ADDR-Flag

	  //sr2itflags = hi2c->Instance->SR2;
	  hi2c->Instance->DR = call_count;
	  already_sent = 1;
	  //sr1itflags = hi2c->Instance->SR1;
	  count++;
	  debug_arr[count] = 1;
	  //return;

  }
  if(IS_BIT_SET(sr1itflags, TXE_FLAG)){
	  if(already_sent == 0){
		  hi2c->Instance->DR = call_count; //write data to DR reg as it has been loaded to shift reg
		  already_sent = 1;
	  }
	  count++;
	  debug_arr[count] = 2;
  }
  if(IS_BIT_SET(sr1itflags, BTF_FLAG)){
	  //hi2c->Instance->DR = 0b11001100;
	  count++;
	  debug_arr[count] = 3;
  }
  if(IS_BIT_SET(sr1itflags, AF_FLAG)){
	  //TODO: Communication done.
	  count++;
	  debug_arr[count] = 4;
	  hi2c->Instance->SR1 = 1 << AF_FLAG;
  }
  if(IS_BIT_SET(sr1itflags, OVF_FLAG)){
	  count++;
	  debug_arr[count] = 5;


  }
  if(count > 60){
	  count ++;
	  count --;
  }
  return;

}
