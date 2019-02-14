
/*----------------------------------------------------------------------^^-
/ File name:  uart_xs2.h
/ Author:     JiangJun
/ Data:       2019/1/31
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/
/------------------------------------------------------------------------*/

#ifndef _UART_XS2_H
#define _UART_XS2_H


#include "main.h"
//------------------------------------------------------------
//              UART STRUCT
//------------------------------------------------------------

/* parity bit enum */
typedef enum {

    _UART_PARITY_NONE = 0,
    _UART_PARITY_ODD,
    _UART_PARITY_EVEN,
    _UART_PARITY_MARK,
    _UART_PARITY_SPACE,
    
} UartParityT;

/* stop bit enum */
typedef enum {

    _UART_STOP_BIT_1 = 0,
    _UART_STOP_BIT_2 = 1,
    
} UartStopBitT;

/* uart hardware definations */
typedef struct {

    /* hardware port */
    out port txd;
    
    /* data format */
    u32 baud_rate;
    u8 data_bit;
    UartParityT parity_bit;     /* reverse */
    UartStopBitT stop_bit;
    
} UartLinkT;


//------------------------------------------------------------
//              XC Revsion
//------------------------------------------------------------
#ifndef __C_PLUS_PLUS_REF__

  extern BOOL UartXs2Init(UartLinkT *uart);
  extern void UartXs2TxByte(UartLinkT *uart, u8 src);
  extern u16 UartXs2Tx(UartLinkT *uart, u8 *src, u16 cnt);
  extern u16 UartXs2Print(UartLinkT *uart, u8 *src);

#else  

  extern BOOL UartXs2Init(UartLinkT &uart);
  extern void UartXs2TxByte(UartLinkT &uart, u8 src);
  extern u16 UartXs2Tx(UartLinkT &uart, u8 *src, u16 cnt);
  extern u16 UartXs2Print(UartLinkT &uart, u8 *src);
  
#endif /* __XC__ */
#endif /* _UART_XS2_H */

//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
