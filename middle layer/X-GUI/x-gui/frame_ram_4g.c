
/*----------------------------------------------------------------------^^-
/ File name:  frame_ram_4g.h
/ Author:     JiangJun
/ Data:       [2018-8-15]
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ Gray Mode Frame RAM Driver
/------------------------------------------------------------------------*/

#include "frame_ram_4g.h"

//------------------------------------------------------------
// Frame RAM
// 
// Map LCD:
//              C1 C2 C3 C4 C5 ...
//      PAGE1
//      PAGE2
//      PAGE3
//        .
//        .
//        .
//------------------------------------------------------------

static gu8 FrameRam[_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS][_FRAME_RAM_SIZE_X]; 

//------------------------------------------------------------
// Function Pointers
//------------------------------------------------------------

static fpWriteFrame FpWriteFrame; static fpSetConstrast FpSetConstrast;
static fpSetBackLight FpSetBackLight;

//------------------------------------------------------------
// B/F Color
//------------------------------------------------------------

// [ B_COLOR/ F_COLOR ] [ WR FUNC ]
// ---
// [ 0/1 ], 0: BackColor 1: FrontColor
static gu8 _FB_COLOR[2] = { _COLOR_WHITE, _COLOR_BLACK };

// [ F/ B COLOR TABLE ] [ WR FUNC ]
static gu8 _FB_COL_TAB[16];

// Bit Ex Table [ FILL FUNC ]
static const gu8 FrameRamCoLEx_8Bt_Tab[4] = { 0x00, 0x55, 0xAA, 0xFF };


