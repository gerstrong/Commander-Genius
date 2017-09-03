/*
 * CPlayerDive.h
 *
 *  Created on: 19.06.2011
 *      Author: gerstrong
 */

#ifndef CPLAYERDIVE_H_
#define CPLAYERDIVE_H_

#include "../../common/ai/CPlayerBase.h"

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
            const int playerID,
            const int spriteVar);

	void kill(const bool force = false);
	void process();

private:

	void processDiving();
	void processDead();

	int m_swimupspeed;
	int m_breathtimer;

    // This will ensure that the player presses multiple times the jump button to swim up...
    bool mDidSwimUp;
};

}

#endif /* CPLAYERDIVE_H_ */
