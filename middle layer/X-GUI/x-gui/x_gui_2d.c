
/*----------------------------------------------------------------------^^-
/ File name:  x_gui_2d.c
/ Author:     JiangJun
/ Data:       2018/8/19
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ X-GUI 2D Draw Driver
/------------------------------------------------------------------------*/

#include "x_gui_2d.h"

/*----------------------------------------------------------------------
 *  FigDrawXLine
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    出错情况: 1.坐标太大舍弃本次操作。
 *---------------------------------------------------------------------*/
void FigDrawXLine(gu16 xs, gu16 xe, gu16 y, gu8 col)
{
    if ((xs >= _LCD_X_SIZE) || (xe >= _LCD_X_SIZE) || (y >= _LCD_Y_SIZE)) {
        return;
    }
    if (xe < xs) {
        return;
    }

    for (; xs <= xe; xs++) {
        pLcdSetPixel(xs, y, col);
    }
}

/*----------------------------------------------------------------------
 *  FigDrawYLine
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    出错情况: 1.坐标太大舍弃本次操作。
 *---------------------------------------------------------------------*/
void FigDrawYLine(gu16 ys, gu16 ye, gu16 x, gu8 col)
{
    if ((ys >= _LCD_Y_SIZE) || (ye >= _LCD_Y_SIZE) || (x >= _LCD_X_SIZE)) {
        return;
    }
    if (ye < ys) {
        return;
    }

    for (; ys <= ye; ys++) {
        pLcdSetPixel(x, ys, col);
    }
}

/*----------------------------------------------------------------------
 *  FigDrawXDotLine - 画X轴虚线，可设置实线像素数和空线像素数
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    出错情况: 1.坐标太大舍弃本次操作。
 *---------------------------------------------------------------------*/
void FigDrawXDotLine(gu16 xs, gu16 xe, gu16 y, gu8 col, gu8 dot_pixs, gu8 emy_pixs)
{
    gu8 dot_cnt = 0, emt_cnt = 0;
    
    if ((xs >= _LCD_X_SIZE) || (xe >= _LCD_X_SIZE) || (y >= _LCD_Y_SIZE)) {
        return;
    }
    if (xe < xs) {
        return;
    }

    for (dot_cnt = 0, emt_cnt = 0; xs <= xe; xs++) 
    {
        if ((dot_cnt >= dot_pixs) && (emt_cnt >= emy_pixs)) { dot_cnt = 0; emt_cnt = 0; }
        
        // Put one piexl
        if (dot_cnt < dot_pixs) { pLcdSetPixel(xs, y, col); dot_cnt++;}
        else { emt_cnt++; }
    }
}

/*----------------------------------------------------------------------
 *  FigDrawYDotLine - 画Y轴虚线，可设置实线像素数和空线像素数
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    出错情况: 1.坐标太大舍弃本次操作。
 *---------------------------------------------------------------------*/
void FigDrawYDotLine(gu16 ys, gu16 ye, gu16 x, gu8 col, gu8 dot_pixs, gu8 emy_pixs)
{
    gu8 dot_cnt = 0, emt_cnt = 0;
    
    if ((ys >= _LCD_Y_SIZE) || (ye >= _LCD_Y_SIZE) || (x >= _LCD_X_SIZE)) {
        return;
    }
    if (ye < ys) {
        return;
    }

    for (dot_cnt = 0, emt_cnt = 0; ys <= ye; ys++) 
    {
        if ((dot_cnt >= dot_pixs) && (emt_cnt >= emy_pixs)) { dot_cnt = 0; emt_cnt = 0; }
        
        // Put one piexl
        if (dot_cnt < dot_pixs) { pLcdSetPixel(x, ys, col); dot_cnt++;}
        else { emt_cnt++; }        
    }
}

/*----------------------------------------------------------------------
 *  FigDrawLine
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    a. 起点和终点都会画上
 *           b. 出错情况: 1.坐标太大舍弃本次操作。
 *---------------------------------------------------------------------*/
