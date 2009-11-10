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
	case 1: ShipFlyMarsToEarth(); break;
	case 2: BackAtHome(); break;
	case 3: ShipFlyEarthToMShip(); break;
	case 4: /*showEndingText(pCKP->Resources.GameDataDirectory);*/ m_step++; break;
	case 5:
		 if (g_pInput->getPressedAnyCommand()) m_step++;
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

		mp_Textbox = new CTextBox(150, 100, getstring("EP1_ESEQ_PART1"));
		mp_Textbox->setAttribs(2, LETTER_TYPE_NORMAL);
		mp_DlgFrame = new CDlgFrame(0, 142, 40, 7);

		m_mustsetup = false;
	}

	if( m_timepassed<50000 && !g_pInput->getPressedAnyCommand() )
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

void CEndingEp1::ShipFlyMarsToEarth()
{
	if(m_mustsetup)
	{
		//Initialization
		std::string path = mp_Map->m_gamepath;
		CMapLoader MapLoader(mp_Map, mp_Player);
		MapLoader.load(1, 81, path);

		mp_Player[0].x = (6<<CSF);
		mp_Player[0].y = (5<<CSF);

		mp_ShipFlySys = new CShipFlySys( mp_Player, mp_Map );

		mp_Map->gotoPos(0,0);
		mp_ShipFlySys->addShipQueue(CMD_MOVE, 60, DUP);
		mp_ShipFlySys->addShipQueue(CMD_WAIT, 12, 0);
		mp_ShipFlySys->addShipQueue(CMD_MOVE, 673, DDOWNRIGHT);
		mp_ShipFlySys->addShipQueue(CMD_WAIT, 25, 0);
		mp_ShipFlySys->addShipQueue(CMD_MOVE, 120, DDOWN);
		mp_ShipFlySys->addShipQueue(CMD_WAIT, 18, 0);
		mp_ShipFlySys->addShipQueue(CMD_SPAWNSPR, 0, SPR_QUESTION);
		mp_ShipFlySys->addShipQueue(CMD_DISABLESCROLLING, 0, 0);
		mp_ShipFlySys->addShipQueue(CMD_WAIT, 88, 0);
		mp_ShipFlySys->addShipQueue(CMD_REMOVESPR, 0, 0);
		mp_ShipFlySys->addShipQueue(CMD_WAIT, 12, 0);
		mp_ShipFlySys->addShipQueue(CMD_MOVE, 175, DLEFT);
		mp_ShipFlySys->addShipQueue(CMD_WAIT, 38, 0);
		mp_ShipFlySys->addShipQueue(CMD_SPAWNSPR, 0, SPR_EXCLAMATION);
		mp_ShipFlySys->addShipQueue(CMD_WAIT, 125, 0);
		mp_ShipFlySys->addShipQueue(CMD_REMOVESPR, 0, 0);
		mp_ShipFlySys->addShipQueue(CMD_WAIT, 12, 0);
		mp_ShipFlySys->addShipQueue(CMD_MOVE, 175, DRIGHT);
		mp_ShipFlySys->addShipQueue(CMD_WAIT, 6, 0);
		mp_ShipFlySys->addShipQueue(CMD_ENABLESCROLLING, 0, 0);
		mp_ShipFlySys->addShipQueue(CMD_MOVE, 116, DDOWN);
		mp_ShipFlySys->addShipQueue(CMD_FADEOUT, 0, 0);
		mp_ShipFlySys->addShipQueue(CMD_MOVE, 25, DDOWN);
		mp_ShipFlySys->addShipQueue(CMD_ENDOFQUEUE, 0, 0);
		mp_Map->drawAll();
		mp_ShipFlySys->m_ShipQueuePtr = 0;

		m_mustsetup = false;
	}

	if( !mp_ShipFlySys->EndOfQueue() || g_pInput->getPressedAnyCommand() )
	{
		// process the normal ship flying level and do all the inited commands
		mp_ShipFlySys->process();
	}
	else
	{
		// Shutdown code here!
		delete mp_ShipFlySys;
		mp_ShipFlySys = NULL;
		m_step++;
		m_mustsetup = true;
	}
}

void CEndingEp1::ShipFlyEarthToMShip()
{
	if(m_mustsetup)
	{
		//Initialization
		std::string path = mp_Map->m_gamepath;
		CMapLoader MapLoader(mp_Map, mp_Player);
		MapLoader.load(1, 81, path);

		mp_Player[0].x = (48<<CSF);
		mp_Player[0].y = (23<<CSF);

		mp_ShipFlySys = new CShipFlySys( mp_Player, mp_Map );

		mp_ShipFlySys->addShipQueue(CMD_MOVE, 58, DUP);
		mp_ShipFlySys->addShipQueue(CMD_DISABLESCROLLING, 0, 0);
		mp_ShipFlySys->addShipQueue(CMD_WAIT, 13, DUPLEFT);
		mp_ShipFlySys->addShipQueue(CMD_MOVE, 208, DLEFT);
		mp_ShipFlySys->addShipQueue(CMD_FADEOUT, 0, 0);
		mp_ShipFlySys->addShipQueue(CMD_MOVE, 25, DDOWN);
		mp_ShipFlySys->addShipQueue(CMD_ENDOFQUEUE, 0, 0);

		mp_Map->drawAll();
		mp_ShipFlySys->m_ShipQueuePtr = 0;

		m_mustsetup = false;
	}

	if( !mp_ShipFlySys->EndOfQueue() || g_pInput->getPressedAnyCommand() )
	{
		// process the normal ship flying level and do all the inited commands
		mp_ShipFlySys->process();
	}
	else
	{
		// Shutdown code here!
		delete mp_ShipFlySys;
		mp_ShipFlySys = NULL;
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
