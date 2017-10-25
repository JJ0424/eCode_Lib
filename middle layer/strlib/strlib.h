

// v1.0 [2017-10-19]

#ifndef _STRLIB_H
#define _STRLIB_H

#include "main.h"

void _strcpy(u8 *src, u8 *dest);
void _strcat(u8 *src, u8 *dest);
bool _strcmp(u8 *str1, u8 *str2);
u16 _strlen(u8 *str);
s16 _strstr(u8 *str, u16 str_len, const u8 *str_2);


#endif
