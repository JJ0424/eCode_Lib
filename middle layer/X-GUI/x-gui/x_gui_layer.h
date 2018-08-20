
/*----------------------------------------------------------------------^^-
/ File name:  x_gui_layer.h
/ Author:     JiangJun
/ Data:       2018/8/19
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ X-GUI Layer Driver
/------------------------------------------------------------------------*/

#ifndef _X_GUI_LAYER_H
#define _X_GUI_LAYER_H

#include "lcd_driver.h"

//------------------------------------------------------------
//                          BMP
//------------------------------------------------------------

typedef struct {

    // Width
    gu16 x_size;

    // Height
    gu16 y_size;

    // Bmp Mask
    const gu8 *mask;
    
} _LayerBmpT;

extern void LayerBmpOut(gu16 x, gu16 y, _LayerBmpT *bmp, gu8 rev);
extern gu16 LayerBmpRead(xCursorT *ulc, xCursorT *lrc, gu8 *p_col, gu16 buff_size);
extern void LayerRender(xCursorT *ulc, xCursorT *lrc, gu8 col, gu8 rev);
extern void LayerRenderScreen(gu8 col, gu8 rev);
extern void LayerClrScreen(gu8 col);
extern void LayerRevScreen(gu8 rev);
extern void LayerClr(gu16 xs, gu16 ys, gu16 xe, gu16 ye, gu8 col);
extern void LayerRev(gu16 xs, gu16 ys, gu16 xe, gu16 ye, gu8 rev);

#endif

//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
