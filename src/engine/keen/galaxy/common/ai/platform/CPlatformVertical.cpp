/*
 * CPlatformVertical.cpp
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 */

#include "CPlatformVertical.h"
#include "engine/core/CBehaviorEngine.h"
#include <base/video/CVideoDriver.h>
#include <graphics/GsGraphics.h>


const int FIRE_CHANGE_TIME = 5;
const int FIRE_SPRITE = 363;
const int FIRE_SPRITE_DEMO = 314;

namespace galaxy
{

CPlatformVertical::CPlatformVertical(std::shared_ptr<CMap> pMap, Uint16 foeID, const Uint32 x, const Uint32 y,
                                     const direction_t vertdir, const int actionOffset, const int sprVar) :
CGalaxySpriteObject(pMap, foeID, x, y, sprVar),
CPlatform(pMap, foeID, x, y),
drawFire(false),
dimFire(false),
m_fireTimer(0)
{
    m_FireSprite = gBehaviorEngine.isDemo() ? FIRE_SPRITE_DEMO : FIRE_SPRITE;

    GsVec2D<Uint32> pos = getPosition();

    pos.y = (pos.y>>CSF);   pos.x = (pos.x>>CSF);


	xDirection = 0;
	yDirection = vertdir;	
	
	const int episode = gBehaviorEngine.getEpisode();	
	if(episode == 4)
	    drawFire = true;
	else
	    drawFire = false;

    moveVertSpeed = 20;


    auto diff = gBehaviorEngine.mDifficulty;

    if(diff > NINJA)
    {
        moveVertSpeed = 40;
    }
    if(diff > EXPERT)
    {
        moveVertSpeed = 30;
    }
    if(diff > HARD)
    {
        moveVertSpeed = 25;
    }

	
	m_ActionBaseOffset = actionOffset;

    solid = false;

	setActionForce(A_PLATFORM_MOVE);
	setActionSprite();
	calcBoundingBoxes();	
}

void CPlatformVertical::process()
{
    GsVec2D<Uint32> pos = getPosition();

    pos.y = (pos.y>>CSF);   pos.x = (pos.x>>CSF);

    auto lowerPos = pos;
    auto upperPos = pos;

    upperPos.y -= 1;
    lowerPos.y += 1;

    bool dontmove = false;

    performCollisions();

    // Scan for blockers nearby
    const auto objectUpper = mpMap->getPlaneDataAt(2, upperPos<<CSF);
    const auto objectLower = mpMap->getPlaneDataAt(2, lowerPos<<CSF);

    // If plat is between two blockers, do not move
    if(objectUpper == 0x1F && objectLower == 0x1F)
    {
        dontmove = true;
    }
    else if( objectLower == 0x1F ) // If there is only one blocker, change the direction
    {
        yDirection = UP;
    }
    else if( objectUpper == 0x1F )
    {
        yDirection = DOWN;
    }

    // If fire under the plats needs to be drawn dim it in case the platform goes down
	if( yDirection == DOWN )
	    dimFire = !dimFire;
	else
	    dimFire = true; 

    if(!dontmove)
    {
        if(yDirection == UP)
        {
            movePlatUp(moveVertSpeed);
        }
        else
        {
            movePlatDown(moveVertSpeed);
        }
    }
	
	// If any Plat is stuck, because it is in some wall and can't move properly, try to pull it!
	if(blockedl)
	{
        movePlatRight(moveVertSpeed);
	}	
	if(blockedr)
	{
        movePlatLeft(moveVertSpeed);
	}

	
	// If Timer passed swap Sprite
	m_fireTimer++;

	if(m_fireTimer >= FIRE_CHANGE_TIME)
	{
		m_fireTimer = 0;
		m_FireSprite = (m_FireSprite==FIRE_SPRITE) ? FIRE_SPRITE+1 : FIRE_SPRITE;
	}

	CPlatform::process();
}


void CPlatformVertical::draw()
{
    // This draw routine also is able to draw a second object in case it is holding one.
    if( dontdraw )
        return;
    
    GsSprite &Sprite = gGraphics.getSprite(mSprVar,mSpriteIdx);

    const auto scroll = mpMap->getMainScrollCoords();

    scrx = (m_Pos.x>>STC)-scroll.x;
    scry = (m_Pos.y>>STC)-scroll.y;
    
    SDL_Rect gameres = gVideoDriver.getGameResolution().SDLRect();
    
    if( scrx < gameres.w && scry < gameres.h && exists )
    {
        int showX = scrx+Sprite.getXOffset();
        int showY = scry+Sprite.getYOffset();

        const int w = Sprite.getWidth();
        const int h = Sprite.getHeight();

        if(drawFire && dimFire)
        {
            GsSprite &fireSpriteR = gGraphics.getSprite(mSprVar,m_FireSprite);
            GsSprite &fireSpriteL = gGraphics.getSprite(mSprVar,m_FireSprite+2);

            const int lw = fireSpriteL.getWidth();
            const int lh = fireSpriteL.getHeight();
            const int rw = fireSpriteR.getWidth();
            const int rh = fireSpriteR.getHeight();

            fireSpriteL.drawSprite(showX+1, showY+12, lw, lh);
            fireSpriteR.drawSprite(showX+Sprite.getWidth()-8, showY+9, rw, rh);
        }

        Sprite.drawSprite( showX, showY, w, h, (255-transluceny) );

        for( auto &player : mCarriedPlayerVec)
        {
            if(!player)
                continue;

            GsSprite &playSprite = gGraphics.getSprite(player->getSpriteVariantIdx(),
                                                          player->mSpriteIdx);
            int distx = player->getXPosition()-getXPosition();
            int disty = player->getYPosition()-getYPosition();

            distx = (distx>>STC);
            distx += (playSprite.getXOffset()-Sprite.getXOffset());
            disty = (disty>>STC);
            disty += (playSprite.getYOffset()-Sprite.getYOffset());

            const int playW = playSprite.getWidth();
            const int playH = playSprite.getHeight();

            playSprite.drawSprite( showX+distx, showY+disty, playW, playH );
        }

        hasbeenonscreen = true;
    }
}

}
