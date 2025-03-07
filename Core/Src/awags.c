#include "awags.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_hal_gpio.h"
#include <stdlib.h>
#include "assert.h"
#include "ringbuffer.h"
#include "adc.h"

#define N_FB_CHANNELS (4)

#define SEL_PIN GPIO_PIN_12
#define CLK_PIN GPIO_PIN_13
#define DI_PIN GPIO_PIN_14		// Data In
#define DO_PIN GPIO_PIN_15		// Data Out
#define RESET_PIN GPIO_PIN_6

#define HIGH GPIO_PIN_SET
#define LOW GPIO_PIN_RESET

#define RESET_TIME (10) //µsec

static Awags_data data_register;
static uint16_t rx_buffer = 0;

extern TIM_HandleTypeDef htim3;


const uint32_t integration_times[] = {20,50,100,200,500,1000};
const uint32_t integration_times_length = (sizeof(integration_times) / sizeof(integration_times[0]));
static uint8_t integration_index = 0;

const FB_Capacitors capacitors[] = {C0,C1,C2,C3,C4};
const uint32_t capacitors_length = (sizeof(capacitors) / sizeof(capacitors[0]));
static uint8_t capacity_index = 0;

static volatile Timer_Routine_Type timer_routine = start_integration;


static_assert((sizeof(integration_times) / sizeof(integration_times[0])) * (sizeof(capacitors) / sizeof(capacitors[0])) == 30, "awags: Das Produkt beider Längen der Arrays muss 24 sein");
static uint16_t adc_measurements[30][3] = {0};
static uint16_t adc_index = 0;

void set_reset(bool state);
void write_awags(Awags_data data, bool high);
void set_feedback_capacitors(FB_Capacitors binary);
Awags_data read_awags(void);
static uint16_t awags_read_register(bool high_register, bool awags_fb);

void safe_best_ADC_value(void);


static void start_timer(uint16_t usec) {
	__HAL_TIM_SET_AUTORELOAD(&htim3, usec);	// set count limit
	__HAL_TIM_SET_COUNTER(&htim3, 0);		//Reset counter register
	HAL_TIM_Base_Start_IT(&htim3);
}

void awags_interrupt_routine(void) {
	switch(timer_routine) {
	case start_integration:
		if (integration_index == 0) {
			set_feedback_capacitors(capacitors[capacity_index]);
			capacity_index ++;
		}
		// trigger start integration impulse
		set_reset(true);
		set_reset(false);
		//start timer
		start_timer(integration_times[integration_index]);	// in µsec
		integration_index ++;


		//set next state
		timer_routine = start_ADC;
		break;
	case start_ADC:
		adc_start_conversion();
		start_timer(RESET_TIME);
		timer_routine = stop_integration;
		break;
	case stop_integration:
		// last measurement finished
		if ((capacity_index >= capacitors_length) &&
				(integration_index >= integration_times_length)) {
			capacity_index = 0;
			integration_index = 0;
			safe_best_ADC_value();
			HAL_TIM_Base_Stop_IT(&htim3);
		}
		else {
			if (integration_index >= integration_times_length) {
				integration_index = 0;
			}
			start_timer(RESET_TIME);
		}
		timer_routine = start_integration;
		break;
	default:
		break;
	}

	//TODO: read in the Datasheet how much time the ADC need to capture the voltage.
	// As backup divide the integraton time into 2 parts:
	// Example 95% -> start ADC & 100% -> Stop integration
}

/*
 * Starts the first integration of the AWAGS
 */
