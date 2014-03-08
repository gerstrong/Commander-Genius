#ifndef CCEILICK_H
#define CCEILICK_H

#include "../../common/ai/CStunnable.h"

namespace galaxy
{

class CCeilick : public CStunnable
{
public:
	CCeilick(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	/**
	 * When it is moving normally
	 */
	void processSleeping();
	
	void processLicking();
	
	void process();

	bool isNearby(CSpriteObject &theObject);
	
	/**
	 * What happens if it gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);

    void draw();

private:
	int mTimer;
	bool mChanceToLick;
	int mCrawlPos;
	bool mLaughed;
};

};

#endif // CCEILICK_H
