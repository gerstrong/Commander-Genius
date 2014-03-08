/*
 * CMeep.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CMEEP_H_
#define CMEEP_H_

#include "../CVorticonSpriteObject.h"
#include "CPlayer.h"

class CMeep : public CVorticonSpriteObject
{
public:
	CMeep(CMap *p_map, Uint32 x, Uint32 y);
	
	bool isNearby(CVorticonSpriteObject &theObject);
	void getTouchedBy(CVorticonSpriteObject &theObject);
	
	void process();
private:
	char state;
	char dir;

	int animframe, animtimer;
	int timer;
};

#endif /* CMEEP_H_ */
