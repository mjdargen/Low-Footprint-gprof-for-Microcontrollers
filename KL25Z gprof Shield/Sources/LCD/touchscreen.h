#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

/*
	 * Channel 0 - YD - PTC0
	 * Channel 1 - XL - PTC1
	 * Channel 2 - YU - PTC 22
	 * Channel 3 - XR - PTC 23
 */
#define YD_CHANNEL 0
#define XL_CHANNEL 1
#define YU_CHANNEL 2
#define XR_CHANNEL 3


void LCD_TS_Init(void);
uint32_t LCD_TS_Read(PT_T * position);
void LCD_TS_Blocking_Read(PT_T * position);
void LCD_TS_Calibrate(void);
void LCD_TS_Test(void);

#endif

