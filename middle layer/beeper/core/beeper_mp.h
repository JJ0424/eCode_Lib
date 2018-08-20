
/*----------------------------------------------------------------------^^-
/ File name:  beeper_mp.h
/ Author:     JiangJun
/ Data:       2018/3/22
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ Music Player by Beeper driver
/------------------------------------------------------------------------*/

#ifndef _BEEPER_MP_H
#define _BEEPER_MP_H

#include "main.h"

//------------------------------------------------------------
// CONFIG
//------------------------------------------------------------

#define _MP_MAX_SCORE_SIZE      10      // Max Store 10 Score

//------------------------------------------------------------
// Music Notes Typedef
//------------------------------------------------------------

// Musical Tone
typedef enum {

    _MT_REST = 0,
    _MT_C = 262,
    _MT_Cs = 277,
    _MT_D = 294,
    _MT_Ds = 311,
    _MT_E = 330,
    _MT_F = 349,
    _MT_Fs = 370,
    _MT_G = 392,
    _MT_Gs = 415,
    _MT_A = 440,
    _MT_As = 466,
    _MT_B = 494,
    _MT_hC = 523,
    _MT_hCs = 554,
    _MT_hD = 587,
    _MT_hDs = 622,
    _MT_hE = 659,
    _MT_hF = 698,
    _MT_hFs = 740,
    _MT_hG = 784,
    _MT_hGs = 831,
    _MT_hA = 880,
    _MT_hAs = 932,
    _MT_hB = 988,
    
} _MusicalToneT;

// Musical Duration
typedef enum {

    _MD_WHOLE = 800,
    _MD_HALF = 400,
    _MD_QUARTER = 200,

    _MD_WHOLE_QUARTER = (_MD_WHOLE + _MD_QUARTER),
    
} _MusicalDurationT;

// Musical Notes
typedef struct {

    _MusicalToneT tone;         // Tone
    _MusicalDurationT duran;    // Duration
    
} _MusicNotesT;


//------------------------------------------------------------
// Music Score Typedef
//------------------------------------------------------------

// Musical Score
typedef struct {

    _MusicNotesT *p_notes;  // the point of current score notes
    u16 note_cnt;           // the count of tocal score
    u16 note_mp_idx;        // the index notes of playing
    u16 duran_timer;        // the timer of duration
    u8 voice_gain;          // 0 - 100 voice gain

} _MusicScoreT;

// Musical Player struct
typedef struct {

    u16 proc_time;      // the period of call 'BeeperMpTimeProc'
    u8 w_idx, r_idx;
    _MusicScoreT p_score_queue[_MP_MAX_SCORE_SIZE];  // Buffer to store Score Points
        
} _MusicMpT;


extern void BeeperMpInit(u16 proc_time);
extern void BeeperMpTimeProc(void);
extern void BeeperMpSetScore(_MusicNotesT *p_note, u16 note_cnt, u8 voice_gain);

#endif

