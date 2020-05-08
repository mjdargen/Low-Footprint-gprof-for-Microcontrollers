/*
 * Lightweight gprof for Microcontrollers
 * main.c
 *
 *  Created on: February 20, 2020
 *  Author: Michael D'Argenio
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "profil.h"
#include "gmon.h"

// to create the GMON validation log
//#define VALIDATION

int main (int argc, char ** argv) {

  uint32_t failures = 0; // tracking for debug

  // start up gmon.out processing
  monstartup (MEM_START, MEM_END);

	// open the file
  char proFile[64] = FILENAME;
  FILE * fp = fopen(proFile, "r");
  if (!fp) {
    fprintf(stderr,"Unable to read file: %s\n", proFile);
    return 1;
  }

#ifdef VALIDATION
  char debugFile[64] = "validation.csv";
  FILE * dfp = fopen(debugFile, "w");
  if (!dfp) {
    fprintf(stderr,"Unable to read file: %s\n", proFile);
    return 1;
  }
  fprintf(dfp, "Type,Addr1,Addr2\n");
#endif

// method if the hex conversion is necessary
#ifdef HEX_CONVERT
  // loop through until end of file
  while (!feof(fp)) {
    uint8_t type[2];
    fscanf(fp, "%c%c", &type[0], &type[1]);

    // P = 0x50
    // if the data is a PC sample
    if (type[0] == '5' && type[1] == '0') {
      uint8_t pcArray[MEM_SIZEX];
      uint32_t pc = 0;

      // capture pc address
      for (int j=0; j<MEM_SIZEX; j++) {
        fscanf(fp, "%c", &pcArray[j]);
      }

      // convert numbers to a single number
      for (int j=0; j<MEM_SIZEX; j++) {
        pcArray[j] = hexconverter(pcArray[j]);
        pc |= (pcArray[j] << (MEM_SIZEX-1-j)*4);
      }
#ifdef VALIDATION
      fprintf(dfp, "P,%x,0\n", pc);
#endif
      // program counter sampling processing
      profil_count(pc);
    }

    // A = 0x41
    // if the data is a call arc
    // first MEM_SIZE bytes are from address, last MEM_SIZE bytes are to address
    else if (type[0] == '4' && type[1] == '1') {
      uint8_t fromArray[MEM_SIZEX];
      uint32_t fromAddr = 0;
      uint8_t toArray[MEM_SIZEX];
      uint32_t toAddr = 0;

      // capture from address
      for (int j=0; j<MEM_SIZEX; j++) {
        fscanf(fp, "%c", &fromArray[j]);
      }
      // capture to address
      for (int j=0; j<MEM_SIZEX; j++) {
        fscanf(fp, "%c", &toArray[j]);
      }

      // convert numbers to a single number
      for (int j=0; j<MEM_SIZEX; j++) {
        fromArray[j] = hexconverter(fromArray[j]);
        fromAddr |= (fromArray[j] << (MEM_SIZEX-1-j)*4);
        toArray[j] = hexconverter(toArray[j]);
        toAddr |= (toArray[j] << (MEM_SIZEX-1-j)*4);
      }
#ifdef VALIDATION
      fprintf(dfp, "A,%x,%x\n", fromAddr, toAddr);
#endif
      // call arc processing
      _mcount_internal(fromAddr, toAddr);
    }
    // if errors for debugging
    else {
      uint8_t garbage;
      fscanf(fp, "%c", &garbage);
      printf("\nError: invalid command type %c\n", garbage);
      failures++;
    }
  }

// method without hex conversion
#else
  // loop through until end of file
  while (!feof(fp)) {
    uint8_t type = 0;
    fscanf(fp, "%c", &type);

    // if P, program counter sample
    if (type == 'P') {
      uint8_t pcArray[MEM_SIZE];
      uint32_t pc = 0;

      // capture pc memory address
      for (int j=0; j<MEM_SIZE; j++) {
        fscanf(fp, "%c", &pcArray[j]);
      }

      // convert numbers to a single number
      for (int j=0; j<MEM_SIZE; j++) {
        pc |= (pcArray[j] << (MEM_SIZE-1-j)*8);
      }
      // program counter sampling processing
      profil_count(pc);
    }
    // if call arc
    else if (type == 'A') {
      uint8_t fromArray[MEM_SIZE];
      uint32_t fromAddr = 0;
      uint8_t toArray[MEM_SIZE];
      uint32_t toAddr = 0;

      // capture from memory address
      for (int j=0; j<MEM_SIZE; j++) {
        fscanf(fp, "%c", &fromArray[j]);
      }
      // capture to memory address
      for (int j=0; j<MEM_SIZE; j++) {
        fscanf(fp, "%c", &toArray[j]);
      }

      // convert numbers to a single number
      for (int j=0; j<MEM_SIZE; j++) {
        fromAddr |= (fromArray[j] << (MEM_SIZE-1-j)*8);
        toAddr |= (toArray[j] << (MEM_SIZE-1-j)*8);
      }
      // call arc processing
      _mcount_internal(fromAddr, toAddr);
    }
    // if errors for debugging
    else {
      uint8_t garbage;
      fscanf(fp, "%c", &garbage);
      printf("\nError: invalid command type %c\n", garbage);
      failures++;
    }
	}
#endif

  fclose(fp);
#ifdef VALIDATION
  fclose(dfp);
#endif

	// input processing complete
	// call cleanup function to generate file
  _mcleanup();
  printf("\n\n# of failures: %d\n\n",failures);

	return 0;
}

// converts ascii characters stored as hex to the original hex numbers
// individual hex numbers are stored as ascii
// i.e. the hex number 0xA is written into the file as '41'
// Must read in '4' and then '1'.
// These numbers are interpreted as 0x34 and 0x31 respectively.
// use this function to convert these ascii interpretations to hex
uint8_t hexconverter(uint8_t num) {
  uint8_t result = 0;
  if (num > 0x40) {
    switch (num-0x40) {
      case 1 :
        result = 0xA;
        break;
      case 2 :
        result = 0xB;
        break;
      case 3 :
        result = 0xC;
        break;
      case 4 :
        result = 0xD;
        break;
      case 5 :
        result = 0xE;
        break;
      case 6 :
        result = 0xF;
        break;
      default :
        printf("Error: unknown hex value %x\n", num);
    }
  }
  else if (num >= 0x30) {
    switch (num-0x30) {
      case 0 :
        result = 0x0;
        break;
      case 1 :
        result = 0x1;
        break;
      case 2 :
        result = 0x2;
        break;
      case 3 :
        result = 0x3;
        break;
      case 4 :
        result = 0x4;
        break;
      case 5 :
        result = 0x5;
        break;
      case 6 :
        result = 0x6;
        break;
      case 7 :
        result = 0x7;
        break;
      case 8 :
        result = 0x8;
        break;
      case 9 :
        result = 0x9;
        break;
      default :
        printf("Error: unknown hex value %x\n", num);
        break;
    }
  }
  // error handling
  else {
    printf("Error: unknown hex value %x\n", num);
  }

  return result;
}
