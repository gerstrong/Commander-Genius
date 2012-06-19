/*
 * CEndingEp1.h
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#ifndef CENDINGEP1_H_
#define CENDINGEP1_H_

#include "CFinale.h"
#include "common/CMap.h"
#include "common/CPlayer.h"
#include "engine/vorticon/dialog/CMessageBoxVort.h"
#include "SmartPointer.h"
#include <vector>
#include <SDL.h>

class CEndingEp1 : public CFinale
{
public:
	CEndingEp1(std::list< SmartPointer<CMessageBoxVort> > &messageBoxes,
				const SmartPointer<CMap> &pMap,
				std::vector<CPlayer> &Player,
				bool &hideobjects,
				std::vector<CSpriteObject*> &Object);

	void process();


	void ReturnsToShip();
	void ShipFlyMarsToEarth();
	void BackAtHome();
	void ShipFlyEarthToMShip();

private:
	bool m_mustsetup;
	Uint32 m_starttime;
	Uint32 m_timepassed;

	std::vector<CPlayer> &m_Player;
	SmartPointer<CShipFlySys> mpShipFlySys;
	SmartPointer<CFinaleStaticScene> mpFinaleStaticScene;

	bool &m_hideobjects;
};

#endif /* CENDINGEP1_H_ */
