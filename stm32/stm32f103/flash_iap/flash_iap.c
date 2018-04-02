
/*----------------------------------------------------------------------^^-
/ File name:  flash_iap.c
/ Author:     JiangJun
/ Data:       2017/05/03
/ Version:    v0.4
/-----------------------------------------------------------------------^^-
/ bootloader common source file. 
/---
/ <1> Support Low/Medium/High density Chip(expect connective line)
/---
/ 2017-01-19
/ <1> Add decode function.
/---
/ 2017-01-23
/ <1> FlashIapWrite() return the result of the write operation.
/ ---
/ 2017-05-03
/ <1> Add Write Flash Check
/ <2> Add Output CPU Info
/------------------------------------------------------------------------*/
 

#include "flash_iap.h"
#include "hconfig.h"


//------------------------------------------------------------
// DEFAULT CONFIG
//------------------------------------------------------------
#define FLASH_IAP_PAGE_SIZE_MAX	(2048)    
#define FLASH_IAP_SECURE_CODE   0x32            // Secure CODE

// STM32 CHIP INFO 
#define STM32_FALSH_INFO_ADDR                       0x1FFFF7E0
#define STM32_FLASH_LOW_DENSITY_FLASH_MAX           32              // unit: KBytes
#define STM32_FLASH_MEDIUM_DENSITY_FLASH_MAX        128
#define STM32_FLASH_HIGH_DENSITY_FLASH_MAX          1024

#define STM32_FLASH_LOW_DENSITY_SRAM_SIZE_ROM_16K       6           // 6K
#define STM32_FLASH_LOW_DENSITY_SRAM_SIZE_ROM_32K       10
#define STM32_FLASH_LOW_DENSITY_SRAM_SIZE_ROM_MEDIUM    20
#define STM32_FLASH_LOW_DENSITY_SRAM_SIZE_ROM_256K      48
#define STM32_FLASH_LOW_DENSITY_SRAM_SIZE_ROM_HIGH      64


//------------------------------------------------------------
// Function Point
//------------------------------------------------------------
typedef  void (*FshIapFunPntT)(void);


//------------------------------------------------------------
// IAP Buffer
//------------------------------------------------------------
typedef struct {

    u16 wt_sector_num;
    u32 dat_buf_cnt;
    u8 dat_buf[FLASH_IAP_PAGE_SIZE_MAX];
} FshIapBufT;


// Write File Buffer.
static FshIapBufT FshIapBuffer = {0, 0 };
static FshIapCpuT FshIapCpu = {0, 0};


 
//------------------------------------------------------------
// Reset the flash address to the beginning of the appcation.
//------------------------------------------------------------
void FlashIapInit(void)
{

    FshIapBuffer.wt_sector_num = 0; FshIapBuffer.dat_buf_cnt = 0;     
    FshIapCpu.sram_size = 0; FshIapCpu.rom_size = 0; FshIapCpu.flash_page_size = 1024;
    
    // Read Chip Info
    FshIapCpu.rom_size = *(u16*)(STM32_FALSH_INFO_ADDR);  
    
    if (FshIapCpu.rom_size <= STM32_FLASH_LOW_DENSITY_FLASH_MAX)    
    {
        // Low-density
        
        if (FshIapCpu.rom_size == 16) { FshIapCpu.sram_size = STM32_FLASH_LOW_DENSITY_SRAM_SIZE_ROM_16K; }
        else if (FshIapCpu.rom_size == 32) { FshIapCpu.sram_size = STM32_FLASH_LOW_DENSITY_SRAM_SIZE_ROM_32K; }
        else { FshIapCpu.sram_size = STM32_FLASH_LOW_DENSITY_SRAM_SIZE_ROM_32K; } 
        
        
        FshIapCpu.flash_page_size = 1024;   // FLASH Page Size
    }
    else if (FshIapCpu.rom_size <= STM32_FLASH_MEDIUM_DENSITY_FLASH_MAX)
    {
        // Medium-density
        
        FshIapCpu.sram_size = STM32_FLASH_LOW_DENSITY_SRAM_SIZE_ROM_MEDIUM;
        
        FshIapCpu.flash_page_size = 1024;   // FLASH Page Size
    }
    else
    {
        // High-density
        
        if (FshIapCpu.rom_size == 256) { FshIapCpu.sram_size = STM32_FLASH_LOW_DENSITY_SRAM_SIZE_ROM_256K; }
        else { FshIapCpu.sram_size = STM32_FLASH_LOW_DENSITY_SRAM_SIZE_ROM_HIGH; }
        
        FshIapCpu.flash_page_size = 2048;   // FLASH Page Size
    }    
}

//------------------------------------------------------------
// Flash Check
//------------------------------------------------------------
bool FlashIapCheck(u8 *fm, u8 *flash, u32 cnt)
{
    u32 idx = 0;
    
    for (idx = 0; idx < cnt; idx++)
    {
        if (fm[idx] != flash[idx]) { return FALSE; }
    }
    
    return TRUE;
}


