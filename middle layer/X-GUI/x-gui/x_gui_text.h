
/*----------------------------------------------------------------------^^-
/ File name:  x_gui_text.h
/ Author:     JiangJun
/ Data:       2018/8/19
/ Version:    v1.1
/-----------------------------------------------------------------------^^-
/ X-GUI Text Driver
/------------------------------------------------------------------------*/

#ifndef _X_GUI_TEXT_H
#define _X_GUI_TEXT_H

//------------------------------------------------------------
//              Header
//------------------------------------------------------------

#include "lcd_driver.h"
#include "font_lib.h"

//------------------------------------------------------------
//              CONFIG
//------------------------------------------------------------

#define _TEXT_OUT_DEFAULT_SIZE              _FONT_SIZE_0  

#define _TEXT_OUT_ALIGN_X_SIDE_DEFAULT      3           // piexls
#define _TEXT_OUT_ALIGN_Y_SIDE_DEFAULT      3       
#define _TEXT_OUT_ALIGN_DEF_ACTION          0x02        // _TEXT_LEFT_ALIGN


//------------------------------------------------------------
//              Function Macros
//------------------------------------------------------------

// 'TextOutExAlign()' Function Parameter
#define _TEXT_REV_DIS           0x01
#define _TEXT_LEFT_ALIGN        0x02
#define _TEXT_RIGHT_ALIGN       0x04        // Rev (N/A)
#define _TEXT_MIDDLE_ALIGN      0x08
#define _TEXT_NO_RETNLE         0x10

extern gu16 TextGetX_MaxSize(const gu8 *text);
extern gu16 TextGetY_MaxSize(const gu8 *text);
extern void TextSizeSet(gu8 size);
extern void TextOut(gu16 x_start, gu16 y_start, const gu8 *text);
extern void TextOutExRev(gu16 x_start, gu16 y_start, const gu8 *text, gu8 rev);
extern void TextOutExRetnle(xCursorT *ulr, const gu8 *text, gu8 x_side, gu8 y_side, gu8 rev);
extern void TextOutExRndFrame(xCursorT *ulr, const gu8 *text, gu8 x_side, gu8 y_side, gu8 r, gu8 rev);
extern void TextOutExAlign(xCursorT *ulr, const gu8 *text, gu16 x_len, gu8 y_side_offset, gu8 func);

#endif

