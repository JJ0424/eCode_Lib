
/*----------------------------------------------------------------------^^-
/ File name:  x_gui_com.c
/ Author:     JiangJun
/ Data:       2018/6/12
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ X-GUI Common Functions
/------------------------------------------------------------------------*/

#include "x_gui_com.h"


//------------------------------------------------------------
//              图片字模数组
//------------------------------------------------------------

const gu8 Img_People_Left_9x16_Mask[] = 
{
    0x00,0x00,0x7C,0x00,0x44,0x00,0x54,0x00,0x44,0x00,0x7C,0x00,0x10,0x00,0x10,0x00,
    0xF0,0x01,0x18,0x00,0x14,0x00,0x12,0x00,0x10,0x00,0x28,0x00,0x44,0x00,0x82,0x00,
};

_LayerBmpT Img_People_Left_9x16 = {9, 16, Img_People_Left_9x16_Mask};

const gu8 Img_People_Right_9x16_Mask[] = 
{
    0x00,0x00,0x7C,0x00,0x44,0x00,0x54,0x00,0x44,0x00,0x7C,0x00,0x10,0x01,0x90,0x00,
    0x70,0x00,0x10,0x00,0x1F,0x00,0x10,0x00,0x10,0x00,0x28,0x00,0x44,0x00,0x82,0x00,
};

_LayerBmpT Img_People_Right_9x16 = {9, 16, Img_People_Right_9x16_Mask};


/*----------------------------------------------------------------------
 *  XProgress_Fill - 进度条样式1
 *  ______________________________________
 * |______________________________________| 中间是填充式进度条
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void XProgress_Fill(gu16 x, gu16 y, gu16 x_size, gu16 y_size, gu8 percent)
{

    xCursorT ulr, lrc; gu16 f_x_size = 0;
    
    // 画轮廓
    ulr.x = x; ulr.y = y; lrc.x = ulr.x + x_size - 1; lrc.y = ulr.y + y_size - 1;    

    // 检查坐标合法性
    if ((lrc.x < _LCD_X_SIZE) && (lrc.y < _LCD_Y_SIZE))
    {
        FigDrawRectangle(&ulr, &lrc, _COLOR_BLACK); 
    }
    else
    {
        return;
    }
    
    // 计算填充的X长度
    f_x_size = ((percent % 101) * (x_size - 1)) / 100;
    if (f_x_size < 1) { f_x_size = 1; }
    
    // 画填充
    ulr.x = x; ulr.y = y; lrc.x = ulr.x + f_x_size - 1; lrc.y = ulr.y + y_size - 1; 
    FigFillRectangle(&ulr, &lrc, _COLOR_BLACK);
}

/*----------------------------------------------------------------------
 *  XProgress_Line - 进度条样式2
 *  ______________________________________
 * |______________________________________| 中间是条状式进度条，宽2个像素，间隔1个像素
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    函数会根据x_max_size变量重新计算x的长度，保证进度线条都是等宽的
 *           实际的X长度会小于等于x_max_size，最大差2个像素
 *---------------------------------------------------------------------*/
void XProgress_Line(gu16 x, gu16 y, gu16 x_max_size, gu16 y_size, gu8 percent)
{

    xCursorT ulr, lrc; gu16 x_size = 0, cnt = 0, idx = 0;

    // 重新计算x_size
    cnt = (x_max_size - 3) / 3;
    x_size = (cnt * 3) + 3;
    
    // 画轮廓
    ulr.x = x; ulr.y = y; lrc.x = ulr.x + x_size - 1; lrc.y = ulr.y + y_size - 1;         

    // 检查坐标合法性
    if ((lrc.x < _LCD_X_SIZE) && (lrc.y < _LCD_Y_SIZE))
    {
        FigDrawRectangle(&ulr, &lrc, _COLOR_BLACK); 
    }
    else
    {
        return;
    }
    
    // 计算填充的X长度
    cnt = ((percent % 101) * cnt) / 100;
    
    // 画填充
    ulr.y = y + 2; lrc.y = y + y_size - 3;
    for (idx = 0; idx < cnt; idx++)
    {                
        ulr.x = x + 2 + (idx * (2 + 1)); lrc.x = ulr.x + 2 - 1;          
        FigFillRectangle(&ulr, &lrc, _COLOR_BLACK);
    }
}

