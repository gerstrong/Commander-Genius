/*
 * se.h
 *
 *  Created on: 18.12.2009
 *      Author: gerstrong
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
	CSectorEffector(CMap *p_map);
};

#endif /* SE_H_ */
