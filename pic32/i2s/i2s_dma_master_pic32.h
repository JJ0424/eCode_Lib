
/*----------------------------------------------------------------------^^-
/ File name:  i2s_dma_master_pic32.h
/ Author:     JiangJun
/ Data:       [2020-12-29]
/ Version:    v1.40
/-----------------------------------------------------------------------^^-
/ i2s master driver
/------------------------------------------------------------------------*/


#ifndef _I2S_DMA_MASTER_PIC32_H
#define _I2S_DMA_MASTER_PIC32_H


#include "main.h"
#include "hw_conf.h"


//------------------------------------------------------------
//              CONFIG
//------------------------------------------------------------

#define _I2S_DMA_BUFFER_SIZE                (1 * 1024)              // Unit: Byte
#define _I2S_ENABLE_ISR_CALLBACK            1

// REFCLKI and REFCLKO
#define _I2S_ENABLE_REFCLK_OUT              0
#define _I2S_ENABLE_REFCLKO_512FS           0
#define _I2S_ENABLE_REFCLKI_48MHz           1                       // 48MHz as REFCLKI source

// 48MHz Osc Select
#define _I2S_48MHz_PBCLK                    1
#define _I2S_48MHz_SYSCLK                   2
#define _I2S_48MHz_POSC                     3

#define _I2S_48MHz_REFCLKI                  _I2S_48MHz_POSC         // POSC as default


//-------------------------------------------------------
//              SPI AND DMA CONFIG
//-------------------------------------------------------                  

// dual DMA macros
#define _I2S_DMA_CHN_1                      DMA_CHANNEL0

// [ 0-3, 3 is the hightest ]
#define _I2S_DMA_PRI                        DMA_CHN_PRI3

// [ 1-7, 7 is the hightest ]
#define _I2S_DMA_INT_PRI                    INT_PRIORITY_LEVEL_7

// [ 0-3, 3 is the hightest ]
#define _I2S_DMA_INT_SUB_PRI                INT_SUB_PRIORITY_LEVEL_3


//------------------------------------------------------------
//              ENUM
//------------------------------------------------------------

// Func Result
typedef enum {

    _RES_FAIL = 0,
    _RES_OK,
    _RES_REFCLKO_SETTING_ERROR,
    _RES_BAUD_SETTING_ERROR,
    
} _I2S_ResT;

// I2S Module REFCLKI Freq
typedef enum {

    _REFCLKI_22_5792MHz = 0,        // extern source-osc Hi-Res
    _REFCLKI_24_576MHz,             // extern source-osc Hi-Res    
    _REFCLKI_48MHz,                 // SYSCLK/ PBCLK/ POSC 48MHz

} _REFCLKI_FreqEnumT;

// I2S Module REFCLKO Freq
typedef enum {

    _REFCLKO_22_5792MHz = 0,        // 88.2K ---> 256Fs
    _REFCLKO_24_576MHz,             // 96K ---> 256Fs
    _REFCLKO_11_2896MHz,            // 44.1K ---> 256Fs
    _REFCLKO_12_288MHz,             // 48K ---> 256Fs
    
} _REFCLKO_FreqEnumT;

// I2S Fs
typedef enum {

    _I2S_FS_44_1K = 0,              // 44.1K
    _I2S_FS_48K,                    // 48K
    _I2S_FS_88_2K,                  // 88.2K
    _I2S_FS_96K,                    // 96K
    
} _I2S_FsEnumT;

// I2S Data Format
typedef enum {

    _I2S_FORMAT_I2S = 0,            // stardand-i2s
    _I2S_FORMAT_LEFT,               // left-justified
    _I2S_FORMAT_RIGHT,              // right-justified
    _I2S_FORMAT_PCM,                // pcm/dsp  
    
} _I2S_FormatEnumT;

// I2S Data Width
typedef enum {

    _I2S_DATA_WIDTH_REV = 0,        // 16-bit data, 32-bit BCLK [ Fs will error !! ]
    _I2S_DATA_WIDTH_16,             // 64bit BCLK
    _I2S_DATA_WIDTH_24,             // 64bit BCLK
    _I2S_DATA_WIDTH_32,             // 64bit BCLK
    
} _I2S_DataWidthEnumT;

