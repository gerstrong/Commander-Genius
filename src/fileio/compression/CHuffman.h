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

#include <base/TypeDefinitions.h>
#include "fileio/CExeFile.h"
#include <string>

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
	bool readDictionaryNumberfromEnd( const CExeFile& ExeFile );
	bool readDictionaryFromFile( const std::string &filename );
	void readDictionaryAt( byte *p_exedata, unsigned long offset);
    void expand(byte *pin, byte *pout, const unsigned long inlen, const unsigned long outlen);

private:

	nodestruct m_nodes[DICT_SIZE];
};

#endif /* CHUFFMAN_H_ */
