/*
 * CEndingEp3.cpp
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#include "CEndingEp3.h"
#include "StringUtils.h"
#include "sdl/input/CInput.h"
#include "sdl/CVideoDriver.h"
#include "graphics/effects/CFlash.h"
#include "graphics/CGfxEngine.h"
#include "common/CMapLoader.h"
#include "common/Playerdefines.h"

CEndingEp3::CEndingEp3(std::list< SmartPointer<CMessageBoxVort> > &messageBoxes,
						const SmartPointer<CMap> &pMap, std::vector<CPlayer> &Player, std::vector<CObject*> &Object) :
CFinale(messageBoxes, pMap, Object),
m_Player(Player)
{
	m_Episode = 3;
	m_step = 0;
	m_starttime = g_pTimer->getTicks();
	m_timepassed = 0;
	m_mustsetup = true;
	m_mustfinishgame = false;
	m_counter = 0;
}

void CEndingEp3::process()
{
	m_timepassed = g_pTimer->getTicks() - m_starttime;

	switch(m_step)
	{
	case 0: HonorScene(); break;
	case 1: PaparazziScene(); break;
	case 2: AwardScene(); break;
	default:
		m_mustfinishgame = true;
		break;
	}
}

void CEndingEp3::HonorScene()
{
	if(m_mustsetup)
	{
		//Initialization
		std::string path = mpMap->m_gamepath;
		CMapLoader MapLoader(mpMap, &m_Player);
		MapLoader.load(3, 81, path);

		m_Player[0].hideplayer = false;
		m_Player[0].moveTo(VectorD2<int>(244<<STC, 104<<STC));
		m_Player[0].sprite = 0;

		mpMap->gotoPos(32, 32);
		mpMap->drawAll();

		m_TextBoxes.push_back(new CMessageBoxVort(g_pBehaviorEngine->getString("EP3_ESEQ_PAGE1"), true));
		m_TextBoxes.push_back(new CMessageBoxVort(g_pBehaviorEngine->getString("EP3_ESEQ_PAGE2"), true));
		m_TextBoxes.push_back(new CMessageBoxVort(g_pBehaviorEngine->getString("EP3_ESEQ_PAGE3"), true));
		m_TextBoxes.push_back(new CMessageBoxVort(g_pBehaviorEngine->getString("EP3_ESEQ_PAGE4"), true));

		int newtile = mpMap->at(2,12);
		for(int x=0 ; x<22 ; x++) // This changes to the Oh No! Tiles to normal Stone-Tiles
		{
			mpMap->changeTile( x, 15, newtile);
			mpMap->changeTile( x, 16, newtile);
		}

		m_mustsetup = false;
	}

	if(!m_TextBoxes.empty())
	{
		CMessageBoxVort *pMB = m_TextBoxes.front();

		pMB->processLogic();

		if(pMB->isFinished())
		{
			delete pMB;
			m_TextBoxes.pop_front();
		}
	}
	else
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
		g_pGfxEngine->setupEffect(new CFlash(500, 32, 0xFFFFFF, 200));
		m_mustsetup = false;
	}

	if(m_counter==30 || m_counter==60 || m_counter==90)
		g_pGfxEngine->setupEffect(new CFlash(500, 32, 0xFFFFFF, 200));

	m_counter++;

	if(mTimer.HasTimeElapsed(3000))
	{
		m_step++;
		m_mustsetup = true;
		m_Player[0].hideplayer = true;
		m_Player[0].moveToForce(VectorD2<int>(330<<STC, 104<<STC));
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
		mp_FinaleStaticScene = new CFinaleStaticScene(mpMap->m_gamepath, "finale.ck3");

		addMsgBoxString("THE_END");

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
		mpMap->m_animation_enabled = true;
		m_step++;
		m_mustsetup = true;
	}
}

CEndingEp3::~CEndingEp3() {
	while(!m_TextBoxes.empty())
	{
		delete m_TextBoxes.front();
		m_TextBoxes.pop_front();
	}

	SAFE_DELETE(mp_FinaleStaticScene);
}
