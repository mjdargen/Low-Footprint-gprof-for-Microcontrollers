/*
 * profiler.S
 *  Implements the gprof profiler arc counting function
 *  For ARMv6-M Architecture
 *  Created on: 01/24/2020
 *  Author: Michael D'Argenio
*/

/*
 * GCC 4.4+ makes call like this:
 *		push   {lr}
 *		bl      __gnu_mcount_nc
 * There is an additional lr push that must be accounted for.
 * Breaks standard (AAPCS Procedure Call Standard for the ARM Architecture)

 * Stack information
	* r0  <-- top of stack (TOS)
	* r1
	* r2
	* r3
	* lr (callee addr.)
	* lr (caller addr.) <-- first lr push, initial (TOS)
*/

    .syntax unified
    .arch armv6-m

.globl __gnu_mcount_nc
.type __gnu_mcount_nc, %function

__gnu_mcount_nc:
/* dummy version for testing, does nothing */
#if 0
	add sp, sp, #4
	bx     lr

/* actual mcount implementation */
#else
	/* save registers */
	push {r0, r1, r2, r3, lr}

	/* r1 contains callee address */
	mov r1, lr
	/* move callee address to ip */
	mov ip, r1

	/* r0 contains caller address */
	ldr r0, [sp, #20]
	/* moves caller address to lr */
	mov lr, r0

	/* jump to _mcount_internal() implementation */
	bl _mcount_internal

	/* restore saved registers */
	pop {r0, r1, r2, r3}

	/* pops pc/lr and ip */
	add sp, sp, #8

	/* branch to callee */
	bx ip
#endif
