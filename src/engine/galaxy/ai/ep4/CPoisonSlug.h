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

#include <vector>
#include "engine/galaxy/ai/CStunnable.h"

namespace galaxy {

#define A_SLUG_MOVE			0	/* Ordinary slug_move action */
#define A_SLUG_POOING		2
#define A_SLUG_STUNNED		3
#define A_SLUG_STUNNED_ALT	4

class CPoisonSlug : public CStunnable
{
public:
	CPoisonSlug(CMap *pmap, Uint32 x, Uint32 y,
				std::vector< SmartPointer<CSpriteObject> >&ObjectPtrs);

	/**
	 * When slug is moving normally
	 */
	void processCrawling();


	/**
	 * When slug is pooing slime
	 */
	void processPooing();


	void process();

	/**
	 * What happens if the slug gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);

private:
	std::vector< SmartPointer<CSpriteObject> > &m_ObjectPtrs;
	int m_timer;
};

}

#endif /* CSLUG_H_ */
