
/*----------------------------------------------------------------------^^-
/ File name:  xu_lib_mx2xx.c
/ Author:     JiangJun
/ Data:       [2020-7-10]
/ Version:    v1.23
/-----------------------------------------------------------------------^^-
/ PIC32 usb driver layer
/ ---
/ v1.1 [2020-5-19]
/ 1. FIX: XUL_Init() remove TRNIE bit BUG
/ 2. add _xul_read()
/ ---
/ v1.2
/ 1. add _ISR code
/ 2. add STALL interrupt
/ ---
/ v1.21
/ 1. modify XUL_EPnResumeTx to XUL_EPnRstTx
/ ---
/ v1.22
/ 1. XUL_EPnInit() will clear BDT
/ 2. add XUL_EPnRxDATA() function
/ ---
/ v1.23
/ 1. some functions e0 % 2 to if (eo) eo = 1; else eo = 0;
/------------------------------------------------------------------------*/


#include "xu_lib_mx2xx.h"


//------------------------------------------------------------
//              MACRO
//------------------------------------------------------------


//------------------------------------------------------------
//              STATIC
//------------------------------------------------------------

// BDT Table
static volatile _EP_BDT_ENTRY _XUL_EpBDT[_XUL_MAX_EP_NUM] __attribute__ (( aligned (512) ));

// ISR
static fpXUL_Isr FpXUL_Isr;


