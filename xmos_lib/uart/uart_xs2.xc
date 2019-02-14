
/*----------------------------------------------------------------------^^-
/ File name:  uart_xs2.xc
/ Author:     JiangJun
/ Data:       2019/1/31
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ XS2 UART Driver
/------------------------------------------------------------------------*/


#include "uart_xs2.h"


/*----------------------------------------------------------------------
 *  _parity_8
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u8 _parity_8(UartParityT party, u8 dat, u8 data_bit)
{
    return 0;
}

#ifndef __C_PLUS_PLUS_REF__ /* c++ reference */
/*----------------------------------------------------------------------
 *  UartXs2Init
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
BOOL UartXs2Init(UartLinkT *uart)
{
    timer tmr; u32 tmr_val = 0;

    /* init txd idle and wait for 2-stop bit interval */
    uart->txd <: 1;             
    tmr :> tmr_val; tmr_val += ((_XCORE_REF_CLOCK_RATE / uart->baud_rate) * 2);
    tmr when timerafter(tmr_val) :> void;
    return TRUE;
}

/*----------------------------------------------------------------------
 *  UartXs2TxByte
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void UartXs2TxByte(UartLinkT *uart, u8 src)
{

    timer tmr; 
    u32 tmr_val = 0, br_ticks = 0; u8 bidx = 0;    

    /* obtains the baudrate ticks */
    br_ticks = _XCORE_REF_CLOCK_RATE / uart->baud_rate;

    /* start bit and byte */
    tmr :> tmr_val;                                             /* read timer and data */ 
    uart->txd <: 0;                                             /* start bit */        
    for (bidx = 0; bidx < uart->data_bit; bidx++) {             /* b2b txd */
        tmr_val += br_ticks;
        tmr when timerafter(tmr_val) :> void;                   /* bit interval */
        uart->txd <: src; src >>= 1;                            /* bit txd */                                       
    }

    /* stop bits */
    tmr_val += br_ticks;
    tmr when timerafter(tmr_val) :> void;                       /* bit interval */
    uart->txd <: 1;                                             /* stop bits */        
    tmr_val += (br_ticks << (u8)uart->stop_bit);        
    tmr when timerafter(tmr_val) :> void;                       /* bit interval */     
}

/*----------------------------------------------------------------------
 *  UartXs2Tx
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u16 UartXs2Tx(UartLinkT *uart, u8 *src, u16 cnt)
{

    u16 idx = 0; timer tmr; 
    u32 tmr_val = 0, br_ticks = 0; u8 bidx = 0, bdat = 0;    

    /* obtains the baudrate ticks */
    br_ticks = _XCORE_REF_CLOCK_RATE / uart->baud_rate;

    /* tx to port */
    for (idx = 0; idx < cnt; idx++)
    {   
        /* start bit and byte */
        bdat = src[idx]; tmr :> tmr_val;                            /* read timer and data */ 
        uart->txd <: 0;                                             /* start bit */        
        for (bidx = 0; bidx < uart->data_bit; bidx++) {             /* b2b txd */
            tmr_val += br_ticks;
            tmr when timerafter(tmr_val) :> void;                   /* bit interval */
            uart->txd <: bdat; bdat >>= 1;                          /* bit txd */                                       
        }

        /* stop bits */
        tmr_val += br_ticks;
        tmr when timerafter(tmr_val) :> void;                       /* bit interval */
        uart->txd <: 1;                                             /* stop bits */        
        tmr_val += (br_ticks << (u8)uart->stop_bit);        
        tmr when timerafter(tmr_val) :> void;                       /* bit interval */        
    }
    
    /* return the tx count with the src */
    return cnt;
}

/*----------------------------------------------------------------------
 *  UartXs2Print
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u16 UartXs2Print(UartLinkT *uart, u8 *src)
{

    u16 idx = 0;
    /* no limit */
    for (idx = 0; src[idx] != '\0'; idx++); 
    /* tx to hw */
    UartXs2Tx(uart, src, idx);              
    return idx;
}

#else
/*----------------------------------------------------------------------
 *  UartXs2Init
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
BOOL UartXs2Init(UartLinkT &uart)
{
    timer tmr; u32 tmr_val = 0;

    /* init txd idle and wait for 2-stop bit interval */
    uart.txd <: 1;             
    tmr :> tmr_val; tmr_val += ((_XCORE_REF_CLOCK_RATE / uart.baud_rate) * 2);
    tmr when timerafter(tmr_val) :> void;
    return TRUE;
}

/*----------------------------------------------------------------------
 *  UartXs2TxByte
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void UartXs2TxByte(UartLinkT &uart, u8 src)
{

    timer tmr; 
    u32 tmr_val = 0, br_ticks = 0; u8 bidx = 0;    

    /* obtains the baudrate ticks */
    br_ticks = _XCORE_REF_CLOCK_RATE / uart.baud_rate;

    /* start bit and byte */
    tmr :> tmr_val;                                             /* read timer and data */ 
    uart.txd <: 0;                                              /* start bit */        
    for (bidx = 0; bidx < uart.data_bit; bidx++) {              /* b2b txd */
        tmr_val += br_ticks;
        tmr when timerafter(tmr_val) :> void;                   /* bit interval */
        uart.txd <: src; src >>= 1;                             /* bit txd */                                       
    }

    /* stop bits */
    tmr_val += br_ticks;
    tmr when timerafter(tmr_val) :> void;                       /* bit interval */
    uart.txd <: 1;                                              /* stop bits */        
    tmr_val += (br_ticks << (u8)uart.stop_bit);        
    tmr when timerafter(tmr_val) :> void;                       /* bit interval */     
}

/*----------------------------------------------------------------------
 *  UartXs2Tx
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u16 UartXs2Tx(UartLinkT &uart, u8 *src, u16 cnt)
{

    u16 idx = 0; timer tmr; 
    u32 tmr_val = 0, br_ticks = 0; u8 bidx = 0, bdat = 0;    

    /* obtains the baudrate ticks */
    br_ticks = _XCORE_REF_CLOCK_RATE / uart.baud_rate;

    /* tx to port */
    for (idx = 0; idx < cnt; idx++)
    {   
        /* start bit and byte */
        bdat = src[idx]; tmr :> tmr_val;                            /* read timer and data */ 
        uart.txd <: 0;                                             /* start bit */        
        for (bidx = 0; bidx < uart.data_bit; bidx++) {             /* b2b txd */
            tmr_val += br_ticks;
            tmr when timerafter(tmr_val) :> void;                   /* bit interval */
            uart.txd <: bdat; bdat >>= 1;                          /* bit txd */                                       
        }

        /* stop bits */
        tmr_val += br_ticks;
        tmr when timerafter(tmr_val) :> void;                       /* bit interval */
        uart.txd <: 1;                                             /* stop bits */        
        tmr_val += (br_ticks << (u8)uart.stop_bit);        
        tmr when timerafter(tmr_val) :> void;                       /* bit interval */        
    }
    
    /* return the tx count with the src */
    return cnt;
}

/*----------------------------------------------------------------------
 *  UartXs2Print
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u16 UartXs2Print(UartLinkT &uart, u8 *src)
{

    u16 idx = 0;
    /* no limit */
    for (idx = 0; src[idx] != '\0'; idx++); 
    /* tx to hw */
    UartXs2Tx(uart, src, idx);              
    return idx;
}

#endif /* __C_PLUS_PLUS_REF__ */

//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
