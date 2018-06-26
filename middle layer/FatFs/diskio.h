
/*----------------------------------------------------------------------^^-
/ File name:  diskio.h
/ Author:     JiangJun
/ Data:       2018/6/22
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ COMMON SD/MMC SPI Driver
/------------------------------------------------------------------------*/

#ifndef _DISKIO_H
#define _DISKIO_H

#include "integer.h"

#ifdef __cplusplus
    extern "C" {
#endif

#define _USE_WRITE	1	/* 1: Enable disk_write function */
#define _USE_IOCTL	1	/* 1: Enable disk_ioctl fucntion */

#define _READONLY	0	/* 1: Read-only mode */


/* Status of Disk Functions, this mean physical sd state */
typedef BYTE DSTATUS;
#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */

/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		// 0: Successful
	RES_ERROR,		// 1: R/W Error
	RES_WRPRT,		// 2: Write Protected
	RES_NOTRDY,		// 3: Not Ready 
	RES_PARERR		// 4: Invalid Parameter
} DRESULT;


/* Command code for disk_ioctrl() */
#define GET_SECTOR_COUNT	1
#define GET_SECTOR_SIZE		2
#define CTRL_SYNC			3
#define CTRL_POWER_OFF		4
#define CTRL_LOCK			5
#define CTRL_EJECT			6
#define MMC_GET_CSD			10
#define MMC_GET_CID			11
#define MMC_GET_OCR			12
#define ATA_GET_REV			20
#define ATA_GET_MODEL		21
#define ATA_GET_SN			22

//------------------------------------------------------------
// Port Functions(extern support)
//------------------------------------------------------------

// CS Pin Control (1 or 0)
typedef void (*fpSpiCsCtrl)(BYTE level);

// Exchange Byte
typedef BYTE (*fpSpiExByte)(BYTE dat);

// Tx Bytes to SPI
typedef void (*fpSpiTxBytes)(const BYTE *src, UINT len);

// Rx Bytes From SPI
typedef void (*fpSpiRxBytes)(BYTE *dst, UINT len);

// Set SPI Speed
typedef void (*fpSpiSpeedMax)(UINT sed);


/*
 * Prototypes for disk control functions 
 */
extern DSTATUS disk_initialize (BYTE);
extern DSTATUS disk_status (BYTE);
extern DRESULT disk_read (BYTE, BYTE*, DWORD, UINT);

/* read only choose */
#if	_READONLY == 0
extern DRESULT disk_write (BYTE,const BYTE*, DWORD, UINT);
#endif

extern DRESULT disk_ioctl (BYTE, BYTE, void*);
extern void disk_timerproc (void);
extern DWORD get_fattime(void);

extern void disk_hwinit(fpSpiCsCtrl fp_cs, fpSpiExByte fp_ex, fpSpiTxBytes fp_tx, fpSpiRxBytes fp_rx, fpSpiSpeedMax fp_spd);

#ifdef __cplusplus
    }
#endif

#endif


//--------------------------------------------------
//---------------- End of file ---------------------
//--------------------------------------------------