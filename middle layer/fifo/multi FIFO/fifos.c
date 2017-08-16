
// v0.0 2017-8-7

#include "fifos.h"


// Clear the all data of the buffer.
void FiFoDpxReset(FiFoDpxT *dpx)
{

    dpx->transmit.write_index = 0;
    dpx->transmit.read_index = 0;
    dpx->transmit.count = 0;
    
    dpx->receive.write_index = 0;
    dpx->receive.read_index = 0;
    dpx->receive.count = 0;

}

//------------------------------------------------------------
// Tx Channel
//------------------------------------------------------------

// Write Tx FIFO
void FiFoDpxWriteTx(FiFoDpxT *dpx, u8 *dat, u16 cnt)
{
    u16 index = 0;
    
    for (; cnt != 0; dat++, cnt--)
    {
    	index = dpx->transmit.count;
    	
    	if (index < sizeof(dpx->transmit.buffer))
    	{
    		(dpx->transmit.count)++;
    		index = dpx->transmit.write_index;
    		dpx->transmit.buffer[index] = *dat;
    		dpx->transmit.write_index = (index + 1) % sizeof(dpx->transmit.buffer);	
    	}
    	else
    	{                
            return;
    	}
    }
}

// return the number of the Tx FIFO
u16 FiFoDpxReadTx(FiFoDpxT *dpx, u8 *dat)
{
    u16 index = 0, cnt = 0;
	
	for (cnt = 0; cnt < FIFO_SPX_SIZE; cnt++)
	{
	    index = dpx->transmit.count;
		
    	// Not Empty
    	if (index)
    	{
    		dpx->transmit.count = (--index);	
    		index = dpx->transmit.read_index;
    		dat[cnt] = dpx->transmit.buffer[index];    		
    		dpx->transmit.read_index = (index + 1) % sizeof(dpx->transmit.buffer);
    	}
    	else
    	{
    	    break;
    	}
	}
	
	return cnt;
}

// return bytes of the Tx FIFO with specified count.
u16 FiFoDpxReadTxBytes(FiFoDpxT *dpx, u8 *dat, u16 num)
{
    u16 index = 0, cnt = 0;
	
	for (cnt = 0; (cnt < FIFO_SPX_SIZE) && (num != 0); cnt++, num--)
	{
	    index = dpx->transmit.count;
		
    	// Not Empty
    	if (index)
    	{
    		dpx->transmit.count = (--index);	
    		index = dpx->transmit.read_index;
    		dat[cnt] = dpx->transmit.buffer[index];    		
    		dpx->transmit.read_index = (index + 1) % sizeof(dpx->transmit.buffer);
    	}
    	else
    	{
    	    break;
    	}
	}
	
	return cnt;
}

u16 FiFoDpxGetTxCount(FiFoDpxT *dpx)
{
    return dpx->transmit.count;
}


//------------------------------------------------------------
// Rx Channel
//------------------------------------------------------------

// Write Rx FIFO
void FiFoDpxWriteRx(FiFoDpxT *dpx, u8 *dat, u16 cnt)
{
    u16 index = 0;
    
    for (; cnt != 0; dat++, cnt--)
    {
    	index = dpx->receive.count;
    	
    	if (index < sizeof(dpx->receive.buffer))
    	{
    		(dpx->receive.count)++;
    		index = dpx->receive.write_index;
    		dpx->receive.buffer[index] = *dat;
    		dpx->receive.write_index = (index + 1) % sizeof(dpx->receive.buffer);	
    	}
    	else
    	{                
            return;
    	}
    }
}

// return the number of the Rx FIFO
u16 FiFoDpxReadRx(FiFoDpxT *dpx, u8 *dat)
{
    u16 index = 0, cnt = 0;
	
	for (cnt = 0; cnt < FIFO_SPX_SIZE; cnt++)
	{
	    index = dpx->receive.count;
		
    	// Not Empty
    	if (index)
    	{
    		dpx->receive.count = (--index);	
    		index = dpx->receive.read_index;
    		dat[cnt] = dpx->receive.buffer[index];    		
    		dpx->receive.read_index = (index + 1) % sizeof(dpx->receive.buffer);
    	}
    	else
    	{
    	    break;
    	}
	}
	
	return cnt;
}

// return bytes of the Rx FIFO with specified count.
u16 FiFoDpxReadRxBytes(FiFoDpxT *dpx, u8 *dat, u16 num)
{
    u16 index = 0, cnt = 0;
	
	for (cnt = 0; (cnt < FIFO_SPX_SIZE) && (num != 0); cnt++, num--)
	{
	    index = dpx->receive.count;
		
    	// Not Empty
    	if (index)
    	{
    		dpx->receive.count = (--index);	
    		index = dpx->receive.read_index;
    		dat[cnt] = dpx->receive.buffer[index];    		
    		dpx->receive.read_index = (index + 1) % sizeof(dpx->receive.buffer);
    	}
    	else
    	{
    	    break;
    	}
	}
	
	return cnt;
}

u16 FiFoDpxGetRxCount(FiFoDpxT *dpx)
{
    return dpx->receive.count;
}
