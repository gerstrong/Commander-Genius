/*
 * CEndingEp2.h
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#ifndef CENDINGEP2_H_
#define CENDINGEP2_H_

#include "CFinale.h"
#include "../../../common/CMap.h"
#include "../../../common/CPlayer.h"
#include "../../../dialog/CMessageBox.h"
#include <vector>
#include <SDL.h>

class CEndingEp2 : public CFinale
{
public:
	CEndingEp2(CMap &map, std::vector<CPlayer> &Player);

	void process();
	void HeadsForEarth();
	void LimpsHome();
	void SnowedOutside();

	virtual ~CEndingEp2();

private:
	bool m_mustsetup;
	Uint32 m_starttime;
	Uint32 m_timepassed;

	std::vector<CPlayer> &m_Player;
	CShipFlySys *mp_ShipFlySys;
	CFinaleStaticScene *mp_FinaleStaticScene;

	CMessageBox *mp_Textbox;
};

#endif /* CENDINGEP2_H_ */
