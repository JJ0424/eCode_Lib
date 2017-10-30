/*----------------------------------------------------------------------^^-
/ File name:  i2c.c
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
/ ------
/ LPC8xx serial code.(NXP-30MHz)
/------------------------------------------------------------------------*/

#include "i2c.h"

#include "hconfig.h"

/*----------------------------------------------------------------------
 *  EnsureSclHigh
 *
 *  Purpose: SCL(Low to High) clock sync.
 *  Entry:   None.
 *  Exit:    TRUE(ok) or FALSE(fail).
 *  NOTE:    the overtime is 100us, so you should modify the while loop.
 *---------------------------------------------------------------------*/
Bool EnsureSclHigh(void)
{
    volatile u16 idx = 0;

    idx = 1000;
    while ((idx != 0) && (I2C_SCL_INPUT() == GPIO_LOW_LEVEL))
    {
        idx--;
    }

    return ((idx != 0)? TRUE: FALSE);
}

/*----------------------------------------------------------------------
 *  EnsureSclLow
 *
 *  Purpose: SCL(High to Low) clock sync.
 *  Entry:   None.
 *  Exit:    TRUE(ok) or FALSE(fail).
 *  NOTE:    the overtime is 300ns, so you should modify the while loop.
 *---------------------------------------------------------------------*/
Bool EnsureSclLow(void)
{
    volatile u16 idx = 0;

    idx = 5;
    while ((idx != 0) && (I2C_SCL_INPUT() == GPIO_HIGH_LEVEL))
    {
        idx--;
    }

    return ((idx != 0)? TRUE: FALSE);
}

/*----------------------------------------------------------------------
 *  I2cDelayMicroSecs
 *
 *  Purpose: Software delay.
 *  Entry:   us - the count of us to delay.
 *  Exit:    None.
 *  NOTE:    the delay time is 1us, so you should modify the while loop.
 *---------------------------------------------------------------------*/
void I2cDelayMicroSecs(u8 us)
{
    volatile u16 idx = 0;  
    
    while(us--)
    {
        idx = 8;
        while(idx--);    
    }
}

