/*
 * CHuffman.cpp
 *
 *  Created on: 27.12.2009
 *      Author: gerstrong
 */

#include "CHuffman.h"
#include <cstring>

CHuffman::CHuffman()
{
	// TODO Auto-generated constructor stub

}

void CHuffman::readDictionary( unsigned char *p_exedata, unsigned long offset)
{
	memcpy(m_nodes, p_exedata+offset,255*sizeof(nodestruct));
}

/* Expand huffman-compressed input file into output buffer */
void CHuffman::expand(unsigned char *pin, unsigned char *pout, unsigned long inlen, unsigned long outlen)
{
	unsigned short curnode;
	unsigned long incnt = 0, outcnt = 0;
	unsigned char c, mask;
	unsigned short nextnode;

	curnode = 254; /* Head node */

	do
	{
		mask = 1;
		c = *(pin++);
		incnt++;

		do
		{
			if(c & mask)
				nextnode = m_nodes[curnode].bit1;
			else
				nextnode = m_nodes[curnode].bit0;


			if(nextnode < 256)
			{
				/* output a char and move back to the head node */
				*(pout++) = nextnode;
				outcnt++;
				curnode = 254;
			}
			else
			{
				/* move to the next node */
				curnode = nextnode & 0xFF;
			}
			/* Move to consider the next bit */
			mask <<= 1;
		}
		while(outcnt < outlen && mask != 0);

	}
	while(incnt < inlen && outcnt < outlen);
}

CHuffman::~CHuffman()
{
	// TODO Auto-generated destructor stub
}