/*----------------------------------------------------------------------
 *  XUL_Init
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_XUL_ResT XUL_Init(fpXUL_Isr fp_xuisr)
{

    u8 idx = 0;

    //------------------------------------------------------------
    //              Hw Reset
    //------------------------------------------------------------

    // USB modude Power ON, before this, you should enable PLL-48MHz
    PLIB_USB_Enable(_XUL_HW_ID);

    // Reset all interrupt enable
    PLIB_USB_InterruptDisable(_XUL_HW_ID, USB_INT_ALL);
    PLIB_USB_ErrorInterruptDisable(_XUL_HW_ID, USB_ERR_INT_ALL);

    // 4 FIFO STAT and isr status reset
    for (idx = 0; idx < 4; idx++) PLIB_USB_InterruptFlagClear(_XUL_HW_ID, USB_INT_ALL);

    // error isr status reset
    PLIB_USB_ErrorInterruptFlagClear(_XUL_HW_ID, USB_ERR_INT_ALL);        

    // Reset All EPn
    for (idx = 0; idx < _XUL_MAX_EP_NUM; idx++) PLIB_USB_EPnClear(_XUL_HW_ID, idx);    
    
    PLIB_USB_PingPongReset(_XUL_HW_ID);                 // Reset PING-PONG buffer
    PLIB_USB_DeviceAddressSet(_XUL_HW_ID, 0);           // Reset Device Addr    
    PLIB_USB_AutoSuspendDisable(_XUL_HW_ID);            // Reset UxCNFG1, default: 0x01    
    

    //------------------------------------------------------------
    //              Hw Init
    //------------------------------------------------------------    

    // Isr Mode
#if _XUL_MODE_INTERRUPT

    // Set Isr Enable
    // RESET/ TOKEN DONE/ IDLE for default
    PLIB_USB_InterruptEnable(   _XUL_HW_ID, 
                                USB_INT_DEVICE_RESET |              // RESET
                                USB_INT_TOKEN_DONE |                // TOKEN
                                USB_INT_IDLE_DETECT |               // IDLE
                                USB_INT_STALL);                     // STALL

    // Set Error Isr All Disable
    PLIB_USB_ErrorInterruptDisable(_XUL_HW_ID, USB_ERR_INT_ALL);

    // Set Isr Priority    
    INTSetVectorPriority(INT_USB_1_VECTOR, _XUL_INT_PRI);
    INTSetVectorSubPriority(INT_USB_1_VECTOR, _XUL_INT_SUB_PRI);    // sub prio
    
    // Enable Vector
    INTEnable(INT_USB, INT_ENABLED);

    // ISR Call-Back
    if (fp_xuisr) FpXUL_Isr = fp_xuisr;
#endif

    
    // Enable Packet Processing
    PLIB_USB_PacketTransferEnable(_XUL_HW_ID);          // [ !! IMPORMENT ]
    

    //------------------------------------------------------------
    //              BDT Init
    //------------------------------------------------------------
    
    // Clear All BDT entries
    for (idx = 0; idx < _XUL_MAX_EP_NUM; idx++)
    {
    
        _XUL_EpBDT[idx]._rx_bdt[0].dlValue = 0;         // even
        _XUL_EpBDT[idx]._rx_bdt[1].dlValue = 0;         // odd
        _XUL_EpBDT[idx]._tx_bdt[0].dlValue = 0;         // even
        _XUL_EpBDT[idx]._tx_bdt[1].dlValue = 0;         // odd
    }

    // Set BDT Address
    PLIB_USB_BDTBaseAddressSet(_XUL_HW_ID, (void *)((uint32_t)KVA_TO_PA(&_XUL_EpBDT)));

    // OK
    return _XUL_RES_OK;
}

/*----------------------------------------------------------------------
 *  XUL_Attach - this will enable USB module and pull-up D+ res
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void XUL_Attach(BOOL_t en)
{

    if (en) PLIB_USB_OperatingModeSelect(_XUL_HW_ID, USB_OPMODE_DEVICE);
    else PLIB_USB_OperatingModeSelect(_XUL_HW_ID, USB_OPMODE_NONE);
}

/*----------------------------------------------------------------------
 *  XUL_GetIsrStatus - this will clear register bit
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u8 XUL_GetIsrStatus(void)
{

    // read register
    u8 status = PLIB_USB_InterruptFlagRead(_XUL_HW_ID);
    PLIB_USB_InterruptFlagClear(_XUL_HW_ID, USB_INT_ALL); 
    return status;
}

/*----------------------------------------------------------------------
 *  XUL_PeekIsrStatus
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u8 XUL_PeekIsrStatus(void)
{

    // read register
    return PLIB_USB_InterruptFlagRead(_XUL_HW_ID);
}

/*----------------------------------------------------------------------
 *  XUL_ResetIsrStatus
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void XUL_ResetIsrStatus(u8 isr)
{

    // clear isr
    PLIB_USB_InterruptFlagClear(_XUL_HW_ID, isr);

    // clear mips isr
    INTClearFlag(INT_USB);
}

/*----------------------------------------------------------------------
 *  XUL_GetErrorStatus - this will clear register bit
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u8 XUL_GetErrorStatus(void)
{

    // read register
    u8 status = PLIB_USB_ErrorInterruptFlagRead(_XUL_HW_ID);
    PLIB_USB_ErrorInterruptFlagClear(_XUL_HW_ID, USB_ERR_INT_ALL);
    return status;
}

/*----------------------------------------------------------------------
 *  XUL_PeekErrorStatus
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u8 XUL_PeekErrorStatus(void)
{

    // read register
    return PLIB_USB_ErrorInterruptFlagRead(_XUL_HW_ID);;
}

/*----------------------------------------------------------------------
 *  XUL_ResetErrorStatus
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void XUL_ResetErrorStatus(u8 eisr)
{

    // clear error isr
    PLIB_USB_ErrorInterruptFlagClear(_XUL_HW_ID, eisr);

    // clear mips isr
    INTClearFlag(INT_USB);
}

/*----------------------------------------------------------------------
 *  XUL_EPnInit - ready to tx/rx transactions
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_XUL_ResT XUL_EPnInit(u8 epn, u8 type, u8 en_shk)
{

    u8 sepn = 0;
    
    /* check ep number */
    if (epn >= _XUL_MAX_EP_NUM) return _XUL_RES_EPN_ERROR;

    /* reset BDT */
    _XUL_EpBDT[epn]._rx_bdt[0].dlValue = 0;         /* even */
    _XUL_EpBDT[epn]._rx_bdt[1].dlValue = 0;         /* odd */
    _XUL_EpBDT[epn]._tx_bdt[0].dlValue = 0;         /* even */
    _XUL_EpBDT[epn]._tx_bdt[1].dlValue = 0;         /* odd */
        
    /* enable handshack */
    if (en_shk) sepn = type | 0x01;
    else sepn = type;    
    PLIB_USB_EPnSet(_XUL_HW_ID, epn, sepn);         /* set to the register */

    /* ok to set */
    return _XUL_RES_OK;     
}

