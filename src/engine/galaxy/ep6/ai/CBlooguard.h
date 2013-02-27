#ifndef CBLOOGUARD_H
#define CBLOOGUARD_H

#include "engine/galaxy/common/ai/CStunnable.h"

namespace galaxy {

class CBlooguard : public CStunnable
{
public:
	CBlooguard(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	/**
	 * When it is moving normally
	 */
	void processWalking();
	
	void processClubbing();

	int checkSolidD( int x1, int x2, int y2, const bool push_mode );

	void process();

	/**
	 * What happens if the slug gets touched by another object
	 */
	bool isNearby(CSpriteObject &theObject);
	
	void getTouchedBy(CSpriteObject &theObject);
	
private:
	int mHealth;
	int mTimer;    
	bool mGoodClubChance;
};

};

#endif // CBLOOGUARD_H
