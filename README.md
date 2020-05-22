# Low Footprint gprof for Microcontrollers
 A low footprint gprof-based profiler for microcontrollers.
\
\
*Michael D'Argenio  
mjdargen@gmail.com  
https://dargenio.dev  
https://github.com/mjdargen  
April 15, 2020*   


## Abstract  
Microcontrollers are a highly constrained platform. They are selected for special-purpose applications to reduce the overhead and keep the cost minimal. Due to their use-cases, microcontrollers are much more restricted when it comes to memory, microarchitecture, speed, processing power, and energy consumption. Because microcontrollers must respond in real-time, performing optimizations can be critical. These optimizations can improve performance and reduce latency to enable the completion of complex tasks on limited hardware. Without profiling, engineers can waste significant development time on optimizations that result in little gain while overlooking opportunities for huge improvement.  
\
Profiling is the key to unlocking critical optimizations for microcontrollers but attempting to profile microcontrollers exposes a paradox. Profiling finds opportunities for improvement; however, profiling can deplete the limited memory and overburden the microcontroller’s processor. There are several profiling solutions for microcontrollers in existence. These solutions often require more memory and processing power than the programs can spare. In addition, many of these profilers are only suitable for higher-end microcontrollers with special debug hardware embedded in the chip. These solutions are typically proprietary with little technology transferred. They function only on their intended platforms. As a result, they have differing feature sets that perform with varying degrees of success.  
\
This thesis attempts to shift that paradigm. The proposed framework tears down those silos of information by delivering a solution that works for all microcontrollers and advances the on-going efforts to produce an open-source, processor-agnostic development toolchain. To adapt gprof for a microcontroller environment, this thesis proposes the novel idea of dividing the profiling responsibilities between the target microcontroller and the host personal computer. By divvying up the profiling responsibilities, it minimizes the overhead on the microcontroller and allows the program to execute unencumbered. The result is a profiler with identical functionality that generates the same, expressive profiles as the original implementation. The profiler surpasses existing solutions by limiting the demand on the microcontroller’s constrained resources. This profiler is ripe for adoption and continued growth due to its elegant approach and its embrace of the burgeoning, open-source development effort within the embedded systems community.  

NCSU Libraries Link to thesis: http://www.lib.ncsu.edu/resolver/1840.20/37531  

## Directory Overview  
```
|-- home
    |-- Host PC Application     -   Host PC Application Source Code
    |   |-- Validation          -   Python Script Validation for Host PC Application
    |-- KL25Z gprof Blinky      -   Blinky Demo Program for Target Microcontroller
    |-- KL25Z gprof Shield      -   Shield Demo Program for Target Microcontroller
    |-- Results                 -   gprof Outputs for Validation Runs
    |   |-- blinky              -   Results for Blinky Program
    |   |-- shield              -   Results for Shield Program
    |-- Target MCU Application  -   Target Microcontroller Profiling Source Code
    |-- defense.pdf             -   Copy of slides from thesis defense
    |-- thesis.pdf              -   Copy of thesis
```

