/*
 * gmon_serial.h
 *
 *  Created on: Feb 1, 2020
 *      Author: Michael
 */

#ifndef PROFILING_GMON_SERIAL_H_
#define PROFILING_GMON_SERIAL_H_

#include <stdint.h>

#define USE_UART_INTERRUPTS 	(1) // 0 for polled UART communications, 1 for interrupt-driven
#define UART_OVERSAMPLE_RATE 	(16)
#define BUS_CLOCK 						(24e6)
#define SYS_CLOCK							(48e6)

void init_UART2();

#endif /* PROFILING_GMON_SERIAL_H_ */
