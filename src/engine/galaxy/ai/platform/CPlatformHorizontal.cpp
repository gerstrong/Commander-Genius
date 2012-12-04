/*
 * CPlatformHorizontal.cpp
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 */

#include "CPlatformHorizontal.h"
#include "common/CBehaviorEngine.h"
#include "sdl/CVideoDriver.h"

const int FIRE_CHANGE_TIME = 5;
const int FIRE_SPRITE = 361;


namespace galaxy {

CPlatformHorizontal::CPlatformHorizontal(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y),
CPlatform(pmap, foeID, x, y),
drawFire(false),
m_FireSprite(FIRE_SPRITE),
m_fireTimer(0)
{
	xDirection = RIGHT;
	yDirection = 0;
	
	const int episode = g_pBehaviorEngine->getEpisode();	
	if(episode == 4)
	{
	    drawFire = true;
	}
	else
	{
	    m_ActionBaseOffset = 0x1B7C;
	    drawFire = false;
	}
	
	setActionForce(A_PLATFORM_MOVE);
	setActionSprite();
	calcBoundingBoxes();
	
	// This should recalibrate the position in case the plats are stuck due some bad level design. (Keen 7 - Fun House)
	processMove( -(3<<CSF), 0 );
	processMove(  (3<<CSF), 0 );
}

const int MOVE_HORIZ_SPEED = 20;

void CPlatformHorizontal::process()
{
	const Uint16 object = mp_Map->getPlaneDataAt(2, getXMidPos(), getYMidPos());
	
	performCollisions();

	// If there is a blocker, change the direction
	if( object == 31 )
	{
	    xDirection = (xDirection == RIGHT) ? LEFT : RIGHT;
	}
	
	if(blockedr)
	{
	    xDirection = LEFT;
	}
	else if(blockedl)
	{
	    xDirection = RIGHT;
	}

	// direction change is processed here!
	if(xDirection == RIGHT)
	{
	    movePlatRight(MOVE_HORIZ_SPEED);
	}
	else if(xDirection == LEFT)
	{
	    movePlatLeft(MOVE_HORIZ_SPEED);
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
    
    CSprite &Sprite = g_pGfxEngine->getSprite(sprite);
    
    scrx = (m_Pos.x>>STC)-mp_Map->m_scrollx;
    scry = (m_Pos.y>>STC)-mp_Map->m_scrolly;
    
    SDL_Rect gameres = g_pVideoDriver->getGameResolution().SDLRect();
    
    if( scrx < gameres.w && scry < gameres.h && exists )
    {
	Uint16 showX = scrx+Sprite.getXOffset();
	Uint16 showY = scry+Sprite.getYOffset();
		
	CSprite &fireSprite = g_pGfxEngine->getSprite(m_FireSprite);
	
	if(drawFire && xDirection == RIGHT)
	    fireSprite.drawSprite(showX-2, showY+8);
	
	Sprite.drawSprite( showX, showY, (255-transluceny) );
	
	if(drawFire && xDirection == LEFT)
	    fireSprite.drawSprite(showX+Sprite.getWidth()-6, showY+8);

	
	if(mp_CarriedPlayer)
	{
	    CSprite &playSprite = g_pGfxEngine->getSprite(mp_CarriedPlayer->sprite);
	    int distx = mp_CarriedPlayer->getXPosition()-getXPosition();
	    int disty = mp_CarriedPlayer->getYPosition()-getYPosition();
	    
	    distx = (distx>>STC);
	    distx += (playSprite.getXOffset()-Sprite.getXOffset());
	    disty = (disty>>STC);
	    disty += (playSprite.getYOffset()-Sprite.getYOffset());
	    
	    playSprite.drawSprite( showX+distx, showY+disty );
	}	    
	
	hasbeenonscreen = true;
    }    
}

}
