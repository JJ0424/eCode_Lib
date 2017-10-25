
/*------------------------------------------------------^-^/
/ File name:  s_bus_driver.c
/ Author:     JiangJun
/ Data:       2017/10/23
/ Version:    V0.0
/-------------------------------------------------------^-^/
/ S-BUS Driver
/---------------------------------------------------------*/


#ifndef _S_BUS_DRIVER_H
#define _S_BUS_DRIVER_H


#include "main.h"


//------------------------------------------------------------
// Driver Config
//------------------------------------------------------------

#define S_BUS_DRIVER_TIME_PROC          1       // 1ms
#define S_BUS_DRIVER_WAIT_FOR_ACK_MAX   3000    // 3s    


//------------------------------------------------------------
// Port Functions(extern support)
//------------------------------------------------------------

// Tx data to hardware
typedef void (*fpSpiTxBytes)(u8 *, u16);

// Get bytes from Hardware
typedef u16 (*fpSpiRxBytes)(u8 *, u16);

// Exchanage Byte
typedef u8 (*fpSpiExByte)(u8);

// Cs(0: ON, 1: OFF)
typedef void (*fpSpiCs)(u8);


extern void SBusDriverInit(fpSpiTxBytes fp_tx, fpSpiRxBytes fp_rx, fpSpiExByte fp_ex_byte, fpSpiCs cs);
extern void SBusDriverTimeProc(void);
extern Bool SBusDrSendDataWaitAck(u8 *input, u16 cmd_len, u8 *output, u16 ack_len);
extern Bool SBusDriverWaitAck(u8 *output, u16 ack_len);


#endif /* _S_BUS_DRIVER_H */


//--------------------------------------------------
//---------------- End of file ---------------------
//--------------------------------------------------