/*----------------------------------------------------------------------
 *  I2cStartCondition
 *
 *  Purpose: Send I2C start condition.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void I2cStartCondition(void)
{   
    // Initialize the SCL.
    I2C_SCL_OUTPUT(GPIO_LOW_LEVEL);
    EnsureSclLow();

    I2C_SDA_OUTPUT(GPIO_HIGH_LEVEL);
    // Delay: Tlow
    // Tlow: standard mode: >= 4.7us; fast mode: >= 1.3us.
    I2cDelayMicroSecs(5);
    
    I2C_SCL_OUTPUT(GPIO_HIGH_LEVEL);
    EnsureSclHigh();
    // Delay: Tsu.sta
    // Tsu.sta: standard mode: >= 4.7us; fast mode: >= 0.6us.
    I2cDelayMicroSecs(5);

    I2C_SDA_OUTPUT(GPIO_LOW_LEVEL);
    // Delay: Tf + Thd.sta
    // Tf: standard mode: <= 0.3us; fast mode: <= 0.3us.
    // Thd.sta: standard mode: >= 4.0us; fast mode: >= 0.6us.
    I2cDelayMicroSecs(5);
    
    // lock the SCL line.
    I2C_SCL_OUTPUT(GPIO_LOW_LEVEL);
    EnsureSclLow();
}

/*----------------------------------------------------------------------
 *  I2cStopCondition
 *
 *  Purpose: Send I2C stop condition.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void I2cStopCondition(void)
{
    I2C_SDA_OUTPUT(GPIO_LOW_LEVEL);

    // Delay: Tlow
    // Tlow: standard mode: >= 4.7us; fast mode: >= 1.3us.
    I2cDelayMicroSecs(5);

    I2C_SCL_OUTPUT(GPIO_HIGH_LEVEL);
    EnsureSclHigh();
    // Delay: Tsu.sto
    // Tsu.sto: standard mode: >= 4.0us; fast mode: >= 0.6us.
    I2cDelayMicroSecs(5);

    I2C_SDA_OUTPUT(GPIO_HIGH_LEVEL);
    // Delay: Tr + Tbuf
    // Tf: standard mode: <= 0.3us; fast mode: <= 0.3us.
    // Tbuf: standard mode: >= 4.7us; fast mode: >= 0.6us.
    I2cDelayMicroSecs(5);
}

/*----------------------------------------------------------------------
 *  I2cTransmitByte
 *
 *  Purpose: Send one byte to salver.
 *
 *  Entry:   tran_byt - the data to send.
 *  Exit:    TRUE - master receive with acknowledge.
 *           FALSE = master receive with no acknowledge.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
Bool I2cTransmitByte(u8 tran_byt)
{
    u8 idx;
    
    // transmit 8-bits, MSB first.
    for(idx = 0; idx < 8; idx++)
    {
        if((tran_byt << idx) & 0x80){
            I2C_SDA_OUTPUT(GPIO_HIGH_LEVEL);
        }
        else {
            I2C_SDA_OUTPUT(GPIO_LOW_LEVEL);
        }
        // Delay: Tlow
        // Tlow: standard mode: >= 4.7us; fast mode: >= 1.3us.
        I2cDelayMicroSecs(5);

        I2C_SCL_OUTPUT(GPIO_HIGH_LEVEL);
        if (EnsureSclHigh() != TRUE) {
            return FALSE;
        }
        // Delay: Thigh
        // Thight: standard mode: >= 4.0us; fast mode: >= 0.6us.
        I2cDelayMicroSecs(5);

        I2C_SCL_OUTPUT(GPIO_LOW_LEVEL);
        if (EnsureSclLow() != TRUE) {
            return FALSE;
        }
    }

    // Receice 1-bit acknowledge.
    I2C_SDA_OUTPUT(GPIO_HIGH_LEVEL);

    // Delay: Tlow
    // Tlow: standard mode: >= 4.7us; fast mode: >= 1.3us.
    I2cDelayMicroSecs(5);

    I2C_SCL_OUTPUT(GPIO_HIGH_LEVEL);
    if (EnsureSclHigh() != TRUE) {
        return FALSE;
    }
    // Delay: Thigh
    // Thight: standard mode: >= 4.0us; fast mode: >= 0.6us.
    I2cDelayMicroSecs(5);

    // read SDA line.
    idx = !I2C_SDA_INPUT();

    I2C_SCL_OUTPUT(GPIO_LOW_LEVEL);
    if (EnsureSclLow() != TRUE) {
        return FALSE;
    }

    return (Bool)idx;
}

/*----------------------------------------------------------------------
 *  I2cReceiveByte
 *
 *  Purpose: Receive one from salver.
 *          
 *  Entry:   rece_byt - the point to receive space.
 *           ack_nack - 1: send 9th with acknowledge.
 *                      0: send 9th with no acknowledge.
 *  Exit:    TRUE - OK, FALSE - fail.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
Bool I2cReceiveByte(u8 *rece_byt, u8 ack_nack)
{
    u8 byt, idx;

    I2C_SDA_OUTPUT(GPIO_HIGH_LEVEL);

    // Receive 8-bits.
    for (idx = 0, byt = 0; idx < 8; idx++)
    {
        // Delay: Tlow
        // Tlow: standard mode: >= 4.7us; fast mode: >= 1.3us.
        I2cDelayMicroSecs(5);

        I2C_SCL_OUTPUT(GPIO_HIGH_LEVEL);
        if (EnsureSclHigh() != TRUE) {   
            return FALSE;
        }
        // Delay: Thigh
        // Thight: standard mode: >= 4.0us; fast mode: >= 0.6us.
        I2cDelayMicroSecs(5);

        byt = (byt << 1) | I2C_SDA_INPUT();

        I2C_SCL_OUTPUT(GPIO_LOW_LEVEL);
        if (EnsureSclLow() != TRUE) {
            return FALSE;
        }
    }

    // Transmit 1-bit acknowledge.
    I2C_SDA_OUTPUT(!ack_nack);

    // Delay: Tlow
    // Tlow: standard mode: >= 4.7us; fast mode: >= 1.3us.
    I2cDelayMicroSecs(5);

    I2C_SCL_OUTPUT(GPIO_HIGH_LEVEL);
    if (EnsureSclHigh() != TRUE) {
        return FALSE;
    }
    // Delay: Thigh
    // Thight: standard mode: >= 4.0us; fast mode: >= 0.6us.
    I2cDelayMicroSecs(5);

    I2C_SCL_OUTPUT(GPIO_LOW_LEVEL);
    if (EnsureSclLow() != TRUE) {
        return FALSE;
    }

    *rece_byt = byt;
    return TRUE;
}


//----------------------------------------------------------------------
//------------------------ END OF FILE ---------------------------------
//----------------------------------------------------------------------
