
/*----------------------------------------------------------------------^^-
/ File name:  st75256.c
/ Author:     JiangJun
/ Data:       2018/2/9
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ st75256 driver
/------------------------------------------------------------------------*/

#include "main.h"
#include "st75256.h"

#include "dbi_8.h"

//------------------------------------------------------------
//              MACROS
//------------------------------------------------------------

#if _ST75256_ENABLE_GREY

  // [ Gray Mode, 2 bits/ piexl ]
  #define _ST75256_BYTE_PIXELS              4
  
#else  

  // [ Gray Mode, 1 bit/ piexl ]
  #define _ST75256_BYTE_PIXELS              8
  
#endif /* _ST75256_ENABLE_GREY */

//------------------------------------------------------------
// CMD Table
//------------------------------------------------------------
typedef enum {
    
    st_ex_cmd_01 = 0x30 | 0x00,
    st_ex_cmd_02 = 0x30 | 0x01,
    st_ex_cmd_03 = 0x30 | 0x02,
    st_ex_cmd_04 = 0x30 | 0x03,
    
    st_sleep_out_mode = 0x94,
    st_power_ctrl = 0x20,
    st_set_vop = 0x81,
    
    st_ana_cir_set = 0x32,
    st_boter_level = 0x51,
    st_driving_sel_int = 0x40,  // internal Power
    st_set_gray_level = 0x20,
    
    st_dis_on = 0xAF,
    st_dis_off = 0xAE,
    
    st_dis_mode = 0xF0,
    st_dis_ctrl = 0xCA,
    st_dat_scan_dir =  0xBC,
    st_inver_dis_normal = 0xA6,
    st_data_format_lsb_bot = 0x08,
    st_data_format_lsb_top = 0x0C,
    
    st_set_paga = 0x75,
    st_set_column = 0x15,
    st_write_data = 0x5C,

    st_all_dis_on = 0x23,
    
} ST75256CmdT;

/*----------------------------------------------------------------------
 *  St75256SetAddress
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    Page: 0-20(0-14h), column: 0-255(0-FFh)
 *---------------------------------------------------------------------*/
void St75256SetAddress(u8 page_s, u8 page_e, u8 column_s, u8 column_e)
{

#if (_ST75256_COMSCAN == 0)

    DbiSendsCommand(st_set_paga);
    DbiWriteParameter(page_s);
    DbiWriteParameter(page_e);
          
#else
    
    u8 cs_page_s = page_s + ((160 - _ST75256_LCD_Y_SIZE) / _ST75256_BYTE_PIXELS);
    u8 cs_page_e = cs_page_s + (page_e - page_s);

    DbiSendsCommand(st_set_paga);
    DbiWriteParameter(cs_page_s);
    DbiWriteParameter(cs_page_e);

#endif

    DbiSendsCommand(st_set_column);
    DbiWriteParameter(column_s);
    DbiWriteParameter(column_e);  
}

