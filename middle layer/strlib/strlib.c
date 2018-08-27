
// v1.3 [2018-8-27]

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

//------------------------------------------------------------
// memory copy
//------------------------------------------------------------
void _memcpy(u8 *dst, u8 *src, u16 size)
{
    u16 idx = 0; u8 *dst_tmp = dst, *src_tmp = src;

    for (idx = 0; idx < size; idx++)
    {
        *dst_tmp = *src_tmp;
        dst_tmp++; src_tmp++;
    }
}

//------------------------------------------------------------
// Compare 2 strings, 指定长度
//------------------------------------------------------------
_bool _strcmp_2(u8 *str1, u8 *str2, u8 len)
{
    u16 idx = 0;
    
    for (idx = 0; idx < len; idx++)
    {
        if (str1[idx] != str2[idx]) { break; }
    }
    
    if (idx == len) { return TRUE; }
    
    return FALSE;    
}

//------------------------------------------------------------
// 获取指定分割符后面的字符串
// ch:          字符
// seg:         指示第几个字符，从1开始...
// in_str:      输入字符串
// out_str:     输出缓冲
// out_len:     输出缓冲长度
//------------------------------------------------------------
_bool _strpause(u8 ch, u16 seg, u8 *in_str, u16 in_len, u8 *out_str, u16 out_len)
{
    u16 idx = 0, st_idx = 0, end_idx = 0; u16 seg_cnt = 0;

    // Find Index Start
    for (idx = 0; (in_str[idx] != '\0') && (idx < in_len); idx++)
	{
	    // Find char
		if (in_str[idx] == ch) { seg_cnt++; }
			
		// Find Start
		if (seg_cnt == seg)			
		{
			st_idx = ++idx; break;
		}		
	}

    // Find Index End
    for (; (in_str[idx] != '\0') && (idx < in_len); idx++)
	{
	    // Find char
		if (in_str[idx] == ch) { seg_cnt++; }
			
		// Find End
	    if (seg_cnt == (seg + 1))
		{
            end_idx = idx - 1; break;
		}
	}
    
	// Check Vaild
	if ((end_idx >= st_idx) && ((end_idx - st_idx + 1) < out_len) && (seg_cnt > 0))
	{
	    // byte count
	    seg_cnt = end_idx - st_idx + 1;
	    
        // Copy to Memory
        for (idx = 0; idx < seg_cnt; idx++)
        {
            out_str[idx] = in_str[st_idx + idx];
        }

        // Ending Char
        out_str[idx] = '\0';

        // True
        return TRUE;
	}
	else
	{
        out_str[0] = '\0';
	}
		    
    return FALSE;    
}