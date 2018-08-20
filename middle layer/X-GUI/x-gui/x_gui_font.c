
/*----------------------------------------------------------------------^^-
/ File name:  x_gui_font.c
/ Author:     JiangJun
/ Data:       2018/5/3
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ Font Driver
/------------------------------------------------------------------------*/

#include "x_gui_font.h"

//------------------------------------------------------------
// Font mask table.
//------------------------------------------------------------
static FontT Font;


/*----------------------------------------------------------------------
 *  FontLibInstall
 *
 *  Purpose: Install FontMaskT point to Font table.
 *
 *  Entry:   lan - font mask language specfiy.
 *           mask_size - specify the size with the mask data. so which 
 *                       size is which mask is define by user. such as
 *                       you can define the fms_0_size with ASCII 8X8.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void FontLibInstall(gu8 lan, gu8 mask_size, const FontMaskT *mask)
{
    Font.font_mask[lan][mask_size] = mask;
}

/*----------------------------------------------------------------------
 *  FontLibGetMask(Not support unicode)
 *
 *  Purpose: Get the mask data with the default mask size.
 *
 *  Entry:   cod - not unicode, can be GB2312 or ascii and so on.
 *
 *  Exit:    bmp_p - BmpT type point, refer BmpT for details.
 *           gtrue or false(not found in the default mask size lib)
 *
 *  NOTE:    Use the default Size Mask
 *---------------------------------------------------------------------*/
gu8 FontLibGetMask(gu8 lan, gu8 mask_size, const gu8 *cod, FontBmpT *bmp)
{

    const gu8 *word_addr; gu16 idx = 0;
    
    //------------------------------------------------------------
    //              CHECK IF Install Font
    //------------------------------------------------------------
    
    if (Font.font_mask[lan][mask_size] == 0) 
    {
        
        bmp->length = 0; bmp->width = 0;                        
    }
    else
    {

        // Search for the Code
        for (idx = 0; idx < Font.font_mask[lan][mask_size]->word_cnt; idx++)
        {

            // Get the Word Address
            word_addr = Font.font_mask[lan][mask_size]->mask + (idx * Font.font_mask[lan][mask_size]->word_size);

            // Compare Code
            if (*word_addr == *cod)
            {
            
                if (Font.font_mask[lan][mask_size]->code_size == 2)
                {                    
                    if (*(word_addr + 1) != *(cod + 1))
                    {
                        continue;
                    }
                }

                // OK to Find                           
                bmp->length = *(word_addr + Font.font_mask[lan][mask_size]->code_size);
                bmp->width = *(word_addr + Font.font_mask[lan][mask_size]->code_size + 1);     

                bmp->image = word_addr + Font.font_mask[lan][mask_size]->code_size + 2;
                
                // Return the Code Size of the word
                return Font.font_mask[lan][mask_size]->code_size;
            }            
        }
    }

    bmp->length = 0; bmp->width = 0;
    return 0;
}

/*----------------------------------------------------------------------
 *  FontLibGetFont_Y_Size
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
gu8 FontLibGetFont_Y_Size(gu8 lan, gu8 mask_size)
{

    //------------------------------------------------------------
    //              CHECK IF Install Font
    //------------------------------------------------------------
    
    if (Font.font_mask[lan][mask_size] == 0) 
    {
        
        return 0;                 
    }
    
    return Font.font_mask[lan][mask_size]->y_max_size;
}

/*----------------------------------------------------------------------
 *  FontLibGetMask_X_Size
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
gu8 FontLibGetMask_X_Size(gu8 lan, gu8 mask_size, const gu8 *cod)
{

    const gu8 *word_addr; gu16 idx = 0;
    
    //------------------------------------------------------------
    //              CHECK IF Install Font
    //------------------------------------------------------------
    
    if (Font.font_mask[lan][mask_size] == 0) 
    {
        
        return 0;                  
    }
    else
    {

        // Search for the Code
        for (idx = 0; idx < Font.font_mask[lan][mask_size]->word_cnt; idx++)
        {

            // Get the Word Address
            word_addr = Font.font_mask[lan][mask_size]->mask + (idx * Font.font_mask[lan][mask_size]->word_size);

            // Compare Code
            if (*word_addr == *cod)
            {
            
                if (Font.font_mask[lan][mask_size]->code_size == 2)
                {                    
                    if (*(word_addr + 1) != *(cod + 1))
                    {
                        continue;
                    }
                }

                // OK to Find                                           
                return (*(word_addr + Font.font_mask[lan][mask_size]->code_size));
            }            
        }
    }

    return 0;
}