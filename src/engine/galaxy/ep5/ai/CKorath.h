/*
 * CKorath.h
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 *
 *  This handles the Korath in Galaxy
 */
 
#ifndef CKORATH_H_
#define CKORATH_H_

#include <vector>
#include "engine/galaxy/common/ai/CStunnable.h"

namespace galaxy {


class CKorath : public CStunnable
{
public:
	CKorath(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	/**
	 * Action functions
	 */
	void processWalking();	
	void processSitting();	
	

	int checkSolidD( int x1, int x2, int y2, const bool push_mode );

	void process();

	/**
	 * What happens if the slug gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);
	
private:
	int mTimer;
};

}

#endif /* CKORATH_H_ */
