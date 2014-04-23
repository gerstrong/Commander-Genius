/*
 * CPlatformHorizontal.cpp
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 */

#include "CPlatformHorizontal.h"
#include "engine/core/CBehaviorEngine.h"
#include <base/video/CVideoDriver.h>

const int FIRE_CHANGE_TIME = 5;
const int FIRE_SPRITE = 361;


namespace galaxy {

CPlatformHorizontal::CPlatformHorizontal(CMap *pmap, const Uint16 foeID, 
					 const direction_t xDir, Uint32 x, const Uint32 y,
                     const int actionOffset, const int sprVar ) :
CGalaxySpriteObject(pmap, foeID, x, y, sprVar),
CPlatform(pmap, foeID, x, y),
drawFire(false),
m_FireSprite(FIRE_SPRITE),
m_fireTimer(0)
{
	m_ActionBaseOffset = actionOffset;
	xDirection = xDir;
	yDirection = 0;
	
	const int episode = g_pBehaviorEngine->getEpisode();	

	if(episode == 4)
	    drawFire = true;
	else
	    drawFire = false;

    moveHorizSpeed = 20;

    auto diff = g_pBehaviorEngine->mDifficulty;

    if(diff > NINJA)
    {
        moveHorizSpeed = 40;
    }
    if(diff > EXPERT)
    {
        moveHorizSpeed = 30;
    }
    if(diff > HARD)
    {
        moveHorizSpeed = 25;
    }

	setActionForce(A_PLATFORM_MOVE);
	setActionSprite();
	calcBoundingBoxes();
    solid = false;
	
	// This should recalibrate the position in case the plats are stuck due some bad level design. (Keen 7 - Fun House)
	processMove( -(3<<CSF), 0 );
	processMove(  (3<<CSF), 0 );
}

void CPlatformHorizontal::process()
{
	const Uint16 object = mp_Map->getPlaneDataAt(2, getXMidPos(), getYMidPos());
	
	performCollisions();

	// If there is a blocker, change the direction
	if( object == 31 )
	{
	    xDirection = (xDirection == RIGHT) ? LEFT : RIGHT;
	}

	// direction change is processed here!
	if(xDirection == RIGHT)
	{
        movePlatRight(moveHorizSpeed);
	}
	else if(xDirection == LEFT)
	{
        movePlatLeft(moveHorizSpeed);
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

void CPlatformHorizontal::draw()
{
    // This draw routine also is able to draw a second object in case it is holding one.
    if( dontdraw )
        return;
    
    GsSprite &Sprite = gGraphics.getSprite(mSprVar,sprite);

    const int sprW = Sprite.getWidth();
    const int sprH = Sprite.getHeight();

    scrx = (m_Pos.x>>STC)-mp_Map->m_scrollx;
    scry = (m_Pos.y>>STC)-mp_Map->m_scrolly;
    
    SDL_Rect gameres = gVideoDriver.getGameResolution().SDLRect();
    
    if( scrx < gameres.w && scry < gameres.h && exists )
    {
        int showX = scrx+Sprite.getXOffset();
        int showY = scry+Sprite.getYOffset();

        GsSprite &fireSprite = gGraphics.getSprite(mSprVar,m_FireSprite);

        const int w = fireSprite.getWidth();
        const int h = fireSprite.getHeight();

        if(drawFire && xDirection == RIGHT)
            fireSprite.drawSprite(showX-2, showY+8, w, h);

        Sprite.drawSprite( showX, showY, sprW, sprH, (255-transluceny) );

        if(drawFire && xDirection == LEFT)
            fireSprite.drawSprite(showX+Sprite.getWidth()-6, showY+8, w, h);


        for( auto &player : mCarriedPlayerVec)
        {
            if(!player)
                continue;

            GsSprite &playSprite = gGraphics.getSprite(player->getSpriteVariantId(),player->sprite);
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