/*----------------------------------------------------------------------
 *  pLcdInit
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void pLcdInit(fpInit fp_init, fpWriteFrame fp_write_fme, fpSetConstrast fp_set_cst, fpSetBackLight fp_set_bkl)
{       

    // Init Hw
    if (fp_init != 0)
    {
        (*fp_init)();
    }

    FpWriteFrame = fp_write_fme;
    FpSetConstrast = fp_set_cst;
    FpSetBackLight = fp_set_bkl;

    // Brush Init
    pLcdSetBrush(_FB_COLOR[1], _FB_COLOR[0]);
}

/*----------------------------------------------------------------------
 *  pLcdUpdate
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void pLcdUpdate(void)
{
    if (FpWriteFrame != 0)
    {
        (*FpWriteFrame)((unsigned char *)FrameRam, _FRAME_RAM_SIZE_X * _FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS);
    }
}

/*----------------------------------------------------------------------
 *  pLcdSetPixel
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void pLcdSetPixel(gu16 x, gu16 y, gu8 color)
{    
    
#if ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))


    // V: 0  H: 0
    
    FrameRam[y / _FRAME_RAM_BYTE_PIXELS][x] &= ~(0x03 << ((y % _FRAME_RAM_BYTE_PIXELS) << 1));
    FrameRam[y / _FRAME_RAM_BYTE_PIXELS][x] |= (color << ((y % _FRAME_RAM_BYTE_PIXELS) << 1));
    
    
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 1) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))


    // V: 1  H: 0
    
    FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - (y / _FRAME_RAM_BYTE_PIXELS) - 1][x] &= ~(0x03 << ((_FRAME_RAM_BYTE_PIXELS - 1 - (y % _FRAME_RAM_BYTE_PIXELS)) << 1));
    FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - (y / _FRAME_RAM_BYTE_PIXELS) - 1][x] |= (color << ((_FRAME_RAM_BYTE_PIXELS - 1 - (y % _FRAME_RAM_BYTE_PIXELS)) << 1));

    
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 1))


    // V: 0  H: 1
    
    FrameRam[y / _FRAME_RAM_BYTE_PIXELS][_FRAME_RAM_SIZE_X - x - 1] &= ~(0x03 << ((y % _FRAME_RAM_BYTE_PIXELS) << 1));
    FrameRam[y / _FRAME_RAM_BYTE_PIXELS][_FRAME_RAM_SIZE_X - x - 1] |= (color << ((y % _FRAME_RAM_BYTE_PIXELS) << 1));

    
#else

    // V: 1  H: 1
    
    FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - (y / _FRAME_RAM_BYTE_PIXELS) - 1][_FRAME_RAM_SIZE_X - x - 1] &= ~(0x03 << ((_FRAME_RAM_BYTE_PIXELS - 1 - (y % _FRAME_RAM_BYTE_PIXELS)) << 1));
    FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - (y / _FRAME_RAM_BYTE_PIXELS) - 1][_FRAME_RAM_SIZE_X - x - 1] |= (color << ((_FRAME_RAM_BYTE_PIXELS - 1 - (y % _FRAME_RAM_BYTE_PIXELS)) << 1));
    
#endif
}

/*----------------------------------------------------------------------
 *  pLcdGetPixel
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void pLcdGetPixel(gu16 x, gu16 y, gu8 *color)
{

#if ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))


    // V: 0  H: 0
    *color = FrameRam[y / _FRAME_RAM_BYTE_PIXELS][x] >> ((y % _FRAME_RAM_BYTE_PIXELS) << 1);        
    
    
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 1) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))


    // V: 1  H: 0
    *color = FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - (y / _FRAME_RAM_BYTE_PIXELS) - 1][x] >> ((_FRAME_RAM_BYTE_PIXELS - 1 - (y % _FRAME_RAM_BYTE_PIXELS)) << 1);   

    
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 1))


    // V: 0  H: 1
    *color = FrameRam[y / _FRAME_RAM_BYTE_PIXELS][_FRAME_RAM_SIZE_X - x - 1] >> ((y % _FRAME_RAM_BYTE_PIXELS) << 1);   

    
#else

    // V: 1  H: 1
    *color = FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - (y / _FRAME_RAM_BYTE_PIXELS) - 1][_FRAME_RAM_SIZE_X - x - 1] >> ((_FRAME_RAM_BYTE_PIXELS - 1 - (y % _FRAME_RAM_BYTE_PIXELS)) << 1);

#endif

    *color &= 0x03;
}

/*----------------------------------------------------------------------
 *  pLcdRevPixel
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void pLcdRevPixel(gu16 x, gu16 y)
{    
    
#if ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))


    // V: 0  H: 0
    
    FrameRam[y / _FRAME_RAM_BYTE_PIXELS][x] ^= (0x03 << ((y % _FRAME_RAM_BYTE_PIXELS) << 1));    
    
    
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 1) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))


    // V: 1  H: 0
    
    FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - (y / _FRAME_RAM_BYTE_PIXELS) - 1][x] ^= (0x03 << ((_FRAME_RAM_BYTE_PIXELS - 1 - (y % _FRAME_RAM_BYTE_PIXELS)) << 1)); 

    
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 1))


    // V: 0  H: 1
    
    FrameRam[y / _FRAME_RAM_BYTE_PIXELS][_FRAME_RAM_SIZE_X - x - 1] ^= (0x03 << ((y % _FRAME_RAM_BYTE_PIXELS) << 1)); 

    
#else

    // V: 1  H: 1
    
    FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - (y / _FRAME_RAM_BYTE_PIXELS) - 1][_FRAME_RAM_SIZE_X - x - 1] ^= (0x03 << ((_FRAME_RAM_BYTE_PIXELS - 1 - (y % _FRAME_RAM_BYTE_PIXELS)) << 1));     
    
#endif
}

/*----------------------------------------------------------------------
 *  pLcdSetBrush
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void pLcdSetBrush(gu8 f_col, gu8 b_col)
{

    gu8 idx = 0; 

    // B/ F COLOR
    _FB_COLOR[0] = b_col; _FB_COLOR[1] = f_col;

    // Init Table
    for (idx = 0; idx < 16; idx++)
    {

        _FB_COL_TAB[idx] = 0;

        // V TAB
#if _FRAME_ENABLE_VERTICAL_FLIP

        // P3
        _FB_COL_TAB[idx] |= _FB_COLOR[(idx >> 0) & 0x01];
        _FB_COL_TAB[idx] <<= 2;
        
        // P2
        _FB_COL_TAB[idx] |= _FB_COLOR[(idx >> 1) & 0x01];
        _FB_COL_TAB[idx] <<= 2;
        
        // P1
        _FB_COL_TAB[idx] |= _FB_COLOR[(idx >> 2) & 0x01];
        _FB_COL_TAB[idx] <<= 2;
        
        // P0
        _FB_COL_TAB[idx] |= _FB_COLOR[(idx >> 3) & 0x01];

#else

        // P3
        _FB_COL_TAB[idx] |= _FB_COLOR[(idx >> 3) & 0x01];
        _FB_COL_TAB[idx] <<= 2;
        
        // P2
        _FB_COL_TAB[idx] |= _FB_COLOR[(idx >> 2) & 0x01];
        _FB_COL_TAB[idx] <<= 2;
        
        // P1
        _FB_COL_TAB[idx] |= _FB_COLOR[(idx >> 1) & 0x01];
        _FB_COL_TAB[idx] <<= 2;
        
        // P0
        _FB_COL_TAB[idx] |= _FB_COLOR[(idx >> 0) & 0x01];
        
#endif /* _FRAME_ENABLE_VERTICAL_FLIP */
    }    
}

