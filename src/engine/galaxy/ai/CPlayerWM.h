/*
 * CPlayer.h
 *
 *  Created on: 14.07.2010
 *      Author: gerstrong
 */

#ifndef CPLAYERWM_H_
#define CPLAYERWM_H_

#include "CPlayerBase.h"

namespace galaxy {

class CPlayerWM : public CPlayerBase
{
public:
	CPlayerWM(CMap *pmap,
			const Uint16 foeID,
			Uint32 x,
			Uint32 y,
			std::vector< SmartPointer<CGalaxySpriteObject> > &ObjectPtrs,
			CInventory &l_Inventory,
			stCheat &Cheatmode,
			const unsigned int actionoffset);

	void process();
	void processMoving();

	void startLevel(Uint16 object);
	void finishLevel(Uint16 object);
	void checkforSwimming(bool &bleft, bool &bright, bool &bup, bool &bdown);

	void performWalkingAnimation(bool walking);
	void performSwimmingAnimation();

private:
	Uint16 m_basesprite;
	Uint16 walkBaseFrame;
	direction_t m_looking_dir;

	Uint8 m_animation;
	Uint8 m_animation_time;
	Uint8 m_animation_ticker;
	bool m_cantswim;
};

}

#endif /* CPLAYERWM_H_ */
