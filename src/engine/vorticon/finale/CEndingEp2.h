/*
 * CEndingEp2.h
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#ifndef CENDINGEP2_H_
#define CENDINGEP2_H_

#include "CFinale.h"
#include "common/CMap.h"
#include "common/CPlayer.h"
#include "engine/vorticon/dialog/CMessageBoxVort.h"
#include <vector>
#include <SDL.h>

class CEndingEp2 : public CFinale
{
public:
	CEndingEp2(std::list< SmartPointer<CMessageBoxVort> > &messageBoxes,
				const SmartPointer<CMap> &pMap, std::vector<CPlayer> &Player, std::vector<CObject*> &Object);

	void process();
	void HeadsForEarth();
	void LimpsHome();
	void SnowedOutside();


private:
	bool m_mustsetup;
	Uint32 m_starttime;
	Uint32 m_timepassed;


	std::vector<CPlayer> &m_Player;
	SmartPointer<CShipFlySys> mpShipFlySys;
	SmartPointer<CFinaleStaticScene> mpFinaleStaticScene;

	//CMessageBoxVort *mp_Textbox;
};

#endif /* CENDINGEP2_H_ */
