
/*----------------------------------------------------------------------^^-
/ File name:  beeper.c
/ Author:     JiangJun
/ Data:       2018/3/22
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ Beeper Common Driver
/------------------------------------------------------------------------*/

#include "beeper.h"
#include "hconfig.h"

/*----------------------------------------------------------------------
 *  BeeperInit
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void BeeperInit(void)
{
    u16 prescaler = 0;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* computer prescaler value. the counter clock
	   source must be 1MHZ.						*/
	prescaler = (u16)((SystemCoreClock / _BEEPER_DEF_TIMER_CLK_HZ) - 1);
	
	// Timer Base Init/ 1MHz
	TIM_TimeBaseStructure.TIM_Period = (_BEEPER_DEF_TIMER_CLK_HZ / _BEEPER_DEF_VOICE_FREQ);
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(BEEPER_USING_TIMER, &TIM_TimeBaseStructure);

	// PWM mode set.
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;

#if (_BEEPER_DEF_PWM_LEVEL == 0)
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   
#else
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  
#endif

    // Output Channel Init
	BEEPER_PWM_OC_INIT(BEEPER_USING_TIMER, &TIM_OCInitStructure);
	BEEPER_PWM_PRELOAD_CONFIG(BEEPER_USING_TIMER, TIM_OCPreload_Enable);	

	// Enable Autoload Preload
	TIM_ARRPreloadConfig(BEEPER_USING_TIMER, ENABLE);
		
	// Enable Timer
	TIM_Cmd(BEEPER_USING_TIMER, ENABLE);
	
	if ((u32)BEEPER_USING_TIMER == (u32)TIM1) { TIM_CtrlPWMOutputs(TIM1, ENABLE); }
}

/*----------------------------------------------------------------------
 *  BeeperSet - Set Frequency and Voice Gain, than Open PWM
 *
 *  Purpose: None.
 *
 *  Entry:   freq: 0 - 65535Hz
 *           gain: 0 - 100
 *
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void BeeperSet(u16 freq, u8 gain)
{

    u16 auto_relod = 0, compare = 0;

    // Freq: 0Hz
    if (freq == 0)
    {
        BEEPER_PWM_SET_VAL(0);

        return; 
    }
    
    // Set the Frequency
    auto_relod = _BEEPER_DEF_TIMER_CLK_HZ / freq;
    TIM_SetAutoreload(BEEPER_USING_TIMER, auto_relod - 1);
    
    // Set the Duty-Cycle
    compare = ((auto_relod * gain) / 100) >> 1;
    BEEPER_PWM_SET_VAL(compare);
}

/*----------------------------------------------------------------------
 *  BeepStop - Stop PWM Output
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void BeeperStop(void)
{
    BEEPER_PWM_SET_VAL(0);
}

