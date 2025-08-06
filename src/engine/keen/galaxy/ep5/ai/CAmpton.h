/*
 * CSparky.h
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 *
 *  This handles the Amptons in Galaxy
 */
 
#ifndef CAMPTON_H_
#define CAMPTON_H_

#include <vector>
#include "../../common/ai/CStunnable.h"

namespace galaxy {


class CAmpton : public CStunnable
{
public:
	CAmpton(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y);


    bool loadLuaScript(const std::string &scriptBaseName) override;

	/**
     * Action methods
	 */
	void processWalking();	
	void processTurn();
	void processStartPole();
	void processPoleSlide();
	void processStopPole();
	void processFlipSwitch();	
	
    bool isNearby(CSpriteObject &);

	int checkSolidD( int x1, int x2, int y2, const bool push_mode );

    /**
     * @brief processShooting   Throwing (Keen 9)
     */
    void processShooting();

	void process();

	/**
	 * What happens if the slug gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);
	
private:
    int mTimer = 0;

    bool mAllowClimbing = true;

    bool mScreamAfterShoot = true;

    bool mKillOnPole = true;
};

}

#endif /* CAMPTON_H_ */
