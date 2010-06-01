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

void CRLE::expand( std::vector<word>& dst, std::vector<byte>& src, word key )
{
	size_t finsize, howmany;
	byte high_byte, low_byte;
	word value;

	low_byte = src.at(0);
	high_byte = src.at(1);
	finsize = (high_byte<<8) | low_byte;
	finsize /= 2;

	for(size_t i=2 ; dst.size() < finsize ;)
    {
		low_byte = src.at(i++);
		high_byte = src.at(i++);
		value = (high_byte<<8) | low_byte;

		if (value == key)
		{
			low_byte = src.at(i++);
			high_byte = src.at(i++);
			howmany = (high_byte<<8) | low_byte;

			low_byte = src.at(i++);
			high_byte = src.at(i++);
			value = (high_byte<<8) | low_byte;

			for(Uint32 j=0;j<howmany;j++)
				dst.push_back(value);
		}
		else
			dst.push_back(value);
    }
}

CRLE::~CRLE()
{
	// TODO Auto-generated destructor stub
}
