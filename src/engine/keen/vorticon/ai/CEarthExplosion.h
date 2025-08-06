/*
 * CEarthExplosion.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CEARTHEXPLOSION_H_
#define CEARTHEXPLOSION_H_

#include "CRay.h"

class CEarthExplosion : public CRay {
public:
	CEarthExplosion(std::shared_ptr<CMap> p_map, Uint32 x, Uint32 y);
	void process();
private:
	int animframe, animtimer;
};

#endif /* CEARTHEXPLOSION_H_ */
