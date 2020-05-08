/*
 * mytasks.c
 *
 *  Created on: Oct 24, 2019
 *      Author: Michael
 */

#include "FRTOS1.h"
#include "FreeRTOSConfig.h"
#include "Cpu.h"
#include "Events.h"
#include "MCUC1.h"
#include "CS1.h"
#include "UTIL1.h"
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
#include "CLS1.h"
#include "ASerialLdd1.h"

#include "LCD.h"
#include "touchscreen.h"
#include "LCD_driver.h"
#include "font.h"
#include "gpio_defs.h"
#include "ST7789.h"
#include "mytasks.h"
#include "MMA8451.h"
#include "myfatfs.h"
#include "sound.h"

#include <stdio.h>

SemaphoreHandle_t LCD_mutex;
EventGroupHandle_t xCreatedEventGroup;

int16_t SineTable[NUM_STEPS];
uint16_t Waveform[2][NUM_WAVEFORM_SAMPLES];
uint8_t write_buffer_num = 0; // Number of waveform buffer currently being written
VOICE_T Voice[NUM_VOICES];

static const CLS1_ParseCommandCallback CmdParserTable[] =
{
  CLS1_ParseCommand,
#if LEDR_PARSE_COMMAND_ENABLED
  LEDR_ParseCommand,
#endif
#if LEDG_PARSE_COMMAND_ENABLED
  LEDG_ParseCommand,
#endif
#if LEDB_PARSE_COMMAND_ENABLED
  LEDB_ParseCommand,
#endif
#if FRTOS1_PARSE_COMMAND_ENABLED
  FRTOS1_ParseCommand,
#endif
#if TmDt1_PARSE_COMMAND_ENABLED
	TmDt1_ParseCommand,
#endif
#if FAT1_PARSE_COMMAND_ENABLED
  FAT1_ParseCommand,
#endif
  NULL /* sentinel */
};

/* control onboard RGB LED */
void control_LEDs (bool R, bool G, bool B) {
	if (R) LEDR_On();
	else LEDR_Off();
	if (G) LEDG_On();
	else LEDG_Off();
	if (B) LEDB_On();
	else LEDB_Off();
}

/* FRTOS Task for command line interface*/
static portTASK_FUNCTION(Cmd_Line_Task, pvParameters) {

	// RTOS task initialization
	// send command list
	unsigned char buf[48];
	buf[0] = '\0';
	(void)CLS1_ParseWithCommandTable((unsigned char*)CLS1_CMD_HELP, CLS1_GetStdio(), CmdParserTable);

	// RTOS loop
  while(1) {
  	(void)CLS1_ReadAndParseWithCommandTable(buf, sizeof(buf), CLS1_GetStdio(), CmdParserTable);
		vTaskDelay(pdMS_TO_TICKS(THREAD_CMD_LINE_INT_PERIOD_MS));
  }
}

/* FRTOS task for toggling RGB LED */
static portTASK_FUNCTION(RGB_Task, pvParameters) {

	// RTOS task initialization

	// RTOS loop
	while(1) {
  	for (char i=0; i<8; i++) {
  		control_LEDs(i&4,i&2,i&1);
  		vTaskDelay(pdMS_TO_TICKS(THREAD_RGB_LED_PERIOD_MS));
  	}
  }
}

/* FRTOS task for FatFS SD card */
static portTASK_FUNCTION(FatFS_SD_Task, pvParameters) {

	// RTOS task initialization
	bool cardMounted = FALSE;
	static FAT1_FATFS fileSystemObject;

	FAT1_Init(); // FatFS init

	// delay for 1 second
	// allows command table to be printed and gives SD card chance to power up
	vTaskDelay(pdMS_TO_TICKS(INITIAL_DELAY_THREAD_FATFS));

	// RTOS loop
	while(1) {
		(void)FAT1_CheckCardPresence(&cardMounted, "0" /* drive */, &fileSystemObject, CLS1_GetStdio());
		vTaskDelay(pdMS_TO_TICKS(THREAD_FATFS_PERIOD_MS));
	}

}

