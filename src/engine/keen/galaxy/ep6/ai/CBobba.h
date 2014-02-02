#ifndef CBOBBA_H
#define CBOBBA_H

#include "../../common/ai/CGalaxyActionSpriteObject.h"

namespace galaxy
{

class CBobba : public CGalaxyActionSpriteObject
{
public:
	CBobba(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	/**
	 * When it is moving normally
	 */
	void processJumping();
	
	void processSitting();
	
	void processShooting();
	
	void process();

	bool isNearby(CSpriteObject &theObject);
	
	/**
	 * What happens if it gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);

private:
	int mTimer;
};

};

#endif // CBOBBA_H
