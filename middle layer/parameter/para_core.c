
/*----------------------------------------------------------------------^^-
/ File name:  para_core.c
/ Author:     JiangJun
/ Data:       2018/3/29
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ Parameter Common Source File
/------------------------------------------------------------------------*/

#include "para_core.h"
#include "_crc.h"

//------------------------------------------------------------
//             Static Var
//------------------------------------------------------------

static fpReadBytes _FpReadBytes; static fpWriteBytes _FpWriteBytes;
static _ParaHeaderT _ParaHeader; static u8 _Para_R_W_Buffer[_PARA_R_W_BUFFER_SIZE];

/*----------------------------------------------------------------------
 *  _para_core_cmp
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u16 _para_core_cmp(u8 *src, u8 *dst, u16 cnt)
{

    u16 idx = 0; u16 no_equ_cnt = 0;    

    // 'src' compare with the 'dst'
    for (idx = 0; idx < cnt; idx++, dst++, src++)
    {
        if (*dst != *src) 
        {
            no_equ_cnt++;
        }
    }

    return no_equ_cnt;
}

/*----------------------------------------------------------------------
 *  _para_core_init - return the Secure check result
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u8 _para_core_init(fpReadBytes fp_r_bytes, fpWriteBytes fp_w_bytes, u8 id, u16 size) 
{

    u8 res = 0;

    // Save points
    _FpReadBytes = fp_r_bytes; _FpWriteBytes = fp_w_bytes;

    //------------------------------------------------------------
    //              Read Bytes
    //------------------------------------------------------------
    
    if (_FpReadBytes == 0) 
    {     
        res |= _PARA_FUNC_RES_R_W_ERROR_MASK;
        return res;
    }

    // Check Size
    if ((size + sizeof(_ParaHeaderT)) > _PARA_R_W_BUFFER_SIZE)
    {    
        res |= _PARA_FUNC_RES_SIZE_ERROR_MASK;
        return res;
    }

    // Read Header and Data Field to Buffer
    if (_FpReadBytes(_Para_R_W_Buffer, size + sizeof(_ParaHeaderT)) != (size + sizeof(_ParaHeaderT)))
    {
        res |= _PARA_FUNC_RES_R_W_ERROR_MASK;
        return res;
    }


    //------------------------------------------------------------
    //              Check Info
    //------------------------------------------------------------

    // Sync Header from Buffer
    _ParaHeader = *(_ParaHeaderT *)_Para_R_W_Buffer;
    
    // Check ID
    if (_ParaHeader._id != id)
    {
        res |= _PARA_FUNC_RES_ID_ERROR_MASK;      
    }

    // Check byte-count
    if (_ParaHeader._byte_cnt != size)
    {
        res |= _PARA_FUNC_RES_BC_ERROR_MASK;         
    }

    // Check CRC16
    if (CRC16(&_Para_R_W_Buffer[sizeof(_ParaHeaderT)], size) != _ParaHeader._crc16)
    {
        res |= _PARA_FUNC_RES_CRC_ERROR_MASK;        
    }
    

    //------------------------------------------------------------
    //              IF Need To Reset Parameter
    //------------------------------------------------------------
    
    if (res & (_PARA_FUNC_RES_ID_ERROR_MASK | _PARA_FUNC_RES_BC_ERROR_MASK | _PARA_FUNC_RES_CRC_ERROR_MASK))
    {
    
        _ParaHeader._id = id;
        _ParaHeader._byte_cnt = size;
        _ParaHeader._save_cnt = 1;
        _ParaHeader._secure_key = 0x00;

        // CRC16
        _ParaHeader._crc16 = 0x00;

        // Write bytes
        if (_FpWriteBytes == 0) 
        {
            res |= _PARA_FUNC_RES_R_W_ERROR_MASK;
        }
        else
        {

            // Sync Header to Buffer
            *(_ParaHeaderT *)_Para_R_W_Buffer = _ParaHeader;
            
            // Write Header and Data Field to FLASH
            if (_FpWriteBytes(_Para_R_W_Buffer, size + sizeof(_ParaHeaderT)) != (size + sizeof(_ParaHeaderT)))
            {
                res |= _PARA_FUNC_RES_R_W_ERROR_MASK;
            }            
        }
    }

    return res;
}

/*----------------------------------------------------------------------
 *  _para_core_read
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u16 _para_core_read(void *tye, u16 size)
{

    u16 idx = 0; u8 *dst = (u8 *)tye;
    
    // Check Pointer
    if (_FpReadBytes == 0)
    {
        return 0;
    }

    // Check Size
    if ((size + sizeof(_ParaHeaderT)) > _PARA_R_W_BUFFER_SIZE)
    {            
        return 0;
    }
    
    // Read Header and Data Field to Buffer
    if (_FpReadBytes(_Para_R_W_Buffer, size + sizeof(_ParaHeaderT)) != (size + sizeof(_ParaHeaderT)))
    {
        return 0;
    }

    // Sync Header from Buffer
    _ParaHeader = *(_ParaHeaderT *)_Para_R_W_Buffer;

    // Copy to tye
    for (idx = 0; idx < size; idx++)
    {
        dst[idx] = _Para_R_W_Buffer[idx + sizeof(_ParaHeaderT)];
    }

    return idx;
}

/*----------------------------------------------------------------------
 *  _para_core_write_check
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u16 _para_core_write_check(void *tye, u16 size)
{

    u16 idx = 0, crc16 = 0; u8 *src = (u8 *)tye, byte_cnt = 0;
    u32 save_cnt = 0;         

    //------------------------------------------------------------
    //              Check Pointer and size
    //------------------------------------------------------------
    
    if (_FpWriteBytes == 0)
    {
        return 0;
    }

    if (_FpReadBytes == 0)
    {
        return 0;
    }

    // Check Size
    if ((size + sizeof(_ParaHeaderT)) > _PARA_R_W_BUFFER_SIZE)
    {            
        return 0;
    }

    //------------------------------------------------------------
    //              Modifie the Header
    //------------------------------------------------------------
    
    _ParaHeader._byte_cnt = size; byte_cnt = size;               
    _ParaHeader._save_cnt++; save_cnt = _ParaHeader._save_cnt;               
    _ParaHeader._crc16 = CRC16(src, size); crc16 = _ParaHeader._crc16;    

    //------------------------------------------------------------
    //           Sync Header and Data Field to Buffer
    //------------------------------------------------------------
    
    // Sync Header to Buffer
    *(_ParaHeaderT *)_Para_R_W_Buffer = _ParaHeader;

    // Sync Data Field to Buffer
    for (idx = 0; idx < size; idx++)
    {
        _Para_R_W_Buffer[idx + sizeof(_ParaHeaderT)] = src[idx];
    }

    //------------------------------------------------------------
    //           Write Buffer to FLASH
    //------------------------------------------------------------

    if (_FpWriteBytes(_Para_R_W_Buffer, size + sizeof(_ParaHeaderT)) != (size + sizeof(_ParaHeaderT)))
    {
        return 0;
    }  

    //------------------------------------------------------------
    //           Read Check
    //------------------------------------------------------------

    // Read Header and Data Field to Buffer
    if (_FpReadBytes(_Para_R_W_Buffer, size + sizeof(_ParaHeaderT)) != (size + sizeof(_ParaHeaderT)))
    {
        return 0;
    }

    // Sync Header from Buffer
    _ParaHeader = *(_ParaHeaderT *)_Para_R_W_Buffer;

    // Compare Header
    if (_ParaHeader._byte_cnt != byte_cnt)  { return 0; }
    if (_ParaHeader._save_cnt != save_cnt)  { return 0; }
    if (_ParaHeader._crc16 != crc16)        { return 0; }
    
    // Compare Data Field
    if (_para_core_cmp(&_Para_R_W_Buffer[sizeof(_ParaHeaderT)], src, byte_cnt) != 0)
    {
        return 0;
    }
    
    return size;
}

/*----------------------------------------------------------------------
 *  _para_core_write_cmp - return the writed bytes size
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *
 *  NOTE:    1. No new data, return 0.
 *           2. Write Failed, return 0.
 *---------------------------------------------------------------------*/
u16 _para_core_write_cmp(void *tye, u16 size)
{

    // Check Size
    if ((size + sizeof(_ParaHeaderT)) > _PARA_R_W_BUFFER_SIZE)
    {            
        return 0;
    }
    
    // Compare Data Field
    if (_para_core_cmp(&_Para_R_W_Buffer[sizeof(_ParaHeaderT)], (u8 *)tye, size) == 0)
    {
        return 0;
    }

    // Write to FLASH and Read Check
    if (_para_core_write_check(tye, size) == size)
    {
        return size;
    }

    return 0;
}

