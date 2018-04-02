
/*----------------------------------------------------------------------^^-
/ File name:  uart_dma_f0.c
/ Author:     JiangJun
/ Data:       2018/03/21
/ Version:    v1.2
/-----------------------------------------------------------------------^^-
/ UART DMA Driver
/ ---
/ v1.2 [2018-3-21]
/ 1. FIX: UartTx() should wait first, than copy to DMA Buffer.
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

    USART_InitTypeDef USART_InitStructure; DMA_InitTypeDef  DMA_InitStructure;
    
    USART_InitStructure.USART_BaudRate = baud_rate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx| USART_Mode_Rx;
    USART_Init(_UART_DEFINE, &USART_InitStructure);
    
    // Common DMA configuration    
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    
    // TX DMA configuration
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)UartTxBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&_UART_DEFINE->TDR;
    DMA_Init(UART_TXD_DMA_CHANNEL, &DMA_InitStructure);
    
    DMA_RemapConfig(_UART_DEFINE_DMA, UART_TXD_DMA_REMAP);

    // RX DMA configuration
    DMA_InitStructure.DMA_BufferSize = _UART_DMA_BUFFER_SIZE;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)UartRxBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&_UART_DEFINE->RDR;
    DMA_Init(UART_RXD_DMA_CHANNEL, &DMA_InitStructure);
    
    DMA_RemapConfig(_UART_DEFINE_DMA, UART_RXD_DMA_REMAP);
    
    // Enable the USART Rx and Tx DMA requests 
    USART_DMACmd(_UART_DEFINE, USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);
    
    
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
        
    DMA_Cmd(UART_RXD_DMA_CHANNEL, ENABLE);  // Rx Channel Enable     
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

    // Wait the last packet for sending over.
    ot_cnt = 40000;
    while ((DMA_GetCurrDataCounter(UART_TXD_DMA_CHANNEL) != 0)  && ot_cnt--);
    
    // data copy.
    memcpy(UartTxBuffer, src_buf, cnt);        

    // Transmit
    DMA_Cmd(UART_TXD_DMA_CHANNEL, DISABLE);
    DMA_SetCurrDataCounter(UART_TXD_DMA_CHANNEL, cnt);
    DMA_Cmd(UART_TXD_DMA_CHANNEL, ENABLE);
}

/*----------------------------------------------------------------------
 *  UartRxTimeProc
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void UartRxTimeProc(void)
{

    u16 cnt = 0; static u8 LastCnt = 0, TimCnt = 0;   
    
    //------------------------------------------------------------
    // Rx Processing
    //------------------------------------------------------------
        
    // DMA transfer complete detect. --- should never happen.
    if (DMA_GetFlagStatus(UART_RXD_DMA_TC_FLAG) == SET)
    {
        DMA_ClearFlag(UART_RXD_DMA_TC_FLAG);
        
        // DMA Rx Restart.
        DMA_Cmd(UART_RXD_DMA_CHANNEL, DISABLE);
        DMA_SetCurrDataCounter(UART_RXD_DMA_CHANNEL, _UART_DMA_BUFFER_SIZE);
        DMA_Cmd(UART_RXD_DMA_CHANNEL, ENABLE);
    }   
    
    // UART overrun detect. --- should never happen.
    if (USART_GetFlagStatus(_UART_DEFINE, USART_FLAG_ORE) == SET)
    {
        // Clear flag.
        USART_ClearFlag(_UART_DEFINE, USART_FLAG_ORE);
        USART_ReceiveData(_UART_DEFINE);
        
        // DMA Rx Restart.
        DMA_Cmd(UART_RXD_DMA_CHANNEL, DISABLE);
        DMA_SetCurrDataCounter(UART_RXD_DMA_CHANNEL, _UART_DMA_BUFFER_SIZE);
        DMA_Cmd(UART_RXD_DMA_CHANNEL, ENABLE);
       
        return;
    }

    // Over time detect.
    cnt = DMA_GetCurrDataCounter(UART_RXD_DMA_CHANNEL);
    cnt = _UART_DMA_BUFFER_SIZE - cnt;
    if ((LastCnt == cnt) && (LastCnt != 0))
    {
        TimCnt++;
    }
    LastCnt = cnt;
    
    // Detect one packet
    if (TimCnt == UartOvertime)           
    {
        DMA_Cmd(UART_RXD_DMA_CHANNEL, DISABLE);
        
        // Write Rx
        if (fpUartRxCnl != NULL) { (*fpUartRxCnl)(UartRxBuffer, LastCnt); }
        
        // Reset        
        LastCnt = 0; TimCnt = 0;
        
        // restart DMA
        DMA_SetCurrDataCounter(UART_RXD_DMA_CHANNEL, _UART_DMA_BUFFER_SIZE);
        DMA_Cmd(UART_RXD_DMA_CHANNEL, ENABLE);
        
    }
}
