/*
 * rle.c
 *
 *  Created on: 29.01.2009
 *      Author: gerstrong
 */

#include <vector>
#include <fstream>
#include <stdio.h>
#include <SDL.h>

#include "CRLE.h"

unsigned long unRLEW(std::ifstream& file, std::vector<Uint16>& filebuf)
{
	/* The algorithm works as follows:
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
	 */
	Uint32 cursize=0;
	Uint16 value;

	/* File can have more stuff. We read a relative start */
	int startpos = file.tellg();

	while(!file.eof()) // check if the file is really RLEW compressed!
	{
		byte high_byte, low_byte;
		low_byte = file.get();
		high_byte = file.get();
		value = (high_byte<<8) | low_byte;
		if(value == 0xFEFE)
		{
			cursize = 1;
			filebuf.push_back(1);
			break;
		}
	}

	file.seekg(startpos);

	if( cursize == 0 )
	{
		file.clear();
		return 0; // This file is not RLEW compressed!
	}

	std::vector<Uint8>	compdata;

	while( !file.eof() )
	{
		Uint8 actual_byte = file.get();
		compdata.push_back(actual_byte);
	}

	CRLE RLE;
	RLE.expand(filebuf,compdata, 0xFEFE);

	return filebuf.size();
}
