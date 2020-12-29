
/*----------------------------------------------------------------------^^-
/ File name:  xu_device.c
/ Author:     JiangJun
/ Data:       [2020-12-14]
/ Version:    v1.33
/-----------------------------------------------------------------------^^-
/ usb2.0 middle layer
/ ---
/ v1.01
/ 1. add interrupt mode
/ 2. add STALL processing
/ ---
/ v1.02
/ 1. XUL_EPnResumeTx to XUL_EPnRstTx
/ 2. HID desc type to xu_class.h
/ ---
/ v1.03
/ 1. XUD_SyncFrame not support for all device
/ ---
/ v1.10
/ 1. FIX: When host need 192bytes, no ZLP packet(64bytes EP0)
/ 2. 8-byte EP0 USB CV3 Tets Passed
/ ---
/ v1.11
/ 1. modify code edit-format
/ ---
/ v1.12
/ 1. add other-EP processing
/ 2. EP0 request point limiter
/ 3. EP0 enter SETUP(0) when return NONE
/ ---
/ v1.20
/ 1. FIX: DispatchTasks save only save new_tx/rx of EP0
/ ---
/ v1.21
/ 1. _xud_memset to public
/ 2. add _XUD_FRES_ER_DATA_NOT_READY_E11 macro code
/ ---
/ v1.22
/ 1. add sof number support
/ ---
/ v1.30
/ 1. modify function name
/ 2. FIX: XUD_DispatchTasks not modify BDT when return _XUD_FRES_NONE
/ 3. FIX: XUD_SetDescriptor should return NS_RX
/ ---
/ v1.31
/ 1. XUD_Init() attach to the end
/ ---
/ v1.32
/ 1. MOD: some comment
/ ---
/ v1.33
/ 1. ADD: XUD_GetDeviceStatus()
/------------------------------------------------------------------------*/



#include "xu_device.h"
#include "xu_class.h"



//------------------------------------------------------------
//              TYPEDEF
//------------------------------------------------------------

// Status Entrys
// Tx/ Rx only happpen once
typedef XUD_FResT (*fpXUD_StateEntry)(  XUD_RunStatusT *,   // the run-status of usb device
                                        XUL_TxRxEndT *,     // the token-packet complete, NLLL means no token
                                        XUL_TxReadyT *,     // the packet need to tx to host, NULL means no tx
                                        XUL_RxReadyT *      // tht packet need to rx from host, NULL means no rx
                                     );

// Standard Request Entrys
typedef XUD_FResT (*fpXUD_StdReqEntry)( XUD_RunStatusT *,   // the run-status of usb device
                                        XUD_DevRequestT*,   // standard request, NULL means no this packet
                                        u8 *,               // DATA0/1
                                        u8 *                // tx/ rx size of the next packet
                                      );


//------------------------------------------------------------
//              STATIC VAR
//------------------------------------------------------------

// The Entrys of status
static const fpXUD_StateEntry FpXUD_StateEntrys[6] = 
{

    NULL,                                       // attached state
    XUD_PoweredStatus,                          // powered status
    XUD_DefaultStatus,                          // wait for addredded
    XUD_AddressStatus,                          // wait for configuard
    XUD_ConfuredStatus,                         // can be work normally
    XUD_SuspendedStatus,                        // Low-Power mode    
};

// The Entrys of Std-Requests
static const fpXUD_StdReqEntry FpXUD_StdReqEntry[13] = 
{

    XUD_GetStatus,
    XUD_ClearFeature,
    NULL,
    XUD_SetFeature,
    NULL,
    XUD_SetAddress,                             // set usb device address
    XUD_GetDescriptor,                          // return descriptor
    XUD_SetDescriptor,
    XUD_GetConfiguration,
    XUD_SetConfiguration,
    XUD_GetInterface,
    XUD_SetInterface,
    XUD_SyncFrame
};

static XUD_RunStatusT XUD_RunStatus;            // Device Run-status
static fpXUD_TaskReport FpXUD_TaskReport;       // Device Task Report (normal UART)


//-------------------------------------------------------
//              Driver Packet
//-------------------------------------------------------

static XUL_TxRxEndT XUL_TxRxEnd;
static XUL_TxReadyT XUL_TxReady; static XUL_RxReadyT XUL_RxReady;



/*----------------------------------------------------------------------
 *  _xud_memset
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void _xud_memset(void *pxud, u8 cnt, u8 val)
{

    u8 idx = 0; u8 *psrc = (u8 *)pxud;
    for (idx = 0; idx < cnt; idx++) *psrc++ = val;
}

/*----------------------------------------------------------------------
 *  XUD_Init
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void XUD_Init(fpXUD_TaskReport fp_tsk_rpt)
{
    

    //------------------------------------------------------------
    //              Init USB module
    //------------------------------------------------------------

    XUL_Init(XUD_DispatchTasks);
    

    //------------------------------------------------------------
    //              Init Var
    //------------------------------------------------------------
    
    XUD_RunStatus._usb_addr = 0;
    XUD_RunStatus._sof_frame = 0;
    XUD_RunStatus._ep0_sreq_now = _XUD_STDR_NONE;
    XUD_RunStatus._ep0_host_need = 0;
    XUD_RunStatus._ep0_total_cnt = 0;
    XUD_RunStatus._ep0_rdy_now_cnt = 0;
    XUD_RunStatus._ep0_dpassed_cnt = 0;
    XUD_RunStatus._p_ep0_desc = 0;
    XUD_RunStatus._dstatus = _XUD_STA_POWERED;

    // struct
    _xud_memset(&XUD_RunStatus._ep0_new_tx, sizeof(XUL_TxRxEndT), 0);
    _xud_memset(&XUD_RunStatus._ep0_new_rx, sizeof(XUL_TxRxEndT), 0);
    _xud_memset(&XUD_RunStatus._epn_new_tk, sizeof(XUL_TxRxEndT), 0);
    _xud_memset(&XUD_RunStatus._ep0_dreq_now, sizeof(XUD_DevRequestT), 0);


    //------------------------------------------------------------
    //              Init Const
    //------------------------------------------------------------

    FpXUD_TaskReport = fp_tsk_rpt;              // xprintf   
    XUD_RunStatus._p_desc_set = XUC_Init();


    //------------------------------------------------------------
    //              PULL-UP Enable
    //------------------------------------------------------------
    
    XUL_Attach(TRUE);                           // device mode
}

/*----------------------------------------------------------------------
 *  XUD_DispatchTasks
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    period mode:
 *              1. 1ms call this function
 *          
 *           isr mode:
*               2. re-call this funcion in isr-sevice
 *---------------------------------------------------------------------*/
