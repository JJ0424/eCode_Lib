
/*----------------------------------------------------------------------^^-
/ File name:  t_bus.c
/ Author:     JiangJun
/ Data:       2017/10/19
/ Version:    v1.1
/-----------------------------------------------------------------------^^-
/  T-BUS receiver
/ ---
/ v0.0 2017/04/14
/ ---
/ v0.1 2017/04/27
/ 1. Modify 'T_BUS_HEADER_SIZE' error
/ ---
/ v0.2 2017/05/03
/ 1. Add CheckSum Code
/ ---
/ v0.3 2017/06/22
/ 1. TBusInit remove UART Init function, so, you need init UART Before 'TBusInit()'
/ ---
/ v1.0 2017/08/07
/ 1. T_Bus.c/.h to common file
/ ---
/ v1.1 [2017-10-19]
/ 1. remove port function(TBusTx/TBusGetRxCount/TBusReadRxByte)
/ 2. protocal to v1.1
/------------------------------------------------------------------------*/

#include "t_bus.h"

#include "string.h"
#include "common.h"


//------------------------------------------------------------
// Packet Constant
//------------------------------------------------------------


#define TBUS_STX_OFFSET                     0
#define TBUS_PAD_LEN_OFFSET                 1       // 2-bytes payload length
#define TBUS_PAD_LEN_SIZE                   2       // size_L size_H

#if T_BUS_ENABLE_LOOP_SEQ

  #define TBUS_LOOP_SEQ_OFFSET              3
  #define TBUS_PKG_ID_OFFSET                (TBUS_LOOP_SEQ_OFFSET + 1)
  #define TBUS_LOOP_SEP_SET(buffer, val)    buffer = val;    
  
#else
  
  #define TBUS_LOOP_SEQ_OFFSET              3
  #define TBUS_PKG_ID_OFFSET                TBUS_LOOP_SEQ_OFFSET
  #define TBUS_LOOP_SEP_SET(buffer, val)    ;      
#endif


//------------------------------------------------------------
// Private variables
//------------------------------------------------------------

#if T_BUS_ENABLE_LOOP_SEQ

  static u8 TbusSeq = 0;
  
#endif

//------------------------------------------------------------
// Port Function
//------------------------------------------------------------

static fpTBusTx FpTBusTx = NULL; static fpTBusGetRxCount FpTBusGetRxCount = NULL;
static fpTBusReadRxByte FpTBusReadRxByte = NULL;



/*----------------------------------------------------------------------
 *  TBusInit 
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void TBusInit(fpTBusTx fp_tbus_tx, fpTBusReadRxByte fp_rx_byte, fpTBusGetRxCount fp_get_cnt)
{

    FpTBusTx = fp_tbus_tx;
    FpTBusGetRxCount = fp_get_cnt;
    FpTBusReadRxByte = fp_rx_byte;
    
}

/*----------------------------------------------------------------------
 *  TBusEncPkg - encode the raw packet to Link Packet
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void TBusEncPkg(TBusTxPkgT *pkg)
{

    u8 buf[T_BUS_BYTE_BUFFER_SIZE];
    
    //------------------------------------------------------------
    // Header
    //------------------------------------------------------------
    buf[TBUS_STX_OFFSET] = TBUS_STX;                                // start flag
    
    *((u16*)&buf[TBUS_PAD_LEN_OFFSET]) = pkg->dat_len;              // payload length    
    
    TBUS_LOOP_SEP_SET(buf[TBUS_LOOP_SEQ_OFFSET], TbusSeq++);        // loop seq             
    buf[TBUS_PKG_ID_OFFSET] = pkg->pkg_id;                          // message ID
    
    // Payload Field
    memcpy(&buf[T_BUS_HEADER_SIZE], &pkg->payload[0], pkg->dat_len);
    
    // CheckSum Field
    buf[T_BUS_HEADER_SIZE + pkg->dat_len] = XorCheckSum(buf, T_BUS_HEADER_SIZE + pkg->dat_len);
    
    //------------------------------------------------------------
    // Write Tx FIFO
    //------------------------------------------------------------
    
    if (FpTBusTx != NULL) 
    {
        (*FpTBusTx)(buf, T_BUS_HEADER_SIZE + pkg->dat_len + T_BUS_END_SIEZ); 
    }
    
}

/*----------------------------------------------------------------------
 *  TBusDecPkg - decode the buffer to packet.
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
Bool TBusDecPkg(u8 *src, u16 len, TBusRxPkgT *pkg)
{    
    
    // Total bytes check
    if (len > (T_BUS_MAX_PAYLOAD_SIZE + T_BUS_HEADER_SIZE + T_BUS_END_SIEZ)) { return FALSE; }
    
    // CheckSum
    if (src[len - T_BUS_END_SIEZ] != XorCheckSum(src, len - T_BUS_END_SIEZ)) { return FALSE; }
    
    // Payload Fill    
    pkg->dat_len = *((u16*)&src[TBUS_PAD_LEN_OFFSET]);              // payload length
    TBUS_LOOP_SEP_SET(pkg->seq, src[TBUS_LOOP_SEQ_OFFSET]);         // loop seq     
    pkg->pkg_id = src[TBUS_PKG_ID_OFFSET];                          // message ID
    
    // Payload length check
    if (pkg->dat_len != (len - T_BUS_HEADER_SIZE - T_BUS_END_SIEZ)) { return FALSE; }
    
    //------------------------------------------------------------
    // Payload
    //------------------------------------------------------------
    memcpy(&pkg->payload[0], &src[T_BUS_HEADER_SIZE], pkg->dat_len);    
    
    return TRUE;
}

/*----------------------------------------------------------------------
 *  TBusGetPkg - Get a packet from the FIFO Buffer.
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
Bool TBusGetPkg(TBusRxPkgT *pkg)
{    

    static u8 buf[T_BUS_BYTE_BUFFER_SIZE]; static u16 cnt = 0, pld_len = 0;
	u8 dat;


	//------------------------------------------------------------
    // Port Function detect
    //------------------------------------------------------------

    if (FpTBusGetRxCount == NULL) { return FALSE; }

    if (FpTBusReadRxByte == NULL) { return FALSE; }


    //------------------------------------------------------------
    // Rx with buffer
    //------------------------------------------------------------
    
    for (; ; )
    {       
        if ((*FpTBusGetRxCount)() == 0) return FALSE;                   // NO data in buffer.
            
        if ((*FpTBusReadRxByte)(&dat) != 1) return FALSE;               // No data in buffer.

        
        if ((cnt == 0) && (dat != TBUS_STX))                            // Check the 'STX'                                          
        {
            continue; 
        }                                
        
        if (cnt < T_BUS_BYTE_BUFFER_SIZE) { buf[cnt++] = dat; }         // Storage and Overload detect
        else { cnt = 0; }


        if (cnt == (TBUS_PAD_LEN_OFFSET + TBUS_PAD_LEN_SIZE))           // Get the Payload length
        {
            pld_len = *((u16 *)&buf[TBUS_PAD_LEN_OFFSET]);   
        }      

        
        // Check if Geting a packet
        if (cnt == (pld_len + T_BUS_HEADER_SIZE + T_BUS_END_SIEZ)) 
        {
            if (TBusDecPkg(buf, cnt, pkg) == TRUE) { cnt = 0; pld_len = 0; return TRUE; }
            else { cnt = 0; pld_len = 0; return FALSE; }
        }
    }
}
