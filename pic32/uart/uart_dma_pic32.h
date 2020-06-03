
/*----------------------------------------------------------------------^^-
/ File name:  uart_dma_pic32.h
/ Author:     JiangJun
/ Data:       [2020-6-3]
/ Version:    v1.1
/-----------------------------------------------------------------------^^-
/ UART DMA driver
/------------------------------------------------------------------------*/

#ifndef _UART_DMA_PIC32_H
#define _UART_DMA_PIC32_H

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

// DMA CONFIG
typedef struct {

    DmaChannel dma_chn;
    DmaChannelPri dma_pri;      // priority with the DMA [0-3, 3 is the hightest]
    
} UartDmaT;

// Uart Parameters
typedef struct {

    // Must Set!!
    UART_MODULE uart_id;
    u32 baud_rate;
    UartDmaT uart_dma;

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
extern u16 UartTx(UartRunT *uart_dma, u8 *src_buf, u16 cnt);
extern _bool UartTxIsEmpty(UartRunT *uart_dma);
extern void UartRxTimeProc(UartRunT *uart_dma);

#endif
//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
