/*
 * gmon_serial.c
 * Handles serial interface used for gprof data transmission.
 *  Created on: Feb 1, 2020
 *      Author: Michael
 */

#include <gmon_queue.h>
#include <gmon_serial.h>
#include <stdint.h>
#include <MKL25Z4.H>
#include "UART_PDD.h"
#include "CPU.h"

/* initialize UART serial interface */
void init_UART2() {
	// Enable clock gating for UART0 and Port A
	SIM_BASE_PTR->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM_BASE_PTR->SCGC5 |= SIM_SCGC5_PORTD_MASK;

	// Make sure transmitter and receiver are disabled before init
  UART_PDD_EnableTransmitter(UART2_BASE_PTR, PDD_DISABLE); /* Disable transmitter. */
  UART_PDD_EnableReceiver(UART2_BASE_PTR, PDD_DISABLE); /* Disable receiver. */

  /* NVIC_ISER: SETENA|=0x4000 */
  NVIC_ISER |= NVIC_ISER_SETENA(0x4000);
  /* NVIC_IPR3: PRI_14=0 */
  NVIC_IPR3 &= (uint32_t)~(uint32_t)(NVIC_IP_PRI_14(0xFF));

	// Set pins to UART2 Rx and Tx
  /* PORTD_PCR3: ISF=0,MUX=3 */
	PORTD_PCR2 = (uint32_t)((PORTD_PCR2 & (uint32_t)~(uint32_t)(
								PORT_PCR_ISF_MASK |
								PORT_PCR_MUX(0x04)
							 )) | (uint32_t)(
								PORT_PCR_MUX(0x03)
							 ));
  /* PORTD_PCR3: ISF=0,MUX=3 */
  PORTD_PCR3 = (uint32_t)((PORTD_PCR3 & (uint32_t)~(uint32_t)(
                PORT_PCR_ISF_MASK |
                PORT_PCR_MUX(0x04)
               )) | (uint32_t)(
                PORT_PCR_MUX(0x03)
               ));

  // Bus clock (24 MHz) / (16 * SBR). SBR is value below
	// Use 13U to set baud rate to 115200
	// Use 1U to set baud rate to 1.5M or 1500000
	UART_PDD_SetBaudRate(UART2_BASE_PTR, 1U); /* Set the baud rate register. */

  /* UART0_C1: LOOPS=0,DOZEEN=0,RSRC=0,M=0,WAKE=0,ILT=0,PE=0,PT=0 */
  UART2_C1 = 0x00U;                    /*  Set the C1 register */
  /* UART0_C3: R8T9=0,R9T8=0,TXDIR=0,TXINV=0,ORIE=0,NEIE=0,FEIE=0,PEIE=0 */
  UART2_C3 = 0x00U;                    /*  Set the C3 register */
  /* UART2_C4: TDMAS=0,??=0,RDMAS=0,??=0,??=0,??=0,??=0,??=0 */
  UART2_C4 = 0x00U;

	// Enable transmit interrupts
	UART2_C2 |= UART_C2_TIE_MASK;

	// Enable UART receiver and transmitter
	UART_PDD_EnableTransmitter(UART2_BASE_PTR, PDD_ENABLE); /* Enable transmitter. */

}

/* UART transmit interrupt service routine */
PE_ISR(Cpu_ivINT_UART2) {
	// transmitter interrupt enabled && tx buffer empty
	if ((UART2_C2 & UART_C2_TIE_MASK) && (UART2_S1 & UART_S1_TDRE_MASK) ) {
		// can send another character
		if (TxQ_Size() != 0) {
			UART2_D = TxQ_Dequeue();
		}
		else {
			// queue is empty so disable transmitter interrupt
			UART2_C2 &= ~UART_C2_TIE_MASK;
		}
	}
}
