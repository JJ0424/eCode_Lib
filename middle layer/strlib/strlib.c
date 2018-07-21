

// v1.1 [2018-07-21]

#include "strlib.h"

//------------------------------------------------------------
// String copy, dins must more than src.
//------------------------------------------------------------
void _strcpy(u8 *src, u8 *dest)
{
    for (; *src != 0; src++)
    {
        *dest = *src;
        dest++;
    }
    
    *dest = 0;
}

//------------------------------------------------------------
// Add the src to the end of the dins.
//------------------------------------------------------------
void _strcat(u8 *src, u8 *dest)
{   
    for (; *dest != 0; dest++);
    
    for (; *src != 0; src++)
    {
        *dest = *src;
        dest++;
    }
    
    *dest = 0;
}

//------------------------------------------------------------
// Compare 2 strings
//------------------------------------------------------------
bool _strcmp(u8 *str1, u8 *str2)
{
    u16 idx = 0;
    
    for (idx = 0; (str1[idx] != '\0') && (str2[idx] != '\0'); idx++)
    {
        if (str1[idx] != str2[idx]) { break; }
    }
    
    if ((str1[idx] == '\0') && (str2[idx] == '\0')) { return TRUE; }
    
    return FALSE;    
}

//------------------------------------------------------------
// Compare 2 strings
//------------------------------------------------------------
u16 _strlen(u8 *str)
{
    u16 idx = 0;
    
    for (idx = 0; str[idx] != '\0'; idx++)
    {
       
    }        
    
    return idx;    
}

//------------------------------------------------------------
// Find string in buffer
// -1: failed
//------------------------------------------------------------
s16 _strstr(u8 *str, u16 str_len, const u8 *str_2)
{
    u16 idx_s = 0, idx_comp = 0; u16 comp_str_len = 0;

    // string length
    comp_str_len = _strlen((u8 *)str_2);

    // 找到字符串，最后不足比较个数的数据不做比较，直接返回
    for (idx_s = 0; idx_s < (str_len - comp_str_len + 1); idx_s++)
    {
        for (idx_comp = 0; idx_comp < comp_str_len; idx_comp++)
        {
            if (str[idx_s + idx_comp] != str_2[idx_comp]) { break; }
        }

        // Find it
        if (idx_comp == comp_str_len) { return idx_s; }
    }        
    
    return -1;    
}

//------------------------------------------------------------
// Trim string with the 'trim' characters
// return: the length of trimed
//------------------------------------------------------------
u16 _strtrim(u8 *str, const u8 *trim)
{
    u16 idx_s = 0, idx_t = 0, cnt = 0, len_s = 0, len_t = 0;
    u8 *p_str = str; const u8 *p_trim = trim;

    // string length
    len_s = _strlen(p_str); len_t = _strlen((u8 *)p_trim);

    // trim
    for (idx_s = 0; idx_s < len_s; idx_s++)
    {
        for (idx_t = 0; idx_t < len_t; idx_t++) 
        {
            if (p_str[idx_s] == p_trim[idx_t]) { break; }
		}

		// copy to str
		if (idx_t >= len_t) { p_str[cnt++] = p_str[idx_s]; }
    }

    return cnt;
}