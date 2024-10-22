/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : MMA1.c
**     CDE edition : Standard
**     Project     : MCUX_PE_KL25Z_FRTOS_ShieldwFatFS_gprof
**     Processor   : MKL25Z128VLK4
**     Component   : MMA8451Q
**     Version     : Component 01.046, Driver 01.00, CPU db: 3.00.000
**     Repository  : My Components
**     Compiler    : GNU C Compiler
**     Date/Time   : 2020-01-10, 10:49, # CodeGen: 0
**     Abstract    :
**         Implements a Driver for the MMA8451 accelerometer from NXP/Freescale.
**     Settings    :
**          Component Name                                 : MMA1
**          Slave Address                                  : 1D
**          I2C Bus                                        : GI2C1
**          Wait                                           : WAIT1
**          SDK                                            : MCUC1
**          Init Driver in startup                         : no
**          Constant Offsets                               : Enabled
**            X offset                                     : 0
**            Y offset                                     : 0
**            Z offset                                     : 0
**          Shell                                          : Disabled
**     Contents    :
**         Enable         - uint8_t MMA1_Enable(void);
**         Disable        - uint8_t MMA1_Disable(void);
**         isEnabled      - uint8_t MMA1_isEnabled(bool *isEnabled);
**         SwReset        - uint8_t MMA1_SwReset(void);
**         ReadReg8       - uint8_t MMA1_ReadReg8(uint8_t addr, uint8_t *val);
**         WriteReg8      - uint8_t MMA1_WriteReg8(uint8_t addr, uint8_t val);
**         GetX           - int16_t MMA1_GetX(void);
**         GetY           - int16_t MMA1_GetY(void);
**         GetZ           - int16_t MMA1_GetZ(void);
**         GetRaw8XYZ     - uint8_t MMA1_GetRaw8XYZ(uint8_t *xyz);
**         CalibrateX1g   - void MMA1_CalibrateX1g(void);
**         CalibrateY1g   - void MMA1_CalibrateY1g(void);
**         CalibrateZ1g   - void MMA1_CalibrateZ1g(void);
**         GetXmg         - int16_t MMA1_GetXmg(void);
**         GetYmg         - int16_t MMA1_GetYmg(void);
**         GetZmg         - int16_t MMA1_GetZmg(void);
**         MeasureGetRawX - uint16_t MMA1_MeasureGetRawX(void);
**         MeasureGetRawY - uint16_t MMA1_MeasureGetRawY(void);
**         MeasureGetRawZ - uint16_t MMA1_MeasureGetRawZ(void);
**         GetXOffset     - int16_t MMA1_GetXOffset(void);
**         GetYOffset     - int16_t MMA1_GetYOffset(void);
**         GetZOffset     - int16_t MMA1_GetZOffset(void);
**         GetX1gValue    - int16_t MMA1_GetX1gValue(void);
**         GetY1gValue    - int16_t MMA1_GetY1gValue(void);
**         GetZ1gValue    - int16_t MMA1_GetZ1gValue(void);
**         SetFastMode    - uint8_t MMA1_SetFastMode(bool on);
**         WhoAmI         - uint8_t MMA1_WhoAmI(uint8_t *value);
**         GetScaleRangeG - uint8_t MMA1_GetScaleRangeG(uint8_t *gScale);
**         SetScaleRangeG - uint8_t MMA1_SetScaleRangeG(uint8_t gScale);
**         Deinit         - uint8_t MMA1_Deinit(void);
**         Init           - uint8_t MMA1_Init(void);
**
**     * Copyright (c) 2013-2018, Erich Styger
**      * Web:         https://mcuoneclipse.com
**      * SourceForge: https://sourceforge.net/projects/mcuoneclipse
**      * Git:         https://github.com/ErichStyger/McuOnEclipse_PEx
**      * All rights reserved.
**      *
**      * Redistribution and use in source and binary forms, with or without modification,
**      * are permitted provided that the following conditions are met:
**      *
**      * - Redistributions of source code must retain the above copyright notice, this list
**      *   of conditions and the following disclaimer.
**      *
**      * - Redistributions in binary form must reproduce the above copyright notice, this
**      *   list of conditions and the following disclaimer in the documentation and/or
**      *   other materials provided with the distribution.
**      *
**      * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**      * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**      * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**      * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**      * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**      * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**      * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**      * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**      * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**      * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
** ###################################################################*/
/*!
** @file MMA1.c
** @version 01.00
** @brief
**         Implements a Driver for the MMA8451 accelerometer from NXP/Freescale.
*/         
/*!
**  @addtogroup MMA1_module MMA1 module documentation
**  @{
*/         

