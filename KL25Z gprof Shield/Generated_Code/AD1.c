/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : AD1.c
**     Project     : MCUX_PE_KL25Z_FRTOS_ShieldwFatFS_gprof
**     Processor   : MKL25Z128VLK4
**     Component   : ADC
**     Version     : Component 01.697, Driver 01.00, CPU db: 3.00.000
**     Repository  : Kinetis
**     Compiler    : GNU C Compiler
**     Date/Time   : 2020-01-10, 10:49, # CodeGen: 0
**     Abstract    :
**         This device "ADC" implements an A/D converter,
**         its control methods and interrupt/event handling procedure.
**     Settings    :
**          Component name                                 : AD1
**          A/D converter                                  : ADC0
**          Sharing                                        : Disabled
**          ADC_LDD                                        : ADC_LDD
**          Interrupt service/event                        : Disabled
**          A/D channels                                   : 4
**            Channel0                                     : 
**              A/D channel (pin)                          : ADC0_SE14/TSI0_CH13/PTC0/EXTRG_IN/CMP0_OUT
**              A/D channel (pin) signal                   : LCD_TS_YD
**              Mode select                                : Single Ended
**            Channel1                                     : 
**              A/D channel (pin)                          : ADC0_SE15/TSI0_CH14/PTC1/LLWU_P6/RTC_CLKIN/I2C1_SCL/TPM0_CH0
**              A/D channel (pin) signal                   : LCD_TS_XL
**              Mode select                                : Single Ended
**            Channel2                                     : 
**              A/D channel (pin)                          : ADC0_DP3/ADC0_SE3/PTE22/TPM2_CH0/UART2_TX
**              A/D channel (pin) signal                   : LCD_TS_YU
**              Mode select                                : Single Ended
**            Channel3                                     : 
**              A/D channel (pin)                          : ADC0_DM3/ADC0_SE7a/PTE23/TPM2_CH1/UART2_RX
**              A/D channel (pin) signal                   : LCD_TS_XR
**              Mode select                                : Single Ended
**          A/D resolution                                 : 16 bits
**          Conversion time                                : 3.916667 �s
**          Low-power mode                                 : Enabled
**          High-speed conversion mode                     : Enabled
**          Asynchro clock output                          : Disabled
**          Sample time                                    : 20 = long
**          Internal trigger                               : Disabled
**          Number of conversions                          : 1
**          Initialization                                 : 
**            Enabled in init. code                        : yes
**            Events enabled in init.                      : yes
**          CPU clock/speed selection                      : 
**            High speed mode                              : This component enabled
**            Low speed mode                               : This component disabled
**            Slow speed mode                              : This component disabled
**     Contents    :
**         Measure        - byte AD1_Measure(bool WaitForResult);
**         MeasureChan    - byte AD1_MeasureChan(bool WaitForResult, byte Channel);
**         GetValue16     - byte AD1_GetValue16(word *Values);
**         GetChanValue16 - byte AD1_GetChanValue16(byte Channel, word *Value);
**         Calibrate      - byte AD1_Calibrate(bool WaitForResult);
**
**     Copyright : 1997 - 2015 Freescale Semiconductor, Inc. 
**     All Rights Reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**     
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**     
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**     
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**     
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**     
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file AD1.c
** @version 01.00
** @brief
**         This device "ADC" implements an A/D converter,
**         its control methods and interrupt/event handling procedure.
*/         
/*!
**  @addtogroup AD1_module AD1 module documentation
**  @{
*/         

/* MODULE AD1. */

#include "AD1.h"

