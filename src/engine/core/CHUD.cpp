/*
 * CHUD.cpp
 *
 *  Created on: 01.07.2010
 *      Author: gerstrong
 */

#include "CHUD.h"
#include "engine/core/CCamera.h"
#include "engine/core/CBehaviorEngine.h"
#include "engine/core/Playerdefines.h"
#include "engine/core/spritedefines.h"
#include "graphics/GsGraphics.h"

#include <base/video/CVideoDriver.h>
#include <base/interface/StringUtils.h>

const int EFFECT_TIME = 10;
const int EFFECT_SPEED = 10;

CHUD::CHUD(unsigned long &score, signed char &lives,
           signed int &charges, const int playerIdx,
           const int spriteVar) :
m_score(score),
mLives(lives),
m_charges(charges),
mOldScore(score),
mOldCharges(charges)
{
    setup(playerIdx, spriteVar);
}

void CHUD::createHUDBlit()
{        
    mHUDBlit.createRGBSurface(mRenderRect.SDLRect());
    mHUDBlit.makeBlitCompatible();    
}

void CHUD::setup(const int playerIdx,
                 const int spriteVar)
{
    mId = playerIdx;

    mRenderRect.pos.x = 8;	mRenderRect.pos.y = 4;

    // Start at zero, beacuse we need more room for the HUD Boxes
    if(gBehaviorEngine.numPlayers() > 3)
    {
        mRenderRect.pos.x = 0;	mRenderRect.pos.y = 0;
    }

    size_t Episode = gBehaviorEngine.getEpisode();

    if( Episode >= 1 && Episode <= 3 )
    {
        mRenderRect.dim.x = 84;	mRenderRect.dim.y = 30;
        mRenderRect.pos.x += (mRenderRect.dim.x-4)*playerIdx;

        createHUDBlit();
        CreateVorticonBackground();
    }
    else // Galaxy HUD
    {
        mRenderRect.dim.x = 80;	mRenderRect.dim.y = 30;

        auto &hudBg = *gGraphics.getSprite(spriteVar, "HUDBACKGROUND");

        mHUDBox.copy(hudBg);
		
        mRenderRect.dim.y = mHUDBox.getHeight();
        mRenderRect.dim.x = mHUDBox.getWidth()-7;
        mRenderRect.pos.x += (mRenderRect.dim.x-2)*playerIdx;

        createHUDBlit();
    }
}

/**
 * \brief This function prepares the Background, so in process it can be rendered.
 * This function should only be called once!
 */
void CHUD::CreateVorticonBackground()
{
    // Create a surface for the Background
    mBackground.createRGBSurface(mRenderRect.SDLRect());

    // This also works better with older SDL versions
    mBackground.applyDisplayFormat();
    mBackground.fillRGB(0x00, 0xFE, 0xFF);
    mBackground.setColorKey(0x00, 0xFE, 0xFF);

	SDL_Rect headsrGsRect, headdstrect;
	headsrGsRect.x = 0;
	headsrGsRect.y = 0;
    headdstrect.w = headsrGsRect.w = 16;
	headdstrect.h = headsrGsRect.h = 16;
    headdstrect.x = 0;
    headdstrect.y = 11;


    mKeenHeadSprite.copy(gGraphics.getSprite(mId,PMAPDOWNFRAME));

    if(mKeenHeadSprite.empty())
        return;


    auto &keenHeadSmartSfc = mKeenHeadSprite.Surface();

    keenHeadSmartSfc.blitTo(mBackground, headsrGsRect, headdstrect);

	int sprite=0;
	const int Episode = gBehaviorEngine.getEpisode();
	if(Episode == 1) sprite = OBJ_RAY_DEFSPRITE_EP1;
	else if(Episode == 2) sprite = OBJ_RAY_DEFSPRITE_EP2;
	else if(Episode == 3) sprite = OBJ_RAY_DEFSPRITE_EP3;

	// Draw the shot
    mKeenGunSprite = gGraphics.getSprite(mId,sprite);
    headdstrect.w = headsrGsRect.w = mKeenGunSprite.getWidth();
    headdstrect.h = headsrGsRect.h = mKeenGunSprite.getHeight();
    headdstrect.x = 45-(headsrGsRect.w/2);
    headdstrect.y = 19-(headsrGsRect.h/2);

    auto &keenGunSfc = mKeenGunSprite.Surface();
    keenGunSfc.blitTo(mBackground, headsrGsRect, headdstrect);

	// Draw the rounded borders
    DrawCircle(0, 0, 76);
    DrawCircle(17-4, 15-2, 22);
    DrawCircle(58-4, 15-2, 22);
}

// Draw a circle on the surface
void CHUD::DrawCircle(int x, int y, int width)
{
    GsRect<Uint16> text;

    GsRect<Uint16> outline;


	Uint8 r,g,b;
    auto &Font = gGraphics.getFontLegacy(1);
	Font.getBGColour(&r, &g, &b);

    outline.pos.x = x+4;
    outline.pos.y = y;
    outline.dim.x = width-8;
    outline.dim.y = 12;

    mBackground.fillRGB(outline, 0,0,0);  // Black

    outline.pos.x = x+2;
    outline.pos.y = y+1;
    outline.dim.x = width-4;
    outline.dim.y = 10;

    mBackground.fillRGB(outline, 0,0,0);  // Black

    outline.pos.x = x+1;
    outline.pos.y = y+2;
    outline.dim.x = width-2;
    outline.dim.y = 8;

    mBackground.fillRGB(outline, 0,0,0);  // Black

    outline.pos.x = x;
    outline.pos.y = y+4;
    outline.dim.x = width;
    outline.dim.y = 4;

    mBackground.fillRGB(outline, 0,0,0);  // Black

    text.pos.x = x+4;
    text.pos.y = y+1;
    text.dim.x = width-8;
    text.dim.y = 10;

    mBackground.fillRGBA(text, r,g,b, 255);  // Background colour

    text.pos.x = x+2;
    text.pos.y = y+2;
    text.dim.x = width-4;
    text.dim.y = 8;

    mBackground.fillRGBA(text, r,g,b, 255);  // Background colour

    text.pos.x = x+1;
    text.pos.y = y+4;
    text.dim.x = width-2;
    text.dim.y = 4;

    mBackground.fillRGBA(text, r,g,b, 255);  // Background colour
}