/* MODULE MMA1. */

#include "MMA1.h"

typedef struct {
  int16_t NxOff; /* offset for X axis */
  int16_t NyOff; /* offset for Y axis */
  int16_t NzOff; /* offset for Z axis */
} tAccelCal;

/* default calibration values from component properties */
static const tAccelCal InitialCalibration = { /* Initial default calibration values */
  0, /* X offset */
  0, /* Y offset */
  0, /* Z offset */
};
static tAccelCal sCalValues; /* calibration values in RAM */

#define CalNxOff   sCalValues.NxOff
#define CalNyOff   sCalValues.NyOff
#define CalNzOff   sCalValues.NzOff

/*
** ===================================================================
**     Method      :  MMA1_GetRaw8XYZ (component MMA8451Q)
**     Description :
**         Returns in an array the x, y and z accelerometer as 8bit
**         values.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * xyz             - Pointer to an array of three unsigned 8bit
**                           values which are used to return the
**                           accelerometer values.
**     Returns     :
**         ---             - Error code, ERR_OK for no error.
** ===================================================================
*/
uint8_t MMA1_GetRaw8XYZ(uint8_t *xyz)
{
  static const uint8_t addr = MMA1_OUT_X_MSB;

  return GI2C1_ReadAddress(MMA1_I2C_ADDR, (uint8_t*)&addr, sizeof(addr), &xyz[0], 3);
}

/*
** ===================================================================
**     Method      :  MMA1_Deinit (component MMA8451Q)
**     Description :
**         Counterpart to Init() method.
**     Parameters  : None
**     Returns     :
**         ---             - Error code, ERR_OK if everything is ok.
** ===================================================================
*/
uint8_t MMA1_Deinit(void)
{
  return ERR_OK; /* nothing to do */
}

/*
** ===================================================================
**     Method      :  MMA1_Init (component MMA8451Q)
**     Description :
**         Initializes the device driver
**     Parameters  : None
**     Returns     :
**         ---             - Error code, ERR_OK if everything is ok.
** ===================================================================
*/
uint8_t MMA1_Init(void)
{
  sCalValues.NxOff = InitialCalibration.NxOff;
  sCalValues.NyOff = InitialCalibration.NyOff;
  sCalValues.NzOff = InitialCalibration.NzOff;
  return GI2C1_WriteByteAddress8(MMA1_I2C_ADDR, MMA1_CTRL_REG_1, MMA1_ACTIVE_BIT_MASK); /* enable device */
}

/*
** ===================================================================
**     Method      :  MMA1_CalibrateX1g (component MMA8451Q)
**     Description :
**         Performs a calibration of the sensor. It is assumed that the
**         Y and Z sensors have 0 g, and the X sensor has 1 g.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void MMA1_CalibrateX1g(void)
{
  /* assumption is that accelerometer is placed with 1g for X and 0g for Y and Z */
  int32_t X=0, Y=0, Z=0;
  uint8_t i;

  /* Get the raw data and remove 2 bits (16bit to 14bit) */
  for (i=0; i<8; i++) {
   X += ((int16_t)MMA1_MeasureGetRawX() >> 2);
   Y += ((int16_t)MMA1_MeasureGetRawY() >> 2);
   Z += ((int16_t)MMA1_MeasureGetRawZ() >> 2);
  }
  /* build average of 8 measured values */
  X >>= 3;
  Y >>= 3;
  Z >>= 3;
  /* store the calibration values */
  /* offset: both Y and Z shall have zero g */
  sCalValues.NzOff = (X - (1000 << 2));
  sCalValues.NyOff = Y;
  sCalValues.NzOff = Z;
}

/*
** ===================================================================
**     Method      :  MMA1_CalibrateY1g (component MMA8451Q)
**     Description :
**         Performs a calibration of the sensor. It is assumed that the
**         X and Z sensors have 0 g, and the Y sensor has 1 g.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void MMA1_CalibrateY1g(void)
{
  /* assumption is that accelerometer is placed with 1g for Y and 0g for X and Z */
  int32_t X=0, Y=0, Z=0;
  uint8_t i;

  /* Get the raw data and remove 2 bits (16bit to 14bit) */
  for (i=0; i<8; i++) {
   X += ((int16_t)MMA1_MeasureGetRawX() >> 2);
   Y += ((int16_t)MMA1_MeasureGetRawY() >> 2);
   Z += ((int16_t)MMA1_MeasureGetRawZ() >> 2);
  }
  /* build average of 8 measured values */
  X >>= 3;
  Y >>= 3;
  Z >>= 3;
  /* store the calibration values */
  /* offset: both X and Z shall have zero g */
  sCalValues.NxOff = X;
  sCalValues.NzOff = (Y - (1000 << 2));
  sCalValues.NzOff = Z;
}

