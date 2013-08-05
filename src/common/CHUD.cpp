/*
 * CHUD.cpp
 *
 *  Created on: 01.07.2010
 *      Author: gerstrong
 */

#include "CHUD.h"
#include "common/CBehaviorEngine.h"
#include "sdl/CVideoDriver.h"
#include "sdl/extensions.h"
#include "graphics/CGfxEngine.h"
#include "StringUtils.h"

const int EFFECT_TIME = 10;
const int EFFECT_SPEED = 10;

CHUD::CHUD(unsigned long &score, signed char &lives,
           unsigned int &charges, const int id,
           int *camlead) :
m_score(score),
m_lives(lives),
m_charges(charges),
m_oldScore(score),
m_oldCharges(charges),
mpHUDBox(NULL),
mpCamlead(camlead),
timer(0)
{
    setup(id);
}

void CHUD::setup(const int id)
{
    mId = id;
    m_Rect.x = 4;	m_Rect.y = 2;
    m_Rect.w = 80;	m_Rect.h = 32;

    size_t Episode = g_pBehaviorEngine->getEpisode();

    if( Episode>=1 && Episode<=3 )
        CreateBackground();
    else
    {
        mpHUDBox = g_pGfxEngine->getSprite(mId,"HUDBACKGROUND");
        m_Rect.h = mpHUDBox->getHeight()+2;
        m_Rect.w = (mpHUDBox->getWidth()+2)*4;
        mpHUDBlit.reset( CG_CreateRGBSurface( m_Rect ), &SDL_FreeSurface );
#if SDL_VERSION_ATLEAST(2, 0, 0)

#else
        mpHUDBlit.reset( SDL_DisplayFormatAlpha(mpHUDBlit.get()), &SDL_FreeSurface );
#endif
    }
}

/**
 * \brief This function prepares the Background, so in process it can be rendered.
 * This function should only be called once!
 */
void CHUD::CreateBackground()
{
	// Create a surface for that
	SDL_Surface *temp;
	mpBackground.reset( CG_CreateRGBSurface( m_Rect ) );

	SDL_Rect headsrcrect, headdstrect;
	headsrcrect.x = 0;
	headsrcrect.y = 0;
	headdstrect.w = headsrcrect.w = 16;
	headdstrect.h = headsrcrect.h = 16;
	headdstrect.x = m_Rect.x;
	headdstrect.y = m_Rect.y+11;
	
#if SDL_VERSION_ATLEAST(2, 0, 0)
    
#else
	temp = SDL_DisplayFormat(mpBackground.get());
	mpBackground.reset(temp);
#endif
	
	const Uint32 colorkey = SDL_MapRGB(mpBackground->format, 0x00, 0xFF, 0xFF);	
	SDL_FillRect(temp, NULL, colorkey );
#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetColorKey( temp, SDL_TRUE, colorkey );
#else
	SDL_SetColorKey( temp, SDL_SRCCOLORKEY, colorkey );
#endif

    CSprite &KeenHeadSprite = g_pGfxEngine->getSprite(mId,PMAPDOWNFRAME);
	SDL_BlitSurface( KeenHeadSprite.getSDLSurface(), &headsrcrect, mpBackground.get(), &headdstrect );	

	int sprite=0;
	const int Episode = g_pBehaviorEngine->getEpisode();
	if(Episode == 1) sprite = OBJ_RAY_DEFSPRITE_EP1;
	else if(Episode == 2) sprite = OBJ_RAY_DEFSPRITE_EP2;
	else if(Episode == 3) sprite = OBJ_RAY_DEFSPRITE_EP3;

	// Draw the shot
    CSprite &KeenGunSprite = g_pGfxEngine->getSprite(mId,sprite);
	headdstrect.w = headsrcrect.w = KeenGunSprite.getWidth();
	headdstrect.h = headsrcrect.h = KeenGunSprite.getHeight();
	headdstrect.x = m_Rect.x+45-(headsrcrect.w/2);
	headdstrect.y = m_Rect.y+19-(headsrcrect.h/2);
		
	SDL_BlitSurface( KeenGunSprite.getSDLSurface(), &headsrcrect, mpBackground.get(), &headdstrect );
	
#if SDL_VERSION_ATLEAST(2, 0, 0)
    
#else
    mpHUDBlit.reset(SDL_DisplayFormatAlpha(mpBackground.get()), &SDL_FreeSurface);
#endif

	// Draw the rounded borders
	DrawCircle(0, 0, 80);
	DrawCircle(17, 15, 22);
	DrawCircle(58, 15, 22);
	if(g_pBehaviorEngine->mPlayers > 1)
		DrawCircle(0, 0, 15);
}

