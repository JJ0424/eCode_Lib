
/*----------------------------------------------------------------------^^-
/ File name:  uart_dma.c
/ Author:     JiangJun
/ Data:       2017/08/02
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/  UART DMA Driver
/------------------------------------------------------------------------*/

#include "main.h"
#include "uart_dma.h"
#include "string.h"


//------------------------------------------------------------
// Configration: Overtime, unit: ms
//------------------------------------------------------------

static u8 UartOvertime = 0;

//------------------------------------------------------------
// Buffer with DMA UART
//------------------------------------------------------------
static u8 UartRxBuffer[_UART_DMA_BUFFER_SIZE];
static u8 UartTxBuffer[_UART_DMA_BUFFER_SIZE];

// Rx Channel pointer
static void (*fpUartRxCnl)(u8 *src, u16 cnt);


/*----------------------------------------------------------------------
 *  UartInit
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void UartInit(u32 baud_rate)
{
    DMA_InitTypeDef uart_dma;
    USART_InitTypeDef uart;

    DMA_DeInit(RF_UART_RXD_DMA_CHANNEL);
    DMA_DeInit(RF_UART_TXD_DMA_CHANNEL);
    
    // initialize the DMA
    uart_dma.DMA_PeripheralBaseAddr = (u32)&_UART_DEFINE->DR;    // RX
    uart_dma.DMA_MemoryBaseAddr = (u32)UartRxBuffer;
    uart_dma.DMA_DIR = DMA_DIR_PeripheralSRC;
    uart_dma.DMA_BufferSize = _UART_DMA_BUFFER_SIZE;               // never complete the DMA transfer.
    uart_dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    uart_dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
    uart_dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    uart_dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    uart_dma.DMA_Mode = DMA_Mode_Normal;                        // normal mode.
    uart_dma.DMA_Priority = DMA_Priority_VeryHigh;
    uart_dma.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(RF_UART_RXD_DMA_CHANNEL, &uart_dma);
    
    uart_dma.DMA_PeripheralBaseAddr = (u32)&_UART_DEFINE->DR;;    // TX
    uart_dma.DMA_MemoryBaseAddr = (u32)UartTxBuffer;
    uart_dma.DMA_DIR = DMA_DIR_PeripheralDST;
    uart_dma.DMA_BufferSize = 0;
    DMA_Init(RF_UART_TXD_DMA_CHANNEL, &uart_dma);


    USART_DeInit(_UART_DEFINE);
    
    // initialize the USART
    uart.USART_BaudRate = baud_rate;
    uart.USART_WordLength = USART_WordLength_8b;
    uart.USART_StopBits = USART_StopBits_1;
    uart.USART_Parity = USART_Parity_No;
    uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    uart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(_UART_DEFINE, &uart);
    
    // Enable the USART Rx and Tx DMA requests 
    USART_DMACmd(_UART_DEFINE, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);    // enable the USART DMA request
    
    
    // Overtime Cala
    if (baud_rate == 115200) { UartOvertime = ( 5 / _UART_DMA_RX_TIMEPROC ); }      // 5 ms
    else if (baud_rate == 9600) { UartOvertime = ( 20 / _UART_DMA_RX_TIMEPROC ); }  // 20 ms
}

/*----------------------------------------------------------------------
 *  UartOpen
 *
 *  Purpose: None.
 *  Entry:   rx_channel - Rx Buffer - Rx Channel(maybe FIFO...)
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void UartOpen(void (*rx_channel)(u8 *src, u16 cnt))
{

    if (rx_channel != NULL) { fpUartRxCnl = rx_channel; }
    
    DMA_Cmd(RF_UART_RXD_DMA_CHANNEL, ENABLE); 
    USART_Cmd(_UART_DEFINE, ENABLE);
}

/*----------------------------------------------------------------------
 *  UartTx
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void UartTx(u8 *src_buf, u16 cnt)
{   

    volatile u16 ot_cnt = 0; 
    
    // check cnt
    if (cnt == 0) { return; }
    
    // data copy.
    memcpy(UartTxBuffer, src_buf, cnt);
    
    // Wait the last packet for sending over.
    ot_cnt = 40000;
    while ((DMA_GetCurrDataCounter(RF_UART_TXD_DMA_CHANNEL) != 0)  && ot_cnt--);

    // Transmit
    DMA_Cmd(RF_UART_TXD_DMA_CHANNEL, DISABLE);
    DMA_SetCurrDataCounter(RF_UART_TXD_DMA_CHANNEL, cnt);
    DMA_Cmd(RF_UART_TXD_DMA_CHANNEL, ENABLE);
}
 
/*----------------------------------------------------------------------
 *  UartRxTimeProc - 5ms called
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void UartRxTimeProc(void)
{

    u16 cnt = 0; static u8 LastCnt = 0, TimCnt;
                
    //------------------------------------------------------------
    // Rx Processing
    //------------------------------------------------------------
        
    // DMA transfer complete detect. --- should never happen.
    if (DMA_GetFlagStatus(RF_UART_RXD_DMA_TC_FLAG) == SET)
    {
        DMA_ClearFlag(RF_UART_RXD_DMA_TC_FLAG);
        
        // DMA restart.
        DMA_Cmd(RF_UART_RXD_DMA_CHANNEL, DISABLE);
        DMA_SetCurrDataCounter(RF_UART_RXD_DMA_CHANNEL, _UART_DMA_BUFFER_SIZE);
        DMA_Cmd(RF_UART_RXD_DMA_CHANNEL, ENABLE);
    }   
    
    // UART overrun detect. --- should never happen.
    if (USART_GetFlagStatus(_UART_DEFINE, USART_FLAG_ORE) == SET)
    {
        // Clear flag.
        USART_ClearFlag(_UART_DEFINE, USART_FLAG_ORE);
        USART_ReceiveData(_UART_DEFINE);
        
        // DMA restart.
        DMA_Cmd(RF_UART_RXD_DMA_CHANNEL, DISABLE);
        DMA_SetCurrDataCounter(RF_UART_RXD_DMA_CHANNEL, _UART_DMA_BUFFER_SIZE);
        DMA_Cmd(RF_UART_RXD_DMA_CHANNEL, ENABLE);
       
        return;
    }

    // Over time detect.
    cnt = DMA_GetCurrDataCounter(RF_UART_RXD_DMA_CHANNEL);
    cnt = _UART_DMA_BUFFER_SIZE - cnt;
    if ((LastCnt == cnt) && (LastCnt != 0))
    {
        TimCnt++;
    }
    LastCnt = cnt;
    
    if (TimCnt == UartOvertime)               // 'UART_PACKET_OVERTIME' overtime if the interrupt is 1ms period.
    {
        DMA_Cmd(RF_UART_RXD_DMA_CHANNEL, DISABLE);
        
        // Write Rx
        if (fpUartRxCnl != NULL) { (*fpUartRxCnl)(UartRxBuffer, LastCnt); }
        
        // Reset        
        LastCnt = 0; TimCnt = 0;
        
        // restart DMA
        DMA_SetCurrDataCounter(RF_UART_RXD_DMA_CHANNEL, _UART_DMA_BUFFER_SIZE);
        DMA_Cmd(RF_UART_RXD_DMA_CHANNEL, ENABLE);
        
    }
}
 