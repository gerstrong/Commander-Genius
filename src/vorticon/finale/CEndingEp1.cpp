/*
 * CEndingEp1.cpp
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#include "CEndingEp1.h"
//#include "../../funcdefs.h"
#include "../../StringUtils.h"
#include "../../sdl/CTimer.h"
#include "../../sdl/CInput.h"
#include "../../sdl/CVideoDriver.h"
#include "../../graphics/CGfxEngine.h"
#include "../../common/CMapLoader.h"
#include "../../common/Playerdefines.h"

CEndingEp1::CEndingEp1(CMap *p_map, std::vector<CPlayer> &Player) :
	m_Player(Player)
{
	m_Episode = 1;
	mp_Map = p_map;
	m_step = 0;
	m_starttime = g_pTimer->getTicks();
	m_timepassed = 0;
	m_mustsetup = true;
	m_mustfinishgame = false;
	mp_Textbox = NULL;
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
	case 4: showEndingText(); break;
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
		m_Player[0].hideplayer = false;
		m_Player[0].x = 6636;
		m_Player[0].y = 19968;
		m_Player[0].playframe = PMAPLEFTFRAME;

		while(m_Player[0].scrollTriggers()); // Scroll the map to players position

		mp_Textbox = new CMessageBox(getstring("EP1_ESEQ_PART1"), true);
		//mp_Textbox->setAttribs(2, LETTER_TYPE_NORMAL);

		m_mustsetup = false;
	}

	if( m_timepassed<50000 && !g_pInput->getPressedAnyCommand() )
	{
		// perform a machine typing like dialog.
		mp_Textbox->process();
	}
	else
	{
		// Shutdown code here!
		m_step++;
		m_mustsetup = true;
		delete mp_Textbox;
		mp_Textbox = NULL;
	}
}

void CEndingEp1::ShipFlyMarsToEarth()
{
	if(m_mustsetup)
	{
		//Initialization
		std::string path = mp_Map->m_gamepath;
		CMapLoader MapLoader(mp_Map, &m_Player[0]);
		MapLoader.load(1, 81, path);

		m_Player[0].hideplayer = false;
		m_Player[0].x = (6<<CSF);
		m_Player[0].y = (5<<CSF);

		mp_ShipFlySys = new CShipFlySys( m_Player[0], mp_Map, SPR_SHIP_RIGHT, SPR_SHIP_LEFT);

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

	if( !mp_ShipFlySys->EndOfQueue() && !g_pInput->getPressedAnyCommand() )
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
	if(m_mustsetup)
	{
		//Initialization
		mp_Map->gotoPos(0,0);
		mp_Map->resetScrolls(); // The Scrollsurface must be (0,0) so the bitmap is correctly drawn
		mp_Map->m_animation_enabled = false; // Needed, because the other map is still loaded
		m_Player[0].hideplayer = true;
		mp_FinaleStaticScene = new CFinaleStaticScene(mp_Map->m_gamepath, "finale.ck1");

		mp_FinaleStaticScene->push_string("EP1_ESEQ_PART2_PAGE1", 6000);
		mp_FinaleStaticScene->push_string("EP1_ESEQ_PART2_PAGE2", 6000);
		mp_FinaleStaticScene->push_string("EP1_ESEQ_PART2_PAGE3", 6000);
		mp_FinaleStaticScene->push_string("EP1_ESEQ_PART2_PAGE4", 5000);
		mp_FinaleStaticScene->push_string("EP1_ESEQ_PART2_PAGE5", 5000);
		mp_FinaleStaticScene->push_string("EP1_ESEQ_PART2_PAGE6", 6000);
		mp_FinaleStaticScene->push_string("EP1_ESEQ_PART2_PAGE7", 6000);
		mp_FinaleStaticScene->push_string("EP1_ESEQ_PART2_PAGE8", 8000);

		// The Bitmaps of the Window Lights on should drawn at Page 4
		mp_FinaleStaticScene->showBitmapAt("WINDON", 2, 6, 80, 0);
		mp_FinaleStaticScene->showBitmapAt("WINDOFF", 6, 8, 80, 0);

		m_mustsetup = false;
	}

	if( !mp_FinaleStaticScene->mustclose() )
	{
		mp_FinaleStaticScene->process();
	}
	else
	{
		// Shutdown code here!
		delete mp_FinaleStaticScene;
		mp_FinaleStaticScene = NULL;
		mp_Map->m_animation_enabled = true;
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
		CMapLoader MapLoader(mp_Map, &m_Player[0]);
		MapLoader.load(1, 81, path);

		m_Player[0].hideplayer = false;
		m_Player[0].x = (48<<CSF);
		m_Player[0].y = (23<<CSF);

		mp_Map->gotoPos((m_Player[0].x>>STC)-100, (m_Player[0].y>>STC)-160);

		mp_ShipFlySys = new CShipFlySys( m_Player[0], mp_Map, SPR_SHIP_RIGHT, SPR_SHIP_LEFT);

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

	if( !mp_ShipFlySys->EndOfQueue() && !g_pInput->getPressedAnyCommand() )
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

CEndingEp1::~CEndingEp1() {
	// TODO Auto-generated destructor stub
}
