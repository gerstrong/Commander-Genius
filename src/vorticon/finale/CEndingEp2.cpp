/*
 * CEndingEp2.cpp
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#include "CEndingEp2.h"
#include "../../StringUtils.h"
#include "../../sdl/CTimer.h"
#include "../../sdl/CInput.h"
#include "../../sdl/CVideoDriver.h"
#include "../../graphics/CGfxEngine.h"
#include "../../common/CMapLoader.h"
#include "../../common/Playerdefines.h"

const int LIMPSHOME_X = 0;
const int LIMPSHOME_Y = 300;

#define SAFE_DELETE(x) if(x) { delete x; x=NULL; }

CEndingEp2::CEndingEp2(CMap *p_map, std::vector<CPlayer> &Player) :
m_Player(Player)
{
	m_Episode = 2;
	mp_Map = p_map;
	m_step = 0;
	m_starttime = g_pTimer->getTicks();
	m_timepassed = 0;
	m_mustsetup = true;
	m_mustfinishgame = false;

	mp_Textbox = NULL;
}

void CEndingEp2::process()
{
	m_timepassed = g_pTimer->getTicks() - m_starttime;

	switch(m_step)
	{
	case 0: HeadsForEarth(); break;
	case 1: LimpsHome(); break;
	case 2: SnowedOutside(); break;
	default:
		m_mustfinishgame = true;
		break;
	}
}

void CEndingEp2::HeadsForEarth()
{
	if(m_mustsetup)
	{
		//Initialization
		std::string path = mp_Map->m_gamepath;
		CMapLoader MapLoader(mp_Map, &m_Player);
		MapLoader.load(2, 81, path);

		m_Player[0].hideplayer = false;
		m_Player[0].sprite = SPR_VORTICON_MOTHERSHIP;

		mp_ShipFlySys = new CShipFlySys( m_Player[0], mp_Map, SPR_SHIP_RIGHT_EP2, SPR_SHIP_LEFT_EP2 );

		mp_Map->gotoPos(0, 0);
		mp_ShipFlySys->addShipQueue(CMD_WAIT, 10, 0);
		mp_ShipFlySys->addShipQueue(CMD_MOVE, 672, DDOWNRIGHT);
		mp_ShipFlySys->addShipQueue(CMD_MOVE, 150, DDOWN);
		mp_ShipFlySys->addShipQueue(CMD_ENDOFQUEUE, 0, 0);
		mp_Map->drawAll();
		mp_ShipFlySys->m_ShipQueuePtr = 0;

		int x, y;
		mp_Map->findTile(593, &x, &y);
		m_Player[0].moveTo(x<<CSF, y<<CSF);

		mp_Textbox = new CMessageBox(getstring("EP2_ESEQ_PART1"), true);

		m_mustsetup = false;
	}

	if(mp_Textbox)
	{
		mp_Textbox->process();

		if(mp_Textbox->isFinished())
		{
			SAFE_DELETE(mp_Textbox);
		}
	}
	else
	{	// process the normal ship flying level and do all the inited commands
		if( !mp_ShipFlySys->EndOfQueue() && !g_pInput->getPressedAnyCommand() )
		{
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
}

void CEndingEp2::LimpsHome()
{
	if(m_mustsetup)
	{	//Initialization
		std::string path = mp_Map->m_gamepath;
		CMapLoader MapLoader(mp_Map, &m_Player);
		MapLoader.load(2, 81, path);

		m_Player[0].hideplayer = false;
		m_Player[0].moveTo(8<<CSF, 26<<CSF);

		mp_ShipFlySys = new CShipFlySys( m_Player[0], mp_Map, SPR_VORTICON_MOTHERSHIP, SPR_VORTICON_MOTHERSHIP );

		mp_Map->gotoPos(LIMPSHOME_X, LIMPSHOME_Y);
		mp_ShipFlySys->addShipQueue(CMD_WAIT, 10, 0);
		mp_ShipFlySys->addShipQueue(CMD_MOVE, 80, DUPLEFT);
		mp_ShipFlySys->addShipQueue(CMD_ENDOFQUEUE, 0, 0);
		mp_Map->drawAll();
		mp_ShipFlySys->m_ShipQueuePtr = 0;

		mp_Textbox = new CMessageBox(getstring("EP2_ESEQ_PART2"), true);

		m_mustsetup = false;
	}


	if(mp_Textbox)
	{
		mp_Textbox->process();

		if(mp_Textbox->isFinished())
		{
			SAFE_DELETE(mp_Textbox);
		}
	}
	else
	{	// process the normal ship flying level and do all the inited commands
		if( !mp_ShipFlySys->EndOfQueue() & !g_pInput->getPressedAnyCommand() )
		{
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
}

void CEndingEp2::SnowedOutside()
{
	if(m_mustsetup)
	{
		//Initialization
		mp_Map->gotoPos(0,0);
		mp_Map->resetScrolls(); // The Scrollsurface must be (0,0) so the bitmap is correctly drawn
		mp_Map->m_animation_enabled = false; // Needed, because the other map is still loaded
		m_Player[0].hideplayer = true;
		mp_FinaleStaticScene = new CFinaleStaticScene(mp_Map->m_gamepath, "finale.ck2");

		mp_FinaleStaticScene->push_string("EP2_ESEQ_PART3_PAGE1", 6000);
		mp_FinaleStaticScene->push_string("EP2_ESEQ_PART3_PAGE2", 6000);
		mp_FinaleStaticScene->push_string("EP2_ESEQ_PART3_PAGE3", 6000);
		mp_FinaleStaticScene->push_string("EP2_ESEQ_PART3_PAGE4", 6000);

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

CEndingEp2::~CEndingEp2() {
	// TODO Auto-generated destructor stub
}


