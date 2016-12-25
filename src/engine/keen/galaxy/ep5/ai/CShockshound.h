/*
 * CShockshound.h
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 *
 *  This handles the Amptons in Galaxy
 */
 
#ifndef CSHOCKSHOUND_H_
#define CSHOCKSHOUND_H_

#include <vector>
#include "../../common/ai/CStunnable.h"

namespace galaxy {


class CShockshound : public CStunnable
{
public:
	CShockshound(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	/**
	 * Action functions
	 */
	void processSit();	
	void processBark();	
	void processWalking();	
	void processJump();	
	//void processTurn();
	
	bool isNearby(CSpriteObject &theObject);

	int checkSolidD( int x1, int x2, int y2, const bool push_mode );

	void process();

	/**
	 * What happens if the slug gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);
	
private:
	int mTimer;
	direction_t mKeenAlignmentX;
	direction_t mKeenAlignmentY;	
};

}

#endif /* CSHOCKSHOUND_H_ */
