
// -------------------------------------------------------------------
// [SECURE CODE]
// ------
// 96-bit ID -> f(x) -> 32-bit KEY
// -------------------------------------------------------------------

#ifndef _SECURE_H
#define _SECURE_H

extern unsigned int _secure_generate(unsigned char *id, unsigned char bytes_to_read);

#endif