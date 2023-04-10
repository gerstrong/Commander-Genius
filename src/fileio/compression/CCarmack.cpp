/*
 * CCarmack.cpp
 *
 *  Created on: 30.05.2010
 *      Author: gerstrong
 */

#include "CCarmack.h"
#include <base/GsLogging.h>
#include <base/interface/StringUtils.h>
#include <base/TypeDefinitions.h>

#define WORDSIZE    2

#define CARMACK_NEARTAG 0xA700
#define CARMACK_FARTAG 0xA800

static void CarmackExpand(void *src, void *dest, int expLength)
{
    uint16_t *srcptr = (uint16_t *)src;
    uint16_t *dstptr = (uint16_t *)dest;
    uint16_t *runptr;
    uint16_t ch, count, offset;
    expLength /= WORDSIZE; // Number of words to deal with

    srcptr++;

    while (expLength > 0)
    {
        ch = GETWORD(reinterpret_cast<gs_byte*>(srcptr));
        srcptr++;

        const auto tag = ch & 0xFF00;
        count = ch & 0xFF;

        if (tag == CARMACK_NEARTAG)
        {
            if (!count)
            {
                // Makes an output starting with A7
                ch &= 0xFF00;
                ch |= GETBYTE(reinterpret_cast<gs_byte*>(srcptr));

                uint8_t *srcptr_8 = (uint8_t *)(srcptr) + 1;
                memcpy(&srcptr, &srcptr_8, sizeof(uint8_t *));

                *(dstptr++) = ch;
                expLength--;
            }
            else
            {
                offset = GETBYTE(reinterpret_cast<gs_byte*>(srcptr));

                uint8_t *srcptr_8 = (uint8_t *)(srcptr) + 1;
                memcpy(&srcptr, &srcptr_8, sizeof(uint8_t *));

                runptr = dstptr - offset;
                expLength -= count;
                while (count--)
                    *(dstptr++) = *(runptr++);
            }
        }
        else if (tag == CARMACK_FARTAG)
        {
            if (!count)
            {
                // Makes an output starting with A8
                ch &= 0xFF00;
                ch |= GETBYTE(reinterpret_cast<gs_byte*>(srcptr));

                uint8_t *srcptr_8 = (uint8_t *)(srcptr) + 1;
                memcpy(&srcptr, &srcptr_8, sizeof(uint8_t *));

                *(dstptr++) = ch;
                expLength--;
            }
            else
            {
                offset = GETWORD(reinterpret_cast<gs_byte*>(srcptr));
                srcptr++;
                runptr = (uint16_t *)dest + offset;;
                expLength -= count;
                while (count--)
                    *(dstptr++) = *(runptr++);
            }
        }
        else
        {
            *(dstptr++) = ch;
            --expLength;
        }
    }
}

/**
 * \brief			This function expands data using carmack
 * 					decompression algorithm
 * \param	source	pointer to the data that is compressed
 * \param	dest	pointer to the data that will be the output
 * \param	length	length of the EXPANDED data
 */
void CCarmack::expand( std::vector<gs_byte>& dst,
                       std::vector<gs_byte>& src,
                       const size_t decarmacksize)
{
    dst.assign(decarmacksize, 0);
    void *srcRaw = src.data();
    void *destRaw = dst.data();

    CarmackExpand(srcRaw, destRaw, decarmacksize);

    for(decltype(dst.size()) i=0 ; i<dst.size() ; i+=2)
    {
        const auto temp = dst[i];
        dst[i] = dst[i+1];
        dst[i+1] = temp;
    }
}
