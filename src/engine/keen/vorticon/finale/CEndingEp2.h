/*
 * CEndingEp2.h
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#ifndef CENDINGEP2_H_
#define CENDINGEP2_H_

#include "CFinale.h"
#include "engine/core/CMap.h"
#include "../ai/CPlayer.h"
#include "../dialog/CMessageBoxVort.h"
#include <vector>
#include <SDL.h>

class CEndingEp2 : public CFinale
{
public:
	CEndingEp2(std::list< std::shared_ptr<CMessageBoxVort> > &messageBoxes,
			const std::shared_ptr<CMap> &pMap, 
			std::vector<CPlayer> &Player, 
			std::vector< std::unique_ptr<CVorticonSpriteObject> > &Object );

    void ponder();
    void render();

    // ponder part
	void HeadsForEarth();
	void LimpsHome();
	void SnowedOutside();

    // render part
    void SnowedOutsideRender();

private:
	bool m_mustsetup;
	Uint32 m_starttime;
	Uint32 m_timepassed;


	std::vector<CPlayer> &m_Player;
	std::unique_ptr<CShipFlySys> mpShipFlySys;
	std::unique_ptr<CFinaleStaticScene> mpFinaleStaticScene;
};

#endif /* CENDINGEP2_H_ */
