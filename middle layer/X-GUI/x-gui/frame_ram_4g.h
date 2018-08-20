
/*----------------------------------------------------------------------^^-
/ File name:  frame_ram_4g.h
/ Author:     JiangJun
/ Data:       [2018-8-15]
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ Gray Mode Frame RAM Driver
/------------------------------------------------------------------------*/

#ifndef _FRAME_RAM_4G_H
#define _FRAME_RAM_4G_H

//------------------------------------------------------------
// DRIVER CONFIG
//------------------------------------------------------------

#define _FRAME_RAM_BYTE_PIXELS          4       // [ 2bits/ piexl ]

#define _FRAME_RAM_SIZE_X               240
#define _FRAME_RAM_SIZE_Y               160

// Vertical Flip
// 
// (0, 0)                               
// ----------------> x                  y              
// |                                    |             
// |                                    |              
// |                       ===>         |              
// |                                    |               
// |                                    |              
// |                                    |               
// y                             (0, 0) ---------------> x

#define _FRAME_ENABLE_VERTICAL_FLIP     0

// Horizontal Flip
// 
// (0, 0)                                       
// ----------------> x              x <---------------(0, 0)
// |                                                 |
// |                                                 |
// |                       ===>                      |
// |                                                 |
// |                                                 |
// |                                                 |
// y                                                 y

#define _FRAME_ENABLE_HORIZONTAL_FLIP   1

// LCD reverse
#define _FRAME_ENABLE_REVERSE           0

//------------------------------------------------------------
// COLOR DEFINE
//------------------------------------------------------------

#if _FRAME_ENABLE_REVERSE

  #define _COLOR_WHITE          3
  #define _COLOR_GRAY_1         2
  #define _COLOR_GRAY_2         1
  #define _COLOR_BLACK          0  
  
#else

  #define _COLOR_WHITE          0
  #define _COLOR_GRAY_1         1
  #define _COLOR_GRAY_2         2
  #define _COLOR_BLACK          3
  
#endif /* _FRAME_ENABLE_REVERSE */

//------------------------------------------------------------
// TYPE
//------------------------------------------------------------

// General type
typedef enum {gfalse = 0, gtrue} gbool;
typedef signed char gs8;
typedef signed short gs16;
typedef signed int gs32;
typedef unsigned char gu8;
typedef unsigned short gu16;
typedef unsigned int gu32;

// Cursor
typedef struct {

    gu16 x;
    gu16 y;
    
} xCursorT;

//------------------------------------------------------------
// TYPE
//------------------------------------------------------------

typedef void (*fpInit)(void);
typedef void (*fpWriteFrame)(unsigned char *dat, unsigned int len);
typedef void (*fpSetConstrast)(unsigned char cst);
typedef void (*fpSetBackLight)(unsigned char bkl);

extern void pLcdInit(fpInit fp_init, fpWriteFrame fp_write_fme, fpSetConstrast fp_set_cst, fpSetBackLight fp_set_bkl);
extern void pLcdSetPixel(gu16 x, gu16 y, gu8 color);
extern void pLcdGetPixel(gu16 x, gu16 y, gu8 *color);
extern void pLcdUpdate(void);
extern void pLcdFillPixels(xCursorT *ulc, xCursorT *lrc, gu8 color);
extern void pLcdWritePixels(xCursorT *ulc, xCursorT *lrc, gu8 *p_col);
extern void pLcdWritePixelsEx(xCursorT *ulc, xCursorT *lrc, gu8 *p_col, gu8 rev);
extern void pLcdSetBrush(gu8 f_col, gu8 b_col);
extern void pLcdRevPixels(xCursorT *ulc, xCursorT *lrc, gu8 rev);
extern void pLcdSetCst(gu8 cst);
extern void pLcdSetBackLight(gu8 bkt);
extern void pLcdFillGray(xCursorT *ulc, xCursorT *lrc, gu8 color);
extern gu16 pLcdReadPixels(xCursorT *ulc, xCursorT *lrc, gu8 *p_col, gu16 buff_size);

#endif

//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
