/*
 * CVolteface.h
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 *
 *  This handles the Volteface in Galaxy
 */
 
#ifndef CVOLTEFACE_H_
#define CVOLTEFACE_H_

#include <vector>
#include "../../common/ai/CStunnable.h"
#include "../../common/ai/CMoveTarget.h"

namespace galaxy {


class CVolteface : public CStunnable, public CMoveTarget
{
public:
	CVolteface(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	/**
	 * Action functions
	 */
	void processMoving();	
	void processStunned();
		
	void process();

	/**
	 * What happens if the foe gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);
	
private:

	int mTimer;	
};

}

#endif /* CVOLTEFACE_H_ */
