#ifndef CBABOBBA_H
#define CBABOBBA_H

#include "engine/galaxy/common/ai/CStunnable.h"

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

	/**
	 * What happens if the slug gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);

private:
	int mTimer;    
};


// TODO: We need a class which also processes the cinder, which is a small yet deadly fireball which can fall on the floor

};

#endif // CBABOBBA_H
