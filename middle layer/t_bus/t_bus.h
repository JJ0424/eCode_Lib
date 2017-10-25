
/*----------------------------------------------------------------------^^-
/ File name:  t_bus.h
/ Author:     JiangJun
/ Data:       2017/10/19
/ Version:    v1.1
/-----------------------------------------------------------------------^^-
/  T-BUS receiver
/------------------------------------------------------------------------*/

#ifndef _T_BUS_H
#define _T_BUS_H

#include "main.h"


//------------------------------------------------------------
// Configuartion
//------------------------------------------------------------

// Device Define
#define TBUS_STX                        '#'

// T-BUS Function Enable/Disable
#define T_BUS_ENABLE_LOOP_SEQ           0

// T-Bus Packet Buffer Size
#define T_BUS_MAX_PAYLOAD_SIZE          520         // UNIT: Byte

// Protocal version
#define T_BUS_VER                       0x100       // v1.00


//------------------------------------------------------------
// Protocal Constants
//
// v1.1
// 
//  0       1      2      3    [4-65539]
// '#'  + size_L size_H + ID + Payload(0-65535) + checksum
//------------------------------------------------------------

#if T_BUS_ENABLE_LOOP_SEQ
  #define T_BUS_HEADER_SIZE     5
  #define T_BUS_END_SIEZ        1
#else 
  #define T_BUS_HEADER_SIZE     4
  #define T_BUS_END_SIEZ        1
#endif

#define T_BUS_FIXED_SIZE        (T_BUS_HEADER_SIZE + T_BUS_END_SIEZ)

#define T_BUS_BYTE_BUFFER_SIZE        (T_BUS_MAX_PAYLOAD_SIZE + T_BUS_HEADER_SIZE + T_BUS_END_SIEZ)  // Header + Payload + checksum(1 byte)

// T-Bus Tx Packet
typedef struct {

    u8 pkg_id;
    u16 dat_len;

#if T_BUS_ENABLE_LOOP_SEQ    
    u8 seq;
#endif

    u8 payload[T_BUS_MAX_PAYLOAD_SIZE];
    
} TBusTxPkgT;

// T-Bus Rx Packet
typedef struct {

    u8 pkg_id;
    u16 dat_len;
    
#if T_BUS_ENABLE_LOOP_SEQ    
    u8 seq;
#endif    
    
    u8 payload[T_BUS_MAX_PAYLOAD_SIZE];
    
} TBusRxPkgT;


//------------------------------------------------------------
// Port Functions(extern support)
//------------------------------------------------------------

// Tx data to hardware
typedef void (*fpTBusTx)(u8 *, u16);

// Get Rx count
typedef u16 (*fpTBusGetRxCount)(void);

// Get one byte from Hardware
typedef u8 (*fpTBusReadRxByte)(u8 *);


extern void TBusInit(fpTBusTx fp_tbus_tx, fpTBusReadRxByte fp_rx_byte, fpTBusGetRxCount fp_get_cnt);
extern void TBusEncPkg(TBusTxPkgT *pkg);
extern Bool TBusDecPkg(u8 *src, u16 len, TBusRxPkgT *pkg);
extern Bool TBusGetPkg(TBusRxPkgT *pkg);
extern void TBusPktToPload(TBusTxPkgT *pkg, u8 *payload, u16 *size);


 
#endif


//--------------------------------------------------
//---------------- End of file ---------------------
//--------------------------------------------------