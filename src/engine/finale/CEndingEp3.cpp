/*
 * CEndingEp3.cpp
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#include "CEndingEp3.h"
#include "../../StringUtils.h"
#include "../../sdl/CInput.h"
#include "../../sdl/CVideoDriver.h"
#include "../../graphics/effects/CFlash.h"
#include "../../graphics/CGfxEngine.h"
#include "../../common/CMapLoader.h"
#include "../../common/Playerdefines.h"

#define SAFE_DELETE(x) if(x) { delete x; x=NULL; }

CEndingEp3::CEndingEp3(CMap &map, std::vector<CPlayer> &Player) :
CFinale(map),
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
		std::string path = m_Map.m_gamepath;
		CMapLoader MapLoader(&m_Map, &m_Player);
		MapLoader.load(3, 81, path);

		m_Player[0].hideplayer = false;
		m_Player[0].moveTo(244<<STC, 104<<STC);
		m_Player[0].sprite = 0;

		m_Map.gotoPos(32, 32);
		m_Map.drawAll();

		m_TextBoxes.push_back(new CMessageBox(getstring("EP3_ESEQ_PAGE1"), true));
		m_TextBoxes.push_back(new CMessageBox(getstring("EP3_ESEQ_PAGE2"), true));
		m_TextBoxes.push_back(new CMessageBox(getstring("EP3_ESEQ_PAGE3"), true));
		m_TextBoxes.push_back(new CMessageBox(getstring("EP3_ESEQ_PAGE4"), true));

		int newtile = m_Map.at(2,12);
		for(int x=0 ; x<22 ; x++) // This changes to the Oh No! Tiles to normal Stone-Tiles
		{
			m_Map.changeTile( x, 15, newtile);
			m_Map.changeTile( x, 16, newtile);
		}

		m_mustsetup = false;
	}

	if(!m_TextBoxes.empty())
	{
		CMessageBox *pMB = m_TextBoxes.front();

		pMB->process();

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
		m_Timer.ResetSecondsTimer();
		g_pGfxEngine->pushEffectPtr(new CFlash(500, 32, 0xFFFFFF, 200));
		m_mustsetup = false;
	}

	if(m_counter==30 || m_counter==60 || m_counter==90)
		g_pGfxEngine->pushEffectPtr(new CFlash(500, 32, 0xFFFFFF, 200));

	m_counter++;

	if(m_Timer.HasTimeElapsed(3000))
	{
		m_step++;
		m_mustsetup = true;
		m_Player[0].hideplayer = true;
		m_Player[0].moveToForce(330<<STC, 104<<STC);
		m_Player[0].sprite = 0;
	}
}

void CEndingEp3::AwardScene()
{
	if(m_mustsetup)
	{
		//Initialization
		m_Player[0].hideplayer = true;
		m_Map.gotoPos(0,0);
		m_Map.resetScrolls(); // The Scrollsurface must be (0,0) so the bitmap is correctly drawn
		m_Map.m_animation_enabled = false; // Needed, because the other map is still loaded
		m_Map.drawAll();
		mp_FinaleStaticScene = new CFinaleStaticScene(m_Map.m_gamepath, "finale.ck3");

		mp_FinaleStaticScene->push_string("THE_END", 6000);

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
		m_Map.m_animation_enabled = true;
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
