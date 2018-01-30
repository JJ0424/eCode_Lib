
/*------------------------------------------------------^-^/
/ File name:  shortcuts.em
/ Author:     JiangJun
/ Data:       2017/08/30
/ Version:    V1.0.0
/-------------------------------------------------------^-^/
/ Shortcuts withe the source insight
/---------------------------------------------------------*/

/*------------------------------------------------------------
 *  Function Names:     InsrtDate()
 *  Parameter:          None
 *  Return Value:       None
 *  Remarks:            None
 *------------------------------------------------------------*/
macro InsrtDate()
{
    var file_name;
    var year;
    var month;
    var day;

    // Get File Buf
    hbuf = GetCurrentBuf();
    if (hbuf == nil) { return; }

    // Get local time
    system = GetSysTime(1);
    year = system.year;
    month = system.month;
    day = system.day;    

   	SetBufSelText(hbuf, "v0.xx [@year@-@month@-@day@]");
}


/*------------------------------------------------------------
 *  Function Names:     InsrtFileEnd()
 *  Parameter:          None
 *  Return Value:       None
 *  Remarks:            Put the cursor in anywhere.
 *------------------------------------------------------------*/
macro InsrtFileEnd()
{
    /* File buffer handler */
    var hbuf;
    var line_number;

    /* Get the handler to current file,
       this file is the most-front file,
       if has no file open, return nil. */
    hbuf = GetCurrentBuf();
    if (hbuf == nil)
    {
        return;
    }

    /* Get the count of the file line. */
    line_number = GetBufLineCount(hbuf);

    /* Insert String */
    InsBufLine(hbuf, line_number + 0, "//---------------------------------------------------------------------------//");
    InsBufLine(hbuf, line_number + 1, "//----------------------------- END OF FILE ---------------------------------//");
    InsBufLine(hbuf, line_number + 2, "//---------------------------------------------------------------------------//");
}

//--------------------------------------------------
//---------------- End of file ---------------------
//--------------------------------------------------
