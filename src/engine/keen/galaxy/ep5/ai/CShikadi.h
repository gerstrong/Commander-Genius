/*
 * CShikadi.h
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 *
 *  This handles the Shikadis in Galaxy
 */
 
#ifndef CSHIKADI_H_
#define CSHIKADI_H_

#include <vector>
#include "../../common/ai/CStunnable.h"

namespace galaxy {


class CShikadi : public CStunnable
{
public:
	CShikadi(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	/**
	 * Action functions
	 */
	void processStanding();	
	void processWalking();	
	void processPoleZaps();	

	
	bool isNearby(CSpriteObject &theObject);

	int checkSolidD( int x1, int x2, int y2, const bool push_mode );

	void process();

	/**
	 * What happens if the slug gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);

    /**
     * @brief processLuaCycle
     * @return true if the Lua cycle was run successfully, otherwise false
     */
    bool processLuaCycle();

private:
    int mHealth;
    int mTimer;
};

}

#endif /* CSHIKADI_H_ */
