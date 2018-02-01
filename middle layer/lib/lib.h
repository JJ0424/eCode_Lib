/*----------------------------------------------------------------------^^-
/ File name:  lib.h
/ Author:     JiangJun
/ Data:       2018/01/31
/ Version:    v1.5
/-----------------------------------------------------------------------^^-
/ 
/------------------------------------------------------------------------*/

#ifndef _LIB_H
#define _LIB_H

#define _180_PI (57.2957795131)
#define _PI_180 (0.01745329252)

#define ASCII_TO_DECIMAL(asc)	(asc - 0x30)
#define DECIMAL_TO_ASCII(dec)	(dec + 0x30)

extern void fcvt(float value, u8 ndigit, u8 *sign, u8* o_str);
extern u32 atoui(u8 *str, u8 str_cnt);
extern double atof(u8 *str, u8 str_cnt);
extern s32 atoi(u8 *str, u8 str_cnt);
extern void dfcvt(double value, u8 ndigit, u8 *sign, u8* o_str);
extern void icvt(s32 value, u8 *sign, u8 *o_str);

#endif
