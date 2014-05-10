/*
 * CRLE.cpp
 *
 *  Created on: 31.05.2010
 *      Author: gerstrong
 *
 *  Class for RLE(W) decompression
 *
 	 The algorithm works as follows:
	 1.) If implemented, get the first dword in the file, [Final Length]
	 2.) If [Length of data so far] < [Final Length] then:
	 3.) Get a word
	 4.) Is this word $FEFE?
	 -> If yes;
	 Get the next two words (Word1 and Word2)
	 Copy Word1 [Word2] times
	 Move forward three words and got to 2.)
	 -> If no;
	 Copy the word
	 Move forward a word and go to 2.)

	 This also means that if the file is already decompressed, the algorithm
	 will just read the bytes and dump a vector<word> which should be 16-bit
 *
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

void CRLE::expandSwapped( std::vector<word>& dst, std::vector<byte>& src, word key )
{
    std::size_t finsize, howmany;
	word value;
    std::size_t inc;

	finsize = (src.at(1)<<8) | src.at(0);
	finsize /= 2;


    for(std::size_t i=WORDSIZE ; dst.size() < finsize ; i+=inc)
    {
		// Read datum (word)
		value = (src.at(i+1)<<8)+src.at(i);
		// If datum is 0xFEFE/0xABCD Then
		if (value == key)
		{
			// Read count (word)
			howmany = (src.at(i+3)<<8)+src.at(i+2);
			value = (src.at(i+5)<<8)+src.at(i+4);

			// Do count times
            for(std::size_t j=0 ; j<howmany ; j++)
				dst.push_back(value);

			inc = 3*WORDSIZE;
		}
		else
		{
			dst.push_back(value);
			inc = WORDSIZE;
		}
    }
}

void CRLE::expand( std::vector<word>& dst, std::vector<byte>& src, word key )
{
    uint16_t word, count, inc;

    std::size_t finsize;
	byte high_byte, low_byte;

	low_byte = src.at(1);
	high_byte = src.at(0);
	finsize = (high_byte<<8) | low_byte;
	finsize /= 2;

    for(std::size_t i=WORDSIZE ; dst.size() < finsize ; i+=inc)
    {
      if(i+1 >= src.size())
      {
	word = 0;
      }
      else
      {	
        // Read datum (word)
        word = (src.at(i)<<8)+src.at(i+1);
      }
      
        // If datum is 0xFEFE/0xABCD Then
        if( word == key )
        {
            // Read count (word)
            count = (src.at(i+2)<<8)+src.at(i+3);
			word = (src.at(i+4)<<8)+src.at(i+5);

            // Do count times
            for(std::size_t j=0;j<count;j++)
				dst.push_back(word);

            inc = 3*WORDSIZE;
        }
        else
        {
            // Write datum (word)
			dst.push_back(word);
			inc = WORDSIZE;
        }
    }
}