/*----------------------------------------------------------------------
 *  XUL_EPnRstTx - clear the BDT with stalled-epn
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_XUL_ResT XUL_EPnRstTx(u8 epn)
{

    //------------------------------------------------------------
    //              Check EP number
    //------------------------------------------------------------
    
    if (epn >= _XUL_MAX_EP_NUM) return _XUL_RES_EPN_ERROR;    


    //------------------------------------------------------------
    //              Clear BDT
    //------------------------------------------------------------
    
    if (_XUL_EpBDT[epn]._tx_bdt[0].bufferStatus.stallEnable) {      // even
        _XUL_EpBDT[epn]._tx_bdt[0].dlValue = 0;                           
    }
    if (_XUL_EpBDT[epn]._tx_bdt[1].bufferStatus.stallEnable) {      // odd
        _XUL_EpBDT[epn]._tx_bdt[1].dlValue = 0;                             
    }


    //------------------------------------------------------------
    //              Clear EPSTALL
    //------------------------------------------------------------
    
    if (PLIB_USB_EPnIsStalled(_XUL_HW_ID, epn)) {           // EPSTALL = 0
        PLIB_USB_EPnStallClear(_XUL_HW_ID, epn);                
    }

    // ok to set
    return _XUL_RES_OK;
}

/*----------------------------------------------------------------------
 *  XUL_EPnReadyRx
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_XUL_ResT XUL_EPnReadyRx(XUL_RxReadyT *rx_rdy, u8 *svalue)
{

    u8 eo = 0, sepn = 0;
    u8 epn = rx_rdy->epn;
    
    
    //------------------------------------------------------------
    //              Check EP number
    //------------------------------------------------------------

    if (rx_rdy->even_odd) eo = 1; else eo = 0;
    if (epn >= _XUL_MAX_EP_NUM) return _XUL_RES_EPN_ERROR;


    //------------------------------------------------------------
    //              Cacl set value
    //------------------------------------------------------------
    
    if (rx_rdy->data01 == _XUL_DATA1) sepn |= 0x40;     /* set data0 or data1 */
    if (rx_rdy->en_dts) sepn |= 0x08;                   /* enable data0/1 sync */
    if (rx_rdy->en_stall) sepn |= 0x04;                 /* stall */
    if (rx_rdy->en_own) sepn |= 0x80;                   /* enable UOWN */    
    if (svalue) *svalue = sepn;                         /* re-back */
    if (rx_rdy->rsize > 1023) rx_rdy->rsize = 1023;     /* max receive size */


    //------------------------------------------------------------
    //              Set to BDT
    //------------------------------------------------------------
    
    _XUL_EpBDT[epn]._rx_bdt[eo].dlValue = 0;
    _XUL_EpBDT[epn]._rx_bdt[eo].byteCount = rx_rdy->rsize;
    _XUL_EpBDT[epn]._rx_bdt[eo].bufferAddress = (u32)KVA_TO_PA(rx_rdy->dst);
    _XUL_EpBDT[epn]._rx_bdt[eo].bufferStatus.sValue = sepn;


    //------------------------------------------------------------
    //              Packet Enable
    //------------------------------------------------------------
    
    PLIB_USB_PacketTransferEnable(_XUL_HW_ID);          /* [ !! IMPROMENT ] */     
    return _XUL_RES_OK; 
}

