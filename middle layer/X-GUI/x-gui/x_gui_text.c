
/*----------------------------------------------------------------------^^-
/ File name:  x_gui_text.c
/ Author:     JiangJun
/ Data:       2018/8/19
/ Version:    v1.1
/-----------------------------------------------------------------------^^-
/ X-GUI Text Driver
/------------------------------------------------------------------------*/

#include "x_gui_text.h"
#include "x_gui_font.h"
#include "x_gui_2d.h"

//------------------------------------------------------------
//              Static Var
//------------------------------------------------------------

static gu8 TextSize = _TEXT_OUT_DEFAULT_SIZE;


/*----------------------------------------------------------------------
 *  TextSizeSet
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void TextSizeSet(gu8 size)
{
    TextSize = size;
}

/*----------------------------------------------------------------------
 *  TextEcode
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void TextEcode(const gu8 *text, gu8 *lan, gu8 *size)
{

    // ASCII
    if ((*text > 0) && (*text <= 0x007F))
    {
        *lan = _FONT_LANGUAGE_ENGLISH; *size = 1;
    }
    else if ((*text >= 0xB0) && (*text <= 0xF7) && (*(text + 1) >= 0xA1) && (*(text + 1) <= 0xFE))      // Chinese
    {
        *lan = _FONT_LANGUAGE_CHINESE; *size = 2;
    }
    else if (((*text == 0xA7) || (*text == 0xA1)) && (*(text + 1) >= 0xA1) && (*(text + 1) <= 0xFE))    // Russian
    {
        *lan = _FONT_LANGUAGE_RUSSIAN; *size = 2;
    }
    else
    {
        *lan = _FONT_LANGUAGE_ENGLISH; *size = 1;   // DEFAULT Language and Size
    }
}

/*----------------------------------------------------------------------
 *  TextGetX_MaxSize
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
gu16 TextGetX_MaxSize(const gu8 *text)
{

    gu16 x_max_size = 0; gu8 char_lan = 0, char_size = 0;
    const gu8 *o_char = text;
    
    //------------------------------------------------------------
    //              Get the Max X-Size
    //------------------------------------------------------------
    for (; *o_char != '\0'; o_char += char_size)
    {

        // Get the Language and Size
        TextEcode(o_char, &char_lan, &char_size);

        // Get the char X-Size
        x_max_size += FontLibGetMask_X_Size(char_lan, TextSize, o_char);
    }

    return x_max_size;
}

/*----------------------------------------------------------------------
 *  TextGetY_MaxSize
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
gu16 TextGetY_MaxSize(const gu8 *text)
{

    gu16 y_max_size = 0; gu8 char_lan = 0, char_size = 0;
    const gu8 *o_char = text;
    
    //------------------------------------------------------------
    //              Get the Max X-Size
    //------------------------------------------------------------
    for (; *o_char != '\0'; o_char += char_size)
    {

        // Get the Language and Size
        TextEcode(o_char, &char_lan, &char_size);

        // Get the char X-Size
        if (FontLibGetFont_Y_Size(char_lan, TextSize) > y_max_size)
        {
        
            y_max_size = FontLibGetFont_Y_Size(char_lan, TextSize);
        }        
    }

    return y_max_size;
}

/*----------------------------------------------------------------------
 *  TextOut
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void TextOut(gu16 x_start, gu16 y_start, const gu8 *text)
{

    FontBmpT bmp; xCursorT ulr, lrc; gu8 char_lan = 0, char_size = 0;
    const gu8 *o_char = text;


    //------------------------------------------------------------
    //              CHECK IF Out-Range
    //------------------------------------------------------------
    
    if ((x_start >= _LCD_X_SIZE) || (y_start >= _LCD_Y_SIZE)) 
    {
        return;
    }

    //------------------------------------------------------------
    //              Text Output
    //------------------------------------------------------------

    ulr.x = x_start; ulr.y = y_start;

    for (; *o_char != '\0'; o_char += char_size)
    {

        // Get the Language and Size
        TextEcode(o_char, &char_lan, &char_size);

        // \n Next Line
        if ((char_lan == _FONT_LANGUAGE_ENGLISH) && (char_size == 1))
        {

            if (*o_char == '\n')
            {
                ulr.x = x_start; ulr.y += FontLibGetFont_Y_Size(char_lan, TextSize);

                continue;
            }
        }

        // Get Mask
        if (FontLibGetMask(char_lan, TextSize, o_char, &bmp) != 0)
        {
            lrc.x = ulr.x + bmp.length - 1; lrc.y = ulr.y + bmp.width - 1;            
        }
        else
        {
            return;
        }

        //-------------------------------------------------------
        //              Output to LCD
        //-------------------------------------------------------

        // Out-X
        if (lrc.x >= _LCD_X_SIZE)
        {            
            ulr.x = x_start; ulr.y += bmp.width;            
            lrc.x = ulr.x + bmp.length - 1; lrc.y = ulr.y + bmp.width - 1;            
        }

        // Out-Y
        if (lrc.y >= _LCD_Y_SIZE)
        {
            return;
        }

        // To LCD
        pLcdWritePixels(&ulr, &lrc, (gu8*)bmp.image);

        // X-Add
        ulr.x += bmp.length;
    }    
}

/*----------------------------------------------------------------------
 *  TextOutExRev
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void TextOutExRev(gu16 x_start, gu16 y_start, const gu8 *text, gu8 rev)
{

    FontBmpT bmp; xCursorT ulr, lrc; gu8 char_lan = 0, char_size = 0;
    const gu8 *o_char = text;


    //------------------------------------------------------------
    //              CHECK IF Out-Range
    //------------------------------------------------------------
    
    if ((x_start >= _LCD_X_SIZE) || (y_start >= _LCD_Y_SIZE)) 
    {
        return;
    }

    //------------------------------------------------------------
    //              Text Output
    //------------------------------------------------------------

    ulr.x = x_start; ulr.y = y_start;

    for (; *o_char != '\0'; o_char += char_size)
    {

        // Get the Language and Size
        TextEcode(o_char, &char_lan, &char_size);

        // \n Next Line
        if ((char_lan == _FONT_LANGUAGE_ENGLISH) && (char_size == 1))
        {

            if (*o_char == '\n')
            {
                ulr.x = x_start; ulr.y += FontLibGetFont_Y_Size(char_lan, TextSize);

                continue;
            }
        }

        // Get Mask
        if (FontLibGetMask(char_lan, TextSize, o_char, &bmp) != 0)
        {
            lrc.x = ulr.x + bmp.length - 1; lrc.y = ulr.y + bmp.width - 1;            
        }
        else
        {
            return;
        }

        //-------------------------------------------------------
        //              Output to LCD
        //-------------------------------------------------------

        // Out-X
        if (lrc.x >= _LCD_X_SIZE)
        {            
            ulr.x = x_start; ulr.y += bmp.width;            
            lrc.x = ulr.x + bmp.length - 1; lrc.y = ulr.y + bmp.width - 1;            
        }

        // Out-Y
        if (lrc.y >= _LCD_Y_SIZE)
        {
            return;
        }

        // To LCD
        pLcdWritePixelsEx(&ulr, &lrc, (gu8*)bmp.image, rev & _TEXT_REV_DIS);

        // X-Add
        ulr.x += bmp.length;
    }    
}

/*----------------------------------------------------------------------
 *  TextOutExRetnle
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void TextOutExRetnle(xCursorT *ulr, const gu8 *text, gu8 x_side, gu8 y_side, gu8 rev)
{

    gu16 y_max_size = 0, x_max_size = 0; gu8 char_lan = 0, char_size = 0;
    const gu8 *o_char = text; xCursorT ulr_s = *ulr, lrc_e;

    
    //------------------------------------------------------------
    //              Get the Max X/Y-Size
    //------------------------------------------------------------

    // Get the Language and Size
    TextEcode(text, &char_lan, &char_size);

    // Get the Max Y-Size
    y_max_size = FontLibGetFont_Y_Size(char_lan, TextSize);

    // Get the Max X-Size
    x_max_size = TextGetX_MaxSize(o_char);    


    //------------------------------------------------------------
    //              CHECK IF Out-X/Y
    //------------------------------------------------------------

    lrc_e.x = ulr_s.x + x_max_size + (x_side << 1) - 1; lrc_e.y = ulr_s.y + y_max_size + (y_side << 1) - 1;
    
    // Out-X
    if (lrc_e.x >= _LCD_X_SIZE)
    {
        return;
    }

    // Out-Y
    if (lrc_e.y >= _LCD_Y_SIZE)
    {
        return;
    }


    //------------------------------------------------------------
    //              To LCD
    //------------------------------------------------------------
    
    if ((rev & _TEXT_REV_DIS) == 0)
    {

        TextOutExRev(ulr_s.x + x_side, ulr_s.y + y_side, text, 0);        
        FigDrawRectangle(&ulr_s, &lrc_e, _COLOR_BLACK);
    }
    else
    {

        FigFillRectangle(&ulr_s, &lrc_e, _COLOR_BLACK);
        TextOutExRev(ulr_s.x + x_side, ulr_s.y + y_side, text, 1);  
    }    
}

/*----------------------------------------------------------------------
 *  TextOutExRndFrame
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void TextOutExRndFrame(xCursorT *ulr, const gu8 *text, gu8 x_side, gu8 y_side, gu8 r, gu8 rev)
{

    gu16 y_max_size = 0, x_max_size = 0; gu8 char_lan = 0, char_size = 0;
    const gu8 *o_char = text; xCursorT ulr_s = *ulr, lrc_e;

    
    //------------------------------------------------------------
    //              Get the Max X/Y-Size
    //------------------------------------------------------------

    // Get the Language and Size
    TextEcode(text, &char_lan, &char_size);

    // Get the Max Y-Size
    y_max_size = FontLibGetFont_Y_Size(char_lan, TextSize);

    // Get the Max X-Size
    x_max_size = TextGetX_MaxSize(o_char);    


    //------------------------------------------------------------
    //              CHECK IF Out-X/Y
    //------------------------------------------------------------

    lrc_e.x = ulr_s.x + x_max_size + (x_side << 1) - 1; lrc_e.y = ulr_s.y + y_max_size + (y_side << 1) - 1;
    
    // Out-X
    if (lrc_e.x >= _LCD_X_SIZE)
    {
        return;
    }

    // Out-Y
    if (lrc_e.y >= _LCD_Y_SIZE)
    {
        return;
    }


    //------------------------------------------------------------
    //              To LCD
    //------------------------------------------------------------
    
    if ((rev & _TEXT_REV_DIS) == 0)
    {

        TextOutExRev(ulr_s.x + x_side, ulr_s.y + y_side, text, 0);        
        FigDrawRoundFrame(&ulr_s, &lrc_e, r, _COLOR_BLACK);
    }
    else
    {

        FigFillRoundFrame(&ulr_s, &lrc_e, r, _COLOR_BLACK);
        TextOutExRev(ulr_s.x + x_side, ulr_s.y + y_side, text, 1);  
    }    
}

/*----------------------------------------------------------------------
 *  TextOutExAlign
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void TextOutExAlign(xCursorT *ulr, const gu8 *text, gu16 x_len, gu8 y_side_offset, gu8 func)
{

    gu16 y_max_size = 0, x_max_size = 0; gu8 char_lan = 0, char_size = 0;
    const gu8 *o_char = text; xCursorT ulr_s = *ulr, lrc_e; gu16 x_side = 0, y_side = _TEXT_OUT_ALIGN_Y_SIDE_DEFAULT;

    //------------------------------------------------------------
    //              Get the Max X/Y-Size
    //------------------------------------------------------------

    // Get the Language and Size
    TextEcode(text, &char_lan, &char_size);

    // Get the Max Y-Size
    y_max_size = FontLibGetFont_Y_Size(char_lan, TextSize);

    // Get the Max X-Size
    x_max_size = TextGetX_MaxSize(o_char); 

    // Check Range
    if (x_len < x_max_size)
    {
        return;
    }
    
    //------------------------------------------------------------
    //              CHECK IF Out-X/Y
    //------------------------------------------------------------

    lrc_e.x = ulr_s.x + x_len - 1; lrc_e.y = ulr_s.y + y_max_size + (y_side << 1) - 1;

    // Out-X
    if (lrc_e.x >= _LCD_X_SIZE)
    {
        return;
    }

    // Out-Y
    if (lrc_e.y >= _LCD_Y_SIZE)
    {
        return;
    }


    //------------------------------------------------------------
    //              To LCD
    //------------------------------------------------------------
    
    if (func & _TEXT_LEFT_ALIGN)
    {
        x_side = _TEXT_OUT_ALIGN_X_SIDE_DEFAULT;
    }
    else if (func & _TEXT_MIDDLE_ALIGN)
    {
        x_side = (x_len - x_max_size) >> 1;
    }
    else
    {
    
        //-------------------------------------------------------
        //              DEFAULT ALIGN ACTION
        //-------------------------------------------------------
        
#if _TEXT_OUT_ALIGN_DEF_ACTION == _TEXT_LEFT_ALIGN

        x_side = _TEXT_OUT_ALIGN_X_SIDE_DEFAULT;
        
#elif _TEXT_OUT_ALIGN_DEF_ACTION == _TEXT_MIDDLE_ALIGN

        x_side = (x_len - x_max_size) >> 1;
#endif

    }

    if ((func & _TEXT_REV_DIS) == 0)
    {   
    
        if ((func & _TEXT_NO_RETNLE) == 0)
        {
        
            // Retangle Output
            
            TextOutExRev(ulr_s.x + x_side, ulr_s.y + y_side + y_side_offset, text, 0);     
            FigDrawRectangle(&ulr_s, &lrc_e, _COLOR_BLACK);
        }
        else
        {

            // No Retangle Output
            TextOutExRev(ulr_s.x + x_side, ulr_s.y + y_side_offset, text, 0);     
        }
    }
    else
    {

        if ((func & _TEXT_NO_RETNLE) == 0)
        {
        
            // Retangle Output

            FigFillRectangle(&ulr_s, &lrc_e, _COLOR_BLACK);
            TextOutExRev(ulr_s.x + x_side, ulr_s.y + y_side + y_side_offset, text, 1);  
        }
        else
        {

            // No Retangle Output
            TextOutExRev(ulr_s.x + x_side, ulr_s.y + y_side_offset, text, 1);  
        }
    }    
}


