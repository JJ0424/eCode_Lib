
/*----------------------------------------------------------------------^^-
/ File name:  frame_ram.c
/ Author:     JiangJun
/ Data:       2018/2/11
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ Mono Mode Frame RAM Driver
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
 *  pLcdSetPixel
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void pLcdSetPixel(CoorT x, CoorT y, ColorT color)
{
    FrameRam[y / 8][x] &= ~(0x01 << (y % 8));
    FrameRam[y / 8][x] |= ((gu8)color << (y % 8));
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
                    FrameRam[mid_y / 8][mid_x] &= ~(0x01 << (mid_y % 8));
                    FrameRam[mid_y / 8][mid_x] |= ((gu8)_COLOR_WHITE << (mid_y % 8));  
                }
                else {
                    FrameRam[mid_y / 8][mid_x] &= ~(0x01 << (mid_y % 8));
                    FrameRam[mid_y / 8][mid_x] |= ((gu8)_COLOR_BLACK << (mid_y % 8)); 
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
                    FrameRam[mid_y / 8][mid_x] &= ~(0x01 << (mid_y % 8));
                    FrameRam[mid_y / 8][mid_x] |= ((gu8)_COLOR_WHITE << (mid_y % 8));  
                }
                else {
                    FrameRam[mid_y / 8][mid_x] &= ~(0x01 << (mid_y % 8));
                    FrameRam[mid_y / 8][mid_x] |= ((gu8)_COLOR_BLACK << (mid_y % 8)); 
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
                    FrameRam[mid_y / 8][mid_x] &= ~(0x01 << (mid_y % 8));
                    FrameRam[mid_y / 8][mid_x] |= ((gu8)_COLOR_WHITE << (mid_y % 8));  
                }
                else {
                    FrameRam[mid_y / 8][mid_x] &= ~(0x01 << (mid_y % 8));
                    FrameRam[mid_y / 8][mid_x] |= ((gu8)_COLOR_BLACK << (mid_y % 8)); 
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
                    FrameRam[mid_y / 8][mid_x] &= ~(0x01 << (mid_y % 8));
                    FrameRam[mid_y / 8][mid_x] |= ((gu8)_COLOR_WHITE << (mid_y % 8));  
                }
                else {
                    FrameRam[mid_y / 8][mid_x] &= ~(0x01 << (mid_y % 8));
                    FrameRam[mid_y / 8][mid_x] |= ((gu8)_COLOR_BLACK << (mid_y % 8)); 
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
                FrameRam[mid_y / 8][mid_x] &= ~(0x01 << (mid_y % 8));
                FrameRam[mid_y / 8][mid_x] |= ((gu8)color << (mid_y % 8));               
            }
        }
    }
    else if ((lrc->x <= ulc->x) && (lrc->y >= ulc->y))
    {
        for (mid_x = ulc->x; mid_x >= lrc->x; mid_x--)
        {
            for (mid_y = ulc->y; mid_y <= lrc->y; mid_y++)
            {
                FrameRam[mid_y / 8][mid_x] &= ~(0x01 << (mid_y % 8));
                FrameRam[mid_y / 8][mid_x] |= ((gu8)color << (mid_y % 8));    
            }
        }
    }
    else if ((lrc->x <= ulc->x) && (lrc->y <= ulc->y))
    {
        for (mid_y = ulc->y; mid_y >= lrc->y; mid_y--)
        {
            for (mid_x = ulc->x; mid_x >= lrc->x; mid_x--)
            {
                FrameRam[mid_y / 8][mid_x] &= ~(0x01 << (mid_y % 8));
                FrameRam[mid_y / 8][mid_x] |= ((gu8)color << (mid_y % 8));  
            }
        }
    }
    else if ((lrc->x >= ulc->x) && (lrc->y <= ulc->y))
    {
        for (mid_x = ulc->x; mid_x <= lrc->x; mid_x++)
        {   
            for (mid_y = ulc->y; mid_y >= lrc->y; mid_y--)
            {
                FrameRam[mid_y / 8][mid_x] &= ~(0x01 << (mid_y % 8));
                FrameRam[mid_y / 8][mid_x] |= ((gu8)color << (mid_y % 8));    
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


