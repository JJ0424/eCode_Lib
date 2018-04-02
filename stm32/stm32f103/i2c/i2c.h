/*----------------------------------------------------------------------^^-
/ File name:  i2c.h
/ Author:     JiangJun
/ Data:       2017/10/30
/ Version:    V1.1
/-----------------------------------------------------------------------^^-
/ Software I2C lib.
/ ------
/ Pin configuaration: 
/                     SDA - open drain or open source output.
/                     SCL - open drain or open source output.
/ ------
/ Port NOTE:
/                     1. EnsureSclHigh() 100us over time.
/                     2. EnsureSclLow() 300ns over time.
/                     3. I2cDelayMicroSecs() delay 1us.
/ you should modify the above three functions to be suitable for i2c time 
/ characteristic.
/------------------------------------------------------------------------*/
#ifndef _I2C_H
#define _I2C_H

#include "main.h"

#define GPIO_LOW_LEVEL      0
#define GPIO_HIGH_LEVEL     1


void I2cStartCondition(void);
void I2cStopCondition(void);
Bool I2cTransmitByte(u8);
Bool I2cReceiveByte(u8 *, u8);

#endif


//----------------------------------------------------------------------
//------------------------ END OF FILE ---------------------------------
//----------------------------------------------------------------------
