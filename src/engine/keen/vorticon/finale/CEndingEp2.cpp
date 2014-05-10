/*
 * CEndingEp2.cpp
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#include "CEndingEp2.h"
#include <base/utils/StringUtils.h>
#include <base/GsTimer.h>
#include <base/CInput.h>
#include "graphics/GsGraphics.h"
#include "../CVorticonMapLoader.h"
#include "engine/core/Playerdefines.h"

const int LIMPSHOME_X = 0;
const int LIMPSHOME_Y = 300;

CEndingEp2::CEndingEp2(std::list< std::shared_ptr<CMessageBoxVort> > &messageBoxes,
		const std::shared_ptr<CMap> &pMap, std::vector<CPlayer> &Player, 
		std::vector< std::unique_ptr<CVorticonSpriteObject> > &Object ) :
CFinale(messageBoxes, pMap, Object),
m_Player(Player)
{
	m_Episode = 2;
	m_step = 0;
	m_starttime = gTimer.getTicks();
	m_timepassed = 0;
	m_mustsetup = true;
	m_mustfinishgame = false;
}

void CEndingEp2::ponder()
{
	m_timepassed = gTimer.getTicks() - m_starttime;

	switch(m_step)
	{
    case 0: HeadsForEarth(); break;
	case 1: LimpsHome(); break;
	case 2: SnowedOutside(); break;
	default:
		m_mustfinishgame = true;
		break;
	}

    m_Player[0].processEvents();
}

void CEndingEp2::render()
{
    m_timepassed = gTimer.getTicks() - m_starttime;

    switch(m_step)
    {
    //case 0: HeadsForEarth(); break;
    //case 1: LimpsHome(); break;
    case 2: SnowedOutsideRender(); break;
    default: break;
    }
}

void CEndingEp2::HeadsForEarth()
{
	if(m_mustsetup)
	{
		//Initialization
		std::string path = mpMap->m_gamepath;
		std::vector< std::unique_ptr<CVorticonSpriteObject> > mpSpriteObjectContainer;
		CVorticonMapLoaderWithPlayer MapLoader(mpMap, m_Player, mpSpriteObjectContainer);
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
		m_Player[0].moveTo(Vector2D<int>(x<<CSF, y<<CSF));

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
		std::vector< std::unique_ptr<CVorticonSpriteObject> > mpSpriteObjectContainer;
		CVorticonMapLoaderWithPlayer MapLoader(mpMap, m_Player, mpSpriteObjectContainer);
		MapLoader.load(2, 81, path);

		m_Player[0].hideplayer = false;
		m_Player[0].moveTo(Vector2D<int>(8<<CSF, 26<<CSF));
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

        addMsgBoxString("EP2_ESEQ_PART3_PAGE1");
        addMsgBoxString("EP2_ESEQ_PART3_PAGE2");
        addMsgBoxString("EP2_ESEQ_PART3_PAGE3");
        addMsgBoxString("EP2_ESEQ_PART3_PAGE4");


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
        mpFinaleStaticScene->ponder();
	}

}

void CEndingEp2::SnowedOutsideRender()
{
    if( !mMessageBoxes.empty() )
    {
        mpFinaleStaticScene->render(m_step);
    }
}


