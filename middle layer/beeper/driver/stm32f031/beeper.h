
/*----------------------------------------------------------------------^^-
/ File name:  beeper.h
/ Author:     JiangJun
/ Data:       2018/3/22
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ Beeper Common Driver
/------------------------------------------------------------------------*/

#ifndef _BEEPER_H
#define _BEEPER_H

#include "main.h"

//------------------------------------------------------------
// CONFIG
//------------------------------------------------------------

#define _BEEPER_DEF_TIMER_CLK_HZ        1000000
#define _BEEPER_DEF_VOICE_FREQ          1000        // 1KHz

#define _BEEPER_DEF_PWM_LEVEL           0           // To OFF state

extern void BeeperInit(void);
extern void BeeperSet(u16 freq, u8 gain);
extern void BeeperStop(void);

#endif

