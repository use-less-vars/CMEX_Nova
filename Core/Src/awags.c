#include "awags.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"

#define N_FB_CHANNELS (4)

static Awags_data data_register;

extern TIM_HandleTypeDef htim3;

void set_reset(bool state);
void write_awags(Awags_data data);
Awags_data read_awags(void);

//TODO: Timer, um die wartezeiten individuell zu setzen

static void start_timer(uint16_t usec) {
	__HAL_TIM_SET_COUNTER(&htim3, usec);
	HAL_TIM_Base_Start_IT(&htim3);
}

void awags_interrupt_routine(void) {

	//TODO: start ADC
	set_reset(true);
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

void trigger_execution(void) {
	set_reset(true);    // trigger impulse
	set_reset(false);

	start_timer(80);
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

	write_awags(data_register);

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
	write_awags(data_register);
}

void set_auto(bool state) {
	data_register.high.ac = state;
	write_awags(data_register);
}

void set_reset(bool state) {
	// Pin X write(state);
}
