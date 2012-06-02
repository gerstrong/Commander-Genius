/*
 * CHUD.cpp
 *
 *  Created on: 01.07.2010
 *      Author: gerstrong
 */

#include "CHUD.h"
#include "common/CBehaviorEngine.h"
#include "sdl/CVideoDriver.h"
#include "sdl/video/colourconvert.h"
#include "sdl/extensions.h"
#include "graphics/CGfxEngine.h"
#include "StringUtils.h"

CHUD::CHUD(unsigned long &score, signed char &lives,
		   unsigned int &charges, int *camlead) :
m_score(score),
m_lives(lives),
m_charges(charges),
mpCamlead(camlead)
{
	m_Rect.x = 4;
	m_Rect.y = 2;
	m_Rect.w = 80;
	m_Rect.h = 32;

	size_t Episode = g_pBehaviorEngine->getEpisode();

	if( Episode>=1 && Episode<=3 )
		CreateBackground();
	else
	{
		CSprite &HUDBox = g_pGfxEngine->getSprite(129);
		m_Rect.h = HUDBox.getHeight()+2;
		m_Rect.w = HUDBox.getWidth()+2;
		mpHUDBlit = CG_CreateRGBSurface( m_Rect );
		mpHUDBlit = SDL_DisplayFormatAlpha( mpHUDBlit.get() );
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
	int flags = 0;
	mpBackground = CG_CreateRGBSurface( m_Rect );

	SDL_Rect headsrcrect, headdstrect;
	headsrcrect.x = 0;
	headsrcrect.y = 0;
	headdstrect.w = headsrcrect.w = 16;
	headdstrect.h = headsrcrect.h = 16;
	headdstrect.x = m_Rect.x;
	headdstrect.y = m_Rect.y+11;

	CSprite &KeenHeadSprite = g_pGfxEngine->getSprite(PMAPDOWNFRAME);
	temp = SDL_ConvertSurface( KeenHeadSprite.getSDLSurface(), mpBackground->format, flags );
	BlitSurfaceMerge( temp, &headsrcrect, mpBackground.get(), &headdstrect );
	SDL_FreeSurface(temp);

	int sprite=0;
	size_t Episode = g_pBehaviorEngine->getEpisode();
	if(Episode == 1) sprite = OBJ_RAY_DEFSPRITE_EP1;
	else if(Episode == 2) sprite = OBJ_RAY_DEFSPRITE_EP2;
	else if(Episode == 3) sprite = OBJ_RAY_DEFSPRITE_EP3;

	// Draw the shot
	CSprite &KeenGunSprite = g_pGfxEngine->getSprite(sprite);
	headdstrect.w = headsrcrect.w = KeenGunSprite.getWidth();
	headdstrect.h = headsrcrect.h = KeenGunSprite.getHeight();
	headdstrect.x = m_Rect.x+45-(headsrcrect.w/2);
	headdstrect.y = m_Rect.y+19-(headsrcrect.h/2);

	temp = SDL_ConvertSurface( KeenGunSprite.getSDLSurface(), mpBackground->format, flags );
	BlitSurfaceMerge( temp, &headsrcrect, mpBackground.get(), &headdstrect );
	SDL_FreeSurface(temp);


	temp = SDL_DisplayFormatAlpha(mpBackground.get());
	mpBackground = temp;

	mpHUDBlit = SDL_DisplayFormatAlpha(mpBackground.get());

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
void CHUD::renderGalaxy()
{
	m_Rect.x = 4;
	m_Rect.y = 2;
	m_Rect.w = 80;
	m_Rect.h = 29;

	// Compute the score that really will be seen
	int score, lives, charges;
	score = (m_score<999999999) ? m_score : 999999999;
	lives = (m_lives<99) ? m_lives : 99;
	charges = (m_charges<99) ? m_charges : 99;

	// Draw the HUD with all the digits
	SDL_Surface* blitsfc = mpHUDBlit.get();
	CSprite &HUDBox = g_pGfxEngine->getSprite(129);
	HUDBox._drawSprite( blitsfc, m_Rect.x, m_Rect.y );
	g_pGfxEngine->drawDigits(getRightAlignedString(itoa(score),9), m_Rect.x+8, m_Rect.y+4, blitsfc );
	g_pGfxEngine->drawDigits(getRightAlignedString(itoa(charges),2), m_Rect.x+64, m_Rect.y+20, blitsfc );
	g_pGfxEngine->drawDigits(getRightAlignedString(itoa(lives),2), m_Rect.x+24, m_Rect.y+20, blitsfc );

	g_pVideoDriver->mDrawTasks.add( new BlitSurfaceTask( mpHUDBlit, NULL, &m_Rect ) );
}
/**
 * \brief This part of the code will render the entire HUD. Vorticon version
 */
void CHUD::renderVorticon()
{
	// Compute the score that really will be seen
	int score, lives, charges;
	score = (m_score<999999999) ? m_score : 999999999;
	lives = (m_lives<99) ? m_lives : 99;
	charges = (m_charges<99) ? m_charges : 99;

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

	g_pVideoDriver->mDrawTasks.add( new BlitSurfaceTask( mpHUDBlit, NULL, &m_Rect ) );
}

/**
 * \brief This part of the code will render the entire HUD
 */
void CHUD::render()
{
	size_t Episode = g_pBehaviorEngine->getEpisode();

	if( Episode>=1 && Episode<=3 )
		renderVorticon();
	else if( Episode>=4 && Episode<=6 )
		renderGalaxy();
}

