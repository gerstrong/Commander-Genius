/*
 * TyoeDefinitions.h
 *
 *  Created on: 30.05.2010
 *      Author: gerstrong
 *
 *
 */

#ifndef TYPEDEFINITIONS_H
#define TYPEDEFINITIONS_H

#include <sys/types.h>

#include <SDL.h>

// Some good basic types and utilities independent of endianess.
typedef uint8_t gs_byte;
typedef uint8_t Uint8;
typedef uint8_t uint8_t;
typedef int8_t Sint8;

typedef uint16_t word;
typedef uint16_t Uint16;
typedef uint16_t uint16_t;

typedef int32_t fixed;
typedef uint32_t longword;
typedef uint32_t uint32_t;


typedef void * memptr;


/**
 * @brief GETWORD Extracts a word (16-bit integer) from a byte pointer.
 *                It is endian independent, assuming we a reading data from
 *                a little endian system
 * @param ptr     Pointer to data
 * @return        The value
 */
auto GETWORD(const gs_byte *ptr) -> word;

/**
 * @brief READWORD Like GETWORD but the pointer will also be modified, by
 *                 going two bytes forward
 * @param ptr      reference to pointer (*&) which will be modified
 * @return         the value
 */
auto READWORD(gs_byte *&ptr) -> word;

/**
 * @brief GETLONGWORD   Like GETWORD but 4 bytes.
 * @param ptr           Pointer to data
 * @return              The value
 */
auto GETLONGWORD(const gs_byte *ptr) -> longword;


/**
 * @brief READLONGWORD  Like READWORD but 4 bytes
 * @param ptr      reference to pointer (*&) which will be modified
 * @return         the value
 */
auto READLONGWORD(gs_byte *&ptr) -> longword;

/**
 * @brief MERGERLOFFSET Strange offset merger.
 *                      For some offsets, it helps detecting the pointer correctly.
 *                      It is used for level names basically
 * @param levelLongWord word to work on
 * @return tranformed value
 */
auto MERGERLOFFSET(const unsigned long levelLongWord) ->  unsigned long;


#endif /* TYPEDEFINITIONS_H */
