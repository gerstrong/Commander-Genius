#ifndef CBABOBBA_H
#define CBABOBBA_H

#include "../../common/ai/CStunnable.h"
#include "../../common/ai/CGalaxyActionSpriteObject.h"

namespace galaxy {

class CBabobba : public CStunnable
{
public:
	CBabobba(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	/**
	 * When it is moving normally
	 */
	void processJumping();
	
	void processSitting();
	
	void processShooting();
	
	void processJumpingAlt();
	
	void processNapping();

	void process();

	bool isNearby(CSpriteObject &theObject);
	
	/**
	 * What happens if it gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);

private:
	int mTimer;    
	bool mGoodFireChance;
};


class CCinder : public CGalaxyActionSpriteObject
{
public:
    CCinder(CMap *pmap, const Uint16 foeID,
        const Uint32 x, const Uint32 y, const int horDir, const int sprVar);
	
	void processTossed();	
	void processLanded();
	void processDying();
	void processVannish();
	
	void process();

	/**
	 * What happens if it gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);   	
	
private:
	int mTimer;
};

// TODO: We need a class which also processes the cinder, which is a small yet deadly fireball which can fall on the floor

};

#endif // CBABOBBA_H
