
/*----------------------------------------------------------------------^^-
/ File name:  disk_spi.c
/ Author:     JiangJun
/ Data:       2018/6/22
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ COMMON SD/MMC SPI Driver
/------------------------------------------------------------------------*/

#include "diskio.h"
#include "integer.h"

//-------------------------------------------------------------
// card type variable.
// ---
// Driver lib can support either SD card V1 or V2(protocal),
// and SDHC also support.
//------------------------------------------------------------- 
static BYTE CardType = 0;

#define CT_MMC		0x01			// MMC ver 3
#define CT_SD1		0x02			// SD ver 1
#define CT_SD2		0x04			// SD ver 2
#define CT_SDC		(CT_SD1|CT_SD2)	// SD-inclue SDV1 and SDV2
#define CT_BLOCK	0x08			// Block addressing(>=2GB) 

//-------------------------------------------------------------
// Disk State variable.
// ---
// default is no card and no init,
// must clear in the disk_initialize function
// if disk_initialize successful
//------------------------------------------------------------- 
static DSTATUS DiskState = STA_NOINIT;

//------------------------------------------------------------
// Port Function
//------------------------------------------------------------

static fpSpiCsCtrl FpSpiCsCtrl = (void*)0; static fpSpiExByte FpSpiExByte = (void*)0;
static fpSpiTxBytes FpSpiTxBytes = (void*)0; static fpSpiRxBytes FpSpiRxBytes = (void*)0;
static fpSpiSpeedMax FpSpiSpeedMax = (void*)0;

//-------------------------------------------------------------
// SD card Command, enum type.
// This is not exactly SD card command.
//-------------------------------------------------------------
typedef enum {
	CMD0 = 0,			// Go ldle mode. 
	CMD1 = 1,			// Initiate initialization process(MMC).   
	CMD8 = 8,			// check voltage(SDV2).
	CMD9 = 9,			// read CSD register.
	CMD10 = 10, 		// read CID register.
	CMD12 = 12,			// stop to read data.
	CMD16 = 16,			// set R/W block size.
	CMD17 = 17,			// read single block.
	CMD18 = 18, 		// read multi blocks.
	CMD23 = 23, 		// Define number of blocks to transfer(MMC).
	CMD24 = 24,			// write single block.
	CMD25 = 25,			// write multi blocks.
	CMD55 = 55,			// leading command of ACMD<n> command.b
	CMD58 = 58,			// read OCR register.
	ACMD23 = 151,		// Define number of blocks to pre-erase(SDC).
	ACMD41 = 169,		// For only SDC. Initiate initialization process.
} CMD_Index;

//--------------------------------------------------------------
// command response. enum type.
//--------------------------------------------------------------
typedef enum {
	IDLE_MODE = 0x01,
	RESPONSE_NORMAL = 0x00,
	NO_RESPONSE = 0xFF,
} CMD_Response;


/*------------------------------------------------------------
 *	Function Names:		WaitReady()
 *	Parameter:			None
 *	Return Value:		None
 *	Remarks:			
 *------------------------------------------------------------*/
static DRESULT WaitReady(void)
{
	BYTE data = 0; volatile WORD overtimr = 0;

    // Overtime
	overtimr = 0xFFFF;	
	
	do 
		data = (*FpSpiExByte)(0xFF);
	while ((data != 0xFF) && overtimr--);

	if (data == 0xFF)
		return RES_OK;
	else
		return RES_NOTRDY;
}

/*------------------------------------------------------------
 *	Function Names:		SecletCard()
 *	Parameter:			None
 *	Return Value:		None
 *	Remarks:			
 *------------------------------------------------------------*/
static DRESULT SecletCard(void)
{
    /* CS low to seclet the card */
	(*FpSpiCsCtrl)(0);
	
	(*FpSpiExByte)(0xFF);

	if (WaitReady() == RES_OK)
		return RES_OK;
	else
		return RES_NOTRDY;
}

/*------------------------------------------------------------
 *	Function Names:		DelectCard()
 *	Parameter:			None
 *	Return Value:		None
 *	Remarks:			
 *------------------------------------------------------------*/
static DRESULT DeSelectCard(void)
{
	/* CS high to deseclet the card */
	(*FpSpiCsCtrl)(1);
	
	(*FpSpiExByte)(0xFF);
	
	return RES_OK;
}

