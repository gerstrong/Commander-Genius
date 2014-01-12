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

#include "engine/galaxy/common/CGalaxySpriteObject.h"
#include <map>
#include <string>

namespace galaxy {

class CCouncilMember : public CGalaxySpriteObject
{
public:
	CCouncilMember(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);

	/** Process walking of elder here */
	void processWalking();

	/** Process thinking of elder here */
	void processThinking();

	/** Normal process routine */
	void process();

	/** called when keen talks to the janitor. Happens in the secret level*/
	void performJanitorMode();

	void getTouchedBy(CSpriteObject &theObject);

private:
	bool	rescued;
	int 	m_timer;
	void (CCouncilMember::*mp_processState)();
	std::map<int, std::string>	answermap;	
};

}

#endif /* CCOUNCIL_MEMBER_H_ */
