
#include "GRM.h"
#include <stdint.h>

static uint32_t impulse_counter = 0;


void GRM_new_pulse(void) {
	impulse_counter ++;
}

uint32_t GRM_get_counter(void) {
	return impulse_counter;
}
