/*
 * CScrub.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CSCRUB_H_
#define CSCRUB_H_

#include "../../../common/CObject.h"

class CScrub : public CObject
{
public:
	CScrub(CMap *p_map, Uint32 x, Uint32 y);
	void process();
private:

	void walkLeft(int mx, int my);
	void walkDown();
	void walkRight(int mx, int my);
	void walkUp();
	void fall();
	void preparetoFall();
	void Scrub_TurnOnCansupportWhereNotKicked();

	unsigned char state;

	unsigned char animtimer, dietimer;
	unsigned char walkframe;
	signed int scrubdie_inertia_y;
	int fallinctimer,fallspeed;

	unsigned char walkdir;
	unsigned char kickedplayer[MAX_PLAYERS];
};

#endif /* CSCRUB_H_ */
