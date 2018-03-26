
/*----------------------------------------------------------------------^^-
/ File name:  keypad_lp.h
/ Author:     JiangJun
/ Data:       2018/3/17
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ Keypad Common Lib
/ ---
/ Support:
/ 1. Long Push-down Function
/ 2. Key Push-down Time record
/------------------------------------------------------------------------*/

#include "main.h"

//------------------------------------------------------------
//             KEYPAD CONFIG
//------------------------------------------------------------

#define _KEYPAD_BUFFER_SIZE         4

//------------------------------------------------------------
//             KEYPAD Value Define
//------------------------------------------------------------

typedef enum {

    _KEY_PUSH_NONE = 0,
    _KEY_PUSH_TOP,
    _KEY_PUSH_BOTTOM,
    
} KeyPushValueT;

//------------------------------------------------------------
//             KEYPAD Data Struct
//------------------------------------------------------------

typedef struct {

    KeyPushValueT value;
    u16 key_push_cnt;
    u32 key_push_time;      // time = cnt * scan-period 
    
} KeyPushT;

// Keypad Data struct
typedef struct {

    KeyPushT key_push_buffer[_KEYPAD_BUFFER_SIZE];
    u8 read_idx, write_idx;
    KeyPushValueT key_last_push;
    u16 key_scan_period;
    u16 key_scan_cnt;
    
} KeypadPushT;


extern void KeypadInit(u16 scan_period);
extern void KeypadReset(void);
extern void KeypadTimeProc(void);
extern _bool KeypadGetValue(KeyPushT *key_push);


//-------------------------------------------------
//-------------End of file-------------------------
//-------------------------------------------------