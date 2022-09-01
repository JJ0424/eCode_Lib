
/*----------------------------------------------------------------------^^-
/ File name:  keypad_lp.h
/ Author:     JiangJun
/ Data:       [2022-9-1]
/ Version:    v1.30
/-----------------------------------------------------------------------^^-
/ Keypad Common Lib
/------------------------------------------------------------------------*/

#ifndef _KEYPAD_LP_H
#define _KEYPAD_LP_H

#include "main.h"

//------------------------------------------------------------
//             KEYPAD CONFIG
//------------------------------------------------------------

// BUFFER WITH KEYPAD
#define _KEYPAD_BUFFER_SIZE                     4

// The Count of the Key
#define _KEYPAD_TOTAL_KEYS                      1


//------------------------------------------------------------
//             KEYPAD Data Struct
//------------------------------------------------------------

typedef struct {

    u8 key_idx;             // Key ID
    u8 key_value;           // 0: No Key, other: Key Value
    u16 key_push_cnt;       // Counter
    u32 key_push_time;      // Time = key_push_cnt * key_scan_period
    
} KeyPushT;

typedef struct {

    u8 key_last_value;
    u16 key_scan_cnt;
    
} KeyCalaT;

// Keypad Data struct
typedef struct {

    // The Keypad Buffer
    u8 read_idx, write_idx;  
    u16 key_scan_period;   
    KeyPushT key_push_buffer[_KEYPAD_BUFFER_SIZE];

    // The Cala-BuF with one KEY
    KeyCalaT key_cala_buffer[_KEYPAD_TOTAL_KEYS];
    
} KeypadPushT;


//------------------------------------------------------------
//             PORT Function Pointer
//------------------------------------------------------------

// Read the Low-Level IO
typedef void (*fpKeyIO_Read)(u8 *out, u8 key_cnt);


extern void KeypadInit(fpKeyIO_Read fp_io_read, u16 scan_period);
extern void KeypadReset(void);
extern void KeypadTimeProc(void);
extern _bool KeypadGetValue(KeyPushT *key_push);
extern void KeypadGetStatus(u8 key_idx, u8 *key_value, u32 *key_push_time);

#endif

//-------------------------------------------------
//-------------End of file-------------------------
//-------------------------------------------------
