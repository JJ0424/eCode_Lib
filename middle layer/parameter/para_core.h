
/*----------------------------------------------------------------------^^-
/ File name:  para_core.h
/ Author:     JiangJun
/ Data:       2018/3/29
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ Parameter Common Source File
/------------------------------------------------------------------------*/

#ifndef _PARA_CORE_H
#define _PARA_CORE_H

#include "main.h"

//------------------------------------------------------------
//              PARA CONFIG
//------------------------------------------------------------

// R/W Buffer Size
#define _PARA_R_W_BUFFER_SIZE               100     // bytes


//------------------------------------------------------------
//              FUNC RESULT
//------------------------------------------------------------

#define _PARA_FUNC_RES_ID_ERROR_MASK        0x01    // ID 
#define _PARA_FUNC_RES_BC_ERROR_MASK        0x02    // byte count
#define _PARA_FUNC_RES_CRC_ERROR_MASK       0x04    // data CRC field
#define _PARA_FUNC_RES_SCE_ERROR_MASK       0x08    // secure
#define _PARA_FUNC_RES_R_W_ERROR_MASK       0x10    // write/ read error
#define _PARA_FUNC_RES_SIZE_ERROR_MASK      0x20    // parameter size is too large


//------------------------------------------------------------
//              PORT FUNCTION PONTER
//------------------------------------------------------------

// Read bytes
typedef u16 (*fpReadBytes)(u8 *dst, u16 cnt);

// Write bytes
typedef u16 (*fpWriteBytes)(u8 *src, u16 cnt);


//------------------------------------------------------------
//              PARAMETER STRUCT
//------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

typedef struct {

    u8 _id;             // Para ID
    u32 _secure_key;    // Secure Key!
    u32 _save_cnt;      // Save time
    u8 _byte_cnt;       // Data count
    u16 _crc16;         // CRC16(data field)
    
} _ParaHeaderT;

#pragma pack(pop)


extern u8 _para_core_init(fpReadBytes fp_r_bytes, fpWriteBytes fp_w_bytes, u8 id, u16 size);
extern u16 _para_core_read(void *tye, u16 size);
extern u16 _para_core_write_check(void *tye, u16 size);
extern u16 _para_core_write_cmp(void *tye, u16 size);
extern u16 _para_core_cmp(u8 *src, u8 *dst, u16 cnt);

#endif

