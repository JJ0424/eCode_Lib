
/*----------------------------------------------------------------------^^-
/ File name:  strlib_ex.c
/ Author:     JiangJun
/ Data:       2018/11/05
/ Version:    v1.2
/-----------------------------------------------------------------------^^-
/ STRING LIB
/ ---
/ v1.1 [2018-10-31]
/ 1. add strlib.c
/ ---
/ v1.2 [2018-11-5]
/ 1. add _ex
/------------------------------------------------------------------------*/


#include "strlib_ex.h"


/*----------------------------------------------------------------------
 *  _cpause
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    return result not include '\0', "118.123456" will return 10
 *---------------------------------------------------------------------*/
u16 _cpause(u8 ch, u8 spt_idx, u8 *in, u16 ilen, u8 *out, u16 olen)
{
    u16 idx = 0, st_idx = 0, end_idx = 0; 
    u8 spt_idxcnt = 0;

    /* spt_idx should > 0 */
    if (!spt_idx) return 0;
    
    /* find index start */
    for (idx = 0; (in[idx] != '\0') && (idx < ilen); idx++)
	{
		if (in[idx] == ch) spt_idxcnt++;                        			
		if (spt_idxcnt == spt_idx) { st_idx = ++idx; break; }           /* st find */		
	}

    /* find index end */
    for (; (in[idx] != '\0') && (idx < ilen); idx++)
	{
		if (in[idx] == ch) spt_idxcnt++;           
	    if (spt_idxcnt == (spt_idx + 1)) { end_idx = idx - 1; break; }  /* end find */	
	}
    
	/* check vaild */
	if ((end_idx >= st_idx) && ((end_idx - st_idx + 1) < olen) && (spt_idxcnt > 0))
	{
	    spt_idxcnt = end_idx - st_idx + 1;                              /* byte count */
        for (idx = 0; idx < spt_idxcnt; idx++) out[idx] = in[st_idx + idx];
        out[idx] = '\0';                                       
        return spt_idxcnt;
	}
	else out[0] = '\0';	return 0;   /* pause invaild */           
}

/*----------------------------------------------------------------------
 *  _atod
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:       [1]     0.1234  -   0.1234
 *              [2]     0       -   0.0
 *              [3]     0.      -   0.0
 *              [4]     0D2.12  -   2.12
 *              [5]     0.12B34 -   0.12
 *              [6]     2.12    -   2.12
 *              [7]     2       -   2.0
 *              [8]     2.      -   2.0
 *              [9]     2D23.12 -   223.12
 *              [10]    2.12B34 -   2.12
 *              [11]    .123    -   0.123
 *              [12]    xxx0.12 -   0.12
 *---------------------------------------------------------------------*/
double _atod(u8 *src, u8 cnt)
{
    u8 idx = 0, pnt_cnt = 0, sign = 0; u32 itr = 0, pnt = 0, scale = 0;    
    double res = 0.0;    

    /* itr caculate, not include non-numerial */
    for (idx = 0; idx < cnt; idx++)
    {
        if (src[idx] == '-') { sign = 0xff; continue; }     /* N double */
        if (src[idx] == '.') break;     /* . */
        if ((src[idx] <= '9') && (src[idx] >= '0')) {       /* number */
            itr *= 10; itr += (src[idx] - 0x30);            
        } else continue;                /* non-number */
    }

    /* pnt caculate */
    for (idx++; idx < cnt; idx++)
    {
        if ((src[idx] <= '9') && (src[idx] >= '0')) {       /* number */
            pnt *= 10; pnt += (src[idx] - 0x30); 
            pnt_cnt++;
        } else break;
    }

    /* to double */
    if (pnt_cnt)
    {
        for (scale = 1; pnt_cnt != 0; pnt_cnt--) scale *= 10;   /* / scale */
            res = (double)itr + (double)((double)pnt / (double)scale);
    } else res = (double)itr;

    /* N double */
    if (sign) res = -res; return res;
}

/*----------------------------------------------------------------------
 *  _atof
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
float _atof(u8 *src, u8 cnt)
{
    u8 idx = 0, pnt_cnt = 0, sign = 0; u32 itr = 0, pnt = 0, scale = 0;    
    float res = 0.0;    

    /* itr caculate, not include non-numerial */
    for (idx = 0; idx < cnt; idx++)
    {
        if (src[idx] == '-') { sign = 0xff; continue; }     /* N double */
        if (src[idx] == '.') break;     /* . */
        if ((src[idx] <= '9') && (src[idx] >= '0')) {       /* number */
            itr *= 10; itr += (src[idx] - 0x30);            
        } else continue;                /* non-number */
    }

    /* pnt caculate */
    for (idx++; idx < cnt; idx++)
    {
        if ((src[idx] <= '9') && (src[idx] >= '0')) {       /* number */
            pnt *= 10; pnt += (src[idx] - 0x30); 
            pnt_cnt++;
        } else break;
    }

    /* to double */
    if (pnt_cnt)
    {
        for (scale = 1; pnt_cnt != 0; pnt_cnt--) scale *= 10;   /* / scale */
            res = (float)itr + (float)((float)pnt / (float)scale);
    } else res = (float)itr;

    /* N double */
    if (sign) res = -res; return res;
}

