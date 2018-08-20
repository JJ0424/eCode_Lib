
/*----------------------------------------------------------------------^^-
/ File name:  x_gui_font.h
/ Author:     JiangJun
/ Data:       2018/5/3
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ Font Driver
/------------------------------------------------------------------------*/

#ifndef _X_GUI_FONT_H
#define _X_GUI_FONT_H

// Port File
#include "lcd_driver.h"

//------------------------------------------------------------
//              CONFIG
//------------------------------------------------------------

// Font Language:  0 - (FONT_LANGUGE_SUM - 1)
#define FONT_LANGUGE_SUM        10

// Font Size: 0 - (FONT_SIZE_SUM - 1), 0 is the Minium Size
#define FONT_SIZE_SUM           5


//------------------------------------------------------------
//              Typedef Struct
//------------------------------------------------------------

//------------------------------------------------------------
// FontT data struct.
// ------
// font is managet by language and size.
//------------------------------------------------------------

// font mask data.
typedef struct {

    gu8 y_max_size;     // the Max Y-Size of the Font Mask
    gu8 monospaced;     // monospaced font.
    gu8 code_size;      // 1/2 bytes
    gu16 word_size;      // bytes numbers of each word.
    gu16 word_cnt;      // the count of the font
    
    const gu8 *mask;    // mask data(bit stream and code)
    
} FontMaskT;

// FontT
typedef struct {

    gu8 font_lan_cnt;
    const FontMaskT *font_mask[FONT_LANGUGE_SUM][FONT_SIZE_SUM]; 
} FontT;

// Output Bmp
typedef struct
{
   gu8 length;              // horization resolutions.
   gu8 width;               // vertical resolutions.
   const gu8 *image;        // mask data(not contain code).
   
} FontBmpT;

extern void FontLibInstall(gu8 lan, gu8 mask_size, const FontMaskT *mask);
extern gu8 FontLibGetMask(gu8 lan, gu8 mask_size, const gu8 *cod, FontBmpT *bmp);
extern gu8 FontLibGetFont_Y_Size(gu8 lan, gu8 mask_size);
extern gu8 FontLibGetMask_X_Size(gu8 lan, gu8 mask_size, const gu8 *cod);

#endif

