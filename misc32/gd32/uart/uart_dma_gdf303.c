
/*----------------------------------------------------------------------^^-
/ File name:  uart_dma_gdf303.c
/ Author:     JiangJun
/ Data:       2022/4/11
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ GD32F303 UART driver
/------------------------------------------------------------------------*/


#include "uart_dma_gdf303.h"
#include "string.h"


//------------------------------------------------------------
//                      DMA CONST
//------------------------------------------------------------

// DMA Channel
typedef struct {

    // DMA
    uint32_t dma_periph;                    // dma
    
    // TXD
    dma_channel_enum tx_channel;            // channel

    // RXD
    dma_channel_enum rx_channel;            // channel                   
    
} UartDmaT;

// Table
const UartDmaT UartDmaConst[4] = 
{

    // UART0    
    {
        DMA0,
        DMA_CH3,                // TXD
        DMA_CH4,                // RXD
    },

    // UART1
    {
        DMA0,
        DMA_CH6,                // TXD
        DMA_CH5,                // RXD
    },

    // UART2
    {
        DMA0,
        DMA_CH1,                // TXD
        DMA_CH2,                // RXD
    },

    // UART3
    {
        DMA1,
        DMA_CH4,                // TXD
        DMA_CH2,                // RXD
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
s8 _uart_get_const_idx(u32 usart_periph)
{

    if (usart_periph == USART0) {

        return 0;
    } 
    else if (usart_periph == USART1) {

        return 1;
    }
    else if (usart_periph == USART2) {

        return 2;
    }
    else if (usart_periph == UART3) {

        return 3;
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

    dma_parameter_struct dma_init_struct;
    s8 idx = _uart_get_const_idx(uart_dma->usart_periph);


    // Index check
    if (idx < 0) { return; }


    //------------------------------------------------------------
    //              DeInit DMA/UART
    //------------------------------------------------------------
    
    dma_deinit(UartDmaConst[idx].dma_periph, UartDmaConst[idx].tx_channel);
    dma_deinit(UartDmaConst[idx].dma_periph, UartDmaConst[idx].rx_channel);
    usart_deinit(uart_dma->usart_periph);


    //------------------------------------------------------------
    //              Init DMA/UART
    //------------------------------------------------------------

    /* DMAyChanlex configured as follows: 
        - Normal mode
        - Memory Data Size: Byte
        - Priority: VeryHigh
    */
    
    dma_struct_para_init(&dma_init_struct);
        
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;    
    dma_init_struct.periph_addr = ((uint32_t)&USART_DATA(uart_dma->usart_periph));
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;

    // Tx
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (u32)uart_dma->tx_buffer;
    dma_init_struct.number = 0;    
    dma_init(UartDmaConst[idx].dma_periph, UartDmaConst[idx].tx_channel, &dma_init_struct);
    dma_circulation_disable(UartDmaConst[idx].dma_periph, UartDmaConst[idx].tx_channel);             /* configure DMA mode */

    // Rx
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (u32)uart_dma->rx_buffer;
    dma_init_struct.number = _UART_DMA_BUFFER_SIZE;    
    dma_init(UartDmaConst[idx].dma_periph, UartDmaConst[idx].rx_channel, &dma_init_struct);
    dma_circulation_disable(UartDmaConst[idx].dma_periph, UartDmaConst[idx].rx_channel);             /* configure DMA mode */


    /* USARTx configured as follows:
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
    */
    
    usart_baudrate_set(uart_dma->usart_periph, uart_dma->baud_rate);
    usart_receive_config(uart_dma->usart_periph, USART_RECEIVE_ENABLE);
    usart_transmit_config(uart_dma->usart_periph, USART_TRANSMIT_ENABLE);

    /* USART DMA enable for transmission and reception */
    usart_dma_transmit_config(uart_dma->usart_periph, USART_DENT_ENABLE);
    usart_dma_receive_config(uart_dma->usart_periph, USART_DENR_ENABLE);
    
    
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

    s8 idx = _uart_get_const_idx(uart_dma->usart_periph);

    // Index check
    if (idx < 0) { return; }
    
    // Enable Rx Channel
    dma_channel_enable(UartDmaConst[idx].dma_periph, UartDmaConst[idx].rx_channel);

    // Enable UART
    usart_enable(uart_dma->usart_periph);
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

    s8 idx = _uart_get_const_idx(uart_dma->usart_periph);

    // Index check
    if (idx < 0) { return 0; }
    
    // check cnt
    if (cnt == 0) { return 0; }

    // if last packet not send-over, loss this packet
    if (dma_transfer_number_get(UartDmaConst[idx].dma_periph, UartDmaConst[idx].tx_channel) != 0) {

        return 0;
    }

    // data copy.
    memcpy(uart_dma->tx_buffer, src_buf, cnt);

    // Txd
    dma_flag_clear(UartDmaConst[idx].dma_periph, UartDmaConst[idx].tx_channel, DMA_INTF_FTFIF);
    dma_channel_disable(UartDmaConst[idx].dma_periph, UartDmaConst[idx].tx_channel);
    dma_transfer_number_config(UartDmaConst[idx].dma_periph, UartDmaConst[idx].tx_channel, cnt);
    dma_channel_enable(UartDmaConst[idx].dma_periph, UartDmaConst[idx].tx_channel);

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

    s8 idx = _uart_get_const_idx(uart_dma->usart_periph);

    // Index check
    if (idx < 0) { return _false; }

    // DMA BuF check
    if (dma_transfer_number_get(UartDmaConst[idx].dma_periph, UartDmaConst[idx].tx_channel) == 0) {

        return _true;
    }

    return _false;
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

    u16 cnt = 0; s8 idx = _uart_get_const_idx(uart_dma->usart_periph);

    // Index check
    if (idx < 0) { return; }


    //------------------------------------------------------------
    //              FLAG
    //------------------------------------------------------------

    if (dma_flag_get(UartDmaConst[idx].dma_periph, UartDmaConst[idx].rx_channel, DMA_INTF_FTFIF) == SET)
    {
    
        dma_flag_clear(UartDmaConst[idx].dma_periph, UartDmaConst[idx].rx_channel, DMA_INTF_FTFIF);        
        dma_channel_disable(UartDmaConst[idx].dma_periph, UartDmaConst[idx].rx_channel);
        dma_transfer_number_config(UartDmaConst[idx].dma_periph, UartDmaConst[idx].rx_channel, _UART_DMA_BUFFER_SIZE);
        dma_channel_enable(UartDmaConst[idx].dma_periph, UartDmaConst[idx].rx_channel);
    }

    // UART overrun
    if (usart_flag_get(uart_dma->usart_periph, USART_FLAG_ORERR) == SET)
    {
    
        usart_flag_clear(uart_dma->usart_periph, USART_FLAG_ORERR);
        usart_data_receive(uart_dma->usart_periph);
        dma_channel_disable(UartDmaConst[idx].dma_periph, UartDmaConst[idx].rx_channel);
        dma_transfer_number_config(UartDmaConst[idx].dma_periph, UartDmaConst[idx].rx_channel, _UART_DMA_BUFFER_SIZE);
        dma_channel_enable(UartDmaConst[idx].dma_periph, UartDmaConst[idx].rx_channel);
    }


    //------------------------------------------------------------
    //              OVERTIME
    //------------------------------------------------------------

    // Get the DMA count
    cnt = dma_transfer_number_get(UartDmaConst[idx].dma_periph, UartDmaConst[idx].rx_channel); 
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
        dma_channel_disable(UartDmaConst[idx].dma_periph, UartDmaConst[idx].rx_channel);

        // write Rx
        if (uart_dma->fp_rx_output != NULL) { (*(uart_dma->fp_rx_output))(uart_dma->rx_buffer, uart_dma->overtime.last_cnt); }
        
        // reset        
        uart_dma->overtime.last_cnt = 0; uart_dma->overtime.pass_time = 0;
        dma_transfer_number_config(UartDmaConst[idx].dma_periph, UartDmaConst[idx].rx_channel, _UART_DMA_BUFFER_SIZE);
        dma_channel_enable(UartDmaConst[idx].dma_periph, UartDmaConst[idx].rx_channel);

        // [LED...]
        if (uart_dma->fp_rx_trigger != NULL) { (*(uart_dma->fp_rx_trigger))(); }
    }
}

//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