/*----------------------------------------------------------------------
 *  pLcdFillPixels
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    
 *                  TEST: [ IAR Medium ] [ V:0 H:1 ]
 *                      
 *                      (0, 0), (239, 159) - 3.22ms
 *                      (0, 1), (239, 158) - 4.00ms(Max)
 *---------------------------------------------------------------------*/
void pLcdFillPixels(xCursorT *ulc, xCursorT *lrc, gu8 color)
{

    gs16 y_s_align = 0, y_e_align = 0; gu16 x = 0, y = 0;

    // Align By 4
    y_s_align = ((ulc->y + (_FRAME_RAM_BYTE_PIXELS - 1)) / _FRAME_RAM_BYTE_PIXELS) * _FRAME_RAM_BYTE_PIXELS;        // [ ( ( y+3 ) / 4 ) * 4 ]
    y_e_align = (((lrc->y + 1) / _FRAME_RAM_BYTE_PIXELS) * _FRAME_RAM_BYTE_PIXELS) - 1;                              // [ ( ( y+1 ) / 4 ) * 4 ] - 1


    //------------------------------------------------------------
    //              Critical Fill
    //------------------------------------------------------------

    // TOP/ BOTTOM FILL
    if (y_s_align < y_e_align)
    {

        // [ TOP ]
        for (y = ulc->y; y < y_s_align; y++)
        {

            // X Line Fill
            for (x = ulc->x; x <= lrc->x; x++)  {        
                pLcdSetPixel(x, y, color);
            }        
        }

        // [ BOTTOM ]
        for (y = y_e_align + 1; y <= lrc->y; y++)
        {

            // X Line Fill
            for (x = ulc->x; x <= lrc->x; x++)  {        
                pLcdSetPixel(x, y, color);
            }        
        }        
    }
    else
    {

        // [ No '4-Align' Block Fill ]
        for (y = ulc->y; y <= lrc->y; y++)
        {

            // X Line Fill
            for (x = ulc->x; x <= lrc->x; x++)  {        
                pLcdSetPixel(x, y, color);
            }        
        }
    }


    //------------------------------------------------------------
    //              Fill 4-Align
    //------------------------------------------------------------
    
    for (; y_s_align < y_e_align; y_s_align += _FRAME_RAM_BYTE_PIXELS)
    {

        // Y idx
        y = y_s_align / _FRAME_RAM_BYTE_PIXELS;
        
        // X Line Fill
        for (x = ulc->x; x <= lrc->x; x++) 
        {            

            //-------------------------------------------------------
            //              x/y [V/H] Flip
            //-------------------------------------------------------
            
#if ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))

            // V: 0  H: 0
            FrameRam[y][x] = FrameRamCoLEx_8Bt_Tab[color];

            
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 1) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))

            // V: 1  H: 0
            FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - y - 1][x] = FrameRamCoLEx_8Bt_Tab[color];
            
            
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 1))

            // V: 0  H: 1
            FrameRam[y][_FRAME_RAM_SIZE_X - x - 1] = FrameRamCoLEx_8Bt_Tab[color];
            
            
