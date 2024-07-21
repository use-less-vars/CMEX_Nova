/*
 * i2c_slave.c
 *
 *  Created on: Jul 1, 2024
 *      Author: jojo
 */


#include "i2c_slave.h"
#include "ringbuffer.h"
#include <stm32f1xx_hal.h>
#include "main.h"
#include <string.h>

#define DATA_SIZE 9



uint8_t data_buffer_empty = 0;
uint8_t data_count = 0;
uint8_t data_buffer[DATA_SIZE];

//testing stuff
uint8_t dummy_in;
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

uint8_t i2c_data_buffer_empty(){
	return data_buffer_empty;
}
void i2c_set_data_buffer_state(i2c_buffer_state_t empty){
	data_buffer_empty = empty;
	data_count = 0;
	memset(data_buffer, 0, DATA_SIZE);
}

void i2c_load_data_to_buffer(uint8_t* data){
	memcpy(data_buffer, data, DATA_SIZE);
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
  uint8_t already_sent = 0;

  call_count++;
  count++;
  debug_arr[count]=0xFF;

  //HAL_GPIO_TogglePin(TEST_GPIO_Port, TEST_Pin);

  if(IS_BIT_SET(sr1itflags,ADDR_FLAG)){
	  //Slave has just been addressed. Load Data to the DR reg.
	  hi2c->Instance->DR = data_buffer[0];
	  data_count = 0;
	  data_count++;
	  already_sent = 1;


	  count++;
	  debug_arr[count] = 1;

  }
  if(IS_BIT_SET(sr1itflags, TXE_FLAG)){
	  if(already_sent == 0){
		  hi2c->Instance->DR = data_buffer[data_count]; //write data to DR reg as it has been loaded to shift reg
		  already_sent = 1;
		  data_count = data_count >=DATA_SIZE-1 ? DATA_SIZE-1 : data_count+1;
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
	  //hi2c->Instance->SR1 = 1 << AF_FLAG;
  }
  if(IS_BIT_SET(sr1itflags, OVF_FLAG)){
	  count++;
	  debug_arr[count] = 5;
	  //HAL_GPIO_TogglePin(TEST_GPIO_Port, TEST_Pin);

  }
  if(count > 60){
	  count ++;
	  count --;
  }
  return;

}
