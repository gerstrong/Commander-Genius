/*
 * CEndingEp1.h
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#ifndef CENDINGEP1_H_
#define CENDINGEP1_H_

#include "CFinale.h"
#include "engine/core/CMap.h"
#include "../ai/CPlayer.h"
#include "../dialog/CMessageBoxVort.h"
#include <memory>
#include <vector>
#include <SDL.h>

class CEndingEp1 : public CFinale
{
public:
	CEndingEp1(std::list< std::shared_ptr<CMessageBoxVort> > &messageBoxes,
				const std::shared_ptr<CMap> &pMap,
				std::vector<CPlayer> &Player,
				bool &hideobjects,
				std::vector< std::unique_ptr< CVorticonSpriteObject> > &Object);

    void ponder();
    void render();


	void ReturnsToShip();
	void ShipFlyMarsToEarth();
	void BackAtHome();

    void renderBackAtHome();

	void ShipFlyEarthToMShip();

private:
	bool m_mustsetup;
	Uint32 m_starttime;
	Uint32 m_timepassed;

	std::vector<CPlayer> &m_Player;
	std::unique_ptr<CShipFlySys> mpShipFlySys;
	std::unique_ptr<CFinaleStaticScene> mpFinaleStaticScene;

	bool &m_hideobjects;
};

#endif /* CENDINGEP1_H_ */
