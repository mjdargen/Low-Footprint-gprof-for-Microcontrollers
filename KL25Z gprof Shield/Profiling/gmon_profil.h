/*
 * gmon_profil.h
 *
 *  Created on: Feb 1, 2020
 *      Author: Michael D'Argenio
 */

#ifndef PROFILING_GMON_PROFIL_H__
#define PROFILING_GMON_PROFIL_H__

// Comment out USING_RTOS definition if not using RTOS
#define USING_RTOS

#define HW_RET_ADX_OFFSET (24)
#define IRQ_FRAME_SIZE (8)
#define PC_OFFSET (24)

#ifdef USING_RTOS // Don't need these since PC is on PSP, not MSP
	#define FRAME_SIZE 	(0)
	#define CUR_SP 			(__get_PSP())
#else // Using MSP, so stack frames are also on the MSP stack
	#define FRAME_SIZE 	(IRQ_FRAME_SIZE + PC_OFFSET)
	#define CUR_SP 			(__get_MSP())
#endif

/* profiling frequency. */
#define PROF_HZ					5
#define SystemCoreClock (48e6)
#define SAMPLE_FREQ_HZ_TO_TICKS ((SystemCoreClock/(2*PROF_HZ))-1)

void init_SysTick();
void init_PIT();
void init_gprof();
void deinit_gprof();

#endif /* PROFILING_GMON_PROFIL_H__ */
