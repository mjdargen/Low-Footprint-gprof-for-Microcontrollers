/*
 * gmon_arc.c
 * Handles call graph arc sampling for gprof implementation.
 *  Created on: Feb 1, 2020
 *      Author: Michael D'Argenio
 */

#include <gmon_profil.h>
#include <gmon_queue.h>
#include <gmon_serial.h>
#include "MKL25Z4.h"
#include "CPU.h"
#include "PIT_PDD.h"

static uint8_t g_Initialized = 0;

/* start profiling */
void init_gprof() {
  TxQ_Init(); // initialize queue for data transmission
	init_UART2(); // initialize UART port for transmit to host computer
	init_PIT(); // initialize PC sampler
	__EI(); // enable interrupts if not previously enabled
  g_Initialized = 1; // signal to arc handler code that gprof has been initialized
}

/* stop profiling */
void deinit_gprof() {
	PIT_PDD_DisableInterrupt(PIT_BASE_PTR, PIT_PDD_CHANNEL_1); // disable PIT interrupt
  g_Initialized = 0; // signal to arc handler code that gprof has been initialized
}

/* call arc handler */
void _mcount_internal(uint32_t *frompcindex, uint32_t *selfpc) {

	// if gprof has been initialized
	if (g_Initialized) {

		uint8_t bp[8];
		uint32_t* p;
		// add 'A' or 0x41 for call arc sample
		// add 3 byte from address
		p = (uint32_t*) &(bp[4]);
		*p = (0x41000000 | (uint32_t)frompcindex);
		// add 3 byte to address
		p = (uint32_t*) bp;
		*p = ((uint32_t)selfpc << 8);

		// disable interrupt to prevent data pollution
		PIT_PDD_DisableInterrupt(PIT_BASE_PTR, PIT_PDD_CHANNEL_1);
		// enqueue local serial buffer
		for (int i=0; i<7; i++) {
			// enqueue next byte
			// must reverse order
			while (!TxQ_Enqueue(bp[7-i]));
			// make sure transmit interrupt is enabled
			// could have been disabled if queue was emptied
			if (!(UART2_C2 & UART_C2_TIE_MASK)) {
				UART2_C2 |= UART_C2_TIE_MASK;
			}
		}
		//re-enable interrupt
		PIT_PDD_EnableInterrupt(PIT_BASE_PTR, PIT_PDD_CHANNEL_1);
	}
}
