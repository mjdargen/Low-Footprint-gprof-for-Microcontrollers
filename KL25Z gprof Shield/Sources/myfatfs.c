/*
 * myfatfs.c
 *
 *  Created on: Oct 24, 2019
 *      Author: Michael
 */

#include "FRTOS1.h"
#include "mytasks.h"
#include "myfatfs.h"
#include "FAT1.h"
#include "LCD.h"
#include "touchscreen.h"
#include "LCD_driver.h"
#include "font.h"
#include "gpio_defs.h"
#include "ST7789.h"

#include <stdio.h>

extern SemaphoreHandle_t LCD_mutex;

char pwd(const uint8_t *dirPathPtr) {
  FAT1_FILINFO fInfo;
  FAT1_FRESULT fres;
  FAT1_DIR dir;  /* Directory object */

  char type;
  char buffer[16];

  fres = FAT1_f_opendir(&dir, (const TCHAR*)dirPathPtr);
  if (fres != FR_OK) {
    return ERR_FAULT;
  }

  for(int i=0; i<0xFF; i++) {
    fres = FAT1_readdir(&dir, &fInfo);
    if (fres != FR_OK) {
      // readdir failed
      break;
    }
    if (!fInfo.fname[0]) { /* end of directory list */
      break;
    }
    /* file attributes */
    if (fInfo.fattrib & AM_DIR) { /* directory */
      type = 'D';
    }
    else { /* file */
      type = '-';
    }

    // print file name and whether it's a directory or not
    sprintf(buffer,"%c %s", type, fInfo.fname);
    if (i < LCD_MAX_ROWS-2) {
			xSemaphoreTake(LCD_mutex, portMAX_DELAY);
			LCD_Text_PrintStr_RC(2+i, 0, buffer);
			xSemaphoreGive(LCD_mutex);
    }
  }
  (void)FAT1_f_closedir(&dir); /* close directory */
  return ERR_OK;
}
