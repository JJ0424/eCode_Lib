
/*----------------------------------------------------------------------^^-
/ File name:  frame_ram.c
/ Author:     JiangJun
/ Data:       [2018-8-14]
/ Version:    v1.4
/-----------------------------------------------------------------------^^-
/ Mono Mode Frame RAM Driver
/ ---
/ v1.1 [2018-6-8]
/ 1. 增加增强型写字节流函数，带反显字符颜色
/ ---
/ v1.2 [2018-6-21]
/ 1. 增加 pLcdRevPixels()，支持块反转颜色
/ ---
/ v1.3 [2018-8-10]
/ 1. 增加 _FRAME_ENABLE_VERTICAL_FLIP / _FRAME_ENABLE_HORIZONTAL_FLIP功能
/ ---
/ v1.4 [2018-8-14]
/ 1. 测试速度：240x160满屏写
/               pLcdWritePixelsEx   - 44ms
/               pLcdFillPixels      - 27ms
/------------------------------------------------------------------------*/

#include "frame_ram.h"

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

gu8 FrameRam[_FRAME_RAM_SIZE_Y / 8][_FRAME_RAM_SIZE_X]; 

//------------------------------------------------------------
// Function Pointers
//------------------------------------------------------------

static fpWriteFrame FpWriteFrame; static fpSetConstrast FpSetConstrast;


/*----------------------------------------------------------------------
 *  _inline_lcd_set_pixel
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
inline void _inline_lcd_set_pixel(CoorT x, CoorT y, ColorT color)
{

#if ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))

    // V: 0  H: 0
    
    FrameRam[y / 8][x] &= ~(0x01 << (y % 8));
    FrameRam[y / 8][x] |= ((gu8)color << (y % 8));

    
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 1) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))

    // V: 1  H: 0
    
    FrameRam[(_FRAME_RAM_SIZE_Y / 8) - (y / 8) - 1][x] &= ~(0x01 << (7 - (y % 8)));
    FrameRam[(_FRAME_RAM_SIZE_Y / 8) - (y / 8) - 1][x] |= ((gu8)color << (7 - (y % 8)));
    
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 1))

    // V: 0  H: 1
    
    FrameRam[y / 8][_FRAME_RAM_SIZE_X - x - 1] &= ~(0x01 << (y % 8));
    FrameRam[y / 8][_FRAME_RAM_SIZE_X - x - 1] |= ((gu8)color << (y % 8));
    
#else

    // V: 1  H: 1
    
    FrameRam[(_FRAME_RAM_SIZE_Y / 8) - (y / 8) - 1][_FRAME_RAM_SIZE_X - x - 1] &= ~(0x01 << (7 - (y % 8)));
    FrameRam[(_FRAME_RAM_SIZE_Y / 8) - (y / 8) - 1][_FRAME_RAM_SIZE_X - x - 1] |= ((gu8)color << (7 - (y % 8)));
    
#endif
}

/*----------------------------------------------------------------------
 *  _inline_lcd_rev_pixel
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
inline void _inline_lcd_rev_pixel(CoorT x, CoorT y)
{    

#if ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))

    // V: 0  H: 0    
    FrameRam[y / 8][x] ^= (0x01 << (y % 8));
    
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 1) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 0))

    // V: 1  H: 0    
    FrameRam[(_FRAME_RAM_SIZE_Y / 8) - (y / 8) - 1][x] ^= (0x01 << (7 - (y % 8)));
    
#elif ((_FRAME_ENABLE_VERTICAL_FLIP == 0) && (_FRAME_ENABLE_HORIZONTAL_FLIP == 1))

    // V: 0  H: 1    
    FrameRam[y / 8][_FRAME_RAM_SIZE_X - x - 1] ^= (0x01 << (y % 8));
    
#else

    // V: 1  H: 1    
    FrameRam[(_FRAME_RAM_SIZE_Y / 8) - (y / 8) - 1][_FRAME_RAM_SIZE_X - x - 1] ^= (0x01 << (7 - (y % 8)));
    
#endif    
}

/*----------------------------------------------------------------------
 *  pLcdSetPixel
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void pLcdSetPixel(CoorT x, CoorT y, ColorT color)
{
    
    _inline_lcd_set_pixel(x, y, color);
}

/*----------------------------------------------------------------------
 *  pLcdGetPixel
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void pLcdGetPixel(CoorT x, CoorT y, ColorT *color)
{
    
}

/*----------------------------------------------------------------------
 *  pLcdWritePixels
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    !! Function use system forground as the color.
 *              color is a byte type, (8-bit --- 8 pixels)
 *              0: background, 1: forground.
 *---------------------------------------------------------------------*/
