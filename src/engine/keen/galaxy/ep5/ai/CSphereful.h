/*
 * CSphereful.h
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 *
 *  This handles the Amptons in Galaxy
 */
 
#ifndef CSPHEREFUL_H_
#define CSPHEREFUL_H_

#include <vector>
#include "../../common/ai/CStunnable.h"

namespace galaxy {


class CSphereful : public CStunnable
{
public:
	CSphereful(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	/**
	 * Action functions
	 */
	void processMoving();	

    void processStun();
	
	bool isNearby(CSpriteObject &theObject);

	void process();

	/**
	 * What happens if the slug gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);
	
private:
    int mTimer;
};

}

#endif /* CSPHEREFUL_H_ */
