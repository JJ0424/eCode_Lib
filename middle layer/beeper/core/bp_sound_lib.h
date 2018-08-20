
/*----------------------------------------------------------------------^^-
/ File name:  bp_sound_lib.h
/ Author:     JiangJun
/ Data:       2018/3/23
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ Beeper Sound Lib and Algorithm
/------------------------------------------------------------------------*/

#ifndef _BP_SOUND_LIB_H
#define _BP_SOUND_LIB_H

#include "main.h"
#include "beeper_mp.h"

// 'BpSndLibScanGen' struct
typedef struct {

    // Tone
    u16 base;
    u16 interval;
    u16 cnt;

    // Duration
    u16 duration;
    
} BpSndScanT;

// 'BpSndLibLoopGen' struct
typedef struct {

    u16 freq;
    u16 duran_on, duran_off;
    u16 period_cnt;             // (ON + OFF) as a period
    
} BpSndLoopT;

extern u16 BpSndLibScanGen(_MusicNotesT *score_buf, u16 cnt, BpSndScanT *scan);
extern u16 BpSndLibLoopGen(_MusicNotesT *score_buf, u16 cnt, BpSndLoopT *loop);

#endif

