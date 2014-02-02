#ifndef CBLORB_H
#define CBLORB_H

#include <vector>
#include "../../common/ai/CStunnable.h"

namespace galaxy {


class CBlorb : public CStunnable
{
public:
	CBlorb(CMap *pmap, const Uint16 foeID, 
		       const Uint32 x, const Uint32 y);

	void processMoving();
	
	void process();

	/**
	 * What happens it gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);

};

}

#endif // CBLORB_H
