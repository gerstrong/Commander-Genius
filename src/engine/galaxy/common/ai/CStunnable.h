/*
 * CStunnable.h
 *
 *  Created on: 23.06.2011
 *      Author: gerstrong
 */

#ifndef CSTUNNABLE_H_
#define CSTUNNABLE_H_

#include "engine/galaxy/common/CGalaxySpriteObject.h"
#include "engine/galaxy/common/ai/CBullet.h"
#include <map>

namespace galaxy
{

class CStunnable: public CGalaxySpriteObject
{
public:
	CStunnable(CMap *pmap,
				const Uint16 foeID,
				Uint32 x,
				Uint32 y );

	/**
	 * What happens when some objects gets touched, or normally shot
	 */
	virtual void getTouchedBy(CSpriteObject &theObject);

	/**
	 * This happens when the object gets shot.
	 * The Starring appears when it hits the floor
	 */
	void processGettingStunned();

	/**
	 * empty cycle which is processed when the object is stunned
	 */
	void processStunned();
	
	/**
	 * This draw routine might show stars in case the enemies are stunned
	 */
	void draw();

	void setActionForce(const size_t ActionNumber);

protected:
	bool m_stunned;	
	void (CStunnable::*mp_processState)();
	std::map< size_t, void (CStunnable::*)() > mActionMap;
	
	int starSprite;
	int starSpriteBase;
	unsigned char m_animation_timer;
	int starTimer;
};

}

#endif /* CSTUNNABLE_H_ */
