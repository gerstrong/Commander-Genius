/*
 * se.h
 *
 *  Created on: 18.12.2009
 *      Author: gerstrong
 *  many different assorted things, many sector-effectors affect tiles
 *  in the level instead of being a sprite. named for the similiarity
 *  to the object in duke3d
 */

#ifndef SE_H_
#define SE_H_

#include "../../../common/CObject.h"

// "Sector Effector" types
enum sector_effector_type{
	SE_MORTIMER_ARM,
	SE_MORTIMER_LEG_LEFT,
	SE_MORTIMER_LEG_RIGHT,
	SE_MORTIMER_SPARK,
	SE_MORTIMER_HEART,
	SE_MORTIMER_ZAPSUP,
	SE_MORTIMER_RANDOMZAPS
};

class CSectorEffector : public CObject
{
public:
	CSectorEffector(CMap *p_map, Uint32 x, Uint32 y,
			std::vector<CPlayer>& Player, std::vector<CObject*>& Object, unsigned int se_type);
	void process();
	void se_mortimer_arm();
	void se_mortimer_spark();
	void se_mortimer_heart();
	void se_mortimer_zapsup();
	void se_mortimer_leg_left();
	void se_mortimer_leg_right();
	void se_mortimer_randomzaps();
	void set_mortimer_surprised(bool yes);

	void getTouchedBy(CObject &theObject);
	void getShotByRay(){};

//private:
	unsigned int setype;

	unsigned char state;
	unsigned int timer;
	unsigned int platx, platy;
	unsigned int dir;

	int counter,destroytiles;
	unsigned int frame;
	int mx,my;

	std::vector<CPlayer>& m_Player;
	std::vector<CObject*>& m_Object;
};

#endif /* SE_H_ */
