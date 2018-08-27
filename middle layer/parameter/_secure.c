
// -------------------------------------------------------------------
// [SECURE CODE]
// ------
// 96-bit ID -> f(x) -> 32-bit KEY
// -------------------------------------------------------------------

#include "_secure.h"

// Set KEY
// bytes_to_read: 8/12
unsigned int _secure_generate(unsigned char *id, unsigned char bytes_to_read)
{
    unsigned char idx_1 = 0,idx_2 = 0, group_cnt = bytes_to_read / 4, mid = 0;
    unsigned int KEY = 0;

    // KEY GEN
    for (idx_1 = 0; idx_1 < bytes_to_read; idx_1 += group_cnt)
    {
        mid = 0;
        for (idx_2 = 0; idx_2 < group_cnt; idx_2++)
        {
            mid ^= id[idx_1 + idx_2];
        }
        
        KEY <<= 8; KEY += mid;
    }
    
    return KEY;
}
