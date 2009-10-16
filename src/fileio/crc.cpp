// This code is copyright © 1993 Richard Black. All rights are reserved. You may use this code only if it includes a statement to that effect.
// http://www.cl.cam.ac.uk/research/srg/bluebook/21/crc/node6.html

#include "crc.h"

#define LITTLE_ENDIAN 1
#define QUOTIENT 0x04C11DB7

unsigned int crctab[256];

void crc32_init(void)
{
    int i,j;

    unsigned int crc;

    for (i = 0; i < 256; i++)
    {
        crc = i << 24;
        for (j = 0; j < 8; j++)
        {
            if (crc & 0x80000000)
                crc = (crc << 1) ^ QUOTIENT;
            else
                crc = crc << 1;
        }
        crctab[i] = crc; // may need to account for big endian
    }
}

unsigned int getcrc32(unsigned char *data, int len)
{
    unsigned int        result;
    unsigned int        *p = (unsigned int *)data;
    unsigned int        *e = (unsigned int *)(data + len);

//    if (len < 4) abort();

    result = ~*p++;
    while( p<e )
    {
#if defined(LITTLE_ENDIAN)
        result = crctab[result & 0xff] ^ result >> 8;
        result = crctab[result & 0xff] ^ result >> 8;
        result = crctab[result & 0xff] ^ result >> 8;
        result = crctab[result & 0xff] ^ result >> 8;
        result ^= *p++;
#else
        result = crctab[result >> 24] ^ result << 8;
        result = crctab[result >> 24] ^ result << 8;
        result = crctab[result >> 24] ^ result << 8;
        result = crctab[result >> 24] ^ result << 8;
        result ^= *p++;
#endif
    }

    return ~result;
}
