/*
 * CPlatformVertical.cpp
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 */

#include "CPlatformVertical.h"
#include "engine/core/CBehaviorEngine.h"
#include <base/video/CVideoDriver.h>


const int FIRE_CHANGE_TIME = 5;
const int FIRE_SPRITE = 363;

namespace galaxy
{

CPlatformVertical::CPlatformVertical(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y,
                                const direction_t vertdir, const int actionOffset, const int sprVar) :
CGalaxySpriteObject(pmap, foeID, x, y, sprVar),
CPlatform(pmap, foeID, x, y),
drawFire(false),
dimFire(false),
m_FireSprite(FIRE_SPRITE),
m_fireTimer(0)
{
	xDirection = 0;
	yDirection = vertdir;	
	
	const int episode = g_pBehaviorEngine->getEpisode();	
	if(episode == 4)
	    drawFire = true;
	else
	    drawFire = false;

    moveVertSpeed = 20;


    auto diff = g_pBehaviorEngine->mDifficulty;

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
    	Uint16 object = 0; 
	
	object = mp_Map->getPlaneDataAt(2, getPosition());

	performCollisions();
	
	// If there is a blocker, change the direction
	if( object == 31 )
	{
	    yDirection = (yDirection == UP) ? DOWN : UP;
	}
	
	
	// If fire needs to be drawn, well check for dimming
	if( yDirection == DOWN )
	    dimFire = !dimFire;
	else
	    dimFire = true; 

	if(yDirection == UP)
	{
        movePlatUp(moveVertSpeed);
	}
	else
	{
        movePlatDown(moveVertSpeed);
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
    
    GsSprite &Sprite = gGraphics.getSprite(mSprVar,sprite);

    scrx = (m_Pos.x>>STC)-mp_Map->m_scrollx;
    scry = (m_Pos.y>>STC)-mp_Map->m_scrolly;
    
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

            GsSprite &playSprite = gGraphics.getSprite(player->getSpriteVariantId(),
                                                          player->sprite);
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
