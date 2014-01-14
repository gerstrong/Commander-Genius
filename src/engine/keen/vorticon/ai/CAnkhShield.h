/*
 * CAnkhShield.h
 *
 *  Created on: 16.07.2010
 *      Author: gerstrong
 *
 *  This new class has been ripped of the player class. I think it should have his own class.
 *
 */

#ifndef CANKHSHIELD_H_
#define CANKHSHIELD_H_

#include "CPlayer.h"

// for ankh shield...
// stage1 is no flicker of the shield, changes colors
// in stage2 the shield flickers fast, changes colors
// in stage3 the shield flashes slow, no color change
#define ANKH_STAGE1_TIME    750
#define ANKH_STAGE2_TIME    380
#define ANKH_STAGE3_TIME    125
#define PLAY_ANKH_TIME      (ANKH_STAGE1_TIME+ANKH_STAGE2_TIME+ANKH_STAGE3_TIME)
// these correspond stage 1,2 and 3 respectively

class CAnkhShield : public CVorticonSpriteObject
{
public:
	CAnkhShield(CPlayer &attachedPlayer);
	void process();
private:
	enum{
		ANKH_STATE_NOFLICKER,
		ANKH_STATE_FLICKERFAST,
		ANKH_STATE_FLICKERSLOW
	}
	state;
	size_t frame;
	size_t timer;
	CPlayer &m_attachedPlayer;
    int &ankhtime;
};

#endif /* CANKHSHIELD_H_ */
