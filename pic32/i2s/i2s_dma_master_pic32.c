
/*----------------------------------------------------------------------^^-
/ File name:  i2s_dma_master_pic32.c
/ Author:     JiangJun
/ Data:       2019/8/14
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ i2s master driver
/ ---
/ 1. can only support one I2S driver
/------------------------------------------------------------------------*/


#include "i2s_dma_master_pic32.h"
#include "hw_conf.h"


//------------------------------------------------------------
//              STATIC VAR
//------------------------------------------------------------

static _I2S_DMA_RunT _I2S_DMA_Run;


//------------------------------------------------------------
//              I2S FLAG AND IRQ
//------------------------------------------------------------

typedef struct {

    u8 tx_irq;          // IQR source
    void *tx_reg;       // SPIxBUF
    
} _I2S_FLAG;

// Table
const _I2S_FLAG _I2S_FLAG_Table[2] = 
{
    
    { _SPI1_TX_IRQ, (void *)&SPI1BUF },         // SPI1    
    { _SPI2_TX_IRQ, (void *)&SPI2BUF },         // SPI2
};


//------------------------------------------------------------
//              Frequency Check
//------------------------------------------------------------
#if _PIC32_PBCLK != _PIC32_SYSCLK

  #error "The PBCLK is not equal to SYSCLK! [i2s_dma_master_pic32.c]"
#endif


//------------------------------------------------------------
//              SYSCLK/ PBCLK as the REFCLKI
//------------------------------------------------------------

// Enable PBCLK as REFCLKI
#if _I2S_PLLCLK_PBCLK 

  #define _I2S_PLLCLK_AS_SOURCE             OSC_REFOCON_PBCLK      
#else

  #define _I2S_PLLCLK_AS_SOURCE             OSC_REFOCON_SYSCLK       
#endif


//------------------------------------------------------------
//              Extern REFCLK as REFCLKI
//------------------------------------------------------------

#define _I2S_REFCLK_AS_SOURCE               (8 << _REFOCON_ROSEL_POSITION)      // PLIB BUG!


//------------------------------------------------------------
//              CONFIG TABLE
//------------------------------------------------------------

// format config con
const u32 _i2s_format_con_TABLE[4] = 
{ 

    // I2S mode
    SPI_CONFIG_MSTEN | SPI_CONFIG_ENHBUF | SPI_OPEN_MCLKSEL | SPI_CONFIG_TBE_NOT_FULL | SPI_CONFIG_CKP_HIGH,

    // LJ mode
    SPI_CONFIG_MSTEN | SPI_CONFIG_ENHBUF | SPI_OPEN_MCLKSEL | SPI_CONFIG_TBE_NOT_FULL | SPI_CONFIG_FSP_HIGH,
    
    // RJ mode
    SPI_CONFIG_MSTEN | SPI_CONFIG_ENHBUF | SPI_OPEN_MCLKSEL | SPI_CONFIG_TBE_NOT_FULL | SPI_CONFIG_FSP_HIGH,
    
    // PCM mode
    SPI_CONFIG_MSTEN | SPI_CONFIG_ENHBUF | SPI_OPEN_MCLKSEL | SPI_CONFIG_TBE_NOT_FULL,
};

// format config con2
const u32 _i2s_format_con2_TABLE[4] = 
{

    // I2S mode
    SPI_CONFIG2_AUDEN | SPI_CONFIG2_IGNTUR | SPI_CONFIG2_AUDMOD_I2S,

    // LJ mode
    SPI_CONFIG2_AUDEN | SPI_CONFIG2_IGNTUR | SPI_CONFIG2_AUDMOD_LJ,
    
    // RJ mode
    SPI_CONFIG2_AUDEN | SPI_CONFIG2_IGNTUR | SPI_CONFIG2_AUDMOD_RJ,

    // PCM mode
    SPI_CONFIG2_AUDEN | SPI_CONFIG2_IGNTUR | SPI_CONFIG2_AUDMOD_DSP,
};

