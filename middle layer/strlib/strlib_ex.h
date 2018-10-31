
/*----------------------------------------------------------------------^^-
/ File name:  strlib_ex.h
/ Author:     JiangJun
/ Data:       2018/10/31
/ Version:    v1.1
/-----------------------------------------------------------------------^^-
/ STRING LIB
/------------------------------------------------------------------------*/

#ifndef _STRLIB_EX_H
#define _STRLIB_EX_H
#include "main.h"

//------------------------------------------------------------
//              MARCO
//------------------------------------------------------------

#define _STREX_LEN_SIZEOF(a)    (sizeof(a) - 1)

//------------------------------------------------------------
//              TYPEDEF
//------------------------------------------------------------

// vpause item
typedef struct {

    u8 ch;          // split char
    u8 sp_idx;      // 1-255
    u8 *fmt;        // %c, %d, %f, %lf
    void *_pvar;    // point to var
    
} _V_PauseItemT;

extern u16 _cpause(u8 ch, u8 spt_idx, u8 *in, u16 ilen, u8 *out, u16 olen);
extern double _atod(u8 *src, u8 cnt);
extern float _atof(u8 *src, u8 cnt);
extern s32 _atoi(u8 *src, u8 cnt);
extern u8 _vpause(u8 *src, u16 cnt, u8 *buff, u16 bfcnt, _V_PauseItemT *vitem, u8 icnt);
extern void _strcpy(u8 *src, u8 *dst);
extern void _strcat(u8 *src, u8 *dst);
extern u16 _strlen(u8 *src);
extern void _icvt(s32 val, u8 *out, u8 out_size);

#endif

//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
