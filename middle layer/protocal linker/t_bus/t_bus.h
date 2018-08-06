
/*----------------------------------------------------------------------^^-
/ File name:  t_bus.h
/ Author:     JiangJun
/ Data:       2018/08/03
/ Version:    v2.0
/-----------------------------------------------------------------------^^-
/ T-BUS Driver
/------------------------------------------------------------------------*/

#ifndef _T_BUS_H
#define _T_BUS_H

#include "main.h"


//------------------------------------------------------------
//                      CONFIG
//------------------------------------------------------------

// Protocal version
#define _T_BUS_VER                  0x20        // v2.0


//------------------------------------------------------------
// Port Functions(extern support)
//------------------------------------------------------------

// Tx bytes to Hardware
typedef void (*fpTBusTx)(u8 *, u16);

// Get Rx count
typedef u16 (*fpTBusGetRxCount)(void);

// Rx bytes from Hardware
typedef u16 (*fpTBusReadRxByte)(u8 *, u16);


//------------------------------------------------------------
// Protocal Constants
//
// v2.0
// 
//      N        N+1    N+2        N+3       N+4     N+5                 (1)
//  ["HEADER"] [size_L size_H] [loop_idx] [ver_ID] [pkt_ID] [payload] [xor_chk]
//------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

// T-Bus Fix Field
typedef struct {

    u16 size;
    u8 loop_idx;
    u8 ver_ID;
    u8 pkt_ID;
    
} TBusFixFieldT;

// T-Bus Packet
typedef struct {

    u8 *head; u8 head_size;
    TBusFixFieldT fix;
    u8 *payload;
    u8 xor_chk;
    
} TBusPktT;

#pragma pack(pop) /* TBusPktT */

// T-Bus Work
typedef struct {

    // must set !!
    u8 *head;
    u8 head_size;
    u8 *tx_buff, *rx_buff;
    u16 buff_len;

    // w/r pointer, must set !!
    fpTBusTx fp_tx;    
    fpTBusReadRxByte fp_rx;
    fpTBusGetRxCount fp_rx_cnt;

    // loop index, not set.
    u8 loop_idx;

    // pause var, not set.
    u16 rx_cnt, pld_len;
        
} TBusWorkT;

extern void TBusInit(TBusWorkT *work, const u8 *header);
extern _bool TBusTxPkt(TBusWorkT *work, u8 pkt_id, u8 *src, u16 cnt);
extern _bool TBusArrayToPkt(u8 *header, u8 *src, u16 cnt, TBusPktT *pkt);
extern _bool TBusPausePkt(TBusWorkT *work, TBusPktT *pkt);
 
#endif

//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//