// data width config
const u32 _i2s_dat_width_conf_TABLE[4] =
{

    0x00,                                               // 16-bit 32 frame
    SPI_CONFIG_MODE16,                                  // 16-bit 64 frame
    SPI_CONFIG_MODE32 | SPI_CONFIG_MODE16,              // 24-bit 64 frame
    SPI_CONFIG_MODE32,                                  // 32-bit 64 frame
};


//------------------------------------------------------------
// N and M Table
// ---
// REFCLK O = REFCLK I / 2/ (N + M/512) [ IF N = 0, REFCLKO = REFCLKI! ]
// ---
// N. M
// ---              REFCLKO (MHz)
// REFCLKI (MHz)    22.5792MHz      24.576      11.2896     12.288
// ---
// 22.5792          0, 0            NC          1, 0        NC
// 24.576           NC              0, 0        NC          1, 0
// 48               1, 32           NC          2, 64       1, 488
// 72               1, 304          1, 238      3, 97       2, 476
// ------
// SYSCLK ---> REFCLKO Error!!!
// ---
// [1] 48MHz SYSCLK
// 22.5792MHz, N = 1, M = 32    (22.588235MHz)
// 24.576MHz ( CAN NOT USED!! )
// 11.2896MHz, N = 2, M = 64    (11.294118MHz)
// 12.288MHz,  N = 1, M = 488   (0%)
// ---
// [2] 72MHz SYSCLK
// 22.5792MHz, N = 1, M = 304 (22.588235MHz)
// 24.576MHz, N = 1, M = 238 (0%)
// 11.2896MHz, N = 3, M = 97    (11.287201MHz)
// 12.288MHz,  N = 2, M = 476   (0%)
//------------------------------------------------------------

#if _PIC32_SYSCLK == 48000000L                              // SYSCLK = 48MHz

  // N - 0xFF means 'NC'
  const u8 _refclk_N_TABLE[3][4] = 
  {
  
      { 0,          0xFF,       1,          0xFF    },      
      { 0xFF,       0,          0xFF,       1       },
      { 1,          0xFF,       2,          1       },
  };

  // M - 0xFFFF means 'NC'
  const u16 _refclk_M_TABLE[3][4] = 
  {

      { 0,          0xFFFF,     0,          0xFFFF  },
      { 0xFFFF,     0,          0xFFFF,     0       },
      { 32,         0xFFFF,     64,         488     },
  };  
  
#elif _PIC32_SYSCLK == 72000000L                            // SYSCLK = 72MHz [ NOT TEST! ]

  // N - 0xFF means 'NC'
  const u8 _refclk_N_TABLE[3][4] = 
  {
  
      { 0,          0xFF,       1,          0xFF    },      // 0xFFFF means 'NC'
      { 0xFF,       0,          0xFF,       1       },
      { 1,          1,          3,          2       },
  };

  // M - 0xFFFF means 'NC'
  const u16 _refclk_M_TABLE[3][4] = 
  {

      { 0,          0xFFFF,     0,          0xFFFF  },
      { 0xFFFF,     0,          0xFFFF,     0       },
      { 304,        238,        97,         476     },
  };  
  
#else 

  #error "SYSCLK is error! [i2s_dma_master_pic32.c]"        // SYSCLK error!
#endif /* _PIC32_SYSCLK */


//------------------------------------------------------------
// BRG Table - 64 Frame Mode
// ---
// BCLK = REFCLKO / 2/ (BRG + 1) = 64Fs
// ---
// ---              Fs (KHz)
// REFCLKO (MHz)    44.1        48          88.2        96
// ---
// 22.5792          3           NC          1           NC
// 24.576           NC          3           NC          1
// 11.2896          1           NC          0           NC
// 12.288           NC          1           NC          0
//------------------------------------------------------------

const u8 _Fs_BRG_Table[4][4] = 
{

    {   3,          0xFF,       1,          0xFF    },    // 0xFF means 'NC'
    {   0xFF,       3,          0xFF,       1       },
    {   1,          0xFF,       0,          0xFF    },
    {   0xFF,       1,          0xFF,       0       },
};


