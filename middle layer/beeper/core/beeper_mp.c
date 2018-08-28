
/*----------------------------------------------------------------------^^-
/ File name:  beeper_mp.c
/ Author:     JiangJun
/ Data:       2018/3/22
/ Version:    v1.0
/-----------------------------------------------------------------------^^-
/ Music Player by Beeper driver
/------------------------------------------------------------------------*/

#include "beeper_mp.h"
#include "beeper.h"

//------------------------------------------------------------
// STATIC Var
//------------------------------------------------------------

static _MusicMpT MusicMp; static _MusicScoreT MusicScore;


/*----------------------------------------------------------------------
 *  BeeperMpInit
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void BeeperMpInit(u16 proc_time)
{

    // Init Hw
    BeeperInit();

    // Init Var
    if (proc_time != 0)
    {
        MusicMp.proc_time = proc_time;
    }        

    MusicMp.w_idx = 0; MusicMp.r_idx = 0;
    MusicScore.note_cnt = 0; MusicScore.note_mp_idx = 0;
    MusicScore.duran_timer = 0; MusicScore.voice_gain = 0; MusicScore.p_notes = NULL;
}

/*----------------------------------------------------------------------
 *  BeeperMpTimeProc
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void BeeperMpTimeProc(void)
{

    // Check Proc Time
    if (MusicMp.proc_time == 0)
    {
        return;
    }

    //------------------------------------------------------------
    // Check if Play OK
    //------------------------------------------------------------
    if (MusicScore.note_mp_idx < MusicScore.note_cnt)
    {

        // Check if duartion is OK
        if (MusicScore.duran_timer >= (u16)MusicScore.p_notes->duran)
        {            
            MusicScore.p_notes++;
            MusicScore.note_mp_idx++;
            MusicScore.duran_timer = 0;
        }

        // Play the notes
        if (MusicScore.note_mp_idx < MusicScore.note_cnt)
        {
            // Timer Add
            MusicScore.duran_timer += MusicMp.proc_time;    

            // Play the notes
            BeeperSet((u16)MusicScore.p_notes->tone, MusicScore.voice_gain);            
        }
        else
        {
            // Stop beeper
            BeeperStop();
        }    

        return;
    }

    // Reset current score
    MusicScore.note_mp_idx = 0; MusicScore.note_cnt = 0; MusicScore.duran_timer = 0; MusicScore.voice_gain = 0;    

    // Stop beeper
    BeeperStop();

    //------------------------------------------------------------
    // Check if Have Next Score
    //------------------------------------------------------------
    if (MusicMp.r_idx >= MusicMp.w_idx)
    {
        MusicMp.r_idx = 0; MusicMp.w_idx = 0;
    }
    else
    {
        MusicScore = MusicMp.p_score_queue[MusicMp.r_idx++];
    }
}

/*----------------------------------------------------------------------
 *  BeeperMpSetScore
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void BeeperMpSetScore(_MusicNotesT *p_note, u16 note_cnt, u8 voice_gain)
{

    if (note_cnt != 0)
    {
        MusicMp.p_score_queue[MusicMp.w_idx].p_notes = p_note;
        MusicMp.p_score_queue[MusicMp.w_idx].note_cnt = note_cnt;
        MusicMp.p_score_queue[MusicMp.w_idx].note_mp_idx = 0;
        MusicMp.p_score_queue[MusicMp.w_idx].duran_timer = 0;
        MusicMp.p_score_queue[MusicMp.w_idx].voice_gain = voice_gain;

        // Index
        if (MusicMp.w_idx < (_MP_MAX_SCORE_SIZE - 1))
        {
            MusicMp.w_idx++;
        }        
    }
}

/*----------------------------------------------------------------------
 *  BeeperMpPlay
 *
 *  Purpose: None.
 *  Entry:   None.
 *  Exit:    None.
 *  NOTE:    None.
 *---------------------------------------------------------------------*/
void BeeperMpPlay(_MusicalToneT tone, u8 voice_gain)
{

    // 0-100
    voice_gain %= 101;
    BeeperSet((u16)tone, voice_gain);
}

//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
