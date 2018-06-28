
/*----------------------------------------------------------------------^^-
/ File name:  common.c
/ Author:     JiangJun
/ Data:       2018-6-28
/ Version:    v1.5
/-----------------------------------------------------------------------^^-
/ Common Functions
/ ---
/ v1.1 2017/05/27
/ [1] Add LoopDelay()
/ ---
/ v1.2 2017/08/21
/ [1] Add STM8S005(16MHz) Test
/ ---
/ v1.3 2017/10/24
/ [1] XorCheckSum(u8 *input, u16 length) to XorCheckSum(u8 *input, u32 length)
/ ---
/ v1.4 [2018-6-21]
/ [1] Add uicvt_format()
/ ---
/ v1.5 [2018-6-28]
/ [1] Add fcvt_format_bit()/ dcvt_format_bit()
/------------------------------------------------------------------------*/

#include "main.h"
#include "common.h"

/*----------------------------------------------------------------------
 *  XorCheckSum
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u8 XorCheckSum(u8 *input, u32 length)
{
    u32 idx = 0; u8 xorResult = 0;
    
    // !! Initialize the reseut with the 'array[0]' !!
    xorResult = input[0];
    
    // X-OR Loop check
    for (idx = 1; idx < length; idx++)
    {        
        xorResult ^= input[idx];
    }
    
    return xorResult;
}

/*----------------------------------------------------------------------
 *  LoopDelay
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    
 *           Platform Test Result:
 *              [1] Stm32f031 (48MHz)
 *                  1. 200 cnt - 250ms
 *                  2. 10 cnt - 12.5ms
 *                  3. 1 cnt - 1.25ms
 *              [2] Stm8s005 (16MHz)
 *                  1. 200 cnt - 688ms
 *                  2. 10 cnt - 34.4ms
 *                  3. 1 cnt - 3.44ms
 *---------------------------------------------------------------------*/
void LoopDelay(u16 cnt)
{

    volatile u16 idx = 0;
    
    // For Loop Delay
    for (; cnt != 0; cnt--)
    {
        idx = 5000;
        for (; idx--; );    
    }
}

/*----------------------------------------------------------------------
 *  uicvt_format_bit - convert unsigned int(u32) to 'bit width' format
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void uicvt_format_bit(u32 val, u8 bit_width, u8 *out, u16 out_size)
{

    u8 idx = 0;
    
    // Check Range
    if (out_size <= bit_width)
    {
        return;
    }

    // Number String
    for (idx = 0; idx < bit_width; idx++)
    {
    
        out[bit_width - idx - 1] = (val % 10) + 0x30;
        val /= 10;
    }

    // '\0' Ending Char
    out[bit_width] = '\0';
}

/*----------------------------------------------------------------------
 *  fcvt_format_bit
 *
 *  Purpose: None.
 *  Entry:   None.
 *
 *  Exit:    The Sign of the value
 *
 *  NOTE:    format: 
 *                      "%2.3f" -   00.000
 *                      "%5.0f" -   00000.
 *                      "%0.6f" -   .000000
 *                      "%0.0f" -   .
 *---------------------------------------------------------------------*/
u8 fcvt_format_bit(float val, const u8 *format, u8 *out, u16 out_size)
{

    u8 i_width = 0, p_width = 0, sign = 0; float val_tmp = val;
    u8 idx = 0;

    // Get Sign
    if (val_tmp < 0)
    {
        val_tmp = -val_tmp;
        sign = 1;
    } 
    else { sign = 0; }
    
    // Get inter and point Width
    if ((format[0] != '%') || (format[2] != '.')) { return sign; }
    i_width = format[1] - 0x30; p_width = format[3] - 0x30;

    // Check Range
    if (out_size <= (i_width + p_width + 1)) { return sign; }
    
    // To String
    uicvt_format_bit((u32)val_tmp, i_width, out, out_size); out[i_width] = '.';

    val_tmp = val_tmp - (u32)val_tmp;
    for (idx = 0; idx < p_width; idx++)
    {
        val_tmp *= 10.0f;
    }
    uicvt_format_bit((u32)val_tmp, p_width, out + i_width + 1, out_size - i_width - 1);

    // Return Sign
    return sign;
}