void pLcdWritePixels(CursorT *ulc, CursorT *lrc, ColorT *color)
{
    CoorSignT mid_x, mid_y;
    ColorT mid_col;
    gu8 cor_bit_point;
    
    // Write block data.
    if ((lrc->x >= ulc->x) && (lrc->y >= ulc->y))
    {
        for (mid_y = ulc->y; mid_y <= lrc->y; mid_y++)
        {
            cor_bit_point = 0;
            
            for (mid_x = ulc->x; mid_x <= lrc->x; mid_x++)
            {
                // Get color.
                mid_col = (*color) & (0x01 << cor_bit_point);
                // color byte jump.
                cor_bit_point++;
                if (cor_bit_point == 8) 
                {
                    cor_bit_point = 0;
                    color++;
                }

                // write color.
                if (mid_col == 0) {
                    _inline_lcd_set_pixel(mid_x, mid_y, _COLOR_WHITE);
                }
                else {
                    _inline_lcd_set_pixel(mid_x, mid_y, _COLOR_BLACK);
                }
            }
            if (cor_bit_point != 0) {
                color++;
            }
        }
    }
    else if ((lrc->x <= ulc->x) && (lrc->y >= ulc->y))
    {
        for (mid_x = ulc->x; mid_x >= lrc->x; mid_x--)
        {
            cor_bit_point = 0;
            
            for (mid_y = ulc->y; mid_y <= lrc->y; mid_y++)
            {
                // Get color.
                mid_col = (*color) & (0x01 << cor_bit_point);
                // color byte jump.
                cor_bit_point++;
                if (cor_bit_point == 8) 
                {
                    cor_bit_point = 0;
                    color++;
                }

                // write color.
                if (mid_col == 0) {
                    _inline_lcd_set_pixel(mid_x, mid_y, _COLOR_WHITE);
                }
                else {
                    _inline_lcd_set_pixel(mid_x, mid_y, _COLOR_BLACK);
                }
            }

            if (cor_bit_point != 0) {
                color++;
            }
        }
    }
    else if ((lrc->x <= ulc->x) && (lrc->y <= ulc->y))
    {
        for (mid_y = ulc->y; mid_y >= lrc->y; mid_y--)
        {
            cor_bit_point = 0;
            
            for (mid_x = ulc->x; mid_x >= lrc->x; mid_x--)
            {
                // Get color.
                mid_col = (*color) & (0x01 << cor_bit_point);
                // color byte jump.
                cor_bit_point++;
                if (cor_bit_point == 8) 
                {
                    cor_bit_point = 0;
                    color++;
                }

                // write color.
                if (mid_col == 0) {
                    _inline_lcd_set_pixel(mid_x, mid_y, _COLOR_WHITE);
                }
                else {
                    _inline_lcd_set_pixel(mid_x, mid_y, _COLOR_BLACK);
                }
            }

            if (cor_bit_point != 0) {
                color++;
            }
        }
    }
    else if ((lrc->x >= ulc->x) && (lrc->y <= ulc->y))
    {
        for (mid_x = ulc->x; mid_x <= lrc->x; mid_x++)
        {
            cor_bit_point = 0;
            
            for (mid_y = ulc->y; mid_y >= lrc->y; mid_y--)
            {
                // Get color.
                mid_col = (*color) & (0x01 << cor_bit_point);
                // color byte jump.
                cor_bit_point++;
                if (cor_bit_point == 8) 
                {
                    cor_bit_point = 0;
                    color++;
                }

                // write color.
                if (mid_col == 0) {
                    _inline_lcd_set_pixel(mid_x, mid_y, _COLOR_WHITE);
                }
                else {
                    _inline_lcd_set_pixel(mid_x, mid_y, _COLOR_BLACK);
                }
            }

            if (cor_bit_point != 0) {
                color++;
            }
        }
    }
    else
    {}  
}

/*----------------------------------------------------------------------
 *  pLcdWritePixelsEx
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    !! Function use system forground as the color.
 *              color is a byte type, (8-bit --- 8 pixels)
 *              0: background, 1: forground.
 *---------------------------------------------------------------------*/