#else

            // V: 1  H: 1
            FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - y - 1][_FRAME_RAM_SIZE_X - x - 1] = FrameRamCoLEx_8Bt_Tab[color];
            
#endif
        }        
    }
}

/*----------------------------------------------------------------------
 *  __x_line_wr
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void __x_line_wr(gu16 xs, gu16 xe, gu16 ys, gu16 y, gu8 *p_col)
{

    gu16 x_off = 0; gu16 x = 0; gu16 bt_off = 0; gu8 col;
    
    // X Size Byte
    bt_off = (((xe - xs) / 8) + 1) * (y - ys);
    
    // X Line Wr 
    for (x = xs, x_off = 0; x <= xe; x++, x_off++)
    {

        // Get Mask Col [0/1]                                        
        col = (*(p_col + (x_off / 8) + bt_off) >> (x_off % 8)) & 0x01;

        // Set Pixel
        pLcdSetPixel(x, y, _FB_COLOR[col]);     
    }
}

/*----------------------------------------------------------------------
 *  pLcdWritePixels
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    
 *                  TEST: [ IAR Medium ] [ V:0 H:1 ]
 *                      
 *                      (0, 0), (239, 159) - 12.30ms
 *                      (0, 1), (239, 158) - 12.94ms(Max)
 *---------------------------------------------------------------------*/
void pLcdWritePixels(xCursorT *ulc, xCursorT *lrc, gu8 *p_col)
{

    gs16 y_s_align = 0, y_e_align = 0; gu16 x = 0, y = 0; gu8 color;
    gu16 x_bt_size = 0; gu8 *p_col_tmp = p_col; gu16 col_bt_off = 0;

    // Align By 4
    y_s_align = ((ulc->y + (_FRAME_RAM_BYTE_PIXELS - 1)) / _FRAME_RAM_BYTE_PIXELS) * _FRAME_RAM_BYTE_PIXELS;        // [ ( ( y+3 ) / 4 ) * 4 ]
    y_e_align = (((lrc->y + 1) / _FRAME_RAM_BYTE_PIXELS) * _FRAME_RAM_BYTE_PIXELS) - 1;                              // [ ( ( y+1 ) / 4 ) * 4 ] - 1


    //------------------------------------------------------------
    //              Critical Wr
    //------------------------------------------------------------

    // TOP/ BOTTOM FILL
    if (y_s_align < y_e_align)
    {

        // [ TOP ]
        for (y = ulc->y; y < y_s_align; y++)
        {

            __x_line_wr(ulc->x, lrc->x, ulc->y, y, p_col_tmp);      
        }

        // Mask Jump
        p_col_tmp += (y_e_align + 1 - ulc->y) * (((lrc->x - ulc->x + 1) / 8) + 1);

        // [ BOTTOM ]
        for (y = y_e_align + 1; y <= lrc->y; y++)
        {

            __x_line_wr(ulc->x, lrc->x, y_e_align + 1, y, p_col_tmp);   
        }        
    }
    else
    {

        // [ No '4-Align' Block Fill ]
        for (y = ulc->y; y <= lrc->y; y++)
        {           
        
            __x_line_wr(ulc->x, lrc->x, ulc->y, y, p_col_tmp);
        }
    }

    
    //------------------------------------------------------------
    //              Wr 4-Align
    //------------------------------------------------------------

    // X Byte Size
    x_bt_size = ((lrc->x - ulc->x) / 8) + 1;     

    // Field Wr
    for (; y_s_align < y_e_align; y_s_align += _FRAME_RAM_BYTE_PIXELS)
    {

        // Mask Jump
        p_col_tmp = p_col + ((y_s_align - ulc->y) * x_bt_size);

        // Y idx
        y = y_s_align / _FRAME_RAM_BYTE_PIXELS;
        
        // X Line Wr
        for (x = ulc->x; x <= lrc->x; x++) 
        {            

            //-------------------------------------------------------
            //              Get COL
            //-------------------------------------------------------

            // P1
            col_bt_off = (x - ulc->x) >> 3;
            color = (*(p_col_tmp + col_bt_off) >> ((x - ulc->x) % 8)) & 0x01;

            // P2
            color |= ((*(p_col_tmp + col_bt_off + x_bt_size) >> ((x - ulc->x) % 8)) & 0x01) << 1;

            // P3
            color |= ((*(p_col_tmp + col_bt_off + (x_bt_size << 1)) >> ((x - ulc->x) % 8)) & 0x01) << 2;

            // P4
            color |= ((*(p_col_tmp + col_bt_off + (x_bt_size << 1) + x_bt_size) >> ((x - ulc->x) % 8)) & 0x01) << 3;

            
            //-------------------------------------------------------
            //              x/y [V/H] Flip
            //-------------------------------------------------------
            
#if ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))

            // V: 0  H: 0
            FrameRam[y][x] = _FB_COL_TAB[color];

            
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 1) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))

            // V: 1  H: 0
            FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - y - 1][x] = _FB_COL_TAB[color];
            
            
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 1))

            // V: 0  H: 1
            FrameRam[y][_FRAME_RAM_SIZE_X - x - 1] = _FB_COL_TAB[color];
                        
