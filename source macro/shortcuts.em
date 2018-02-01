
/*------------------------------------------------------^-^/
/ File name:  shortcuts.em
/ Author:     JiangJun
/ Data:       2018/01/30
/ Version:    V1.1
/-------------------------------------------------------^-^/
/ Shortcuts withe the source insight
/---------------------------------------------------------*/

/*------------------------------------------------------------
 *  Function Names:     macro_get_file_name()
 *  Parameter:          None
 *  Return Value:       None
 *  Remarks:            return _XXX_X(upper)
 *------------------------------------------------------------*/
macro macro_get_file_name()
{
	var file_name;
	var file_extension;

	hbuf = GetCurrentBuf();
	if (hbuf == hnil) {
	    return;
	}

	/* Get file name, include extension */
	name = GetBufName(hbuf);

	index = "0";

	/* Get file name */
	file_name = nil;
	while (name[index] != ".")
	{
	    file_name = file_name # name[index];
	    index++;
	}

	/* Get extension */
	file_extension = nil;
	while (name[index] != nil)
	{
	    if (name[index] != ".")
	        file_extension= file_extension # name[index];
	    index++;
	}

	/* Convert to upper */
	file_name = ToUpper(file_name);
	file_extension = ToUpper(file_extension);

	return ("_" # file_name # "_" # file_extension);
}

/*------------------------------------------------------------
 *  Function Names:     macro_get_func_name()
 *  Parameter:          None
 *  Return Value:       None
 *  Remarks:            void main(void) function will return
 *                      main.
 *------------------------------------------------------------*/
macro macro_get_func_name(line_str)
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
 *  Function Names:     macro_get_date()
 *  Parameter:          None
 *  Return Value:       None
 *  Remarks:            None
 *------------------------------------------------------------*/
macro macro_get_date()
{
    var year;
    var month;
    var day;

    // Get local time
    system = GetSysTime(1);
    year = system.year;
    month = system.month;
    day = system.day;    

   	return ("@year@/@month@/@day@");
}
 
/*------------------------------------------------------------
 *  Function Names:     Insrt_ver_date()
 *  Parameter:          None
 *  Return Value:       None
 *  Remarks:            None
 *------------------------------------------------------------*/
macro Insrt_ver_date()
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
 *  Function Names:     Insrt_file_end()
 *  Parameter:          None
 *  Return Value:       None
 *  Remarks:            Put the cursor in anywhere.
 *------------------------------------------------------------*/
macro Insrt_file_end()
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

/*------------------------------------------------------------
 *  Function Names:     Insrt_ifdef()
 *  Parameter:          None
 *  Return Value:       None
 *  Remarks:            Put the cursor in anywhere.
 *------------------------------------------------------------*/
macro Insrt_ifdef()
{
    /* File buffer handler */
    var hbuf;
    var line_number;
    var name;

    /* Get the handler to current file,
       this file is the most-front file,
       if has no file open, return nil. */
    hbuf = GetCurrentBuf();
    if (hbuf == nil)
    {
        return;
    }

    /* Get the count of the file line. */
	line_number = GetBufLnCur(hbuf)

	/* Insert #idndef/ #define/ #endif */
	if (line_number == 0)
	{
		name = macro_get_file_name();		
	}
	else
	{
		name = "_XXX_H";
	}

	InsBufLine(hbuf, line_number, 		"#ifndef " # name);
	InsBufLine(hbuf, line_number + 1, 	"#define " # name);
	InsBufLine(hbuf, line_number + 2, 	"");
	InsBufLine(hbuf, line_number + 3, 	"#endif");    
}

/*------------------------------------------------------------
 *  Function Names:     Insrt_func_header()
 *  Parameter:          None
 *  Return Value:       None
 *  Remarks:            Put the cursor in anywhere.
 *------------------------------------------------------------*/
macro Insrt_func_header()
{
    /* File buffer handler */
    var hbuf;
    var line_number, line_text;
    var func_name;

    /* Get the handler to current file,
       this file is the most-front file,
       if has no file open, return nil. */
    hbuf = GetCurrentBuf();
    if (hbuf == nil)
    {
        return;
    }

    /* Get the count of the file line. */
	line_number = GetBufLnCur(hbuf)

	/* Get the function name */
	line_text = GetBufLine(hbuf, line_number + 1);		// the next line
	func_name = macro_get_func_name(line_text);
	
	/* Insert Function Header */
	InsBufLine(hbuf, line_number + 0, "/*----------------------------------------------------------------------");
    InsBufLine(hbuf, line_number + 1, " *  @func_name@" );
    InsBufLine(hbuf, line_number + 2, " *");
    InsBufLine(hbuf, line_number + 3, " *  Purpose: None.");
    InsBufLine(hbuf, line_number + 4, " *  Entry:   None.");
    InsBufLine(hbuf, line_number + 5, " *  Exit:    None.");
    InsBufLine(hbuf, line_number + 6, " *  NOTE:    None.");
    InsBufLine(hbuf, line_number + 7, " *---------------------------------------------------------------------*/");
    DelBufLine(hbuf, line_number + 8);
}

/*------------------------------------------------------------
 *  Function Names:     Insrt_file_header()
 *  Parameter:          None
 *  Return Value:       None
 *  Remarks:            Put the cursor in anywhere.
 *------------------------------------------------------------*/
macro Insrt_file_header()
{
    /* File buffer handler */
    var hbuf;
    var line_number;
    var file_name, date;

    /* Get the handler to current file,
       this file is the most-front file,
       if has no file open, return nil. */
    hbuf = GetCurrentBuf();
    if (hbuf == nil)
    {
        return;
    }

    /* Get the count of the file line. */
	line_number = 0;

	/* Get the file name */
	file_name = GetBufName(hbuf);
	date = macro_get_date();	
	
	/* Insert Function Header */
	InsBufLine(hbuf, line_number + 0, "/*----------------------------------------------------------------------^^-");
    InsBufLine(hbuf, line_number + 1, "/ File name:  @file_name@");
    InsBufLine(hbuf, line_number + 2, "/ Author:     JiangJun");
    InsBufLine(hbuf, line_number + 3, "/ Data:       @date@");
    InsBufLine(hbuf, line_number + 4, "/ Version:    v1.0");
    InsBufLine(hbuf, line_number + 5, "/-----------------------------------------------------------------------^^-");
    InsBufLine(hbuf, line_number + 6, "/");
    InsBufLine(hbuf, line_number + 7, "/------------------------------------------------------------------------*/");
    DelBufLine(hbuf, line_number + 8);
}

//--------------------------------------------------
//---------------- End of file ---------------------
//--------------------------------------------------
