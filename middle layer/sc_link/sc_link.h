
/*----------------------------------------------------------------------^^-
/ File name:  sc_link.h
/ Author:     JiangJun
/ Data:       2018/9/20
/ Version:    v1.1
/-----------------------------------------------------------------------^^-
/ sx_link driver
/------------------------------------------------------------------------*/

#ifndef _SC_LINK_H
#define _SC_LINK_H

#include "main.h"

//------------------------------------------------------------
//                      ERROR
//------------------------------------------------------------

#define _SC_RES_NVE             -1      // No Vaild Event
#define _SC_RES_NEC             -2      // No End Char
#define _SC_RES_ECO             -3      // End Char OverRun
#define _SC_RES_XRE             -4      // X-OR Error

//------------------------------------------------------------
//                  STR LENGTH
//------------------------------------------------------------

#define __SC_STR_LEN_SIZEOF(s)              (sizeof(s) - 1)

//------------------------------------------------------------
//                      TYPEDEF
//------------------------------------------------------------

// Event Tx Pointer
typedef void (*fpEvtTx)(u8 *, u16);

// Event Table
typedef struct {
    
    u8 *p_hdr;
    u8 hdr_len;
    u8 en_xor_chk;      // Not Inc '#'/ '$' and '*'
    u8 en_call_back;
    u8 en_mix_cod;      // Mix code mode, &STR, 123*321, 123,*34\r\n
    
    // Call Back
    void (*pevt)(u8 *, u16, fpEvtTx);    
    
} ScLinkEvtT;

// Work BuF
typedef struct {

    u8 end_char;

    // Event Table
    u8 tab_len;
    const ScLinkEvtT *evt_tab;

    // Call Back tx
    fpEvtTx pevt_tx;
    
} _ScLinkWorkT;

extern void ScLinkInit(_ScLinkWorkT *work, const ScLinkEvtT *evt_tab, u8 tab_len, u8 end_char, fpEvtTx pevt_tx);
extern s8 ScLinkPausePkt(_ScLinkWorkT *work, u8 *src, u16 cnt, u16 pkt_max_size, u16 *edc_idx);
extern u16 ScLinkTxPkt(u8 *src, u16 src_size, u16 buff_size);

#endif
//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
