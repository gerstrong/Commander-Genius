/*
 * CMastersShot.h
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 *
 *  This handles the Shikadi Master Shot in Galaxy
 */
 
#ifndef CMASTERSSHOT_H_
#define CMASTERSSHOT_H_

#include <vector>
#include "engine/galaxy/common/ai/CStunnable.h"

namespace galaxy {


class CMastersShot : public CStunnable
{
public:
	CMastersShot(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	/**
	 * Action functions
	 */
	void processWalking();	
	//void processTurn();
	
	bool isNearby(CSpriteObject &theObject);

	int checkSolidD( int x1, int x2, int y2, const bool push_mode );

	void process();

	/**
	 * What happens if the slug gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);
	
private:
	int mTimer;
};

}

#endif /* CMASTERSSHOT_H_ */
