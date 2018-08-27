
/*----------------------------------------------------------------------^^-
/ File name:  port_para.c
/ Author:     JiangJun
/ Data:       2018/3/29
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ R/W Flash driver
/------------------------------------------------------------------------*/

#include "port_para.h"

/*----------------------------------------------------------------------
 *  portReadBytes
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u16 portReadBytes(u8 *dst, u16 cnt)
{

    u16 idx = 0; u8 *src = (u8 *)_PARA_SAVE_ADDRESS;

    //------------------------------------------------------------
    //              Read From FLASH
    //------------------------------------------------------------
    
    for (idx = 0; idx < cnt; idx++, dst++, src++)
    {
        *dst = *src;
    }

    return idx;
}

/*----------------------------------------------------------------------
 *  portWriteBytes
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u16 portWriteBytes(u8 *src, u16 cnt)
{

    u16 idx = 0;    

    //------------------------------------------------------------
    //              Write to FLASH
    //------------------------------------------------------------

    /* Unlock the Flash to enable the flash control register access *************/ 
    FLASH_Unlock();

    /* Clear pending flags (if any) */  
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 

    /* Erase the FLASH pages */
    FLASH_ErasePage(_PARA_SAVE_ADDRESS);

    // Write bytes
    for (idx = 0; idx < cnt; idx += 2)
    {
        if (FLASH_ProgramHalfWord(_PARA_SAVE_ADDRESS + idx, *((u16 *)((u32)src + idx))) != FLASH_COMPLETE)
        {
            return 0;
        }
    }

    /* Lock the Flash to disable the flash control register access (recommended
       to protect the FLASH memory against possible unwanted operation) *********/
    FLASH_Lock(); 
    
    return cnt;
}