/*----------------------------------------------------------------------
 *  _atoi
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:           [1]     dafda123df  -   123
 *                  [2]     .12B34      -   12
 *                  [3]     A0          -   0
 *                  [4]     0123Cd      -   123
 *                  [5]     daf-10cd    -   -10
 *---------------------------------------------------------------------*/
s32 _atoi(u8 *src, u8 cnt)
{
    u8 idx = 0, sign = 0; s32 itr = 0;

    /* ignore non-number char */
    for (idx = 0; idx < cnt; idx++)
    {
        if (src[idx] == '-') { sign = 0xff; continue; }         /* N interger */
        if ((src[idx] <= '9') && (src[idx] >= '0')) break;      /* number */
    }
    
    /* itr caculate, not include non-numerial */
    for (; idx < cnt; idx++)
    {              
        if ((src[idx] <= '9') && (src[idx] >= '0')) {       /* number */
            itr *= 10; itr += (src[idx] - 0x30);            
        } else break;               /* non-number */
    }

    /* N interger */
    if (sign) itr = -itr; return itr;
}

/*----------------------------------------------------------------------
 *  _vpause
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u8 _vpause(u8 *src, u16 cnt, u8 *buff, u16 bfcnt, _V_PauseItemT *vitem, u8 icnt)
{

    u8 pcnt = 0, idx = 0; u16 cpidx = 0;

    /* search var */
    for (idx = 0; idx < icnt; idx++)
    {
        /* char pause */
        cpidx = _cpause(vitem[idx].ch, vitem[idx].sp_idx, src, cnt, buff, bfcnt);
        if (cpidx)
        {            
            if (vitem[idx].fmt[0] != '%') continue;     /* % check */            
            switch (vitem[idx].fmt[1])                  /* format to var */
            {
                case 'c':   /* char */
                    *(u8 *)vitem[idx]._pvar = buff[0];
                    break;
                case 'd':   /* decimal */
                    *(s32 *)vitem[idx]._pvar = _atoi(buff, cpidx);                    
                    break;
                case 'f':   /* float */
                    *(float *)vitem[idx]._pvar = _atof(buff, cpidx);
                    break;
                case 'l':   /* double */
                    if (vitem[idx].fmt[2] == 'f') *(double *)vitem[idx]._pvar = _atod(buff, cpidx);                        
                    break;                
            }
            pcnt++;
        }
    }
    return pcnt;    /* result - pause count */
}

/*----------------------------------------------------------------------
 *  _strcpy_ex
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void _strcpy_ex(u8 *src, u8 *dst)
{

    /* no limit */
    for (; *src != 0; src++)
    {
        *dst = *src;
        dst++;
    }    
    *dst = 0;
}

/*----------------------------------------------------------------------
 *  _strcat_ex
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void _strcat_ex(u8 *src, u8 *dst)
{   

    /* to the end of the dst */
    for (; *dst != 0; dst++);
    
    /* copy to dst, no limit */
    for (; *src != 0; src++)
    {
        *dst = *src;
        dst++;
    }    
    *dst = 0;
}

/*----------------------------------------------------------------------
 *  _strlen_ex
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u16 _strlen_ex(u8 *src)
{

    u16 idx = 0;
    /* no limit */
    for (idx = 0; src[idx] != '\0'; idx++);             
    return idx;    
}

/*----------------------------------------------------------------------
 *  _icvt
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:           [1]         -1              -       "-1\0"
 *                  [2]         12345678        -       "12345678\0"
 *                  [3]         -34567890       -       "-34567890\0"
 *                  [4]         0               -       "0\0"
 *                 *[5]         123456 (max 5)  -       "3456\0"
 *                 *[6]         -123456(max 5)  -       "-456\0"
 *---------------------------------------------------------------------*/
void _icvt(s32 val, u8 *out, u8 out_size)
{

    u8 idx = 0, len = 0; s32 val_tmp = 0;

    /* N/P */
    if (val < 0) {
        *out = '-'; val = -val; idx++;
    }

    /* i len, 0i - 1, 123i - 3*/
    for (val_tmp = val, len = 0; ; len++) {
        val_tmp /= 10; if (val_tmp == 0) break;     /* exit */
    } ++len;

    /* check range */
    if ((len + idx) >= out_size) len = out_size - idx - 1;

    /* '\0' ending char */
    out[len + idx] = '\0';
    
    /* inter to char */
    for (; len; len--) {               
        out[idx + len - 1] = (val % 10) + 0x30;     /* LSB */
        val /= 10;                    
    }
}

//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
