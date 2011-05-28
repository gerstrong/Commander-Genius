/*
 * CPoisonSlug.h
 *
 *  Created on: 23FEB2011
 *      Author: FCTW
 *
 *  This handles the Poison Slugs in Galaxy
 */
 
#ifndef CPOISONSLUG_H_
#define CPOISONSLUG_H_

#include "common/CObject.h"

namespace galaxy {

#define A_SLUG_MOVE			0	/* Ordinary slug_move action */
#define A_SLUG_POOING		2
#define A_SLUG_STUNNED		3
#define A_SLUG_STUNNED_ALT	4

class CPoisonSlug : public CObject {
public:
	CPoisonSlug(CMap *pmap, Uint32 x, Uint32 y);

	/**
	 * When slug is moving normally
	 */
	void processCrawling();


	/**
	 * When slug is pooing slime
	 */
	void processPooing();

	void process();

private:
	int m_timer;
	void (CPoisonSlug::*mp_processState)();
};

}

#endif /* CSLUG_H_ */
