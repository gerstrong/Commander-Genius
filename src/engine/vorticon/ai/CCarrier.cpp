#include "CCarrier.h"
#include "sdl/CVideoDriver.h"

CCarrier::CCarrier(CMap* pmap, Uint32 x, Uint32 y, object_t type) : 
CVorticonSpriteObject(pmap, x, y, type),
mpCarriedPlayer(nullptr)
{

}

void CCarrier::getTouchedBy(CVorticonSpriteObject& theObject)
{
    if(CPlayer *player = dynamic_cast<CPlayer*>(&theObject))
    {
	if(player->getYDownPos() > getYMidPos())
	{
	    if( (player->getXMidPos() < getXMidPos() && yDirection >= 0) || yDirection <= 0 )
		player->push(*this);	    
	}
	else
	{
	    mpCarriedPlayer = player;
	    player->pSupportedbyobject = this;
	    player->dontdraw = true;
	}
    }
}

void CCarrier::process()
{
        // check if someone is still standing on the platform
	if(mpCarriedPlayer)
	{
		if(!hitdetect(*mpCarriedPlayer) || mpCarriedPlayer->blockedu)
		{
			mpCarriedPlayer->pSupportedbyobject = nullptr;
			mpCarriedPlayer->dontdraw = false;
			mpCarriedPlayer = nullptr;
		}
	}
}


void CCarrier::draw()
{
    // This draw routine also is able to draw a second object in case it is holding one.
    if( dontdraw )
	return;
    
    CSprite &Sprite = g_pGfxEngine->getSprite(mSprVar,sprite);
    
    scrx = (m_Pos.x>>STC)-mp_Map->m_scrollx;
    scry = (m_Pos.y>>STC)-mp_Map->m_scrolly;
    
    SDL_Rect gameres = g_pVideoDriver->getGameResolution().SDLRect();
    
    if( scrx < gameres.w && scry < gameres.h && exists )
    {
	Uint16 showX = scrx+Sprite.getXOffset();
	Uint16 showY = scry+Sprite.getYOffset();
	if(m_blinktime > 0)
	{
	    Sprite.drawBlinkingSprite( showX, showY );
	    m_blinktime--;
	}
	else
	{
	    Sprite.drawSprite( showX, showY, (255-transluceny) );
	    if(mpCarriedPlayer)
	    {
        CSprite &playSprite = g_pGfxEngine->getSprite(mSprVar,mpCarriedPlayer->sprite);
		int distx = mpCarriedPlayer->getXPosition()-getXPosition();
		int disty = mpCarriedPlayer->getYPosition()-getYPosition();
		
		distx = (distx>>STC);
		distx += (playSprite.getXOffset()-Sprite.getXOffset());
		disty = (disty>>STC);
		disty += (playSprite.getYOffset()-Sprite.getYOffset());
		
		playSprite.drawSprite( showX+distx, showY+disty );
	    }
	}
	hasbeenonscreen = true;
    }    
}


void CCarrier::moveCarrierLeft(const int amnt)
{
    // If the Player is standing on the plat move him with it!
    if(amnt <= 0)
	return;
    
    if(mpCarriedPlayer)
    {
	m_EventCont.add(new ObjMoveCouple(-amnt,0, *mpCarriedPlayer));
	return;
    }
    moveLeft(amnt);
}


void CCarrier::moveCarrierRight(const int amnt)
{
    // If the Player is standing on the plat move him with it!
    if(amnt <= 0)
	return;
    
    if(mpCarriedPlayer)
    {	    
	m_EventCont.add(new ObjMoveCouple(amnt,0,*mpCarriedPlayer));
	return;
    }
    
    // Now move the platform itself.
    moveRight(amnt);
}

void CCarrier::moveCarrierUp(const int amnt)
{
    // First move the object on platform if any
    if(mpCarriedPlayer)
    {
	m_EventCont.add(new ObjMoveCouple(0,-amnt,*mpCarriedPlayer));
	return;
    }
    
    // Now move the platform itself.
    moveUp(amnt);
}

void CCarrier::moveCarrierDown(const int amnt)
{
    // First move the object on platform if any
    if(mpCarriedPlayer)
    {
	m_EventCont.add(new ObjMoveCouple(0,amnt,*mpCarriedPlayer));
	return;
    }
    
    // Now move the platform itself.
    moveDown(amnt);
}
