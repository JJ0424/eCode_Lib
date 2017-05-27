
/*----------------------------------------------------------------------^^-
/ File name:  common.c
/ Author:     JiangJun
/ Data:       2017/05/27
/ Version:    v1.1
/-----------------------------------------------------------------------^^-
/ Common Functions
/ ---
/ v1.1 2017/05/27
/ [1] Add LoopDelay()
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
u8 XorCheckSum(u8 *input, u16 length)
{
    u16 idx = 0; u8 xorResult = 0;
    
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