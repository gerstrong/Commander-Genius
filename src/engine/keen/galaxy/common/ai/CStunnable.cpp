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
#include <graphics/GsGraphics.h>

const int STARRING_SPRITE = 40;
const int STARRING_SPRITE_EP5 = 41;

const int TIME_STAR_SHOWN = 20;

namespace galaxy
{

const unsigned int STARRING_ANIMATION_TIME = 5;

CStunnable::CStunnable(	std::shared_ptr<CMap> pMap,
                        const Uint16 foeID,
                        Uint32 x, Uint32 y ) :
    CGalaxyActionSpriteObject( pMap, foeID, x, y, 0 ),
    m_stunned(false),
    m_animation_timer(0),
    starTimer(TIME_STAR_SHOWN)
{
    mInvincible = false;

    starSprite = STARRING_SPRITE;

    if(gBehaviorEngine.getEpisode() == 5)
        starSprite = STARRING_SPRITE_EP5;

    starSpriteBase = starSprite;
}

void CStunnable::getTouchedBy(CSpriteObject &theObject)
{
    CBullet *bullet = dynamic_cast<CBullet*>(&theObject);
    if( theObject.exists && bullet != nullptr )
    {
        if(!bullet->getActionNumber(A_KEENSHOT_IMPACT))
        {
            bullet->setAction(A_KEENSHOT_IMPACT);
            bullet->playSound( GameSound::SHOT_HIT );
        }
    }
}

void CStunnable::processGettingStunned()
{
    if(blockedd)
    {
        if( !mInvincible )
        {
            yinertia = -30; // It gets a small impulse

            mp_processState = static_cast<GASOFctr>(&CStunnable::processStunned);
            blockedd = false;

            moveUp(8<<STC);
        }
    }
}


void CStunnable::processStunned()
{ }


void CStunnable::drawStars()
{
    const auto scroll = mpMap->getMainScrollCoords();

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


        GsSprite &foeSprite = gGraphics.getSprite(mSprVar, mSpriteIdx);

        if(foeSprite.empty())
        {
            return;
        }

        GsSprite &StarSprite = gGraphics.getSprite(mSprVar, starSprite);
        const auto midXPt = ((foeSprite.getWidth()-StarSprite.getWidth())/2);
        scrx = (m_Pos.x>>STC)-scroll.x+midXPt;
        scry = (m_Pos.y>>STC)-scroll.y;
        int showY = scry+foeSprite.getYOffset();
        int showX = scrx+foeSprite.getXOffset();


        scrx = showX;

        if(mDispStarsBelow)
        {
            scry = showY+StarSprite.getHeight()+foeSprite.getHeight();
        }
        else
        {
            scry = showY-StarSprite.getHeight();
        }


        SDL_Rect gameres = gVideoDriver.getGameResolution().SDLRect();

        if( scrx < gameres.w && scry < gameres.h && exists )
        {
            int showX = scrx+StarSprite.getXOffset();
            int showY = scry+StarSprite.getYOffset();
            int w = StarSprite.getWidth();
            int h = StarSprite.getHeight();

            const auto visGA = gVideoDriver.mpVideoEngine->mRelativeVisGameArea;

            if( showX+StarSprite.getWidth() < visGA.pos.x ||
                    showX > visGA.pos.x+visGA.dim.x )
                return;

            if( showY+StarSprite.getHeight() < visGA.pos.y ||
                    showY > visGA.pos.y+visGA.dim.y )
                return;

            StarSprite.drawSprite( showX, showY, w, h, (255-transluceny) );
        }
    }
}

void CStunnable::draw()
{
    if( dontdraw )
        return;

    CGalaxySpriteObject::draw();

    if(mIsDead && blockedd && yinertia == 0)
    {
        drawStars();
    }

}


}