#else

            // V: 1  H: 1
            FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - y - 1][_FRAME_RAM_SIZE_X - x - 1] = _FB_COL_TAB[color];
            
#endif
        }        
    }
}

/*----------------------------------------------------------------------
 *  pLcdWritePixelsEx
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    
 *                  TEST: [ IAR Medium ] [ V:0 H:1 ]
 *                      
 *                      (0, 0), (239, 159) - 12.30ms
 *                      (0, 1), (239, 158) - 12.94ms(Max)
 *---------------------------------------------------------------------*/
void pLcdWritePixelsEx(xCursorT *ulc, xCursorT *lrc, gu8 *p_col, gu8 rev)
{

    gs16 y_s_align = 0, y_e_align = 0; gu16 x = 0, y = 0; gu8 color;
    gu16 x_bt_size = 0; gu8 *p_col_tmp = p_col; gu16 col_bt_off = 0;

    // Align By 4
    y_s_align = ((ulc->y + (_FRAME_RAM_BYTE_PIXELS - 1)) / _FRAME_RAM_BYTE_PIXELS) * _FRAME_RAM_BYTE_PIXELS;        // [ ( ( y+3 ) / 4 ) * 4 ]
    y_e_align = (((lrc->y + 1) / _FRAME_RAM_BYTE_PIXELS) * _FRAME_RAM_BYTE_PIXELS) - 1;                              // [ ( ( y+1 ) / 4 ) * 4 ] - 1


    //------------------------------------------------------------
    //              Brush Rev
    //------------------------------------------------------------

    if (rev)
    {
        pLcdSetBrush(_FB_COLOR[0], _FB_COLOR[1]);
    }
    
    
    //------------------------------------------------------------
    //              Critical Wr
    //------------------------------------------------------------

    // TOP/ BOTTOM FILL
    if (y_s_align < y_e_align)
    {

        // [ TOP ]
        for (y = ulc->y; y < y_s_align; y++)
        {

            __x_line_wr(ulc->x, lrc->x, ulc->y, y, p_col_tmp);      
        }

        // Mask Jump
        p_col_tmp += (y_e_align + 1 - ulc->y) * (((lrc->x - ulc->x + 1) / 8) + 1);

        // [ BOTTOM ]
        for (y = y_e_align + 1; y <= lrc->y; y++)
        {

            __x_line_wr(ulc->x, lrc->x, y_e_align + 1, y, p_col_tmp);   
        }        
    }
    else
    {

        // [ No '4-Align' Block Fill ]
        for (y = ulc->y; y <= lrc->y; y++)
        {           
        
            __x_line_wr(ulc->x, lrc->x, ulc->y, y, p_col_tmp);
        }
    }

    
    //------------------------------------------------------------
    //              Wr 4-Align
    //------------------------------------------------------------

    // X Byte Size
    x_bt_size = ((lrc->x - ulc->x) / 8) + 1;     

    // Field Wr
    for (; y_s_align < y_e_align; y_s_align += _FRAME_RAM_BYTE_PIXELS)
    {

        // Mask Jump
        p_col_tmp = p_col + ((y_s_align - ulc->y) * x_bt_size);

        // Y idx
        y = y_s_align / _FRAME_RAM_BYTE_PIXELS;
        
        // X Line Wr
        for (x = ulc->x; x <= lrc->x; x++) 
        {            

            //-------------------------------------------------------
            //              Get COL
            //-------------------------------------------------------

            // P1
            col_bt_off = (x - ulc->x) >> 3;
            color = (*(p_col_tmp + col_bt_off) >> ((x - ulc->x) % 8)) & 0x01;

            // P2
            color |= ((*(p_col_tmp + col_bt_off + x_bt_size) >> ((x - ulc->x) % 8)) & 0x01) << 1;

            // P3
            color |= ((*(p_col_tmp + col_bt_off + (x_bt_size << 1)) >> ((x - ulc->x) % 8)) & 0x01) << 2;

            // P4
            color |= ((*(p_col_tmp + col_bt_off + (x_bt_size << 1) + x_bt_size) >> ((x - ulc->x) % 8)) & 0x01) << 3;

            
            //-------------------------------------------------------
            //              x/y [V/H] Flip
            //-------------------------------------------------------
            
#if ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))

            // V: 0  H: 0
            FrameRam[y][x] = _FB_COL_TAB[color];

            
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 1) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))

            // V: 1  H: 0
            FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - y - 1][x] = _FB_COL_TAB[color];
            
            
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 1))

            // V: 0  H: 1
            FrameRam[y][_FRAME_RAM_SIZE_X - x - 1] = _FB_COL_TAB[color];
                        
