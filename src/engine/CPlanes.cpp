/*
 * CPlanes.cpp
 *
 *  Created on: 12.07.2009
 *      Author: gerstrong
 */

#include "CPlanes.h"
#include <stdio.h>


/**
 * \brief	Constructor initializes the positions,getbit will retrieve data from
 * \param	dataptr	Pointer to the data where the bit plane will be read.
 */
CPlanes::CPlanes(byte *dataptr) :
m_dataptr(dataptr)
{}

/**
 * \brief	THis function is used for setting the offsets, so the bits can be read.
 * \param	p1, p2, p3, p4, p5 The offsets of the planes where the bits are going to be read
 */
void CPlanes::setOffsets(unsigned long p1, unsigned long p2, unsigned long p3,
				 unsigned long p4, unsigned long p5)
{
	
	getbit_bytepos[0] = p1;
	getbit_bytepos[1] = p2;
	getbit_bytepos[2] = p3;
	getbit_bytepos[3] = p4;
	getbit_bytepos[4] = p5;
	
	for(int i=0;i<5;i++)
	    getbit_bitmask[i] = 128;
}


// retrieves a bit from plane "plane". the positions of the planes
// should have been previously initilized with setplanepositions()
unsigned char CPlanes::getbit(unsigned char plane)
{
	int retval;
	int byt;
	if (!getbit_bitmask[plane])
	{
		getbit_bitmask[plane] = 128;
		getbit_bytepos[plane]++;
	}
	byt = m_dataptr[getbit_bytepos[plane]];
	
	if (byt & getbit_bitmask[plane])
	{
		retval = 1;
	}
	else
	{
		retval = 0;
	}
	
	getbit_bitmask[plane] >>= 1;
	
	return retval;
}
