/*
 * CSparky.h
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 *
 *  This handles the Amptons in Galaxy
 */
 
#ifndef CSPINDRED_H_
#define CSPINDRED_H_

#include <vector>
#include "../../common/ai/CStunnable.h"

namespace galaxy {


class CSpindred : public CStunnable
{
public:
	CSpindred(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	void process();

	/**
	 * What happens if the slug gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);
	
private:
    //int mTimer = 0;
    bool mInverseGravity = false;
    int mBounces = 0;
};

}

#endif /* CSPINDRED_H_ */
