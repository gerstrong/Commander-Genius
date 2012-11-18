/*
 * CEndingEp2.cpp
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#include "CEndingEp2.h"
#include "StringUtils.h"
#include "sdl/CTimer.h"
#include "sdl/input/CInput.h"
#include "sdl/CVideoDriver.h"
#include "graphics/CGfxEngine.h"
#include "common/CVorticonMapLoader.h"
#include "common/Playerdefines.h"

const int LIMPSHOME_X = 0;
const int LIMPSHOME_Y = 300;

CEndingEp2::CEndingEp2(std::list< SmartPointer<CMessageBoxVort> > &messageBoxes,
		const std::shared_ptr<CMap> &pMap, std::vector<CPlayer> &Player, std::vector<CVorticonSpriteObject*> &Object) :
CFinale(messageBoxes, pMap, Object),
m_Player(Player)
{
	m_Episode = 2;
	m_step = 0;
	m_starttime = g_pTimer->getTicks();
	m_timepassed = 0;
	m_mustsetup = true;
	m_mustfinishgame = false;
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
		std::string path = mpMap->m_gamepath;
		CVorticonMapLoader MapLoader(mpMap, &m_Player);
		MapLoader.load(2, 81, path);

		m_Player[0].hideplayer = false;
		m_Player[0].sprite = SPR_VORTICON_MOTHERSHIP;
		m_Player[0].solid = false;

		mpShipFlySys.reset( new CShipFlySys( m_Player[0], mpMap, SPR_SHIP_RIGHT_EP2, SPR_SHIP_LEFT_EP2 ) );

		mpMap->gotoPos(0, 0);
		mpShipFlySys->addShipQueue(CMD_WAIT, 10, 0);
		mpShipFlySys->addShipQueue(CMD_MOVE, 672, DDOWNRIGHT);
		mpShipFlySys->addShipQueue(CMD_MOVE, 150, DDOWN);
		mpShipFlySys->addShipQueue(CMD_ENDOFQUEUE, 0, 0);
		mpMap->drawAll();
		mpShipFlySys->m_ShipQueuePtr = 0;

		int x, y;
		mpMap->findTile(593, &x, &y);
		m_Player[0].moveTo(VectorD2<int>(x<<CSF, y<<CSF));

		addMsgBoxString("EP2_ESEQ_PART1");

		m_mustsetup = false;
	}


	// process the normal ship flying level and do all the inited commands
	if( !mpShipFlySys->EndOfQueue() || !mMessageBoxes.empty() )
	{
		mpShipFlySys->process();
	}
	else
	{
		// Shutdown code here!
		mpShipFlySys.release();
		m_step++;
		m_mustsetup = true;
	}

}

void CEndingEp2::LimpsHome()
{
	if(m_mustsetup)
	{	//Initialization
		std::string path = mpMap->m_gamepath;
		CVorticonMapLoader MapLoader(mpMap, &m_Player);
		MapLoader.load(2, 81, path);

		m_Player[0].hideplayer = false;
		m_Player[0].moveTo(VectorD2<int>(8<<CSF, 26<<CSF));
		m_Player[0].solid = false;

		mpShipFlySys.reset( new CShipFlySys( m_Player[0], mpMap, SPR_VORTICON_MOTHERSHIP, SPR_VORTICON_MOTHERSHIP ) );

		mpMap->gotoPos(LIMPSHOME_X, LIMPSHOME_Y);
		mpShipFlySys->addShipQueue(CMD_WAIT, 10, 0);
		mpShipFlySys->addShipQueue(CMD_MOVE, 80, DUPLEFT);
		mpShipFlySys->addShipQueue(CMD_ENDOFQUEUE, 0, 0);
		mpMap->drawAll();
		mpShipFlySys->m_ShipQueuePtr = 0;

		addMsgBoxString("EP2_ESEQ_PART2");

		m_mustsetup = false;
	}


	// process the normal ship flying level and do all the inited commands
	if( !mpShipFlySys->EndOfQueue() || !mMessageBoxes.empty() )
	{
		mpShipFlySys->process();
	}
	else
	{
		// Shutdown code here!
		mpShipFlySys.release();
		m_step++;
		m_mustsetup = true;
	}

}

void CEndingEp2::SnowedOutside()
{
	if(m_mustsetup)
	{
		//Initialization
		mpMap->gotoPos(0,0);
		mpMap->resetScrolls(); // The Scrollsurface must be (0,0) so the bitmap is correctly drawn
		mpMap->m_animation_enabled = false; // Needed, because the other map is still loaded
		m_Player[0].hideplayer = true;
		mpFinaleStaticScene.reset( new CFinaleStaticScene( mpMap->m_gamepath, "finale.ck2" ) );

		addMsgBoxString("EP2_ESEQ_PART1");
		addMsgBoxString("EP2_ESEQ_PART2");
		addMsgBoxString("EP2_ESEQ_PART3");
		addMsgBoxString("EP2_ESEQ_PART4");


		m_mustsetup = false;
	}

	if( mMessageBoxes.empty() )
	{
		// Shutdown code here!
		mpFinaleStaticScene.release();
		mpMap->m_animation_enabled = true;
		m_step++;
		m_mustsetup = true;
	}
	else
	{
		mpFinaleStaticScene->process();
	}

}


