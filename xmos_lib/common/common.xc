
/*----------------------------------------------------------------------^^-
/ File name:  common.xc
/ Author:     JiangJun
/ Data:       2019/2/13
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ xCore-200 common funtioncs
/------------------------------------------------------------------------*/


#include "common.h"

/*----------------------------------------------------------------------
 *  timer_delay_ms
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u32 timer_delay_ms(u32 tick)
{
    timer tmr; u32 tmr_val = 0;
    
    tmr :> tmr_val;     /* read timer value */
    tmr when timerafter(tmr_val + ((_XCORE_REF_CLOCK_RATE / 1000) * tick)) :> tmr_val;
    return tmr_val;     /* return the timer tick */
}

/*----------------------------------------------------------------------
 *  timer_delay_us
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u32 timer_delay_us(u32 tick)
{
    timer tmr; u32 tmr_val = 0;
    
    tmr :> tmr_val;     /* read timer value */
    tmr when timerafter(tmr_val + ((_XCORE_REF_CLOCK_RATE / 1000000U) * tick)) :> tmr_val;
    return tmr_val;     /* return the timer tick */
}

/*----------------------------------------------------------------------
 *  timer_delay_s
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u32 timer_delay_s(u32 tick)
{
    timer tmr; u32 tmr_val = 0;
    
    tmr :> tmr_val;     /* read timer value */
    tmr when timerafter(tmr_val + (_XCORE_REF_CLOCK_RATE * tick)) :> tmr_val;
    return tmr_val;     /* return the timer tick */
}

/*----------------------------------------------------------------------
 *  timer_delay_tick
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u32 timer_delay_tick(u32 tick)
{
    timer tmr; u32 tmr_val = 0;
    
    tmr :> tmr_val;     /* read timer value */
    tmr when timerafter(tmr_val + tick) :> tmr_val;
    return tmr_val;     /* return the timer tick */
}

//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
