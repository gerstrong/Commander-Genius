#ifndef CBLOOGLET_H
#define CBLOOGLET_H

#include "engine/galaxy/common/ai/CStunnable.h"

namespace galaxy {

class CBlooglet : public CStunnable
{
    
public:
	CBlooglet(CMap *pmap, const Uint16 foeID,  const int actionOff, const bool carriesGem,
		  const Uint32 x, const Uint32 y);

	/**
	 * When it is moving normally
	 */
	void processRunning();

	int checkSolidD( int x1, int x2, int y2, const bool push_mode );

	void process();

	/**
	 * What happens if the slug gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);

private:
	int mTimer;    
	const bool mCarriesGem;
};

};

#endif // CBLOOGLET_H
