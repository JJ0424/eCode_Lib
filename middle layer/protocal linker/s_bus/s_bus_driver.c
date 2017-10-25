
/*------------------------------------------------------^-^/
/ File name:  s_bus_driver.c
/ Author:     JiangJun
/ Data:       2017/10/23
/ Version:    V0.0
/-------------------------------------------------------^-^/
/ S-BUS Driver
/---------------------------------------------------------*/


#include "s_bus_driver.h"

#include "s_bus.h"
#include "strlib.h"
#include "string.h"

//------------------------------------------------------------
// Port Function
//------------------------------------------------------------

static fpSpiTxBytes FpSpiTxBytes = NULL; static fpSpiRxBytes FpSpiRxBytes = NULL;
static fpSpiExByte FpSpiExByte = NULL; static fpSpiCs FpSpiCs;


//------------------------------------------------------------
// Timer
//------------------------------------------------------------

volatile u16 SBusTimer = 0, SBusSubTimer = 0;



/*----------------------------------------------------------------------
 *  SBusDriverInit
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void SBusDriverInit(fpSpiTxBytes fp_tx, fpSpiRxBytes fp_rx, fpSpiExByte fp_ex_byte, fpSpiCs cs)
{

    FpSpiTxBytes = fp_tx;
    FpSpiRxBytes = fp_rx;
    FpSpiExByte = fp_ex_byte;
    FpSpiCs = cs;
    
}



/*----------------------------------------------------------------------
 *  wait_for_ack - return the "#sbus"
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
static Bool wait_for_ack(void)
{    

    u8 tmp[_S_BUS_STX_LEN];

    // check functions
    if (FpSpiExByte == NULL) { return FALSE; }

    if (FpSpiRxBytes == NULL) { return FALSE; }


    // timer start
    SBusTimer = (S_BUS_DRIVER_WAIT_FOR_ACK_MAX / S_BUS_DRIVER_TIME_PROC);

    //------------------------------------------------------------
    // Wait for first "Vaild" byte
    //------------------------------------------------------------
    for (; SBusTimer; )
    {
        tmp[0] = FpSpiExByte(0xFF);

        if (tmp[0] == _S_BUS_STX[0])                    // '#' pause
        {
            break;
        }

        // 10ms delay
        SBusSubTimer = (10 / S_BUS_DRIVER_TIME_PROC);

        for (; SBusSubTimer; );
    }

    if (!SBusTimer) { return FALSE; }


    //------------------------------------------------------------
    // Wait for "sbus" 
    //------------------------------------------------------------
    for (; SBusTimer; )
    {
        tmp[1] = FpSpiExByte(0xFF);

        if (tmp[1] == _S_BUS_STX[1])                    // 's' pause
        {
            // "bus" pause
            FpSpiRxBytes(&tmp[2], _S_BUS_STX_LEN - 2);
            
            // comp header
            if (_strstr(tmp, _S_BUS_STX_LEN, _S_BUS_STX) == 0) { return TRUE; }
            else { return FALSE; }
        }

        // 10ms delay
        SBusSubTimer = (10 / S_BUS_DRIVER_TIME_PROC);

        for (; SBusSubTimer; );
    }

    return FALSE;
}


/*----------------------------------------------------------------------
 *  SBusDriverTimeProc(100Hz Decrement Timer) - 10ms called
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void SBusDriverTimeProc(void)
{
    u16 tmp;

    tmp = SBusTimer;
    if (tmp) { SBusTimer = (--tmp); }    

    tmp = SBusSubTimer;
    if (tmp) { SBusSubTimer = (--tmp); }    
}

/*----------------------------------------------------------------------
 *  SBusDrSendDataWaitAck
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
Bool SBusDrSendDataWaitAck(u8 *input, u16 cmd_len, u8 *output, u16 ack_len)
{

    if (FpSpiTxBytes == NULL) { return FALSE; }

    if (FpSpiRxBytes == NULL) { return FALSE; }

    if (FpSpiCs == NULL) { return FALSE; }

    // CS ON
    FpSpiCs(0);
    
    // Send Command or Data
    FpSpiTxBytes(input, cmd_len);


    // Wait for ack
    if (wait_for_ack() == FALSE) { return FALSE; }


    // Read least bytes
    memcpy(output, _S_BUS_STX, _S_BUS_STX_LEN);                         // "#sbus"
    FpSpiRxBytes(output + _S_BUS_STX_LEN, ack_len - _S_BUS_STX_LEN);    // Other ACK bytes  

    // CS OFF
    FpSpiCs(1);

    return TRUE;
}

/*----------------------------------------------------------------------
 *  SBusDriverWaitAck
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
Bool SBusDriverWaitAck(u8 *output, u16 ack_len)
{   

    if (FpSpiRxBytes == NULL) { return FALSE; }    


    // Wait for ack
    if (wait_for_ack() == FALSE) { return FALSE; }

    // Read least bytes
    memcpy(output, _S_BUS_STX, _S_BUS_STX_LEN);                         // "#sbus"
    FpSpiRxBytes(output + _S_BUS_STX_LEN, ack_len - _S_BUS_STX_LEN);    // Other ACK bytes  


    return TRUE;
}

