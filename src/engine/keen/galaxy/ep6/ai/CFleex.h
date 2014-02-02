#ifndef CFLEEX_H
#define CFLEEX_H

#include "../../common/ai/CStunnable.h"

namespace galaxy
{

class CFleex : public CStunnable
{
  
public:
	CFleex(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);
	
	/**
	 * When it is moving normally
	 */
	void processWalk();
	
	void processTrack();
	
	void processLook();
	
	int checkSolidD( int x1, int x2, int y2, const bool push_mode );

	void process();

	/**
	 * What happens if the nospike gets touched by another object
	 */
	bool isNearby(CSpriteObject &theObject);
	
	void getTouchedBy(CSpriteObject &theObject);
	
private:
	int mHealth;
	int mTimer;
	direction_t mKeenAlignment;
	bool mGoodTrackChance;    
  
};

};

#endif // CFLEEX_H
