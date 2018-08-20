
/*----------------------------------------------------------------------^^-
/ File name:  x_gui_2d.h
/ Author:     JiangJun
/ Data:       2018/8/19
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ X-GUI 2D Draw Driver
/------------------------------------------------------------------------*/

#ifndef _X_GUI_2D_H
#define _X_GUI_2D_H

// Port File
#include "lcd_driver.h"

//------------------------------------------------------------
//              Ext Functions
//------------------------------------------------------------

#define FigDrawPixel(x, y, col) pLcdSetPixel(x, y, col)
#define FigFillRectangle(left_top, right_bottom, col) pLcdFillPixels(left_top, right_bottom, col)

void FigDrawLine(xCursorT *begin, xCursorT *end, gu8 col);
void FigDrawCircle(gu16 x, gu16 y, gu16 r, gu8 col);
void FigFillCircle(gu16 x_coor, gu16 y_coor, gu16 r, gu8 col);
void FigDrawRectangle(xCursorT *left_top, xCursorT *right_bottom, gu8 Col);
void FigDrawPolygon(xCursorT *pnt, gu8 cnt, gu8 col);
void FigFillPolygon(xCursorT *pnt, gu8 cnt, gu8 col);
void FigDrawXLine(gu16 xs, gu16 xe, gu16 y, gu8 col);
void FigDrawYLine(gu16 ys, gu16 ye, gu16 x, gu8 col);
void FigDrawXDotLine(gu16 xs, gu16 xe, gu16 y, gu8 col, gu8 dot_pixs, gu8 emy_pixs);
void FigDrawYDotLine(gu16 ys, gu16 ye, gu16 x, gu8 col, gu8 dot_pixs, gu8 emy_pixs);
void FigDrawRoundFrame(xCursorT *left_top, xCursorT *right_bottom, gu16 r, gu8 col);
void FigFillRoundFrame(xCursorT *left_top, xCursorT *right_bottom, gu16 r, gu8 col);

#endif
//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
