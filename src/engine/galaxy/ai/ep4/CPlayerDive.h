/*
 * CPlayerDive.h
 *
 *  Created on: 19.06.2011
 *      Author: gerstrong
 */

#ifndef CPLAYERDIVE_H_
#define CPLAYERDIVE_H_

#include "CPlayerBase.h"

namespace galaxy {

#define A_KEENSWIM_MOVE	0

class CPlayerDive: public CPlayerBase {
public:
	CPlayerDive(CMap *pmap,
			Uint32 x,
			Uint32 y,
			std::vector<CObject*>& ObjectPtrs,
			direction_t facedir,
			CInventory &l_Inventory,
			stCheat &Cheatmode);

	void process();

private:
	int m_swimupspeed;
};

}

#endif /* CPLAYERDIVE_H_ */