#else

            // V: 1  H: 1
            FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - y - 1][_FRAME_RAM_SIZE_X - x - 1] = _FB_COL_TAB[color];
            
#endif
        }        
    }
    

    //------------------------------------------------------------
    //              Brush Rev
    //------------------------------------------------------------

    if (rev)
    {
        pLcdSetBrush(_FB_COLOR[0], _FB_COLOR[1]);
    }
}

/*----------------------------------------------------------------------
 *  pLcdRevPixels
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    
 *                  TEST: [ IAR Medium ] [ V:0 H:1 ]
 *                      
 *                      (0, 0), (239, 159) - 2.70ms
 *                      (0, 1), (239, 158) - 3.48ms(Max)
 *---------------------------------------------------------------------*/
void pLcdRevPixels(xCursorT *ulc, xCursorT *lrc, gu8 rev)
{

    gs16 y_s_align = 0, y_e_align = 0; gu16 x = 0, y = 0;


    //------------------------------------------------------------
    //              Check Rev
    //------------------------------------------------------------

    if (!rev) return;


    //------------------------------------------------------------
    //              Align By 4
    //------------------------------------------------------------
    
    y_s_align = ((ulc->y + (_FRAME_RAM_BYTE_PIXELS - 1)) / _FRAME_RAM_BYTE_PIXELS) * _FRAME_RAM_BYTE_PIXELS;        // [ ( ( y+3 ) / 4 ) * 4 ]
    y_e_align = (((lrc->y + 1) / _FRAME_RAM_BYTE_PIXELS) * _FRAME_RAM_BYTE_PIXELS) - 1;                              // [ ( ( y+1 ) / 4 ) * 4 ] - 1


    //------------------------------------------------------------
    //              Critical Fill
    //------------------------------------------------------------

    // TOP/ BOTTOM FILL
    if (y_s_align < y_e_align)
    {

        // [ TOP ]
        for (y = ulc->y; y < y_s_align; y++)
        {

            // X Line Fill
            for (x = ulc->x; x <= lrc->x; x++)  {        
                pLcdRevPixel(x, y);
            }        
        }

        // [ BOTTOM ]
        for (y = y_e_align + 1; y <= lrc->y; y++)
        {

            // X Line Fill
            for (x = ulc->x; x <= lrc->x; x++)  {        
                pLcdRevPixel(x, y);
            }        
        }        
    }
    else
    {

        // [ No '4-Align' Block Fill ]
        for (y = ulc->y; y <= lrc->y; y++)
        {

            // X Line Fill
            for (x = ulc->x; x <= lrc->x; x++)  {        
                pLcdRevPixel(x, y);
            }        
        }
    }


    //------------------------------------------------------------
    //              Fill 4-Align
    //------------------------------------------------------------
    
    for (; y_s_align < y_e_align; y_s_align += _FRAME_RAM_BYTE_PIXELS)
    {

        // Y idx
        y = y_s_align / _FRAME_RAM_BYTE_PIXELS;
        
        // X Line Fill
        for (x = ulc->x; x <= lrc->x; x++) 
        {            

            //-------------------------------------------------------
            //              x/y [V/H] Flip
            //-------------------------------------------------------
            
#if ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))

            // V: 0  H: 0
            FrameRam[y][x] = ~FrameRam[y][x];

            
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 1) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))

            // V: 1  H: 0
            FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - y - 1][x] = ~FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - y - 1][x];
            
            
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 1))

            // V: 0  H: 1
            FrameRam[y][_FRAME_RAM_SIZE_X - x - 1] = ~FrameRam[y][_FRAME_RAM_SIZE_X - x - 1];
            
            
