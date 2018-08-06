
/*----------------------------------------------------------------------^^-
/ File name:  fifo.c
/ Author:     JiangJun
/ Data:       2018/8/2
/ Version:    v0.1
/-----------------------------------------------------------------------^^-
/ FIFO Driver
/------------------------------------------------------------------------*/

#ifndef _FIFO_H
#define _FIFO_H

//------------------------------------------------------------
//              TYPEDEF
//------------------------------------------------------------

typedef unsigned char _u8_t;
typedef unsigned short _idx_u16_t;
typedef unsigned int _idx_u32_t;

//------------------------------------------------------------
//              FIFO Simplex Channel
//------------------------------------------------------------

typedef struct {

	_idx_u16_t _w_idx;
	_idx_u16_t _r_idx;
	_idx_u16_t _cnt;
	_idx_u32_t _total_cnt;
	_u8_t *buff;
	_idx_u16_t buff_len;
	_u8_t _full;
	
} FiFoSpxT;

//------------------------------------------------------------
//              FIFO Duplex Channel
//------------------------------------------------------------

typedef struct {

    FiFoSpxT _tx_channel;
    FiFoSpxT _rx_channel;
    
} FiFoDpxT;

extern void FiFoSpxInit(FiFoSpxT *dpx, _u8_t *buff, _idx_u16_t buff_len);
extern void FiFoSpxWrite(FiFoSpxT *spx, _u8_t *dat, _idx_u16_t cnt);
extern _idx_u16_t FiFoSpxReadBytes(FiFoSpxT *spx, _u8_t *dat, _idx_u16_t cnt);
extern _idx_u16_t FiFoSpxGetCount(FiFoSpxT *spx);
extern _u8_t FiFoSpxIsEmpty(FiFoSpxT *spx);
extern _u8_t FiFoSpxIsFull(FiFoSpxT *spx);

#endif
