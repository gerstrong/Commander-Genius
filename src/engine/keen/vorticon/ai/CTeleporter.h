/*
 * CTeleporter.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CTELEPORTER_H
#define CTELEPORTER_H

#include "../CVorticonSpriteObject.h"
#include "CPlayer.h"

enum teleportation_state{
TELEPORTING_OUT,
TELEPORTING_SCROLL,
TELEPORTING_IN
};

class CTeleporter : public CVorticonSpriteObject
{
public:
	CTeleporter(CMap *p_map, std::vector<CPlayer> &mp_vec_Player,
            const Uint32 x, const Uint32 y );

	void reset();
	
	void process();

	char animtimer;
	char animframe;
	char numframechanges;

	teleportation_state direction;
	int whichplayer;
    int destx;
    int desty;

	int baseframe;
	int idleframe;

	char fadeamt;
	char fadetimer;

private:
	std::vector<CPlayer> &m_Player;
};

#endif /* CTELEPORTER_H */
