/*
 * TyoeDefinitions.h
 *
 *  Created on: 30.05.2010
 *      Author: gerstrong
 *
 *
 */

#ifndef TYOEDEFINITIONS_H_
#define TYOEDEFINITIONS_H_

#include <SDL.h>

typedef uint8_t byte;
typedef uint16_t word;
typedef int32_t fixed;
typedef uint32_t longword;
typedef void * memptr;

static inline word READWORD(byte *&ptr)
{
    word val = ptr[0] | ptr[1] << 8;
    ptr += 2;
    return val;
}

static inline word GETWORD(byte *ptr){
	return (ptr[0] | ptr[1] << 8);
}

static inline longword READLONGWORD(byte *&ptr)
{
    longword val = ptr[0] | ptr[1] << 8 | ptr[2] << 16 | ptr[3] << 24;
    ptr += 4;
    return val;
}


#endif /* TYOEDEFINITIONS_H_ */