/*----------------------------------------------------------------------
 *  XProgress_Fill_Empty - 进度条样式3
 *  ______________________________________
 * |______________________________________| 中间是填充式进度条，上下左右有1个像素间隔
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void XProgress_Fill_Empty(gu16 x, gu16 y, gu16 x_size, gu16 y_size, gu8 percent)
{

    xCursorT ulr, lrc; gu16 f_x_size = 0;
    
    // 画轮廓
    ulr.x = x; ulr.y = y; lrc.x = ulr.x + x_size - 1; lrc.y = ulr.y + y_size - 1;         

    // 检查坐标合法性
    if ((lrc.x < _LCD_X_SIZE) && (lrc.y < _LCD_Y_SIZE))
    {
        FigDrawRectangle(&ulr, &lrc, _COLOR_BLACK); 
    }
    else
    {
        return;
    }
    
    // 计算填充的X长度
    f_x_size = ((percent % 101) * (x_size - 2)) / 100;
    if (f_x_size < 1) { f_x_size = 1; }
    
    // 画填充
    ulr.x = x; ulr.y = y + 2; lrc.x = ulr.x + f_x_size - 1; lrc.y = y + y_size - 3; 
    FigFillRectangle(&ulr, &lrc, _COLOR_BLACK);

    // 最左边删除掉一竖行像素
    FigDrawYLine(ulr.y, lrc.y, x + 1, _COLOR_WHITE);
}

/*----------------------------------------------------------------------
 *  XProgress_Fill_Flash - 特殊进度条样式，带火柴人，原型是样式1
 *  ______________________________________
 * |______________________________________| 中间是填充式进度条
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void XProgress_Fill_Flash(gu16 x, gu16 y, gu16 x_size, gu16 y_size, gu8 percent)
{

    xCursorT ulr, lrc; gu16 f_x_size = 0;
        
    // 画轮廓
    ulr.x = x; ulr.y = y + Img_People_Left_9x16.y_size + 1; lrc.x = ulr.x + x_size - 1; lrc.y = ulr.y + y_size - 1;         

    // 检查坐标合法性
    if ((lrc.x < _LCD_X_SIZE) && (lrc.y < _LCD_Y_SIZE))
    {
        FigDrawRectangle(&ulr, &lrc, _COLOR_BLACK); 
    }
    else
    {
        return;
    }
    
    // 计算填充的X长度
    f_x_size = ((percent % 101) * (x_size - 1)) / 100;
    if (f_x_size < 1) { f_x_size = 1; }
    
    // 画填充
    ulr.x = x; ulr.y = y + Img_People_Left_9x16.y_size + 1;; lrc.x = ulr.x + f_x_size - 1; lrc.y = ulr.y + y_size - 1; 
    FigFillRectangle(&ulr, &lrc, _COLOR_BLACK);

    // 画火柴人
    if (percent % 2)
    {
        LayerBmpOut(lrc.x, y, &Img_People_Left_9x16, 0);
    }
    else
    {
        LayerBmpOut(lrc.x, y, &Img_People_Right_9x16, 0);
    }
}

/*----------------------------------------------------------------------
 *  XProgress_Fill_R_Tile - 进度条样式4
 *  ______________________________________
 * |//____________________________________| 中间是斜度填充式进度条
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void XProgress_Fill_R_Tile(gu16 x, gu16 y, gu16 x_size, gu16 y_size, gu8 percent)
{

    xCursorT ulr, lrc; gu16 f_x_size = 0, idx = 0, cnt = 0;
    
    // 画轮廓
    ulr.x = x; ulr.y = y; lrc.x = ulr.x + x_size - 1; lrc.y = ulr.y + y_size - 1;         

    // 检查坐标合法性
    if ((lrc.x < _LCD_X_SIZE) && (lrc.y < _LCD_Y_SIZE))
    {
        FigDrawRectangle(&ulr, &lrc, _COLOR_BLACK); 
    }
    else
    {
        return;
    }
    
    // 计算填充的X长度
    f_x_size = ((percent % 101) * (x_size - 2)) / 100;
    if (f_x_size < 1) { f_x_size = 1; }
    
    // 画填充
    ulr.x = x; ulr.y = y; lrc.x = ulr.x + f_x_size - 1; lrc.y = ulr.y + y_size - 1; 
    FigFillRectangle(&ulr, &lrc, _COLOR_BLACK);
       
    
    //------------------------------------------------------------
    //              画三角倾斜
    //------------------------------------------------------------

    cnt = f_x_size - 1;

    // 中间处理
    if (cnt > (y_size - 2)) { cnt = y_size - 2; }
    
    for (idx = 0; idx < cnt; idx++)
    {
        // 结尾处理
        if ((lrc.x + cnt - idx) > (x + x_size - 1))
        {
            FigDrawXLine(lrc.x, x + x_size - 1, y + idx + 1, _COLOR_BLACK);
        }
        else
        {
            FigDrawXLine(lrc.x, lrc.x + cnt - idx, y + idx + 1, _COLOR_BLACK);
        }        
    }
}

/*----------------------------------------------------------------------
 *  XProgress_Fill_L_Tile - 进度条样式5
 *  _____________________________________
 * |\\___________________________________| 中间是斜度填充式进度条
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void XProgress_Fill_L_Tile(gu16 x, gu16 y, gu16 x_size, gu16 y_size, gu8 percent)
{

    xCursorT ulr, lrc; gu16 f_x_size = 0, idx = 0, cnt = 0;
    
    // 画轮廓
    ulr.x = x; ulr.y = y; lrc.x = ulr.x + x_size - 1; lrc.y = ulr.y + y_size - 1;         

    // 检查坐标合法性
    if ((lrc.x < _LCD_X_SIZE) && (lrc.y < _LCD_Y_SIZE))
    {
        FigDrawRectangle(&ulr, &lrc, _COLOR_BLACK); 
    }
    else
    {
        return;
    }
    
    // 计算填充的X长度
    f_x_size = ((percent % 101) * (x_size - 2)) / 100;
    if (f_x_size < 1) { f_x_size = 1; }
    
    // 画填充
    ulr.x = x; ulr.y = y; lrc.x = ulr.x + f_x_size - 1; lrc.y = ulr.y + y_size - 1; 
    FigFillRectangle(&ulr, &lrc, _COLOR_BLACK);
       
    
    //------------------------------------------------------------
    //              画三角倾斜
    //------------------------------------------------------------

    cnt = f_x_size - 1;

    // 中间处理
    if (cnt > (y_size - 2)) { cnt = y_size - 2; }
    
    for (idx = 0; idx < cnt; idx++)
    {
        // 结尾处理
        if ((lrc.x + cnt - idx) > (x + x_size - 1))
        {
            FigDrawXLine(lrc.x, x + x_size - 1, y + y_size - 2 - idx, _COLOR_BLACK);
        }
        else
        {
            FigDrawXLine(lrc.x, lrc.x + cnt - idx, y + y_size - 2 - idx, _COLOR_BLACK);
        }        
    }
}

/*----------------------------------------------------------------------
 *  XScrollBar_Fill - 滚动条样式1，固定宽度3像素
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    hor_vert 非0为垂直滚动条
 *---------------------------------------------------------------------*/