/*------------------------------------------------------------
 *	Function Names:		SendCommand()
 *	Parameter:			cmd_index:
 *						argument:
 *	Return Value:		None
 *	Remarks:			
 *------------------------------------------------------------*/
static CMD_Response SendCommand(CMD_Index cmd_index, UINT argument)
{
	BYTE data = 0, result = 0;

	// ---------- computer CMD ----------------------------------
	data = (BYTE)cmd_index;

	/* ACMD command */
	if (data & 0x80)
	{
		data &= 0x7F;
		result = SendCommand(CMD55, 0);
		if (result > IDLE_MODE)
			return (CMD_Response)result;
	}
	data |= 0x40;
	
	/* seclet card and wait for ready */
	DeSelectCard();
	if (SecletCard() == RES_NOTRDY)
		return NO_RESPONSE;

	/* send command package */

	(*FpSpiExByte)(data);					// CMD index.
	// parameter. 32 bits.
	(*FpSpiExByte)((BYTE)(argument >> 24));	
	(*FpSpiExByte)((BYTE)(argument >> 16));
	(*FpSpiExByte)((BYTE)(argument >> 8));
	(*FpSpiExByte)((BYTE)argument);
	
	// CRC computer and send.
	data = 0x01;						/* Stop bit */
	if (cmd_index == CMD0) data = 0x95;	/* Valid CRC for CMD0(0) + Stop */
	if (cmd_index == CMD8) data = 0x87;	/* Valid CRC for CMD8(0x1AA) Stop */
	
	(*FpSpiExByte)(data);
	
	/* Receive command response */
	if (cmd_index == CMD12) (*FpSpiExByte)(0xFF);		
	data = 10;	
	// Wait for a valid response in timeout of 10 attempts.
	do
		result = (*FpSpiExByte)(0xFF);
	while ((result & 0x80) && (--data));

	
	/* Return with the response value */
	return (CMD_Response)result;			
}

/*------------------------------------------------------------
 *	Function Names:		SoftwareReset()
 *	Parameter:			None
 *	Return Value:		DRESULT
 *	Remarks:			
 *------------------------------------------------------------*/
static DRESULT SoftwareReset(void)
{
	BYTE index = 0;

    /* CS must be high level */
    (*FpSpiCsCtrl)(1);

	/* send >= 74 dummy SCLK clock MOSI(DI) AND CS must be high */
	for(index = 80; index != 0; index--)
		(*FpSpiExByte)(0xFF);

	/* send CMD0 and receive response if error close spi and return */
	if (SendCommand(CMD0, 0) == IDLE_MODE)
		return RES_OK;
	else
		return RES_NOTRDY;
}

/*------------------------------------------------------------
 *	Function Names:		SdcV2Init()
 *	Parameter:			None
 *	Return Value:		card type. error return 0.
 *	Remarks:			
 *------------------------------------------------------------*/
static BYTE SdcV2Init(void)
{
	BYTE index = 0;
	BYTE response[4] = {0, 0, 0, 0}; volatile WORD overtimr = 0;
	
	overtimr = 0xFFFF;
	
	for (index = 0; index < 4; index++)
		response[index] = (*FpSpiExByte)(0xFF);
	if ((response[2] == 0x01) && (response[3] == 0xAA))
	{
		// wait for init end.
		while ((--overtimr) && SendCommand(ACMD41, 0X40000000));
		// cheak CCS in OCR register.
		if (overtimr && (SendCommand(CMD58, 0) == RESPONSE_NORMAL))
		{
			for (index = 0; index < 4; index++)
				response[index] = (*FpSpiExByte)(0xFF);
			/* card type */
			index = 0;
			if (response[0] & 0x40)
			{
				index = CT_SD2 | CT_BLOCK;
			}
			else
			{
				index = CT_SD2;
				
				/* Set R/W block length to 512 */
				if (SendCommand(CMD16, 512) != RESPONSE_NORMAL)	
					index = 0;
			}
		}
		else
		{
            index = 0;
		}
	}

	return index;
}

/*------------------------------------------------------------
 *	Function Names:		SdcV1MmcInit()
 *	Parameter:			None
 *	Return Value:		card type. error return 0.
 *	Remarks:			
 *------------------------------------------------------------*/
