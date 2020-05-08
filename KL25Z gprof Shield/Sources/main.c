/* ###################################################################
**     Filename    : main.c
**     Project     : MCUX_PE_KL25Z_SerialEcho
**     Processor   : MKL25Z128VLK4
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2019-10-16, 14:37, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "MCUC1.h"
#include "CS1.h"
#include "FRTOS1.h"
#include "UTIL1.h"
#include "AudioAmp.h"
#include "LEDR.h"
#include "LEDpin1.h"
#include "BitIoLdd1.h"
#include "LEDG.h"
#include "LEDpin2.h"
#include "BitIoLdd2.h"
#include "LEDB.h"
#include "LEDpin3.h"
#include "BitIoLdd3.h"
#include "FAT1.h"
#include "SD1.h"
#include "WAIT1.h"
#include "TMOUT1.h"
#include "TmDt1.h"
#include "XF1.h"
#include "DA1.h"
#include "DacLdd1.h"
#include "DacLdd1_DMA0.h"
#include "CLS1.h"
#include "AS1.h"
#include "ASerialLdd1.h"
#include "SM2.h"
#include "LCD_Data_Bits.h"
#include "LCD_BL_PWM.h"
#include "TPM1_0.h"
#include "LCD_D_NC.h"
#include "LCD_NWR.h"
#include "LCD_NRD.h"
#include "LCD_NRST.h"
#include "AD1.h"
#include "AdcLdd1.h"
#include "DBG_1.h"
#include "DBG_2.h"
#include "DBG_3.h"
#include "DBG_4.h"
#include "DBG_5.h"
#include "DBG_6.h"
#include "DBG_7.h"
#include "MMA1.h"
#include "GI2C1.h"
#include "CI2C1.h"
#include "HBLED.h"
#include "DMA1.h"
#include "TPM0.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* User includes (#include below this line is not maintained by Processor Expert) */
#include "mytasks.h"
#include "LCD.h"
#include "touchscreen.h"
#include "LCD_driver.h"
#include "font.h"
#include "gpio_defs.h"
#include "ST7789.h"
#include "sound.h"

#include "gmon_profil.h"

void HBLED_Test(void) {

	// Send a short pulse (active low)
	HBLED_ClrVal(NULL);
	WAIT1_Waitms(10);
	HBLED_SetVal(NULL);
}

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  /* For example: for(;;) { } */

  // initialize gprof
  init_gprof();

  CLS1_SendStr((unsigned char*)"Hello World\r\n", CLS1_GetStdio()->stdOut);

  LCD_Init();
	LCD_Text_Init(1);
	LCD_Erase();
	LCD_Text_PrintStr_RC(0,0, "Hello World");

	Sound_Init();
	Sound_Enable_Amp();
	Play_Tone();
	Sound_Disable_Amp();

	HBLED_Test();

	CreateTasks();
  FRTOS1_vTaskStartScheduler();

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
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
