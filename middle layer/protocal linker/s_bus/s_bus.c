
/*----------------------------------------------------------------------^^-
/ File name:  s_bus.c
/ Author:     JiangJun
/ Data:       2017/10/19
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ S-BUS receiver
/------------------------------------------------------------------------*/


#include "s_bus.h"

#include "string.h"
#include "common.h"
#include "strlib.h"


//------------------------------------------------------------
// Port Function
//------------------------------------------------------------

static fpSBusTxBytes FpSBusTxBytes = NULL; static fpSBusGetRxCount  FpSBusGetRxCount = NULL;
static fpSBusRxBytes FpSBusRxBytes = NULL; static fpSBusResetRx FpSBusResetRx = NULL;


// Loop Seq
static u8 SbusSeq = 0;



/*----------------------------------------------------------------------
 *  SBusInit 
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void SBusInit(fpSBusTxBytes fp_sbus_tx, fpSBusRxBytes fp_rx_byte, fpSBusGetRxCount fp_get_cnt, fpSBusResetRx fp_rst_rx)
{

    FpSBusTxBytes = fp_sbus_tx;
    FpSBusGetRxCount = fp_get_cnt;
    FpSBusRxBytes = fp_rx_byte;
    FpSBusResetRx = fp_rst_rx;
    
}

/*----------------------------------------------------------------------
 *  SBusTxPayload 
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/

void SBusTxPayload(u8 cmd_dat, u8 *payload, u16 size)
{

    SBusPktT tx_pkg;

    // header
    memcpy(&tx_pkg.sbus_header.head[0], _S_BUS_STX, _S_BUS_STX_LEN);
          
    tx_pkg.sbus_header.cmd_dat = cmd_dat;       // [D/C]
    tx_pkg.sbus_header.reg0 = 0x00;
    tx_pkg.sbus_header.loop_idx = SbusSeq++;    // Loop Req
    tx_pkg.sbus_header.size = size;
    tx_pkg.sbus_header.xor_chk = XorCheckSum(&tx_pkg.sbus_header.cmd_dat, sizeof(SBusHdrT) - _S_BUS_STX_LEN - 2);
    tx_pkg.sbus_header.end_char = '*';

    // payload
    memcpy(&tx_pkg.payload[0], payload, size);


    //------------------------------------------------------------
    // Write Tx FIFO
    //------------------------------------------------------------
    
    if (FpSBusTxBytes != NULL) 
    {
        (*FpSBusTxBytes)((u8 *)&tx_pkg, sizeof(SBusHdrT) + size); 
    }
    
}

/*----------------------------------------------------------------------
 *  SBusArrayToPkt 
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
Bool SBusArrayToPkt(u8 *sbus_buf, u16 cnt, SBusPktT *pkg)
{

    s16 idx = 0;
    
    // Get "#sbus" index
    idx = _strstr(sbus_buf, cnt, _S_BUS_STX);

    if (idx < 0) { return FALSE; }

    
    // SBus header checksum
    pkg->sbus_header = *((SBusHdrT *)&sbus_buf[idx]);

    if (XorCheckSum(&sbus_buf[idx + _S_BUS_STX_LEN], sizeof(SBusHdrT) - _S_BUS_STX_LEN - 2) != pkg->sbus_header.xor_chk) { return FALSE; }
        

    // Payload copy
    if ((cnt - sizeof(SBusHdrT) - idx) >= pkg->sbus_header.size)
    {        
        memcpy(&pkg->payload[0], &sbus_buf[idx + sizeof(SBusHdrT)], pkg->sbus_header.size);
    }
    else { return FALSE; }


    // Successful Pause one Packet.
    return TRUE;
    
}



/*----------------------------------------------------------------------
 *  SBusPausePkt - Get a packet from the FIFO Buffer.
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    1. 不支持缓冲，每次从FIFO中读取所有数据做检索，返回第一包数据包
 *           剩余数据将会被忽略。
 *
 *           2. 若发现FIFO中数据比缓冲数组多，则复位FIFO，
 *           返回失败
 *           
 *---------------------------------------------------------------------*/
Bool SBusPausePkt(SBusPktT *pkg)
{    

    u8 sbus_buf[_S_BUS_BYTE_BUFFER_SIZE]; u16 cnt = 0;
    s16 idx = 0;


	//------------------------------------------------------------
    // Port Function detect
    //------------------------------------------------------------

    if (FpSBusGetRxCount == NULL) { return FALSE; }

    if (FpSBusRxBytes == NULL) { return FALSE; }

    if (FpSBusResetRx == NULL) { return FALSE; }


    //------------------------------------------------------------
    // Rx with buffer
    //------------------------------------------------------------   
    
    // Get count with Rx FIFO
    cnt = (*FpSBusGetRxCount)();   

    // Get data from Rx FIFO
    if (cnt == 0)                           // No data.
    {
    
        return FALSE;         
    }
    else if (cnt > _S_BUS_BYTE_BUFFER_SIZE) 
    {
    
        (*FpSBusResetRx)();                 // Reset Rx FIFO.
        
        return FALSE; 
    }   
    else
    {
    
        (*FpSBusRxBytes)(sbus_buf, cnt);    // Successful Rx datas.
    }


    //------------------------------------------------------------
    // Packet pause
    //------------------------------------------------------------ 
        
    // Get "#sbus" index
    idx = _strstr(sbus_buf, cnt, _S_BUS_STX);

    if (idx < 0) { return FALSE; }

    
    // SBus header checksum
    pkg->sbus_header = *((SBusHdrT *)&sbus_buf[idx]);

    if (XorCheckSum(&sbus_buf[idx + _S_BUS_STX_LEN], sizeof(SBusHdrT) - _S_BUS_STX_LEN - 2) != pkg->sbus_header.xor_chk) { return FALSE; }
        

    // Payload copy
    if ((cnt - sizeof(SBusHdrT) - idx) >= pkg->sbus_header.size)
    {        
        memcpy(&pkg->payload[0], &sbus_buf[idx + sizeof(SBusHdrT)], pkg->sbus_header.size);
    }
    else { return FALSE; }


    // Successful Pause one Packet.
    return TRUE;
}


