/*
 * CTeleporter.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef _CTELEPORTER_H_
#define _CTELEPORTER_H_

#include "engine/vorticon/CVorticonSpriteObject.h"
#include "common/CPlayer.h"

enum teleportation_state{
TELEPORTING_OUT,
TELEPORTING_SCROLL,
TELEPORTING_IN
};

class CTeleporter : public CVorticonSpriteObject
{
public:
	CTeleporter(CMap *p_map, std::vector<CPlayer> &mp_vec_Player,
			Uint32 x, Uint32 y );

	void reset();
	
	void process();

	char animtimer;
	char animframe;
	char numframechanges;

	teleportation_state direction;
	int whichplayer;
	unsigned int destx;
	signed int desty;

	int baseframe;
	int idleframe;

	char fadeamt;
	char fadetimer;

private:
	std::vector<CPlayer> &m_Player;
};

#endif /* _CTELEPORTER_H_ */
