#ifndef CBIP_H
#define CBIP_H

#include "../../common/ai/CGalaxyActionSpriteObject.h"


namespace galaxy
{

    
class CBip : public CGalaxyActionSpriteObject
{
public:
    	CBip(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);
	
	/**
	 * When it is moving normally
	 */
	void processStanding();
		
	void processWalking();

	void processSquished();

	int checkSolidD( int x1, int x2, int y2, const bool push_mode );

	void process();

	/**
	 * What happens if the slug gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);	
	
private:
    int mTimer;
};


};

#endif // CBIP_H
