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
#include "../../dialog/CTextBox.h"
#include "../../dialog/CDlgFrame.h"
#include <SDL/SDL.h>

class CEndingEp1 : public CFinale {
public:
	CEndingEp1(CMap *p_map, CPlayer *p_Player);
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

	CPlayer *mp_Player;
	CShipFlySys *mp_ShipFlySys;
	CFinaleStaticScene *mp_FinaleStaticScene;
	CTextBox *mp_Textbox;
	CDlgFrame *mp_DlgFrame;
};

#endif /* CENDINGEP1_H_ */
