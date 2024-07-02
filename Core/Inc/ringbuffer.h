/*
 * ringbuffer.h
 *
 *  Created on: Jul 1, 2024
 *      Author: jojo
 */

#ifndef SRC_RINGBUFFER_H_
#define SRC_RINGBUFFER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "assert.h"

#define BUFFER_SIZE 1000  // Size of the ring buffer

// Enum to represent the types
typedef enum {
    AWAGS_data = 0,
    GRM_data = 1
} DataType;

// Struct to represent the data stored in the ring buffer
typedef struct __attribute__((packed)){
    uint32_t timestamp;
    uint8_t type;
    union awags{
    	uint32_t data32_t;
    	struct __attribute__((packed)){
    		uint16_t value;
    		uint8_t integration_type;
    		uint8_t adc_channel:2;
    		uint8_t capacity_type:6;
    	}awags;
    }data;

} DataItem;

static_assert(sizeof(DataItem) == 9, "DataItem should be 9 bytes");

// Ring buffer structure
typedef struct {
    DataItem buffer[BUFFER_SIZE];
    uint16_t head;
    uint16_t tail;
    uint16_t size;
} RingBuffer;

void initRingBuffer();

void enqueue(DataItem data);

bool dequeue(DataItem *data);


#endif /* SRC_RINGBUFFER_H_ */
