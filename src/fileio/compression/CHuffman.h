/*
 * CHuffman.h
 *
 *  Created on: 27.12.2009
 *      Author: gerstrong
 *      Original Author: Andrew Durdin
 *      Copyright (c)2002-2004
 *
 *  Integration of the Huffman compression to Commander Genius
 */

#ifndef CHUFFMAN_H_
#define CHUFFMAN_H_

#include "fileio/TypeDefinitions.h"
#include "fileio/CExeFile.h"

#define DICT_SIZE       256

struct nodestruct{
	unsigned short bit0;
	unsigned short bit1;
};
struct compstruct{
	int num;
	unsigned long bits;
};

class CHuffman
{
public:
	bool readDictionaryNumber( const CExeFile& ExeFile, const int dictnum );
	void readDictionaryAt( byte *p_exedata, unsigned long offset);
	void expand(byte *pin, byte *pout, unsigned long inlen, unsigned long outlen);

private:

	nodestruct m_nodes[256];
	//compstruct m_comptable[256];
};

#endif /* CHUFFMAN_H_ */