void XUD_DispatchTasks(void)
{


    u8 isrsta = 0, isrclr = 0;
    XUD_FResT fres = _XUD_FRES_NONE;
    


    //------------------------------------------------------------
    //              Peek ISR Status
    //------------------------------------------------------------
    
    isrsta = XUL_PeekIsrStatus();


    //------------------------------------------------------------
    //              PRIORITY 1: STALL
    //------------------------------------------------------------

    if (isrsta & _XUL_INT_STALL)
    {

        // mark
        isrclr |= _XUL_INT_STALL; XUL_EPnTxResume(0);        
    }
    

    //------------------------------------------------------------
    //              PRIORITY 2: TOKEN DONE
    //------------------------------------------------------------
    
    if (isrsta & _XUL_INT_TOKEN_DONE)        
    {     
    

        // mark
        isrclr |= _XUL_INT_TOKEN_DONE;
        

        //-------------------------------------------------------
        //              STAT FIFO
        //-------------------------------------------------------
        
        if (XUL_EPnParseToken(&XUL_TxRxEnd) == _XUL_RES_OK)
        {                       
            
            // New Token
            XUD_RunStatus._epn_new_tk = XUL_TxRxEnd;
            

            //-------------------------------------------------------
            //              EP Transaction
            //-------------------------------------------------------
            
            if (XUL_TxRxEnd.epn == 0)
            {

                //-------------------------------------------------------
                //              New Tx/Rx Update
                //-------------------------------------------------------
                
                if (XUL_TxRxEnd.token == _XUL_TOKEN_IN) XUD_RunStatus._ep0_new_tx = XUL_TxRxEnd;
                else XUD_RunStatus._ep0_new_rx = XUL_TxRxEnd;
            
                // EP0
                fres = (*FpXUD_StateEntrys[XUD_RunStatus._dstatus])(&XUD_RunStatus, &XUL_TxRxEnd, &XUL_TxReady, &XUL_RxReady);
            }
            else
            {

                // EP OTHER
                fres = XUC_EPnRequest(&XUD_RunStatus, &XUL_TxRxEnd, &XUL_TxReady, &XUL_RxReady);
            }
        }
        else
        {

            // !! ERROR, System ready for next packet
            fres = _XUD_FRES_ER_PARSE_TK_E01;
        }
    }    

    
    //------------------------------------------------------------
    //              PRIORITY 3: RESET
    //------------------------------------------------------------
    
    if (isrsta & _XUL_INT_DEVICE_RESET)           
    {

        // mark
        isrclr |= _XUL_INT_DEVICE_RESET;        
        fres = XUD_PoweredStatus(&XUD_RunStatus, NULL, &XUL_TxReady, &XUL_RxReady);      
    }


    //------------------------------------------------------------
    //              PRIORITY 4: SOF Frame
    //------------------------------------------------------------
    
    if (isrsta & _XUL_INT_SOF)
    {

        // mark
        isrclr |= _XUL_INT_SOF; XUD_RunStatus._sof_frame = XUL_GetFrameNum();
        XUC_SOF_Request(XUD_RunStatus._sof_frame);
    }


    //------------------------------------------------------------
    //              PRIORITY 5: IDLE
    //------------------------------------------------------------
    
    if (isrsta & _XUL_INT_IDLE_DETECT) isrclr |= _XUL_INT_IDLE_DETECT;                  // mark


    //------------------------------------------------------------
    //              Reset Marked ISR
    //------------------------------------------------------------
    
    XUL_ResetIsrStatus(isrclr);    


    
    //------------------------------------------------------------
    //              Tx/Rx Packet
    //------------------------------------------------------------

    switch (fres)
    {

        //-------------------------------------------------------
        //              NS_RX/ SETUP/ RX
        //-------------------------------------------------------

        case _XUD_FRES_NOT_SUPPORT_RX:                          // SETUP(0)
        case _XUD_FRES_SETUP:                                   // SETUP(0)
        case _XUD_FRES_RX:                                      // OUT
        
            XUL_EPnReadyRx(&XUL_RxReady, NULL);             
            break;


        //-------------------------------------------------------
        //              TX
        //-------------------------------------------------------
        
        case _XUD_FRES_TX:                                      // IN

            XUL_EPnReadyTx(&XUL_TxReady, NULL);
            break;
       

        //-------------------------------------------------------
        //              NS_TX
        //-------------------------------------------------------
        
        case _XUD_FRES_NOT_SUPPORT_TX:                          // SETUP(0) and IN(1) STALL

            XUL_EPnReadyTx(&XUL_TxReady, NULL);                 // IN(1) STALL            
            XUL_EPnReadyRx(&XUL_RxReady, NULL);                 // SETUP(0)
            break;       
        
        default:
            break;
    }   


    //------------------------------------------------------------
    //              Report System Status
    //------------------------------------------------------------
               
    if (FpXUD_TaskReport) (*FpXUD_TaskReport)(isrsta, &XUD_RunStatus, &XUL_TxRxEnd, fres);       
}

