/*
 * CRLE.cpp
 *
 *  Created on: 31.05.2010
 *      Author: gerstrong
 *
 *  Class for RLE(W) decompression
 */

#include "CRLE.h"

CRLE::CRLE()
{}

#define NEARTAG     0xA7
#define FARTAG      0xA8
#define WORDSIZE    2

#define COUNT       i
#define TAG         i+1
#define OFFSET      i+2

#define OFFSET_MSB  i+3 // Fartag Offset
#define OFFSET_LSB  i+2

#define COPY_BYTE1  j
#define COPY_BYTE2  j+1

void CRLE::expand( std::vector<uint8_t>& dst, std::vector<uint8_t>& src, uint16_t key )
{
    uint16_t i, j, word, count, inc;

    for( i=WORDSIZE; i<src.size(); i+=inc )
    {
        // Read datum (word)
        word = (src.at(i)<<8)+src.at(i+1);
        // If datum is 0xFEFE/0xABCD Then
        if( word == key )
        {
            // Read count (word)
            count = (src.at(i+2)<<8)+src.at(i+3);
            // Do count times
            for( j=0; j<count; j++ )
            {
                // Write datum (word)
                dst.push_back(src.at(i+4));
                dst.push_back(src.at(i+5));
            }
            inc = 3*WORDSIZE;
        }
        else
        {
            // Write datum (word)
            dst.push_back(src.at(i));
            dst.push_back(src.at(i+1));
            inc = WORDSIZE;
        }
    }
}

CRLE::~CRLE()
{
	// TODO Auto-generated destructor stub
}
