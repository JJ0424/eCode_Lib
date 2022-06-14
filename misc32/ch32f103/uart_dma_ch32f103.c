
/*----------------------------------------------------------------------^^-
/ File name:  uart_dma_ch32f103.c
/ Author:     JiangJun
/ Data:       [2022-6-13]
/ Version:    v1.3
/-----------------------------------------------------------------------^^-
/ ch32f103 uart driver based by STM32F103
/ ---
/ v1.2 [2019-1-3]
/ 1. add 19200 baudrate support
/ ---
/ v1.3 [2022-6-13]
/ 1. FIX: overtime bug
/ 2. ADD: 4800/ 2400 baudrate
/------------------------------------------------------------------------*/


#include "uart_dma_ch32f103.h"
#include "string.h"


//------------------------------------------------------------
//                      DMA CONST
//------------------------------------------------------------

// DMA Channel
typedef struct {

    // TXD
    DMA_Channel_TypeDef *tx_channel;        // channel
    u32 tx_TC_flag;                         // flag

    // RXD
    DMA_Channel_TypeDef *rx_channel;        // channel                   
    u32 rx_TC_flag;                         // flag
    
} UartDmaT;

// Table
const UartDmaT UartDmaConst[3] = 
{

    // UART1
    {
        DMA1_Channel4,          // TXD
        DMA1_FLAG_TC4,

        DMA1_Channel5,          // RXD
        DMA1_FLAG_TC5
    },

    // UART2
    {
        DMA1_Channel7,          // TXD
        DMA1_FLAG_TC7,

        DMA1_Channel6,          // RXD
        DMA1_FLAG_TC6,
    },

    // UART3
    {
        DMA1_Channel2,          // TXD
        DMA1_FLAG_TC2,

        DMA1_Channel3,          // RXD
        DMA1_FLAG_TC3,
    }
};

/*----------------------------------------------------------------------
 *  _uart_get_const_idx
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
s8 _uart_get_const_idx(USART_TypeDef *USARTx)
{

    if ((u32)(USARTx) == USART1_BASE) {

        return 0;
    } 
    else if ((u32)(USARTx) == USART2_BASE) {

        return 1;
    }
    else if ((u32)(USARTx) == USART3_BASE) {

        return 2;
    }
    else {

        return -1;
    }
}

/*----------------------------------------------------------------------
 *  UartInit
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void UartInit(UartRunT *uart_dma)
{

    USART_InitTypeDef USART_InitStructure; DMA_InitTypeDef  DMA_InitStructure;
    s8 idx = _uart_get_const_idx(uart_dma->USARTx);

    // Index check
    if (idx < 0) { return; }

    
    //------------------------------------------------------------
    //              DeInit DMA/UART
    //------------------------------------------------------------

    DMA_DeInit(UartDmaConst[idx].tx_channel);
    DMA_DeInit(UartDmaConst[idx].rx_channel);

    USART_DeInit(uart_dma->USARTx);

    
    //------------------------------------------------------------
    //              Init DMA/UART
    //------------------------------------------------------------

    /* DMAyChanlex configured as follows: 
        - Normal mode
        - Memory Data Size: Byte
        - Priority: VeryHigh
    */
        
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(uart_dma->USARTx->DATAR);                  
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                      
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)uart_dma->tx_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 0;   
    DMA_Init(UartDmaConst[idx].tx_channel, &DMA_InitStructure);
    
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)uart_dma->rx_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = _UART_DMA_BUFFER_SIZE;   
    DMA_Init(UartDmaConst[idx].rx_channel, &DMA_InitStructure);

    
    /* USARTx configured as follows:
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
    */
    
    USART_InitStructure.USART_BaudRate = uart_dma->baud_rate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(uart_dma->USARTx, &USART_InitStructure);
    
    // Enable the USART Rx and Tx DMA requests 
    USART_DMACmd(uart_dma->USARTx, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);


    //------------------------------------------------------------
    //              OVERTIME
    //------------------------------------------------------------

    if (uart_dma->baud_rate == 115200) { 

        // 1 ms
        uart_dma->overtime.max_time = ( 1 / _UART_DMA_RX_TIMEPROC ); 
    }
    else if (uart_dma->baud_rate == 9600) { 

        // 5 ms
        uart_dma->overtime.max_time = ( 5 / _UART_DMA_RX_TIMEPROC ); 
    }
    else if (uart_dma->baud_rate == 19200) {

        // 3 ms
        uart_dma->overtime.max_time = ( 3 / _UART_DMA_RX_TIMEPROC ); 
    }
    else if (uart_dma->baud_rate == 2400) {

        // 20 ms
        uart_dma->overtime.max_time = ( 20 / _UART_DMA_RX_TIMEPROC ); 
    }
    else if (uart_dma->baud_rate == 4800) {

        // 10 ms
        uart_dma->overtime.max_time = ( 10 / _UART_DMA_RX_TIMEPROC ); 
    }
    else {

        // default: 5 ms
        uart_dma->overtime.max_time = ( 5 / _UART_DMA_RX_TIMEPROC );
    }
}

