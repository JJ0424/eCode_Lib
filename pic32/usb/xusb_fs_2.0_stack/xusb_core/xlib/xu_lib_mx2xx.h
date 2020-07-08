
/*----------------------------------------------------------------------^^-
/ File name:  xu_lib_mx2xx.h
/ Author:     JiangJun
/ Data:       [2020-7-6]
/ Version:    v1.22
/-----------------------------------------------------------------------^^-
/  PIC32 usb driver layer
/------------------------------------------------------------------------*/

#ifndef _XU_LIB_MX2XX_H
#define _XU_LIB_MX2XX_H


#include "main.h"
#include "pic32_usb.h"


//------------------------------------------------------------
//              CONFIG [ default ]
//------------------------------------------------------------

// [ default: USB_ID_1 ]
#define _XUL_HW_ID                          USB_ID_1

// Max Endpoint Size [ >= 2 ]                     
#define _XUL_MAX_EP_NUM                     3

// Isr Mode
#define _XUL_MODE_INTERRUPT                 1 

// [ 1-7, 7 is the hightest ]
#define _XUL_INT_PRI                        INT_PRIORITY_LEVEL_7

// [ 0-3, 3 is the hightest ]
#define _XUL_INT_SUB_PRI                    INT_SUB_PRIORITY_LEVEL_0



//------------------------------------------------------------
//              MACROS
//------------------------------------------------------------

// isr status
#define _XUL_INT_DEVICE_RESET               0x01        // vaild reset condition
#define _XUL_INT_ERROR                      0x02        // Only valid when U1EIE set bits
#define _XUL_INT_SOF                        0x04        // 1ms SOF packet (FS Mode)
#define _XUL_INT_TOKEN_DONE                 0x08        // a transaction has been done
#define _XUL_INT_IDLE_DETECT                0x10        // idle state for 3ms or more
#define _XUL_INT_RESUME                     0x20        // Only vaild in Hardware Suspend Mode
#define _XUL_INT_ATTACH                     0x40        // Host mode only
#define _XUL_INT_STALL                      0x80        // device issue a 'STALL' handshake
#define _XUL_INT_ALL                        0xFF        // ALL WITH THE STATUS

// error status
#define _XUL_ERR_INT_PID_ERROR              0x01
#define _XUL_ERR_INT_BAD_CRC5               0x02
#define _XUL_ERR_INT_BAD_CRC16              0x04
#define _XUL_ERR_INT_BAD_DATA_SIZE          0x08
#define _XUL_ERR_INT_BUS_TIMEOUT            0x10
#define _XUL_ERR_INT_DMA_ERROR              0x20
#define _XUL_ERR_INT_BUS_MATRIX_ERROR       0x40        // error BDT/ buffer address
#define _XUL_ERR_INT_BIT_STUFF_ERROR        0x80
#define _XUL_ERR_INT_ALL                    0XFF        // ALL WITH THE ERROR STATUS

// transactions token
#define _XUL_TOKEN_SETUP                    0x0D        // stardand prototal
#define _XUL_TOKEN_IN                       0x09
#define _XUL_TOKEN_OUT                      0x01

// data0/1
#define _XUL_DATA0                          0x00        // data packet
#define _XUL_DATA1                          0x01

// even or odd
#define _XUL_EVEN_PKT                       0x00        // even or odd BDT buffer
#define _XUL_ODD_PKT                        0x01

// ep config
#define _XUL_EP_TYPE_OFF                    0x00        // OFF this epn
#define _XUL_EP_TYPE_CTRL                   0x0D        // SETUP/ IN/ OUT/ [ HSHK ]
#define _XUL_EP_TYPE_OUT                    0x18        // OUT, epn rx data
#define _XUL_EP_TYPE_IN                     0x14        // IN, epn tx data
#define _XUL_EP_TYPE_OUT_IN                 0x1C        // OUT/ IN, epn rx/tx data



//------------------------------------------------------------
//              RESULT
//------------------------------------------------------------

typedef enum {

    _XUL_RES_OK = 1,                                    // ok 
    _XUL_RES_EPN_ERROR,                                 // error with end-point number
    _XUL_RES_TRNIE_NOTSET,                              // pause token error
        
} _XUL_ResT;



//------------------------------------------------------------
//              STRUCT
//------------------------------------------------------------

// set epn enter rx state
typedef struct {

    u8 epn;             // end-point number    
    u8 even_odd;        // 0 - even, other - odd
    u8 data01;          // data0 or data1 rx
    u8 en_dts;          // data0/1 sync enable
    u8 en_stall;        // any host to this end-point will ack with 'stall'
    u8 en_own;          // enable usb to own this BDT
    u16 rsize;          // max rx size
    void *dst;          // buffer address of 'dst' rx
    
} XUL_RxReadyT;

// set epn enter tx state
typedef struct {

    u8 epn;             // end-point number    
    u8 even_odd;        // 0 - even, other - odd
    u8 data01;          // data0 or data1 tx
    u8 en_dts;          // data0/1 sync enable
    u8 en_stall;        // any host to this end-point will ack with 'stall'
    u8 en_own;          // enable usb to own this BDT
    u16 tsize;          // the data size of tx
    void *src;          // buffer address of 'src' tx
    
} XUL_TxReadyT;

// the end of tx/rx
typedef struct {

    u8 epn;             // end-point number
    u8 even_odd;        // 0 - even, other - odd
    u8 vstat;           // value of U1STAT
    u8 data01;          // data0 or data1, [ when tx, not change ]
    u8 token;           // this transaction token
    u8 vbdt;            // value of bdt status
    u16 rtsize;         // the size of rx/tx complete
    void *rtphy;        // the buffer address of rx/tx * PHY address
    
} XUL_TxRxEndT;

// isr call-back
typedef void (*fpXUL_Isr)(void);


//------------------------------------------------------------
//              FUNCTIONS
//------------------------------------------------------------

extern _XUL_ResT XUL_Init(fpXUL_Isr fp_xuisr);
extern void XUL_Attach(BOOL_t en);
extern u8 XUL_GetIsrStatus(void);
extern u8 XUL_PeekIsrStatus(void);
extern void XUL_ResetIsrStatus(u8 isr);
extern u8 XUL_GetErrorStatus(void);
extern u8 XUL_PeekErrorStatus(void);
extern void XUL_ResetErrorStatus(u8 eisr);
extern _XUL_ResT XUL_EPnInit(u8 epn, u8 type, u8 en_shk);
extern _XUL_ResT XUL_EPnRstTx(u8 epn);
extern _XUL_ResT XUL_EPnReadyRx(XUL_RxReadyT *rx_rdy, u8 *svalue);
extern _XUL_ResT XUL_EPnReadyTx(XUL_TxReadyT *tx_rdy, u8 *svalue);
extern _XUL_ResT XUL_EPnTxDATA0(u8 epn, u8 eo, void *src, u16 size);
extern _XUL_ResT XUL_EPnTxDATA1(u8 epn, u8 eo, void *src, u16 size);
extern _XUL_ResT XUL_EPnRxDATA(u8 epn, u8 eo, void *dst, u16 size);
extern _XUL_ResT XUL_EPnParseToken(XUL_TxRxEndT *rx_tx);
extern void XUL_SetAddress(u8 addr);
extern void _xul_read(void *phy, u16 psize, void *dst, u16 dsize);

#endif

//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
