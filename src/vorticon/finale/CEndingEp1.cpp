/*
 * CEndingEp1.cpp
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#include "CEndingEp1.h"
#include "../../sdl/CTimer.h"
#include "../../sdl/CInput.h"

CEndingEp1::CEndingEp1() {
	m_step = 0;
	m_starttime = g_pTimer->getTicks();
	m_timepassed = 0;

	CFinale::init_ToBeContinued();
}

void CEndingEp1::process()
{
	m_timepassed = g_pTimer->getTicks() - m_starttime;

	switch(m_step)
	{
	case 0: ReturnsToShip(); m_step++; break;
	case 1: ShipFlys(false); m_step++; break;
	case 2: BackAtHome(); m_step++; break;
	case 3: ShipFlys(true); m_step++; break;
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
