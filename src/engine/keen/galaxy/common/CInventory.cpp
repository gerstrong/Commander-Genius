/*
 * CInventory.cpp
 *
 *  Created on: 25.11.2010
 *      Author: gerstrong
 */

#include "CInventory.h"

#include "engine/core/CBehaviorEngine.h"
#include "graphics/GsGraphics.h"
#include <base/video/CVideoDriver.h>
#include <base/CInput.h>
#include <base/interface/FindFile.h>
#include <graphics/CScrollEffect.h>

#include "fileio/KeenFiles.h"

CInventory::CInventory(const int playerIdx,
                       const int spriteVar) :
m_HUD(Item.m_points, Item.m_lifes, Item.m_bullets,
      playerIdx, spriteVar)
{
	reset();

    auto ep = gBehaviorEngine.getEpisode();

    if(ep >= 4)
	{
	    const Difficulty difficulty = gBehaviorEngine.mDifficulty;
	    
	    Item.m_bullets = 8;
	    if( difficulty > EASY )
        {
            Item.m_bullets = 5;
        }

        mp_StatusScreen.reset(new CStatusScreenGalaxy(Item));
	}

    const auto fullFName = JoinPaths(gKeenFiles.gameDir, "inventory.lua");
    mLua.loadFile(fullFName);

    if(mLua)
    {
        int startBullets = Item.m_bullets;
        mLua.runFunctionRetOneInt("getStartWithNumBullets", startBullets);
        Item.m_bullets = startBullets;

        bool usePogo = Item.m_special.mCanPogo;
        mLua.runFunctionRetOneBool("mayUsePogo", usePogo);
        Item.m_special.mCanPogo = usePogo;
    }
	
	m_HUD.sync();
}


void CInventory::reset()
{
	Item.reset();
}


void CInventory::fetchImportantStuff(const CInventory &other)
{
    Item.m_special.swimsuit |= other.Item.m_special.swimsuit;
    Item.m_special.elders += other.Item.m_special.elders;
    Item.m_special.sandwich |= other.Item.m_special.sandwich;
    Item.m_special.hook |= other.Item.m_special.hook;
    Item.m_special.rocketKeycard |= other.Item.m_special.rocketKeycard;
}


void CInventory::toggleStatusScreen()
{
	mp_StatusScreen->m_showstatus = !mp_StatusScreen->m_showstatus;

    SDL_Rect gameres = gVideoDriver.getGameResolution().SDLRect();
    const int scaleFac = gameres.h/200;

	if(mp_StatusScreen->m_showstatus)
	{
		int scroll_pos = 0;


        const auto Ep = gBehaviorEngine.getEpisode();

        if(Ep == 6)
        {
            mp_StatusScreen->GenerateStatusEp6();
        }
        else if(Ep == 5)
        {
            mp_StatusScreen->GenerateStatusEp5();
        }
        else
        {
            mp_StatusScreen->GenerateStatusEp4();
        }

        mp_StatusScreen->scaleToResolution();
		gVideoDriver.collectSurfaces();


        CScrollEffect* ScrollEffect =
                dynamic_cast<CScrollEffect*>(gEffectController.Effect());

		if( ScrollEffect )
        {
			scroll_pos = ScrollEffect->getScrollPosition();
        }

        gEffectController.setupEffect(
                    new CScrollEffect(mp_StatusScreen->mStatusSfcTransformed,
                                      scroll_pos,
                                      scaleFac*scaleFac,
                                      CENTER, DOWN));
	}
	else
	{		
        int scroll_pos = 160*scaleFac;

        CScrollEffect* ScrollEffect = dynamic_cast<CScrollEffect*>(gEffectController.Effect());

        // Check if it's already scrolling and get the position
		if( ScrollEffect )
        {
			scroll_pos = ScrollEffect->getScrollPosition();
        }

        gEffectController.setupEffect(
                    new CScrollEffect(mp_StatusScreen->mStatusSfcTransformed,
                                      scroll_pos, -scaleFac*scaleFac,
                                      CENTER, DOWN));

	}

	gInput.flushAll();
}


void CInventory::drawHUD()
{
    m_HUD.render();
}

void CInventory::setup(const int playerIdx,
                       const int sprVar)
{
    mSpriteVar = sprVar;
    m_HUD.setup(playerIdx, sprVar);
}

void CInventory::drawStatus()
{
    if(gEffectController.applyingEffects())
		return;

	mp_StatusScreen->draw();
}


void CInventory::operator>>(CSaveGameController &savedGame)
{
	savedGame.encodeData(Item);
}

void CInventory::operator>>(GsKit::ptree &invNode)
{
    auto &achieve = gAchievements;

    Item >> invNode;
    achieve >> invNode;
}

void CInventory::operator<<(CSaveGameController &savedGame)
{
	savedGame.decodeData(Item);
}

void CInventory::operator<<(GsKit::ptree &invNode)
{
    auto &achieve = gAchievements;

    Item << invNode;
    achieve << invNode;
}

void CInventory::addAchievementTask(const std::string which,
                                    const int numTasks)
{
    auto &achieve = gAchievements;
    achieve.addTaskCompletion(which, numTasks);
}

