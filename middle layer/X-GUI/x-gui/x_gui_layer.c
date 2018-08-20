
/*----------------------------------------------------------------------^^-
/ File name:  x_gui_layer.c
/ Author:     JiangJun
/ Data:       2018/8/19
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ X-GUI Layer Driver
/------------------------------------------------------------------------*/

#include "x_gui_layer.h"

/*----------------------------------------------------------------------
 *  LayerBmpOut
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void LayerBmpOut(gu16 x, gu16 y, _LayerBmpT *bmp, gu8 rev)
{

    xCursorT ulc, lrc;

    //------------------------------------------------------------
    //              Check Range
    //------------------------------------------------------------

    ulc.x = x; ulc.y = y; 
    lrc.x = x + bmp->x_size - 1; lrc.y = y + bmp->y_size - 1;

    if ((lrc.x >= _LCD_X_SIZE) || (lrc.y >= _LCD_Y_SIZE)) return;


    //------------------------------------------------------------
    //              Out Mask
    //------------------------------------------------------------
    
    pLcdWritePixelsEx(&ulc, &lrc, (gu8*)bmp->mask, rev);
}

/*----------------------------------------------------------------------
 *  LayerBmpRead
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
gu16 LayerBmpRead(xCursorT *ulc, xCursorT *lrc, gu8 *p_col, gu16 buff_size)
{
    
    //------------------------------------------------------------
    //              Check Range
    //------------------------------------------------------------

    if ((lrc->x >= _LCD_X_SIZE) || (lrc->y >= _LCD_Y_SIZE)) return 0;
    if (((lrc->y - ulc->y + 1) * (lrc->x - ulc->x + 1)) > buff_size) return 0;

    //------------------------------------------------------------
    //              B-B Read
    //------------------------------------------------------------

    return pLcdReadPixels(ulc, lrc, p_col, buff_size);
}

/*----------------------------------------------------------------------
 *  LayerRender
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void LayerRender(xCursorT *ulc, xCursorT *lrc, gu8 col, gu8 rev)
{

    // Fill Gray
    pLcdFillGray(ulc, lrc, col);

    // Rev
    if (rev) { pLcdRevPixels(ulc, lrc, rev); }
}

/*----------------------------------------------------------------------
 *  LayerClrScreen
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void LayerClrScreen(gu8 col)
{

    xCursorT ulc, lrc;

    ulc.x = 0; ulc.y = 0; lrc.x = _LCD_X_SIZE - 1; lrc.y = _LCD_Y_SIZE - 1;
    pLcdFillPixels(&ulc, &lrc, col);
}

/*----------------------------------------------------------------------
 *  LayerRevScreen
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void LayerRevScreen(gu8 rev)
{

    xCursorT ulc, lrc;

    ulc.x = 0; ulc.y = 0; lrc.x = _LCD_X_SIZE - 1; lrc.y = _LCD_Y_SIZE - 1;
    if (rev) { pLcdRevPixels(&ulc, &lrc, rev); }    
}

//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
