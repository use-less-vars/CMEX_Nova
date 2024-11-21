/*
 * ADC.c
 *
 *  Created on: Oct 7, 2024
 *      Author: Adrian
 */

#include "main.h"
#include "stm32f1xx_hal_gpio.h"

void adc_start_conversion(void) {
	HAL_GPIO_TogglePin(CONVST_GPIO_Port, CONVST_Pin);
}

void adc_reset(void) {
	HAL_GPIO_WritePin(ADC_RST_GPIO_Port, ADC_RST_Pin, GPIO_PIN_SET);
	__asm__("nop");
	HAL_GPIO_WritePin(ADC_RST_GPIO_Port, ADC_RST_Pin, GPIO_PIN_RESET);

	// deactivate Chip Select (High == deactivated)
	HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin, GPIO_PIN_SET);

}

