/*
 * CPlatform.cpp
 *
 *  Created on: 23.02.2011
 *      Author: Gerstrong and FCTW
 */

#include "CPlatform.h"
#include "CLogFile.h"
#include "sdl/CVideoDriver.h"

namespace galaxy {

CPlatform::CPlatform(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y),
mp_CarriedPlayer(NULL)
{
	m_ActionBaseOffset = 0x316A;
}

void CPlatform::movePlatX(const int amnt)
{
	if(amnt > 0)
		movePlatRight(amnt);
	else if(amnt < 0)
		movePlatLeft(-amnt);
}

void CPlatform::movePlatY(const int amnt)
{
	if(amnt > 0)
		movePlatDown(amnt);
	else if(amnt < 0)
		movePlatUp(-amnt);
}

void CPlatform::movePlatLeft(const int amnt)
{
    // If the Player is standing on the plat move him with it!
	if(amnt <= 0)
	    return;

	if(mp_CarriedPlayer)
	{
		if(!mp_CarriedPlayer->m_jumpdownfromobject)
		{
		    m_EventCont.add(new ObjMoveCouple(-amnt,0, *mp_CarriedPlayer));
		    return;
		}
	}		    
	moveLeft(amnt);
}

void CPlatform::movePlatRight(const int amnt)
{
	// If the Player is standing on the plat move him with it!
	if(amnt <= 0)
	    return;

	if(mp_CarriedPlayer)
	{	    
		if(!mp_CarriedPlayer->m_jumpdownfromobject)
		{
		    m_EventCont.add(new ObjMoveCouple(amnt,0,*mp_CarriedPlayer));
		    return;
		}			
	}

	// Now move the platform itself.
	moveRight(amnt);
}

void CPlatform::movePlatUp(const int amnt)
{
	// First move the object on platform if any
	if(mp_CarriedPlayer)
	{
		if(!mp_CarriedPlayer->m_jumpdownfromobject)
		{
		    m_EventCont.add(new ObjMoveCouple(0,-amnt,*mp_CarriedPlayer));
		    return;
		}
	}

	// Now move the platform itself.
	moveUp(amnt);
}

void CPlatform::movePlatDown(const int amnt)
{
	// First move the object on platform if any
	if(mp_CarriedPlayer)
	{
		if(!mp_CarriedPlayer->m_jumpdownfromobject)
		{
		    m_EventCont.add(new ObjMoveCouple(0,amnt,*mp_CarriedPlayer));
		    return;
		}
	}

	// Now move the platform itself.
	moveDown(amnt);
}

void CPlatform::movePlat(const VectorD2<int> &speed)
{
    if(speed.x == 0 && speed.y == 0)
	return;
    
	// First move the object on platform if any
	if(mp_CarriedPlayer)
	{
		if(!mp_CarriedPlayer->m_jumpdownfromobject)
		{
		    m_EventCont.add(new ObjMoveCouple(speed,*mp_CarriedPlayer));
		    return;
		}
	}

	// Now move the platform itself.
	moveDir(speed);    
}

void CPlatform::process()
{
	// check if someone is still standing on the platform
	if(mp_CarriedPlayer)
	{
		if(!hitdetect(*mp_CarriedPlayer) || mp_CarriedPlayer->blockedu )
		{
			mp_CarriedPlayer->pSupportedbyobject = nullptr;
			mp_CarriedPlayer->m_jumpdownfromobject = false;
			mp_CarriedPlayer->dontdraw = false;
			mp_CarriedPlayer = NULL;
		}
		else if(!mp_CarriedPlayer->m_jumpdownfromobject)
		{
		    if(mp_CarriedPlayer->getActionNumber(A_KEEN_STAND) || mp_CarriedPlayer->getActionNumber(A_KEEN_ON_PLAT))
		    {
			// Check that he correctly stands on the platform
			const unsigned int standY = getYUpPos()+1;
			
			if( standY > mp_CarriedPlayer->getYDownPos() )
			    mp_CarriedPlayer->moveDown((standY-mp_CarriedPlayer->getYDownPos())/2 + 1);
			else if( standY < mp_CarriedPlayer->getYDownPos() )
			    mp_CarriedPlayer->moveUp((mp_CarriedPlayer->getYDownPos()-standY)/2 + 1);
		    }    
		}
	}

	processActionRoutine();
}

void CPlatform::getTouchedBy(CSpriteObject &theObject)
{
	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		const int m_py2 = player->getYDownPos();
		const int m_y2 = getYUpPos()+(4<<STC);
		if( m_py2 <= m_y2 && !player->pSupportedbyobject && !player->m_jumpdownfromobject )
		{
			mp_CarriedPlayer = player;
			player->pSupportedbyobject = this;
			player->dontdraw = true;
		}
	}
}


void CPlatform::draw()
{
    // This draw routine also is able to draw a second object in case it is holding one.
    if( sprite == BLANKSPRITE || dontdraw )
	return;
    
    CSprite &Sprite = g_pGfxEngine->getSprite(sprite);
    
    scrx = (m_Pos.x>>STC)-mp_Map->m_scrollx;
    scry = (m_Pos.y>>STC)-mp_Map->m_scrolly;
    
    SDL_Rect gameres = g_pVideoDriver->getGameResolution().SDLRect();
    
    if( scrx < gameres.w && scry < gameres.h && exists )
    {
	Uint16 showX = scrx+Sprite.getXOffset();
	Uint16 showY = scry+Sprite.getYOffset();
		
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

bool CPlatform::calcVisibility()
{
	return true;
}


}
