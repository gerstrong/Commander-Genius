/*
 * CScrub.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CSCRUB_H_
#define CSCRUB_H_

#include "engine/vorticon/CVorticonSpriteObject.h"
#include "../../../common/CPlayer.h"

class CScrub : public CVorticonSpriteObject
{
public:
	CScrub(CMap *p_map, Uint32 x, Uint32 y);
	void getTouchedBy(CVorticonSpriteObject &theObject);
	void draw();
	void process();

private:

	void moveCarrierLeft(const int amnt);
	void moveCarrierRight(const int amnt);
	void moveCarrierUp(const int amnt);
	void moveCarrierDown(const int amnt);
    
	void walkLeft(int mx, int my);
	void walkDown();
	void walkRight(int mx, int my);
	void walkUp();
	void fall();
	void preparetoFall();

	unsigned char state;

	unsigned char animtimer, dietimer;
	unsigned char walkframe;
	int scrubdie_inertia_y;
	int fallspeed;

	CPlayer *mpCarriedPlayer;
};

#endif /* CSCRUB_H_ */
