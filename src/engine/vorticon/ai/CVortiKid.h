/*
 * CVortiKid.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CVORTIKID_H_
#define CVORTIKID_H_

#include "../../../common/CObject.h"
#include "../../../common/CPlayer.h"
#include <vector>

enum vort_baby_actions{
	BABY_RUN, BABY_DYING
};

class CVortikid : public CObject
{
public:
	CVortikid( CMap *p_map, std::vector<CPlayer> &mp_vec_Player,
			Uint32 x, Uint32 y );
	void process();
	void baby_jump(int big);
private:
	vort_baby_actions state;
	direction_t dir;
	signed int inertia_x, inertia_y;
	int jumpdectimer, xdectimer;
	int dietimer;

	char walkframe;
	int walktimer;

	std::vector<CPlayer> &m_Player;
};

#endif /* CVORTIKID_H_ */