/*----------------------------------------------------------------------
 *  XUD_PoweredStatus
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
XUD_FResT XUD_PoweredStatus(XUD_RunStatusT *xrstatus, XUL_TxRxEndT *ptrcp, XUL_TxReadyT *ptxrdy, XUL_RxReadyT *prxrdy)
{

    u8 isr_status = 0;

    //------------------------------------------------------------
    //              Hw Init
    //------------------------------------------------------------

    // Reset
    XUL_Init(XUD_DispatchTasks);
    XUL_EPnInit(0, _XUL_EP_TYPE_CTRL, 0);    


    //------------------------------------------------------------
    //              Run-Status Init
    //------------------------------------------------------------

    XUD_RunStatus._usb_addr = 0;
    XUD_RunStatus._sof_frame = 0;
    XUD_RunStatus._ep0_sreq_now = _XUD_STDR_NONE;
    XUD_RunStatus._ep0_host_need = 0;
    XUD_RunStatus._ep0_total_cnt = 0;
    XUD_RunStatus._ep0_dpassed_cnt = 0;
    XUD_RunStatus._ep0_rdy_now_cnt = 0;
    XUD_RunStatus._p_ep0_desc = 0;
    XUD_RunStatus._dstatus = _XUD_STA_POWERED;
    
    // struct
    _xud_memset(&XUD_RunStatus._ep0_new_tx, sizeof(XUL_TxRxEndT), 0);
    _xud_memset(&XUD_RunStatus._ep0_new_rx, sizeof(XUL_TxRxEndT), 0);
    _xud_memset(&XUD_RunStatus._epn_new_tk, sizeof(XUL_TxRxEndT), 0);
    _xud_memset(&XUD_RunStatus._ep0_dreq_now, sizeof(XUD_DevRequestT), 0);
    

    //------------------------------------------------------------
    //              Ready for First Packet
    //------------------------------------------------------------

    prxrdy->epn = 0;
    prxrdy->even_odd = 0;
    prxrdy->data01 = 0;
    prxrdy->en_dts = 0;
    prxrdy->en_stall = 1;                           // [ STALL ]
    prxrdy->en_own = 1;
    prxrdy->rsize = _XUD_EP0_BUFF_SIZE;             // EP0 max size
    prxrdy->dst = &XUD_RunStatus._ep0_rx_buff[0];   // EP0 buffer address
    

    //------------------------------------------------------------
    //              Enter Default Status
    //------------------------------------------------------------

    xrstatus->_dstatus = _XUD_STA_DEFAULT;

    // SETUP Packet Only
    return _XUD_FRES_SETUP;
}

/*----------------------------------------------------------------------
 *  XUD_DefaultStatus
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
XUD_FResT XUD_DefaultStatus(XUD_RunStatusT *xrstatus, XUL_TxRxEndT *ptrcp, XUL_TxReadyT *ptxrdy, XUL_RxReadyT *prxrdy)
{
    
    XUD_FResT fres = _XUD_FRES_NONE;
    

    //------------------------------------------------------------
    //              EP0 Request
    //------------------------------------------------------------
    
    fres = XUD_EP0_Request(xrstatus, ptrcp, ptxrdy, prxrdy);
    return fres;
}

/*----------------------------------------------------------------------
 *  XUD_AddressStatus
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
XUD_FResT XUD_AddressStatus(XUD_RunStatusT *xrstatus, XUL_TxRxEndT *ptrcp, XUL_TxReadyT *ptxrdy, XUL_RxReadyT *prxrdy)
{

    XUD_FResT fres = _XUD_FRES_NONE;
    

    //------------------------------------------------------------
    //              EP0 Request
    //------------------------------------------------------------
    
    fres = XUD_EP0_Request(xrstatus, ptrcp, ptxrdy, prxrdy);    
    return fres;
}

/*----------------------------------------------------------------------
 *  XUD_ConfuredStatus
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
XUD_FResT XUD_ConfuredStatus(XUD_RunStatusT *xrstatus, XUL_TxRxEndT *ptrcp, XUL_TxReadyT *ptxrdy, XUL_RxReadyT *prxrdy)
{

    XUD_FResT fres = _XUD_FRES_NONE;
    

    //------------------------------------------------------------
    //              EP0 Request
    //------------------------------------------------------------
    
    fres = XUD_EP0_Request(xrstatus, ptrcp, ptxrdy, prxrdy);
    return fres;
}

/*----------------------------------------------------------------------
 *  XUD_SuspendedStatus
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
XUD_FResT XUD_SuspendedStatus(XUD_RunStatusT *xrstatus, XUL_TxRxEndT *ptrcp, XUL_TxReadyT *ptxrdy, XUL_RxReadyT *prxrdy)
{

}

/*----------------------------------------------------------------------
 *  XUD_EP0_Request
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
XUD_FResT XUD_EP0_Request(XUD_RunStatusT *xrstatus, XUL_TxRxEndT *ptrcp, XUL_TxReadyT *ptxrdy, XUL_RxReadyT *prxrdy)
{

    u8 type = 0, reci = 0, trsize = 0, data01 = 0;
    XUD_DevRequestT dreq; XUD_FResT fres = _XUD_FRES_NONE;
    

    //------------------------------------------------------------
    //              Parse Token
    //------------------------------------------------------------
    
    if (ptrcp->token == _XUL_TOKEN_SETUP)
    {

        //-------------------------------------------------------
        //              Parse bmRequestType
        //-------------------------------------------------------
        
        dreq = *(XUD_DevRequestT *)&xrstatus->_ep0_rx_buff[0];        
        type = (dreq._bm_rtype >> 5) & 0x03;        // D6..5
        reci = dreq._bm_rtype & 0x1F;               // D4..0


        //-------------------------------------------------------
        //              Standard Request
        //-------------------------------------------------------
        // Type:
        // 0 = Standard
        // 1 = Class
        // 2 = Vendor ( not used )
        // 3 = Reserved
        // ------
        // Reci:
        // 0 = Device
        // 1 = Interface ( Sub-Class Processing)
        // 2 = End-Point
        // 3 = Other
        // 4-31: Reserved
        if (type == 0)
        {

            // Init Run-Status
            xrstatus->_ep0_sreq_now = dreq._b_req;
            xrstatus->_ep0_dreq_now = dreq;
            xrstatus->_ep0_host_need = dreq._w_len;
            xrstatus->_ep0_total_cnt = 0;
            xrstatus->_ep0_rdy_now_cnt = 0;
            xrstatus->_ep0_dpassed_cnt = 0;
            xrstatus->_p_ep0_desc = 0;

            // Standard Entry
            if ((dreq._b_req < 13) && (FpXUD_StdReqEntry[dreq._b_req] != NULL)) {
                fres = (*FpXUD_StdReqEntry[dreq._b_req])(xrstatus, &dreq, &data01, &trsize);            // VALID
            }
        }
        else
        {

            // Class-Request
            xrstatus->_ep0_sreq_now = _XUD_STDR_NONE;
            xrstatus->_ep0_dreq_now = dreq;
            fres = XUC_EP0_Request(xrstatus, &dreq, &data01, &trsize);
        }
    }
    else
    {

        // Standard Entry
        if (xrstatus->_ep0_sreq_now < _XUD_STDR_CNT) {           
            fres = (*FpXUD_StdReqEntry[xrstatus->_ep0_sreq_now])(xrstatus, NULL, &data01, &trsize);     // VALID
        }
        else
        {

            // Class-Request
            fres = XUC_EP0_Request(xrstatus, NULL, &data01, &trsize);
        }
    }


    //------------------------------------------------------------
    //              Tx/ Rx
    //------------------------------------------------------------

    // Limit value
    if (data01) data01 = _XUL_DATA1;
    if (trsize > _XUD_EP0_BUFF_SIZE) trsize = _XUD_EP0_BUFF_SIZE;

    // Tx/ Rx
    switch (fres)
    {

        case _XUD_FRES_TX:            
            
            if (xrstatus->_ep0_new_tx.token) ptxrdy->even_odd = !xrstatus->_ep0_new_tx.even_odd;
            else ptxrdy->even_odd = 0;
            ptxrdy->epn = 0;
            ptxrdy->data01 = data01;
            ptxrdy->en_dts = 0;
            ptxrdy->en_stall = 0;
            ptxrdy->en_own = 1;
            ptxrdy->tsize = trsize;
            ptxrdy->src = &xrstatus->_ep0_tx_buff[0];            
            
            break;


        case _XUD_FRES_RX:

            if (xrstatus->_ep0_new_rx.token) prxrdy->even_odd = !xrstatus->_ep0_new_rx.even_odd;
            else prxrdy->even_odd = 0;
            prxrdy->epn = 0;
            prxrdy->data01 = data01;
            prxrdy->en_dts = 0;
            prxrdy->en_stall = 0;       
            prxrdy->en_own = 1;
            prxrdy->rsize = trsize;             
            prxrdy->dst = &xrstatus->_ep0_rx_buff[0];
            
            break;


        case _XUD_FRES_NOT_SUPPORT_TX:

            //-------------------------------------------------------
            //              Tx STALL
            //-------------------------------------------------------
            
            if (xrstatus->_ep0_new_tx.token) ptxrdy->even_odd = !xrstatus->_ep0_new_tx.even_odd;
            else ptxrdy->even_odd = 0;
            ptxrdy->epn = 0;
            ptxrdy->data01 = 1;             // DATA1
            ptxrdy->en_dts = 0;
            ptxrdy->en_stall = 1;           // STALL
            ptxrdy->en_own = 1;
            ptxrdy->tsize = 0;
            ptxrdy->src = &xrstatus->_ep0_tx_buff[0];


            //-------------------------------------------------------
            //              Rx SETUP
            //-------------------------------------------------------
            
            if (xrstatus->_ep0_new_rx.token) prxrdy->even_odd = !xrstatus->_ep0_new_rx.even_odd;
            else prxrdy->even_odd = 0;
            prxrdy->epn = 0;
            prxrdy->data01 = 0;             // DATA0
            prxrdy->en_dts = 0;
            prxrdy->en_stall = 1;           // STALL
            prxrdy->en_own = 1;
            prxrdy->rsize = _XUD_EP0_BUFF_SIZE;             
            prxrdy->dst = &xrstatus->_ep0_rx_buff[0];


            //-------------------------------------------------------
            //              Stardand-Req None
            //-------------------------------------------------------
            
            xrstatus->_ep0_sreq_now = _XUD_STDR_NONE;
            xrstatus->_ep0_dreq_now._b_req = (u8)_XUD_STDR_NONE;
            
            break;


        case _XUD_FRES_SETUP:
        case _XUD_FRES_NOT_SUPPORT_RX:

            //-------------------------------------------------------
            //              Rx SETUP
            //-------------------------------------------------------
            
            if (xrstatus->_ep0_new_rx.token) prxrdy->even_odd = !xrstatus->_ep0_new_rx.even_odd;
            else prxrdy->even_odd = 0;
            prxrdy->epn = 0;
            prxrdy->data01 = 0;             // DATA0
            prxrdy->en_dts = 0;
            prxrdy->en_stall = 1;           // STALL
            prxrdy->en_own = 1;
            prxrdy->rsize = _XUD_EP0_BUFF_SIZE;             
            prxrdy->dst = &xrstatus->_ep0_rx_buff[0];


            //-------------------------------------------------------
            //              Stardand-Req None
            //-------------------------------------------------------
            
            xrstatus->_ep0_sreq_now = _XUD_STDR_NONE;
            xrstatus->_ep0_dreq_now._b_req = (u8)_XUD_STDR_NONE;
            
            break;
            

        // [ OTHER ACK ]
        default:
            break;
    }
    
    return fres;
}

/*----------------------------------------------------------------------
 *  XUD_GetStatus
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
XUD_FResT XUD_GetStatus(XUD_RunStatusT *xrstatus, XUD_DevRequestT *devreq, u8 *data01, u8 *trsize)
{

    u8 reci = 0, pwr_mod = 0, epn = 0, eps = 0;


    //------------------------------------------------------------
    //              LOGIC 1: SETUP
    //------------------------------------------------------------

    if (devreq)
    {

        //-------------------------------------------------------
        //              Sub-Class Status
        //-------------------------------------------------------
        
        reci = devreq->_bm_rtype & 0x1F;                                // D4..0
        epn = devreq->_w_idx & 0x0F;                                    // wIndex D3..0
        if (XUC_GetStatus(&pwr_mod, epn, &eps) != _XUD_FRES_OK) return _XUD_FRES_NOT_SUPPORT_TX;


        //-------------------------------------------------------
        //              Tx Data
        //-------------------------------------------------------
        
        if (reci == 0) xrstatus->_ep0_tx_buff[0] = pwr_mod & 0x01;      // device
        else if (reci == 1) xrstatus->_ep0_tx_buff[0] = 0;              // interface
        else if (reci == 2) xrstatus->_ep0_tx_buff[0] = eps & 0x01;     // endpoint
        else return _XUD_FRES_NOT_SUPPORT_TX;                           // [ NOT SUPPORT ]
        xrstatus->_ep0_tx_buff[1] = 0x00;


        //-------------------------------------------------------
        //              STATUS PARSE
        //-------------------------------------------------------

        // [ NOT SUPPORT ]
        if (xrstatus->_dstatus == _XUD_STA_ADDRESS) {
            if (devreq->_w_idx != 0) return _XUD_FRES_NOT_SUPPORT_TX;
        }
        else if (xrstatus->_dstatus == _XUD_STA_CONFIGUARED) {
            if (reci == 2) { if (epn >= _XUL_MAX_EP_NUM) return _XUD_FRES_NOT_SUPPORT_TX; } 
            else { if (devreq->_w_idx != 0) return _XUD_FRES_NOT_SUPPORT_TX; }
        }
        else return _XUD_FRES_NOT_SUPPORT_TX;


        //-------------------------------------------------------
        //              [ DATA STAGE ]
        //-------------------------------------------------------
               
        *data01 = 1; *trsize = 2;
        return _XUD_FRES_TX;                                            // IN(1) [2]
    }
    else
    {

        //-------------------------------------------------------
        //              LOGIC 2: IN/ OUT
        //-------------------------------------------------------

        // [ STATE STAGE ]
        if (xrstatus->_epn_new_tk.token == _XUL_TOKEN_IN) { *data01 = 1; *trsize = 0; return _XUD_FRES_RX; }    // OUT(1) [0]                       
        else if (xrstatus->_epn_new_tk.token == _XUL_TOKEN_OUT) return _XUD_FRES_SETUP;                         // SETUP(0)     
    }


    //------------------------------------------------------------
    //              [ ERROR ]
    //------------------------------------------------------------
    
    return _XUD_FRES_ER_SET_LOGIC_ER_E06;
}

/*----------------------------------------------------------------------
 *  XUD_ClearFeature
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
XUD_FResT XUD_ClearFeature(XUD_RunStatusT *xrstatus, XUD_DevRequestT *devreq, u8 *data01, u8 *trsize)
{

    return _XUD_FRES_NOT_SUPPORT_TX;
}

/*----------------------------------------------------------------------
 *  XUD_SetFeature
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
XUD_FResT XUD_SetFeature(XUD_RunStatusT *xrstatus, XUD_DevRequestT *devreq, u8 *data01, u8 *trsize)
{

    return _XUD_FRES_NOT_SUPPORT_TX;
}

/*----------------------------------------------------------------------
 *  XUD_SetAddress
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
XUD_FResT XUD_SetAddress(XUD_RunStatusT *xrstatus, XUD_DevRequestT *devreq, u8 *data01, u8 *trsize)
{

    
    //------------------------------------------------------------
    //              LOGIC 1: SETUP
    //------------------------------------------------------------

    if (devreq)
    {

        // [ NOT SUPPORT ]
        if (xrstatus->_dstatus == _XUD_STA_CONFIGUARED) return _XUD_FRES_NOT_SUPPORT_TX;
        
        // [ STATE STAGE ]
        *data01 = 1; *trsize = 0; return _XUD_FRES_TX;          // IN(1) [0]
    }
    else
    {

        //-------------------------------------------------------
        //              LOGIC 2: IN
        //-------------------------------------------------------
        
        if (xrstatus->_epn_new_tk.token == _XUL_TOKEN_IN)
        {

            // 0: DEFAULT, 1-127: ADDRESS
            xrstatus->_usb_addr = xrstatus->_ep0_dreq_now._w_val;                                   
            if ((xrstatus->_usb_addr > 0) && (xrstatus->_usb_addr < 128)) xrstatus->_dstatus = _XUD_STA_ADDRESS;
            else xrstatus->_dstatus = _XUD_STA_DEFAULT;         // DEFAULT

            // Successful
            XUL_SetAddress(xrstatus->_usb_addr);            
            return _XUD_FRES_SETUP;                             // SETUP(0)
        }
    }


    //------------------------------------------------------------
    //              [ ERROR ]
    //------------------------------------------------------------
    
    return _XUD_FRES_ER_SET_LOGIC_ER_E06;
}

/*----------------------------------------------------------------------
 *  XUD_GetDescriptor
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
XUD_FResT XUD_GetDescriptor(XUD_RunStatusT *xrstatus, XUD_DevRequestT *devreq, u8 *data01, u8 *trsize)
{

    u8 reci = 0, didx = 0;
    u8 *pdesc = 0;
    XUD_FResT fres = _XUD_FRES_NONE; XUD_DescTypeT dtype = _XUD_DESC_NONE;
    

    //------------------------------------------------------------
    //              LOGIC 1: SETUP/ IN/ OUT
    //------------------------------------------------------------

    if (devreq)
    {

        reci = devreq->_bm_rtype & 0x1F;                    // D4..0
        dtype = (XUD_DescTypeT)(devreq->_w_val >> 8);       // wValue [ D15..8 ]
        didx = devreq->_w_val & 0xFF;                       // wValue [ D7..0 ]


        //-------------------------------------------------------
        //              Init Run-Status
        //-------------------------------------------------------

        // No Descriptor
        // Data Phase: 0
        // State Phase: OUT(1) (rx: 0)
        if (xrstatus->_p_desc_set == 0) return _XUD_FRES_NOT_SUPPORT_TX;        // [ NOT SUPPORT ]
   

        //-------------------------------------------------------
        //              Descriptor Selector
        //------------------------------------------------------- 
        
        switch (dtype)
        {

            //-------------------------------------------------------
            //              #1 DEVICE
            //-------------------------------------------------------
            
            case _XUD_DESC_TYPE_DEVICE:
                
                pdesc = (u8 *)&xrstatus->_p_desc_set->_dev_desc;                // src
                xrstatus->_p_ep0_desc = pdesc;                
                if (pdesc) xrstatus->_ep0_total_cnt = sizeof(XUD_DevDescT);     // size   
                else xrstatus->_ep0_total_cnt = 0;                              // size 0
                
            break;


            //-------------------------------------------------------
            //              #2 CONFIG
            //-------------------------------------------------------

            case _XUD_DESC_TYPE_CONFIG:

                pdesc = xrstatus->_p_desc_set->_p_conf_desc;                    // src
                xrstatus->_p_ep0_desc = pdesc;
                xrstatus->_ep0_total_cnt = xrstatus->_p_desc_set->_conf_size;   // size
                
            break;


            //-------------------------------------------------------
            //              #3 STRING
            //-------------------------------------------------------

            case _XUD_DESC_TYPE_STRING:

                if (didx > 3) xrstatus->_ep0_total_cnt = 0;                     // Must be <= 3
                else
                {                
                    pdesc = xrstatus->_p_desc_set->_str_desc[didx]._p_str;      // src
                    xrstatus->_p_ep0_desc = pdesc;
                    xrstatus->_ep0_total_cnt = xrstatus->_p_desc_set->_str_desc[didx]._b_len;
                }
                
            break;


            //-------------------------------------------------------
            //              OTHER DESCRIPTOR
            //-------------------------------------------------------

            default:

                if (reci == 0) return _XUD_FRES_NOT_SUPPORT_TX;                 // [ NOT SUPPORT ]
                if (XUC_GetDescriptor(devreq, &pdesc, &xrstatus->_ep0_total_cnt) == _XUD_FRES_OK) {
                    xrstatus->_p_ep0_desc = pdesc;
                }
                else xrstatus->_ep0_total_cnt = 0;

            break;

        }


        //-------------------------------------------------------
        //              Size Limiter
        //------------------------------------------------------- 

        if (xrstatus->_ep0_total_cnt == 0) return _XUD_FRES_NOT_SUPPORT_TX;     // [ NOT SUPPORT ]        
        if (devreq->_w_len < xrstatus->_ep0_total_cnt) {
            xrstatus->_ep0_total_cnt = devreq->_w_len;                          // return the HOST needed
        }
               
        // reset counter
        xrstatus->_ep0_dpassed_cnt = 0;
        xrstatus->_ep0_rdy_now_cnt = 0;                                                  
    }
    else
    {        

        //-------------------------------------------------------
        //              IN/ OUT
        //-------------------------------------------------------

        // IN [ device to host, tx ]
        if (xrstatus->_epn_new_tk.token == _XUL_TOKEN_IN)
        {

            // Last tx successful
            if (xrstatus->_epn_new_tk.rtsize == xrstatus->_ep0_rdy_now_cnt)
            {                

                // [ STATE STAGE ]
                // ---
                // (1) Device Descriptor and Addr = 0
                // (2) 0 Packet Tx OK
                if (xrstatus->_ep0_rdy_now_cnt == 0) { *trsize = 0; *data01 = 1; return _XUD_FRES_RX; }     // OUT(1)
                else if ((xrstatus->_usb_addr == 0) && ((xrstatus->_ep0_dreq_now._w_val >> 8) == 0x01))     // SPECIAL
                {

                    // Device Addr: 0, Host Needed > Desc Size
                    if (xrstatus->_ep0_total_cnt < xrstatus->_ep0_host_need) {
                        *trsize = 0; *data01 = 1; return _XUD_FRES_RX;
                    }
                }

                // tx least data
                pdesc = xrstatus->_p_ep0_desc;

                // Passed cnt
                xrstatus->_ep0_dpassed_cnt += xrstatus->_ep0_rdy_now_cnt;
            }
            else
            {

                // [ ERROR ]            
                return _XUD_FRES_ER_EP0_TX_SIZE_E02;
            }
        }
        else if (xrstatus->_epn_new_tk.token == _XUL_TOKEN_OUT)
        {

            // [ SETUP RX ]
            return _XUD_FRES_SETUP;                     // SETUP(0)
        }
        else
        {

            // [ ERROR ]            
            return _XUD_FRES_ER_EP0_TOKEN_CHK_E03;
        }
    }
    

    //------------------------------------------------------------
    //              LOGIC 2: Tx Data
    //------------------------------------------------------------

    // Cacl 'rdy_now_cnt'
    if (xrstatus->_ep0_dpassed_cnt >= xrstatus->_ep0_total_cnt)
    {

        //-------------------------------------------------------
        //              0 Tx or State Phase
        //              0 Tx: total = n*EP0 MAX, total < wLength
        //-------------------------------------------------------

        if (((xrstatus->_ep0_total_cnt % _XUD_EP0_BUFF_SIZE) == 0) && (xrstatus->_ep0_total_cnt < xrstatus->_ep0_host_need))
        {

            // [ 0 Packet ]
            xrstatus->_ep0_rdy_now_cnt = 0;
        }
        else
        {

            // [ STATE STAGE ]
            *trsize = 0; *data01 = 1;                                                            
            return _XUD_FRES_RX;                        // OUT(1)
        }
    }
    else
    {

        //-------------------------------------------------------
        //              Tx Least Data
        //-------------------------------------------------------

        // Limit 64-bytes
        xrstatus->_ep0_rdy_now_cnt = xrstatus->_ep0_total_cnt - xrstatus->_ep0_dpassed_cnt;
        if (xrstatus->_ep0_rdy_now_cnt > _XUD_EP0_BUFF_SIZE) {
            xrstatus->_ep0_rdy_now_cnt = _XUD_EP0_BUFF_SIZE;
        }
    }


    // Get Data From Struct
    pdesc += xrstatus->_ep0_dpassed_cnt;
    for (didx = 0; didx < xrstatus->_ep0_rdy_now_cnt; didx++)
    {
        xrstatus->_ep0_tx_buff[didx] = *pdesc++;
    }


    // [ IN Transaction ]
    *trsize = xrstatus->_ep0_rdy_now_cnt;
    *data01 = !xrstatus->_epn_new_tk.data01;            // DATA Sync
    return _XUD_FRES_TX;
}

/*----------------------------------------------------------------------
 *  XUD_SetDescriptor
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
XUD_FResT XUD_SetDescriptor(XUD_RunStatusT *xrstatus, XUD_DevRequestT *devreq, u8 *data01, u8 *trsize)
{

    // [ NOT SUPPORT ]
    return _XUD_FRES_NOT_SUPPORT_RX;
}

/*----------------------------------------------------------------------
 *  XUD_GetConfiguration
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
XUD_FResT XUD_GetConfiguration(XUD_RunStatusT *xrstatus, XUD_DevRequestT *devreq, u8 *data01, u8 *trsize)
{


    //------------------------------------------------------------
    //              LOGIC 1: SETUP
    //------------------------------------------------------------

    if (devreq)
    {
        
        // Value
        if (xrstatus->_dstatus == _XUD_STA_CONFIGUARED) xrstatus->_ep0_tx_buff[0] = 1;      // only 1 support
        else if (xrstatus->_dstatus == _XUD_STA_ADDRESS) xrstatus->_ep0_tx_buff[0] = 0;     // must 0
        else return _XUD_FRES_NOT_SUPPORT_TX;         
        
        // [ DATA STAGE ]        
        *data01 = 1; *trsize = 1;
        return _XUD_FRES_TX;                                                                // IN(1) [1]
    }
    else
    {

        //-------------------------------------------------------
        //              LOGIC 2: IN/ OUT
        //-------------------------------------------------------

        // [ STATE STAGE ]
        if (xrstatus->_epn_new_tk.token == _XUL_TOKEN_IN) { *data01 = 1; *trsize = 0; return _XUD_FRES_RX; }    // OUT(1) [0]
        else if (xrstatus->_epn_new_tk.token == _XUL_TOKEN_OUT) return _XUD_FRES_SETUP;     // SETUP(0)
    }


    //------------------------------------------------------------
    //              [ ERROR ]
    //------------------------------------------------------------
    
    return _XUD_FRES_ER_SET_LOGIC_ER_E06;
}

/*----------------------------------------------------------------------
 *  XUD_SetConfiguration
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
XUD_FResT XUD_SetConfiguration(XUD_RunStatusT *xrstatus, XUD_DevRequestT *devreq, u8 *data01, u8 *trsize)
{


    //------------------------------------------------------------
    //              LOGIC 1: SETUP
    //------------------------------------------------------------

    if (devreq)
    {        

        //-------------------------------------------------------
        //              STATUS PARSE
        //-------------------------------------------------------

        // [ NOT SUPPORT ]
        if (xrstatus->_dstatus == _XUD_STA_DEFAULT) return _XUD_FRES_NOT_SUPPORT_TX;        


        //-------------------------------------------------------
        //              SET CLASS
        //-------------------------------------------------------

        if (devreq->_w_val == 0) xrstatus->_dstatus = _XUD_STA_ADDRESS;             // 0
        else
        {
            if (XUC_SetConfiguration(devreq->_w_val) == _XUD_FRES_OK) {             // CONFIGURD                
                xrstatus->_dstatus = _XUD_STA_CONFIGUARED;
            }
        }        


        //-------------------------------------------------------
        //              [ STATE STAGE ]
        //-------------------------------------------------------
        
        *data01 = 1; *trsize = 0;
        return _XUD_FRES_TX;                                                        // IN(1) [0]
    }
    else
    {

        //------------------------------------------------------------
        //              LOGIC2: IN
        //------------------------------------------------------------

        // [ SETUP RX ]
        if (xrstatus->_epn_new_tk.token == _XUL_TOKEN_IN) return _XUD_FRES_SETUP;   // SETUP(0)
    }


    //------------------------------------------------------------
    //              [ ERROR ]
    //------------------------------------------------------------
    
    return _XUD_FRES_ER_SET_LOGIC_ER_E06;
}

/*----------------------------------------------------------------------
 *  XUD_GetInterface
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
XUD_FResT XUD_GetInterface(XUD_RunStatusT *xrstatus, XUD_DevRequestT *devreq, u8 *data01, u8 *trsize)
{

    u8 alt = 0;

    
    //------------------------------------------------------------
    //              LOGIC 1: SETUP
    //------------------------------------------------------------

    if (devreq)
    {
        
        //-------------------------------------------------------
        //              STATUS PARSE
        //-------------------------------------------------------
        
        // [ NOT SUPPORT ]
        if (xrstatus->_dstatus != _XUD_STA_CONFIGUARED) return _XUD_FRES_NOT_SUPPORT_TX;


        //-------------------------------------------------------
        //              GET INTERFACE
        //-------------------------------------------------------

        if (XUC_GetInterface(devreq->_w_idx, &xrstatus->_ep0_tx_buff[0]) != _XUD_FRES_OK) {
            return _XUD_FRES_NOT_SUPPORT_TX;
        }
        

        //-------------------------------------------------------
        //              [ DATA STAGE ]
        //-------------------------------------------------------
            
        *data01 = 1; *trsize = 1;
        return _XUD_FRES_TX;                                                                // IN(1) [1]
    }
    else
    {

        //-------------------------------------------------------
        //              LOGIC 2: IN/ OUT
        //-------------------------------------------------------
        
        // [ STATE STAGE ]
        if (xrstatus->_epn_new_tk.token == _XUL_TOKEN_IN) { *data01 = 1; *trsize = 0; return _XUD_FRES_RX; }    // OUT(1) [0]
        else if (xrstatus->_epn_new_tk.token == _XUL_TOKEN_OUT) return _XUD_FRES_SETUP;     // SETUP(0)
    }


    //------------------------------------------------------------
    //              [ ERROR ]
    //------------------------------------------------------------
    
    return _XUD_FRES_ER_SET_LOGIC_ER_E06;
}

/*----------------------------------------------------------------------
 *  XUD_SetInterface
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
XUD_FResT XUD_SetInterface(XUD_RunStatusT *xrstatus, XUD_DevRequestT *devreq, u8 *data01, u8 *trsize)
{


    //------------------------------------------------------------
    //              LOGIC 1: SETUP
    //------------------------------------------------------------

    if (devreq)
    {        

        //-------------------------------------------------------
        //              STATUS PARSE
        //-------------------------------------------------------

        // [ NOT SUPPORT ]
        if (xrstatus->_dstatus != _XUD_STA_CONFIGUARED) return _XUD_FRES_NOT_SUPPORT_TX;        


        //-------------------------------------------------------
        //              SET CLASS
        //-------------------------------------------------------

        // [ NOT SUPPORT ]
        if (XUC_SetInterface(devreq->_w_idx, devreq->_w_val) != _XUD_FRES_OK) return _XUD_FRES_NOT_SUPPORT_TX;


        //-------------------------------------------------------
        //              [ STATE STAGE ]
        //-------------------------------------------------------
        
        *data01 = 1; *trsize = 0;
        return _XUD_FRES_TX;                                                            // IN(1) [0]
    }
    else
    {

        //------------------------------------------------------------
        //              LOGIC2: IN
        //------------------------------------------------------------

        // [ SETUP RX ]
        if (xrstatus->_epn_new_tk.token == _XUL_TOKEN_IN) return _XUD_FRES_SETUP;       // SETUP(0)
    }


    //------------------------------------------------------------
    //              [ ERROR ]
    //------------------------------------------------------------
    
    return _XUD_FRES_ER_SET_LOGIC_ER_E06;
}

/*----------------------------------------------------------------------
 *  XUD_SyncFrame
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
XUD_FResT XUD_SyncFrame(XUD_RunStatusT *xrstatus, XUD_DevRequestT *devreq, u8 *data01, u8 *trsize)
{

    return _XUD_FRES_NOT_SUPPORT_TX;
}

/*----------------------------------------------------------------------
 *  XUD_GetDeviceStatus
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void XUD_GetDeviceStatus(XUD_StatusEnumT *stat, u8 *addr, u16 *fnum)
{

    *stat = XUD_RunStatus._dstatus;
    *addr = XUD_RunStatus._usb_addr; *fnum = XUD_RunStatus._sof_frame;    
}


//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
