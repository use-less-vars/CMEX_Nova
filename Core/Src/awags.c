#include "awags.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_hal_gpio.h"

#define N_FB_CHANNELS (4)

#define SEL_PIN GPIO_PIN_12
#define CLK_PIN GPIO_PIN_13
#define DI_PIN GPIO_PIN_14		// Data In
#define DO_PIN GPIO_PIN_15		// Data Out
#define RESET_PIN GPIO_PIN_6

#define HIGH GPIO_PIN_SET
#define LOW GPIO_PIN_RESET

static Awags_data data_register;
static uint16_t rx_buffer = 0;

extern TIM_HandleTypeDef htim3;

void set_reset(bool state);
void write_awags(Awags_data data, bool high);
Awags_data read_awags(void);

//TODO: Timer, um die wartezeiten individuell zu setzen

static void start_timer(uint16_t usec) {
	__HAL_TIM_SET_COUNTER(&htim3, usec);
	HAL_TIM_Base_Start_IT(&htim3);
}

void awags_interrupt_routine(void) {

	//TODO: start sample of ADC
	//TODO: read in the Datasheet how much time the ADC need to capture the voltage.
	// As backup divide the integraton time into 2 parts:
	// Example 95% -> start ADC & 100% -> Stop integration
	set_reset(true);
}

void write_awags(Awags_data data, bool high) {
	//
	//TODO: Chip select high
	uint16_t tx_buffer = 0;
	if (high) {
		tx_buffer = (uint16_t) data.high;
	} else {
		tx_buffer = (uint16_t) data.low;
	}

	// Disable Interrupts
	HAL_GPIO_WritePin(GPIOB, CLK_PIN, LOW);
	HAL_GPIO_WritePin(GPIOB, SEL_PIN, LOW);
	HAL_GPIO_WritePin(GPIOB, DO_PIN, LOW);

	HAL_GPIO_WritePin(GPIOB, SEL_PIN, HIGH);

	for (uint8_t i = 0; i <= 15; i++) {
		HAL_GPIO_WritePin(GPIOB, DO_PIN, (GPIO_PinState)((tx_buffer >> i) & 0x1));
		HAL_GPIO_WritePin(GPIOB, CLK_PIN, HIGH);

		if (i == 15) {
			HAL_GPIO_WritePin(GPIOC, RESET_PIN, (GPIO_PinState)high);
			HAL_GPIO_WritePin(GPIOB, SEL_PIN, LOW);
			HAL_GPIO_WritePin(GPIOB, CLK_PIN, LOW);
		}
		else {
			HAL_GPIO_WritePin(GPIOB, CLK_PIN, LOW);
		}
	}
	HAL_GPIO_WritePin(GPIOC, RESET_PIN, LOW);
	// Enable Inerrupts

}

uint16_t awags_read_register(bool high_register, bool awags_fb) {
	// Disable interrupts
	rx_buffer = 0;
	uint8_t bit = 0;
	HAL_GPIO_WritePin(GPIOB, CLK_PIN, LOW);
	HAL_GPIO_WritePin(GPIOB, SEL_PIN, LOW);
	HAL_GPIO_WritePin(GPIOB, DO_PIN, LOW);

	if (awags_fb) {
		HAL_GPIO_WritePin(GPIOB, CLK_PIN, LOW);
	} else {
		HAL_GPIO_WritePin(GPIOB, CLK_PIN, HIGH);
		HAL_GPIO_WritePin(GPIOC, RESET_PIN, HIGH);		// high --> slow readout / low --> fast
	}

	HAL_GPIO_WritePin(GPIOB, SEL_PIN, HIGH);	//start read

	if(awags_fb) {
		HAL_GPIO_WritePin(GPIOC, RESET_PIN, HIGH);
	}
	HAL_GPIO_WritePin(GPIOC, RESET_PIN, LOW);
	HAL_GPIO_WritePin(GPIOB, CLK_PIN, LOW);
	HAL_GPIO_WritePin(GPIOB, SEL_PIN, LOW);

	for(uint8_t i = 0;i <= 15; i++ ) {
		if ( i > 0)  {
			HAL_GPIO_WritePin(GPIOB, CLK_PIN, HIGH);
			HAL_GPIO_WritePin(GPIOB, CLK_PIN, LOW);
		}
		bit = HAL_GPIO_ReadPin(GPIOB, DI_PIN);
		rx_buffer = rx_buffer | (bit <<i);
	}
	// Enable Interrupts
	return rx_buffer;

}

/*
 Loop:
 Reset --> High
 TRIG High --> ADC
 Reset --> Low
 wait (IntegrationTime/2)
 TRIG Low --> read ADC?
 wait (IntegrationTime/2)
 Reset --> High
 wait(100ms)
 */

void trigger_execution(uint16_t integration_time) {
	set_reset(true);    // trigger impulse
	set_reset(false);

	start_timer(integration_time);	// in µsec
}

/**
 * @brief Set the feedback capacitors of the awags
 * 
 * @param binary 4bits for C1,C2,C3,C4
 */
void set_feedback_capacitors(uint8_t binary) {

	binary = binary & 0x00FF; // only the last 4 bit are relevant
	data_register.low.ch1_fb_capacity = binary;
	data_register.low.ch2_fb_capacity = binary;
	data_register.low.ch3_fb_capacity = binary;
	data_register.low.ch4_fb_capacity = binary;

	write_awags(data_register, false);

}

/**
 * @brief Set the dac object
 * 
 * @param voltage in mili volt, min=0, max=1800
 */
void set_dac(uint16_t voltage) {
	if (voltage > 1800) {
		voltage = 1800;
	}
	uint16_t dac_converted = ((voltage * 1023) / 1800); //1800 --> 0x3FF (10 bits)
	data_register.high.dac = dac_converted;
	write_awags(data_register, true);
}

void set_auto(bool state) {
	data_register.high.ac = state;
	write_awags(data_register,true);
}

void set_reset(bool state) {
	// Pin X write(state);
}
