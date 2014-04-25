/*
 * CEndingEp1.cpp
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#include "CEndingEp1.h"
#include <base/utils/StringUtils.h>
#include <base/GsTimer.h>
#include <base/CInput.h>
#include "graphics/GsGraphics.h"
#include "../CVorticonMapLoader.h"
#include "engine/core/Playerdefines.h"

CEndingEp1::CEndingEp1(std::list< std::shared_ptr<CMessageBoxVort> > &messageBoxes,
						const std::shared_ptr<CMap> &pMap, std::vector<CPlayer> &Player,
						bool &hideobjects, 
						std::vector< std::unique_ptr<CVorticonSpriteObject> > &Object) :
CFinale(messageBoxes, pMap, Object),
m_Player(Player),
m_hideobjects(hideobjects)
{
	m_Episode = 1;
	m_step = 0;
	m_starttime = gTimer.getTicks();
	m_timepassed = 0;
	m_mustsetup = true;
	m_mustfinishgame = false;
}

void CEndingEp1::ponder()
{
	m_timepassed = gTimer.getTicks() - m_starttime;

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

    m_Player[0].processEvents();
}

void CEndingEp1::render()
{    
    switch(m_step)
    {
    case 2: renderBackAtHome(); break;
    case 4: renderEndingText(); break;
    default: break;
    }
}


void CEndingEp1::ReturnsToShip()
{
	if(m_mustsetup)
	{
		//Initialization
		mpMap->gotoPos( 40, 540 );

  	    // draw keen next to his ship
		m_Player[0].hideplayer = false;
		m_Player[0].solid = false;
		m_Player[0].moveTo(Vector2D<int>(6636, 19968));
		m_Player[0].sprite = PMAPLEFTFRAME;
        m_Player[0].processEvents();

		addMsgBoxString("EP1_ESEQ_PART1");

		m_mustsetup = false;
	}

	if(m_Player[0].mpCamera->m_moving)
	{
		m_starttime = gTimer.getTicks();
		m_Player[0].processCamera();
		return;
	}


	if( mMessageBoxes.empty() )
	{
		// Shutdown code here!
		m_step++;
		m_mustsetup = true;
	}
}

void CEndingEp1::ShipFlyMarsToEarth()
{
	if(m_mustsetup)
	{
		//Initialization
		std::string path = mpMap->m_gamepath;
		CVorticonMapLoaderWithPlayer MapLoader(mpMap, m_Player, m_Object);
		MapLoader.load(1, 81, path);

		m_Player[0].hideplayer = false;
		m_Player[0].moveTo(Vector2D<int>(6<<CSF, 5<<CSF));

		mpShipFlySys.reset( new CShipFlySys( m_Player[0], mpMap, SPR_SHIP_RIGHT, SPR_SHIP_LEFT) );

		mpMap->gotoPos(0,0);
		mpShipFlySys->addShipQueue(CMD_MOVE, 60, DUP);
		mpShipFlySys->addShipQueue(CMD_WAIT, 12, 0);
		mpShipFlySys->addShipQueue(CMD_MOVE, 673, DDOWNRIGHT);
		mpShipFlySys->addShipQueue(CMD_WAIT, 25, 0);
		mpShipFlySys->addShipQueue(CMD_MOVE, 120, DDOWN);
		mpShipFlySys->addShipQueue(CMD_WAIT, 18, 0);
		mpShipFlySys->addShipQueue(CMD_SPAWNSPR, 0, SPR_QUESTION);
		mpShipFlySys->addShipQueue(CMD_DISABLESCROLLING, 0, 0);
		mpShipFlySys->addShipQueue(CMD_WAIT, 88, 0);
		mpShipFlySys->addShipQueue(CMD_REMOVESPR, 0, 0);
		mpShipFlySys->addShipQueue(CMD_WAIT, 12, 0);
		mpShipFlySys->addShipQueue(CMD_MOVE, 175, DLEFT);
		mpShipFlySys->addShipQueue(CMD_WAIT, 38, 0);
		mpShipFlySys->addShipQueue(CMD_SPAWNSPR, 0, SPR_EXCLAMATION);
		mpShipFlySys->addShipQueue(CMD_WAIT, 125, 0);
		mpShipFlySys->addShipQueue(CMD_REMOVESPR, 0, 0);
		mpShipFlySys->addShipQueue(CMD_WAIT, 12, 0);
		mpShipFlySys->addShipQueue(CMD_MOVE, 175, DRIGHT);
		mpShipFlySys->addShipQueue(CMD_WAIT, 6, 0);
		mpShipFlySys->addShipQueue(CMD_ENABLESCROLLING, 0, 0);
		mpShipFlySys->addShipQueue(CMD_MOVE, 116, DDOWN);
		mpShipFlySys->addShipQueue(CMD_FADEOUT, 0, 0);
		mpShipFlySys->addShipQueue(CMD_MOVE, 25, DDOWN);
		mpShipFlySys->addShipQueue(CMD_ENDOFQUEUE, 0, 0);
		mpMap->drawAll();
		mpShipFlySys->m_ShipQueuePtr = 0;

		m_mustsetup = false;
	}

	if( !mpShipFlySys->EndOfQueue() && mMessageBoxes.empty() )
	{
		// process the normal ship flying level and do all the inited commands
		mpShipFlySys->process();
	}
	else
	{
		// Shutdown code here!
        mpShipFlySys = nullptr;
		m_step++;
		m_mustsetup = true;
	}
}


void CEndingEp1::BackAtHome()
{
    if(m_mustsetup)
    {
        //Initialization
        m_hideobjects = true;
        mpMap->gotoPos(0,0);
        mpMap->resetScrolls(); // The Scrollsurface must be (0,0) so the bitmap is correctly drawn
        mpMap->m_animation_enabled = false; // Needed, because the other map is still loaded
        m_Player[0].hideplayer = true;
        mpFinaleStaticScene.reset( new CFinaleStaticScene(mpMap->m_gamepath, "finale.ck1") );

        addMsgBoxString("EP1_ESEQ_PART2_PAGE1");
        addMsgBoxString("EP1_ESEQ_PART2_PAGE2");
        addMsgBoxString("EP1_ESEQ_PART2_PAGE3");
        addMsgBoxString("EP1_ESEQ_PART2_PAGE4");
        addMsgBoxString("EP1_ESEQ_PART2_PAGE5");
        addMsgBoxString("EP1_ESEQ_PART2_PAGE6");
        addMsgBoxString("EP1_ESEQ_PART2_PAGE7");
        addMsgBoxString("EP1_ESEQ_PART2_PAGE8");

        // The Bitmaps of the Window Lights on should drawn at Page 4
        mpFinaleStaticScene->showBitmapAt("WINDON", 2, 6, 80, 0);
        mpFinaleStaticScene->showBitmapAt("WINDOFF", 6, 8, 80, 0);

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

void CEndingEp1::renderBackAtHome()
{
    if( !mMessageBoxes.empty() )
    {
        mpFinaleStaticScene->render(8-mMessageBoxes.size());
    }

}

void CEndingEp1::ShipFlyEarthToMShip()
{
	if(m_mustsetup)
	{	//Initialization
		int x, y;
		m_hideobjects = false;
		std::string path = mpMap->m_gamepath;
		CVorticonMapLoaderWithPlayer MapLoader(mpMap, m_Player, m_Object);
		MapLoader.load(1, 81, path);

		m_Player[0].hideplayer = false;
		x = 48<<CSF;
		y = 23<<CSF;
		m_Player[0].moveTo( Vector2D<int>(x,y) );

		mpMap->gotoPos((x>>STC)-100, (y>>STC)-160);

		mpShipFlySys.reset( new CShipFlySys( m_Player[0], mpMap, SPR_SHIP_RIGHT, SPR_SHIP_LEFT) );

		mpShipFlySys->addShipQueue(CMD_MOVE, 58, DUP);
		mpShipFlySys->addShipQueue(CMD_DISABLESCROLLING, 0, 0);
		mpShipFlySys->addShipQueue(CMD_WAIT, 13, DUPLEFT);
		mpShipFlySys->addShipQueue(CMD_MOVE, 208, DLEFT);
		mpShipFlySys->addShipQueue(CMD_FADEOUT, 0, 0);
		mpShipFlySys->addShipQueue(CMD_MOVE, 25, DDOWN);
		mpShipFlySys->addShipQueue(CMD_ENDOFQUEUE, 0, 0);

		mpMap->drawAll();
		mpShipFlySys->m_ShipQueuePtr = 0;

		m_mustsetup = false;
	}

	if( !mpShipFlySys->EndOfQueue() && mMessageBoxes.empty() )
	{
		// process the normal ship flying level and do all the inited commands
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
