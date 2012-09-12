/*
 * CPlatform.h
 *
 *  Created on: 23.02.2011
 *      Author: gerstrong
 *
 *  This handles the floating platforms in Galaxy
 */

#ifndef CPLATFORM_H_
#define CPLATFORM_H_

#include "engine/galaxy/CGalaxySpriteObject.h"
#include "engine/galaxy/ai/CPlayerLevel.h"

namespace galaxy {

enum platform_actions
{
A_PLATFORM_MOVE	 = 0,	/* Ordinary moving platform */
A_PLATFORM_SIT = 1,  	/* Dropping platform waiting for passenger */
A_PLATFORM_DROP	 = 2,	/* Dropping platform dropping */
A_PLATFORM_RISE = 	3	/* Dropping platform rising */
};

class CPlatform : public virtual CGalaxySpriteObject
{
public:
	CPlatform(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);
	virtual void process();
	void getTouchedBy(CSpriteObject &theObject);

	virtual bool calcVisibility();

protected:

	void movePlatX(const int amnt);
	void movePlatY(const int amnt);

	void movePlatLeft(const int amnt);
	void movePlatRight(const int amnt);
	void movePlatUp(const int amnt);
	void movePlatDown(const int amnt);

	CPlayerLevel *mp_CarriedPlayer;
};

}

#endif /* CPLATFORM_H_ */
