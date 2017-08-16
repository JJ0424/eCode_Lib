
#ifndef _FIFO_H
#define _FIFO_H

#include "main.h"

#define ENABLE_FIFO_TX  1
#define ENABLE_FIFO_RX  1

#define FIFO_SIZE	        512     // UNIT: Byte

//------------------------------------------------------------
// FIFO Data Struct Type define.
//------------------------------------------------------------
#if FIFO_SIZE >= 256
	typedef u16	IndexT;
#else
	typedef u8  IndexT;
#endif

typedef struct {
	IndexT write_index;
	IndexT read_index;
	IndexT count;
	u8 buffer[FIFO_SIZE];
} FIFOT;

//------------------------------------------------------------
// The structure of the one UART.
//------------------------------------------------------------
typedef struct {

#if ENABLE_FIFO_TX
    FIFOT transmit;
#endif

#if ENABLE_FIFO_RX
    FIFOT receive;
#endif
} FiFoChannelT;

void FiFoReset(void);

void FiFoWriteTx(u8 *dat, IndexT cnt);
IndexT FiFoReadTx(u8 *dat);
IndexT FiFoReadTxBytes(u8 *dat, IndexT num);
IndexT FiFoGetTxCount(void);

void FiFoWriteRx(u8 *dat, IndexT cnt);
IndexT FiFoReadRx(u8 *dat);
IndexT FiFoReadRxBytes(u8 *dat, IndexT num);
IndexT FiFoGetRxCount(void);


#endif