static BYTE SdcV1MmcInit(void)
{
	BYTE type = 0, cmd = 0; volatile WORD overtimr = 0;

	overtimr = 0xFFFF;
	
	// SDC V1 or MMC V3
	if (SendCommand(ACMD41, 0) <= IDLE_MODE) 
	{
		type = CT_SD1; 
		cmd = ACMD41;	/* SDv1 */
	}
	else 
	{
		type = CT_MMC; 
		cmd = CMD1;		/* MMCv3 */
	}
	
	/* Wait for leaving idle state */		
	while ((--overtimr) && SendCommand((CMD_Index)cmd, 0));	
	
	/* Set R/W block length to 512 */
	if (overtimr && (SendCommand(CMD16, 512) == RESPONSE_NORMAL))	
	{
        return type;
	}			
			
	return 0;
}

/*------------------------------------------------------------
 *	Function Names:		ReceiveDataPacket()
 *	Parameter:			buffer:
 *						the point to buffer.
 *						byte_count:
 *						buffer size.
 *	Return Value:		None
 *	Remarks:			
 *------------------------------------------------------------*/
static DRESULT ReceiveDataPacket(BYTE *buffer, WORD byte_count)
{
	BYTE token = 0; volatile WORD overtimr = 0;

	overtimr = 0xFFFF;
	
	do
	{
		token = (*FpSpiExByte)(0xFF);
	}
	while ((token == NO_RESPONSE) && overtimr--);
	
	if (token != 0xFE)
		return RES_NOTRDY;
	(*FpSpiRxBytes)(buffer, byte_count);
	/* CRC */
	(*FpSpiExByte)(0xFF);
	(*FpSpiExByte)(0xFF);
	
	return RES_OK;
}

/*------------------------------------------------------------
 *	Function Names:		SendDataPacket()
 *	Parameter:			buffer:
 *						the point to buffer.
 *						byte_count:
 *						buffer size.
 *	Return Value:		None
 *	Remarks:			the function must send 512 bytes.
 *------------------------------------------------------------*/
static DRESULT SendDataPacket(const BYTE *buffer, BYTE token)
{
	BYTE response = 0;

	if (WaitReady() != RES_OK)
		return RES_NOTRDY;

	/* send data packet */
	(*FpSpiExByte)(token);					// data token.
	if (token != 0xFD)
	{
		(*FpSpiTxBytes)(buffer, 512);		// 512 byte write.
		(*FpSpiExByte)(0xFF);				// 2bytes CRC.
		(*FpSpiExByte)(0xFF);
		
		response = (*FpSpiExByte)(0xFF);
		
		/* If not accepted, return with error */
		if ((response & 0x1F) != 0x05)		
			return RES_NOTRDY;
	}
	
	return RES_OK;
}

/*------------------------------------------------------------
 *	Function Names:		ReadSectorCnt()
 *	Parameter:			blk_cnt - Number of blocks.
 *	Return Value:		operation result.
 *						this is a enum constant.
 *	Remarks:			The capcity of the SD card is blk_cnt
 *                      * 512Byte. The unit is byte.
 *------------------------------------------------------------*/
static DRESULT ReadSectorCnt(UINT *blk_cnt)
{
    BYTE csd_buf[16];
    BYTE idx = 0, c_size_mult = 0, rd_bl_len = 0;
    UINT c_size = 0;

    // Initialize The CSD Buffer.
    for (idx = 0; idx < 16; idx++) {
        csd_buf[idx] = 0;
    }

    // Return If SD is Not Ready.
    if (DiskState & STA_NODISK) {
        return RES_NOTRDY;
    }
    
    // Send CMD9(SEND CSD) command, if successful, the CMD line
    // will receive R1 response 0(RESPONSE_NORMAL).
    if (SendCommand(CMD9, 0) == RESPONSE_NORMAL) 
    {
        // Receive 16 bytes CSD Register Contents.
        ReceiveDataPacket(csd_buf, 16);
    }
    else 
    {
        DeSelectCard();
        return RES_NOTRDY;
    }

    if (csd_buf[0] & 0x40)
    {
        // CSD Register V2.0.
        // Computer C_SIZE.
        c_size = csd_buf[9] + ((UINT)csd_buf[8] << 8) + ((UINT)(csd_buf[7] & 0x3F) << 16);
        *blk_cnt = (c_size + 1) * 1024;
    }
    else
    {
        // CSD Register V1.0
        // Computer RD_BL_LEN.
        rd_bl_len = (csd_buf[5] & 0x0F);
        
        // Computer C_SIZE.
        c_size = (csd_buf[8] >> 6) + ((WORD)csd_buf[7] << 2) + ((WORD)(csd_buf[6] & 0x03) << 10);

        // Computer C_SIZE_MULT.
        c_size_mult = ((csd_buf[10] & 0x80) >> 7) + ((csd_buf[9] & 0x03) << 1);

        *blk_cnt = (c_size + 1) * (1 << (c_size_mult + 2)) * (1 << rd_bl_len) / 512;
    }
    
    DeSelectCard();
    return RES_OK;
}

