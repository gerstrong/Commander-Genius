/*
 * CEndingEp1.cpp
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#include "CEndingEp1.h"
#include "../../funcdefs.h"
#include "../../sdl/CTimer.h"
#include "../../sdl/CInput.h"
#include "../../sdl/CVideoDriver.h"
#include "../../common/CMapLoader.h"
#include "../../common/Playerdefines.h"

CEndingEp1::CEndingEp1(CMap *p_map, CPlayer *p_Player) : mp_Map(p_map), mp_Player(p_Player) {
	m_step = 0;
	m_starttime = g_pTimer->getTicks();
	m_timepassed = 0;
	m_mustsetup = true;
	m_mustfinishgame = false;
	mp_Textbox = NULL;

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
	default:
		m_mustfinishgame = true;
		break;
	}
}

void CEndingEp1::ReturnsToShip()
{
	if(m_mustsetup)
	{
		//Initialization
		mp_Map->gotoPos( 40, 540 );

  	    // draw keen next to his ship
		mp_Player[0].x = 6636;
		mp_Player[0].y = 19968;
		mp_Player[0].playframe = PMAPLEFTFRAME;

		while(mp_Player[0].scrollTriggers()); // Scroll the map to players position

		mp_Textbox = new CTextBox(158, 100, getstring("EP1_ESEQ_PART1"));
		mp_Textbox->setAttribs(2, LETTER_TYPE_NORMAL);
		mp_DlgFrame = new CDlgFrame(0, 150, 40, 7);

		m_mustsetup = false;
	}

	if( m_timepassed<100000 || g_pInput->getPressedAnyCommand() )
	{
		// perform a machine typing like dialog.
		mp_DlgFrame->draw(g_pVideoDriver->FGLayerSurface);
		mp_Textbox->process();
	}
	else
	{
		// Shutdown code here!
		m_step++;
		m_mustsetup = true;
		delete mp_Textbox;
		delete mp_DlgFrame;
		mp_Textbox = NULL;
		mp_DlgFrame = NULL;
	}
}

void CEndingEp1::ShipFlys(bool flyback)
{
	if(m_mustsetup)
	{
		//Initialization
		mp_ShipFlySys = new CShipFlySys( mp_Player, mp_Map );

		mp_ShipFlySys->setInitialPostion(SHIPFLY_X, SHIPFLY_Y);
		mp_ShipFlySys->addShipQueue(CMD_MOVE, 230, DUP);
		mp_ShipFlySys->addShipQueue(CMD_WAIT, 50, 0);
		mp_ShipFlySys->addShipQueue(CMD_MOVE, 2690, DDOWNRIGHT);
		mp_ShipFlySys->addShipQueue(CMD_WAIT, 100, 0);
		mp_ShipFlySys->addShipQueue(CMD_MOVE, 480, DDOWN);
		mp_ShipFlySys->addShipQueue(CMD_WAIT, 150, 0);
		mp_ShipFlySys->addShipQueue(CMD_SPAWNSPR, 0, SPR_QUESTION);
		mp_ShipFlySys->addShipQueue(CMD_DISABLESCROLLING, 0, 0);
		mp_ShipFlySys->addShipQueue(CMD_WAIT, 350, 0);
		mp_ShipFlySys->addShipQueue(CMD_REMOVESPR, 0, 0);
		mp_ShipFlySys->addShipQueue(CMD_WAIT, 50, 0);
		mp_ShipFlySys->addShipQueue(CMD_MOVE, 700, DLEFT);
		mp_ShipFlySys->addShipQueue(CMD_WAIT, 150, 0);
		mp_ShipFlySys->addShipQueue(CMD_SPAWNSPR, 0, SPR_EXCLAMATION);
		mp_ShipFlySys->addShipQueue(CMD_WAIT, 500, 0);
		mp_ShipFlySys->addShipQueue(CMD_REMOVESPR, 0, 0);
		mp_ShipFlySys->addShipQueue(CMD_WAIT, 50, 0);
		mp_ShipFlySys->addShipQueue(CMD_MOVE, 700, DRIGHT);
		mp_ShipFlySys->addShipQueue(CMD_WAIT, 25, 0);
		mp_ShipFlySys->addShipQueue(CMD_ENABLESCROLLING, 0, 0);
		mp_ShipFlySys->addShipQueue(CMD_MOVE, 465, DDOWN);
		mp_ShipFlySys->addShipQueue(CMD_FADEOUT, 0, 0);
		mp_ShipFlySys->addShipQueue(CMD_MOVE, 100, DDOWN);
		mp_ShipFlySys->addShipQueue(CMD_ENDOFQUEUE, 0, 0);

		m_mustsetup = false;
	}

	if( !mp_ShipFlySys->EndOfQueue() || g_pInput->getPressedAnyCommand() )
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

void CEndingEp1::BackAtHome()
{
	m_step++;
}

CEndingEp1::~CEndingEp1() {
	// TODO Auto-generated destructor stub
}
