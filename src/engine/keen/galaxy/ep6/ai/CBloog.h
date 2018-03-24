/*
 * CSparky.h
 *
 *  Created on: 26 Dez 2012
 *      Author: Gerstrong
 *
 *  This handles the Bloog in Galaxy
 */
 
#ifndef CBLOOG_H_
#define CBLOOG_H_

#include "../../common/ai/CStunnable.h"

namespace galaxy {


class CBloog : public CStunnable
{
public:
	CBloog(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

    bool isNearby(CSpriteObject &theObject) override;

	/**
	 * When it is moving normally
	 */
	void processWalking();

    int checkSolidD( int x1, int x2, int y2, const bool push_mode ) override;

    void process() override;

	/**
	 * What happens if the slug gets touched by another object
	 */
    void getTouchedBy(CSpriteObject &theObject) override;

private:
	int mTimer;
  
};

}

#endif /* CBLOOG_H_ */
