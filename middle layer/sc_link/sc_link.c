
/*----------------------------------------------------------------------^^-
/ File name:  sc_link.c
/ Author:     JiangJun
/ Data:       2018/9/20
/ Version:    v1.1
/-----------------------------------------------------------------------^^-
/ sx_link driver
/------------------------------------------------------------------------*/


#include "sc_link.h"


//------------------------------------------------------------
//              Hex2Ascii
//------------------------------------------------------------

static const u8 _Hex2Asc_Tab[] = "0123456789ABCDEF";


/*----------------------------------------------------------------------
 *  _xor_chksum
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u8 _xor_chksum(u8 *input, u16 length)
{
    u16 idx = 0; u8 xorResult = 0;
        
    xorResult = input[0];
    for (idx = 1; idx < length; idx++) {        
        xorResult ^= input[idx];
    }    
    return xorResult;
}

/*----------------------------------------------------------------------
 *  char_cmpstr
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u8 _cmpstr(u8 *str1, u8 *str2, u16 len)
{
    u16 idx = 0;
    
    for (idx = 0; idx < len; idx++) {
        if (str1[idx] != str2[idx]) break;
    }   
    if (idx == len) return 0xFF;    
    return 0;    
}

/*----------------------------------------------------------------------
 *  _stridx
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
s16 _stridx(u8 *str, u16 str_len, const u8 *hdr, u8 hdr_len)
{
    u16 idx_s = 0, idx_comp = 0;       
    for (idx_s = 0; idx_s < (str_len - hdr_len + 1); idx_s++)
    {
        for (idx_comp = 0; idx_comp < hdr_len; idx_comp++) {
            if (str[idx_s + idx_comp] != hdr[idx_comp]) break;
        }        
        if (idx_comp == hdr_len) return idx_s;
    }            
    return -1; 
}

/*----------------------------------------------------------------------
 *  _hdrcmp
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
s16 _hdrcmp(u8 *str, const u8 *hdr, u8 hdr_len)
{
    u16 idx = 0;
    for (idx = 0; idx < hdr_len; idx++)
    {
        if (str[idx] != hdr[idx]) break;
    }
    if (idx >= hdr_len) return hdr_len;
    else return -1;
}

/*----------------------------------------------------------------------
 *  ScLinkInit
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void ScLinkInit(_ScLinkWorkT *work, const ScLinkEvtT *evt_tab, u8 tab_len, u8 end_char, fpEvtTx pevt_tx)
{

    work->end_char = end_char;
    work->evt_tab = evt_tab;
    work->tab_len = tab_len;
    work->pevt_tx = pevt_tx;
}

/*----------------------------------------------------------------------
 *  ScLinkPausePkt - pause 1 packet in buff
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    Example: ($GPRMC) (*)
 *
 *                      (1) &GPRMC,123,345*23\r\n
 *                      (2) &GPRMC,123,345*23
 *                      (3) abc&GPRMC,123,345*23\r\n
 *                      (3) abc&GPRMC,123,345*23###
 *---------------------------------------------------------------------*/
s8 ScLinkPausePkt(_ScLinkWorkT *work, u8 *src, u16 cnt, u16 pkt_max_size, u16 *edc_idx)
{

    s16 idx = 0, sc_idx = 0, ec_idx = 0, evt_idx = -1; u8 xor = 0;    

    /* find header */
    for (sc_idx = 0; sc_idx < cnt; sc_idx++)
    {        
        /* search table */
        for (idx = 0; idx < work->tab_len; idx++)                       /* cnt * tab_len - loop */
        {
            if ((cnt - sc_idx) < work->evt_tab[idx].hdr_len) continue;  /* overrun check */
            if (_hdrcmp(src + sc_idx, work->evt_tab[idx].p_hdr, work->evt_tab[idx].hdr_len) > 0) {  /* cmp header */
                evt_idx = idx; break; 
            }
        }
        if (idx < work->tab_len) break;     /* string find */
    }
    if (evt_idx < 0) return _SC_RES_NVE;    /* no vaild event */

    /* find end char */
    for (idx = 0; (idx < pkt_max_size) && (idx < (cnt - sc_idx)); idx++)
    {
        if (src[idx + sc_idx] == work->end_char) 
        {
            ec_idx = idx + sc_idx;
            if (!work->evt_tab[evt_idx].en_mix_cod) break;  /* mix-code mode */
        }
    }
    *edc_idx = ec_idx;                                      /* end char index [ exclude "x-or" ] */
    if (ec_idx <= sc_idx) return _SC_RES_NEC;               /* no vaild end-char */
        
    /* xor-check */
    if (work->evt_tab[evt_idx].en_xor_chk)
    {
        if (ec_idx >= (cnt - 2)) return _SC_RES_ECO;    /* overrun check */
        xor = _xor_chksum(&src[sc_idx + 1], ec_idx - sc_idx - 1);               /* not inc 's'/ 'e' char */
        if (_Hex2Asc_Tab[xor >> 4] != src[ec_idx + 1]) return _SC_RES_XRE;      /* xor check */
        if (_Hex2Asc_Tab[xor & 0x0F] != src[ec_idx + 2]) return _SC_RES_XRE;    /* xor check */
    }    

    /* call back */
    if (work->evt_tab[evt_idx].en_call_back) {
        if (work->evt_tab[evt_idx].pevt != NULL)    /* call-back vaild */
            (*work->evt_tab[evt_idx].pevt)(src + sc_idx, ec_idx - sc_idx + 1, work->pevt_tx);        
    }
    return evt_idx;
}

/*----------------------------------------------------------------------
 *  ScLinkTxPkt - Tx src with x-or + "\r\n"
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u16 ScLinkTxPkt(u8 *src, u16 src_size, u16 buff_size)
{
    u8 xor = 0;
    if ((src_size + 4) > buff_size) return 0;    
    xor = _xor_chksum(src + 1, src_size - 2);               /* x-or [ not inc '$','*' ]*/
    src[src_size] = _Hex2Asc_Tab[xor >> 4];
    src[src_size + 1] = _Hex2Asc_Tab[xor & 0x0F];           /* to ascii */
    src[src_size + 2] = '\r'; src[src_size + 3] = '\n';     /* \r\n */
    return src_size + 4;
}

//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
