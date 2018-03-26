
/*----------------------------------------------------------------------^^-
/ File name:  keypad_lp.c
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

#include "keypad_lp.h"
#include "hconfig.h"

//------------------------------------------------------------
//             Keypad Data Struct
//------------------------------------------------------------

static KeypadPushT KeypadPush;


/*----------------------------------------------------------------------
 *  portKeypadGetHwPort
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
KeyPushValueT portKeypadGetHwPort(void)
{

    // Read Hw IO
    if (KEYPAD_TOP_READ() == KEYPAD_PUSH_DOWN_LEVEL)
    {
        return _KEY_PUSH_TOP;
    }
    else if (KEYPAD_BOTTOM_READ() == KEYPAD_PUSH_DOWN_LEVEL)
    {
        return _KEY_PUSH_BOTTOM;
    }
    
	return _KEY_PUSH_NONE;  // No key to push-down
}

/*----------------------------------------------------------------------
 *  KeypadInit
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void KeypadInit(u16 scan_period)
{	

    // Init the Var
	KeypadPush.key_last_push = _KEY_PUSH_NONE;
	KeypadPush.key_scan_period = scan_period;
	KeypadPush.read_idx = 0; KeypadPush.write_idx = 0;
	KeypadPush.key_scan_cnt = 0;
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

    KeypadPush.key_last_push = _KEY_PUSH_NONE;
	KeypadPush.read_idx = 0; KeypadPush.write_idx = 0;
	KeypadPush.key_scan_cnt = 0;
}

/*----------------------------------------------------------------------
 *  KeypadScan
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
_bool KeypadScan(KeyPushValueT *p_scan_key, u16 *cnt)
{					

	KeyPushValueT new_key_push = _KEY_PUSH_NONE;												

    // Scan Hw to Get the Key value
	new_key_push = portKeypadGetHwPort();

	// State one, just pull down the key
	if ((new_key_push != _KEY_PUSH_NONE) && (KeypadPush.key_last_push == _KEY_PUSH_NONE))	
	{
	
		KeypadPush.key_last_push = new_key_push;
		KeypadPush.key_scan_cnt = 1;
		
		return _false;
	}

	// State two, pulling-down the Key
	if ((new_key_push == KeypadPush.key_last_push) && (KeypadPush.key_last_push != _KEY_PUSH_NONE))
	{

	    // Time Add, 'key_scan_cnt' Must >= 1
	    if (KeypadPush.key_scan_cnt > 0)
	    {
            KeypadPush.key_scan_cnt++;
	    }

	    return _false;
	}
	
	// State three, release the key
	if ((new_key_push != KeypadPush.key_last_push) && (KeypadPush.key_last_push != _KEY_PUSH_NONE))
	{		
	
		*p_scan_key = KeypadPush.key_last_push; *cnt = KeypadPush.key_scan_cnt;

		// Reset the last push
		KeypadPush.key_last_push = _KEY_PUSH_NONE;
		KeypadPush.key_scan_cnt = 0;
		
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

	KeyPushValueT new_key_push = _KEY_PUSH_NONE; u16 time_cnt = 0;
	
    // Reset the Index to 0, to avoid overflow
	if (KeypadPush.read_idx == KeypadPush.write_idx)			   	
	{
		KeypadPush.read_idx = 0; KeypadPush.write_idx = 0;
	}

    // Scan keypad to get the new key-value
	if (KeypadScan(&new_key_push, &time_cnt) == _false) 
	{								 
		return;
	}
	else
	{
	    // Store to the Buffer
		KeypadPush.key_push_buffer[KeypadPush.write_idx].value = new_key_push;
		KeypadPush.key_push_buffer[KeypadPush.write_idx].key_push_cnt = time_cnt;
		KeypadPush.key_push_buffer[KeypadPush.write_idx].key_push_time = time_cnt * KeypadPush.key_scan_period;
		
		// To avoid the array overflow
		if (KeypadPush.write_idx < (_KEYPAD_BUFFER_SIZE - 1)) 
		{													 	
			KeypadPush.write_idx++;	
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
	    key_push->value = _KEY_PUSH_NONE;
		return _false;	
	}
	
	return _true;	
}


//------------------------------------------------------
//----------------End of file---------------------------
//------------------------------------------------------
