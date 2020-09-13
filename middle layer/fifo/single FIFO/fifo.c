
/*----------------------------------------------------------------------^^-
/ File name:  fifo.c
/ Author:     JiangJun
/ Data:       [2020-9-13]
/ Version:    v0.2
/-----------------------------------------------------------------------^^-
/ FIFO Driver
/ ---
/ v0.2 [2020-9-13]
/ 1. add reset function
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
void FiFoSpxInit(FiFoSpxT *spx, _u8_t *buff, _idx_u16_t buff_len)
{

    // Init
    spx->_w_idx = 0; spx->_r_idx = 0;
    spx->_cnt = 0; spx->_total_cnt = 0; spx->_full = 0;
    if (buff != 0) {

        spx->buff = buff; spx->buff_len = buff_len;
    }
    else {

        spx->buff = 0; spx->buff_len = 0;
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

/*----------------------------------------------------------------------
 *  FiFoSpxReset
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void FiFoSpxReset(FiFoSpxT *spx)
{

    spx->_w_idx = 0; spx->_r_idx = 0;
    spx->_cnt = 0; spx->_total_cnt = 0; spx->_full = 0;
}

//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
