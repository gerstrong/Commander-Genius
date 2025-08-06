#ifndef CSECURITYDOOR_H
#define CSECURITYDOOR_H

#include "../../common/CGalaxySpriteObject.h"


class CSecurityDoor : public CGalaxySpriteObject
{
public:
    CSecurityDoor(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	void process();
	
private:
	int mTimer;    	
	int mFrameCount;    
};

#endif // CSECURITYDOOR_H
