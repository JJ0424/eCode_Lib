
/*----------------------------------------------------------------------^^-
/ File name:  keypad_lp.c
/ Author:     JiangJun
/ Data:       [2022-9-1]
/ Version:    v1.30
/-----------------------------------------------------------------------^^-
/ Keypad Common Lib
/ ---
/ Support:
/ 1. Long Push-down Function
/ 2. Key Push-down Time record
/ 3. Key Status Check
/ ---
/ v1.1 [2022-4-18]
/ 1. modify the TRUE/ FALSE to _true/ _false
/ ---
/ v1.2 [2022-4-22]
/ 1. FIX: KeypadGetValue() - key_value will return '\0' when no key push
/ ---
/ v1.21 [2022-6-14]
/ 1. adpter to keil
/ ---
/ v1.30
/ 1. FIX: init and reset should init buffer
/------------------------------------------------------------------------*/

#include "keypad_lp.h"

//------------------------------------------------------------
//             Keypad Data Struct
//------------------------------------------------------------

static KeypadPushT KeypadPush; static fpKeyIO_Read FpKeyIO_Read;

/*----------------------------------------------------------------------
 *  KeypadInit
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void KeypadInit(fpKeyIO_Read fp_io_read, u16 scan_period)
{	

    u8 idx = 0;
    
    // Init the Var
    for (idx = 0; idx < _KEYPAD_TOTAL_KEYS; idx++)
    {

        KeypadPush.key_cala_buffer[idx].key_last_value = 0; KeypadPush.key_cala_buffer[idx].key_scan_cnt = 0;        
    }

    // Init the Buffer
    for (idx = 0; idx < _KEYPAD_BUFFER_SIZE; idx++)
    {

        KeypadPush.key_push_buffer[idx].key_idx = 0; KeypadPush.key_push_buffer[idx].key_value = 0;        
        KeypadPush.key_push_buffer[idx].key_push_cnt = 0; KeypadPush.key_push_buffer[idx].key_push_time = 0;                       
    }

    // r/w index to 0
	KeypadPush.key_scan_period = scan_period;
	KeypadPush.read_idx = 0; KeypadPush.write_idx = 0;

    // IO Function Pointer
	FpKeyIO_Read = fp_io_read;
}

/*----------------------------------------------------------------------
 *  KeypadReset
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void KeypadReset(void)
{

    u8 idx = 0;
    
    // Init the Var
    for (idx = 0; idx < _KEYPAD_TOTAL_KEYS; idx++)
    {

        KeypadPush.key_cala_buffer[idx].key_last_value = 0; KeypadPush.key_cala_buffer[idx].key_scan_cnt = 0;        
    }

    // Init the Buffer
    for (idx = 0; idx < _KEYPAD_BUFFER_SIZE; idx++)
    {

        KeypadPush.key_push_buffer[idx].key_idx = 0; KeypadPush.key_push_buffer[idx].key_value = 0;        
        KeypadPush.key_push_buffer[idx].key_push_cnt = 0; KeypadPush.key_push_buffer[idx].key_push_time = 0;                       
    }

    // r/w index to 0
	KeypadPush.read_idx = 0; KeypadPush.write_idx = 0;
}

/*----------------------------------------------------------------------
 *  KeypadScan
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_bool KeypadScan(u8 new_key_value, KeyCalaT *p_scan_key, u8 *rt_key_value, u16 *scan_cnt)
{					

    // RESET Return Value
    *rt_key_value = 0; *scan_cnt = 0;
    
	// State one, just pull down the key
	if ((new_key_value != 0) && (p_scan_key->key_last_value == 0))	
	{
	
		p_scan_key->key_last_value = new_key_value;
		p_scan_key->key_scan_cnt = 1;
		
		return _false;
	}

	// State two, pulling-down the Key
	if ((new_key_value == p_scan_key->key_last_value) && (p_scan_key->key_last_value != 0))
	{

	    // Time Add, 'key_scan_cnt' Must >= 1
	    if (p_scan_key->key_scan_cnt > 0)
	    {
            p_scan_key->key_scan_cnt++;
	    }

	    return _false;
	}
	
	// State three, release the key
	if ((new_key_value != p_scan_key->key_last_value) && (p_scan_key->key_last_value != 0))
	{		
	
		*rt_key_value = p_scan_key->key_last_value; *scan_cnt = p_scan_key->key_scan_cnt;

		// Reset the last push
		p_scan_key->key_last_value = 0; p_scan_key->key_scan_cnt = 0;
				
		return _true;			
	}

	return _false;
}

/*----------------------------------------------------------------------
 *  KeypadTimeProc
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void KeypadTimeProc(void)
{

    u8 key_value_BuF[_KEYPAD_TOTAL_KEYS]; u8 scan_value; u16 time_cnt = 0;	
    u8 idx = 0;
	
    // Reset the Index to 0, to avoid overflow
	if (KeypadPush.read_idx == KeypadPush.write_idx)			   	
	{
	
		KeypadPush.read_idx = 0; KeypadPush.write_idx = 0;
	}

	// Read Low-Level IO
	if (FpKeyIO_Read != NULL)
	{

        (*FpKeyIO_Read)(key_value_BuF, _KEYPAD_TOTAL_KEYS);
	}


    //------------------------------------------------------------
    //              STATE MACHINE
    //------------------------------------------------------------

    for (idx = 0; idx < _KEYPAD_TOTAL_KEYS; idx++)
    {

        // Scan keypad to get the new key-value
    	if (KeypadScan(key_value_BuF[idx], &KeypadPush.key_cala_buffer[idx], &scan_value, &time_cnt) == _false) 
    	{			

    	    // To Next Key
    		continue;
    	}
    	else
    	{

            // Store to the Buffer
    		KeypadPush.key_push_buffer[KeypadPush.write_idx].key_idx = idx;
    		KeypadPush.key_push_buffer[KeypadPush.write_idx].key_value = scan_value;
    		KeypadPush.key_push_buffer[KeypadPush.write_idx].key_push_cnt = time_cnt;
    		KeypadPush.key_push_buffer[KeypadPush.write_idx].key_push_time = time_cnt * KeypadPush.key_scan_period;
    		
    		// To avoid the array overflow
    		if (KeypadPush.write_idx < (_KEYPAD_BUFFER_SIZE - 1)) 
    		{													 	
    			KeypadPush.write_idx++;	
    		}		
    	}
    }   
}

/*----------------------------------------------------------------------
 *  KeypadGetValue
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_bool KeypadGetValue(KeyPushT *key_push)
{

	KeyPushT new_key;

	// Read from buffer
	if (KeypadPush.read_idx < KeypadPush.write_idx)		   
	{
	    new_key = KeypadPush.key_push_buffer[KeypadPush.read_idx];
		KeypadPush.read_idx++;

		*key_push = new_key;
	}
	else 
	{
	    key_push->key_idx = 0; key_push->key_value = '\0';
	    key_push->key_push_cnt = 0; key_push->key_push_time = 0;
	    
		return _false;	
	}
	
	return _true;	
}

/*----------------------------------------------------------------------
 *  KeypadGetStatus
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void KeypadGetStatus(u8 key_idx, u8 *key_value, u32 *key_push_time)
{

    if (key_idx < _KEYPAD_TOTAL_KEYS)
    {

        *key_value = KeypadPush.key_cala_buffer[key_idx].key_last_value;
        *key_push_time = KeypadPush.key_cala_buffer[key_idx].key_scan_cnt * KeypadPush.key_scan_period;
    }
    else
    {

        *key_value = 0; *key_push_time = 0;
    }
}

//------------------------------------------------------
//----------------End of file---------------------------
//------------------------------------------------------

