
// v0.0 2016-8-18

#include "fifo.h"

//------------------------------------------------------------
// FIFO Buffer
//------------------------------------------------------------
FiFoChannelT FiFoChannel;


// Clear the all data of the buffer.
void FiFoReset(void)
{
#if ENABLE_FIFO_TX

    FiFoChannel.transmit.write_index = 0;
    FiFoChannel.transmit.read_index = 0;
    FiFoChannel.transmit.count = 0;
    
#endif    

#if ENABLE_FIFO_RX

    FiFoChannel.receive.write_index = 0;
    FiFoChannel.receive.read_index = 0;
    FiFoChannel.receive.count = 0;

#endif
}

#if ENABLE_FIFO_TX

// Write Tx FIFO
void FiFoWriteTx(u8 *dat, IndexT cnt)
{
    IndexT index = 0;
    
    for (; cnt != 0; dat++, cnt--)
    {
    	index = FiFoChannel.transmit.count;
    	
    	if (index < sizeof(FiFoChannel.transmit.buffer))
    	{
    		(FiFoChannel.transmit.count)++;
    		index = FiFoChannel.transmit.write_index;
    		FiFoChannel.transmit.buffer[index] = *dat;
    		FiFoChannel.transmit.write_index = (index + 1) % sizeof(FiFoChannel.transmit.buffer);	
    	}
    	else
    	{                
            return;
    	}
    }
}

// return the number of the Tx FIFO
IndexT FiFoReadTx(u8 *dat)
{
    IndexT index = 0, cnt = 0;
	
	for (cnt = 0; cnt < FIFO_SIZE; cnt++)
	{
	    index = FiFoChannel.transmit.count;
		
    	// Not Empty
    	if (index)
    	{
    		FiFoChannel.transmit.count = (--index);	
    		index = FiFoChannel.transmit.read_index;
    		dat[cnt] = FiFoChannel.transmit.buffer[index];    		
    		FiFoChannel.transmit.read_index = (index + 1) % sizeof(FiFoChannel.transmit.buffer);
    	}
    	else
    	{
    	    break;
    	}
	}
	
	return cnt;
}

IndexT FiFoGetTxCount(void)
{
    return FiFoChannel.transmit.count;
}

#endif /* ENABLE_FIFO_TX */

#if ENABLE_FIFO_RX

// Write Rx FIFO
void FiFoWriteRx(u8 *dat, IndexT cnt)
{
    IndexT index = 0;
    
    for (; cnt != 0; dat++, cnt--)
    {
    	index = FiFoChannel.receive.count;
    	
    	if (index < sizeof(FiFoChannel.receive.buffer))
    	{
    		(FiFoChannel.receive.count)++;
    		index = FiFoChannel.receive.write_index;
    		FiFoChannel.receive.buffer[index] = *dat;
    		FiFoChannel.receive.write_index = (index + 1) % sizeof(FiFoChannel.receive.buffer);	
    	}
    	else
    	{                
            return;
    	}
    }
}

// return the number of the Rx FIFO
IndexT FiFoReadRx(u8 *dat)
{
    IndexT index = 0, cnt = 0;
	
	for (cnt = 0; cnt < FIFO_SIZE; cnt++)
	{
	    index = FiFoChannel.receive.count;
		
    	// Not Empty
    	if (index)
    	{
    		FiFoChannel.receive.count = (--index);	
    		index = FiFoChannel.receive.read_index;
    		dat[cnt] = FiFoChannel.receive.buffer[index];    		
    		FiFoChannel.receive.read_index = (index + 1) % sizeof(FiFoChannel.receive.buffer);
    	}
    	else
    	{
    	    break;
    	}
	}
	
	return cnt;
}

// return bytes of the Rx FIFO with specified count.
IndexT FiFoReadRxBytes(u8 *dat, IndexT num)
{
    IndexT index = 0, cnt = 0;
	
	for (cnt = 0; (cnt < FIFO_SIZE) && (num != 0); cnt++, num--)
	{
	    index = FiFoChannel.receive.count;
		
    	// Not Empty
    	if (index)
    	{
    		FiFoChannel.receive.count = (--index);	
    		index = FiFoChannel.receive.read_index;
    		dat[cnt] = FiFoChannel.receive.buffer[index];    		
    		FiFoChannel.receive.read_index = (index + 1) % sizeof(FiFoChannel.receive.buffer);
    	}
    	else
    	{
    	    break;
    	}
	}
	
	return cnt;
}

IndexT FiFoGetRxCount(void)
{
    return FiFoChannel.receive.count;
}

#endif /* ENABLE_FIFO_RX */