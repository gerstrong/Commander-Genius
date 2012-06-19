/*
 * CScrub.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CSCRUB_H_
#define CSCRUB_H_

#include "../../../common/CSpriteObject.h"
#include "../../../common/CPlayer.h"

class CScrub : public CSpriteObject
{
public:
	CScrub(CMap *p_map, Uint32 x, Uint32 y,
			std::vector<CPlayer>& Player);
	void process();
	void SetAllCanSupportPlayer(bool state);

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
	int scrubdie_inertia_y;
	int fallspeed;

	unsigned char walkdir;
	unsigned char kickedplayer[MAX_PLAYERS];
	std::vector<CPlayer>& m_Player;
};

#endif /* CSCRUB_H_ */