/* FRTOS task for reading the touch screen */
static portTASK_FUNCTION(Read_TS_Task, pvParameters) {

	// RTOS task initialization
	PT_T p, pp;
	COLOR_T c;

	pp.X = 0;
	pp.Y = 0;

	c.R = 255;
	c.G = 200;
	c.B = 200;

	//LCD_Text_PrintStr_RC(LCD_MAX_ROWS-2, 0, "Dim <--------> Bright");
	LCD_Text_PrintStr_RC(LCD_MAX_ROWS-2, 0, "Mute");
	LCD_Text_PrintStr_RC(LCD_MAX_ROWS-2, 12, "Unmute");
	// originally was -4, adjust touch area

	// RTOS loop
	while(1) {
		//DEBUG_START(DBG_TREADTS_POS);
		if (LCD_TS_Read(&p)) {
			if (p.Y > ROW_TO_Y(LCD_MAX_ROWS-3)) {  // ROW_TO_Y(LCD_MAX_ROWS-3) p.Y > 240
				if (p.X < LCD_WIDTH/2) {
					Sound_Disable_Amp();
				}
				else {
					Sound_Enable_Amp();
				}
			}
			else {
				// Now draw on screen
				if ((pp.X == 0) && (pp.Y == 0)) {
					pp = p;
				}
				xSemaphoreTake(LCD_mutex, portMAX_DELAY);
				LCD_Draw_Line(&p, &pp, &c);
				xSemaphoreGive(LCD_mutex);
				pp = p;
			}
		}
		else {
			pp.X = 0;
			pp.Y = 0;
		}
		//DEBUG_STOP(DBG_TREADTS_POS);
		vTaskDelay(pdMS_TO_TICKS(THREAD_READ_TS_PERIOD_MS));
	}

}

/* FRTOS task for updating the screen */
static portTASK_FUNCTION(Update_Screen_Task, pvParameters) {

	// RTOS task initialization
	int16_t paddle_pos=LCD_WIDTH/2;
	PT_T p1, p2;
	COLOR_T paddle_color;

	paddle_color.R = 100;
	paddle_color.G = 10;
	paddle_color.B = 100;

	// RTOS loop
	while(1) {
		//DEBUG_START(DBG_TUPDATESCR_POS);

		if ((roll < -2.0) || (roll > 2.0)) {
			p1.X = paddle_pos;
			p1.Y = PADDLE_Y_POS;
			p2.X = p1.X + PADDLE_WIDTH;
			p2.Y = p1.Y + PADDLE_HEIGHT;
			xSemaphoreTake(LCD_mutex, portMAX_DELAY);
			LCD_Fill_Rectangle(&p1, &p2, &black);
			xSemaphoreGive(LCD_mutex);

			paddle_pos += roll;
			paddle_pos = MAX(0, paddle_pos);
			paddle_pos = MIN(paddle_pos, LCD_WIDTH-1-PADDLE_WIDTH);

			p1.X = paddle_pos;
			p1.Y = PADDLE_Y_POS;
			p2.X = p1.X + PADDLE_WIDTH;
			p2.Y = p1.Y + PADDLE_HEIGHT;
			paddle_color.R = 150+5*roll;
			paddle_color.G = 150-5*roll;
			// ? vs.
			//paddle_color.R = 100+roll;
			//paddle_color.B = 100-roll;
			xSemaphoreTake(LCD_mutex, portMAX_DELAY);
			LCD_Fill_Rectangle(&p1, &p2, &white);
			p1.X++;
			p2.X--;
			p1.Y++;
			p2.Y--;
			LCD_Fill_Rectangle(&p1, &p2, &paddle_color);
			xSemaphoreGive(LCD_mutex);
		}

		//DEBUG_STOP(DBG_TUPDATESCR_POS);
		vTaskDelay(pdMS_TO_TICKS(THREAD_UPDATE_SCREEN_PERIOD_MS));
	}

}

