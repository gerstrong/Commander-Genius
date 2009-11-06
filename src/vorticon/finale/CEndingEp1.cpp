/*
 * CEndingEp1.cpp
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#include "CEndingEp1.h"
#include "../../sdl/CTimer.h"
#include "../../sdl/CInput.h"
#include "../../common/CMapLoader.h"
#include "../../common/Playerdefines.h"

CEndingEp1::CEndingEp1(CMap *p_map, CPlayer *p_Player) : mp_Map(p_map), mp_Player(p_Player) {
	m_step = 0;
	m_starttime = g_pTimer->getTicks();
	m_timepassed = 0;
	m_mustsetup = true;

	CFinale::init_ToBeContinued();
}

void CEndingEp1::process()
{
	m_timepassed = g_pTimer->getTicks() - m_starttime;

	switch(m_step)
	{
	case 0: ReturnsToShip(); break;
	case 1: ShipFlys(false); break;
	case 2: BackAtHome(); break;
	case 3: ShipFlys(true); break;
	case 4: /*showEndingText(pCKP->Resources.GameDataDirectory);*/ m_step++; break;
	case 5:
		 if (g_pInput->getPressedAnyCommand()) m_step++;
		 g_pInput->pollEvents();
		 //eseq_showmsg(text, dlgX, dlgY, dlgW, dlgH, 0, extrascreen);
		 break;
	}
}

void CEndingEp1::ReturnsToShip()
{
	if(m_mustsetup)
	{
		//Initialization
		//while(mp_Map->gotoPos( 40, 540 ););   // Scroll the map to players position

  	    // draw keen next to his ship
		mp_Player[0].x = 6636;
		mp_Player[0].y = 19968;
		mp_Player[0].playframe = PMAPLEFTFRAME;

		m_mustsetup = false;
	}

	if( m_timepassed<10000 )
	{
		// perform a machine typing like dialog.
	}
	else
	{
		// Shutdown code here!
		m_step++;
		m_mustsetup = true;
	}
}

void CEndingEp1::ShipFlys(bool flyback)
{

}

void CEndingEp1::BackAtHome()
{

}

CEndingEp1::~CEndingEp1() {
	// TODO Auto-generated destructor stub
}