//------------------------------------------------------------
// DMA Distnation Table
// ---
// Audio 16bits - 2
// Audio 24bits - 4
// Audio 32bits - 4
//------------------------------------------------------------

const u8 _Dist_Table[4] = { 2, 2, 4, 4 };


/*----------------------------------------------------------------------
 *  _i2s_refclk_set
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
static _bool _i2s_refclk_set(_REFCLKI_FreqEnumT refclki, _REFCLKO_FreqEnumT refclko)
{

    u32 source = 0, refcon_flag = 0; u8 div_N = 0; u16 trim_M = 0;  


    //------------------------------------------------------------
    //              Disable REFCLK
    //------------------------------------------------------------

    OSCREFConfig(0, 0, 0);
    

    //------------------------------------------------------------
    //              REFCLKI Pin Map
    //------------------------------------------------------------

    // pin map
    if (refclki == _REFCLKI_22_5792MHz) I2S_REFCLKI_225792_PPS_INPUT();      // Hi-Res osc input
    else if (refclki == _REFCLKI_24_576MHz) I2S_REFCLKI_24576_PPS_INPUT();   // Hi-Res osc input


    //------------------------------------------------------------
    //              REFCON Setting
    //------------------------------------------------------------

    // config flag
#if _I2S_ENABLE_REFCLK_OUT

    refcon_flag = OSC_REFOCON_OE | OSC_REFOCON_ON;      // MCLK output
#else

    refcon_flag = OSC_REFOCON_OE
#endif    
    
    // source select
    if (refclki == _REFCLKI_PLLCLK) source = _I2S_PLLCLK_AS_SOURCE;
    else source = _I2S_REFCLK_AS_SOURCE;

    // N and M
    div_N = _refclk_N_TABLE[(u8)refclki][(u8)refclko];
    trim_M = _refclk_M_TABLE[(u8)refclki][(u8)refclko];
    if ((div_N == 0xFF) || (trim_M ==0xFFFF)) return FALSE;             // error with combination


    //------------------------------------------------------------
    //              Set to Registers
    //------------------------------------------------------------
    
    mOSCREFOTRIMSet(trim_M);                            // set trim M
    OSCREFConfig(source, (OSCREFConfigFlags)refcon_flag, div_N);
    
    // ok to set the MCLK and MLCK output
    return TRUE;
}

/*----------------------------------------------------------------------
 *  _i2s_init
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_I2S_ResT _i2s_init(_I2S_DMA_ConT *conf)
{

    u32 con_reg = 0, con2_reg = 0;    

    
    //------------------------------------------------------------
    //              Init I2S
    //------------------------------------------------------------

    // Disable I2S Module
    SpiChnEnable(conf->spi_id, 0);

    
    //-------------------------------------------------------
    //              Basic setting
    //-------------------------------------------------------

    // CON/ CON2 Register
    con_reg =   _i2s_format_con_TABLE[(u8)conf->_format] | 
                _i2s_dat_width_conf_TABLE[(u8)conf->_dat_width];
    con2_reg =  _i2s_format_con2_TABLE[(u8)conf->_format];

    // Mono Mode
    if (conf->_enable_mono) con2_reg |= SPI_CONFIG2_AUDMONO;    
    SpiChnConfigureEx(conf->spi_id, (SpiConfigFlags)con_reg, (SpiConfig2Flags)con2_reg);
    

    //-------------------------------------------------------
    //              REFCLK setting
    //-------------------------------------------------------

    // refclki and refclko   
    if (!_i2s_refclk_set(conf->_refclki_freq, conf->_refclko_freq)) {       
        return _RES_REFCLKO_SETTING_ERROR; 
    }   


    //-------------------------------------------------------
    //              BaudRate setting
    //-------------------------------------------------------
    
    con_reg = _Fs_BRG_Table[(u8)conf->_refclko_freq][(u8)conf->_fs];

    // Check For Suitable Para
    if (con_reg != 0xFF) SpiChnSetBrg(conf->spi_id, con_reg);        
    else return _RES_BAUD_SETTING_ERROR; 


    //-------------------------------------------------------
    //              Open I2S to Generate BCLK and LRCK
    //-------------------------------------------------------
    
    SpiChnEnable(conf->spi_id, TRUE); return _RES_OK;   // ok to set
}

/*----------------------------------------------------------------------
 *  _i2s_dma_init
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
static void _i2s_dma_init(SpiChannel id, DmaChannel chn, void *tx_buff, _DMA_DistSizeT dst_size)
{

    // basci mode
    DmaChnConfigure(chn, _I2S_DMA_PRI, DMA_CONFIG_DEFAULT);

    // event config
    DmaChnSetEventControl(  chn, 
                            DMA_EV_START_IRQ(_I2S_FLAG_Table[(u8)id - 1].tx_irq));


    //------------------------------------------------------------
    //              Set Tx-Fer
    //------------------------------------------------------------
    
    DmaChnSetTxfer( chn, 
                    tx_buff,                                // source
                    _I2S_FLAG_Table[(u8)id - 1].tx_reg,     // dst
                    0,                                      // zero
                    (u8)dst_size,                           // dist - depend by data width
                    (u8)dst_size);                          // dist - depend by data width   

    // enable CHSDIE event
    DmaChnWriteEvEnableFlags(chn, DMA_EV_SRC_FULL);
    

    //------------------------------------------------------------
    //              Set ISR
    //------------------------------------------------------------

    // set isr priority    
    INTSetVectorPriority(INT_DMA_0_VECTOR + chn, _I2S_DMA_INT_PRI);
    INTSetVectorSubPriority(INT_DMA_0_VECTOR + chn, _I2S_DMA_INT_SUB_PRI);
    
    // enable vector
    DmaChnIntEnable(chn);
}

/*----------------------------------------------------------------------
 *  _i2s_dma_restart
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
static void _i2s_dma_restart(DmaChannel chn, void *tx_buff, u16 ssize)
{

    u8 dst_size = 0;
    
    // abort channel [ this function has while loop !!! ]
    DmaChnAbortTxfer(chn);

    // set txfer
    dst_size = (u8)_I2S_DMA_Run._dma_dst_size;
    DmaChnSetTxfer( chn, 
                    tx_buff, _I2S_FLAG_Table[(u8)_I2S_DMA_Run._conf.spi_id - 1].tx_reg, 
                    ssize, dst_size, dst_size);

    // enable dma to tx buffer
    DmaChnEnable(chn);
}

/*----------------------------------------------------------------------
 *  _i2s_memcpy_2_4 - 2 or 4 bytes to copy data
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u16 _i2s_memcpy_2_4(void *src, void *dst, u16 cnt)
{

    u32 *psrc = src, *pdst = dst; u16 size = 0;
    u16 idx = 0; u8 csize = 0;

    /* address align by 2 or 4 */
    if ((((u32)src % 4) == 0) && (((u32)dst % 4) == 0)) csize = 4;      
    else if ((((u32)src % 2) == 0) && (((u32)dst % 2) == 0)) csize = 2;
    else return 0;      /* address error */          

    /* memory copy */
    size = cnt / csize;
    if (csize == 4) {                                      
        for (idx = 0; idx < size; idx++) { *pdst++ = *psrc++; }     /* copy 4-bytes loop */                                  
    } else {        
        for (idx = 0; idx < size; idx++) {          /* copy 2-bytes loop */                   
            *(u16 *)pdst = *(u16 *)psrc; (u16 *)pdst++; (u16 *)psrc++; 
        }
    }
    size = cnt % csize;         /* remain bytes */   
    for (idx = 0; idx < size; idx++) {                  /* copy 1-byte loop */
        *((u8 *)pdst + idx) = *((u8 *)psrc + idx);             
    }
    return cnt;         /* finish */
}