/* FRTOS task for reading the accelerometer */
static portTASK_FUNCTION(Read_Accel_Task, pvParameters) {

	// RTOS task initialization
	init_mma();
	// delay after setup
	vTaskDelay(pdMS_TO_TICKS(THREAD_READ_ACCELEROMETER_PERIOD_MS));

	// RTOS loop
	while(1) {
		char buffer[16];
		//DEBUG_START(DBG_TREADACC_POS);

		read_full_xyz();
		convert_xyz_to_roll_pitch();

		// convert and print roll
		//sprintf(buffer, "Roll: %6.2f", roll);
		char *tmpSign = (roll < 0) ? "-" : "";
		float tmpVal = (roll < 0) ? -roll : roll;
		int tmpInt1 = tmpVal;                  // Get the integer (678).
		float tmpFrac = tmpVal - tmpInt1;      // Get fraction (0.0123).
		int tmpInt2 = (tmpFrac * 100);  // Turn into integer (123).
		sprintf (buffer, "Roll: %s%d.%02d  ", tmpSign, tmpInt1, tmpInt2);
		xSemaphoreTake(LCD_mutex, portMAX_DELAY);
		LCD_Text_PrintStr_RC(0, 0, buffer);
		xSemaphoreGive(LCD_mutex);

		// convert and print pitch
		//sprintf(buffer, "Pitch: %6.2f", pitch);
		tmpSign = (pitch < 0) ? "-" : "";
		tmpVal = (pitch < 0) ? -pitch : pitch;
		tmpInt1 = tmpVal;                  // Get the integer (678).
		tmpFrac = tmpVal - tmpInt1;      // Get fraction (0.0123).
		tmpInt2 = (tmpFrac * 100);  // Turn into integer (123).
		sprintf (buffer, "Pitch: %s%d.%02d  ", tmpSign, tmpInt1, tmpInt2);
		xSemaphoreTake(LCD_mutex, portMAX_DELAY);
		LCD_Text_PrintStr_RC(1, 0, buffer);
		xSemaphoreGive(LCD_mutex);

		//DEBUG_STOP(DBG_TREADACC_POS);
		vTaskDelay(pdMS_TO_TICKS(THREAD_READ_ACCELEROMETER_PERIOD_MS));
	}
}

/* FRTOS task for updating directory listing for SD card */
static portTASK_FUNCTION(PWD_Task, pvParameters) {

	// RTOS task initialization
	bool once = TRUE;
	vTaskDelay(pdMS_TO_TICKS(INITIAL_DELAY_THREAD_PWD));

	// RTOS loop
	while(1) {
		uint8_t fres = FR_OK;

		// just once, print the working directory, home directory
		// in the future, will update here
		// we will have a flag here. it will be set based on where the screen is touched.
		// checked by Read_TS
		if (once) {
			once = FALSE;
			char filename[16];

			fres = FAT1_getcwd((TCHAR*)filename, sizeof(filename));
			if(fres!=FR_OK) {
				// getcwd failed
			}
			pwd((TCHAR*)filename);

		}
		vTaskDelay(pdMS_TO_TICKS(THREAD_PWD_PERIOD_MS));
	}
}

/* FRTOS task for managing sound */
static portTASK_FUNCTION(Sound_Mngr_Task, pvParameters) {
	uint16_t lfsr=1234;
	uint16_t bit;

	while (1) {
#if 1
		// make a new random sound periodically
		vTaskDelay(pdMS_TO_TICKS(200));
		//DEBUG_START(DBG_TSNDMGR_POS);
		Voice[0].Volume = 0xAFFF;
		Voice[0].Duration = 6000;
		bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
		lfsr =  (lfsr >> 1) | (bit << 15);
#if 1
		Voice[0].Period = FREQ_TO_PERIOD((lfsr & 0x03FF) + 40);
		Voice[0].Decay = 30;
#else // fixed frequency
		Voice[0].Period = FREQ_TO_PERIOD(1000);
		Voice[0].Decay = 90;
#endif
		Voice[0].Counter = 0;
		Voice[0].CounterIncrement = (NUM_STEPS*256)/Voice[0].Period;
		Voice[0].Type = VW_SINE;
#else
		// TODO: convert to new API
		os_evt_wait_and(EV_PLAYSOUND, osWaitForever); // wait for trigger
		// Add code to initialize voices
#endif

		// Signal event requesting source buffer refill
		xEventGroupSetBits(	xCreatedEventGroup,   /* The event group being updated. */
												BIT_0 );							/* The bits being set. */
		Play_Waveform_with_DMA();
		//DEBUG_STOP(DBG_TSNDMGR_POS);
	}
}

