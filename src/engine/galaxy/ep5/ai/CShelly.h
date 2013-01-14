/*
 * CShelly.h
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 *
 *  This handles the Amptons in Galaxy
 */
 
#ifndef CSHELLY_H_
#define CSHELLY_H_

#include <vector>
#include "engine/galaxy/common/ai/CStunnable.h"

namespace galaxy {


class CShelly : public CStunnable
{
public:
	CShelly(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	/**
	 * Action functions
	 */
	void processWalking();
	void processPrepareJump();
	void processJump();	
	void processFall();
	void processSmash();
	void processSmoke();
	
	bool isNearby(CSpriteObject &theObject);

	int checkSolidD( int x1, int x2, int y2, const bool push_mode );

	void process();

	/**
	 * What happens if the slug gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);
	
private:
	int mTimer;
	
	direction_t mKeenAlignment;
	bool mGoodJumpChance;
	
};





////////////////////////
/// Shelly Fragments ///
////////////////////////

class CShellyFrags : public CStunnable
{
  
public:  
    CShellyFrags(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y, const int xSpeed);
    
    void process();
    
    /**
     * What happens if the foe gets touched by another object
     */
    void getTouchedBy(CSpriteObject &theObject);    
    
private:
  int mXSpeed;    

};


}

#endif /* CSHELLY_H_ */
