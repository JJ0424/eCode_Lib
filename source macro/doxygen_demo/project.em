/*------------------------------------------------------^-^/
/ File name:  project.em
/ Author:     JiangJun
/ Data:       2017/07/17
/ Version:    V1.0.0
/-------------------------------------------------------^-^/
/ The Comments and Operations of C Language Project.
/---------------
/ This file include two kinds of macros. The one is Comment
/ macro, and the two is operation macro(#ifndef, #endif...).
/---------------------------------------------------------*/

/*------------------------------------------------------------
 *  Function Names:     GetFileName()
 *  Parameter:          None
 *  Return Value:       None
 *  Remarks:             c:\aa\bb\cc.c will return cc.c as file
 *                      name.
 *------------------------------------------------------------*/
macro GetFileName(path_name)
{
    var length;
    var return_value;

    return_value = nil;
    length = strlen(path_name);
    while (length > 0)
    {
        char = path_name[length - 1];
        if (char != "\\")
        {
            return_value = char # return_value;
        }
        else
        {
            return return_value;
        }
        length--;
    }
    return return_value;
}

/*------------------------------------------------------------
 *  Function Names:     GetFunctionName()
 *  Parameter:          None
 *  Return Value:       None
 *  Remarks:            void main(void) function will return
 *                      main.
 *------------------------------------------------------------*/
macro GetFunctionName(line_str)
{
    var fun_name;
    var record;

    record = 0;
    index = 0;
    fun_name = nil;
    length = strlen(line_str);
    while (index < length)
    {
        char = strmid(line_str, index, index + 1);

        /* Detect the name start position. */
        if ((record == 0) && (char != " "))
        {
            index++;
            continue;
        }

        record = 1;
        /* Detect the name end position. */
        if (record == 1)
        {
            if (char == "(")
            {
                return fun_name;
            }
        }

        /* record functio name */
        if (char != " ")
            fun_name = fun_name # char;
        index++;
    }
}

/*------------------------------------------------------------
 *  Function Names:     DoxygenInsFileHeader()
 *  Parameter:          None
 *  Return Value:       None
 *  Remarks:             Put the cursro at anywhere of the file,
 *                      invoke CTRL+ALT+F to insert project
 *                      header.
 *------------------------------------------------------------*/
macro DoxygenInsFileHeader()
{
    var file_name;
    var year;
    var month;
    var day;

    /* Get the handler to current file,
       this file is the most-front file,
       if has no file open, return nil. */
    hbuf = GetCurrentBuf();
    if (hbuf == nil)
    {
        return;
    }

    /* Get local time */
    system = GetSysTime(1);
    year = system.year;
    month = system.month;
    day = system.day;

    /* Get file name */
    buf_name = GetBufName(hbuf);
    file_name = GetFileName(buf_name);

    /* Insert String */
    InsBufLine(hBuf, 0, "/*!");
    InsBufLine(hBuf, 1, " ******************************************************************************");
    InsBufLine(hBuf, 2, " * @@file      @file_name@");
    InsBufLine(hBuf, 3, " * @@brief");
    InsBufLine(hBuf, 4, " * @@author");
    InsBufLine(hBuf, 5, " * @@date      @year@/@month@/@day@");
    InsBufLine(hBuf, 6, " * @@version");
    InsBufLine(hBuf, 7, " * @@note");
    InsBufLine(hBuf, 8, " ******************************************************************************");
    InsBufLine(hBuf, 9, "*/");
    InsBufLine(hBuf, 10, " ");
}

/*------------------------------------------------------------
 *  Function Names:     DoxygenInsFunctionHeader()
 *  Parameter:          None
 *  Return Value:       None
 *  Remarks:             Put the cursor in a empty line which
 *                      under the line of the function name.
 *                       And this macro will write parameter with
 *                      the next line of cursor.
 *------------------------------------------------------------*/
macro DoxygenInsFunctionHeader()
{
    /* File buffer handler */
    var hbuf;
    var cursor_line;
    var function_name;
    var line;
    var year;
    var month;
    var day;

    /* Get the handler to current file,
       this file is the most-front file,
       if has no file open, return nil. */
    hbuf = GetCurrentBuf();
    if (hbuf == nil)
    {
        return;
    }

    /* Get cursor line number */
    cursor_line = GetBufLnCur(hbuf);
    line = GetBufLine(hbuf, cursor_line + 1);

    /* Get function name. */
    function_name = GetFunctionName(line);

    /* Get local time */
    system = GetSysTime(1);
    year = system.year;
    month = system.month;
    day = system.day;

    /* Insert String */
    InsBufLine(hbuf, cursor_line + 0, "/*!");
    InsBufLine(hbuf, cursor_line + 1, " * @@brief      @function_name@");
    InsBufLine(hbuf, cursor_line + 2, " * @@param[in]  None.");
    InsBufLine(hbuf, cursor_line + 3, " * @@param[out] None.");
    InsBufLine(hbuf, cursor_line + 4, " * @@return     None.");
    InsBufLine(hbuf, cursor_line + 5, " * @@date       @year@/@month@/@day@");
    InsBufLine(hbuf, cursor_line + 6, " * @@note       None.");
    InsBufLine(hbuf, cursor_line + 7, "*/");
    DelBufLine(hbuf, cursor_line + 8);
}




/*------------------------------------------------------------
 *  Function Names:     DoxygenInsInsCommentBlock()
 *  Parameter:          None
 *  Return Value:       None
 *  Remarks:            Put the cursor in a empty line, or the
 *                      contents of the line will killed.
 *------------------------------------------------------------*/
macro DoxygenInsCommentBlock()
{
    /* File buffer handler */
    var hbuf;
    var cursor_line;

    /* Get the handler to current file,
       this file is the most-front file,
       if has no file open, return nil. */
    hbuf = GetCurrentBuf();
    if (hbuf == nil)
    {
        return;
    }

    /* Get cursor line number */
    cursor_line = GetBufLnCur(hbuf);

    /* Insert String */
    InsBufLine(hbuf, cursor_line + 0, "/*!");
    InsBufLine(hbuf, cursor_line + 1, " * @@brief");
    InsBufLine(hbuf, cursor_line + 2, " */");

    DelBufLine(hbuf, cursor_line + 3);
}

/*------------------------------------------------------------
 *  Function Names:     InsEndFile()
 *  Parameter:          None
 *  Return Value:       None
 *  Remarks:            Put the cursor in anywhere.
 *------------------------------------------------------------*/
macro InsEndFile()
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