/* FRTOS task for refilling the sound buffer */
static portTASK_FUNCTION(Refill_Sound_Task, pvParameters) {
	uint32_t i;
	uint16_t v;
	int32_t sum, sample;
#if USE_DOUBLE_BUFFER
	uint8_t initialized = 0;
#endif

	while (1) {
	  /* Wait forever for either bit 0 within the event group.
	  Clear the bits before exiting. */
	  xEventGroupWaitBits(
	            xCreatedEventGroup,   /* The event group being tested. */
	            BIT_0, 				 /* The bits within the event group to wait for. */
	            pdTRUE,        /* BIT_0 & BIT_4 should be cleared before returning. */
							pdTRUE,       /* Don't wait for both bits, either bit will do. */
							portMAX_DELAY );/* Wait a maximum of 100ms for either bit to be set. */

		//DEBUG_START(DBG_TREFILLSB_POS);
		for (i=0; i<NUM_WAVEFORM_SAMPLES; i++) {
			sum = 0;
			for (v=0; v<NUM_VOICES; v++) {
				if (Voice[v].Duration > 0) {
					sample = Sound_Generate_Next_Sample(&(Voice[v]));

					sample = (sample*Voice[v].Volume)>>16;
					sum += sample;
					// update volume with decayed version
					Voice[v].Volume = (Voice[v].Volume * (((uint32_t) 65536) - Voice[v].Decay)) >> 16;
					Voice[v].Duration--;
				}
			}
			sum = sum + (MAX_DAC_CODE/2);
			sum = MIN(sum, MAX_DAC_CODE-1);
			Waveform[write_buffer_num][i] = sum;
		}
#if USE_DOUBLE_BUFFER
		write_buffer_num = 1 - write_buffer_num;
		if (!initialized) { // Fill up both buffers the first time
			initialized = 1;
			for (i=0; i<NUM_WAVEFORM_SAMPLES; i++) {
				sum = 0;
				for (v=0; v<NUM_VOICES; v++) {
					if (Voice[v].Duration > 0) {
						sample = Sound_Generate_Next_Sample(&(Voice[v]));

						sample = (sample*Voice[v].Volume)>>16;
						sum += sample;
						// update volume with decayed version
						Voice[v].Volume = (Voice[v].Volume * (((uint32_t) 65536) - Voice[v].Decay)) >> 16;
						Voice[v].Duration--;
					}
				}
				sum = sum + (MAX_DAC_CODE/2);
				sum = MIN(sum, MAX_DAC_CODE-1);
				Waveform[write_buffer_num][i] = sum;
			}
			write_buffer_num = 1 - write_buffer_num;
		}
#endif
		//DEBUG_STOP(DBG_TREFILLSB_POS);
	}
}

