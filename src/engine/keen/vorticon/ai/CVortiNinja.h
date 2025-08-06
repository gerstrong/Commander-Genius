/*
 * CVortiNinja.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CVORTININJA_H_
#define CVORTININJA_H_

#include "../CVorticonSpriteObject.h"
#include "CPlayer.h"

// Ninja AI (the black, bear-like karate-kicking creature in ep3)
enum ninja_actions{
	NINJA_STAND, NINJA_KICK, NINJA_DYING
};

class CVortiNinja : public CVorticonSpriteObject
{
public:
	CVortiNinja(std::shared_ptr<CMap> p_map, Uint32 x, Uint32 y);
	void init();
	bool isNearby(CSpriteObject &theObject);
	void getTouchedBy(CSpriteObject &theObject);	
	void process();
private:
	ninja_actions state;
	char dir;

	int animframe, animtimer;
	unsigned int timetillkick;

	int dietimer;
	bool longjump;
};

#endif /* CVORTININJA_H_ */
