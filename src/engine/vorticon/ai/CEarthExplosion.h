/*
 * CEarthExplosion.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CEARTHEXPLOSION_H_
#define CEARTHEXPLOSION_H_

#include "../../../common/CObject.h"

class CEarthExplosion : public CObject {
public:
	CEarthExplosion(CMap *p_map, Uint32 x, Uint32 y);
	void process();
};

#endif /* CEARTHEXPLOSION_H_ */