void FigDrawLine(xCursorT *begin, xCursorT *end, gu8 col)
{
    gs16 Sub = 0x00;
	gs16 xs = (gs16)begin->x, ys = (gs16)begin->y;
	gs16 dx = end->x - xs, dy = end->y - ys;
	gs16 ux = ((dx > 0) << 1) - 1, uy = ((dy > 0) << 1) - 1;

	if ((begin->x >= _LCD_X_SIZE) || (begin->y >= _LCD_Y_SIZE)) {
        return;
	}
	if ((end->x >= _LCD_X_SIZE) || (end->y >= _LCD_Y_SIZE)) {
        return;
	}

    // can convert to logic operate.
	if (dy < 0) dy = -dy;				
	if (dx < 0)	dx = -dx;

	pLcdSetPixel(end->x, end->y, col);

	if (dx > dy)
	{
		while  (xs != end->x)		
		{
			pLcdSetPixel(xs, ys, col);	
			
			Sub = Sub + dy;					
			if ((Sub << 1) >= dx)
			{
				ys = ys + uy;			
				Sub = Sub - dx;				
			}
			xs += ux;
		}
	}
	else
	{
		while (ys != end->y)		
		{
			pLcdSetPixel(xs, ys, col);	
			
			Sub = Sub + dx;					
			if ((Sub << 1) >= dy)
			{
				xs = xs + ux;			
				Sub = Sub - dy;				
			}
			ys += uy;
		}
	}
}

/*----------------------------------------------------------------------
 *  FigDrawCircle
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    a. 出错情况:       1.坐标太大舍弃本次操作。
 *                          2.半径大于长度或宽度的1/2，舍弃本次操作。
 *
 *           b. 所做圆必须在显示器上全部可见
 *---------------------------------------------------------------------*/
void FigDrawCircle(gu16 x, gu16 y, gu16 r, gu8 col)
{
    gs16 x0, y0, xd, yd, e;

    if ((x >= _LCD_X_SIZE) || (y >= _LCD_Y_SIZE)) {
        return;
    }

    if ((r > (_LCD_X_SIZE / 2)) || ((r > (_LCD_Y_SIZE/ 2)))) {
        return;
    }
    
    xd = 1 - (r << 1);
    yd = 0;
    e = 0;
    x0 = r;
    y0 = 0;

    for (; x0 >= y0; )
    {
        pLcdSetPixel(x - x0, y + y0, col);
        pLcdSetPixel(x - x0, y - y0, col);
        pLcdSetPixel(x + x0, y + y0, col);
        pLcdSetPixel(x + x0, y - y0, col);
        pLcdSetPixel(x - y0, y + x0, col);
        pLcdSetPixel(x - y0, y - x0, col);
        pLcdSetPixel(x + y0, y + x0, col);
        pLcdSetPixel(x + y0, y - x0, col);

        y0++;
        e += yd;
        yd += 2;
        if (((e << 1) + xd) > 0)
        {
            x0--;
            e += xd;
            xd += 2;
        }
    }
}

/*----------------------------------------------------------------------
 *  FigFillCircle - 填充圆，包括最外圈，和画圆函数可以组成带边框的实心圆，
 *                  但是半径是画空心圆半径减一
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    a. 出错情况:       1.坐标太大舍弃本次操作。
 *                          2.半径大于长度或宽度的1/2，舍弃本次操作。
 *
 *           b. 所填充圆必须在显示器上全部可见
 *---------------------------------------------------------------------*/
