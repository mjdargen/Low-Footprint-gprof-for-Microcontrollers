/*-
 * Copyright (c) 1983, 1992, 1993, 2011
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * This file is taken from glibc 2.30.
 * Modified by Michael D'Argenio for host PC gmon.out file creation
 * for lightweight gprof profiling of microcontrollers.
*/

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include "gmon.h"
#include "gmon_out.h"
#include "profil.h"

#define	SCALE_1_TO_1	0x10000L
#define ERR(s)        write (STDERR_FILENO, s, sizeof (s) - 1)

struct gmonparam _gmonparam = { GMON_PROF_OFF, NULL, 0, NULL, 0, NULL, 0, 0L, 0, 0, 0};
static int s_scale;

static void moncontrol (int mode);
static void write_hist (int fd);
static void write_call_graph (int fd);


/********************* Utility Functions *********************/
/* defined in ffs.c included in <strings.h> for glibc-2.30 */
/* not included in my environment so redefined here */
int ffs (int i) {
  static const unsigned char table[] =
    {
      0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
      6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
      7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
      7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
      8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
      8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
      8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
      8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8
    };
  unsigned int a;
  unsigned int x = i & -i;

  a = x <= 0xffff ? (x <= 0xff ? 0 : 8) : (x <= 0xffffff ?  16 : 24);

  return table[x >> a] + a;
}

/*
 * Control profiling
 *	profiling is what mcount checks to see if
 *	all the data structures are ready.
 */
static void moncontrol (int mode) {
  struct gmonparam *p = &_gmonparam;

  /* Don't change the state if we ran into an error.  */
  if (p->state == GMON_PROF_ERROR)
    return;

  if (mode)
    {
      /* start */
      profil((void *) p->kcount, p->kcountsize, p->lowpc, s_scale);
      p->state = GMON_PROF_ON;
    }
  else
    {
      /* stop */
      profil(NULL, 0, 0, 0);
      p->state = GMON_PROF_OFF;
    }
}

void monstartup (uint32_t lowpc, uint32_t highpc) {
  int o;
  char *cp;
  struct gmonparam *p = &_gmonparam;

  /*
   * round lowpc and highpc to multiples of the density we're using
   * so the rest of the scaling (here and in gprof) stays in ints.
   */
  p->lowpc = ROUNDDOWN(lowpc, HISTFRACTION * sizeof(HISTCOUNTER));
  p->highpc = ROUNDUP(highpc, HISTFRACTION * sizeof(HISTCOUNTER));
  p->textsize = p->highpc - p->lowpc;
  p->kcountsize = ROUNDUP(p->textsize / HISTFRACTION, sizeof(*p->froms));
  p->hashfraction = HASHFRACTION;
  p->log_hashfraction = -1;
  /* The following test must be kept in sync with the corresponding
     test in mcount.c.  */
  if ((HASHFRACTION & (HASHFRACTION - 1)) == 0) {
      /* if HASHFRACTION is a power of two, mcount can use shifting
	 instead of integer division.  Precompute shift amount. */
      p->log_hashfraction = ffs(p->hashfraction * sizeof(*p->froms)) - 1;
  }
  p->fromssize = p->textsize / HASHFRACTION;
  p->tolimit = p->textsize * ARCDENSITY / 100;
  if (p->tolimit < MINARCS)
    p->tolimit = MINARCS;
  else if (p->tolimit > MAXARCS)
    p->tolimit = MAXARCS;
  p->tossize = p->tolimit * sizeof(struct tostruct);

  cp = calloc (p->kcountsize + p->fromssize + p->tossize, 1);
  if (! cp)
    {
      ERR("monstartup: out of memory\n");
      p->tos = NULL;
      p->state = GMON_PROF_ERROR;
      return;
    }
  p->tos = (struct tostruct *)cp;
  cp += p->tossize;
  p->kcount = (HISTCOUNTER *)cp;
  cp += p->kcountsize;
  p->froms = (ARCINDEX *)cp;

  p->tos[0].link = 0;

  o = p->highpc - p->lowpc;
  if (p->kcountsize < (uint32_t) o)
    s_scale = ((float)p->kcountsize / o ) * SCALE_1_TO_1;
  else
    s_scale = SCALE_1_TO_1;

  moncontrol(1);
}

static void write_hist (int fd) {
  write(fd, _gmonparam.kcount, _gmonparam.kcountsize);
}

static void write_call_graph (int fd) {
  ARCINDEX from_index, to_index;
  uint32_t from_len;
  uint32_t frompc;

  from_len = _gmonparam.fromssize / sizeof (*_gmonparam.froms);
  for (from_index = 0; from_index < from_len; ++from_index) {
    if (_gmonparam.froms[from_index] == 0)
      continue;

    frompc = _gmonparam.lowpc;
    frompc += (from_index * _gmonparam.hashfraction * sizeof (*_gmonparam.froms));
    for (to_index = _gmonparam.froms[from_index]; to_index != 0; to_index = _gmonparam.tos[to_index].link) {
      struct rawarc arc;
      arc.raw_frompc = frompc;
      arc.raw_selfpc = _gmonparam.tos[to_index].selfpc;
      arc.raw_count = _gmonparam.tos[to_index].count;
      write(fd, &arc, sizeof(arc));

    }
  }
}

static void write_gmon (void) {

  /* create gmon.out file */
  int fd = -1;
  fd = open ("gmon.out", O_CREAT|O_TRUNC|O_WRONLY|O_BINARY, 0666);
  if (fd < 0) {
    ERR("_mcleanup: gmon.out\n");
    return;
  }

  /* write gmon header */
  struct gmon_hist_hdr thdr, *hdr;
  hdr = &thdr;
  thdr.low_pc = _gmonparam.lowpc;
  thdr.high_pc = _gmonparam.highpc;
  thdr.hist_size = _gmonparam.kcountsize + sizeof (thdr);
  thdr.prof_rate = PROF_HZ;
  thdr.version = GMON_VERSION;
  write(fd, (char *)hdr, sizeof *hdr);

  /* write PC histogram */
  write_hist (fd);

  /* write call-graph */
  write_call_graph (fd);

  close (fd);
}

void _mcleanup (void) {
  /* stop */
  moncontrol (0);

  /* create gmon.out */
  if (_gmonparam.state != GMON_PROF_ERROR)
    write_gmon ();

  /* free the memory. */
  free (_gmonparam.tos);
}