void awags_trigger_execution() {
	static uint16_t test_val = 0;
	Awags_data test_data;
	static uint8_t test_count = 0;
	test_count++;
	uint16_t test_readout = 0;
//	test_data.high_bytes = 0xa400;
	test_data.high.dac = test_val;//test_count %  2 ? 0b0000001000000001 : 0b0000001000000001;
	test_data.low_bytes = 0xFFFFFFFF;
	capacity_index = 0;
	integration_index = 0;
	adc_index = 0;
	timer_routine = start_integration;
	//start manually, next measurements will be triggered by the timer
	//awags_interrupt_routine();
	//test_val = awags_read_register(false, false);
	write_awags(test_data, true);
	HAL_GPIO_WritePin(GPIOC, RESET_PIN, HIGH);
	test_val++;
	test_val = test_val % 1024;
	//test_readout = awags_read_register(true, false);
	//test_readout++;
	HAL_GPIO_WritePin(GPIOB, DO_PIN, HIGH);
	for(uint16_t i=0; i < 60000; i++){
		__asm("nop");
	}
	HAL_GPIO_WritePin(GPIOB, DO_PIN, LOW);
	adc_start_conversion();
}

void save_ADC_measurement(uint8_t *value_array, uint8_t size) {
	int16_t data[4] = {0};
	double data_double[4];
    // Transform the uint8_t array to uint16_t array
    for (size_t i = 0; i < size; i += 2) {
        uint16_t highByte = value_array[i+1];
        uint16_t lowByte = value_array[i];
        data[i / 2] = (highByte << 8) | lowByte;
        data_double[i/2] = ((double)data[i/2])*5000.0/32767.0;

    }
    for (uint32_t i = 0; i < 3; i++) {
		// check if the index is inside the array
		if(adc_index <(sizeof(adc_measurements) / sizeof(adc_measurements[0]))) {
			adc_measurements[adc_index][i] = data[i];
			adc_index ++;
		}
    }
}

void safe_best_ADC_value(void) {
	const uint16_t target_value = UINT16_MAX/2;
	uint16_t best_index = 0;
	for (uint32_t j = 0; j < 3; j++) {
		uint16_t best_dist = UINT16_MAX;

		for ( uint16_t i = 0; i < (integration_times_length*capacitors_length); i++) {
			if (abs(target_value-adc_measurements[i][j]) < abs(target_value-best_dist)) {
				best_dist = adc_measurements[i][j];
				best_index = i;
			}
		}
		// get index of best capacity and integration time
		uint8_t integration_setting = best_index % capacitors_length;
		uint8_t capacity_setting = best_index / capacitors_length;
		//write data & settings into ring buffer
		RINGBUFFER_DataItem item;
		item.timestamp = HAL_GetTick();
		item.type = AWAGS_data_ch0;
		item.data.awags.value = best_dist;
		item.data.awags.capacity_type = capacity_setting;
		item.data.awags.integration_type = integration_setting;
		item.data.awags.adc_channel = j;
		RINGBUFFER_enqueue(item);
	}
	uint8_t integration_setting = best_index % capacitors_length;
	uint8_t capacity_setting = best_index / capacitors_length;
	//write best dist into ring buffer

}



void write_awags(Awags_data data, bool high) {
	//
	//TODO: Chip select high
	__disable_irq();

	uint16_t tx_buffer = 0;
	if (high) {
		tx_buffer = data.high_bytes;
	} else {
		tx_buffer = data.low_bytes;
	}
	// WARNING, DANGER, EXPLOSION, CLK and ADI are hardware-wise inverted, so software must invert them again.
	// So a CLK=HIGH is actually CLK = LOW on the AWAGS Chip. Same with ADI.
	// Disable Interrupts
	HAL_GPIO_WritePin(GPIOB, CLK_PIN, HIGH);
	HAL_GPIO_WritePin(GPIOB, SEL_PIN, LOW);
	HAL_GPIO_WritePin(GPIOB, DO_PIN, HIGH);
	HAL_GPIO_WritePin(GPIOC, RESET_PIN, LOW);

	HAL_GPIO_WritePin(GPIOB, SEL_PIN, HIGH);

	for (uint8_t i = 0; i <= 15; i++) {
		HAL_GPIO_WritePin(GPIOB, DO_PIN, (GPIO_PinState)(~(tx_buffer >>(i)) & 0x1));
		HAL_GPIO_WritePin(GPIOB, CLK_PIN, LOW);

		if (i == 15) {
			HAL_GPIO_WritePin(GPIOC, RESET_PIN, (GPIO_PinState) high);
			HAL_GPIO_WritePin(GPIOB, SEL_PIN, LOW);
			HAL_GPIO_WritePin(GPIOB, CLK_PIN, HIGH);
		}
		else {
			HAL_GPIO_WritePin(GPIOB, CLK_PIN, HIGH);
		}
	}
	HAL_GPIO_WritePin(GPIOC, RESET_PIN, LOW);
	HAL_GPIO_WritePin(GPIOB, DO_PIN, HIGH);
	__enable_irq();
	// Enable Inerrupts

}

