/*
 * gmon_queue.c
 * Handles serial data queueing for gprof implementation.
 *  Created on: Feb 20, 2020
 *      Author: Michael D'Argenio
 */

#include <gmon_queue.h>
#include "CS1.h"

static uint8_t TxQ[MAX_Q_SIZE]; /* ring queue data buffer */
static uint16_t TxQHead; /* queue head index */
static uint16_t TxQTail; /* queue tail index */
static uint16_t TxQSize; /* queue size data */

/* initializes transmit queue */
void TxQ_Init() {
	TxQHead = 0;
	TxQTail = 0;
	TxQSize = 0;
}

/* gets the queue size */
uint16_t TxQ_Size() {
  return TxQSize;
}

/* enqueues 1 byte of data for transmission */
uint8_t TxQ_Enqueue(uint8_t data) {

	// if queue is full, return error
  if (TxQSize==MAX_Q_SIZE) {
    return 0; // failure
  }
  // else we can enqueue new element
  else {
		// save current masking state
    // disable interrupts
    EnterCritical();
    // store data, increment tail
    TxQ[TxQTail++] = data;
    TxQTail %= MAX_Q_SIZE;
    // increment size
    TxQSize++;
    // restore interrupt masking state
    ExitCritical();
    return 1; // success
  }
}

/* removes 1 byte of data from buffer for transmission */
uint8_t TxQ_Dequeue() {
	uint8_t temp = 0;

	// don't dequeue if queue is empty
  if (TxQSize) {
		// save current masking state
    // disable interrupts
  	EnterCritical();
  	// remove data
  	temp = TxQ[TxQHead];
  	// increment head and decrement size
  	// unused entries for debugging
  	TxQHead++;
  	TxQHead %= MAX_Q_SIZE;
  	TxQSize--;
  	// restore interrupt masking state
  	ExitCritical();
  }
  return temp;
}
