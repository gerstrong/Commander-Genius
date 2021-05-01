#include "TypeDefinitions.h"
#include <SDL.h>


auto GETWORD(const gs_byte *ptr) -> word
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
auto READWORD(gs_byte *&ptr) -> word
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
auto GETLONGWORD(const gs_byte *ptr) -> longword
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
auto READLONGWORD(gs_byte *&ptr) -> longword
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
auto MERGERLOFFSET(const unsigned long levelLongWord) ->  unsigned long
{
    const unsigned long part1 = levelLongWord >> 12;
    const unsigned long part2 = levelLongWord & 0xFFF;
    return part1 + part2;
};