uint16_t awags_read_register(bool high_register, bool awags_fb) {
	// Disable interrupts
	// !!! Fast Mode does not work. Always do slow read out -> awags_fb must be false!
	rx_buffer = 0;
	uint8_t bit = 0;
	HAL_GPIO_WritePin(GPIOB, CLK_PIN, HIGH);
	HAL_GPIO_WritePin(GPIOB, SEL_PIN, LOW);
	HAL_GPIO_WritePin(GPIOB, DO_PIN, LOW);

	// WARNING, DANGER, EXPLOSION, CLK and ADI are hardware-wise inverted, so software must invert them again.
	// So a CLK=HIGH is actually CLK = LOW on the AWAGS Chip. Same with ADI.

	if (awags_fb) { //fast mode: only FB-reg readable in fast mode (FB means capacitor-setting
		HAL_GPIO_WritePin(GPIOB, CLK_PIN, HIGH);
	} else { // slow mode: both high and low reg readable in slow-mode,
			 //rst-pin = high means high-reg (dac-settings), rst-pin = low means FB (capacitor-settings-reg)
		HAL_GPIO_WritePin(GPIOB, CLK_PIN, LOW); // clk = high at sel rising-edge means slow read-out, low means fast read out
		HAL_GPIO_WritePin(GPIOC, RESET_PIN, high_register);	// reset pin in slow mode decides which register to read (high = high-reg and vice versa)
	}

	HAL_GPIO_WritePin(GPIOB, SEL_PIN, HIGH);	//start read

	if(awags_fb) {
		HAL_GPIO_WritePin(GPIOC, RESET_PIN, HIGH);
	}
	HAL_GPIO_WritePin(GPIOC, RESET_PIN, LOW);
	HAL_GPIO_WritePin(GPIOB, CLK_PIN, HIGH);
	HAL_GPIO_WritePin(GPIOB, SEL_PIN, LOW);

	for(uint8_t i = 0;i <= 15; i++ ) {
		if ( i > 0)  {
			HAL_GPIO_WritePin(GPIOB, CLK_PIN, LOW);
			HAL_GPIO_WritePin(GPIOB, CLK_PIN, HIGH);
		}
		bit = HAL_GPIO_ReadPin(GPIOB, DI_PIN);
		rx_buffer = rx_buffer | (bit <<i);
	}
	// Enable Interrupts
	return rx_buffer;

}

/**
 * @brief Set the feedback capacitors of the awags
 * 
 * @param binary 4bits for C1,C2,C3,C4
 */
void set_feedback_capacitors(FB_Capacitors binary) {

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
void set_dac(uint32_t mili_voltage) {
	if (mili_voltage > 1800) {
		mili_voltage = 1800;
	}
	uint16_t dac_converted = ((mili_voltage * 1023) / 1800); //1800 --> 0x3FF (10 bits) WRONG!!!!
	data_register.high.dac = dac_converted;
	write_awags(data_register, true);
}

void set_auto(bool state) {
	data_register.high.ac = state;
	write_awags(data_register,true);
}

void set_reset(bool state) {
	HAL_GPIO_WritePin(GPIOC, RESET_PIN, state);
}