void pLcdWritePixelsEx(CursorT *ulc, CursorT *lrc, ColorT *color, gu8 rev)
{
    CoorSignT mid_x, mid_y;
    ColorT mid_col;
    gu8 cor_bit_point;
    
    // Write block data.
    if ((lrc->x >= ulc->x) && (lrc->y >= ulc->y))
    {
        for (mid_y = ulc->y; mid_y <= lrc->y; mid_y++)
        {
            cor_bit_point = 0;
            
            for (mid_x = ulc->x; mid_x <= lrc->x; mid_x++)
            {
                // Get color.
                mid_col = (*color) & (0x01 << cor_bit_point);
                // color byte jump.
                cor_bit_point++;
                if (cor_bit_point == 8) 
                {
                    cor_bit_point = 0;
                    color++;
                }

                // write color.
                if (((mid_col == 0) && (rev == 0)) || ((mid_col != 0) && (rev == 1))) 
                {
                    _inline_lcd_set_pixel(mid_x, mid_y, _COLOR_WHITE);
                }
                else {
                    _inline_lcd_set_pixel(mid_x, mid_y, _COLOR_BLACK);
                }
            }
            if (cor_bit_point != 0) {
                color++;
            }
        }
    }
    else if ((lrc->x <= ulc->x) && (lrc->y >= ulc->y))
    {
        for (mid_x = ulc->x; mid_x >= lrc->x; mid_x--)
        {
            cor_bit_point = 0;
            
            for (mid_y = ulc->y; mid_y <= lrc->y; mid_y++)
            {
                // Get color.
                mid_col = (*color) & (0x01 << cor_bit_point);
                // color byte jump.
                cor_bit_point++;
                if (cor_bit_point == 8) 
                {
                    cor_bit_point = 0;
                    color++;
                }

                // write color.
                if (((mid_col == 0) && (rev == 0)) || ((mid_col != 0) && (rev == 1))) 
                {
                    _inline_lcd_set_pixel(mid_x, mid_y, _COLOR_WHITE); 
                }
                else {
                    _inline_lcd_set_pixel(mid_x, mid_y, _COLOR_BLACK); 
                }
            }

            if (cor_bit_point != 0) {
                color++;
            }
        }
    }
    else if ((lrc->x <= ulc->x) && (lrc->y <= ulc->y))
    {
        for (mid_y = ulc->y; mid_y >= lrc->y; mid_y--)
        {
            cor_bit_point = 0;
            
            for (mid_x = ulc->x; mid_x >= lrc->x; mid_x--)
            {
                // Get color.
                mid_col = (*color) & (0x01 << cor_bit_point);
                // color byte jump.
                cor_bit_point++;
                if (cor_bit_point == 8) 
                {
                    cor_bit_point = 0;
                    color++;
                }

                // write color.
                if (((mid_col == 0) && (rev == 0)) || ((mid_col != 0) && (rev == 1))) 
                {
                    _inline_lcd_set_pixel(mid_x, mid_y, _COLOR_WHITE);
                }
                else {
                    _inline_lcd_set_pixel(mid_x, mid_y, _COLOR_BLACK);
                }
            }

            if (cor_bit_point != 0) {
                color++;
            }
        }
    }
    else if ((lrc->x >= ulc->x) && (lrc->y <= ulc->y))
    {
        for (mid_x = ulc->x; mid_x <= lrc->x; mid_x++)
        {
            cor_bit_point = 0;
            
            for (mid_y = ulc->y; mid_y >= lrc->y; mid_y--)
            {
                // Get color.
                mid_col = (*color) & (0x01 << cor_bit_point);
                // color byte jump.
                cor_bit_point++;
                if (cor_bit_point == 8) 
                {
                    cor_bit_point = 0;
                    color++;
                }

                // write color.
                if (((mid_col == 0) && (rev == 0)) || ((mid_col != 0) && (rev == 1))) 
                {
                    _inline_lcd_set_pixel(mid_x, mid_y, _COLOR_WHITE);
                }
                else {
                    _inline_lcd_set_pixel(mid_x, mid_y, _COLOR_BLACK);
                }
            }

            if (cor_bit_point != 0) {
                color++;
            }
        }
    }
    else
    {}  
}

