#ifndef CNOSPIKE_H
#define CNOSPIKE_H

#include "engine/galaxy/common/ai/CStunnable.h"

namespace galaxy {
   

class CNospike : public CStunnable
{
public:
	CNospike(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	/**
	 * When it is moving normally
	 */
	void processSitting();
	
	void processWalking();
	
	void processCharging();
	
	void processStanding();
	
	int checkSolidD( int x1, int x2, int y2, const bool push_mode );
	
	void process();

	/**
	 * What happens if the nospike gets touched by another object
	 */
	bool isNearby(CSpriteObject &theObject);
	
	void getTouchedBy(CSpriteObject &theObject);
	
private:
	char mHealth;
	int mTimer;      
	direction_t mKeenAlignment;
	bool mGoodChargeChance;
	bool mCanFinishGame;
};

};

#endif // CNOSPIKE_H
