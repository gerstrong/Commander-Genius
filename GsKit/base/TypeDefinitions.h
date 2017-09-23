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

// Some good basic types

#include <SDL.h>

typedef uint8_t byte;
typedef uint16_t word;
typedef int32_t fixed;
typedef uint32_t longword;
typedef void * memptr;


/**
 * @brief GETWORD Extracts a word (16-bit integer) from a byte pointer.
 *                It endian independent, assuming we a reading data from
 *                a little endian system
 * @param ptr     Pointer to data
 * @return        The value
 */
static inline auto GETWORD(const byte *ptr) -> word
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    return (ptr[0]<<8 | ptr[1]);
#else
    return (ptr[0] | ptr[1] << 8);
#endif
}

/**
 * @brief READWORD Like GETWORD but the pointer will also be modified, by
 *                 going two bytes forward
 * @param ptr      reference to pointer (*&) which will be modified
 * @return         the value
 */
static inline auto READWORD(byte *&ptr) -> word
{
    const auto val = GETWORD(ptr);
    ptr += sizeof(word);
    return val;
}


/**
 * @brief GETLONGWORD   Like GETWORD but 4 bytes.
 * @param ptr           Pointer to data
 * @return              The value
 */
static inline auto GETLONGWORD(const byte *ptr) -> longword
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    longword val = ptr[0]<<24 | ptr[1] << 16 | ptr[2] << 8 | ptr[3];
#else
    longword val = ptr[0] | ptr[1] << 8 | ptr[2] << 16 | ptr[3] << 24;
#endif
    return val;
}


/**
 * @brief READLONGWORD  Like READWORD but 4 bytes
 * @param ptr      reference to pointer (*&) which will be modified
 * @return         the value
 */
static inline auto READLONGWORD(byte *&ptr) -> longword
{
    const auto val = GETLONGWORD(ptr);
    ptr += 4;
    return val;
}

/**
 * @brief MERGERLOFFSET Strange offset merger.
 *                      For some offsets, it helps detecting the pointer correctly.
 *                      It is used for level names basically
 * @param levelLongWord word to work on
 * @return tranformed value
 */
static inline auto MERGERLOFFSET(const unsigned long levelLongWord) ->  unsigned long
{
    const unsigned long part1 = levelLongWord >> 12;
    const unsigned long part2 = levelLongWord & 0xFFF;
    return part1 + part2;
};


#endif /* __TYPEDEFINITIONS_H__ */
