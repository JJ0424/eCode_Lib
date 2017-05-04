/*----------------------------------------------------------------------^^-
/ File name:  flash_iap.h
/ Author:     JiangJun
/ Data:       2017/05/03
/ Version:    v0.4
/-----------------------------------------------------------------------^^-
/ bootloader common source file. 
/------------------------------------------------------------------------*/


#ifndef _FLASH_IAP_H
#define _FLASH_IAP_H

#include "main.h"
#include "sysconfig.h"


// Interrupt vector table and Flash erase operation need byte alignment.
// Must align with 1KBytes.
// DEFAULT: 20K
#ifndef SYS_FLASH_IAP_APP_ADDR
  #define FLASH_IAP_APP_ADDR		(0x08000000 + (0x400 * 20)) 
  #warning "No System Addr, use defaul: 20K for Bootloader!"
#else
  #define FLASH_IAP_APP_ADDR	    SYS_FLASH_IAP_APP_ADDR
#endif

#ifndef SYS_FLASH_IAP_ENABLE_SECURE_CODE
  #define FLASH_IAP_ENABLE_SECURE_CODE  1
#else
  #define FLASH_IAP_ENABLE_SECURE_CODE  SYS_FLASH_IAP_ENABLE_SECURE_CODE
#endif


//------------------------------------------------------------
// Chip ROM And RAM Info
//------------------------------------------------------------
typedef struct {
    
    u32 rom_size;           // unit: KBytes
    u32 sram_size;          // unit: KBytes
    u16 flash_page_size;    // unit: bytes
} FshIapCpuT;


void FlashIapInit(void);
bool FlashIapWrite(u8 *bin, u32 data_size);
void FlashIapJump(u32 Addr);
void FlashSync(void);
void FlashFmDecode(u8 *fm, u32 size);
void FlashGetCpuInfo(FshIapCpuT *cpu);

#endif
