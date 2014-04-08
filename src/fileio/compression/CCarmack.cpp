/*
 * CCarmack.cpp
 *
 *  Created on: 30.05.2010
 *      Author: gerstrong
 */

#include "CCarmack.h"
#include <base/GsLogging.h>
#include <base/utils/StringUtils.h>

CCarmack::CCarmack()
{
	// TODO Auto-generated constructor stub
}

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

/**
 * \brief			This function expands data using carmack
 * 					decompression algorithm
 * \param	source	pointer to the data that is compressed
 * \param	dest	pointer to the data that will be the output
 * \param	length	length of the EXPANDED data
 */
void CCarmack::expand( std::vector<byte>& dst, std::vector<byte>& src )
{
	uint32_t i, j, offset, length, inc;

	dst.clear();

    const unsigned int srcSize = src.size();

    for( i=WORDSIZE ; i<srcSize ; i+=inc )
	{
        if(TAG >= srcSize)
        {
            gLogging.textOut("Something went wrong with the Carmack compression!\n");
            return;
        }

		switch( src.at(TAG) )
		{
		case NEARTAG:
			if( src.at(COUNT)==0x00 && src.at(OFFSET_MSB)==0x00 )
			{
				dst.push_back(NEARTAG);
				inc = WORDSIZE+1;
			}
			else
			{
				offset = dst.size()-(WORDSIZE*src.at(OFFSET));
				length = WORDSIZE*src.at(COUNT);
				for( j=offset; j<offset+length; j+=WORDSIZE )
				{
					// Word is already swapped in the destination vector
					dst.push_back(dst.at(COPY_BYTE1));
					dst.push_back(dst.at(COPY_BYTE2));
				}
			}
			inc = WORDSIZE+1;
			break;
		case FARTAG:
			if( src.at(COUNT)==0x00 && src.at(OFFSET_MSB)==0x00 )
			{
				dst.push_back(FARTAG);
				inc = WORDSIZE+1;
			}
			else
			{
				offset = WORDSIZE*((src.at(OFFSET_MSB)<<8)+src.at(OFFSET_LSB));
				length = WORDSIZE*src.at(COUNT);
				for( j=offset; j<offset+length; j+=WORDSIZE )
				{
					if( j+src.at(COUNT)<dst.size() )
					{
						// Word is already swapped in the destination vector
						dst.push_back(dst.at(COPY_BYTE1));
						dst.push_back(dst.at(COPY_BYTE2));
					}
					else
					{
						gLogging.textOut("ERROR Offset overflow offset="+ itoa(j) +", actual size="+itoa(dst.size())+"\n");
						return;
					}
				}
			}
			inc = WORDSIZE+2;
			break;
		default:
			// Swap the bytes for the word
			dst.push_back(src.at(TAG));
			dst.push_back(src.at(COUNT));
			inc = WORDSIZE;
			break;
		}
	}
}