/*----------------------------------------------------------------------
 *  _i2s_memcpy_4 - 4-byte to copy data
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u16 _i2s_memcpy_4(void *src, void *dst, u16 cnt)
{

    u32 *psrc = src, *pdst = dst; u16 size = 0;
    u16 idx = 0;

    /* must check the address */
    if (((u32)src % 4) != 0) return 0;          /* 4-byte align */
    if (((u32)dst % 4) != 0) return 0;          

    /* memory copy */
    size = cnt / 4;
    for (idx = 0; idx < size; idx++) { *pdst++ = *psrc++; }     /* copy 4-bytes loop */
    size = cnt % 4;                             /* remain bytes */   
    for (idx = 0; idx < size; idx++) {          /* copy 1-byte loop */
        *((u8 *)pdst + idx) = *((u8 *)psrc + idx);             
    }
    return cnt;     /* finish */
}

/*----------------------------------------------------------------------
 *  _i2s_memcpy_2 - 2 bytes to copy data
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u16 _i2s_memcpy_2(void *src, void *dst, u16 cnt)
{

    u16 *psrc = src, *pdst = dst; u16 size = 0;
    u16 idx = 0;

    /* must check the address */
    if (((u32)src % 2) != 0) return 0;          /* 2-byte align */
    if (((u32)dst % 2) != 0) return 0;          

    /* memory copy */
    size = cnt / 2;
    for (idx = 0; idx < size; idx++) { *pdst++ = *psrc++; }     /* copy 2-bytes loop */
    size = cnt % 2;                             /* remain bytes */   
    for (idx = 0; idx < size; idx++) {          /* copy 1-byte loop */
        *((u8 *)pdst + idx) = *((u8 *)psrc + idx);             
    }
    return cnt;     /* finish */
}

