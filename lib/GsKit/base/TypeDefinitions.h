/*
 * TyoeDefinitions.h
 *
 *  Created on: 30.05.2010
 *      Author: gerstrong
 *
 *
 */

#ifndef __TYPEDEFINITIONS_H__
#define __TYPEDEFINITIONS_H__

#include <SDL.h>

typedef uint8_t byte;
typedef uint16_t word;
typedef int32_t fixed;
typedef uint32_t longword;
typedef void * memptr;

static inline word READWORD(byte *&ptr)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	word val = ptr[0]<<8 | ptr[1];
#else
	word val = ptr[0] | ptr[1] << 8;
#endif
    ptr += sizeof(word);
    return val;
}

static inline word GETWORD(byte *ptr){
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	return (ptr[0]<<8 | ptr[1]);
#else
	return (ptr[0] | ptr[1] << 8);
#endif
}

static inline longword READLONGWORD(byte *&ptr)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	longword val = ptr[0]<<24 | ptr[1] << 16 | ptr[2] << 8 | ptr[3];
#else
	longword val = ptr[0] | ptr[1] << 8 | ptr[2] << 16 | ptr[3] << 24;
#endif
    ptr += 4;
    return val;
}


#endif /* __TYPEDEFINITIONS_H__ */
