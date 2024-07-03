/*
 * ringbuffer.c
 *
 *  Created on: Jul 1, 2024
 *      Author: jojo
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "ringbuffer.h"

static RingBuffer rb;

void RINGBUFFER_init() {
    rb.head = 0;
    rb.tail = 0;
    rb.size = 0;
}

void RINGBUFFER_reset(){
	RINGBUFFER_init();
}

static bool RINGBUFFER_isFull() {
    return rb.size == BUFFER_SIZE;
}

static bool RINGBUFFER_isEmpty() {
    return rb.size == 0;
}

void RINGBUFFER_enqueue(RINGBUFFER_DataItem data) {
    if (RINGBUFFER_isFull(rb)) {
        rb.tail = (rb.tail + 1) % BUFFER_SIZE;  // Move tail forward to overwrite the oldest data
    } else {
        rb.size++;
    }
    rb.buffer[rb.head] = data;
    rb.head = (rb.head + 1) % BUFFER_SIZE;
}

bool RINGBUFFER_dequeue(RINGBUFFER_DataItem *data) {
    if (RINGBUFFER_isEmpty(rb)) {
        return false;  // Buffer is empty, cannot remove data
    }
    *data = rb.buffer[rb.tail];
    rb.tail = (rb.tail + 1) % BUFFER_SIZE;
    rb.size--;
    return true;
}
