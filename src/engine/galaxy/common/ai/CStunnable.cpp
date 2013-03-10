/*
 * CStunnable.cpp
 *
 *  Created on: 23.06.2011
 *      Author: gerstrong
 *
 *  Those are in the galaxy Engine the stunnable objects
 *  Most of the enemies but not all foes can inherit this object
 *  and will get the stars when stunned or not, depending on how stunnable
 *  the enemies are...
 */

#include "CStunnable.h"
#include "CBullet.h"
#include "common/CBehaviorEngine.h"
#include "sdl/CVideoDriver.h"

const int STARRING_SPRITE = 40;
const int STARRING_SPRITE_EP5 = 41;

const int TIME_STAR_SHOWN = 20;

namespace galaxy
{
  
const unsigned int STARRING_ANIMATION_TIME = 5;

CStunnable::CStunnable(	CMap *pmap,
			const Uint16 foeID,
			Uint32 x, Uint32 y ) :
CGalaxySpriteObject( pmap, foeID, x, y ),
m_stunned(false),
mp_processState(NULL),
m_animation_timer(0),
starTimer(TIME_STAR_SHOWN)
{
	m_invincible = false;
	
	starSprite = STARRING_SPRITE;
	
	if(g_pBehaviorEngine->getEpisode() == 5)
	  starSprite = STARRING_SPRITE_EP5;
	
	starSpriteBase = starSprite;
}

void CStunnable::getTouchedBy(CSpriteObject &theObject)
{
	CBullet *bullet = dynamic_cast<CBullet*>(&theObject);
	if( theObject.exists && bullet != NULL )
	{
		bullet->setAction(A_KEENSHOT_IMPACT);
		bullet->playSound( SOUND_SHOT_HIT );
	}
}

void CStunnable::processGettingStunned()
{
  if(blockedd)
  {
    if( !m_invincible )
    {      
      yinertia = -30; // It gets a small impulse
      
      mp_processState = &CStunnable::processStunned;
      blockedd = false;
      
      moveUp(8<<STC);
    }
  }
}

void CStunnable::processStunned()
{ }

void CStunnable::draw()
{
  if( dontdraw )
	return;    
  
  CGalaxySpriteObject::draw();
  
  if(dead && blockedd && yinertia == 0)
  {                
    if(starTimer > 0)
    {
      starTimer--;
    }
    else
    {            
      const unsigned char anim = m_animation_timer % STARRING_ANIMATION_TIME;
      
      if(anim == 0)
      {
	starSprite++;
	
	if(starSprite > starSpriteBase+2)
	{
	  starSprite = starSpriteBase;
	  m_animation_timer = 0;
	}
      }
      
      // Animation timer increasing all the time
      m_animation_timer++;
      
      
      CSprite &StarSprite = g_pGfxEngine->getSprite(starSprite);
      
      int yoffset = (StarSprite.getHeight()<<STC);
      
      scrx = (getXMidPos()>>STC)-mp_Map->m_scrollx;
      scry = ((m_Pos.y-yoffset)>>STC)-mp_Map->m_scrolly;
      
      SDL_Rect gameres = g_pVideoDriver->getGameResolution().SDLRect();
      
      if( scrx < gameres.w && scry < gameres.h && exists )
      {
	Uint16 showX = scrx+StarSprite.getXOffset();
	Uint16 showY = scry+StarSprite.getYOffset();
	StarSprite.drawSprite( showX, showY, (255-transluceny) );
      }
    }
  }    
  	
}


void CStunnable::setActionForce(const size_t ActionNumber)
{
	CGalaxySpriteObject::setActionForce(ActionNumber);

	if( mActionMap.find(ActionNumber) != mActionMap.end() )
		mp_processState = mActionMap[ActionNumber];
	else
		CGalaxySpriteObject::setActionForce(0); // This might happen, when the action-map is incomplete
}

};