// I2S Status
typedef enum {

    _I2S_STA_DISABLE = 0,           // all clock disable
    _I2S_STA_DATA_IDLE,             // no data in pusher
    _I2S_STA_RUN,                   // tx i2s data
    _I2S_STA_ERROR,                 // error
    
} _I2S_StatusEnumT;

// Pusher Status
typedef enum {

    _PUSHER_STA_NO_DATA = 0,        // no data in buffer
    _PUSHER_STA_WAIT_FOR_TX,        // can write data to buffer
    _PUSHER_STA_TX_NOW,             // using by DMA
    
} _PusherStatusT;

// DMA Dist Size
typedef enum {

    _DMA_DIST_SIZE_16 = 2,          // 2 - tx 16-bits audio data to SPIxBUF
    _DMA_DIST_SIZE_24 = 3,          // 3 - tx 24-bits audio data to SPIxBUF
    _DMA_DIST_SIZE_32 = 4,          // 4 - tx 32-bits audio data to SPIxBUF
    
} _DMA_DistSizeT;

// I2S Hw Status
typedef enum {

    _I2S_HW_STA_DISABLE = 0,
    _I2S_HW_STA_SR_IDLE,            // no byte in SR shift-register
    _I2S_HW_STA_BUSY,               // at least 1 byte in SPIxBUF/ SR
    
} _I2S_HwStatusT;


//------------------------------------------------------------
//              STRUCT
//------------------------------------------------------------

// DMA Pusher
typedef struct {

    u16 cnt;                                // the cnt of buffer
    u32 tx_buff[_I2S_DMA_BUFFER_SIZE / 4];  // tx buffer
    
} _I2S_DMA_PusherT;


// CONFIG
typedef struct {

    // Hw Setting    
    SpiChannel spi_id; 


    //-------------------------------------------------------
    //              Basic Setting
    //-------------------------------------------------------
    
    _REFCLKI_FreqEnumT _refclki_freq;       // refclk input select
    _REFCLKO_FreqEnumT _refclko_freq;       // refclk output select
    _I2S_FsEnumT _fs;                       // sampling rate
    _I2S_FormatEnumT _format;               // i2s format
    _I2S_DataWidthEnumT _dat_width; 

    
    //-------------------------------------------------------
    //              Other Setting
    //-------------------------------------------------------
    
    _bool _enable_mono;                     // default: stereo mode
    void (*_isr_call)(_I2S_StatusEnumT);    // isr call-back
    
} _I2S_DMA_ConT;


//------------------------------------------------------------
//              Run Var
//------------------------------------------------------------

typedef struct {


    _I2S_DMA_ConT _conf;                    // conf of i2s        

    //-------------------------------------------------------
    //              Do Not Need To Set !!!
    //-------------------------------------------------------    
    
    _I2S_StatusEnumT _i2s_status;           // I2S Status            
    u8 _pusher_idle;                        // idle pusher now
    _bool _locked;                          // pusher locker
    _I2S_DMA_PusherT _tx_pusher[2];         // dual buffer          
    _DMA_DistSizeT _dma_dst_size;           // DMA ssize

        
} _I2S_DMA_RunT;


//------------------------------------------------------------
//              External Function
//------------------------------------------------------------

extern _I2S_ResT I2S_Init(_I2S_DMA_ConT *conf);
extern u16 I2S_TxStream(void *src, u16 cnt);
extern u8 I2S_GetPusherSize(u16 *re_size);
extern _I2S_ResT I2S_SetStream(_REFCLKI_FreqEnumT, _REFCLKO_FreqEnumT, _I2S_FsEnumT, _I2S_DataWidthEnumT);
extern _I2S_ResT I2S_SetAudio(u32 fs, u8 nbit);
extern _I2S_ResT I2S_GetREFCLK(_I2S_FsEnumT i2sfs, _REFCLKI_FreqEnumT *refi, _REFCLKO_FreqEnumT *refo);
extern _I2S_StatusEnumT I2S_GetTxStatus(void);
extern _I2S_HwStatusT I2S_GetHwStatus(void);
extern void I2S_ResetTx(void);
extern _I2S_ResT I2S_SetMonoMode(_bool en);

#endif
//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
