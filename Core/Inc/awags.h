#ifndef AWAGS_H_
#define AWAGS_H_

#include <stdint.h>
#include <stdbool.h>
#include "assert.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_spi.h"

/* each bit is an additional capacitor
 Bit0->C1 - Bit3->C4
 basic capacitor C0 has 330fF
 C1 = 4x C0
 C2 = 8x C0
 C3 = 32x C0
 C4 = 128x C0*/

typedef enum {
	C0 = 0x00,
	C1 = 0x11,
	C2 = 0x22,
	C3 = 0x44,
	C4 = 0x88
} FB_Capacitors;

typedef enum {
	start_integtation,
	start_ADC,
	stop_integration

}Timer_Routine_Type;

typedef struct __attribute__((packed)) {
	uint16_t ch1_fb_capacity :4;
	uint16_t ch2_fb_capacity :4;
	uint16_t ch3_fb_capacity :4;
	uint16_t ch4_fb_capacity :4;
} Register_low;

static_assert(sizeof(Register_low) == 2, "Register_low should be 2 bytes");

typedef struct __attribute__((packed)) {
	uint16_t dac :10;         // 0x200 is default -> 900mV  | max. 0x3FF -> 1.8V
	uint16_t nc :3;             // not connected
	uint16_t sc :1;             // Sel comp -> not relevant (future use)
	uint16_t ac :1;  // Auto Set -> automatic feedback capacitances can be added
	uint16_t sl :1;             // sven loechner bit -> not relevant
} Register_high;
static_assert(sizeof(Register_high) == 2, "Register_high should be 2 bytes");

typedef struct Awags_data {
	union{
		Register_low low;
		uint16_t low_bytes;
	};
	union{
		Register_high high;
		uint16_t high_bytes;
	};
} Awags_data;

void awags_trigger_execution(void);
void awags_interrupt_routine(void);
uint16_t awags_read_register(bool high_register, bool awags_fb);

void set_feedback_capacitors(FB_Capacitors binary);

void save_ADC_measurement(uint8_t *value_aray, uint8_t size);


#endif /* AWAGS_H_ */
