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

#define A_PLATFORM_MOVE	0	/* Ordinary moving platform */
#define A_PLATFORM_SIT	1	/* Dropping platform waiting for passenger */
#define A_PLATFORM_DROP	2	/* Dropping platform dropping */
#define A_PLATFORM_RISE	3	/* Dropping platform rising */

class CPlatform : public virtual CGalaxySpriteObject
{
public:
	CPlatform(CMap *pmap, Uint32 x, Uint32 y);
	virtual void process();
	void getTouchedBy(CSpriteObject &theObject);

	virtual bool calcVisibility();

protected:

	void movePlatLeft(const int& amnt);
	void movePlatRight(const int& amnt);
	void movePlatUp(const int& amnt);
	void movePlatDown(const int& amnt);

	CPlayerLevel *mp_CarriedPlayer;
};

}

#endif /* CPLATFORM_H_ */
