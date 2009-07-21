/*
 * CPlanes.h
 *
 *  Created on: 12.07.2009
 *      Author: gerstrong
 */

#ifndef CPLANES_H_
#define CPLANES_H_

class CPlanes {
public:
	CPlanes(unsigned long p1, unsigned long p2, unsigned long p3,\
            unsigned long p4, unsigned long p5);

	unsigned char getbit(char *buf, unsigned char plane);

private:
	unsigned long getbit_bytepos[5];
	unsigned char getbit_bitmask[5];

};

#endif /* CPLANES_H_ */