#ifdef __cplusplus
extern "C" {
#endif 
#define STOP            0x00U          /* STOP state           */
#define MEASURE         0x01U          /* MESURE state         */
#define CONTINUOUS      0x02U          /* CONTINUOUS state     */
#define SINGLE          0x03U          /* SINGLE state         */
#define CALIBRATING     0x04U          /* CALIBRATING state    */

static volatile byte SumChan;          /* Counter of measured channels */
static volatile byte ModeFlg;          /* Current state of device */
LDD_TDeviceData *AdcLdd1_DeviceDataPtr; /* Device data pointer */
/* Sample group configuration */
static LDD_ADC_TSample SampleGroup[AD1_SAMPLE_GROUP_SIZE];
static const  byte Table[4] = {0x01U,0x02U,0x04U,0x08U};  /* Table of mask constants */
/* Measure multiple channels flags  */
/* Temporary buffer for converting results */
volatile word AD1_OutV[AD1_SAMPLE_GROUP_SIZE]; /* Sum of measured values */
static bool WaitForRes;                /* Wait for result flag */
/* Calibration in progress flag */
static volatile byte OutFlg;           /* Measurement finish flag */

/*
** ===================================================================
**     Method      :  MainMeasure (component ADC)
**
**     Description :
**         The method performs the conversion of the input channels in 
**         the polling mode.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void AD1_MainMeasure(void)
{

  if (ModeFlg != SINGLE) {
    for (SumChan=0U; SumChan<4U; SumChan++) {
      SampleGroup[0].ChannelIdx = SumChan;
      (void)AdcLdd1_CreateSampleGroup(AdcLdd1_DeviceDataPtr, (LDD_ADC_TSample *)SampleGroup, 1U); /* Configure sample group */
      (void)AdcLdd1_StartSingleMeasurement(AdcLdd1_DeviceDataPtr);
      while (AdcLdd1_GetMeasurementCompleteStatus(AdcLdd1_DeviceDataPtr) == FALSE) {} /* Wait for AD conversion complete */
      AdcLdd1_GetMeasuredValues(AdcLdd1_DeviceDataPtr, (LDD_TData *)&AD1_OutV[SumChan]);
    }
    OutFlg = 0x0FU;                    /* Measured values are available */
  }
  else {
    SampleGroup[0].ChannelIdx = SumChan;
    (void)AdcLdd1_CreateSampleGroup(AdcLdd1_DeviceDataPtr, (LDD_ADC_TSample *)SampleGroup, 1U); /* Configure sample group */
    (void)AdcLdd1_StartSingleMeasurement(AdcLdd1_DeviceDataPtr);
    if (!WaitForRes) {                 /* If doesn't wait for result */
      return;                          /* then return */
    }
    while (AdcLdd1_GetMeasurementCompleteStatus(AdcLdd1_DeviceDataPtr) == FALSE) {} /* Wait for AD conversion complete */
    AdcLdd1_GetMeasuredValues(AdcLdd1_DeviceDataPtr, (LDD_TData *)&AD1_OutV[SumChan]);
    OutFlg |= Table[SumChan];          /* Value of measured channel is available */
  }
  ModeFlg = STOP;                      /* Set the device to the stop mode */
}
/*
** ===================================================================
**     Method      :  AD1_HWEnDi (component ADC)
**
**     Description :
**         Enables or disables the peripheral(s) associated with the 
**         component. The method is called automatically as a part of the 
**         Enable and Disable methods and several internal methods.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void AD1_HWEnDi(void)
{
  if (ModeFlg) {                       /* Start or stop measurement? */
    if (ModeFlg != SINGLE) {
      OutFlg = 0U;                     /* Output values aren't available */
      SampleGroup[0].ChannelIdx = 0U;
    }
    else {
      OutFlg &= (byte)(~(byte)Table[SumChan]); /* Output value isn't available */
      SampleGroup[0].ChannelIdx = SumChan;
    }
    AD1_MainMeasure();
  }
}