/*----------------------------------------------------------------------
 *  pLcdFillPixels
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void pLcdFillPixels(CursorT *ulc, CursorT *lrc, ColorT color)
{
    CoorSignT mid_x, mid_y;

    // Write block data.
    if ((lrc->x >= ulc->x) && (lrc->y >= ulc->y))
    {
        for (mid_y = ulc->y; mid_y <= lrc->y; mid_y++)
        {   
            for (mid_x = ulc->x; mid_x <= lrc->x; mid_x++)
            {                
                _inline_lcd_set_pixel(mid_x, mid_y, color);        
            }
        }
    }
    else if ((lrc->x <= ulc->x) && (lrc->y >= ulc->y))
    {
        for (mid_x = ulc->x; mid_x >= lrc->x; mid_x--)
        {
            for (mid_y = ulc->y; mid_y <= lrc->y; mid_y++)
            {
                _inline_lcd_set_pixel(mid_x, mid_y, color);    
            }
        }
    }
    else if ((lrc->x <= ulc->x) && (lrc->y <= ulc->y))
    {
        for (mid_y = ulc->y; mid_y >= lrc->y; mid_y--)
        {
            for (mid_x = ulc->x; mid_x >= lrc->x; mid_x--)
            {
                _inline_lcd_set_pixel(mid_x, mid_y, color);     
            }
        }
    }
    else if ((lrc->x >= ulc->x) && (lrc->y <= ulc->y))
    {
        for (mid_x = ulc->x; mid_x <= lrc->x; mid_x++)
        {   
            for (mid_y = ulc->y; mid_y >= lrc->y; mid_y--)
            {
                _inline_lcd_set_pixel(mid_x, mid_y, color);      
            }
        }
    }
    else
    {}    
}

/*----------------------------------------------------------------------
 *  pLcdRevPixels
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void pLcdRevPixels(CursorT *ulc, CursorT *lrc, gu8 rev)
{
    CoorSignT mid_x, mid_y;

    // Check Rev
    if (rev == 0) 
    { 
        return; 
    }
    
    // Write block data.
    if ((lrc->x >= ulc->x) && (lrc->y >= ulc->y))
    {
        for (mid_y = ulc->y; mid_y <= lrc->y; mid_y++)
        {                        
            for (mid_x = ulc->x; mid_x <= lrc->x; mid_x++)
            {                
                _inline_lcd_rev_pixel(mid_x, mid_y);
            }            
        }
    }
    else if ((lrc->x <= ulc->x) && (lrc->y >= ulc->y))
    {
        for (mid_x = ulc->x; mid_x >= lrc->x; mid_x--)
        {            
            for (mid_y = ulc->y; mid_y <= lrc->y; mid_y++)
            {
                _inline_lcd_rev_pixel(mid_x, mid_y);
            }
        }
    }
    else if ((lrc->x <= ulc->x) && (lrc->y <= ulc->y))
    {
        for (mid_y = ulc->y; mid_y >= lrc->y; mid_y--)
        {           
            for (mid_x = ulc->x; mid_x >= lrc->x; mid_x--)
            {
                _inline_lcd_rev_pixel(mid_x, mid_y);
            }
        }
    }
    else if ((lrc->x >= ulc->x) && (lrc->y <= ulc->y))
    {
        for (mid_x = ulc->x; mid_x <= lrc->x; mid_x++)
        {           
            for (mid_y = ulc->y; mid_y >= lrc->y; mid_y--)
            {
                _inline_lcd_rev_pixel(mid_x, mid_y);
            }
        }
    }
    else
    {}  
}

/*----------------------------------------------------------------------
 *  pLcdReadPixels
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void pLcdReadPixels(CursorT *ulc, CursorT *lrc, ColorT *color)
{

}

/*----------------------------------------------------------------------
 *  pLcdInit
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void pLcdInit(fpInit fp_init, fpWriteFrame fp_write_fme, fpSetConstrast fp_set_cst)
{       

    // Init Hw
    if (fp_init != 0)
    {
        (*fp_init)();
    }

    FpWriteFrame = fp_write_fme;
    FpSetConstrast = fp_set_cst;
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
        (*FpWriteFrame)((unsigned char *)FrameRam, _FRAME_RAM_SIZE_X * _FRAME_RAM_SIZE_Y / 8);
    }
}


