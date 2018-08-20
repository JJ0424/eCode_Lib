
/*----------------------------------------------------------------------^^-
/ File name:  x_gui_com.h
/ Author:     JiangJun
/ Data:       2018/6/12
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ X-GUI Common Functions
/------------------------------------------------------------------------*/

#ifndef _X_GUI_COM_H
#define _X_GUI_COM_H

#include "x_gui.h"

extern void XProgress_Fill(gu16 x, gu16 y, gu16 x_size, gu16 y_size, gu8 percent);
extern void XProgress_Line(gu16 x, gu16 y, gu16 x_max_size, gu16 y_size, gu8 percent);
extern void XProgress_Fill_Empty(gu16 x, gu16 y, gu16 x_size, gu16 y_size, gu8 percent);
extern void XProgress_Fill_Flash(gu16 x, gu16 y, gu16 x_size, gu16 y_size, gu8 percent);
extern void XProgress_Fill_R_Tile(gu16 x, gu16 y, gu16 x_size, gu16 y_size, gu8 percent);
extern void XProgress_Fill_L_Tile(gu16 x, gu16 y, gu16 x_size, gu16 y_size, gu8 percent);
extern void XScrollBar_Fill(gu16 x, gu16 y, gu16 x_size, gu16 y_size, gu8 hor_vert, gu8 percent);
extern void XPanel_TextMid(xCursorT *ulr, xCursorT *lrc, const gu8 *text, gu8 y_side_offset);

#endif
//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
