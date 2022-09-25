/*
 * CRoboRed.h
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 *
 *  This handles the Red Robot in Galaxy
 */
 
#ifndef CROBORED_H_
#define CROBORED_H_

#include <vector>
#include "../../common/ai/CStunnable.h"

namespace galaxy {


class CRoboRed : public CStunnable
{
public:
	CRoboRed(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	/**
	 * Action functions
	 */
	void processMoving();	
	void processPauseBeforeShoot();	
	void processShoot();	
	
    bool isNearby(CSpriteObject &theObject) override;

    int checkSolidD( int x1, int x2, int y2, const bool push_mode ) override;

    bool loadLuaScript(const std::string &scriptBaseName) override;

    void process() override;

	/**
	 * What happens if the slug gets touched by another object
	 */
    void getTouchedBy(CSpriteObject &theObject) override;
	
private:
    int mTimer = 0;
    int mLookTimer = 0;
    bool swapYDir = false;
    bool mKeenNearby = false;

    int moveHorizSpeed;

    bool mAlternateShot = false;
    bool mJiggle = false;
    int mJiggleFreq = (2<<STC);
};

}

#endif /* CROBORED_H_ */
