

/*----------------------------------------------------------------------^^-
/ File name:  common.h
/ Author:     JiangJun
/ Data:       2018-6-21
/ Version:    v1.4
/-----------------------------------------------------------------------^^-
/  Common Functions
/------------------------------------------------------------------------*/

#ifndef _COMMON_H
#define _COMMON_H


extern u8 XorCheckSum(u8 *input, u32 length);
extern void LoopDelay(u16 cnt);
extern void uicvt_format_bit(u32 val, u8 bit_width, u8 *out, u16 out_size);

#endif