void FigFillCircle(gu16 x_coor, gu16 y_coor, gu16 r, gu8 col)
{
    gs16 x0, y0, xd;
    xCursorT beg, end;

    if ((x_coor >= _LCD_X_SIZE) || (y_coor >= _LCD_Y_SIZE)) {
        return;
    }

    if ((r > (_LCD_X_SIZE / 2)) || ((r > (_LCD_Y_SIZE/ 2)))) {
        return;
    }

    xd = 3 - (r << 1);
    x0 = 0;
    y0 = r;

    for (; x0 <= y0; )
    {
        if( y0 > 0 )
        {
            beg.x = x_coor - x0; beg.y = y_coor - y0;
            end.x = x_coor - x0; end.y = y_coor + y0;
            FigDrawLine(&beg, &end, col);

            beg.x = x_coor + x0; beg.y = y_coor - y0;
            end.x = x_coor + x0; end.y = y_coor + y0;
            FigDrawLine(&beg, &end, col);
        }
        if( x0 > 0 )
        {
            beg.x = x_coor - y0; beg.y = y_coor - x0;
            end.x = x_coor - y0; end.y = y_coor + x0;
            FigDrawLine(&beg, &end, col);

            beg.x = x_coor + y0; beg.y = y_coor - x0;
            end.x = x_coor + y0; end.y = y_coor + x0;
            FigDrawLine(&beg, &end, col);
        }
        if ( xd < 0 )
        {
            xd += (x0 << 2) + 6;
        }
        else
        {
            xd += ((x0 - y0) << 2) + 10;
            y0--;
        }
        x0++;
    }

    FigDrawCircle(x_coor, y_coor, r, col);
}

/*----------------------------------------------------------------------
 *  FigDrawRectangle
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    出错情况: 方框坐标必须在LCD分辨率范围内, 否则画不出方框，或者只能画一部分方框
 *---------------------------------------------------------------------*/
void FigDrawRectangle(xCursorT *left_top, xCursorT *right_bottom, gu8 col)
{
    xCursorT beg, end;

    beg.x = left_top->x; beg.y = left_top->y;
    end.x = right_bottom->x; end.y = left_top->y;
    FigDrawLine(&beg, &end, col);

    beg.x = right_bottom->x; beg.y = left_top->y;
    end.x = right_bottom->x; end.y = right_bottom->y;
    FigDrawLine(&beg, &end, col);

    beg.x = right_bottom->x; beg.y = right_bottom->y;
    end.x = left_top->x; end.y = right_bottom->y;
    FigDrawLine(&beg, &end, col);

    beg.x = left_top->x; beg.y = right_bottom->y;
    end.x = left_top->x; end.y = left_top->y;
    FigDrawLine(&beg, &end, col);   
}

/*----------------------------------------------------------------------
 *  FigDrawPolygon - 画空心多变形(三角、四角、五角...)
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    a. 最多10边形
 *
 *           b. 出错情况: 方框坐标必须在LCD分辨率范围内, 
 *              否则画不出多边形，或者只能画一部分多边形
 *---------------------------------------------------------------------*/
void FigDrawPolygon(xCursorT *pnt, gu8 cnt, gu8 col)
{
    gu8 idx;
    xCursorT beg, end;

    if ((cnt > 10) || (cnt < 1)) {
        return;
    }

    for (idx = 0; idx < (cnt - 1); idx++)
    {
        beg.x = pnt[idx].x; beg.y = pnt[idx].y;
        end.x = pnt[idx + 1].x; end.y = pnt[idx + 1].y;
        FigDrawLine(&beg, &end, col);
    }

    beg.x = pnt[idx].x; beg.y = pnt[idx].y;
    end.x = pnt[0].x; end.y = pnt[0].y;
    FigDrawLine(&beg, &end, col);
}

/*----------------------------------------------------------------------
 *  FigFillPolygon
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    a. 最多10边形
 *
 *           b. 出错情况: 方框坐标必须在LCD分辨率范围内, 
 *              否则画不出多边形，或者只能画一部分多边形
 *---------------------------------------------------------------------*/
