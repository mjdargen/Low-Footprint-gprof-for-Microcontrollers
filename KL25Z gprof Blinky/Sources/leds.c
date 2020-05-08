/*
 * leds.c
 *
 *  Created on: Mar 3, 2020
 *      Author: Michael
 */

#include "LEDR.h"
#include "LEDG.h"
#include "LEDB.h"

void control_LEDs (bool R, bool G, bool B) {
	if (R) LEDR_On();
	else LEDR_Off();
	if (G) LEDG_On();
	else LEDG_Off();
	if (B) LEDB_On();
	else LEDB_Off();
}
