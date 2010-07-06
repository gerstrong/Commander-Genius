/*
 * CVortiKid.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CVORTIKID_H_
#define CVORTIKID_H_

#include "../../../common/CObject.h"

class CVortikid : public CObject
{
public:
	CVortikid( CMap *p_map, std::vector<CPlayer> &mp_vec_Player,
			Uint32 x, Uint32 y );
};

#endif /* CVORTIKID_H_ */