/*
** ===================================================================
**     Method      :  MMA1_CalibrateZ1g (component MMA8451Q)
**     Description :
**         Performs a calibration of the sensor. It is assumed that the
**         X and Y sensors have 0 g, and the Z sensor has 1 g.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void MMA1_CalibrateZ1g(void)
{
  /* assumption is that accelerometer is placed with 1g for Z and 0g for X and Y */
  int32_t X=0, Y=0, Z=0;
  uint8_t i;

  /* Get the raw data and remove 2 bits (16bit to 14bit) */
  for (i=0; i<8; i++) {
   X += ((int16_t)MMA1_MeasureGetRawX() >> 2);
   Y += ((int16_t)MMA1_MeasureGetRawY() >> 2);
   Z += ((int16_t)MMA1_MeasureGetRawZ() >> 2);
  }
  /* build average of 8 measured values */
  X >>= 3;
  Y >>= 3;
  Z >>= 3;
  /* store the calibration values */
  /* offset: both X and Y shall have zero g (midpoint) */
  sCalValues.NxOff = X;
  sCalValues.NyOff = Y;
  sCalValues.NzOff = (Z - (1000 << 2));
}

/*
** ===================================================================
**     Method      :  MMA1_GetXmg (component MMA8451Q)
**     Description :
**         Returns the X value in mg
**     Parameters  : None
**     Returns     :
**         ---             - The g value in 1/1000g units
** ===================================================================
*/
int16_t MMA1_GetXmg(void)
{
  return MMA1_GetX() / 4;
}

/*
** ===================================================================
**     Method      :  MMA1_GetYmg (component MMA8451Q)
**     Description :
**         Returns the Y value in mg
**     Parameters  : None
**     Returns     :
**         ---             - The g value in 1/1000g units
** ===================================================================
*/
int16_t MMA1_GetYmg(void)
{
  return MMA1_GetY() / 4;
}

/*
** ===================================================================
**     Method      :  MMA1_GetZmg (component MMA8451Q)
**     Description :
**         Returns the Z value in mg
**     Parameters  : None
**     Returns     :
**         ---             - The g value in 1/1000g units
** ===================================================================
*/
int16_t MMA1_GetZmg(void)
{
  return MMA1_GetZ() / 4;
}

/*
** ===================================================================
**     Method      :  MMA1_MeasureGetRawX (component MMA8451Q)
**     Description :
**         Performs a measurement on X channel and returns the raw
**         value.
**     Parameters  : None
**     Returns     :
**         ---             - X sensor value
** ===================================================================
*/
uint16_t MMA1_MeasureGetRawX(void)
{
  union {
    uint8_t buf[2]; /* value from device is in big endian */
    uint16_t be;
  } val;
  static const uint8_t addr = MMA1_OUT_X_MSB;

  if(GI2C1_ReadAddress(MMA1_I2C_ADDR, (uint8_t*)&addr, sizeof(addr), &val.buf[0], sizeof(val.buf))!=ERR_OK) {
    return 0; /* failure */
  }
#if MCUC1_CONFIG_CPU_IS_LITTLE_ENDIAN
  return (uint16_t)((val.buf[0]<<8)|val.buf[1]); /* transform into LE value */
#else
  return val.be; /* already in BE */
#endif
}

/*
** ===================================================================
**     Method      :  MMA1_MeasureGetRawY (component MMA8451Q)
**     Description :
**         Performs a measurement on Y channel and returns the raw
**         value.
**     Parameters  : None
**     Returns     :
**         ---             - Y sensor value
** ===================================================================
*/
uint16_t MMA1_MeasureGetRawY(void)
{
  union {
    uint8_t buf[2]; /* value from device is in big endian */
    uint16_t be;
  } val;
  static const uint8_t addr = MMA1_OUT_Y_MSB;

  if(GI2C1_ReadAddress(MMA1_I2C_ADDR, (uint8_t*)&addr, sizeof(addr), &val.buf[0], sizeof(val.buf))!=ERR_OK) {
    return 0; /* failure */
  }
#if MCUC1_CONFIG_CPU_IS_LITTLE_ENDIAN
  return (uint16_t)((val.buf[0]<<8)|val.buf[1]); /* transform into LE value */
#else
  return val.be; /* already in BE */
#endif
}

