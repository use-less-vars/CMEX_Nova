
#include "GRM.h"
#include <stdint.h>
#include "ringbuffer.h"

static uint32_t impulse_counter = 0;


void GRM_new_pulse(void) {
	impulse_counter ++;
}

void GRM_write_counter_into_ringbuffer(void) {

	RINGBUFFER_DataItem item;
	item.timestamp = HAL_GetTick();
	item.type = GRM_data;
	item.data.data32_t = impulse_counter;
	RINGBUFFER_enqueue(item);
}
