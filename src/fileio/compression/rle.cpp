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

	Uint32 cursize=0;
	Uint16 value;

	/* File can have more stuff. We read a relative start */

	filebuf.push_back(1);

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
