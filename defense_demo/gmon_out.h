/* Copyright (C) 1996-2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by David Mosberger <davidm@cs.arizona.edu>.

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

/* This file specifies the format of gmon.out files.  It should have
   as few external dependencies as possible as it is going to be included
   in many different programs.  That is, minimize the number of #include's.

   A gmon.out file consists of a header (defined by gmon_hdr) followed by
   a sequence of records.  Each record starts with a one-byte tag
   identifying the type of records, followed by records specific data. */

/*
 * This file is taken from glibc 2.30.
 * Modified by Michael D'Argenio for host PC gmon.out file creation
 * for lightweight gprof profiling of microcontrollers.
*/

#ifndef _SYS_GMON_OUT_H
#define _SYS_GMON_OUT_H	1

#define GMON_VERSION	0x00051879 /* version number */

struct gmon_hist_hdr {
	uint32_t low_pc;       /* base pc address of sample buffer */
	uint32_t high_pc;      /* max pc address of sampled buffer */
	uint32_t hist_size;    /* size of sample buffer (plus this header) */
	int version;           /* version number */
	int prof_rate;         /* profiling clock rate */
	int spare[3];          /* reserved */
};

#endif /* sys/gmon_out.h */
