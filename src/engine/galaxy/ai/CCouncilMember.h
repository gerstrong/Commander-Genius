/*
 * CCouncilMember.h
 *
 *  Created on: 23FEB2011
 *      Author: FCTW
 *
 *  This handles the council members in Galaxy
 */

#ifndef CCOUNCIL_MEMBER_H_
#define CCOUNCIL_MEMBER_H_

#include "common/CObject.h"

namespace galaxy {

#define A_COUNCIL_MEMBER_MOVE		0	/* Ordinary walking council member. */
#define A_COUNCIL_MEMBER_THINK		2	/* Council member stopping to ponder. */

class CCouncilMember : public CObject {
public:
	CCouncilMember(CMap *pmap, Uint32 x, Uint32 y);

	/** Process walking of elder here */
	void processWalking();

	/** Process thinking of elder here */
	void processThinking();

	/** Normal process routine */
	void process();

	void getTouchedBy(CObject &theObject);

private:
	bool	rescued;
	int 	m_timer;
	void (CCouncilMember::*mp_processState)();
};

}

#endif /* CCOUNCIL_MEMBER_H_ */
