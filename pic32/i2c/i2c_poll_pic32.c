
/*----------------------------------------------------------------------^^-
/ File name:  i2c.c
/ Author:     JiangJun
/ Data:       [2020-12-6]
/ Version:    V1.01
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
/                     3. I2cDelayUs() delay 1us.
/ you should modify the above three functions to be suitable for i2c time 
/ characteristic.
/ ------
/ PIC32 serial code.(48MHz)
/ ------
/ [ TEST ] - about 200KHz
/ ------
/ v1.01 [2020-12-6]
/ 1. I2cDelayUs() to common function
/------------------------------------------------------------------------*/


#include "i2c_poll_pic32.h"
#include "hw_conf.h"


/*----------------------------------------------------------------------
 *  EnsureSclHigh - 100us overtime
 *
 *  Purpose: SCL(Low to High) clock sync.
 *  Entry:   None.
 *  Exit:    TRUE(ok) or FALSE(fail).
 *
 *  NOTE:    TEST: 169.56us
 *---------------------------------------------------------------------*/
BOOL_t EnsureSclHigh(void)
{
    volatile u16 idx = 0;

    idx = 300;
    while ((idx != 0) && (I2C_SCL_INPUT() == GPIO_LOW_LEVEL))
    {
        idx--;
    }

    return ((idx != 0)? TRUE: FALSE);
}

/*----------------------------------------------------------------------
 *  EnsureSclLow - 300ns overtime
 *
 *  Purpose: SCL(High to Low) clock sync.
 *  Entry:   None.
 *  Exit:    TRUE(ok) or FALSE(fail).
 *
 *  NOTE:    TEST: 1us
 *---------------------------------------------------------------------*/
BOOL_t EnsureSclLow(void)
{
    volatile u16 idx = 0;

    idx = 1;
    while ((idx != 0) && (I2C_SCL_INPUT() == GPIO_HIGH_LEVEL))
    {
        idx--;
    }

    return ((idx != 0)? TRUE: FALSE);
}

/*----------------------------------------------------------------------
 *  I2cDelayUs
 *
 *  Purpose: Software delay.
 *  Entry:   us - the count of us to delay.
 *  Exit:    None.
 *
 *  NOTE:       [ TEST: ]
 *              us          time
 *              1           1us
 *              5           5us
 *              50          46.3125us
 *              200         183.8us
 *---------------------------------------------------------------------*/
