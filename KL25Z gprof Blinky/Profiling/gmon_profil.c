/*
 * gmon_profil.c
 * Handles program counter sampling for gprof implementation.
 *  Created on: Feb 1, 2020
 *      Author: Michael D'Argenio
 */

#include <gmon_profil.h>
#include <gmon_queue.h>
#include <gmon_serial.h>
#include "MKL25Z4.h"
#include "Cpu.h"

/**
  \brief   Get Process Stack Pointer
  \details Returns the current value of the Process Stack Pointer (PSP).
  \return               PSP Register value
 */
static inline uint32_t __get_PSP(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, psp"  : "=r" (result) );
  return(result);
}

/**
  \brief   Get Main Stack Pointer
  \details Returns the current value of the Main Stack Pointer (MSP).
  \return               MSP Register value
 */
static inline uint32_t __get_MSP(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, msp" : "=r" (result) );
  return(result);
}

/* initialize periodic interrupt timer */
void init_PIT(void) {
  /* SIM_SCGC6: PIT=1 */
  SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;

  /* NVIC_IPR5: PRI_22=0 */
  //NVIC_IPR5 &= (uint32_t)~(uint32_t)(NVIC_IP_PRI_22(0xFF));
  /* NVIC_IPR5: PRI_22=0xC0 */
  NVIC_IPR5 = (uint32_t)((NVIC_IPR5 & (uint32_t)~(uint32_t)(
               NVIC_IP_PRI_22(0x3F)
              )) | (uint32_t)(
               NVIC_IP_PRI_22(0xC0)
              ));
  /* NVIC_ISER: SETENA|=0x00400000 */
  NVIC_ISER |= NVIC_ISER_SETENA(0x00400000);

  /* PIT_MCR: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,MDIS=0,FRZ=0 */
  PIT_MCR = 0x00U;
  /* PIT_TFLG0: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,TIF=1 */
  PIT_TFLG0 = PIT_TFLG_TIF_MASK;
  /* PIT_TFLG1: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,TIF=1 */
  PIT_TFLG1 = PIT_TFLG_TIF_MASK;
  /* PIT_LDVAL0: TSV=0 */
  PIT_LDVAL0 = PIT_LDVAL_TSV(0x00);
  /* PIT_LDVAL1: TSV=0x5DB8 */
  PIT_LDVAL1 = PIT_LDVAL_TSV(SAMPLE_FREQ_HZ_TO_TICKS);
  /* PIT_TCTRL0: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,TIE=0,TEN=0 */
  PIT_TCTRL0 &= (uint32_t)~(uint32_t)(
                 PIT_TCTRL_TIE_MASK |
                 PIT_TCTRL_TEN_MASK |
                 0xFFFFFFF8U
                );
  /* PIT_TCTRL1: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,CHN=0,TIE=1,TEN=0 */
  PIT_TCTRL1 = PIT_TCTRL_TIE_MASK;
  /* PIT_TCTRL1: TEN=1 */
  PIT_TCTRL1 |= PIT_TCTRL_TEN_MASK;
}

/* PIT interrupt service routine to sample the program counter */
PE_ISR(Cpu_ivINT_PIT) {
	// check to see which channel triggered interrupt
	if (PIT_TFLG0 & PIT_TFLG_TIF_MASK) {
		// clear status flag for timer channel 1
		PIT_TFLG0 &= PIT_TFLG_TIF_MASK;
	}
	else if (PIT_TFLG1 & PIT_TFLG_TIF_MASK) {
		// clear status flag for timer channel 1
		PIT_TFLG1 &= PIT_TFLG_TIF_MASK;

		static uint32_t pc, sp;
		// retrieve stack pointer
		// add additional frames on stack to retrieve pc
		sp = (CUR_SP + FRAME_SIZE + HW_RET_ADX_OFFSET);

		// works for ARM processors
		// in RTOS cases where psp will be used but has not been used yet.
		// swap and use msp for the time being
#ifdef USING_RTOS
		if (sp > 0xf0000000) {
			sp = ((__get_MSP()) + HW_RET_ADX_OFFSET + IRQ_FRAME_SIZE + PC_OFFSET);
		}
#endif

		// retrieve program counter
		pc = *(uint32_t*)(sp);

		// add 'P' or 0x50 for pc sample
		// add 3 byte pc address
		uint8_t bp[4];
		uint32_t* p;
		p = (uint32_t*) bp;
		*p = (0x50000000 | pc);

		// enqueue data packet one byte at a time
		for (int i=0; i<4; i++) {
			// enqueue next byte
			// must reverse order
			while (!TxQ_Enqueue(bp[3-i]));
			// make sure transmit interrupt is enabled
			// could have been disabled if queue was emptied
			if (!(UART2_C2 & UART_C2_TIE_MASK)) {
				UART2_C2 |= UART_C2_TIE_MASK;
			}
		}
	}
}
