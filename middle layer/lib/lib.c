/*----------------------------------------------------------------------^^-
/ File name:  lib.c
/ Author:     JiangJun
/ Data:       2017/08/17
/ Version:    v1.4
/-----------------------------------------------------------------------^^-
/ converter with string and interger/ double/ float...
/ ----------------------
/ v1.2 2017-2-22
/ <1> FIX: atof() add sign output 
/ -----------------------
/ v1.3  
/ <1> FIX: fcvt/dfcvt output .xx when input 0.xx
/ -----------------------
/ v1.4  
/ <1> UPDATE: modify fcvt/dfcvt output 0.00... when input 0.0f
/------------------------------------------------------------------------*/

#include "main.h"
#include "lib.h"

/*----------------------------------------------------------------------
 *  atof
 *
 *  Purpose: Convert string to double.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
double atof(u8 *str, u8 str_cnt)
{
    u8 idx, cnt = 0, sign = 0;
    u32 interger = 0, point = 0, scale = 0;
    double res = 0.0;
    
    // find the first numerial character
    for(idx = 0; idx < str_cnt; idx++)
    {
        if ((str[idx] > '9') || (str[idx] < '0') && (str[idx] != '-')) continue;
        else 
        {
            if (str[idx] == '-') { sign = 1; idx++; }   // negative number.
            else sign = 0;                              // positive number.
            
            break;
        }
    }
    
    // interger computer.
    for (; idx < str_cnt; idx++)
    {
        if ((str[idx] > '9') || (str[idx] < '0') && (str[idx] != '.')) break;
        if (str[idx] != '.')
        {
            interger *= 10;
            interger += ASCII_TO_DECIMAL(str[idx]);
        }
        else break;
    }
    // point number computer.
    for (; idx < str_cnt; idx++)
    {
        if ((str[idx] > '9') || (str[idx] < '0') && (str[idx] != '.')) break;
        if (str[idx] != '.')
        {
            point *= 10;
            point += ASCII_TO_DECIMAL(str[idx]);
            cnt++;
        }
    }
    
    if (cnt != 0) 
    {
        scale = 1;
        for (; cnt != 0; cnt--) {
            scale = scale * 10;
        }
        res = (double)interger + (double)((double)point / (double)scale);
    }
    else {
        res = (double)interger;
    }
    
    if (sign == 1) { res = -res; }
    
    return res;
}

// 字符串转有符号整数
// 第一个字符为符号或者是数字开始转换，直到遇到非数字字符结束
s32 atoi(u8 *str, u8 str_cnt)
{
    return 0;
}

/*----------------------------------------------------------------------
 *  atoui
 *
 *  Purpose: Convert string to unsigned interger.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u32 atoui(u8 *str, u8 str_cnt)
{
    u8 idx = 0; u32 interger = 0;
    
    if ((*str > '9') || (*str < '0')) return 0;     // 判断第一个字符是不是数字
    
    for (idx = 0; idx < str_cnt; idx++)
    {
        if ((str[idx] >= '0') && (str[idx] <= '9')) // 字符为数字
        {
            interger *= 10;
            interger += ASCII_TO_DECIMAL(str[idx]);
        }
        else break;
    }
    
    return interger;
}

/*----------------------------------------------------------------------
 *  fcvt
 *
 *  Purpose: Convert string to double.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    精确显示有效数字7位
 *---------------------------------------------------------------------*/