/*----------------------------------------------------------------------
 *  I2S_Init - can recalled this function to change the i2s config
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_I2S_ResT I2S_Init(_I2S_DMA_ConT *conf)
{

    u32 con_reg = 0, con2_reg = 0;

    
    //------------------------------------------------------------
    //              Init I2S
    //------------------------------------------------------------

    _I2S_DMA_Run._conf = *conf;
    if (_i2s_init(conf) != _RES_OK) return _RES_FAIL;


    //------------------------------------------------------------
    //              Init DMA
    //------------------------------------------------------------

    // size of the distnation
    _I2S_DMA_Run._dma_dst_size = (_DMA_DistSizeT)_Dist_Table[(u8)_I2S_DMA_Run._conf._dat_width];
    
    // not enable DMA
    _i2s_dma_init(  _I2S_DMA_Run._conf.spi_id,
                    _I2S_DMA_CHN_1,                         // chn 1
                    _I2S_DMA_Run._tx_pusher[0].tx_buff,     // pusher 1
                    _I2S_DMA_Run._dma_dst_size);
    
    // dual DMA
#if _I2S_ENABLE_DUAL_DMA

    _i2s_dma_init(  _I2S_DMA_Run._conf.spi_id,
                    _I2S_DMA_CHN_2,                         // chn 2
                    _I2S_DMA_Run._tx_pusher[1].tx_buff,     // pusher 2
                    _I2S_DMA_Run._dma_dst_size);
#endif
                    

    //------------------------------------------------------------
    //              Init Pusher
    //------------------------------------------------------------

    _I2S_DMA_Run._pusher_idle = 0;
    _I2S_DMA_Run._i2s_status = _I2S_STA_DATA_IDLE;
    _I2S_DMA_Run._tx_pusher[0].cnt = 0; _I2S_DMA_Run._tx_pusher[1].cnt = 0;
    _I2S_DMA_Run._locked = FALSE;

    return _RES_OK;
}

/*----------------------------------------------------------------------
 *  I2S_TxStream - tx data by the data-width to buffer
 *
 *  Purpose: None.
 *
 *  Entry:      src - u16 or u32 (not support u8 !!!)
 *              cnt - the byte size of the src
 *
 *  Exit:    None.
 *
 *  NOTE:       [ 48MHz SYSCLK / 4 byte ] Unit: us
 *
 *              [1] 1024        ---         29.2325
 *              [2] 5120        ---         135.91
 *              [3] 10240       ---         269.26
 *
 *              [ 48MHz SYSCLK / 2 byte ] Unit: us
 *
 *              [1] 1024        ---         56.005
 *              [2] 5120        ---         269.365
 *              [3] 10238       ---         535.855
 * 
 *              ------
 *              Min cnt:
 *              16bit - 4,  24/32bit - 8    (normal mode)
 *              16bit - 2,  24/32bit - 4    (mono mode)
 *
 *---------------------------------------------------------------------*/
