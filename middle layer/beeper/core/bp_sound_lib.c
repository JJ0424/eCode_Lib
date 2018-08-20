
/*----------------------------------------------------------------------^^-
/ File name:  bp_sound_lib.c
/ Author:     JiangJun
/ Data:       2018/3/23
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ Beeper Sound Lib and Algorithm
/------------------------------------------------------------------------*/

#include "bp_sound_lib.h"


/*----------------------------------------------------------------------
 *  BpSndLibScanGen
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u16 BpSndLibScanGen(_MusicNotesT *score_buf, u16 cnt, BpSndScanT *scan)
{

    u16 idx = 0; BpSndScanT scan_info = *scan;

    // Gen
    for (idx = 0; (idx < cnt) && (idx < scan_info.cnt); idx++)
    {
        score_buf->tone = (_MusicalToneT)(scan_info.base + (idx * scan_info.interval));
        score_buf->duran = (_MusicalDurationT)scan_info.duration;
        score_buf++;
    }

    return idx;
}

/*----------------------------------------------------------------------
 *  BpSndLibLoopGen
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
u16 BpSndLibLoopGen(_MusicNotesT *score_buf, u16 cnt, BpSndLoopT *loop)
{

    u16 idx = 0; BpSndLoopT loop_info = *loop; u16 tone_cnt = loop_info.period_cnt << 1;

    // Gen
    for (idx = 0; (idx < cnt) && (idx < tone_cnt); idx++)
    {
        if ((idx % 2) == 0)
        {
            score_buf->tone = (_MusicalToneT)loop_info.freq;  
            score_buf->duran = (_MusicalDurationT)loop_info.duran_on;
        }
        else
        {
            score_buf->tone = _MT_REST;  
            score_buf->duran = (_MusicalDurationT)loop_info.duran_off;
        }
           
        score_buf++;
    }

    return idx;
}