void fcvt(float value, u8 ndigit, u8 *sign, u8* o_str)
{
    u32 middle, scales;
    u8 cnt, swap, idx, mid;

    // 负数处理
    if (value < 0) 
    {
        value = -value;
        *sign = 1;
    }
    else {
        *sign = 0;
    }

    if (value == 0)
    {
        o_str[0] = '0';
        cnt = 1;
        
        if (ndigit != 0)
        {
            // 添加.
            o_str[cnt] = '.'; cnt++;
            
            // 添加小数点后面的0
            for (idx = ndigit; idx != 0; idx--, cnt++) 
            {
                o_str[cnt] = '0';
            }
        }
    }
    else
    {
        // value放大scales倍
        for (idx = ndigit, scales = 1; idx != 0; idx--) {
            scales *= 10;
        }
       
        middle = (u32)(value * (float)scales);

        // 按位移出，逆序
        for (idx = 0, cnt = 0; middle != 0; idx++)
        {
            o_str[idx] = DECIMAL_TO_ASCII(middle % 10);
            middle /= 10;
            cnt++;
            if (cnt == ndigit)
            {
                idx++;
                o_str[idx] = '.';
                cnt++;
                
                // 判断middle是否是0,，若是0，则输入的数小于1
                if (middle == 0)
                {
                    idx++;
                    o_str[idx] = '0';
                    cnt++;
                }
            }
        }

        // 正序
        mid = cnt >> 1;
        for (idx = 0; idx < mid; idx++)
        {
            swap = o_str[idx];
            o_str[idx] = o_str[cnt - idx - 1];
            o_str[cnt - idx - 1] = swap;
        }
    }
    // 字符串结束符
    o_str[cnt] = '\0';
}

/*----------------------------------------------------------------------
 *  dfcvt
 *
 *  Purpose: Convert double to string
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void dfcvt(double value, u8 ndigit, u8 *sign, u8* o_str)
{
    u32 middle, scales;
    u8 cnt, swap, idx, mid;

    // 负数处理
    if (value < 0) 
    {
        value = -value;
        *sign = 1;
    }
    else {
        *sign = 0;
    }
    
    if (value == 0)
    {   
        o_str[0] = '0';
        cnt = 1;
        
        if (ndigit != 0)
        {
            // 添加.
            o_str[cnt] = '.'; cnt++;
            
            // 添加小数点后面的0
            for (idx = ndigit; idx != 0; idx--, cnt++) 
            {
                o_str[cnt] = '0';
            }
        }
    }   
    else
    {
        // value放大scales倍
        for (idx = ndigit, scales = 1; idx != 0; idx--) {
            scales *= 10;
        }
       
        middle = (u32)(value * (double)scales);

        // 按位移出，逆序
        for (idx = 0, cnt = 0; middle != 0; idx++)
        {
            o_str[idx] = DECIMAL_TO_ASCII(middle % 10);
            middle /= 10;
            cnt++;
            if (cnt == ndigit)
            {
                idx++;
                o_str[idx] = '.';
                cnt++;
                
                // 判断middle是否是0,，若是0，则输入的数小于1
                if (middle == 0)
                {
                    idx++;
                    o_str[idx] = '0';
                    cnt++;
                }
            }
        }

        // 正序
        mid = cnt >> 1;
        for (idx = 0; idx < mid; idx++)
        {
            swap = o_str[idx];
            o_str[idx] = o_str[cnt - idx - 1];
            o_str[cnt - idx - 1] = swap;
        }
    }

    // 字符串结束符
    o_str[cnt] = '\0';
}

/*----------------------------------------------------------------------
 *  icvt
 *
 *  Purpose: Convert signed interger to string.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void icvt(s32 value, u8 *sign, u8 *o_str)
{
    u8 cnt, swap, idx, mid;

    // 负数处理
    if (value < 0) 
    {
        value = -value;
        *sign = 1;
    }
    else {
        *sign = 0;
    }

    if (value == 0)
    {
        o_str[0] = '0';
        cnt = 1;
    }
    else 
    {
        // 按位移出，逆序
        for (idx = 0, cnt = 0; value != 0; idx++)
        {
            o_str[idx] = DECIMAL_TO_ASCII(value % 10);
            value /= 10;
            cnt++;
        }

        // 正序
        mid = cnt >> 1;
        for (idx = 0; idx < mid; idx++)
        {
            swap = o_str[idx];
            o_str[idx] = o_str[cnt - idx - 1];
            o_str[cnt - idx - 1] = swap;
        }
    }

    // 字符串结束符
    o_str[cnt] = '\0';
}
