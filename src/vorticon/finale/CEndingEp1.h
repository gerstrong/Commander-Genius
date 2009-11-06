/*
 * CEndingEp1.h
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#ifndef CENDINGEP1_H_
#define CENDINGEP1_H_

#include "CFinale.h"
#include "../../common/CMap.h"
#include "../../common/CPlayer.h"
#include <SDL.h>

class CEndingEp1 : public CFinale {
public:
	CEndingEp1(CMap *p_map, CPlayer *p_Player);
	void process();

	void ReturnsToShip();
	void ShipFlys(bool flyback);
	void BackAtHome();

	virtual ~CEndingEp1();

private:
	char m_step;
	bool m_mustsetup;
	Uint32 m_starttime;
	Uint32 m_timepassed;

	CMap *mp_Map;
	CPlayer *mp_Player;
};

#endif /* CENDINGEP1_H_ */
