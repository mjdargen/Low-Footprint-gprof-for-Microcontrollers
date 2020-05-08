/*
 * mytasks.h
 *
 *  Created on: Oct 24, 2019
 *      Author: Michael
 */

#ifndef SOURCES_MYTASKS_H_
#define SOURCES_MYTASKS_H_

// RTOS task periods in ms
#define THREAD_READ_TS_PERIOD_MS 						50
#define THREAD_UPDATE_SCREEN_PERIOD_MS 			50
#define THREAD_READ_ACCELEROMETER_PERIOD_MS 100
#define INITIAL_DELAY_THREAD_FATFS					1000
#define THREAD_FATFS_PERIOD_MS 							100
#define THREAD_RGB_LED_PERIOD_MS						500
#define THREAD_CMD_LINE_INT_PERIOD_MS 			50
#define INITIAL_DELAY_THREAD_PWD						1500
#define THREAD_PWD_PERIOD_MS								500

// Game Constants
#define PADDLE_WIDTH (40)
#define PADDLE_HEIGHT (15)
#define PADDLE_Y_POS (LCD_HEIGHT-4-PADDLE_HEIGHT)

void CreateTasks(void);
void control_LEDs(bool R, bool G, bool B);

extern SemaphoreHandle_t LCD_mutex;
extern EventGroupHandle_t xCreatedEventGroup;

#endif /* SOURCES_MYTASKS_H_ */
