
/*----------------------------------------------------------------------^^-
/ File name:  uart_dma_ch32f103.h
/ Author:     JiangJun
/ Data:       [2022-6-13]
/ Version:    v1.3
/-----------------------------------------------------------------------^^-
/ ch32f103 uart driver based by STM32F103
/------------------------------------------------------------------------*/

#ifndef _UART_DMA_CH32F103_H
#define _UART_DMA_CH32F103_H

#include "main.h"

//------------------------------------------------------------
//                      CONFIG
//------------------------------------------------------------

// DMA Buffer size. unit: byte
#define _UART_DMA_BUFFER_SIZE               240

// Rx Timerproc, unit: ms
#define _UART_DMA_RX_TIMEPROC               1

//------------------------------------------------------------
//                      UART Typedef
//------------------------------------------------------------

// Rx Overtime Output Chnanel
typedef void (*fpUartRxCnl)(u8 *src, u16 cnt);

// Rx Overtime Trigger [LED...]
typedef void (*fpUartRxTrigger)(void);

// Overtime runtime var
typedef struct {

    u16 max_time;
    u16 last_cnt;
    u16 pass_time;
    
} UartOvertimeT;

// Uart Parameters
typedef struct {

    // Must Set!!
    USART_TypeDef *USARTx;
    u32 baud_rate;

    // Public TX Buffer
    u8 tx_buffer[_UART_DMA_BUFFER_SIZE];

    // Private RX Buffer
    u8 rx_buffer[_UART_DMA_BUFFER_SIZE];

    // Can Not Set.
    fpUartRxCnl fp_rx_output;
    fpUartRxTrigger fp_rx_trigger;

    // Not Need Set!!
    UartOvertimeT overtime;
    
} UartRunT;

extern void UartInit(UartRunT *uart_dma);
extern void UartOpen(UartRunT *uart_dma);
extern void UartStop(UartRunT *uart_dma);
extern u16 UartTx(UartRunT *uart_dma, u8 *src_buf, u16 cnt);
extern _bool UartTxIsEmpty(UartRunT *uart_dma);
extern void UartRxTimeProc(UartRunT *uart_dma);

#endif

//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