/*----------------------------------------------------------------------
 *  UartOpen
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void UartOpen(UartRunT *uart_dma)
{

    s8 idx = _uart_get_const_idx(uart_dma->USARTx);

    // Index check
    if (idx < 0) { return; }
    
    // Enable Rx Channel
    DMA_Cmd(UartDmaConst[idx].rx_channel, ENABLE);

    // Enable UART
    USART_Cmd(uart_dma->USARTx, ENABLE);
}

/*----------------------------------------------------------------------
 *  UartStop
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void UartStop(UartRunT *uart_dma)
{

    s8 idx = _uart_get_const_idx(uart_dma->USARTx);

    // Index check
    if (idx < 0) { return; }
    
    // Disable Rx Channel
    DMA_Cmd(UartDmaConst[idx].rx_channel, DISABLE);

    // Disable UART
    USART_Cmd(uart_dma->USARTx, DISABLE);
}

/*----------------------------------------------------------------------
 *  UartTx
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u16 UartTx(UartRunT *uart_dma, u8 *src_buf, u16 cnt)
{   

    s8 idx = _uart_get_const_idx(uart_dma->USARTx);

    // Index check
    if (idx < 0) { return 0; }
    
    // check cnt
    if (cnt == 0) { return 0; }

    // if last packet not send-over, loss this packet
    if (DMA_GetCurrDataCounter(UartDmaConst[idx].tx_channel) != 0) {

        return 0;
    }

    // data copy.
    memcpy(uart_dma->tx_buffer, src_buf, cnt);

    // Txd
    DMA_ClearFlag(UartDmaConst[idx].tx_TC_flag);
    DMA_Cmd(UartDmaConst[idx].tx_channel, DISABLE);
    DMA_SetCurrDataCounter(UartDmaConst[idx].tx_channel, cnt);
    DMA_Cmd(UartDmaConst[idx].tx_channel, ENABLE);

    // return count of Txd
    return cnt;
}

/*----------------------------------------------------------------------
 *  UartTxIsEmpty
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_bool UartTxIsEmpty(UartRunT *uart_dma)
{

    s8 idx = _uart_get_const_idx(uart_dma->USARTx);

    // Index check
    if (idx < 0) { return FALSE; }

    // DMA BuF check
    if (DMA_GetCurrDataCounter(UartDmaConst[idx].tx_channel) == 0) {

        return TRUE;
    }

    return FALSE;
}
 
/*----------------------------------------------------------------------
 *  UartRxTimeProc - 5ms called
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void UartRxTimeProc(UartRunT *uart_dma)
{

    u16 cnt = 0; s8 idx = _uart_get_const_idx(uart_dma->USARTx);

    // Index check
    if (idx < 0) { return; }


    //------------------------------------------------------------
    //              FLAG
    //------------------------------------------------------------

    if (DMA_GetFlagStatus(UartDmaConst[idx].rx_TC_flag) == SET)
    {
    
        DMA_ClearFlag(UartDmaConst[idx].rx_TC_flag);        
        DMA_Cmd(UartDmaConst[idx].rx_channel, DISABLE);
        DMA_SetCurrDataCounter(UartDmaConst[idx].rx_channel, _UART_DMA_BUFFER_SIZE);
        DMA_Cmd(UartDmaConst[idx].rx_channel, ENABLE);
    }

    
    if (USART_GetFlagStatus(uart_dma->USARTx, USART_FLAG_ORE) == SET)
    {
    
        USART_ClearFlag(uart_dma->USARTx, USART_FLAG_ORE);
        USART_ReceiveData(uart_dma->USARTx);
        DMA_Cmd(UartDmaConst[idx].rx_channel, DISABLE);
        DMA_SetCurrDataCounter(UartDmaConst[idx].rx_channel, _UART_DMA_BUFFER_SIZE);
        DMA_Cmd(UartDmaConst[idx].rx_channel, ENABLE);
    }


    //------------------------------------------------------------
    //              OVERTIME
    //------------------------------------------------------------

    // Get the DMA count
    cnt = DMA_GetCurrDataCounter(UartDmaConst[idx].rx_channel); 
    cnt = _UART_DMA_BUFFER_SIZE - cnt;

    // 'pass_time' ++
    if ((uart_dma->overtime.last_cnt == cnt) && (uart_dma->overtime.last_cnt != 0)) uart_dma->overtime.pass_time++;
    else uart_dma->overtime.pass_time = 0;    
    uart_dma->overtime.last_cnt = cnt;


    //------------------------------------------------------------
    //              Detected IF Overtime
    //------------------------------------------------------------
    
    if (uart_dma->overtime.pass_time >= uart_dma->overtime.max_time)
    {

        // disable DMA Rx
        DMA_Cmd(UartDmaConst[idx].rx_channel, DISABLE);

        // write Rx
        if (uart_dma->fp_rx_output != NULL) { (*(uart_dma->fp_rx_output))(uart_dma->rx_buffer, uart_dma->overtime.last_cnt); }
        
        // reset        
        uart_dma->overtime.last_cnt = 0; uart_dma->overtime.pass_time = 0;
        DMA_SetCurrDataCounter(UartDmaConst[idx].rx_channel, _UART_DMA_BUFFER_SIZE);
        DMA_Cmd(UartDmaConst[idx].rx_channel, ENABLE);

        // [LED...]
        if (uart_dma->fp_rx_trigger != NULL) { (*(uart_dma->fp_rx_trigger))(); }
    }
}


//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