#else

            // V: 1  H: 1
            FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - y - 1][_FRAME_RAM_SIZE_X - x - 1] = ~FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - y - 1][_FRAME_RAM_SIZE_X - x - 1];
            
#endif
        }        
    }
}

/*----------------------------------------------------------------------
 *  __x_line_fill_gray
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void __x_line_fill_gray(gu16 xs, gu16 xe, gu16 y, gu8 color)
{

    gu8 pix_col = 0;
    
    // X Line Fill Gray
    for (; xs <= xe; xs++)
    {

        // Get Color
        pLcdGetPixel(xs, y, &pix_col);

        // Not-White
        if (pix_col != _COLOR_WHITE) { pLcdSetPixel(xs, y, color); }
    }
}

/*----------------------------------------------------------------------
 *  pLcdFillGray
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    
 *                  TEST: [ IAR Medium ] [ V:0 H:1 ]
 *                      
 *                      (0, 0), (239, 159) - 6.16ms
 *                      (0, 1), (239, 158) - 6.86ms(Max)
 *---------------------------------------------------------------------*/
void pLcdFillGray(xCursorT *ulc, xCursorT *lrc, gu8 color)
{

    gs16 y_s_align = 0, y_e_align = 0; gu16 x = 0, y = 0; gu8 ram_col = 0, gray_col = 0;
    gu8 FrameRamGray_Tab[4];


    //------------------------------------------------------------
    //              Gen Gray Table
    //------------------------------------------------------------

    FrameRamGray_Tab[_COLOR_WHITE] = _COLOR_WHITE; FrameRamGray_Tab[_COLOR_GRAY_1] = color;
    FrameRamGray_Tab[_COLOR_GRAY_2] = color; FrameRamGray_Tab[_COLOR_BLACK] = color;
    

    // Align By 4
    y_s_align = ((ulc->y + (_FRAME_RAM_BYTE_PIXELS - 1)) / _FRAME_RAM_BYTE_PIXELS) * _FRAME_RAM_BYTE_PIXELS;        // [ ( ( y+3 ) / 4 ) * 4 ]
    y_e_align = (((lrc->y + 1) / _FRAME_RAM_BYTE_PIXELS) * _FRAME_RAM_BYTE_PIXELS) - 1;                              // [ ( ( y+1 ) / 4 ) * 4 ] - 1


    //------------------------------------------------------------
    //              Critical Fill
    //------------------------------------------------------------

    // TOP/ BOTTOM FILL
    if (y_s_align < y_e_align)
    {

        // [ TOP ]
        for (y = ulc->y; y < y_s_align; y++)
        {

            __x_line_fill_gray(ulc->x, lrc->x, y, color);
        }

        // [ BOTTOM ]
        for (y = y_e_align + 1; y <= lrc->y; y++)
        {

            __x_line_fill_gray(ulc->x, lrc->x, y, color);   
        }        
    }
    else
    {

        // [ No '4-Align' Block Fill ]
        for (y = ulc->y; y <= lrc->y; y++)
        {

            __x_line_fill_gray(ulc->x, lrc->x, y, color);
        }
    }


    //------------------------------------------------------------
    //              Fill 4-Align
    //------------------------------------------------------------
    
    for (; y_s_align < y_e_align; y_s_align += _FRAME_RAM_BYTE_PIXELS)
    {

        // Y idx
        y = y_s_align / _FRAME_RAM_BYTE_PIXELS;
        
        // X Line Fill
        for (x = ulc->x; x <= lrc->x; x++) 
        {            

            //-------------------------------------------------------
            //              x/y [V/H] Flip
            //-------------------------------------------------------
            
#if ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))

            // V: 0  H: 0
            ram_col = FrameRam[y][x];

            
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 1) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))

            // V: 1  H: 0
            ram_col = FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - y - 1][x];
            
            
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 1))

            // V: 0  H: 1
            ram_col = FrameRam[y][_FRAME_RAM_SIZE_X - x - 1];
            
            
