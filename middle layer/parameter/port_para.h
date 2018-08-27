
/*----------------------------------------------------------------------^^-
/ File name:  port_para.h
/ Author:     JiangJun
/ Data:       2018/3/29
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ R/W Flash driver
/------------------------------------------------------------------------*/

#ifndef _PORT_PARA_H
#define _PORT_PARA_H

#include "main.h"

//------------------------------------------------------------
//              FLASH CONFIG
//------------------------------------------------------------

#define _FLASH_PAGE_SIZE                    ((u32)0x00000400)

// FLASH Save Address
#define _PARA_SAVE_ADDRESS                  (1024 * 31)

extern u16 portReadBytes(u8 *dst, u16 cnt);
extern u16 portWriteBytes(u8 *src, u16 cnt);

#endif