/*------------------------------------------------------------
 *	Function Names:		disk_hwinit()
 *	Parameter:			None
 *	Return Value:		None
 *	Remarks:			
 *------------------------------------------------------------*/
void disk_hwinit(fpSpiCsCtrl fp_cs, fpSpiExByte fp_ex, fpSpiTxBytes fp_tx, fpSpiRxBytes fp_rx, fpSpiSpeedMax fp_spd)
{

    FpSpiCsCtrl = fp_cs; FpSpiExByte = fp_ex; FpSpiTxBytes = fp_tx; FpSpiRxBytes = fp_rx;
    FpSpiSpeedMax = fp_spd;
}

/*------------------------------------------------------------
 *	Function Names:		disk_initialize()
 *	Parameter:			None
 *	Return Value:		operation result.
 *						this is a enum constant.
 *	Remarks:			
 *------------------------------------------------------------*/
DSTATUS disk_initialize(BYTE drv)
{
	BYTE sd_type = 0;

	if (drv) 
		return STA_NOINIT;	
		
	/* start initizlize */
	if (DiskState & STA_NODISK) 			// no disk.
		return DiskState;	

	// Max: 400KHz
	(*FpSpiSpeedMax)(400000);
	
	if (SoftwareReset() == RES_OK)
	{
		if (SendCommand(CMD8, 0X1AA) == IDLE_MODE)
			sd_type = SdcV2Init();
		else
			sd_type = SdcV1MmcInit();
	}

	/* when enter there, the card type and Protocol is ensure 
	   save card type. this version not support SDV3        */
	CardType = sd_type;
	DeSelectCard();

	/* Remove card physical state storage variables --- DiskState */
	if (sd_type)
	{
		DiskState &= (~STA_NOINIT);
		DiskState &= (~STA_NODISK);
		
		// 9MHz Max
		(*FpSpiSpeedMax)(9000000);
		return DiskState;
	}
	else
	{
		return DiskState;
	}	
}

/*------------------------------------------------------------
 *	Function Names:		disk_initialize()
 *	Parameter:			drv:
 *						Physical drive nmuber (0)
 *	Return Value:		the disk state.
 *	Remarks:			
 *------------------------------------------------------------*/
DSTATUS disk_status (BYTE drv)
{
	/* Supports only single drive */
	if (drv) 
		return STA_NOINIT | STA_NODISK;		
	else
		return DiskState;
}

/*------------------------------------------------------------
 *	Function Names:		disk_read()
 *	Parameter:			drv:
 *						driver number.
 *
 *      				sector:
 *						the logic sector address, this parameter
 *						will be convert to physical block address
 *						SDV1 is sector*512, SDV2 is sector itself.
 *
 *						setor_count:
 *						suppotr multi block read. 1 ~ 255.
 *
 *						buffer:
 *						the point to a receive buffer.
 *
 *	Return Value:		operation result.
 *						this is a enum constant.
 *	Remarks:			
 *------------------------------------------------------------*/