void XScrollBar_Fill(gu16 x, gu16 y, gu16 x_size, gu16 y_size, gu8 hor_vert, gu8 percent)
{

    xCursorT ulr, lrc; gu16 f_size = 0, size = 0;
    
    // 画轮廓
    ulr.x = x; ulr.y = y; lrc.x = ulr.x + x_size - 1; lrc.y = ulr.y + y_size - 1;         

    // 检查坐标合法性
    if ((lrc.x < _LCD_X_SIZE) && (lrc.y < _LCD_Y_SIZE))
    {
        FigDrawRectangle(&ulr, &lrc, _COLOR_BLACK); 
    }
    else
    {
        return;
    }

    // 计算填充位置
    if (!hor_vert)
    {
        size = x_size;
    } else
    {
        size = y_size;
    }

    size = size - 4;    // 固定填充条3个像素，减4    
    f_size = ((percent % 101) * (size - 1)) / 100;

    // 画填充    
    if (!hor_vert)
    {
        // 水平滚动
        ulr.x = x + f_size + 1; ulr.y = y + 1; 
    } else
    {
        // 垂直滚动
        ulr.x = x + 1; ulr.y = y + f_size + 1; 
    }
    
    lrc.x = ulr.x + 2; lrc.y = ulr.y + 2;     
    FigFillRectangle(&ulr, &lrc, _COLOR_BLACK);
}

/*----------------------------------------------------------------------
 *  XPanel_TextMid
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    y_side_offset: 文本下偏移调节
 *---------------------------------------------------------------------*/
void XPanel_TextMid(xCursorT *ulr, xCursorT *lrc, const gu8 *text, gu8 y_side_offset)
{

    xCursorT ulr_s = *ulr, lrc_e = *lrc; gu16 y_size = 0;

    // 检查坐标合法性
    if ((lrc_e.x >= _LCD_X_SIZE) && (lrc_e.y >= _LCD_Y_SIZE))
    {
        return;
    }                

    // 画轮廓    
    FigDrawRectangle(&ulr_s, &lrc_e, _COLOR_BLACK); 

    // 画下方两点
    FigDrawPixel(ulr_s.x + 1, lrc_e.y - 1, _COLOR_BLACK);
    FigDrawPixel(lrc_e.x - 1, lrc_e.y - 1, _COLOR_BLACK);

    // 获取文本 Y 宽度
    y_size = TextGetY_MaxSize(text);
    
    // 写文本
    ulr_s.x = ulr->x; ulr_s.y = ulr->y - (y_size >> 1);
    if ((ulr_s.x >= _LCD_X_SIZE) && (ulr_s.y >= _LCD_Y_SIZE))
    {
        return;
    }           
    
    TextOutExAlign(&ulr_s, text, lrc_e.x - ulr_s.x + 1, y_side_offset, _TEXT_NO_RETNLE | _TEXT_MIDDLE_ALIGN);
}

//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