/*----------------------------------------------------------------------
 *  XUL_EPnReadyTx
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_XUL_ResT XUL_EPnReadyTx(XUL_TxReadyT *tx_rdy, u8 *svalue)
{

    u8 eo = 0, sepn = 0;
    u8 epn = tx_rdy->epn;
    

    //------------------------------------------------------------
    //              Check EP number
    //------------------------------------------------------------

    if (tx_rdy->even_odd) eo = 1; else eo = 0;
    if (epn >= _XUL_MAX_EP_NUM) return _XUL_RES_EPN_ERROR;


    //------------------------------------------------------------
    //              Cacl set value
    //------------------------------------------------------------
    
    if (tx_rdy->data01 == _XUL_DATA1) sepn |= 0x40;     /* set data0 or data1 */
    if (tx_rdy->en_dts) sepn |= 0x08;                   /* enable data0/1 sync */
    if (tx_rdy->en_stall) sepn |= 0x04;                 /* stall */
    if (tx_rdy->en_own) sepn |= 0x80;                   /* enable UOWN */    
    if (svalue) *svalue = sepn;                         /* re-back */
    if (tx_rdy->tsize > 1023) tx_rdy->tsize = 1023;     /* limit max tx size */


    //------------------------------------------------------------
    //              Set to BDT
    //------------------------------------------------------------
    
    _XUL_EpBDT[epn]._tx_bdt[eo].dlValue = 0;
    _XUL_EpBDT[epn]._tx_bdt[eo].byteCount = tx_rdy->tsize;
    _XUL_EpBDT[epn]._tx_bdt[eo].bufferAddress = (u32)KVA_TO_PA(tx_rdy->src);
    _XUL_EpBDT[epn]._tx_bdt[eo].bufferStatus.sValue = sepn;


    //------------------------------------------------------------
    //              Packet Enable
    //------------------------------------------------------------
    
    PLIB_USB_PacketTransferEnable(_XUL_HW_ID);          /* [ !! IMPROMENT ] */
    return _XUL_RES_OK;
}

/*----------------------------------------------------------------------
 *  XUL_EPnTxDATA0
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_XUL_ResT XUL_EPnTxDATA0(u8 epn, u8 eo, void *src, u16 size)
{

    u8 sepn = 0;

    /* check ep number */
    if (eo) eo = 1; else eo = 0;
    if (epn >= _XUL_MAX_EP_NUM) return _XUL_RES_EPN_ERROR;

    /* cacl set value */    
    sepn |= 0x80;                       /* enable UOWN */
    if (size > 1023) size = 1023;       /* limit max tx size */

    /* set to BDT */
    _XUL_EpBDT[epn]._tx_bdt[eo].dlValue = 0;
    _XUL_EpBDT[epn]._tx_bdt[eo].byteCount = size;
    _XUL_EpBDT[epn]._tx_bdt[eo].bufferAddress = (u32)KVA_TO_PA(src);
    _XUL_EpBDT[epn]._tx_bdt[eo].bufferStatus.sValue = sepn;

    /* packet enable */
    PLIB_USB_PacketTransferEnable(_XUL_HW_ID);      /* [ !! IMPROMENT ] */           

    /* ok to set */
    return _XUL_RES_OK;
}

/*----------------------------------------------------------------------
 *  XUL_EPnTxDATA1
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_XUL_ResT XUL_EPnTxDATA1(u8 epn, u8 eo, void *src, u16 size)
{

    u8 sepn = 0;

    /* check ep number */
    if (eo) eo = 1; else eo = 0;
    if (epn >= _XUL_MAX_EP_NUM) return _XUL_RES_EPN_ERROR;

    /* cacl set value */
    sepn |= 0x40;                       /* enable data1 */
    sepn |= 0x80;                       /* enable UOWN */
    if (size > 1023) size = 1023;       /* limit max tx size */

    /* set to BDT */
    _XUL_EpBDT[epn]._tx_bdt[eo].dlValue = 0;
    _XUL_EpBDT[epn]._tx_bdt[eo].byteCount = size;
    _XUL_EpBDT[epn]._tx_bdt[eo].bufferAddress = (u32)KVA_TO_PA(src);
    _XUL_EpBDT[epn]._tx_bdt[eo].bufferStatus.sValue = sepn;

    /* packet enable */
    PLIB_USB_PacketTransferEnable(_XUL_HW_ID);      /* [ !! IMPROMENT ] */           

    /* ok to set */
    return _XUL_RES_OK;
}