void FigFillPolygon(xCursorT *pnt, gu8 cnt, gu8 col)
{
    gu16 x_node[10];
    gu16 x_node_sum, y, i, j, swap;
    gu16 y_top, y_bottom, x_right, x_left; 

    if ((cnt > 10) || (cnt < 1)) {
        return;
    }

    // 获取多边形尺寸
    x_right = pnt[0].x; x_left = pnt[0].x;
    y_top = pnt[0].y; y_bottom = pnt[0].y;
    for (i = 1; i < cnt; i++)
    {
        if (pnt[i].x < x_left) {
            x_left = pnt[i].x;
        }
        if (pnt[i].y < y_top) {
            y_top = pnt[i].y;
        }

        if (pnt[i].x > x_right) {
            x_right = pnt[i].x;
        }

        if (pnt[i].y > y_bottom) {
            y_bottom = pnt[i].y;
        }
    }

    // 逐行扫描,第1行和最后1行不做扫描
    // y代表行数
    for (y = (y_top + 1); y < y_bottom; y++)
    {
        // 找出1行所有的交点，必须为偶数
        x_node_sum = 0;
        for (i = 0, j = cnt - 1; i < cnt; i++)
        {
            if ( ((pnt[i].y < y) && (pnt[j].y >= y)) || ((pnt[i].y >= y) && (pnt[j].y < y)) )
            {  
                x_node[x_node_sum++] = (gu16)( pnt[i].x + ((float)(y - pnt[i].y) / (float)(pnt[j].y - pnt[i].y) * (float)(pnt[j].x - pnt[i].x)) );
            }
            j = i;
        }

        // 排序
        if (x_node_sum != 0)
        {
            for (i = 0; i < (x_node_sum - 1); )
            {
                if (x_node[i] > x_node[i + 1]) 
                {  
                    swap = x_node[i]; 
                    x_node[i] = x_node[i+1]; 
                    x_node[i + 1] = swap;
                    
                    if (i) {
                        i--; 
                    }
                }  
                else {  
                    i++; 
                }
            }
        }

        // 输出
        for (i = 0; i < x_node_sum; i += 2) 
        {  
            if (x_node[i] >= x_right) {
                break;
            }
            
            if (x_node[i + 1] > x_left) 
            {  
                if (x_node[i] < x_left ) {
                    x_node[i] = x_left ;  
                }
                if (x_node[i + 1] > x_right) {
                    x_node[i + 1] = x_right;
                }
                
                FigDrawXLine(x_node[i], x_node[i + 1], y, col);
            }
        }
    }
}

/*----------------------------------------------------------------------
 *  FigDrawArc
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    出错情况: 方框坐标必须在LCD分辨率范围内
 *---------------------------------------------------------------------*/
void FigDrawArc(gu16 x0, gu16 y0, gu16 r, gu8 s, gu8 col)
{
    gs16 x, y, xd, yd, e;

    if ((x0 >= _LCD_X_SIZE) || (y0 >= _LCD_Y_SIZE)) {
    return;
    }
    
    xd = 1 - (r << 1);
    yd = 0;
    e = 0;
    x = r;
    y = 0;

    while ( x >= y )
    {
        // Q1
        if ( s & 0x01 ) pLcdSetPixel(x0 + x, y0 - y, col);
        if ( s & 0x02 ) pLcdSetPixel(x0 + y, y0 - x, col);

        // Q2
        if ( s & 0x04 ) pLcdSetPixel(x0 - y, y0 - x, col);
        if ( s & 0x08 ) pLcdSetPixel(x0 - x, y0 - y, col);

        // Q3
        if ( s & 0x10 ) pLcdSetPixel(x0 - x, y0 + y, col);
        if ( s & 0x20 ) pLcdSetPixel(x0 - y, y0 + x, col);

        // Q4
        if ( s & 0x40 ) pLcdSetPixel(x0 + y, y0 + x, col);
        if ( s & 0x80 ) pLcdSetPixel(x0 + x, y0 + y, col);

        y++;
        e += yd;
        yd += 2;
        if (((e << 1) + xd) > 0)
        {
            x--;
            e += xd;
            xd += 2;
        }
    }
}

