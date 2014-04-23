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
#include "engine/core/CBehaviorEngine.h"
#include <base/video/CVideoDriver.h>

const int STARRING_SPRITE = 40;
const int STARRING_SPRITE_EP5 = 41;

const int TIME_STAR_SHOWN = 20;

namespace galaxy
{
  
const unsigned int STARRING_ANIMATION_TIME = 5;

CStunnable::CStunnable(	CMap *pmap,
			const Uint16 foeID,
            Uint32 x, Uint32 y ) :
CGalaxyActionSpriteObject( pmap, foeID, x, y, 0 ),
m_stunned(false),
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
      
      mp_processState = (GASOFctr) &CStunnable::processStunned;
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
      
      
      GsSprite &StarSprite = gGraphics.getSprite(mSprVar,starSprite);
      
      int yoffset = (StarSprite.getHeight()<<STC);
      int xoffset = (StarSprite.getWidth()<<STC);
      
      scrx = ((getXMidPos()-xoffset/2)>>STC)-mp_Map->m_scrollx;
      scry = ((m_Pos.y-yoffset)>>STC)-mp_Map->m_scrolly;
      
      SDL_Rect gameres = gVideoDriver.getGameResolution().SDLRect();
      
      if( scrx < gameres.w && scry < gameres.h && exists )
      {
        int showX = scrx+StarSprite.getXOffset();
        int showY = scry+StarSprite.getYOffset();
        int w = StarSprite.getWidth();
        int h = StarSprite.getHeight();

        auto visGA = gVideoDriver.mpVideoEngine->mRelativeVisGameArea;

        if( showX+StarSprite.getWidth() < visGA.x || showX > visGA.x+visGA.w )
            return;

        if( showY+StarSprite.getHeight() < visGA.y || showY > visGA.y+visGA.h )
            return;

        StarSprite.drawSprite( showX, showY, w, h, (255-transluceny) );
      }
    }
  }    
  	
}


};
