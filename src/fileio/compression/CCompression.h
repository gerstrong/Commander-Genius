/*
 * CCompression.h
 *
 *  Created on: 31.05.2010
 *      Author: gerstrong
 */

#ifndef CCOMPRESSION_H_
#define CCOMPRESSION_H_

#include "../TypeDefinitions.h"

#include <vector>

class CCompression
{
public:
	CCompression();
	void unpack( std::vector<byte>& dst, std::vector<byte>& src, word key );
	virtual ~CCompression();
};

#endif /* CCOMPRESSION_H_ */