u16 I2S_TxStream(void *src, u16 cnt)
{
    
    u8 push_id = 0; 
    u16 cnt_ps = 0, tx_size = 0; u8 *pdst = 0;

    
    // No-Zero Tx
    if (!cnt) return 0;
    
    //------------------------------------------------------------
    //              LR CNT Check
    //------------------------------------------------------------

    // At Least Count
    push_id = _I2S_DMA_Run._dma_dst_size << 1;
    if (_I2S_DMA_Run._conf._enable_mono) push_id >>= 1;             // mono mode    

    // Cut with the Dst
    cnt = cnt - (cnt % push_id);
    
        
    //------------------------------------------------------------
    //              Queue Pusher
    //------------------------------------------------------------

    // Locked [ *Prevent Int ]
    _I2S_DMA_Run._locked = TRUE;
    
    // Queue the Idle
    push_id = _I2S_DMA_Run._pusher_idle;    


    //------------------------------------------------------------
    //              Pause the Tx Size
    //------------------------------------------------------------

    // The data Length of the pusher
    cnt_ps = _I2S_DMA_Run._tx_pusher[push_id].cnt;

    // Cacl the Tx Size
    tx_size = _I2S_DMA_BUFFER_SIZE - cnt_ps; if (cnt < tx_size) tx_size = cnt;                        


    //------------------------------------------------------------
    //              2 or 4 bytes Copy
    //------------------------------------------------------------

    // Distnation Address
    pdst = (u8 *)_I2S_DMA_Run._tx_pusher[push_id].tx_buff + cnt_ps;

    // Quickly Copy
    if ((!((u32)src % 4)) && (!(cnt_ps % 4))) tx_size = _i2s_memcpy_4(src, pdst, tx_size);      // 4bytes
    else if ((!((u32)src % 2)) && (!(cnt_ps % 2))) tx_size = _i2s_memcpy_2(src, pdst, tx_size); // 2bytes
    else tx_size = 0;   // [ address must aligh by 2 or 4 !!! ] 


    //------------------------------------------------------------
    //              Increase Counter and UnLocked  
    //------------------------------------------------------------
    
    _I2S_DMA_Run._tx_pusher[push_id].cnt += tx_size;                                 
    _I2S_DMA_Run._locked = FALSE;

    // Check Counter
    if (_I2S_DMA_Run._tx_pusher[push_id].cnt == 0) return 0;        // nozero
    

    //------------------------------------------------------------
    //              DMA Operation
    //------------------------------------------------------------

    // Queue I2S Status
    if (_I2S_DMA_Run._i2s_status == _I2S_STA_DATA_IDLE)             // no DMA is running
    {        

        
        // Must Modify Status First
        _I2S_DMA_Run._i2s_status = _I2S_STA_RUN;
        _I2S_DMA_Run._pusher_idle = !push_id;
        

        // Enable DMA
#if !_I2S_ENABLE_DUAL_DMA
        
        _i2s_dma_restart(   _I2S_DMA_CHN_1, 
                            _I2S_DMA_Run._tx_pusher[push_id].tx_buff,                         
                            _I2S_DMA_Run._tx_pusher[push_id].cnt);
#else                
#endif /* _I2S_ENABLE_DUAL_DMA */
    }

    return tx_size;         // ok to tx
}

