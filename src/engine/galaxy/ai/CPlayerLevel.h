/*
 * CPlayerLevel.h
 *
 *  Created on: 06.08.2010
 *      Author: gerstrong
 */

#ifndef CPLAYERLEVEL_H_
#define CPLAYERLEVEL_H_

#include "common/CObject.h"
#include "engine/CEvent.h"


namespace galaxy {

enum player_actions{
PA_X = 0,
PA_Y,
PA_JUMP, PA_POGO,
PA_FIRE, PA_STATUS,
PA_MAX_ACTIONS
};


class CPlayerLevel : public CObject {
public:
	CPlayerLevel(CMap *pmap, Uint32 x, Uint32 y,
			std::vector<CObject*>& ObjectPtrs, direction_t facedir);

	void process();
	void processInput();
	void processFalling();
	void processMoving();
	void processJumping();

	void performWalkingAnimation(bool walking);
	void processExiting();

	virtual ~CPlayerLevel();

private:
	Uint16 m_basesprite;
	direction_t m_looking_dir;

	Uint8 m_animation;
	Uint8 m_animation_time;
	Uint8 m_animation_ticker;
	std::vector<CObject*>& m_ObjectPtrs;

	char m_playcontrol[PA_MAX_ACTIONS];

	bool m_pfiring;
	int m_jumpheight;

	enum {
		STANDING, WALKING, JUMPING, FALLING
	} state;

};

}

#endif /* CPLAYERLEVEL_H_ */
