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
#include "graphics/CGfxEngine.h"
#include "StringUtils.h"

CHUD::CHUD(unsigned long &score, signed char &lives, unsigned int &charges) :
m_score(score),
m_lives(lives),
m_charges(charges),
mp_Background(NULL)
{
	m_Rect.x = 4;
	m_Rect.y = 2;
	m_Rect.w = 80;
	m_Rect.h = 29;

	size_t Episode = g_pBehaviorEngine->getEpisode();

	if( Episode>=1 && Episode<=3 )
		CreateBackground();
}

/**
 * \brief This function prepares the Background, so in process it can be rendered.
 * This function should only be called once!
 */
void CHUD::CreateBackground()
{
	// Create a surface for that
	SDL_Surface *temp;
	const Uint32 flags = 0;
	// For some reason the Alpha mask doesn't work, if blitsurface == screensurface. Not sure if every system is affected of that.
	// Maybe I write a function for the proper masks...
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		const Uint32 rmask = 0xFF000000;
		const Uint32 gmask = 0x00FF0000;
		const Uint32 bmask = 0x0000FF00;
    	const Uint32 amask = 0x000000FF;
    #else
    	const Uint32 rmask = 0x000000FF;
    	const Uint32 gmask = 0x0000FF00;
    	const Uint32 bmask = 0x00FF0000;
    	const Uint32 amask = 0xFF000000;
    #endif

	mp_Background = SDL_CreateRGBSurface( flags, m_Rect.w, m_Rect.h, 32, rmask, gmask, bmask, amask);

	SDL_Rect headsrcrect, headdstrect;
	headsrcrect.x = 0;
	headsrcrect.y = 0;
	headdstrect.w = headsrcrect.w = 16;
	headdstrect.h = headsrcrect.h = 16;
	headdstrect.x = m_Rect.x;
	headdstrect.y = m_Rect.y+11;

	CSprite &KeenHeadSprite = g_pGfxEngine->getSprite(PMAPDOWNFRAME);
	temp = SDL_ConvertSurface( KeenHeadSprite.getSDLSurface(), mp_Background->format, flags );
	BlitSurfaceMerge( temp, &headsrcrect, mp_Background, &headdstrect );
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

	temp = SDL_ConvertSurface( KeenGunSprite.getSDLSurface(), mp_Background->format, flags );
	BlitSurfaceMerge( temp, &headsrcrect, mp_Background, &headdstrect );
	SDL_FreeSurface(temp);


	temp = SDL_DisplayFormatAlpha(mp_Background);
	SDL_FreeSurface(mp_Background);
	mp_Background = temp;

	mp_HUDBlit = SDL_DisplayFormatAlpha(mp_Background);

	// Draw the rounded borders
	DrawCircle(0, 0, 80);
	DrawCircle(17, 15, 22);
	DrawCircle(58, 15, 22);
}

// Draw a circle on the surface
void CHUD::DrawCircle(int x, int y, int width)
{
	SDL_Rect text, outline;

	Uint8 r,g,b;
	//CFont &Font = g_pGfxEngine->getFont(0);
	//Font.getBGColour(false, &r, &g, &b);

	outline.x = x+4;
	outline.y = y;
	outline.w = width-8;
	outline.h = 12;
	SDL_FillRect(mp_Background, &outline, SDL_MapRGBA(mp_Background->format, 0,0,0,255)); // Black
	outline.x = x+2;
	outline.y = y+1;
	outline.w = width-4;
	outline.h = 10;
	SDL_FillRect(mp_Background, &outline, SDL_MapRGBA(mp_Background->format, 0,0,0,255)); // Black
	outline.x = x+1;
	outline.y = y+2;
	outline.w = width-2;
	outline.h = 8;
	SDL_FillRect(mp_Background, &outline, SDL_MapRGBA(mp_Background->format, 0,0,0,255)); // Black
	outline.x = x;
	outline.y = y+4;
	outline.w = width;
	outline.h = 4;
	SDL_FillRect(mp_Background, &outline, SDL_MapRGBA(mp_Background->format, 0,0,0,255)); // Black
	text.x = x+4;
	text.y = y+1;
	text.w = width-8;
	text.h = 10;
	SDL_FillRect(mp_Background, &text, SDL_MapRGBA(mp_Background->format, r,g,b,255)); // Background colour
	text.x = x+2;
	text.y = y+2;
	text.w = width-4;
	text.h = 8;
	SDL_FillRect(mp_Background, &text, SDL_MapRGBA(mp_Background->format, r,g,b,255)); // Background colour
	text.x = x+1;
	text.y = y+4;
	text.w = width-2;
	text.h = 4;
	SDL_FillRect(mp_Background, &text, SDL_MapRGBA(mp_Background->format, r,g,b,255)); // Background colour
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

	// Draw the background
	CSprite &HUDBox = g_pGfxEngine->getSprite(129);

	HUDBox.drawSprite( m_Rect.x, m_Rect.y);

	SDL_Surface* blitsfc = g_pVideoDriver->getBlitSurface();
	g_pGfxEngine->drawDigits(getRightAlignedString(itoa(score),9), m_Rect.x+8, m_Rect.y+4, blitsfc );
	g_pGfxEngine->drawDigits(getRightAlignedString(itoa(charges),2), m_Rect.x+64, m_Rect.y+20, blitsfc );
	g_pGfxEngine->drawDigits(getRightAlignedString(itoa(lives),2), m_Rect.x+24, m_Rect.y+20, blitsfc );
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

	SDL_Surface *blitsurface = g_pVideoDriver->getBlitSurface();

	// Draw the background
	g_pVideoDriver->mDrawTasks.add( new BlitSurfaceTask( mp_Background, NULL, &m_Rect ) );
	SDL_BlitSurface(mp_Background, NULL, mp_HUDBlit, NULL );
	
	CFont &Font = g_pGfxEngine->getFont(0);
	//Font.setFGColour(blitsurface->format, 0x000000);
	// Draw the score
	Font.drawFont(mp_HUDBlit, getRightAlignedString(itoa(score),9), m_Rect.x, m_Rect.y);

	// Draw the lives
	Font.drawFont(mp_HUDBlit, getRightAlignedString(itoa(lives),2), 15+m_Rect.x, 15+m_Rect.y);

	// Draw the charges
	Font.drawFont(mp_HUDBlit, getRightAlignedString(itoa(charges),2), 56+m_Rect.x, 15+m_Rect.y);

	//Font.setFGColour(blitsurface->format, 0x0);

	// Draw the background
	g_pVideoDriver->mDrawTasks.add( new BlitSurfaceTask( mp_HUDBlit, NULL, &m_Rect ) );
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


CHUD::~CHUD() {
	if(mp_Background) SDL_FreeSurface(mp_Background);
	if(mp_HUDBlit) SDL_FreeSurface(mp_HUDBlit);
}