// Draw a circle on the surface
void CHUD::DrawCircle(int x, int y, int width)
{
	SDL_Rect text, outline;

	Uint8 r,g,b;
	CFont &Font = g_pGfxEngine->getFont(1);
	Font.getBGColour(&r, &g, &b);

	outline.x = x+4;
	outline.y = y;
	outline.w = width-8;
	outline.h = 12;
	SDL_FillRect(mpBackground.get(), &outline, SDL_MapRGBA(mpBackground->format, 0,0,0,255)); // Black
	outline.x = x+2;
	outline.y = y+1;
	outline.w = width-4;
	outline.h = 10;
	SDL_FillRect(mpBackground.get(), &outline, SDL_MapRGBA(mpBackground->format, 0,0,0,255)); // Black
	outline.x = x+1;
	outline.y = y+2;
	outline.w = width-2;
	outline.h = 8;
	SDL_FillRect(mpBackground.get(), &outline, SDL_MapRGBA(mpBackground->format, 0,0,0,255)); // Black
	outline.x = x;
	outline.y = y+4;
	outline.w = width;
	outline.h = 4;
	SDL_FillRect(mpBackground.get(), &outline, SDL_MapRGBA(mpBackground->format, 0,0,0,255)); // Black
	text.x = x+4;
	text.y = y+1;
	text.w = width-8;
	text.h = 10;
	SDL_FillRect(mpBackground.get(), &text, SDL_MapRGBA(mpBackground->format, r,g,b,255)); // Background colour
	text.x = x+2;
	text.y = y+2;
	text.w = width-4;
	text.h = 8;
	SDL_FillRect(mpBackground.get(), &text, SDL_MapRGBA(mpBackground->format, r,g,b,255)); // Background colour
	text.x = x+1;
	text.y = y+4;
	text.w = width-2;
	text.h = 4;
	SDL_FillRect(mpBackground.get(), &text, SDL_MapRGBA(mpBackground->format, r,g,b,255)); // Background colour
}

/**
 * \brief This part of the code will render the entire HUD. Galaxy Version
 */
void CHUD::renderGalaxy(const int place,  const int players)
{
  auto rect = m_Rect;

  rect.w = 40;	rect.h = 29;

  if(players > 3)
  {
      rect.x = place*(rect.w-1);	rect.y = 0;
  }
  else
  {
      rect.x = 4 + place*rect.w;	rect.y = 2;
  }


  // Compute the score that really will be seen
  int score, lives, charges;
  score = (m_oldScore<999999999) ? m_oldScore : 999999999;
  lives = (m_lives<99) ? m_lives : 99;
  charges = (m_oldCharges<99) ? m_oldCharges : 99;

  // Draw the HUD with all the digits
  SDL_Surface* blitsfc = mpHUDBlit.get();
  mpHUDBox->drawSprite( blitsfc, rect.x, rect.y );
  g_pGfxEngine->drawDigits(getRightAlignedString(itoa(score),9), rect.x+8, rect.y+4, blitsfc );
  g_pGfxEngine->drawDigits(getRightAlignedString(itoa(charges),2), rect.x+64, rect.y+20, blitsfc );
  g_pGfxEngine->drawDigits(getRightAlignedString(itoa(lives),2), rect.x+24, rect.y+20, blitsfc );

  SDL_BlitSurface( blitsfc, NULL, g_pVideoDriver->getBlitSurface(), &rect );
}
/**
 * \brief This part of the code will render the entire HUD. Vorticon version
 */
void CHUD::renderVorticon()
{
	// Compute the score that really will be seen
	int score, lives, charges;
	score = (m_oldScore<999999999) ? m_oldScore : 999999999;
	lives = (m_lives<99) ? m_lives : 99;
	charges = (m_oldCharges<99) ? m_oldCharges : 99;

	// Draw the background
	SDL_BlitSurface(mpBackground.get(), NULL, mpHUDBlit.get(), NULL );
	
	CFont &Font = g_pGfxEngine->getFont(1);
	// Draw the lives
	Font.drawFont(mpHUDBlit.get(), getRightAlignedString(itoa(lives),2), 15+m_Rect.x, 15+m_Rect.y);

	// Draw the charges
	Font.drawFont(mpHUDBlit.get(), getRightAlignedString(itoa(charges),2), 56+m_Rect.x, 15+m_Rect.y);

	// In multiplayer mode we show a number indicating the cam owner
	if( mpCamlead && g_pBehaviorEngine->mPlayers > 1 )
	{
		// Draw the Player which controls the camera
		Font.drawFont(mpHUDBlit.get(), itoa((*mpCamlead)+1), m_Rect.x, m_Rect.y);

		// Draw the score
		Font.drawFont(mpHUDBlit.get(), getRightAlignedString(itoa(score),7), 16+m_Rect.x, m_Rect.y);
	}
	else
	{
		Font.drawFont(mpHUDBlit.get(), getRightAlignedString(itoa(score),9), m_Rect.x, m_Rect.y);
	}

    SDL_BlitSurface( mpHUDBlit.get(), NULL, g_pVideoDriver->getBlitSurface(), &m_Rect );
}


void CHUD::render(const int place, const int players)
{
	size_t Episode = g_pBehaviorEngine->getEpisode();
	
	timer++;
	
	if(timer >= EFFECT_TIME)
	{
	    timer = 0;
	    
	    if(m_oldCharges < m_charges)
		m_oldCharges++;
	    else if(m_oldCharges > m_charges)
		m_oldCharges--;
	}
	
	int delta = (m_score-m_oldScore)/EFFECT_SPEED;
	
	if(m_oldScore < m_score)
	{
	    if(delta == 0)
		delta = 1;
	    m_oldScore += delta;
	    if(m_oldScore > m_score)
		m_oldScore = m_score;
	}
	else if(m_oldScore > m_score)
	{
    	    if(delta == 0)
		delta = -1;

	    m_oldScore += delta;
    	    if(m_oldScore < m_score)
		m_oldScore = m_score;
	}

	if( Episode>=1 && Episode<=3 )
	  renderVorticon();
	else if( Episode>=4 && Episode<=6 )
	  renderGalaxy(place, players);
}


void CHUD::sync()
{
    m_oldCharges = m_charges;
    m_oldScore = m_score;
}