/*
** ===================================================================
**     Method      :  MMA1_MeasureGetRawZ (component MMA8451Q)
**     Description :
**         Performs a measurement on Z channel and returns the raw
**         value.
**     Parameters  : None
**     Returns     :
**         ---             - Z sensor value
** ===================================================================
*/
uint16_t MMA1_MeasureGetRawZ(void)
{
  union {
    uint8_t buf[2]; /* value from device is in big endian */
    uint16_t be;
  } val;
  static const uint8_t addr = MMA1_OUT_Z_MSB;

  if(GI2C1_ReadAddress(MMA1_I2C_ADDR, (uint8_t*)&addr, sizeof(addr), &val.buf[0], sizeof(val.buf))!=ERR_OK) {
    return 0; /* failure */
  }
#if MCUC1_CONFIG_CPU_IS_LITTLE_ENDIAN
  return (uint16_t)((val.buf[0]<<8)|val.buf[1]); /* transform into LE value */
#else
  return val.be; /* already in BE */
#endif
}

/*
** ===================================================================
**     Method      :  MMA1_GetXOffset (component MMA8451Q)
**     Description :
**         Returns the offset applied to the X value.
**     Parameters  : None
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
int16_t MMA1_GetXOffset(void)
{
  return CalNxOff;
}

/*
** ===================================================================
**     Method      :  MMA1_GetYOffset (component MMA8451Q)
**     Description :
**         Returns the offset applied to the Y value.
**     Parameters  : None
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
int16_t MMA1_GetYOffset(void)
{
  return CalNyOff;
}

/*
** ===================================================================
**     Method      :  MMA1_GetZOffset (component MMA8451Q)
**     Description :
**         Returns the offset applied to the Z value.
**     Parameters  : None
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
int16_t MMA1_GetZOffset(void)
{
  return CalNzOff;
}

/*
** ===================================================================
**     Method      :  MMA1_GetX1gValue (component MMA8451Q)
**     Description :
**         Returns the value for 1g for channel  X.
**     Parameters  : None
**     Returns     :
**         ---             - 1g value for X
** ===================================================================
*/
int16_t MMA1_GetX1gValue(void)
{
  return 4096;
}

/*
** ===================================================================
**     Method      :  MMA1_GetY1gValue (component MMA8451Q)
**     Description :
**         Returns the value for 1g for channel  Y.
**     Parameters  : None
**     Returns     :
**         ---             - 1g value for Y
** ===================================================================
*/
int16_t MMA1_GetY1gValue(void)
{
  return 4096;
}

/*
** ===================================================================
**     Method      :  MMA1_GetZ1gValue (component MMA8451Q)
**     Description :
**         Returns the value for 1g for channel  Z.
**     Parameters  : None
**     Returns     :
**         ---             - 1g value for Z
** ===================================================================
*/
int16_t MMA1_GetZ1gValue(void)
{
  return 4096;
}

/*
** ===================================================================
**     Method      :  MMA1_GetX (component MMA8451Q)
**     Description :
**         Retrieves the value for the X axis. The value is adjusted
**         with the zero calibration value (0 for 0 g, negative for
**         negative acceleration and positive for positive acceleration).
**     Parameters  : None
**     Returns     :
**         ---             - Measured X value
** ===================================================================
*/
int16_t MMA1_GetX(void)
{
  int16_t value;

  value = (int16_t)MMA1_MeasureGetRawX() >> 2; /* remove 2 bits (16bit to 14bit) */
  value -= CalNxOff; /* adjust with calibration offset */
  return value;
}

/*
** ===================================================================
**     Method      :  MMA1_GetY (component MMA8451Q)
**     Description :
**         Retrieves the value for the Y axis. The value is adjusted
**         with the zero calibration value (0 for 0 g, negative for
**         negative acceleration and positive for positive acceleration).
**     Parameters  : None
**     Returns     :
**         ---             - Measured Y value
** ===================================================================
*/
int16_t MMA1_GetY(void)
{
  int16_t value;

  value = (int16_t)MMA1_MeasureGetRawY() >> 2; /* remove 2 bits (16bit to 14bit) */
  value -= CalNyOff; /* adjust with calibration offset */
  return value;
}