void I2cDelayUs(u8 us)
{
    while (us--)
    {
        asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop");
        asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop");
        asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop");
        asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop");
        asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop");
        asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop");
        asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop");
        asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop");
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
    I2C_SCL_OUT_LOW();
    EnsureSclLow();

    I2C_SDA_OUT_HIGH();
    // Delay: Tlow
    // Tlow: standard mode: >= 4.7us; fast mode: >= 1.3us.
    I2cDelayUs(_I2C_SPD_DELAY);
    
    I2C_SCL_OUT_HIGH();
    EnsureSclHigh();
    // Delay: Tsu.sta
    // Tsu.sta: standard mode: >= 4.7us; fast mode: >= 0.6us.
    I2cDelayUs(_I2C_SPD_DELAY);

    I2C_SDA_OUT_LOW();
    // Delay: Tf + Thd.sta
    // Tf: standard mode: <= 0.3us; fast mode: <= 0.3us.
    // Thd.sta: standard mode: >= 4.0us; fast mode: >= 0.6us.
    I2cDelayUs(_I2C_SPD_DELAY);
    
    // lock the SCL line.
    I2C_SCL_OUT_LOW();
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
    I2C_SDA_OUT_LOW();

    // Delay: Tlow
    // Tlow: standard mode: >= 4.7us; fast mode: >= 1.3us.
    I2cDelayUs(_I2C_SPD_DELAY);

    I2C_SCL_OUT_HIGH();
    EnsureSclHigh();
    // Delay: Tsu.sto
    // Tsu.sto: standard mode: >= 4.0us; fast mode: >= 0.6us.
    I2cDelayUs(_I2C_SPD_DELAY);

    I2C_SDA_OUT_HIGH();
    // Delay: Tr + Tbuf
    // Tf: standard mode: <= 0.3us; fast mode: <= 0.3us.
    // Tbuf: standard mode: >= 4.7us; fast mode: >= 0.6us.
    I2cDelayUs(_I2C_SPD_DELAY);
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
BOOL_t I2cTransmitByte(u8 tran_byt)
{
    u8 idx;
    
    // transmit 8-bits, MSB first.
    for(idx = 0; idx < 8; idx++)
    {
        if((tran_byt << idx) & 0x80){
            I2C_SDA_OUT_HIGH();
        }
        else {
            I2C_SDA_OUT_LOW();
        }
        // Delay: Tlow
        // Tlow: standard mode: >= 4.7us; fast mode: >= 1.3us.
        I2cDelayUs(_I2C_SPD_DELAY);

        I2C_SCL_OUT_HIGH();
        if (EnsureSclHigh() != TRUE) {
            return FALSE;
        }
        // Delay: Thigh
        // Thight: standard mode: >= 4.0us; fast mode: >= 0.6us.
        I2cDelayUs(_I2C_SPD_DELAY);

        I2C_SCL_OUT_LOW();
        if (EnsureSclLow() != TRUE) {
            return FALSE;
        }
    }

    // Receice 1-bit acknowledge.
    I2C_SDA_OUT_HIGH();

    // Delay: Tlow
    // Tlow: standard mode: >= 4.7us; fast mode: >= 1.3us.
    I2cDelayUs(_I2C_SPD_DELAY);

    I2C_SCL_OUT_HIGH();
    if (EnsureSclHigh() != TRUE) {
        return FALSE;
    }
    // Delay: Thigh
    // Thight: standard mode: >= 4.0us; fast mode: >= 0.6us.
    I2cDelayUs(_I2C_SPD_DELAY);

    // read SDA line.
    idx = !I2C_SDA_INPUT();

    I2C_SCL_OUT_LOW();
    if (EnsureSclLow() != TRUE) {
        return FALSE;
    }

    return (BOOL_t)idx;
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
BOOL_t I2cReceiveByte(u8 *rece_byt, u8 ack_nack)
{
    u8 byt, idx;

    I2C_SDA_OUT_HIGH();

    // Receive 8-bits.
    for (idx = 0, byt = 0; idx < 8; idx++)
    {
        // Delay: Tlow
        // Tlow: standard mode: >= 4.7us; fast mode: >= 1.3us.
        I2cDelayUs(_I2C_SPD_DELAY);

        I2C_SCL_OUT_HIGH();
        if (EnsureSclHigh() != TRUE) {   
            return FALSE;
        }
        // Delay: Thigh
        // Thight: standard mode: >= 4.0us; fast mode: >= 0.6us.
        I2cDelayUs(_I2C_SPD_DELAY);

        byt = (byt << 1) | I2C_SDA_INPUT();

        I2C_SCL_OUT_LOW();
        if (EnsureSclLow() != TRUE) {
            return FALSE;
        }
    }

    // Transmit 1-bit acknowledge.
    if (ack_nack) I2C_SDA_OUT_LOW();
    else I2C_SDA_OUT_HIGH();

    // Delay: Tlow
    // Tlow: standard mode: >= 4.7us; fast mode: >= 1.3us.
    I2cDelayUs(_I2C_SPD_DELAY);

    I2C_SCL_OUT_HIGH();
    if (EnsureSclHigh() != TRUE) {
        return FALSE;
    }
    // Delay: Thigh
    // Thight: standard mode: >= 4.0us; fast mode: >= 0.6us.
    I2cDelayUs(_I2C_SPD_DELAY);

    I2C_SCL_OUT_LOW();
    if (EnsureSclLow() != TRUE) {
        return FALSE;
    }

    *rece_byt = byt;
    return TRUE;
}

//----------------------------------------------------------------------
//------------------------ END OF FILE ---------------------------------
//----------------------------------------------------------------------
