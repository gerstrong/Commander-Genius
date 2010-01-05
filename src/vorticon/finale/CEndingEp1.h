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
#include "../../dialog/CMessageBox.h"
#include <vector>
#include <SDL.h>

class CEndingEp1 : public CFinale {
public:
	CEndingEp1(CMap &map, std::vector<CPlayer> &Player);
	void process();

	void ReturnsToShip();
	void ShipFlyMarsToEarth();
	void BackAtHome();
	void ShipFlyEarthToMShip();

	virtual ~CEndingEp1();

private:
	bool m_mustsetup;
	Uint32 m_starttime;
	Uint32 m_timepassed;

	std::vector<CPlayer> &m_Player;
	CShipFlySys *mp_ShipFlySys;
	CFinaleStaticScene *mp_FinaleStaticScene;
	CMessageBox *mp_Textbox;
};

#endif /* CENDINGEP1_H_ */