/*
** ===================================================================
**     Method      :  MMA1_GetZ (component MMA8451Q)
**     Description :
**         Retrieves the value for the Z axis. The value is adjusted
**         with the zero calibration value (0 for 0 g, negative for
**         negative acceleration and positive for positive acceleration).
**     Parameters  : None
**     Returns     :
**         ---             - Measured Z value
** ===================================================================
*/
int16_t MMA1_GetZ(void)
{
  int16_t value;

  value = (int16_t)MMA1_MeasureGetRawZ() >> 2; /* remove 2 bits (16bit to 14bit) */
  value -= CalNzOff; /* adjust with calibration offset */
  return value;
}

/*
** ===================================================================
**     Method      :  MMA1_SetFastMode (component MMA8451Q)
**     Description :
**         Turns the F_READ (Fast Read Mode) on or off
**     Parameters  :
**         NAME            - DESCRIPTION
**         on              - if to turn the F_READ mode on or off
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t MMA1_SetFastMode(bool on)
{
  uint8_t val, res;

  res = GI2C1_ReadByteAddress8(MMA1_I2C_ADDR, MMA1_CTRL_REG_1, &val);
  if (res!=ERR_OK) {
    return res;
  }
  if (on) {
    val |= MMA1_F_READ_BIT_MASK; /* enable F_READ: Fast read mode, data format limited to single byte (auto increment counter will skip LSB) */
  } else {
    val &= ~MMA1_F_READ_BIT_MASK; /* disable F_READ: Fast read mode, data format limited to single byte (auto increment counter will skip LSB) */
  }
  return GI2C1_WriteByteAddress8(MMA1_I2C_ADDR, MMA1_CTRL_REG_1, val);
}

/*
** ===================================================================
**     Method      :  MMA1_Enable (component MMA8451Q)
**     Description :
**         Enables the device with setting the ACTIVE bit in the CTRL
**         register 1
**     Parameters  : None
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t MMA1_Enable(void)
{
  uint8_t val, res;

  res = GI2C1_ReadByteAddress8(MMA1_I2C_ADDR, MMA1_CTRL_REG_1, &val);
  if (res!=ERR_OK) {
    return res;
  }
  if (val&MMA1_ACTIVE_BIT_MASK) {
    return ERR_OK; /* already enabled */
  }
  WAIT1_Waitms(1); /* for unknown reasons, need to wait for 1 ms after reading the register */
  val |= MMA1_ACTIVE_BIT_MASK; /* enable device */
  return GI2C1_WriteByteAddress8(MMA1_I2C_ADDR, MMA1_CTRL_REG_1, val);
}

/*
** ===================================================================
**     Method      :  MMA1_Disable (component MMA8451Q)
**     Description :
**         Disables the device with clearing the ACTIVE bit in the CTRL
**         register 1
**     Parameters  : None
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t MMA1_Disable(void)
{
  uint8_t val, res;

  res = GI2C1_ReadByteAddress8(MMA1_I2C_ADDR, MMA1_CTRL_REG_1, &val);
  if (res!=ERR_OK) {
    return res;
  }
  if ((val&MMA1_ACTIVE_BIT_MASK)==0) {
    return ERR_OK; /* already disabled */
  }
  WAIT1_Waitms(1); /* for unknown reasons, need to wait for 1 ms after reading the register? */
  val &= ~MMA1_ACTIVE_BIT_MASK; /* disable device */
  return GI2C1_WriteByteAddress8(MMA1_I2C_ADDR, MMA1_CTRL_REG_1, val);
}