/*
** ===================================================================
**     Method      :  AD1_Measure (component ADC)
*/
/*!
**     @brief
**         This method performs one measurement on all channels that
**         are set in the component inspector. (Note: If the [number of
**         conversions] is more than one the conversion of A/D channels
**         is performed specified number of times.)
**     @param
**         WaitForResult   - Wait for a result of a
**                           conversion. If [interrupt service] is
**                           disabled, A/D peripheral doesn't support
**                           measuring all channels at once or Autoscan
**                           mode property isn't enabled and at the same
**                           time the [number of channels] is greater
**                           than 1, then the WaitForResult parameter is
**                           ignored and the method waits for each
**                           result every time. If the [interrupt
**                           service] is disabled and a [number of
**                           conversions] is greater than 1, the
**                           parameter is ignored and the method also
**                           waits for each result every time.
**     @return
**                         - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED - Device is disabled
**                           ERR_BUSY - A conversion is already running
*/
/* ===================================================================*/
byte PE_AD1_Measure(void)
{
  if (ModeFlg != STOP) {               /* Is the device in different mode than "stop"? */
    return ERR_BUSY;                   /* If yes then error */
  }
  ModeFlg = MEASURE;                   /* Set state of device to the measure mode */
  AD1_HWEnDi();                        /* Enable the device */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  AD1_MeasureChan (component ADC)
*/
/*!
**     @brief
**         This method performs measurement on one channel. (Note: If
**         the [number of conversions] is more than one the conversion
**         of the A/D channel is performed specified number of times.)
**     @param
**         WaitForResult   - Wait for a result of
**                           conversion. If the [interrupt service] is
**                           disabled and at the same time a [number of
**                           conversions] is greater than 1, the
**                           WaitForResult parameter is ignored and the
**                           method waits for each result every time.
**     @param
**         Channel         - Channel number. If only one
**                           channel in the component is set this
**                           parameter is ignored, because the parameter
**                           is set inside this method.
**     @return
**                         - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED - Device is disabled
**                           ERR_BUSY - A conversion is already running
**                           ERR_RANGE - Parameter "Channel" out of range
*/
/* ===================================================================*/
/*
The definition of method:
byte AD1_MeasureChan(bool WaitForResult, byte Channel)
was optimised based on the current component setting. An appropriate macro has been defined 
in the AD1.h to maintain API compatibility.
*/
byte AD1_MeasureChan(bool WaitForResult,byte Channel)
{
  if (Channel >= 4U) {                 /* Is channel number greater than or equal to 4 */
    return ERR_RANGE;                  /* If yes then error */
  }
  if (ModeFlg != STOP) {               /* Is the device in different mode than "stop"? */
    return ERR_BUSY;                   /* If yes then error */
  }
  ModeFlg = SINGLE;                    /* Set state of device to the measure mode */
  SumChan = Channel;                   /* Set required channel */
  WaitForRes = WaitForResult;          /* Save Wait for result flag */
  AD1_HWEnDi();                        /* Enable the device */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  AD1_GetValue16 (component ADC)
*/
/*!
**     @brief
**         This method returns the last measured values of all channels.
**         Compared with [GetValue] method this method returns more
**         accurate result if the [number of conversions] is greater
**         than 1 and [AD resolution] is less than 16 bits. In addition,
**         the user code dependency on [AD resolution] is eliminated.
**     @param
**         Values          - Pointer to the array that contains
**                           the measured data.
**     @return
**                         - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_NOTAVAIL - Requested value not
**                           available
**                           ERR_OVERRUN - External trigger overrun flag
**                           was detected after the last value(s) was
**                           obtained (for example by GetValue). This
**                           error may not be supported on some CPUs
**                           (see generated code).
*/
/* ===================================================================*/
byte AD1_GetValue16(word *Values)
{
  if (OutFlg != 0x0FU) {               /* Is output flag set? */
    if (AdcLdd1_GetMeasurementCompleteStatus(AdcLdd1_DeviceDataPtr) != FALSE) {
      AdcLdd1_GetMeasuredValues(AdcLdd1_DeviceDataPtr, (LDD_TData *)&AD1_OutV[SumChan]); /* Save measured value */
      OutFlg |= Table[SumChan];        /* Value of measured channel is available */
      (void)AdcLdd1_CancelMeasurement(AdcLdd1_DeviceDataPtr); /* Stop the device */
      ModeFlg = STOP;                  /* Set the device to the stop mode */
      if (OutFlg != 0x0FU) {           /* Is output flag set? */
        return ERR_NOTAVAIL;           /* If no then error */
      }
    }
    else {
      return ERR_NOTAVAIL;             /* If no then error */
    }
  }
  Values[0] = AD1_OutV[0];             /* Save measured values to the output buffer */
  Values[1] = AD1_OutV[1];             /* Save measured values to the output buffer */
  Values[2] = AD1_OutV[2];             /* Save measured values to the output buffer */
  Values[3] = AD1_OutV[3];             /* Save measured values to the output buffer */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  AD1_GetChanValue16 (component ADC)
*/
/*!
**     @brief
**         This method returns the last measured value of the required
**         channel. Compared with [GetChanValue] method this method
**         returns more accurate result if the [number of conversions]
**         is greater than 1 and [AD resolution] is less than 16 bits.
**         In addition, the user code dependency on [AD resolution] is
**         eliminated.
**     @param
**         Channel         - Channel number. If only one
**                           channel in the component is set then this
**                           parameter is ignored.
**     @param
**         Value           - Pointer to the measured value.
**     @return
**                         - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_NOTAVAIL - Requested value not
**                           available
**                           ERR_RANGE - Parameter "Channel" out of
**                           range
**                           ERR_OVERRUN - External trigger overrun flag
**                           was detected after the last value(s) was
**                           obtained (for example by GetValue). This
**                           error may not be supported on some CPUs
**                           (see generated code).
*/
/* ===================================================================*/
byte AD1_GetChanValue16(byte Channel, word *Value)
{
  if (Channel >= 4U) {                 /* Is channel number greater than or equal to 4 */
    return ERR_RANGE;                  /* If yes then error */
  }
  if ((OutFlg & Table[Channel]) == 0U) { /* Is output flag set? */
    if (AdcLdd1_GetMeasurementCompleteStatus(AdcLdd1_DeviceDataPtr) != FALSE) {
      AdcLdd1_GetMeasuredValues(AdcLdd1_DeviceDataPtr, (LDD_TData *)&AD1_OutV[SumChan]); /* Save measured value */
      OutFlg |= Table[SumChan];        /* Value of measured channel is available */
      (void)AdcLdd1_CancelMeasurement(AdcLdd1_DeviceDataPtr); /* Stop the device */
      ModeFlg = STOP;                  /* Set the device to the stop mode */
      if ((OutFlg & Table[Channel]) == 0U) { /* Is output flag set? */
        return ERR_NOTAVAIL;           /* If no then error */
      }
    }
    else {
      return ERR_NOTAVAIL;             /* If no then error */
    }
  }
  *Value = AD1_OutV[Channel];          /* Save measured values to the output buffer */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  AD1_Calibrate (component ADC)
**     Description :
**         This method starts self calibration process. Calibration is
**         typically used to remove the effects of the gain and offset
**         from a specific reading.
**     Parameters  :
**         NAME            - DESCRIPTION
**         WaitForResult   - Wait for a result of
**                           calibration. If the <interrupt service> is
**                           disabled, the WaitForResult parameter is
**                           ignored and the method waits for
**                           calibration result every time.
**     Returns     :
**         ---             - Error code
**                           ERR_OK - OK
**                           ERR_BUSY - A conversion is already running
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED - Device is disabled
**                           ERR_FAILED - Calibration hasn't been
**                           finished correctly
** ===================================================================
*/
byte PE_AD1_Calibrate(void)
{
  if (ModeFlg != STOP) {               /* Is the device in different mode than "stop"? */
    return ERR_BUSY;                   /* If yes then error */
  }
  (void)AdcLdd1_GetMeasurementCompleteStatus(AdcLdd1_DeviceDataPtr); /* Clear measurement complete status */
  (void)AdcLdd1_StartCalibration(AdcLdd1_DeviceDataPtr); /* Start calibration */
  while (!AdcLdd1_GetMeasurementCompleteStatus(AdcLdd1_DeviceDataPtr)) {}; /* Wait until calibration ends */
  if (AdcLdd1_GetCalibrationResultStatus(AdcLdd1_DeviceDataPtr) != ERR_OK) { /* If calibration failed flag is set */
    ModeFlg = STOP;                    /* Set the device to the stop mode */
    return ERR_FAILED;                 /* Return ERR_FAILED error code */
  }
  return ERR_OK;                       /* ADC device is now calibrated */
}

/*
** ===================================================================
**     Method      :  AD1_Init (component ADC)
**
**     Description :
**         Initializes the associated peripheral(s) and the component 
**         internal variables. The method is called automatically as a 
**         part of the application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void AD1_Init(void)
{
  OutFlg = 0U;                         /* No measured value */
  ModeFlg = STOP;                      /* Device isn't running */
  AdcLdd1_DeviceDataPtr = AdcLdd1_Init(NULL); /* Calling init method of the inherited component */
}

/* END AD1. */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

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
