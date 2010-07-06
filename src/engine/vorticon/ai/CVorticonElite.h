/*
 * CVorticonElite.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CVORTICONELITE_H_
#define CVORTICONELITE_H_

#include "../../../common/CObject.h"

class CVorticonElite : public CObject
{
public:
	CVorticonElite( CMap *p_map, std::vector<CPlayer> &mp_vec_Player,
			Uint32 x, Uint32 y );
private:
};

#endif /* CVORTICONELITE_H_ */
