
/*----------------------------------------------------------------------^^-
/ File name:  uart_dma.h
/ Author:     JiangJun
/ Data:       2018/03/21
/ Version:    v1.2
/-----------------------------------------------------------------------^^-
/ UART DMA Driver
/------------------------------------------------------------------------*/


#ifndef _UART_DMA_H
#define _UART_DMA_H

#include "hconfig.h"
#include "sysconfig.h"


//------------------------------------------------------------
// CONFIG
//------------------------------------------------------------

// DMA Buffer size. unit: byte
#define _UART_DMA_BUFFER_SIZE           200

// Rx Timerproc, unit: ms
#define _UART_DMA_RX_TIMEPROC           5    

// UART UESD
#define _UART_DEFINE                    T_BUS_UART
#define _UART_DEFINE_DMA                T_BUS_UART_DMA


extern void UartInit(u32 baud_rate);
extern void UartOpen(void (*rx_channel)(u8 *src, u16 cnt));
extern void UartTx(u8 *src_buf, u16 cnt);
extern void UartRxTimeProc(void);


#endif /* _UART_DMA_H */
