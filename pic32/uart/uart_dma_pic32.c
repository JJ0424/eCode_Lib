
/*----------------------------------------------------------------------^^-
/ File name:  uart_dma_pic32.c
/ Author:     JiangJun
/ Data:       [2020-6-3]
/ Version:    v1.1
/-----------------------------------------------------------------------^^-
/ UART DMA driver
/ ---
/ v1.1 [2020-6-3]
/ 1. add UartTxIsEmpty()
/------------------------------------------------------------------------*/


#include "uart_dma_pic32.h"


//------------------------------------------------------------
//              UART FLAG AND IRQ
//------------------------------------------------------------

typedef struct {

    u8 tx_irq, rx_irq;
    void *tx_reg, *rx_reg;
    
} _UART_FLAG;

// Table
const _UART_FLAG _UART_FLAG_Table[2] = 
{

    // UART1
    { _UART1_TX_IRQ, _UART1_RX_IRQ, (void *)&U1TXREG, (void *)&U1RXREG },

    // UART2
    { _UART2_TX_IRQ, _UART2_RX_IRQ, (void *)&U2TXREG, (void *)&U2RXREG },
};


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

    //------------------------------------------------------------
    //              Init UART
    //------------------------------------------------------------

    // Tx/Rx pins only
    UARTConfigure(uart_dma->uart_id, UART_ENABLE_PINS_TX_RX_ONLY);
    UARTSetLineControl(uart_dma->uart_id, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);

    // peripheral clock with the baudrate
    UARTSetDataRate(uart_dma->uart_id, _PIC32_PBCLK, uart_dma->baud_rate);  


    //------------------------------------------------------------
    //              Init DMA
    //------------------------------------------------------------

    // dma channel config, basci mode
    DmaChnConfigure(uart_dma->uart_dma.dma_chn, uart_dma->uart_dma.dma_pri, DMA_CONFIG_DEFAULT);

    // event config
    DmaChnSetEventControl(uart_dma->uart_dma.dma_chn, DMA_EV_START_IRQ(_UART_FLAG_Table[(u8)uart_dma->uart_id].tx_irq));

    // source and destination setting
    DmaChnSetTxfer( uart_dma->uart_dma.dma_chn, 
                    uart_dma->tx_buffer, _UART_FLAG_Table[(u8)uart_dma->uart_id].tx_reg, 
                    0, 1, 1);


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

    // uart enable
    UARTEnable(uart_dma->uart_id, UART_ENABLE | UART_PERIPHERAL | UART_TX | UART_RX);

    // dma enable
    DmaEnable(TRUE);
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

    u16 idx = 0;
    
    // check cnt
    if ((cnt == 0) || (cnt > _UART_DMA_BUFFER_SIZE)) { return 0; }

    // disable channel [ this function has while loop !!! ]
    DmaChnDisable(uart_dma->uart_dma.dma_chn);
    
    // data copy.
    for (idx = 0; idx < cnt; idx++) uart_dma->tx_buffer[idx] = src_buf[idx];

    // Txd
    DmaChnSetTxfer( uart_dma->uart_dma.dma_chn, 
                    uart_dma->tx_buffer, _UART_FLAG_Table[(u8)uart_dma->uart_id].tx_reg, 
                    cnt, 1, 1);
    DmaChnEnable(uart_dma->uart_dma.dma_chn);
    
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

    // source done
    if (DmaChnGetEvFlags(uart_dma->uart_dma.dma_chn) & DMA_EV_SRC_FULL) {
        return TRUE;
    }
    else return FALSE;
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
}

//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
