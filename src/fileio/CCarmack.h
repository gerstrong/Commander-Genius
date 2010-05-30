/*
 * CCarmack.h
 *
 *  Created on: 30.05.2010
 *      Author: gerstrong
 *
 *  Integration of the Carmack decompression to Commander Genius
 */

#ifndef CCARMACK_H_
#define CCARMACK_H_

#include "TypeDefinitions.h"

class CCarmack
{
public:
	CCarmack();
	void expand(byte *source, word *dest, int length);

	virtual ~CCarmack();
};

#endif /* CCARMACK_H_ */
