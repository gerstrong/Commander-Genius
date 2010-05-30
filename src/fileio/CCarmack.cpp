/*
 * CCarmack.cpp
 *
 *  Created on: 30.05.2010
 *      Author: gerstrong
 */

#include "CCarmack.h"

typedef uint8_t byte;
typedef uint16_t word;
typedef int32_t fixed;
typedef uint32_t longword;
typedef int8_t boolean;
typedef void * memptr;

CCarmack::CCarmack()
{
	// TODO Auto-generated constructor stub
}

/**
 * \brief			This function expands data using carmack
 * 					decompression algorithm
 * \param	source	pointer to the data that is compressed
 * \param	dest	pointer to the data that will be the output
 * \param	length	length of the EXPANDED data
 */
void CCarmack::expand(byte *source, word *dest, int length)
{

const word NEARTAG=0xa7;
const word FARTAG=0xa8;

	word ch,chhigh,count,offset;
	byte *inptr;
	word *copyptr, *outptr;

	length/=2;

	inptr = (byte *) source;
	outptr = dest;

	while (length>0)
	{
		ch = READWORD(inptr);
		chhigh = ch>>8;
		if (chhigh == NEARTAG)
		{
			count = ch&0xff;
			if (!count)
			{                               // have to insert a word containing the tag byte
				ch |= *inptr++;
				*outptr++ = ch;
				length--;
			}
			else
			{
				offset = *inptr++;
				copyptr = outptr - offset;
				length -= count;
				if(length<0) return;
				while (count--)
					*outptr++ = *copyptr++;
			}
		}
		else if (chhigh == FARTAG)
		{
			count = ch&0xff;
			if (!count)
			{                               // have to insert a word containing the tag byte
				ch |= *inptr++;
				*outptr++ = ch;
				length --;
			}
			else
			{
				offset = READWORD(inptr);
				copyptr = dest + offset;
				length -= count;
				if(length<0) return;
				while (count--)
					*outptr++ = *copyptr++;
			}
		}
		else
		{
			*outptr++ = ch;
			length --;
		}
	}
}

CCarmack::~CCarmack()
{
	// TODO Auto-generated destructor stub
}
