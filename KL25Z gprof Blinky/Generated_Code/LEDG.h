/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : LEDG.h
**     Project     : MCUX_PE_KL25Z_Blinky_gprof
**     Processor   : MKL25Z128VLK4
**     Component   : LED
**     Version     : Component 01.077, Driver 01.00, CPU db: 3.00.000
**     Compiler    : GNU C Compiler
**     Date/Time   : 2020-01-21, 08:24, # CodeGen: 0
**     Abstract    :
**          This component implements a universal driver for a single LED.
**     Settings    :
**          Component name                                 : LEDG
**          Turned On with initialization                  : no
**          HW Interface                                   : 
**            On/Off                                       : Enabled
**              Pin                                        : LEDpin
**            PWM                                          : Disabled
**            High Value means ON                          : no
**          Shell                                          : Disabled
**     Contents    :
**         On         - void LEDG_On(void);
**         Off        - void LEDG_Off(void);
**         Neg        - void LEDG_Neg(void);
**         Get        - uint8_t LEDG_Get(void);
**         Put        - void LEDG_Put(uint8_t val);
**         SetRatio16 - void LEDG_SetRatio16(uint16_t ratio);
**         Deinit     - void LEDG_Deinit(void);
**         Init       - void LEDG_Init(void);
**
**Copyright : 1997 - 2015 Freescale Semiconductor, Inc. 
**All Rights Reserved.
**
**Redistribution and use in source and binary forms, with or without modification,
**are permitted provided that the following conditions are met:
**
**o Redistributions of source code must retain the above copyright notice, this list
**  of conditions and the following disclaimer.
**
**o Redistributions in binary form must reproduce the above copyright notice, this
**  list of conditions and the following disclaimer in the documentation and/or
**  other materials provided with the distribution.
**
**o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**  contributors may be used to endorse or promote products derived from this
**  software without specific prior written permission.
**
**THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**http: www.freescale.com
**mail: support@freescale.com
** ###################################################################*/
/*!
** @file LEDG.h
** @version 01.00
** @brief
**          This component implements a universal driver for a single LED.
*/         
/*!
**  @addtogroup LEDG_module LEDG module documentation
**  @{
*/         

#ifndef __LEDG_H
#define __LEDG_H

/* MODULE LEDG. */
#include "MCUC1.h" /* SDK and API used */
#include "LEDGconfig.h" /* LED configuration */
#include "LEDpin2.h" /* interface to pin */

#define LEDG_ClrVal()    LEDpin2_ClrVal() /* put the pin on low level */
#define LEDG_SetVal()    LEDpin2_SetVal() /* put the pin on high level */
#define LEDG_SetInput()  LEDpin2_SetInput() /* use the pin as input pin */
#define LEDG_SetOutput() LEDpin2_SetOutput() /* use the pin as output pin */

#define LEDG_PARSE_COMMAND_ENABLED  0 /* set to 1 if method ParseCommand() is present, 0 otherwise */


#if LEDG_CONFIG_IS_LOW_ACTIVE
  #define LEDG_On() LEDpin2_ClrVal()
#else
  #define LEDG_On() LEDpin2_SetVal()
#endif
/*
** ===================================================================
**     Method      :  LEDG_On (component LED)
**     Description :
**         This turns the LED on.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

#if LEDG_CONFIG_IS_LOW_ACTIVE
  #define LEDG_Off() LEDpin2_SetVal()
#else
  #define LEDG_Off() LEDpin2_ClrVal()
#endif
/*
** ===================================================================
**     Method      :  LEDG_Off (component LED)
**     Description :
**         This turns the LED off.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

#define LEDG_Neg() LEDpin2_NegVal()
/*
** ===================================================================
**     Method      :  LEDG_Neg (component LED)
**     Description :
**         This negates/toggles the LED
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

#if LEDG_CONFIG_IS_LOW_ACTIVE
  #define LEDG_Get() (!(LEDpin2_GetVal()))
#else
  #define LEDG_Get() LEDpin2_GetVal()
#endif
/*
** ===================================================================
**     Method      :  LEDG_Get (component LED)
**     Description :
**         This returns logical 1 in case the LED is on, 0 otherwise.
**     Parameters  : None
**     Returns     :
**         ---             - Status of the LED (on or off)
** ===================================================================
*/

void LEDG_Init(void);
/*
** ===================================================================
**     Method      :  LEDG_Init (component LED)
**     Description :
**         Performs the LED driver initialization.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

#define LEDG_Put(val)  ((val) ? LEDG_On() : LEDG_Off())
/*
** ===================================================================
**     Method      :  LEDG_Put (component LED)
**     Description :
**         Turns the LED on or off.
**     Parameters  :
**         NAME            - DESCRIPTION
**         val             - value to define if the LED has to be on or
**                           off.
**     Returns     : Nothing
** ===================================================================
*/

void LEDG_Deinit(void);
/*
** ===================================================================
**     Method      :  LEDG_Deinit (component LED)
**     Description :
**         Deinitializes the driver
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void LEDG_SetRatio16(uint16_t ratio);
/*
** ===================================================================
**     Method      :  LEDG_SetRatio16 (component LED)
**     Description :
**         Method to specify the duty cycle. If using a PWM pin, this
**         means the duty cycle is set. For On/off pins, values smaller
**         0x7FFF means off, while values greater means on.
**     Parameters  :
**         NAME            - DESCRIPTION
**         ratio           - Ratio value, where 0 means 'off' and
**                           0xffff means 'on'
**     Returns     : Nothing
** ===================================================================
*/

/* END LEDG. */

#endif
/* ifndef __LEDG_H */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
