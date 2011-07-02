/*
 * CStunnable.h
 *
 *  Created on: 23.06.2011
 *      Author: gerstrong
 */

#ifndef CSTUNNABLE_H_
#define CSTUNNABLE_H_

#include "common/CObject.h"
#include "engine/galaxy/ai/CBullet.h"

namespace galaxy
{

class CStunnable: public CObject {
public:
	CStunnable(	CMap *pmap,
				Uint32 x,
				Uint32 y,
				object_t type=OBJ_NONE );

	/**
	 * What happens when some objects gets touched, or normally shot
	 */
	virtual void getTouchedBy(CObject &theObject);

	/**
	 * This happens when the object gets shot.
	 * The Starring appears when it hits the floor
	 */
	void processGettingStunned();

	/**
	 * empty cycle which is processed when the object is stunned
	 */
	void processStunned();

protected:
	bool m_stunned;
	void (CStunnable::*mp_processState)();
};

}

#endif /* CSTUNNABLE_H_ */
