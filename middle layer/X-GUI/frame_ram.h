
/*----------------------------------------------------------------------^^-
/ File name:  frame_ram.h
/ Author:     JiangJun
/ Data:       2018/2/11
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ Mono Mode Frame RAM Driver
/------------------------------------------------------------------------*/

#ifndef _FRAME_RAM_H
#define _FRAME_RAM_H

//------------------------------------------------------------
// DRIVER CONFIG
//------------------------------------------------------------

#define _FRAME_RAM_SIZE_X       192
#define _FRAME_RAM_SIZE_Y       96

// Vertical Flip
// 
// (0, 0)
// ----------------> x                               y
// |                                                 |
// |                                                 |
// |                       ===>                      |
// |                                                 |
// |                                                 |
// |                                                 |
// y                                x <---------------(0, 0)

#define _FRAME_ENABLE_VERTICAL_FLIP     0

// LCD reverse
#define _FRAME_ENABLE_REVERSE           0

//------------------------------------------------------------
// COLOR DEFINE
//------------------------------------------------------------

#if _FRAME_ENABLE_REVERSE

  #define _COLOR_BLACK          0
  #define _COLOR_WHITE          1
#else

  #define _COLOR_BLACK          1
  #define _COLOR_WHITE          0
#endif

//------------------------------------------------------------
// TYPE
//------------------------------------------------------------

// general type
typedef enum {gfalse = 0, gtrue} gbool;
typedef signed char gs8;
typedef signed short gs16;
typedef signed int gs32;
typedef unsigned char gu8;
typedef unsigned short gu16;
typedef unsigned int gu32;

// color type.
typedef gu8 ColorT;

// coordinate position.
typedef gu16 CoorT;
typedef gs16 CoorSignT;

// cursor
typedef struct {
    CoorT x;
    CoorT y;
} CursorT;

//------------------------------------------------------------
// TYPE
//------------------------------------------------------------

typedef void (*fpInit)(void);
typedef void (*fpWriteFrame)(unsigned char *dat, unsigned int len);
typedef void (*fpSetConstrast)(unsigned char *cst);

extern void pLcdInit(fpInit fp_init, fpWriteFrame fp_write_fme, fpSetConstrast fp_set_cst);
extern void pLcdSetPixel(CoorT x, CoorT y, ColorT color);
extern void pLcdGetPixel(CoorT x, CoorT y, ColorT *color);
extern void pLcdWritePixels(CursorT *ulc, CursorT *lrc, ColorT *color);
extern void pLcdFillPixels(CursorT *ulc, CursorT *lrc, ColorT color);
extern void pLcdReadPixels(CursorT *ulc, CursorT *lrc, ColorT *color);
extern void pLcdUpdate(void);

#endif

