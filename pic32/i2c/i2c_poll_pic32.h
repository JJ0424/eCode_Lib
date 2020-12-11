
/*----------------------------------------------------------------------^^-
/ File name:  i2c.h
/ Author:     JiangJun
/ Data:       [2020-12-6]
/ Version:    V1.01
/-----------------------------------------------------------------------^^-
/ Software I2C lib.
/------------------------------------------------------------------------*/

#ifndef _I2C_H
#define _I2C_H


#include "main.h"


//------------------------------------------------------------
//              CONFIG
//------------------------------------------------------------

#define GPIO_LOW_LEVEL          0
#define GPIO_HIGH_LEVEL         1
#define _I2C_SPD_DELAY          1


//------------------------------------------------------------
//              EXTERN
//------------------------------------------------------------

extern void I2cStartCondition(void);
extern void I2cStopCondition(void);
extern Bool I2cTransmitByte(u8);
extern Bool I2cReceiveByte(u8 *, u8);
extern void I2cDelayUs(u8 us);

#endif
//----------------------------------------------------------------------
//------------------------ END OF FILE ---------------------------------
//----------------------------------------------------------------------