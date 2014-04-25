/*
 * CPlanes.h
 *
 *  Created on: 12.07.2009
 *      Author: gerstrong
 */

#include "fileio/TypeDefinitions.h"

#ifndef CPLANES_H_
#define CPLANES_H_

class CPlanes {
public:
	CPlanes(byte *dataptr);

	unsigned char getbit(unsigned char plane);
	void setOffsets(unsigned long p1, unsigned long p2, unsigned long p3,
					 unsigned long p4, unsigned long p5 = 0);
    void readPlane(uint32_t p, uint8_t *pixels, uint16_t width, uint16_t height);
    void readPlaneofTiles(uint32_t p, uint8_t *pixels, uint16_t columns,
                                uint16_t tilesize, uint16_t numtiles);
	
private:
	unsigned long getbit_bytepos[5];
	unsigned char getbit_bitmask[5];
	
	byte *m_dataptr;
};

#endif /* CPLANES_H_ */
