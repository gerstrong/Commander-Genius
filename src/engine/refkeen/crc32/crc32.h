#ifndef __CRC32_H__
#define __CRC32_H__

#include <stdio.h>
#include <stdint.h>

int Crc32_ComputeFile( FILE *file, uint32_t *outCrc32 );

#endif