/*----------------------------------------------------------------------
 *  I2S_GetPusherSize
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u8 I2S_GetPusherSize(u16 *re_size)
{

    u8 push_id = 0;
    
    // Return the Remain Size
    push_id = _I2S_DMA_Run._pusher_idle;    
    *re_size =  _I2S_DMA_BUFFER_SIZE - _I2S_DMA_Run._tx_pusher[push_id].cnt;    
    return push_id;         // Return Pusher ID
}

/*----------------------------------------------------------------------
 *  I2S_SetStream
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_I2S_ResT I2S_SetStream(  _REFCLKI_FreqEnumT refclki, _REFCLKO_FreqEnumT refclko, 
                          _I2S_FsEnumT fs, 
                          _I2S_DataWidthEnumT dw )
{

    // NOT SET IN RUNNING
    if (I2S_GetHwStatus() == _I2S_HW_STA_BUSY) return _RES_FAIL;

    
    //------------------------------------------------------------
    //              Init Var
    //------------------------------------------------------------
    
    _I2S_DMA_Run._conf._refclki_freq = refclki;
    _I2S_DMA_Run._conf._refclko_freq = refclko;
    _I2S_DMA_Run._conf._fs = fs;
    _I2S_DMA_Run._conf._dat_width = dw;
    _I2S_DMA_Run._dma_dst_size = (_DMA_DistSizeT)_Dist_Table[(u8)dw];   // dst

    
    //------------------------------------------------------------
    //              Init I2S
    //------------------------------------------------------------

    if (_i2s_init(&_I2S_DMA_Run._conf) != _RES_OK) { _I2S_DMA_Run._i2s_status = _I2S_STA_DISABLE;        
        return _RES_FAIL;
    }
    else 
    { 
        _I2S_DMA_Run._i2s_status = _I2S_STA_DATA_IDLE;  // data idle        
    }
    
    return _RES_OK;
}

/*----------------------------------------------------------------------
 *  I2S_GetTxStatus - get the status of the DMA module
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_I2S_StatusEnumT I2S_GetTxStatus(void)
{

    //------------------------------------------------------------
    //              Logic Status 
    //------------------------------------------------------------
    
    return _I2S_DMA_Run._i2s_status;                
}

/*----------------------------------------------------------------------
 *  I2S_GetHwStatus - get the status of the SPI module
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:       Status Table:
 *              
 *              [1] _I2S_HW_STA_DISABLE - query i2s status 
 *              [2] _I2S_HW_STA_BUSY - query SRMT/ SPITBE bits
 *              [3] _I2S_HW_STA_SR_IDLE - no byte in SR shift-register
 *---------------------------------------------------------------------*/
_I2S_HwStatusT I2S_GetHwStatus(void)
{
    
    SpiStatusFlags reg = 0;    


    // Query Enable
    if (_I2S_DMA_Run._i2s_status == _I2S_STA_DISABLE) return _I2S_HW_STA_DISABLE;


    //------------------------------------------------------------
    //              Query the SRMT and SPITBE
    //------------------------------------------------------------

    // Pause SPI Status Register
    reg = SpiChnGetStatus(_I2S_DMA_Run._conf.spi_id);       

    // SRMT and SPITBE
    if ((reg & (SPI_STAT_SPITBE | SPI_STAT_SPISRMT)) == (SPI_STAT_SPITBE | SPI_STAT_SPISRMT)) {           
        return _I2S_HW_STA_SR_IDLE;         // IDLE status        
    }

    // Busy Status
    return _I2S_HW_STA_BUSY;    // i2s is tx now
}