/**
 * \brief This part of the code will render the entire HUD. Galaxy Version
 */
void CHUD::renderGalaxy()
{
  // Compute the score that really will be seen
  int score, lives, charges;
  score = (mOldScore<999999999) ? mOldScore : 999999999;
  lives = (mLives<99) ? mLives : 99;
  charges = (mOldCharges<99) ? mOldCharges : 99;

  // Draw the HUD with all the digits
  auto hudBlitsfc = mHUDBlit.getSDLSurface();

  const int w = mHUDBox.getWidth();
  const int h = mHUDBox.getHeight();

  mHUDBox.drawSprite( hudBlitsfc, -4, 0, w, h);

  if(lives >= 0)
  {
    gGraphics.drawDigits(getRightAlignedString(itoa(score),9), 4, 4, hudBlitsfc );

    CPhysicsSettings &Physics = gBehaviorEngine.getPhysicsSettings();
    if(!Physics.player.infiniteAmmo)
        gGraphics.drawDigits(getRightAlignedString(itoa(charges),2),60, 20, hudBlitsfc );

    gGraphics.drawDigits(getRightAlignedString(itoa(lives),2), 20, 20, hudBlitsfc );

    if(gBehaviorEngine.numPlayers() > 1 && mId == CCamera::getLead())
    {
        SDL_Rect rect;
        rect.x = 7; rect.y = 29;
        rect.h = 1; rect.w = 10;

        SDL_FillRect(hudBlitsfc, &rect, 0xffff0000);
    }
  }

  auto finalRenderRect = mRenderRect;     // Finally pull it a bit down if there are extra borders.
  finalRenderRect.pos.y += gVideoDriver.getVidConfig().mHorizBorders;

  auto weak = GsWeakSurface(gVideoDriver.getBlitSurface());

  mHUDBlit.blitTo(weak, finalRenderRect);
}
/**
 * \brief This part of the code will render the entire HUD. Vorticon version
 */
void CHUD::renderVorticon()
{
	// Compute the score that really will be seen
	int score, lives, charges;
    score = (mOldScore<99999999) ? mOldScore : 99999999;
    lives = (mLives<99) ? mLives : 99;
    charges = (mOldCharges<99) ? mOldCharges : 99;

	// Draw the background
    mBackground.blitTo(mHUDBlit);


    auto &Font = gGraphics.getFontLegacy(1);

    // Print the lives
    Font.drawFont(mHUDBlit, getRightAlignedString(itoa(lives),2), 15, 15, false);

    // Print the charges
    CPhysicsSettings &Physics = gBehaviorEngine.getPhysicsSettings();
    if(!Physics.player.infiniteAmmo)
        Font.drawFont(mHUDBlit, getRightAlignedString(itoa(charges),2), 56, 15, false);

    // Print the score
    Font.drawFont(mHUDBlit, getRightAlignedString(itoa(score),8),8, 2, false );

    if(gBehaviorEngine.numPlayers() > 1)
    {
        SDL_Rect rect;
        rect.x = 2; rect.y = 29;
        rect.h = 2; rect.w = 10;

        if(mId == CCamera::getLead())
        {
            mHUDBlit.fillRGBA(rect, 0xFF, 0x0, 0x0, 0xFF);
        }
        else
        {
            mHUDBlit.fillRGBA(rect, 0x0, 0x0, 0x0, 0x0);
        }
    }


    auto finalRenderRect = mRenderRect;     // Finally pull it a bit down if there are extra borders.
    finalRenderRect.pos.y += gVideoDriver.getVidConfig().mHorizBorders;

    GsWeakSurface blit(gVideoDriver.getBlitSurface());

    mBackground.blitTo(blit, finalRenderRect);
    mHUDBlit.blitTo(blit, finalRenderRect);
}


void CHUD::render()
{
	size_t Episode = gBehaviorEngine.getEpisode();

	timer++;

	if(timer >= EFFECT_TIME)
	{
	    timer = 0;
	    
	    if(mOldCharges < m_charges)
	    {
		mOldCharges++;
	    }
	    
	    else if(mOldCharges > m_charges)
	    {
		mOldCharges--;
	    }        
	}

    int delta = (m_score-mOldScore)/EFFECT_SPEED;

    if(mOldScore < m_score)
	{
	    if(delta == 0)
		delta = 1;
        mOldScore += delta;
        if(mOldScore > m_score)
        mOldScore = m_score;
	}
    else if(mOldScore > m_score)
	{
    	    if(delta == 0)
		delta = -1;

        mOldScore += delta;
            if(mOldScore < m_score)
        mOldScore = m_score;
	}

	if( Episode>=1 && Episode<=3 )
    {
	  renderVorticon();
    }
	else if( Episode>=4 && Episode<=6 )
    {
      renderGalaxy();
    }
}


void CHUD::sync()
{
    mOldCharges = m_charges;
    mOldScore = m_score;
}
