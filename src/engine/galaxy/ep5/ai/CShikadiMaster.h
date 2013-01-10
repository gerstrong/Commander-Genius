/*
 * CShikadiMaster.h
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 *
 *  This handles the Amptons in Galaxy
 */
 
#ifndef CSHIKADIMASTER_H_
#define CSHIKADIMASTER_H_

#include <vector>
#include "engine/galaxy/common/ai/CStunnable.h"
#include "engine/galaxy/common/ai/CPlayerLevel.h"

namespace galaxy {


class CShikadiMaster : public CStunnable
{
public:
	CShikadiMaster(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	/**
	 * Action functions
	 */
	void processStanding();	
	void processShooting();	
	void processTeleporting();	
	
	bool isNearby(CSpriteObject &theObject);

	void process();

	/**
	 * What happens if the slug gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);
	
private:
	int mTimer;
	direction_t mKeenAlignment;
	bool mTeleport;
	
	CPlayerLevel *mpPlayer;
};

}

#endif /* CSHIKADIMASTER_H_ */
