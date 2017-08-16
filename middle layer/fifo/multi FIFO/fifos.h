
#ifndef _FIFOS_H
#define _FIFOS_H

#include "main.h"


#define FIFO_SPX_SIZE	        200     // UNIT: Byte

//------------------------------------------------------------
// FIFO Simplex Channel
//------------------------------------------------------------

typedef struct {

	u16 write_index;
	u16 read_index;
	u16 count;
	u8 buffer[FIFO_SPX_SIZE];
	
} FiFoSpxT;

//------------------------------------------------------------
// FIFO Duplex Channel
//------------------------------------------------------------
typedef struct {

    FiFoSpxT transmit;
    FiFoSpxT receive;

} FiFoDpxT;

void FiFoDpxReset(FiFoDpxT *dpx);

void FiFoDpxWriteTx(FiFoDpxT *dpx, u8 *dat, u16 cnt);
u16 FiFoDpxReadTx(FiFoDpxT *dpx, u8 *dat);
u16 FiFoDpxReadTxBytes(FiFoDpxT *dpx, u8 *dat, u16 num);
u16 FiFoDpxGetTxCount(FiFoDpxT *dpx);

void FiFoDpxWriteRx(FiFoDpxT *dpx, u8 *dat, u16 cnt);
u16 FiFoDpxReadRx(FiFoDpxT *dpx, u8 *dat);
u16 FiFoDpxReadRxBytes(FiFoDpxT *dpx, u8 *dat, u16 num);
u16 FiFoDpxGetRxCount(FiFoDpxT *dpx);


#endif
