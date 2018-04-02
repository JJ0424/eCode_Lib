
/*----------------------------------------------------------------------^^-
/ File name:  uart_dma.h
/ Author:     JiangJun
/ Data:       2017/08/02
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/  UART DMA Driver
/------------------------------------------------------------------------*/

#ifndef _UART_DMA_H
#define _UART_DMA_H


#include "sysconfig.h"
#include "hconfig.h"


//------------------------------------------------------------
// CONFIG
//------------------------------------------------------------

// DMA Buffer size. unit: byte
#define _UART_DMA_BUFFER_SIZE           200

// Rx Timerproc, unit: ms
#define _UART_DMA_RX_TIMEPROC           5    

// UART UESD
#define _UART_DEFINE                    RF_LINKER_UART
#define _UART_DEFINE_DMA                RF_LINKER_UART_DMA


extern void UartInit(u32 baud_rate);
extern void UartOpen(void (*rx_channel)(u8 *src, u16 cnt));
extern void UartTx(u8 *src_buf, u16 cnt);
extern void UartRxTimeProc(void);

#endif
