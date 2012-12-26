/*
 * CPlatformVertical.cpp
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 */

#include "CPlatformVertical.h"
#include "common/CBehaviorEngine.h"
#include "sdl/CVideoDriver.h"

// Vertical platform speed
const int MOVE_VERT_SPEED = 20;

const int FIRE_CHANGE_TIME = 5;
const int FIRE_SPRITE = 363;

namespace galaxy
{

CPlatformVertical::CPlatformVertical(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y,
								const direction_t vertdir, const int actionOffset) :
CGalaxySpriteObject(pmap, foeID, x, y),
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
	{
	    drawFire = true;
	}
	else
	{
	    drawFire = false;
	}
	
	m_ActionBaseOffset = actionOffset;
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

	if(yDirection == UP && blockedu)
		yDirection = DOWN;
	else if(yDirection == DOWN && blockedd)
		yDirection = UP;

	if(yDirection == UP)
	{
	    movePlatUp(MOVE_VERT_SPEED);
	}
	else
	{
	    movePlatDown(MOVE_VERT_SPEED);
	}
	
	// If any Plat is stuck, because it is in some wall and can't move properly, try to pull it!
	if(blockedl)
	{
	    movePlatRight(MOVE_VERT_SPEED);
	}	
	if(blockedr)
	{
	    movePlatLeft(MOVE_VERT_SPEED);
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
    
    CSprite &Sprite = g_pGfxEngine->getSprite(sprite);
    
    scrx = (m_Pos.x>>STC)-mp_Map->m_scrollx;
    scry = (m_Pos.y>>STC)-mp_Map->m_scrolly;
    
    SDL_Rect gameres = g_pVideoDriver->getGameResolution().SDLRect();
    
    if( scrx < gameres.w && scry < gameres.h && exists )
    {
	Uint16 showX = scrx+Sprite.getXOffset();
	Uint16 showY = scry+Sprite.getYOffset();
		
	
	if(drawFire && dimFire)
	{
	    CSprite &fireSpriteR = g_pGfxEngine->getSprite(m_FireSprite);
	    CSprite &fireSpriteL = g_pGfxEngine->getSprite(m_FireSprite+2);
	    
	    fireSpriteL.drawSprite(showX+1, showY+12);
	    fireSpriteR.drawSprite(showX+Sprite.getWidth()-8, showY+9);
	}
	
	Sprite.drawSprite( showX, showY, (255-transluceny) );
	
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