#else

            // V: 1  H: 1
            ram_col = FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - y - 1][_FRAME_RAM_SIZE_X - x - 1];
            
#endif


            //-------------------------------------------------------
            //              Gray Set
            //-------------------------------------------------------

            // P1
            gray_col = FrameRamGray_Tab[ram_col & 0x03];

            // P2
            gray_col |= FrameRamGray_Tab[(ram_col >> 2) & 0x03] << 2;
            
            // P3
            gray_col |= FrameRamGray_Tab[(ram_col >> 4) & 0x03] << 4;
            
            // P4
            gray_col |= FrameRamGray_Tab[(ram_col >> 6) & 0x03] << 6;

            
            //-------------------------------------------------------
            //              x/y [V/H] Flip
            //-------------------------------------------------------
            
#if ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))

            // V: 0  H: 0
            FrameRam[y][x] = gray_col;

            
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 1) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))

            // V: 1  H: 0
            FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - y - 1][x] = gray_col;
            
            
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 1))

            // V: 0  H: 1
            FrameRam[y][_FRAME_RAM_SIZE_X - x - 1] = gray_col;
            
            
#else

            // V: 1  H: 1
            FrameRam[(_FRAME_RAM_SIZE_Y / _FRAME_RAM_BYTE_PIXELS) - y - 1][_FRAME_RAM_SIZE_X - x - 1] = gray_col;
            
#endif
        }        
    }
}

/*----------------------------------------------------------------------
 *  pLcdReadPixels - [ H Scan ]
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    p_col out(x,y):
 *
 *                      (0,0) (1,0) ... (0,1) (1,1) ...
 *---------------------------------------------------------------------*/
gu16 pLcdReadPixels(xCursorT *ulc, xCursorT *lrc, gu8 *p_col, gu16 buff_size)
{

    gu16 x = 0, y = 0; gu8 col = 0;

    //------------------------------------------------------------
    //              Check Size
    //------------------------------------------------------------

    if (((lrc->y - ulc->y + 1) * (lrc->x - ulc->x + 1)) > buff_size) return 0;


    //------------------------------------------------------------
    //              Pixel Read
    //------------------------------------------------------------

    for (y = ulc->y; y <= lrc->y; y++)
    {

        // X-Line Read
        for (x = ulc->x; x <= lrc->x; x++) {

            pLcdGetPixel(x, y, &col); *p_col = col; p_col++;            
        }        
    }

    return ((lrc->y - ulc->y + 1) * (lrc->x - ulc->x + 1));
}

/*----------------------------------------------------------------------
 *  pLcdSetCst
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void pLcdSetCst(gu8 cst)
{

    // 0-100
    cst %= 101;

    // Set Hw
    if (FpSetConstrast)
    {

        (*FpSetConstrast)(cst);
    }
}

/*----------------------------------------------------------------------
 *  pLcdSetBackLight
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void pLcdSetBackLight(gu8 bkt)
{

    // 0-100
    bkt %= 101;

    // Set Hw
    if (FpSetBackLight)
    {

        (*FpSetBackLight)(bkt);
    }
}


//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