## File Structure  
```
|-- home
    |-- .gitattributes
    |-- .gitignore
    |-- LICENSE
    |-- README.md
    |-- Host PC Application
    |   |-- gmon.c
    |   |-- gmon.h
    |   |-- gmon_out.h
    |   |-- main.c
    |   |-- Makefile
    |   |-- mcount.c
    |   |-- profil.c
    |   |-- profil.h
    |   |-- Validation
    |       |-- arc.csv
    |       |-- gmon_validation.py
    |       |-- pc.csv
    |       |-- validation.csv
    |-- KL25Z gprof Blinky
    |   |-- .cproject
    |   |-- .cwGeneratedFileSetLog
    |   |-- .project
    |   |-- ProcessorExpert.pe
    |   |-- ProjectInfo.xml
    |   |-- .settings
    |   |   |-- com.freescale.processorexpert.core.prefs
    |   |   |-- com.freescale.processorexpert.derivative.prefs
    |   |   |-- language.settings.xml
    |   |   |-- org.eclipse.cdt.managedbuilder.core.prefs
    |   |-- Debug
    |   |   |-- .gitignore
    |   |-- Documentation
    |   |-- Generated_Code
    |   |   |-- BitIoLdd2.c
    |   |   |-- BitIoLdd2.h
    |   |   |-- BitIoLdd3.c
    |   |   |-- BitIoLdd3.h
    |   |   |-- BitIoLdd4.c
    |   |   |-- BitIoLdd4.h
    |   |   |-- Cpu.c
    |   |   |-- Cpu.h
    |   |   |-- CS1.c
    |   |   |-- CS1.h
    |   |   |-- CS1config.h
    |   |   |-- IO_Map.h
    |   |   |-- KSDK1.c
    |   |   |-- KSDK1.h
    |   |   |-- LEDB.c
    |   |   |-- LEDB.h
    |   |   |-- LEDBconfig.h
    |   |   |-- LEDG.c
    |   |   |-- LEDG.h
    |   |   |-- LEDGconfig.h
    |   |   |-- LEDpin2.c
    |   |   |-- LEDpin2.h
    |   |   |-- LEDpin3.c
    |   |   |-- LEDpin3.h
    |   |   |-- LEDpin4.c
    |   |   |-- LEDpin4.h
    |   |   |-- LEDR.c
    |   |   |-- LEDR.h
    |   |   |-- LEDRconfig.h
    |   |   |-- MCUC1.c
    |   |   |-- MCUC1.h
    |   |   |-- MCUC1config.h
    |   |   |-- PE_Const.h
    |   |   |-- PE_Error.h
    |   |   |-- PE_LDD.c
    |   |   |-- PE_LDD.h
    |   |   |-- PE_Types.h
    |   |   |-- Vectors.c
    |   |   |-- WAIT1.c
    |   |   |-- WAIT1.h
    |   |   |-- WAIT1config.h
    |   |-- Profiling
    |   |   |-- gmon_arc.c
    |   |   |-- gmon_profil.c
    |   |   |-- gmon_profil.h
    |   |   |-- gmon_queue.c
    |   |   |-- gmon_queue.h
    |   |   |-- gmon_serial.c
    |   |   |-- gmon_serial.h
    |   |   |-- mcount.S
    |   |-- Project_Settings
    |   |   |-- Debugger
    |   |   |   |-- mysimpleblinky_Debug_PNE.launch
    |   |   |-- Linker_Files
    |   |   |   |-- ProcessorExpert.ld
    |   |   |-- Startup_Code
    |   |       |-- startup.c
    |   |-- Sources
    |   |   |-- Events.c
    |   |   |-- Events.h
    |   |   |-- leds.c
    |   |   |-- leds.h
    |   |   |-- main.c
    |   |-- Static_Code
    |       |-- IO_Map
    |       |   |-- MKL25Z4.h
    |       |-- PDD
    |           |-- ADC_PDD.h
    |           |-- CMP_PDD.h
    |           |-- COP_PDD.h
    |           |-- DAC_PDD.h
    |           |-- DMAMUX_PDD.h
    |           |-- DMA_PDD.h
    |           |-- FTFA_PDD.h
    |           |-- GPIO_PDD.h
    |           |-- I2C_PDD.h
    |           |-- LPTMR_PDD.h
    |           |-- MCG_PDD.h
    |           |-- MCM_PDD.h
    |           |-- OSC_PDD.h
    |           |-- PDD_Types.h
    |           |-- PIT_PDD.h
    |           |-- PORT_PDD.h
    |           |-- RTC_PDD.h
    |           |-- SIM_PDD.h
    |           |-- SPI_PDD.h
    |           |-- SysTick_PDD.h
    |           |-- TPM_PDD.h
    |           |-- TSI_PDD.h
    |           |-- UART0_PDD.h
    |           |-- UART_PDD.h
    |           |-- USB_PDD.h
    |-- KL25Z gprof Shield
    |   |-- .cproject
    |   |-- .cwGeneratedFileSetLog
    |   |-- .project
    |   |-- ProcessorExpert.pe
    |   |-- ProjectInfo.xml
    |   |-- .settings
    |   |   |-- com.freescale.processorexpert.core.prefs
    |   |   |-- com.freescale.processorexpert.derivative.prefs
    |   |   |-- language.settings.xml
    |   |   |-- org.eclipse.cdt.codan.core.prefs
    |   |   |-- org.eclipse.cdt.managedbuilder.core.prefs
    |   |-- Debug
    |   |   |-- .gitignore
    |   |-- Documentation
    |   |-- Generated_Code
    |   |   |-- .gdbinit-FreeRTOS-helpers
    |   |   |-- 00readme.txt
    |   |   |-- AD1.c
    |   |   |-- AD1.h
    |   |   |-- AdcLdd1.c
    |   |   |-- AdcLdd1.h
    |   |   |-- AS1.c
    |   |   |-- AS1.h
    |   |   |-- ASerialLdd1.c
    |   |   |-- ASerialLdd1.h
    |   |   |-- AudioAmp.c
    |   |   |-- AudioAmp.h
    |   |   |-- BitIoLdd1.c
    |   |   |-- BitIoLdd1.h
    |   |   |-- BitIoLdd2.c
    |   |   |-- BitIoLdd2.h
    |   |   |-- BitIoLdd3.c
    |   |   |-- BitIoLdd3.h
    |   |   |-- CI2C1.c
    |   |   |-- CI2C1.h
    |   |   |-- CLS1.c
    |   |   |-- CLS1.h
    |   |   |-- CLS1config.h
    |   |   |-- Cpu.c
    |   |   |-- Cpu.h
    |   |   |-- croutine.c
    |   |   |-- croutine.h
    |   |   |-- CS1.c
    |   |   |-- CS1.h
    |   |   |-- CS1config.h
    |   |   |-- DA1.c
    |   |   |-- DA1.h
    |   |   |-- DacLdd1.c
    |   |   |-- DacLdd1.h
    |   |   |-- DacLdd1_DMA0.c
    |   |   |-- DacLdd1_DMA0.h
    |   |   |-- DBG_1.c
    |   |   |-- DBG_1.h
    |   |   |-- DBG_2.c
    |   |   |-- DBG_2.h
    |   |   |-- DBG_3.c
    |   |   |-- DBG_3.h
    |   |   |-- DBG_4.c
    |   |   |-- DBG_4.h
    |   |   |-- DBG_5.c
    |   |   |-- DBG_5.h
    |   |   |-- DBG_6.c
    |   |   |-- DBG_6.h
    |   |   |-- DBG_7.c
    |   |   |-- DBG_7.h
    |   |   |-- deprecated_definitions.h
    |   |   |-- diskio.h
    |   |   |-- DMA1.c
    |   |   |-- DMA1.h
    |   |   |-- event_groups.c
    |   |   |-- event_groups.h
    |   |   |-- FAT1.c
    |   |   |-- FAT1.h
    |   |   |-- FAT1config.h
    |   |   |-- ff.c
    |   |   |-- ff.h
    |   |   |-- ffconf.h
    |   |   |-- FreeRTOS.h
    |   |   |-- FreeRTOSConfig.h
    |   |   |-- FreeRTOS_license.txt
    |   |   |-- freertos_tasks_c_additions.h
    |   |   |-- FRTOS1.c
    |   |   |-- FRTOS1.h
    |   |   |-- FRTOS1config.h
    |   |   |-- GI2C1.c
    |   |   |-- GI2C1.h
    |   |   |-- GI2C1config.h
    |   |   |-- HBLED.c
    |   |   |-- HBLED.h
    |   |   |-- heap_1.c
    |   |   |-- heap_2.c
    |   |   |-- heap_3.c
    |   |   |-- heap_4.c
    |   |   |-- heap_5.c
    |   |   |-- heap_useNewlib.c
    |   |   |-- heap_useNewlib.txt
    |   |   |-- integer.h
    |   |   |-- IO_Map.h
    |   |   |-- LCD_BL_PWM.c
    |   |   |-- LCD_BL_PWM.h
    |   |   |-- LCD_Data_Bits.c
    |   |   |-- LCD_Data_Bits.h
    |   |   |-- LCD_D_NC.c
    |   |   |-- LCD_D_NC.h
    |   |   |-- LCD_NRD.c
    |   |   |-- LCD_NRD.h
    |   |   |-- LCD_NRST.c
    |   |   |-- LCD_NRST.h
    |   |   |-- LCD_NWR.c
    |   |   |-- LCD_NWR.h
    |   |   |-- LEDB.c
    |   |   |-- LEDB.h
    |   |   |-- LEDBconfig.h
    |   |   |-- LEDG.c
    |   |   |-- LEDG.h
    |   |   |-- LEDGconfig.h
    |   |   |-- LEDpin1.c
    |   |   |-- LEDpin1.h
    |   |   |-- LEDpin2.c
    |   |   |-- LEDpin2.h
    |   |   |-- LEDpin3.c
    |   |   |-- LEDpin3.h
    |   |   |-- LEDR.c
    |   |   |-- LEDR.h
    |   |   |-- LEDRconfig.h
    |   |   |-- list.c
    |   |   |-- list.h
    |   |   |-- MCUC1.c
    |   |   |-- MCUC1.h
    |   |   |-- MCUC1config.h
    |   |   |-- message_buffer.h
    |   |   |-- MMA1.c
    |   |   |-- MMA1.h
    |   |   |-- MMA1config.h
    |   |   |-- mpu_prototypes.h
    |   |   |-- mpu_wrappers.c
    |   |   |-- mpu_wrappers.h
    |   |   |-- PE_Const.h
    |   |   |-- PE_Error.h
    |   |   |-- PE_LDD.c
    |   |   |-- PE_LDD.h
    |   |   |-- PE_Types.h
    |   |   |-- port.c
    |   |   |-- portable.h
    |   |   |-- portasm.s
    |   |   |-- portmacro.h
    |   |   |-- portTicks.h
    |   |   |-- projdefs.h
    |   |   |-- queue.c
    |   |   |-- queue.h
    |   |   |-- readme_gdbBacktraceDebug.txt
    |   |   |-- SD1.c
    |   |   |-- SD1.h
    |   |   |-- semphr.h
    |   |   |-- SM2.c
    |   |   |-- SM2.h
    |   |   |-- stack_macros.h
    |   |   |-- stream_buffer.c
    |   |   |-- stream_buffer.h
    |   |   |-- task.h
    |   |   |-- tasks.c
    |   |   |-- timers.c
    |   |   |-- timers.h
    |   |   |-- TmDt1.c
    |   |   |-- TmDt1.h
    |   |   |-- TmDt1config.h
    |   |   |-- TMOUT1.c
    |   |   |-- TMOUT1.h
    |   |   |-- TMOUT1config.h
    |   |   |-- TPM0.c
    |   |   |-- TPM0.h
    |   |   |-- TPM1_0.c
    |   |   |-- TPM1_0.h
    |   |   |-- UTIL1.c
    |   |   |-- UTIL1.h
    |   |   |-- UTIL1config.h
    |   |   |-- Vectors.c
    |   |   |-- WAIT1.c
    |   |   |-- WAIT1.h
    |   |   |-- WAIT1config.h
    |   |   |-- XF1.c
    |   |   |-- XF1.h
    |   |   |-- XF1config.h
    |   |-- Profiling
    |   |   |-- gmon_arc.c
    |   |   |-- gmon_profil.c
    |   |   |-- gmon_profil.h
    |   |   |-- gmon_queue.c
    |   |   |-- gmon_queue.h
    |   |   |-- gmon_serial.c
    |   |   |-- gmon_serial.h
    |   |   |-- mcount.S
    |   |-- Project_Settings
    |   |   |-- Debugger
    |   |   |   |-- MCUX_PE_KL25Z_FRTOS_ShieldwFatFS_Debug_PNE.launch
    |   |   |-- Linker_Files
    |   |   |   |-- ProcessorExpert.ld
    |   |   |-- Startup_Code
    |   |       |-- startup.c
    |   |-- Sources
    |   |   |-- Events.c
    |   |   |-- Events.h
    |   |   |-- GPIO_defs.h
    |   |   |-- main.c
    |   |   |-- MMA8451.c
    |   |   |-- MMA8451.h
    |   |   |-- myfatfs.c
    |   |   |-- myfatfs.h
    |   |   |-- mytasks.c
    |   |   |-- mytasks.h
    |   |   |-- sound.c
    |   |   |-- sound.h
    |   |   |-- LCD
    |   |       |-- font.h
    |   |       |-- LCD.h
    |   |       |-- LCD_driver.h
    |   |       |-- LCD_graphics.c
    |   |       |-- LCD_graphics.h
    |   |       |-- LCD_text.c
    |   |       |-- lucida_12x19.c
    |   |       |-- lucida_20x31.c
    |   |       |-- lucida_8x13.c
    |   |       |-- misc.h
    |   |       |-- ST7789.c
    |   |       |-- ST7789.h
    |   |       |-- touchscreen.c
    |   |       |-- touchscreen.h
    |   |-- Static_Code
    |       |-- IO_Map
    |       |   |-- MKL25Z4.h
    |       |-- PDD
    |           |-- ADC_PDD.h
    |           |-- CMP_PDD.h
    |           |-- COP_PDD.h
    |           |-- DAC_PDD.h
    |           |-- DMAMUX_PDD.h
    |           |-- DMA_PDD.h
    |           |-- FTFA_PDD.h
    |           |-- GPIO_PDD.h
    |           |-- I2C_PDD.h
    |           |-- LPTMR_PDD.h
    |           |-- MCG_PDD.h
    |           |-- MCM_PDD.h
    |           |-- OSC_PDD.h
    |           |-- PDD_Types.h
    |           |-- PIT_PDD.h
    |           |-- PORT_PDD.h
    |           |-- RTC_PDD.h
    |           |-- SIM_PDD.h
    |           |-- SPI_PDD.h
    |           |-- SysTick_PDD.h
    |           |-- TPM_PDD.h
    |           |-- TSI_PDD.h
    |           |-- UART0_PDD.h
    |           |-- UART_PDD.h
    |           |-- USB_PDD.h
    |-- Results
    |   |-- blinky
    |   |   |-- blink1cg.png
    |   |   |-- blink1cg.svg
    |   |   |-- blink1gmon.out
    |   |   |-- blink1gprof.txt
    |   |   |-- blink1hex.txt
    |   |   |-- blink2cg.png
    |   |   |-- blink2cg.svg
    |   |   |-- blink2gmon.out
    |   |   |-- blink2gprof.txt
    |   |   |-- blink2hex.txt
    |   |   |-- blink3cg.png
    |   |   |-- blink3cg.svg
    |   |   |-- blink3gmon.out
    |   |   |-- blink3gprof.txt
    |   |   |-- blink3hex.txt
    |   |   |-- blink4cg.png
    |   |   |-- blink4cg.svg
    |   |   |-- blink4gmon.out
    |   |   |-- blink4gprof.txt
    |   |   |-- blink4hex.txt
    |   |   |-- blinky.elf
    |   |-- shield
    |       |-- shield.elf
    |       |-- shield1cg.png
    |       |-- shield1cg.svg
    |       |-- shield1gmon.out
    |       |-- shield1gprof.txt
    |       |-- shield1hex.txt
    |       |-- shield2cg.png
    |       |-- shield2cg.svg
    |       |-- shield2gmon.out
    |       |-- shield2gprof.txt
    |       |-- shield2hex.txt
    |       |-- shield3cg.png
    |       |-- shield3cg.svg
    |       |-- shield3gmon.out
    |       |-- shield3gprof.txt
    |       |-- shield3hex.txt
    |       |-- shield4cg.png
    |       |-- shield4cg.svg
    |       |-- shield4gmon.out
    |       |-- shield4gprof.txt
    |       |-- shield4hex.txt
    |-- Target MCU Application
        |-- gmon_arc.c
        |-- gmon_profil.c
        |-- gmon_profil.h
        |-- gmon_queue.c
        |-- gmon_queue.h
        |-- gmon_serial.c
        |-- gmon_serial.h
        |-- mcount.S
```
