#ifndef CMOLLY_H
#define CMOLLY_H

#include "../../common/CGalaxySpriteObject.h"

namespace galaxy
{
    
class CMolly : public CGalaxySpriteObject
{
public:
	CMolly(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);

	/** Process walking of elder here */
	void processMoving();

	/** Normal process routine */
	void process();

	void getTouchedBy(CSpriteObject &theObject);

private:
	bool	rescued;
};

};

#endif // CMOLLY_H