/*----------------------------------------------------------------------
 *  XUL_EPnRxDATA
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_XUL_ResT XUL_EPnRxDATA(u8 epn, u8 eo, void *dst, u16 size)
{

    u8 sepn = 0;

    /* check ep number */
    if (eo) eo = 1; else eo = 0;
    if (epn >= _XUL_MAX_EP_NUM) return _XUL_RES_EPN_ERROR;

    /* cacl set value */
    sepn |= 0x80;                       /* enable UOWN */
    if (size > 1023) size = 1023;       /* limit max tx size */

    /* set to BDT */
    _XUL_EpBDT[epn]._rx_bdt[eo].dlValue = 0;
    _XUL_EpBDT[epn]._rx_bdt[eo].byteCount = size;
    _XUL_EpBDT[epn]._rx_bdt[eo].bufferAddress = (u32)KVA_TO_PA(dst);
    _XUL_EpBDT[epn]._rx_bdt[eo].bufferStatus.sValue = sepn;

    /* packet enable */
    PLIB_USB_PacketTransferEnable(_XUL_HW_ID);      /* [ !! IMPROMENT ] */           

    /* ok to set */
    return _XUL_RES_OK;
}

/*----------------------------------------------------------------------
 *  XUL_EPnParseToken
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_XUL_ResT XUL_EPnParseToken(XUL_TxRxEndT *rx_tx)
{

    u8 sepn = 0; volatile BDT_ENTRY *prt_bdt;


    //------------------------------------------------------------
    //              STAT Get
    //------------------------------------------------------------

    /* check TRNIE bit */
    sepn = PLIB_USB_InterruptFlagRead(_XUL_HW_ID);
    if ((sepn & _XUL_INT_TOKEN_DONE) == 0) return _XUL_RES_TRNIE_NOTSET;
    
    /* read STAT */
    sepn = PLIB_USB_LastTransactionGet(_XUL_HW_ID);
    rx_tx->epn = sepn >> 4;
    if (rx_tx->epn >= _XUL_MAX_EP_NUM) return _XUL_RES_EPN_ERROR;   /* epn read */
    if (sepn & 0x04) rx_tx->even_odd = _XUL_ODD_PKT;    /* odd */
    else rx_tx->even_odd = _XUL_EVEN_PKT;       /* even */
    rx_tx->vstat = sepn;    /* re-back value */                    


    //------------------------------------------------------------
    //              BDT Get
    //------------------------------------------------------------

    /* pause bdt point */
    if (sepn & 0x08) prt_bdt = &_XUL_EpBDT[rx_tx->epn]._tx_bdt[rx_tx->even_odd];    /* tx */
    else prt_bdt = &_XUL_EpBDT[rx_tx->epn]._rx_bdt[rx_tx->even_odd];    /* rx */               
    
    /* pause last token */
    rx_tx->data01 = prt_bdt->bufferStatus.dataToggle % 2;       /* data0 or data1 */
    rx_tx->token = prt_bdt->bufferStatus.packetID;              /* token */
    rx_tx->vbdt = (u8)(prt_bdt->bufferStatus.sValue & 0xFF);    /* BDT status value */
    rx_tx->rtsize = prt_bdt->byteCount;                         /* the size of tx/rx */    
    rx_tx->rtphy = (void *)prt_bdt->bufferAddress;              /* phy address of the src/ dst */
    
    /* ok */
    return _XUL_RES_OK;
}

/*----------------------------------------------------------------------
 *  _xul_read - read bytes from phy address
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void _xul_read(void *phy, u16 psize, void *dst, u16 dsize)
{

    u16 idx = 0, bdst = 0;
    u8 *vir = (u8 *)PA_TO_KVA1((u32)phy);
    u8 *pdst = (u8 *)dst;    
    if (dsize < psize) bdst = dsize;    /* max dst */
    else bdst = psize;
    for (idx = 0; idx < bdst; idx++) *pdst++ = *vir++;
}

/*----------------------------------------------------------------------
 *  XUL_SetAddress
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void XUL_SetAddress(u8 addr)
{

    /* device address */
    PLIB_USB_DeviceAddressSet(_XUL_HW_ID, addr);
}

/*----------------------------------------------------------------------
 *  __ISR XUL_Handler
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void __ISR(_USB_1_VECTOR, ipl7AUTO) XUL_Handler(void)
{

    if (FpXUL_Isr) (*FpXUL_Isr)();
}


//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
