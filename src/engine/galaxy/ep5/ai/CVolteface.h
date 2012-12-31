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
#include "engine/galaxy/common/ai/CStunnable.h"

namespace galaxy {


class CVolteface : public CStunnable
{
public:
	CVolteface(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	/**
	 * Action functions
	 */
	void processMoving();	
	void processStunned();
		
	void detectNextTarget(const VectorD2<int> &oldTarget);
	
	void process();

	/**
	 * What happens if the foe gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);
	
private:
  
	void fetchInitialDir();
	void readDirection(const Uint16 object, 	
				int &xDirection,		
				int &yDirection );
  
	int mTimer;
	
	bool targetmode;
	VectorD2<int> target;	
};

}

#endif /* CVOLTEFACE_H_ */