/* Creates FRTOS Tasks and other RTOS objects */
void CreateTasks(void) {
	LCD_mutex = xSemaphoreCreateMutex();
  if(LCD_mutex == NULL) while(1); // mutex was not created successfully

  // Attempt to create the event group.
  xCreatedEventGroup = xEventGroupCreate();

  // Was the event group created successfully?
  if(xCreatedEventGroup == NULL)
  	while(1); // event group not created successfully because there was insufficient

  TaskHandle_t * Cmd_Line_taskHandle = NULL;
  TaskHandle_t * RGB_taskHandle = NULL;
  TaskHandle_t * FatFS_SD_taskHandle = NULL;
  TaskHandle_t * Read_TS_taskHandle = NULL;
  TaskHandle_t * Update_Screen_taskHandle = NULL;
  TaskHandle_t * Read_Accel_taskHandle = NULL;
  TaskHandle_t * PWD_taskHandle  = NULL;
  TaskHandle_t * Sound_Mngr_taskHandle = NULL;
  TaskHandle_t * Refill_Sound_taskHandle  = NULL;

  if (FRTOS1_xTaskCreate(
  		Cmd_Line_Task,  /* pointer to the task */
      "Cmd_Line", /* task name for kernel awareness debugging */
      configMINIMAL_STACK_SIZE + 300, /* task stack size */
      (void*)NULL, /* optional task startup argument */
      tskIDLE_PRIORITY + 0,  /* initial priority */
			Cmd_Line_taskHandle /* optional task handle to create */
    ) != pdPASS) {
  			while(1); /* error! probably out of memory */
  }
  if (FRTOS1_xTaskCreate(
     RGB_Task,  /* pointer to the task */
      "RGB", /* task name for kernel awareness debugging */
      configMINIMAL_STACK_SIZE + 4, /* task stack size */
      (void*)NULL, /* optional task startup argument */
      tskIDLE_PRIORITY + 0,  /* initial priority */
			RGB_taskHandle /* optional task handle to create */
    ) != pdPASS) {
  			while(1); /* error! probably out of memory */
  }
  if (FRTOS1_xTaskCreate(
  		FatFS_SD_Task,  /* pointer to the task */
        "FatFS_SD", /* task name for kernel awareness debugging */
        configMINIMAL_STACK_SIZE + 80, /* task stack size */
        (void*)NULL, /* optional task startup argument */
        tskIDLE_PRIORITY + 0,  /* initial priority */
				FatFS_SD_taskHandle /* optional task handle to create */
      ) != pdPASS) {
  			while(1); /* error! probably out of memory */
  }
  if (FRTOS1_xTaskCreate(
  		Read_TS_Task,  /* pointer to the task */
          "Read_TS", /* task name for kernel awareness debugging */
          configMINIMAL_STACK_SIZE + 64, /* task stack size */
          (void*)NULL, /* optional task startup argument */
          tskIDLE_PRIORITY + 0,  /* initial priority */
					Read_TS_taskHandle /* optional task handle to create */
        ) != pdPASS) {
  				while(1); /* error! probably out of memory */
   }
  if (FRTOS1_xTaskCreate(
			Update_Screen_Task,  /* pointer to the task */
				"Update_Screen", /* task name for kernel awareness debugging */
				configMINIMAL_STACK_SIZE + 32, /* task stack size */
				(void*)NULL, /* optional task startup argument */
				tskIDLE_PRIORITY + 0,  /* initial priority */
				Update_Screen_taskHandle /* optional task handle to create */
			) != pdPASS) {
  			while(1); /* error! probably out of memory */
  }
  if (FRTOS1_xTaskCreate(
  		Read_Accel_Task,  /* pointer to the task */
				"Read_Accel", /* task name for kernel awareness debugging */
				configMINIMAL_STACK_SIZE + 128, /* task stack size */
				(void*)NULL, /* optional task startup argument */
				tskIDLE_PRIORITY + 0,  /* initial priority */
				Read_Accel_taskHandle/* optional task handle to create */
			) != pdPASS) {
  			while(1); /* error! probably out of memory */
	}
  if (FRTOS1_xTaskCreate(
			PWD_Task,  /* pointer to the task */
				"PWD", /* task name for kernel awareness debugging */
				configMINIMAL_STACK_SIZE + 128, /* task stack size */
				(void*)NULL, /* optional task startup argument */
				tskIDLE_PRIORITY + 0,  /* initial priority */
				PWD_taskHandle /* optional task handle to create */
			) != pdPASS) {
  			while(1); /* error! probably out of memory */
	}
  if (FRTOS1_xTaskCreate(
  		Sound_Mngr_Task,  /* pointer to the task */
				"Sound_Mngr", /* task name for kernel awareness debugging */
				configMINIMAL_STACK_SIZE + 32, /* task stack size */
				(void*)NULL, /* optional task startup argument */
				tskIDLE_PRIORITY + 0,  /* initial priority */
				Sound_Mngr_taskHandle/* optional task handle to create */
			) != pdPASS) {
				while(1); /* error! probably out of memory */
	}
	if (FRTOS1_xTaskCreate(
			Refill_Sound_Task,  /* pointer to the task */
				"Refill_Sound", /* task name for kernel awareness debugging */
				configMINIMAL_STACK_SIZE + 32, /* task stack size */
				(void*)NULL, /* optional task startup argument */
				tskIDLE_PRIORITY + 0,  /* initial priority */
				Refill_Sound_taskHandle /* optional task handle to create */
			) != pdPASS) {
				while(1); /* error! probably out of memory */
	}
}