/*
** ===================================================================
**     Method      :  MMA1_isEnabled (component MMA8451Q)
**     Description :
**         Returns the status of the the ACTIVE bit in the CTRL
**         register 1
**     Parameters  :
**         NAME            - DESCRIPTION
**       * isEnabled       - Pointer to where to store the
**                           result, TRUE if ACTIVE bit is set, FALSE
**                           otherwise
**     Returns     :
**         ---             - error code
** ===================================================================
*/
uint8_t MMA1_isEnabled(bool *isEnabled)
{
  uint8_t val, res;

  res = GI2C1_ReadByteAddress8(MMA1_I2C_ADDR, MMA1_CTRL_REG_1, &val);
  if (res!=ERR_OK) {
    *isEnabled = FALSE;
    return ERR_FAILED;
  }
  *isEnabled = (val&MMA1_ACTIVE_BIT_MASK)!=0; /* TRUE if bit is set, FALSE otherwise */
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  MMA1_WhoAmI (component MMA8451Q)
**     Description :
**         Returns the value of the WHO_AM_I (0x0D) register
**     Parameters  :
**         NAME            - DESCRIPTION
**       * value           - Pointer to value to store
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t MMA1_WhoAmI(uint8_t *value)
{
  return GI2C1_ReadByteAddress8(MMA1_I2C_ADDR, MMA1_WHO_AM_I, value);
}

/*
** ===================================================================
**     Method      :  MMA1_ReadReg8 (component MMA8451Q)
**     Description :
**         Reads an 8bit device register
**     Parameters  :
**         NAME            - DESCRIPTION
**         addr            - device memory map address
**       * val             - Pointer to value
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t MMA1_ReadReg8(uint8_t addr, uint8_t *val)
{
  if (GI2C1_ReadByteAddress8(MMA1_I2C_ADDR, addr, val)!=ERR_OK) {
    return ERR_FAILED;
  }
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  MMA1_WriteReg8 (component MMA8451Q)
**     Description :
**         Write an 8bit device register
**     Parameters  :
**         NAME            - DESCRIPTION
**         addr            - device memory map address
**         val             - value to write
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t MMA1_WriteReg8(uint8_t addr, uint8_t val)
{
  return GI2C1_WriteByteAddress8(MMA1_I2C_ADDR, addr, val);
}

/*
** ===================================================================
**     Method      :  MMA1_SwReset (component MMA8451Q)
**     Description :
**         Perform a software reset using the rst bit in the CTRL
**         register 2
**     Parameters  : None
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t MMA1_SwReset(void)
{
  uint8_t val, res;

  res = GI2C1_ReadByteAddress8(MMA1_I2C_ADDR, MMA1_CTRL_REG_2, &val);
  if (res!=ERR_OK) {
    return res;
  }
  val |= MMA1_CTRL_REG_2_MASK_RST; /* reset device */
  res = GI2C1_WriteByteAddress8(MMA1_I2C_ADDR, MMA1_CTRL_REG_2, val);
  if (res!=ERR_OK) {
    return res;
  }
  WAIT1_Waitms(1); /* wait for one ms after sending the software reset */
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  MMA1_GetScaleRangeG (component MMA8451Q)
**     Description :
**         Returns the selected Full Scale Range from the XYZ_Data_CFG
**         Register
**     Parameters  :
**         NAME            - DESCRIPTION
**       * gScale          - Pointer to where to store the
**                           desired full scale g value (returns 2, 4 or
**                           8)
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t MMA1_GetScaleRangeG(uint8_t *gScale)
{
  uint8_t val, res;

  res = GI2C1_ReadByteAddress8(MMA1_I2C_ADDR, MMA1_XYZ_DATA_CFG, &val);
  if (res!=ERR_OK) {
    return res;
  }
  switch(val&0x3) { /* check FS1 and FS2 bits */
    case 0: *gScale = 2; /* 00: 2g */
      return ERR_OK;
    case 1: *gScale = 4; /* 01: 4g */
      return ERR_OK;
    case 2: *gScale = 8; /* 10: 8g */
      return ERR_OK;
    default: break;
  } /* switch */
  *gScale = 0; /* error case */
  return ERR_RANGE;
}

/*
** ===================================================================
**     Method      :  MMA1_SetScaleRangeG (component MMA8451Q)
**     Description :
**         Sets the desired full scale range G value (2, 4 or 6) using
**         the XYZ_DATA_CFG register. Note that the device has to be
**         disabled.
**     Parameters  :
**         NAME            - DESCRIPTION
**         gScale          - desired g value, has to be 2, 4 or 8
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t MMA1_SetScaleRangeG(uint8_t gScale)
{
  uint8_t val, res;

  res = GI2C1_ReadByteAddress8(MMA1_I2C_ADDR, MMA1_XYZ_DATA_CFG, &val);
  if (res!=ERR_OK) {
    return res;
  }
  switch(gScale) { /* set FS1 and FS0 bits */
    case 2:
        val &= ~0x3; /* 00: 2g */
        break;
    case 4:
        val &= ~0x3; /* clear bits */
        val |= 0x1; /* 00: 4g */
        break;
    case 8:
        val &= ~0x3; /* clear bits */
        val |= 0x2; /* 10: 8g */
        break;
    default:
        return ERR_FAILED;
  } /* switch */
  return GI2C1_WriteByteAddress8(MMA1_I2C_ADDR, MMA1_XYZ_DATA_CFG, val);
}

/* END MMA1. */

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
