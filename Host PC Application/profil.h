/* Copyright (C) 2001-2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

/*
 * This file is taken from glibc 2.30.
 * Modified by Michael D'Argenio for host PC gmon.out file creation
 * for lightweight gprof profiling of microcontrollers.
 * Takes inspiration from Erich Styger's solution discussed here:
 * https://mcuoneclipse.com/2015/08/23/tutorial-using-gnu-profiling-gprof-with-arm-cortex-m/
*/

#ifndef __PROFIL_H__
#define __PROFIL_H__

#include <stdint.h>

/* user defines to update */
#define HEX_CONVERT                 /* ifdef data captured as hex from serial monitor */
#define FILENAME    "blink1hex.txt" /* profiling data text file */
#define MEM_START   0x410           /* memory start address */
#define MEM_END     0x1FFFF         /* memory end address */
#define MEM_SIZE    3               /* number of bytes in memory address */
#define MEM_SIZEX   MEM_SIZE*2      /* converted mem size */
#define PROF_HZ     1000            /* profiling frequency */

typedef enum {
  PROFILE_NOT_INIT = 0,
  PROFILE_ON,
  PROFILE_OFF
} PROFILE_State;

struct profinfo {
  PROFILE_State state;      /* profiling state */
  uint16_t      *counter;		/* profiling counters kcount[] */
  uint32_t      lowpc;      /* lower memory address bound */
  uint32_t      highpc;     /* upper memory address bound */
  uint32_t      scale;			/* scale value to retrieve index */
};

int profile_ctl(struct profinfo *, char *, uint32_t, uint32_t, uint32_t);
int profil(char *, uint32_t, uint32_t, uint32_t);
void profil_count(uint32_t pc);
uint8_t hexconverter(uint8_t num);

#endif /* __PROFIL_H__ */
