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

#include <vector>
#include "../TypeDefinitions.h"

class CCarmack
{
public:
	CCarmack();
	void expand( std::vector<byte>& dst, std::vector<byte>& src );
};

#endif /* CCARMACK_H_ */
