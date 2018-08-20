
/*----------------------------------------------------------------------^^-
/ File name:  st75256.h
/ Author:     JiangJun
/ Data:       2018/2/9
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ st75256 driver
/------------------------------------------------------------------------*/

#ifndef _ST75256_H
#define _ST75256_H

#include "main.h"

//------------------------------------------------------------
// DRIVER CONFIG
//------------------------------------------------------------

// 4G/Mono Mode
#define _ST75256_ENABLE_GREY            1

// Solutions
#define _ST75256_LCD_X_SIZE             240
#define _ST75256_LCD_Y_SIZE             160

// VOP Driver Voltage
#define _ST75256_VOP_DEF_VTG            17.0f

// COMSCAN 
#define _ST75256_COMSCAN                0

// Gray Setting
#if _ST75256_ENABLE_GREY

  // [ Gray 1 ]
  #define _ST75256_LIGHT_GRAY           16

  // [ Gray 2 ]
  #define _ST75256_DARK_GRAY            22
  
#endif /* _ST75256_ENABLE_GREY */

// V/H Flip
#define _ST75256_MX_ENABLE              0
#define _ST75256_MY_ENABLE              0

extern void St75256Init();
extern void St75256ClrScreen(void);
extern void St75256WriteFrame(u8 *dat, u32 len);

#endif

//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
