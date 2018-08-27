
// v1.3 [2018-8-27]

#ifndef _STRLIB_H
#define _STRLIB_H

#include "main.h"

void _strcpy(u8 *src, u8 *dest);
void _strcat(u8 *src, u8 *dest);
bool _strcmp(u8 *str1, u8 *str2);
u16 _strlen(u8 *str);
s16 _strstr(u8 *str, u16 str_len, const u8 *str_2);
u16 _strtrim(u8 *str, const u8 *trim);
void _memcpy(u8 *dst, u8 *src, u16 size);
_bool _strcmp_2(u8 *str1, u8 *str2, u8 len);
_bool _strpause(u8 ch, u16 seg, u8 *in_str, u16 in_len, u8 *out_str, u16 out_len);

#endif
