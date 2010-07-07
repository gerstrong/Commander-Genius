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
	SE_EXTEND_PLATFORM,
	SE_RETRACT_PLATFORM,
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
	CSectorEffector(CMap *p_map, Uint32 x, Uint32 y);
	void process();

private:
	unsigned int setype;

	unsigned char state;
	unsigned int timer;
	unsigned int platx, platy;
	unsigned int dir;

	int counter,destroytiles;
	unsigned int frame;
	int mx,my;
	int blowx,blowy;
};

#endif /* SE_H_ */
