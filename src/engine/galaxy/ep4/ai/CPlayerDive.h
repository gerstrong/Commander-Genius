/*
 * CPlayerDive.h
 *
 *  Created on: 19.06.2011
 *      Author: gerstrong
 */

#ifndef CPLAYERDIVE_H_
#define CPLAYERDIVE_H_

#include "engine/galaxy/common/ai/CPlayerBase.h"

namespace galaxy {

class CPlayerDive : public CPlayerBase
{
public:
	CPlayerDive(CMap *pmap,
			const Uint16 foeID,
			Uint32 x,
			Uint32 y,
			direction_t facedir,
			CInventory &l_Inventory,
			stCheat &Cheatmode);

	void kill(const bool force = false);
	void process();

private:


	void processDiving();
	void processDead();

	int m_swimupspeed;
	int m_breathtimer;
};

}

#endif /* CPLAYERDIVE_H_ */
