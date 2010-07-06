/*
 * CMessie.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CMESSIE_H_
#define CMESSIE_H_

#include "../../../common/CObject.h"

class CMessie : public CObject
{
public:
	CMessie(CMap *p_map, Uint32 x, Uint32 y);
	void process();
private:
};

#endif /* CMESSIE_H_ */