DRESULT disk_read(BYTE drv, BYTE *buffer, DWORD sector, UINT sector_count)
{
	if (drv || (!sector_count)) return RES_PARERR;

	if (DiskState & STA_NOINIT) return RES_NOTRDY;

	//------------------------------------------------------------
	/* computer block address */
	if (!(CardType & CT_BLOCK)) sector *= 512;

	/* single or multiple read */
	if (sector_count == 1)				// single 
	{
		if (SendCommand(CMD17, sector) == RESPONSE_NORMAL)
			if (ReceiveDataPacket(buffer, 512) == RES_OK)
				sector_count = 0;
	}
	else								// multiple.
	{
		if (SendCommand(CMD18, sector) == RESPONSE_NORMAL)
		{
			while (sector_count)
			{
				if (ReceiveDataPacket(buffer, 512) != RES_OK)
					break;
					
				buffer += 512;
				sector_count--;
			}
			SendCommand(CMD12, 0);		// stop read.
		}
	}

	DeSelectCard();

	return sector_count? RES_ERROR : RES_OK;
}

/*------------------------------------------------------------
 *	Function Names:		disk_write()
 *	Parameter:			drv:
 *						driver number.
 *
 *						sector:
 *						the logic sector address, this parameter
 *						will be convert to physical block address
 *						SDV1 is sector*512, SDV2 is sector itself.
 *
 *						setor_count:
 *						suppotr multi block read. 1 ~ 255.
 *
 *						buffer:
 *						the point to a write buffer.
 *
 *	Return Value:		operation result.
 *						this is a enum constant.
 *	Remarks:			
 *------------------------------------------------------------*/
DRESULT disk_write(BYTE drv, const BYTE *buffer, DWORD sector, UINT sector_count)
{
	if (drv || (!sector_count)) return RES_PARERR;
	
	if (DiskState & STA_NOINIT) return RES_NOTRDY;
	
	if (DiskState & STA_PROTECT) return RES_WRPRT;

	//------------------------------------------------------------
	/* computer block address */
	if (!(CardType & CT_BLOCK)) sector *= 512;

	/* single or multiple read */
	if (sector_count == 1)				// single 
	{
		if (SendCommand(CMD24, sector) == RESPONSE_NORMAL)
		{
			if (SendDataPacket(buffer, 0xFE) == RES_OK)
				sector_count = 0;
		}
	}
	else								// multiple.
	{
		if (CardType & CT_SDC)
			SendCommand(ACMD23, sector_count);
		if (SendCommand(CMD25, sector) == RESPONSE_NORMAL)
		{
			while (sector_count)
			{
				if (SendDataPacket(buffer, 0xFC) != RES_OK)
					break;
					
				buffer += 512;
				sector_count--;
			}
			if (SendDataPacket(buffer, 0xFD) != RES_OK)	// stop token.
				sector_count = 1;
		}
	}

	DeSelectCard();

	return sector_count? RES_ERROR : RES_OK;
}

/*------------------------------------------------------------
 *	Function Names:		disk_ioctl()
 *	Parameter:			drv:
 *						Physical drive nmuber (0)
 *
 *	Return Value:		operation result.
 *						this is a enum constant.
 *	Remarks:			
 *------------------------------------------------------------*/
DRESULT disk_ioctl(BYTE drv, BYTE ctrl, void *buff)
{
	DRESULT res;
	UINT cnt = 0;

	if (drv) return RES_PARERR;

	if (DiskState & STA_NOINIT) 
		return RES_NOTRDY;

	res = RES_ERROR;
	switch (ctrl) 
	{
	case CTRL_SYNC:
		if (SecletCard() == RES_OK) 
		{
			DeSelectCard();
			res = RES_OK;
		}
		break;
		
    case GET_SECTOR_COUNT:
        if (ReadSectorCnt(&cnt) == RES_OK) 
        {
            (*(UINT *)buff) = cnt;
            res = RES_OK;
        }
        break;
        
	default:
		res = RES_PARERR;
		break;
	}

	DeSelectCard();

	return res;
}

/*------------------------------------------------------------
 *	Function Names:		disk_timerproc()
 *	Parameter:			None
 *	Return Value:		None
 *	Remarks:			This function must be called 
 *						in period of 10ms.
 *						this is a task function,
 *						the card insert will be find.
 *------------------------------------------------------------*/
void disk_timerproc(void)
{
	
}

/*------------------------------------------------------------
 *	Function Names:		get_fattime()
 *	Parameter:			None
 *	Return Value:		None
 *	Remarks:			No RTC funciton.
 *------------------------------------------------------------*/
DWORD get_fattime(void)
{
	return 0;
}


//--------------------------------------------------
//---------------- End of file ---------------------
//--------------------------------------------------