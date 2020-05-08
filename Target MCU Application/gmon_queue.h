/*
 * gmon_queue.h
 *
 *  Created on: Feb 20, 2020
 *      Author: Michael D'Argenio
 */

#ifndef PROFILING_GMON_QUEUE_H_
#define PROFILING_GMON_QUEUE_H_

#include <stdint.h>

#define MAX_Q_SIZE	64   /* number of elements in the buffer */

void TxQ_Init();
uint16_t TxQ_Size();
uint8_t TxQ_Enqueue(uint8_t data);
uint8_t TxQ_Dequeue();

#endif /* PROFILING_GMON_QUEUE_H_ */
