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
#define A_SLUG_POOHING		2
#define A_SLUG_STUNNED		3

class CPoisonSlug : public CObject {
public:
	CPoisonSlug(CMap *pmap, Uint32 x, Uint32 y);
	void process();
};

}

#endif /* CSLUG_H_ */