/*----------------------------------------------------------------------
 *  I2S_ResetTx - reset pusher and disable DMA module
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void I2S_ResetTx(void)
{

    //------------------------------------------------------------
    //              Abort DMA
    //------------------------------------------------------------

    // [ This function has while loop !!! ]
    DmaChnAbortTxfer(_I2S_DMA_CHN_1);
    

    //------------------------------------------------------------
    //              Reset Pusher
    //------------------------------------------------------------
    
    _I2S_DMA_Run._pusher_idle = 0;      
    _I2S_DMA_Run._tx_pusher[0].cnt = 0; _I2S_DMA_Run._tx_pusher[1].cnt = 0;
    _I2S_DMA_Run._locked = FALSE;

    // Status
    if (_I2S_DMA_Run._i2s_status) _I2S_DMA_Run._i2s_status = _I2S_STA_DATA_IDLE;   
}

/*----------------------------------------------------------------------
 *  I2S_SetMonoMode - enable or disable mono mode
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_I2S_ResT I2S_SetMonoMode(_bool en)
{

    // NOT SET IN RUNNING
    if (I2S_GetHwStatus() == _I2S_HW_STA_BUSY) return _RES_FAIL;    // failed

    
    //------------------------------------------------------------
    //              Init Var
    //------------------------------------------------------------
    
    _I2S_DMA_Run._conf._enable_mono = en;

    
    //------------------------------------------------------------
    //              Init I2S
    //------------------------------------------------------------

    if (_i2s_init(&_I2S_DMA_Run._conf) != _RES_OK) { _I2S_DMA_Run._i2s_status = _I2S_STA_DISABLE;        
        return _RES_FAIL;
    }
    else 
    { 
        _I2S_DMA_Run._i2s_status = _I2S_STA_DATA_IDLE;  // data idle        
    }
    
    return _RES_OK;
}


/*----------------------------------------------------------------------
 *  __ISR
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
#if _I2S_ENABLE_DUAL_DMA 
void __ISR(_DMA_0_VECTOR + (u8)_I2S_DMA_CHN_1, ipl7AUTO) DmaPusher1Handler(void)
{
}

/*----------------------------------------------------------------------
 *  __ISR
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void __ISR(_DMA_0_VECTOR + (u8)_I2S_DMA_CHN_2, ipl7AUTO) DmaPusher2Handler(void)
{
}

#else
/*----------------------------------------------------------------------
 *  __ISR
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void __ISR(_DMA_0_VECTOR + (u8)_I2S_DMA_CHN_1, ipl7AUTO) DmaPusher1Handler(void)
{

    u8 push_id = 0;


    //------------------------------------------------------------
    //              Clear DMA/ ISR FLAG
    //------------------------------------------------------------
     
    DmaChnClrEvFlags(_I2S_DMA_CHN_1, DMA_EV_SRC_FULL);      
    INTClearFlag(INT_DMA0 + _I2S_DMA_CHN_1);                


    //------------------------------------------------------------
    //              Reset Pusher Now
    //------------------------------------------------------------

    // Pusher Now
    push_id = _I2S_DMA_Run._pusher_idle; push_id = !push_id;
    _I2S_DMA_Run._tx_pusher[push_id].cnt = 0;                       // reset the counter now


    //------------------------------------------------------------
    //              Ready to Tx Next Pusher
    //------------------------------------------------------------
    
    // Queue Pusher    
    if (_I2S_DMA_Run._locked) 
    {     
    
        _I2S_DMA_Run._i2s_status = _I2S_STA_DATA_IDLE;              // 'no-data' status 
        return; 
    }
    else 
    {                
    
        push_id = !push_id;                                    
        if (_I2S_DMA_Run._tx_pusher[push_id].cnt)
        {            
            
            // new idle pusher            
            _I2S_DMA_Run._pusher_idle = !push_id;               

            // DMA1 to tx
            _i2s_dma_restart(   _I2S_DMA_CHN_1, 
                                _I2S_DMA_Run._tx_pusher[push_id].tx_buff,                         
                                _I2S_DMA_Run._tx_pusher[push_id].cnt);
        }
        else 
        {

            _I2S_DMA_Run._i2s_status = _I2S_STA_DATA_IDLE;
        }
    }


    //------------------------------------------------------------
    //              CALL-BACK
    //------------------------------------------------------------

    // Macro to Enable it
#if _I2S_ENABLE_ISR_CALLBACK

    if (_I2S_DMA_Run._conf._isr_call != NULL) (*_I2S_DMA_Run._conf._isr_call)();
#endif

}

#endif /* _I2S_ENABLE_DUAL_DMA */
//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
