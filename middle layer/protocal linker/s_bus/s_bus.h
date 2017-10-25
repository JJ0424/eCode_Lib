
/*----------------------------------------------------------------------^^-
/ File name:  s_bus.h
/ Author:     JiangJun
/ Data:       2017/10/19
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ S-BUS receiver
/------------------------------------------------------------------------*/


#ifndef _S_BUS_H
#define _S_BUS_H


#include "main.h"


//------------------------------------------------------------
// Configuartion
//------------------------------------------------------------

#define _S_BUS_MAX_PAYLOAD_SIZE          (1024 + 100)         // UNIT: Byte



//------------------------------------------------------------
// Protocal Definations
//
// [v1.0]
//    0       5     6      7       8       9       10      11    12
// "#sbus" [D/C][reg0][loop_idx][size_L size_H] [xor_chk] '*' [payload]
//
//------------------------------------------------------------

// SBus header
#define _S_BUS_STX                          "#sbus"
#define _S_BUS_STX_LEN                      (sizeof(_S_BUS_STX) - 1)

// SBusHdrT - 'cmd_dat' field
#define _S_BUS_CMD                          'C'
#define _S_BUS_DAT                          'D'


/* SBusHdrT/ SBusPktT*/
#pragma pack(push)
#pragma pack(1)

typedef struct {

    u8 head[_S_BUS_STX_LEN];    
    u8 cmd_dat;                     // [D/C]

    u8 loop_idx;
    u8 reg0;

    u16 size;                       // [payload size]
    
    u8 xor_chk;                     // checksum: from [cmd_dat ~ size]
    u8 end_char;                    // '*'
    
} SBusHdrT;

typedef struct {

    SBusHdrT sbus_header;                       // 12 bytes
    
    u8 payload[_S_BUS_MAX_PAYLOAD_SIZE];        // define by extern(由其他协议定义payload内容)
    
} SBusPktT;

#pragma pack(pop)   /* SBusHdrT / SBusPktT */


#define _S_BUS_BYTE_BUFFER_SIZE        (sizeof(SBusHdrT) + _S_BUS_MAX_PAYLOAD_SIZE)  // Header + Payload



//------------------------------------------------------------
// Port Functions(extern support)
//------------------------------------------------------------

// Tx data to hardware
typedef void (*fpSBusTxBytes)(u8 *, u16);

// Get Rx count
typedef u16 (*fpSBusGetRxCount)(void);

// Get bytes from Hardware
typedef u16 (*fpSBusRxBytes)(u8 *, u16);

// Reset Rx FIFO
typedef void (*fpSBusResetRx)(void);


extern void SBusInit(fpSBusTxBytes, fpSBusRxBytes, fpSBusGetRxCount, fpSBusResetRx);
extern void SBusTxPayload(u8 cmd_dat, u8 *payload, u16 size);
extern Bool SBusPausePkt(SBusPktT *pkg);
extern Bool SBusArrayToPkt(u8 *sbus_buf, u16 cnt, SBusPktT *pkg);


#endif /* _S_BUS_H */


//--------------------------------------------------
//---------------- End of file ---------------------
//--------------------------------------------------

