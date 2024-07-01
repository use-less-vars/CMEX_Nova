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

void initRingBuffer() {
    rb.head = 0;
    rb.tail = 0;
    rb.size = 0;
}

static bool isFull() {
    return rb.size == BUFFER_SIZE;
}

static bool isEmpty() {
    return rb.size == 0;
}

void enqueue(DataItem data) {
    if (isFull(rb)) {
        rb.tail = (rb.tail + 1) % BUFFER_SIZE;  // Move tail forward to overwrite the oldest data
    } else {
        rb.size++;
    }
    rb.buffer[rb.head] = data;
    rb.head = (rb.head + 1) % BUFFER_SIZE;
}

bool dequeue(DataItem *data) {
    if (isEmpty(rb)) {
        return false;  // Buffer is empty, cannot remove data
    }
    *data = rb.buffer[rb.tail];
    rb.tail = (rb.tail + 1) % BUFFER_SIZE;
    rb.size--;
    return true;
}
