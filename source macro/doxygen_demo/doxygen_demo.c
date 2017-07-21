/*!
 ******************************************************************************
 * @file      doxygen_demo.c
 * @brief
 * @author
 * @date      2017/7/17
 * @version
 * @note
 ******************************************************************************
*/
 
 
/*!
 * @brief      XorCheckSum
 * @param[in]  None.
 * @param[out] None.
 * @return     None.
 * @date       2017/7/17
 * @note       None.
*/
u8 XorCheckSum(u8 *input, u16 length)
{
    u16 idx = 0; u8 xorResult = 0;
    
    // !! Initialize the reseut with the 'array[0]' !!
    xorResult = input[0];
    
    // X-OR Loop check
    for (idx = 1; idx < length; idx++)
    {        
        xorResult ^= input[idx];
    }
    
    return xorResult;
}
 

/*!
 * @brief
 */