/*----------------------------------------------------------------------
 *  FigDrawRoundFrame - 画带拐角的方框
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void FigDrawRoundFrame(xCursorT *left_top, xCursorT *right_bottom, gu16 r, gu8 col)
{
   gs16 n; xCursorT beg, end;
   
   if (right_bottom->x < left_top->x)
   {
        n = right_bottom->x;
        right_bottom->x = left_top->x;
        left_top->x = n;
   }
   if (right_bottom->y < left_top->y)
   {
        n = right_bottom->y;
        right_bottom->y = left_top->y;
        left_top->y = n;
   }

   if ( r > right_bottom->x ) return;
   if ( r > right_bottom->y ) return;

   beg.x = left_top->x + r; beg.y = left_top->y; end.x = right_bottom->x - r; end.y = left_top->y;
   FigDrawLine(&beg, &end, col);
   
   beg.x = left_top->x + r; beg.y = right_bottom->y; end.x = right_bottom->x - r; end.y = right_bottom->y;
   FigDrawLine(&beg, &end, col);
   
   beg.x = left_top->x; beg.y = left_top->y + r; end.x = left_top->x; end.y = right_bottom->y - r;
   FigDrawLine(&beg, &end, col);
   
   beg.x = right_bottom->x; beg.y = left_top->y + r; end.x = right_bottom->x; end.y = right_bottom->y - r;
   FigDrawLine(&beg, &end, col);
   
   FigDrawArc(left_top->x+r, left_top->y+r, r, 0x0C, col);
   FigDrawArc(right_bottom->x-r, left_top->y+r, r, 0x03, col);
   FigDrawArc(left_top->x+r, right_bottom->y-r, r, 0x30, col);
   FigDrawArc(right_bottom->x-r, right_bottom->y-r, r, 0xC0, col);
}

/*----------------------------------------------------------------------
 *  FigFillRoundFrame - 填充带拐角的框
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void FigFillRoundFrame(xCursorT *left_top, xCursorT *right_bottom, gu16 r, gu8 col)
{
    gs16 x,y,xd; xCursorT beg, end;

    if ( right_bottom->x  < left_top->x )
    {
        x = right_bottom->x ;
        right_bottom->x  = left_top->x;
        left_top->x = x;
    }
    if ( right_bottom->y < left_top->y )
    {
        y = right_bottom->y;
        right_bottom->y = left_top->y;
        left_top->y = y;
    }

    if ( r <= 0 ) return;

    xd = 3 - (r << 1);
    x = 0;
    y = r;

    beg.x = left_top->x + r; beg.y = left_top->y; end.x = right_bottom->x - r; end.y = right_bottom->y;    
    FigFillRectangle(&beg, &end, col);

    while ( x <= y )
    {
        if( y > 0 )
        {
            beg.x = right_bottom->x  + x - r; beg.y = left_top->y - y + r; end.x = right_bottom->x + x - r; end.y = y + right_bottom->y - r;        
            FigDrawLine(&beg, &end, col);

            beg.x = left_top->x - x + r; beg.y = left_top->y - y + r; end.x = left_top->x- x + r; end.y = y + right_bottom->y - r;
            FigDrawLine(&beg, &end, col);
        }
        if( x > 0 )
        {
            beg.x = left_top->x - y + r; beg.y = left_top->y - x + r; end.x = left_top->x - y + r; end.y = x + right_bottom->y - r;
            FigDrawLine(&beg, &end, col);

            beg.x = right_bottom->x  + y - r; beg.y = left_top->y - x + r; end.x = right_bottom->x  + y - r; end.y = x + right_bottom->y - r;
            FigDrawLine(&beg, &end, col);
        }
        if ( xd < 0 )
        {
            xd += (x << 2) + 6;
        }
        else
        {
            xd += ((x - y) << 2) + 10;
            y--;
        }
        
        x++;
    }
}

//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
