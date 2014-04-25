/*
 * CEndingEp3.cpp
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#include "CEndingEp3.h"
#include <base/utils/StringUtils.h>
#include <base/CInput.h>
#include "graphics/effects/CFlash.h"
#include "graphics/GsGraphics.h"
#include "../CVorticonMapLoader.h"
#include "engine/core/Playerdefines.h"

CEndingEp3::CEndingEp3(std::list< std::shared_ptr<CMessageBoxVort> > &messageBoxes,
			const std::shared_ptr<CMap> &pMap, 
			std::vector<CPlayer> &Player, 
		       std::vector< std::unique_ptr<CVorticonSpriteObject> > &Object) :
CFinale(messageBoxes, pMap, Object),
m_Player(Player)
{
	m_Episode = 3;
	m_step = 0;
	m_starttime = gTimer.getTicks();
	m_timepassed = 0;
	m_mustsetup = true;
	m_mustfinishgame = false;
	m_counter = 0;
}

void CEndingEp3::ponder()
{
    m_timepassed = gTimer.getTicks() - m_starttime;

    switch(m_step)
    {
    case 0: HonorScene(); break;
    case 1: PaparazziScene(); break;
    case 2: AwardScene(); break;
    default:
        m_mustfinishgame = true;
        break;
    }

    m_Player[0].processEvents();
}

void CEndingEp3::render()
{
    switch(m_step)
    {
    /*case 0: HonorScene(); break;
    case 1: PaparazziScene(); break;*/
    case 2: AwardSceneRender(); break;
    default: break;
    }
}

void CEndingEp3::HonorScene()
{
	if(m_mustsetup)
	{
		//Initialization
		std::string path = mpMap->m_gamepath;
		std::vector< std::unique_ptr<CVorticonSpriteObject> > spriteObjectContainer;
		CVorticonMapLoaderWithPlayer MapLoader(mpMap, m_Player, spriteObjectContainer);
		MapLoader.load(3, 81, path);

		m_Player[0].hideplayer = false;
		m_Player[0].moveTo(Vector2D<int>(244<<STC, 104<<STC));
		m_Player[0].sprite = 0;

		mpMap->gotoPos(32, 32);
		mpMap->drawAll();

		addMsgBoxString("EP3_ESEQ_PAGE1");
		addMsgBoxString("EP3_ESEQ_PAGE2");
		addMsgBoxString("EP3_ESEQ_PAGE3");
		addMsgBoxString("EP3_ESEQ_PAGE4");

		int newtile = mpMap->at(2,12);
		for(int x=0 ; x<22 ; x++) // This changes to the Oh No! Tiles to normal Stone-Tiles
		{
			mpMap->changeTile( x, 15, newtile);
			mpMap->changeTile( x, 16, newtile);
		}

		m_mustsetup = false;
	}

	if( mMessageBoxes.empty() )
	{
		m_step++;
		m_mustsetup = true;
	}
}

// here comes a short flashing effect
void CEndingEp3::PaparazziScene()
{
	if(m_mustsetup)
	{
		mTimer.ResetSecondsTimer();
        gEffectController.setupEffect(new CFlash(500, 32, 0xFFFFFF, 200));
		m_mustsetup = false;
	}

	if(m_counter==30 || m_counter==60 || m_counter==90)
    {
        gEffectController.setupEffect(new CFlash(500, 32, 0xFFFFFF, 200));
    }

	m_counter++;

	if(mTimer.HasTimeElapsed(3000))
	{
		m_step++;
		m_mustsetup = true;
		m_Player[0].hideplayer = true;
		m_Player[0].moveToForce(Vector2D<int>(330<<STC, 104<<STC));
		m_Player[0].sprite = 0;
	}
}

void CEndingEp3::AwardScene()
{
	if(m_mustsetup)
	{
		//Initialization
		m_Player[0].hideplayer = true;
		mpMap->gotoPos(0,0);
		mpMap->resetScrolls(); // The Scrollsurface must be (0,0) so the bitmap is correctly drawn
		mpMap->m_animation_enabled = false; // Needed, because the other map is still loaded
		mpMap->drawAll();
		mpFinaleStaticScene.reset(new CFinaleStaticScene(mpMap->m_gamepath, "finale.ck3"));

		addMsgBoxString("THE_END");

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


void CEndingEp3::AwardSceneRender()
{
    if( !mMessageBoxes.empty() )
    {
        mpFinaleStaticScene->render(m_step);
    }

}

