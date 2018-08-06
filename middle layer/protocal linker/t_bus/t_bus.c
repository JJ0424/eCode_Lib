
/*----------------------------------------------------------------------^^-
/ File name:  t_bus.c
/ Author:     JiangJun
/ Data:       2018/08/03
/ Version:    v2.0
/-----------------------------------------------------------------------^^-
/ T-BUS Driver
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
/ ---
/ v1.2 [2017-10-20]
/ 1. add TBusPktToPload()
/ ---
/ v2.0 [2018-8-3]
/ 1. t_bus.c/.h to common protocal
/------------------------------------------------------------------------*/

#include "t_bus.h"

#include "common.h"
#include "strlib.h"


/*----------------------------------------------------------------------
 *  TBusInit 
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void TBusInit(TBusWorkT *work, const u8 *header)
{

    work->loop_idx = 0;
    work->head = (u8 *)header;
    
    // header size
    if (work->head != 0) {

        work->head_size = _strlen(work->head);
    }
    else {

        work->head_size = 0;
    }

    work->rx_cnt = 0; work->pld_len = 0;
}

/*----------------------------------------------------------------------
 *  TBusTxPkt --- [WORK] -> [PKT] -> [UART]
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_bool TBusTxPkt(TBusWorkT *work, u8 pkt_id, u8 *src, u16 cnt)
{

    TBusPktT tx_pkt; u16 idx = 0; TBusWorkT *work_tmp = work;


    //------------------------------------------------------------
    // CHECK
    //------------------------------------------------------------
    
    if ((work_tmp->head_size + sizeof(TBusFixFieldT) + cnt + 1) > work_tmp->buff_len)
    {

        return FALSE;
    }


    //------------------------------------------------------------
    // To BuF
    //------------------------------------------------------------
    
    // header
    tx_pkt.head = work_tmp->head; tx_pkt.head_size = work_tmp->head_size;
    
    tx_pkt.fix.size = cnt; tx_pkt.fix.loop_idx = work_tmp->loop_idx++;
    tx_pkt.fix.ver_ID = _T_BUS_VER; tx_pkt.fix.pkt_ID = pkt_id;
    tx_pkt.payload = src;

    // [to work buff]
    _memcpy(&work_tmp->tx_buff[idx], tx_pkt.head, tx_pkt.head_size); idx += tx_pkt.head_size;
    _memcpy(&work_tmp->tx_buff[idx], (u8 *)&tx_pkt.fix, sizeof(TBusFixFieldT)); idx += sizeof(TBusFixFieldT);
    _memcpy(&work_tmp->tx_buff[idx], tx_pkt.payload, tx_pkt.fix.size); idx += tx_pkt.fix.size;

    // XOR check
    tx_pkt.xor_chk = XorCheckSum(work_tmp->tx_buff, idx);    
    work_tmp->tx_buff[idx] = tx_pkt.xor_chk; idx += 1;


    //------------------------------------------------------------
    // Write Tx FIFO
    //------------------------------------------------------------

    if (work_tmp->fp_tx != 0)
    {

        (*work_tmp->fp_tx)(work_tmp->tx_buff, idx);
    }
    
    return TRUE;
}

/*----------------------------------------------------------------------
 *  TBusArrayToPkt --- [UART] -> [WORK] -> [PKT]
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_bool TBusArrayToPkt(u8 *header, u8 *src, u16 cnt, TBusPktT *pkt)
{

    s16 idx = 0; u8 *src_tmp = src; u16 pkt_cnt = 0;


    //------------------------------------------------------------
    // Pause Header
    //------------------------------------------------------------
    
    // Get "Header" index
    idx = _strstr(src, cnt, header);
    if (idx < 0) 
    { 
        return FALSE; 
    }
    

    //------------------------------------------------------------
    // To Pkt
    //------------------------------------------------------------
    
    // [to pkt]
    pkt->head = header; pkt->head_size = _strlen(header); pkt_cnt += pkt->head_size;
    pkt->fix = *((TBusFixFieldT *)(src_tmp + pkt_cnt + idx)); pkt_cnt += sizeof(TBusFixFieldT);
    pkt->payload = src_tmp + pkt_cnt + idx; pkt_cnt += pkt->fix.size;
    pkt->xor_chk = *(src_tmp + pkt_cnt + idx);

    if (pkt->xor_chk != XorCheckSum(src_tmp + idx, pkt_cnt)) { return FALSE; }        
    pkt_cnt += 1;
        

    //------------------------------------------------------------
    // CHECK
    //------------------------------------------------------------

    if (pkt_cnt > cnt) { return FALSE; }
    return TRUE;
}

/*----------------------------------------------------------------------
 *  TBusPausePkt - pause one packet
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    Example(Header: #TBUS):
 *                                  (1) #TBUS...
 *                                  (2) 1234XXX#TBUS...
 *                                  (3) ##TBUS...
 *                                  (4) XXXX###TBUS...
 *---------------------------------------------------------------------*/
_bool TBusPausePkt(TBusWorkT *work, TBusPktT *pkt)
{    
    
	u8 dat[1]; u16 cnt = 0;


	//------------------------------------------------------------
    //                      CHECK
    //------------------------------------------------------------

    if ((work->fp_rx == 0) || (work->fp_rx_cnt == 0)) {
        return FALSE;
    }


    //------------------------------------------------------------
    //                      B2B Read
    //------------------------------------------------------------
    
    for (cnt = (*work->fp_rx_cnt)(); ; cnt--)
    {

        // Read one byte
        if (cnt == 0) { return FALSE; }            
        if ((*work->fp_rx)(dat, 1) != 1) { return FALSE; }        

        // HDR Pause
        if ((work->rx_cnt < work->head_size) && (dat[0] != work->head[work->rx_cnt]))
        {

            work->rx_cnt = 0; work->pld_len = 0;

            // ## Field
            if (dat[0] != work->head[work->rx_cnt])
            {
                continue;
            }
        }

        // Storage and Overload detect
        if (work->rx_cnt < work->buff_len) { work->rx_buff[work->rx_cnt++] = dat[0]; }
        else 
        {        
            work->rx_cnt = 0; work->pld_len = 0; continue;
        }

        // Payload len Pause
        if (work->rx_cnt == (work->head_size + sizeof(u16))) {
        
            work->pld_len = *(u16 *)(work->rx_buff + work->head_size);
        }
        
        // Check if Geting a packet
        if (work->rx_cnt >= (work->head_size + sizeof(TBusFixFieldT) + work->pld_len + 1)) 
        {        
            break;
        }
    }


    //------------------------------------------------------------
    //                      Pause TBus Packet
    //------------------------------------------------------------

    if (TBusArrayToPkt(work->head, work->rx_buff, work->rx_cnt, pkt) == TRUE) 
    { 

        work->rx_cnt = 0; work->pld_len = 0; 
        return TRUE; 
    }
    else 
    {

        work->rx_cnt = 0; work->pld_len = 0;
        return FALSE;
    }
}


//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
