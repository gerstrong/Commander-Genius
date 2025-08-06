/*
 * CStunnable.h
 *
 *  Created on: 23.06.2011
 *      Author: gerstrong
 */

#ifndef CSTUNNABLE_H_
#define CSTUNNABLE_H_

#include "CGalaxyActionSpriteObject.h"

namespace galaxy
{

class CStunnable: public CGalaxyActionSpriteObject
{
public:
	CStunnable(std::shared_ptr<CMap> pMap,
                   const Uint16 foeID,
                   Uint32 x, Uint32 y );

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
    virtual void processStunned();
	
    /**
     * @brief drawStars unconditional stars of animated stars
     */
    void drawStars();

	/**
	 * This draw routine might show stars in case the enemies are stunned
	 */
	void draw();


protected:
	bool m_stunned;	
	
	int starSprite;
	int starSpriteBase;
	unsigned char m_animation_timer;
	int starTimer;
    bool mDispStarsBelow = false;
};

}

#endif /* CSTUNNABLE_H_ */