/*----------------------------------------------------------------------
 *  St75256WriteByte
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void St75256WriteByte(u8 page, u8 x, u8 dat)
{

#if (_ST75256_COMSCAN == 0)

    DbiSendsCommand(st_set_paga);
    DbiWriteParameter(page);
    DbiWriteParameter(page);
          
#else
    
    u8 cs_page = page + ((160 - _ST75256_LCD_Y_SIZE) / _ST75256_BYTE_PIXELS);

    DbiSendsCommand(st_set_paga);
    DbiWriteParameter(cs_page);
    DbiWriteParameter(cs_page);

#endif

    DbiSendsCommand(st_set_column);
    DbiWriteParameter(x);
    DbiWriteParameter(x);  

    // Write data.
    DbiSendsCommand(st_write_data);
    DbiWriteParameter(dat);
}

/*----------------------------------------------------------------------
 *  St75256ClrScreen
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void St75256ClrScreen(void)
{
    u16 x = 0, y = 0;   
    
    // Line Scan Write 
    for (y = 0; y < (_ST75256_LCD_Y_SIZE / _ST75256_BYTE_PIXELS); y++)
    {
        St75256SetAddress(y, y, 0, _ST75256_LCD_X_SIZE - 1);
    
        // Write data.
        DbiSendsCommand(st_write_data);
    
        for (x = 0; x < _ST75256_LCD_X_SIZE; x++)
        {                                                
            DbiWriteParameter(0x00);
        }
    }
}

/*----------------------------------------------------------------------
 *  St75256Init
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void St75256Init()
{       
    DbiBusOutputModeEnable();
    
    // hardware reset
    DbiHardwareRst();     
   
    // Extension Cmd 01
    DbiSendsCommand(st_ex_cmd_01); 
    
    // LCD power
    DbiSendsCommand(st_sleep_out_mode);     
    DbiSendsCommand(st_power_ctrl); 
    DbiWriteParameter(0x0B);            // VB/VR/VF All On
    
    DbiSendsCommand(st_set_vop);        
    DbiWriteParameter((u8)((u16)((_ST75256_VOP_DEF_VTG - 3.6f) / 0.04f) & 0x3F));
    DbiWriteParameter((u8)((u16)((_ST75256_VOP_DEF_VTG - 3.6f) / 0.04f) >> 6));
    
    // Extension Cmd 02
    DbiSendsCommand(st_ex_cmd_02); 
    
    DbiSendsCommand(st_ana_cir_set);    // Analog Circuit Set
    DbiWriteParameter(0x00);
    DbiWriteParameter(0x01);            // Booster Efficiency = Level 1
    DbiWriteParameter(0x03);            // Bias=1/11
    DbiSendsCommand(st_boter_level);    // Booster Level x10
    DbiWriteParameter(0xFB);
    DbiSendsCommand(st_driving_sel_int);    // Internal Power
    
    // Gray Level
    DbiSendsCommand(st_set_gray_level);
    DbiWriteParameter(0x00);
    DbiWriteParameter(0x00);
    DbiWriteParameter(0x00);
    
    DbiWriteParameter(_ST75256_LIGHT_GRAY);      //Light Gray Level Setting
    DbiWriteParameter(_ST75256_LIGHT_GRAY);      //Light Gray Level Setting
    DbiWriteParameter(_ST75256_LIGHT_GRAY);      //Light Gray Level Setting
    
    DbiWriteParameter(0x00);
    DbiWriteParameter(0x00);
    DbiWriteParameter(_ST75256_DARK_GRAY);      //Dark Gray Level Setting
    DbiWriteParameter(0x00);
    DbiWriteParameter(0x00);
    DbiWriteParameter(_ST75256_DARK_GRAY);      //Dark Gray Level Setting
    DbiWriteParameter(_ST75256_DARK_GRAY);      //Dark Gray Level Setting
    DbiWriteParameter(_ST75256_DARK_GRAY);      //Dark Gray Level Setting
    DbiWriteParameter(0x00);
    DbiWriteParameter(0x00);
    
    // Extension Cmd 01
    DbiSendsCommand(st_ex_cmd_01); 
    
    DbiSendsCommand(st_dis_mode);               // Display Mode
    
#if _ST75256_ENABLE_GREY    
    DbiWriteParameter(0x11);                    // 4G Mode
#else
    DbiWriteParameter(0x10);                    // Mono Mode
#endif
    DbiSendsCommand(st_dis_ctrl);               // Display Control
    DbiWriteParameter(0x00);                    // CL Dividing Ratio, Not Divide
    DbiWriteParameter(0x9F);                    // Duty Set, 160 Duty
    DbiWriteParameter(0x00);                    // Frame Inversion
    DbiSendsCommand(st_dat_scan_dir);           // Data Scan Direction

#if ((_ST75256_MX_ENABLE == 0) && (_ST75256_MY_ENABLE == 0))

    // MX: 0  MY: 0
    DbiWriteParameter(0x00);
#elif ((_ST75256_MX_ENABLE == 0) && (_ST75256_MY_ENABLE == 1))

    // MX: 0  MY: 1
    DbiWriteParameter(0x01);
#elif ((_ST75256_MX_ENABLE == 1) && (_ST75256_MY_ENABLE == 0))

    // MX: 1  MY: 0
    DbiWriteParameter(0x02);
#else

    // MX: 1  MY: 1
    DbiWriteParameter(0x03);
#endif    
    
    DbiSendsCommand(st_inver_dis_normal);       // Normal Display
    
    DbiSendsCommand(st_data_format_lsb_top);    // LSB on TOP 

    St75256ClrScreen();
    
    DbiSendsCommand(st_dis_on);     
}

/*----------------------------------------------------------------------
 *  St75256WriteFrame
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
// [ Gray/ Mono Write ]
void St75256WriteFrame(u8 *dat, u32 len)
{

    u16 x = 0, y = 0;  
    
    if (len != (_ST75256_LCD_X_SIZE * _ST75256_LCD_Y_SIZE / _ST75256_BYTE_PIXELS))
    {
        return;
    }     
    
    // Line Scan Write 
    for (y = 0; y < (_ST75256_LCD_Y_SIZE / _ST75256_BYTE_PIXELS); y++)
    {
        St75256SetAddress(y, y, 0, _ST75256_LCD_X_SIZE - 1);
    
        // Write data.
        DbiSendsCommand(st_write_data);
    
        for (x = 0; x < _ST75256_LCD_X_SIZE; x++)
        {                                                
            DbiWriteParameter(*dat); dat++;
        }
    }
}

/*----------------------------------------------------------------------
 *  St75256SetContrast
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    50% is Default
 *---------------------------------------------------------------------*/
void St75256SetContrast(u8 contrast)
{
    float vop = 0.0f;

    if (contrast <= 50)
    {
        vop = _ST75256_VOP_DEF_VTG - ((float)(contrast) * 0.02f);
    }
    else
    {
        vop = _ST75256_VOP_DEF_VTG + ((float)(contrast) * 0.02f);
    }
    
    DbiSendsCommand(st_set_vop);       
    DbiWriteParameter((u8)((u16)((vop - 3.6f) / 0.04f) & 0x3F));
    DbiWriteParameter((u8)((u16)((vop - 3.6f) / 0.04f) >> 6));
}

//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//