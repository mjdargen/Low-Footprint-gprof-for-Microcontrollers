/* Low-level statistical profiling support function.  Mostly POSIX.1 version.
   Copyright (C) 1996-2019 Free Software Foundation, Inc.
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

#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include "profil.h"

static uint32_t nsamples;
static uint32_t pc_offset;
static uint32_t pc_scale;

/* global profinfo for profil() call */
static struct profinfo prof = {
  PROFILE_NOT_INIT, /* profiling state */
  0,  /* profiling counters */
  0,  /* range to be profiled - lowpc */
  0,  /* range to be profiled - highpc */
  0   /* scale value of bins */
};

/* increment profile counter */
void profil_count (uint32_t pc) {
  uint32_t i = ((pc - pc_offset) / 2) * pc_scale / 65536;
  // make sure its in bounds
  if (i < nsamples)
    prof.counter[i]++;
}

/*  Enable statistical profiling, writing samples of the PC into at most
    SIZE bytes of SAMPLE_BUFFER; every processor clock tick while profiling
    is enabled, the system examines the user PC and increments
    SAMPLE_BUFFER[((PC - OFFSET) / 2) * SCALE / 65536].  If SCALE is zero,
    disable profiling.  Returns zero on success, -1 on error.  */
int profil (char *samples, uint32_t size, uint32_t offset, uint32_t scale) {
  nsamples = size;
  pc_scale = scale;
  pc_offset = offset;

  if (scale > 65536) {
    errno = EINVAL;
    return -1;
  }

  /* profiling on */
  if (scale) {
    memset(samples, 0, size);
    memset(&prof, 0, sizeof prof);
    prof.counter = (uint16_t*)samples;
    prof.lowpc = offset;
    prof.highpc = offset + ((((size >> 1) << 16) / scale) << 1);
    prof.scale = scale;
    prof.state = PROFILE_ON;
  }
  /* profiling off */
  else
    prof.state = PROFILE_OFF;

  return 0;
}
