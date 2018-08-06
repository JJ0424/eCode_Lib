
/*----------------------------------------------------------------------^^-
/ File name:  fifo.c
/ Author:     JiangJun
/ Data:       2018/8/2
/ Version:    v0.1
/-----------------------------------------------------------------------^^-
/ FIFO Driver
/------------------------------------------------------------------------*/

#include "fifo.h"

/*----------------------------------------------------------------------
 *  FiFoSpxInit
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void FiFoSpxInit(FiFoSpxT *dpx, _u8_t *buff, _idx_u16_t buff_len)
{

    // Init
    dpx->_w_idx = 0; dpx->_r_idx = 0;
    dpx->_cnt = 0; dpx->_total_cnt = 0; dpx->_full = 0;
    if (buff != 0) {

        dpx->buff = buff; dpx->buff_len = buff_len;
    }
    else {

        dpx->buff = 0; dpx->buff_len = 0;
    }    
}

/*----------------------------------------------------------------------
 *  FiFoSpxWrite - Write FIFO
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void FiFoSpxWrite(FiFoSpxT *spx, _u8_t *dat, _idx_u16_t cnt)
{

    _idx_u16_t idx = 0; _u8_t *dat_tmp = dat; FiFoSpxT *spx_tmp = spx;

    // total count +
    spx_tmp->_total_cnt += cnt;

    // check
    if ((spx_tmp->buff == 0) || (spx_tmp->buff_len == 0)) {

        spx_tmp->_full = 0xFF;
        return;
    }
           
    // b2b write
    for (idx = 0; idx < cnt; idx++)
    {

        spx_tmp->buff[spx_tmp->_w_idx] = *dat_tmp; dat_tmp++;
        spx_tmp->_w_idx++; spx_tmp->_w_idx %= spx_tmp->buff_len;
        
    	// not full
    	if (spx_tmp->_cnt < spx_tmp->buff_len) { spx_tmp->_cnt++; }
    }

    // full check
    if (spx_tmp->_cnt >= spx_tmp->buff_len) { spx_tmp->_full = 0xFF; }
}

/*----------------------------------------------------------------------
 *  FiFoSpxReadBytes
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_idx_u16_t FiFoSpxReadBytes(FiFoSpxT *spx, _u8_t *dat, _idx_u16_t cnt)
{

    _idx_u16_t idx = 0; _u8_t *dat_tmp = dat; FiFoSpxT *spx_tmp = spx;

	// check
    if ((spx_tmp->buff == 0) || (spx_tmp->buff_len == 0)) {
        
        return 0;
    }

    // b2b read
    for (idx = 0; (idx < cnt) && (idx < spx_tmp->buff_len); idx++)
	{

        // not empty
    	if (spx_tmp->_cnt == 0) { break; }
    	
	    *dat_tmp = spx_tmp->buff[spx_tmp->_r_idx]; dat_tmp++;
	    spx_tmp->_r_idx++; spx_tmp->_r_idx %= spx_tmp->buff_len;
	    spx_tmp->_cnt--;
	}

	// full clear
	if (spx_tmp->_cnt < spx_tmp->buff_len) { spx_tmp->_full = 0x00; }
	
	return idx;
}

/*----------------------------------------------------------------------
 *  FiFoSpxGetCount
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_idx_u16_t FiFoSpxGetCount(FiFoSpxT *spx)
{

    return spx->_cnt;
}

/*----------------------------------------------------------------------
 *  FiFoSpxIsEmpty - 0xFF Empty
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_u8_t FiFoSpxIsEmpty(FiFoSpxT *spx)
{

    if (spx->_cnt == 0) { return 0xFF; }
    return 0x00;
}

/*----------------------------------------------------------------------
 *  FiFoSpxIsFull - 0xFF Full
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_u8_t FiFoSpxIsFull(FiFoSpxT *spx)
{

    if (spx->_full) { return 0xFF; }
    return 0x00;
}

//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
