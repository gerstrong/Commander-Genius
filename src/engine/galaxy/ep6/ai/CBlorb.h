#ifndef CBLORB_H
#define CBLORB_H

#include <vector>
#include "engine/galaxy/common/ai/CStunnable.h"

namespace galaxy {


class CBlorb : public CStunnable
{
public:
	CBlorb(CMap *pmap, const Uint16 foeID, 
		       const Uint32 x, const Uint32 y);

	void processMoving();
	
	void process();

	/**
	 * What happens if the slug gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);

};

}

#endif // CBLORB_H