//------------------------------------------------------------
// Write Bin to Buffer
//------------------------------------------------------------
bool FlashIapWrite(u8 *bin, u32 data_size)
{
    u32 address_offset; u8 *pu8 = NULL;
    u16 byte_offest = 0, bin_idx = 0, idx = 0;
    
    address_offset = FLASH_IAP_APP_ADDR + (FshIapBuffer.wt_sector_num  * FshIapCpu.flash_page_size);
    
    // Check if need to write data to Flash.
    if ((FshIapBuffer.dat_buf_cnt + data_size) >= FshIapCpu.flash_page_size)
    {
        // copy to the buffer with fulling.
        for (bin_idx = 0; FshIapBuffer.dat_buf_cnt < FshIapCpu.flash_page_size; FshIapBuffer.dat_buf_cnt++)   
        {
            FshIapBuffer.dat_buf[FshIapBuffer.dat_buf_cnt] = bin[bin_idx];
            bin_idx++;
        }
        
#if FLASH_IAP_ENABLE_SECURE_CODE        
        // Secure decode.
        for (idx = 0; idx < FshIapCpu.flash_page_size; idx++)
        {
            FshIapBuffer.dat_buf[idx] ^= FLASH_IAP_SECURE_CODE;
        }
#endif        
        
        // Write FLASH
        FLASH_Unlock();
        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
        
        if (FLASH_ErasePage(address_offset) != FLASH_COMPLETE) { FLASH_Lock(); return FALSE; }
        
        
        // Write this page(Erase all of this page).
    	for(byte_offest = 0; byte_offest < FshIapCpu.flash_page_size; byte_offest += 4) 
    	{
    		if (FLASH_ProgramWord(address_offset + byte_offest, *(u32*)(FshIapBuffer.dat_buf + byte_offest)) != FLASH_COMPLETE)
    		{
    		     FLASH_Lock(); return FALSE;
    		}
	    }
	
	    FLASH_Lock();
	    
	    //------------------------------------------------------------
        // Page Check
        //------------------------------------------------------------
        
        pu8 = (u8 *)address_offset;
        if (FlashIapCheck(FshIapBuffer.dat_buf, pu8, FshIapCpu.flash_page_size) == FALSE) { return FALSE; }
    
        //------------------------------------------------------------
        // Page Counter ++
        //------------------------------------------------------------
        FshIapBuffer.dat_buf_cnt = 0;
        FshIapBuffer.wt_sector_num++;
        
#if FLASH_IAP_WRITE_DEBUG
    
        RUN_LED = !RUN_LED;             // FLASH LED
#endif 

    }

    // just copy to buffer.
    for (; bin_idx < data_size; bin_idx++)   
    {
        FshIapBuffer.dat_buf[FshIapBuffer.dat_buf_cnt] = bin[bin_idx];
        FshIapBuffer.dat_buf_cnt++;
    } 
    
    return TRUE;
}

//------------------------------------------------------------
// Jump to User App
//------------------------------------------------------------
void FlashIapJump(u32 Addr)
{
    volatile FshIapFunPntT p_fun; 
    
    // Check if the value of MSP is valid
	if (((*(vu32*)Addr) >= 0x20000000) || ((*(vu32*)Addr) <= (0x20000000 | FshIapCpu.sram_size)))	
	{
	    __set_MSP((*(vu32*)Addr));
	    
		// Addr is the value of the MSP.(Addr + 4) is the reset vector.
		p_fun = (FshIapFunPntT)(*(vu32*)(Addr + 4));				
		
		// move PC to reset vector.
		p_fun();
	}
}

//------------------------------------------------------------
// Sync the Buffer to FLASH
//------------------------------------------------------------
void FlashSync(void)
{
    u32 address_offset;
    u16 byte_offest = 0;
    u16 idx = 0; u8 *pu8 = NULL;
    
    address_offset = FLASH_IAP_APP_ADDR + (FshIapBuffer.wt_sector_num * FshIapCpu.flash_page_size);
    
    if (FshIapBuffer.dat_buf_cnt != 0)
    {
    
#if FLASH_IAP_ENABLE_SECURE_CODE     

        // Secure decode.
        for (idx = 0; idx < FshIapCpu.flash_page_size; idx++)
        {
            FshIapBuffer.dat_buf[idx] ^= FLASH_IAP_SECURE_CODE;
        }
#endif         
        
        FLASH_Unlock();
        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
        FLASH_ErasePage(address_offset);
        
        // Write this page(Erase all of this page).
    	for(byte_offest = 0; byte_offest < FshIapCpu.flash_page_size; byte_offest += 2) {
    		FLASH_ProgramHalfWord(address_offset + byte_offest, *(u16*)(FshIapBuffer.dat_buf + byte_offest));
    	}
    	
    	FLASH_Lock();
    	
        //------------------------------------------------------------
        // Page Check
        //------------------------------------------------------------
        
        pu8 = (u8 *)address_offset;
        if (FlashIapCheck(FshIapBuffer.dat_buf, pu8, FshIapCpu.flash_page_size) == FALSE) { return; }
        
        //------------------------------------------------------------
        // Page Counter ++
        //------------------------------------------------------------
        FshIapBuffer.dat_buf_cnt = 0;
        FshIapBuffer.wt_sector_num++;
    }
}


//------------------------------------------------------------
// decode firmware with the input data 
//------------------------------------------------------------
void FlashFmDecode(u8 *fm, u32 size)
{
    u32 idx = 0;
    
#if FLASH_IAP_ENABLE_SECURE_CODE

    for (idx = 0; idx < size; idx++)
    {
        *fm ^= FLASH_IAP_SECURE_CODE; fm++;
    }
    
#endif    
}

//------------------------------------------------------------
// Get the ROM Info and RAM Info
//------------------------------------------------------------
void FlashGetCpuInfo(FshIapCpuT *cpu)
{
    *cpu = FshIapCpu;
}