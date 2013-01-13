/*
 * CShikadiMine.h
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 *
 *  This handles the Amptons in Galaxy
 */
 
#ifndef CSHIKADIMINE_H_
#define CSHIKADIMINE_H_

#include <vector>
#include "engine/galaxy/common/ai/CStunnable.h"

namespace galaxy {


class CShikadiMine : public CStunnable
{
public:
	CShikadiMine(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	/**
	 * Action functions
	 */
	void processWalking();	
	//void processTurn();
	
	bool isNearby(CSpriteObject &theObject);

	int checkSolidD( int x1, int x2, int y2, const bool push_mode );

	void process();

	/**
	 * What happens if the foe gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);
	
private:
	int mTimer;
};


///////////////////
/// Mine shards ///
///////////////////

class CMineShards : public CStunnable
{
  
public:  
    CMineShards(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y, const int xSpeed);
    
    void process();
    
    /**
     * What happens if the foe gets touched by another object
     */
    void getTouchedBy(CSpriteObject &theObject);    
    
private:
  int mXSpeed;

};


}

#endif /* CSHIKADIMINE_H_ */
