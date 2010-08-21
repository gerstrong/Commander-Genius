/*
 * CHUD.cpp
 *
 *  Created on: 01.07.2010
 *      Author: gerstrong
 */

#include "CHUD.h"
#include "CBehaviorEngine.h"
#include "../sdl/CVideoDriver.h"
#include "../graphics/CGfxEngine.h"
#include "../StringUtils.h"

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
	CreateBackground();
}

std::string getRightAligned(std::string text, size_t size)
{
	if(text.size() > size)
		return text;

	std::string output;
	for(size_t i=0 ; i<(size-text.size()) ; i++)
		output += ' ';
	output += text;
	return output;
}

/**
 * \brief This function prepares the Background, so in process it can be rendered.
 * This function should only be called once!
 */
void CHUD::CreateBackground()
{
	// Create a surface for that
	SDL_Surface *pBlitsurface = g_pVideoDriver->getBlitSurface();
	const Uint32 flags = pBlitsurface->flags;
	const Uint32 depth = pBlitsurface->format->BitsPerPixel;
	const Uint32 rmask = pBlitsurface->format->Rmask;
	const Uint32 gmask = pBlitsurface->format->Gmask;
	const Uint32 bmask = pBlitsurface->format->Bmask;
	// For some reason the Alpha mask doesn't work, if blitsurface == screensurface. Not sure if every system is affected of that.
	// Maybe I write a function for the proper masks...
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    	const Uint32 amask = 0x000000FF;
    #else
    	const Uint32 amask = 0xFF000000;
    #endif

	mp_Background = SDL_CreateRGBSurface(flags, m_Rect.w, m_Rect.h, depth, rmask, gmask, bmask, amask);

	// Draw the background
	DrawCircle(0, 0, 80);
	DrawCircle(17, 15, 22);
	DrawCircle(58, 15, 22);
}

void CHUD::DrawCircle(int x, int y, int width)
{
	SDL_Rect text, outline;

	Uint8 r,g,b;
	CFont &Font = g_pGfxEngine->getFont(0);
	Uint32 bgcolor = Font.getBGColour(false);
	SDL_GetRGB(bgcolor, mp_Background->format, &r, &g, &b);

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
	SDL_FillRect(mp_Background, &text, SDL_MapRGBA(mp_Background->format, r,g,b,255)); // White
	text.x = x+2;
	text.y = y+2;
	text.w = width-4;
	text.h = 8;
	SDL_FillRect(mp_Background, &text, SDL_MapRGBA(mp_Background->format, r,g,b,255)); // White
	text.x = x+1;
	text.y = y+4;
	text.w = width-2;
	text.h = 4;
	SDL_FillRect(mp_Background, &text, SDL_MapRGBA(mp_Background->format, r,g,b,255)); // White
}

/**
 * \brief This part of the code will render the entire HUD
 */
void CHUD::render()
{
	// Compute the score that really will be seen
	int score, lives, charges;
	score = (m_score<999999999) ? m_score : 999999999;
	lives = (m_lives<99) ? m_lives : 99;
	charges = (m_charges<99) ? m_charges : 99;

	SDL_Surface *blitsurface = g_pVideoDriver->getBlitSurface();

	// Draw the background
	SDL_BlitSurface( mp_Background, NULL, blitsurface, &m_Rect);
	
		// Draw the keen head icon
	SDL_Rect headsrcrect, headdstrect;
	headsrcrect.x = 0;
	headsrcrect.y = 0;
	headdstrect.w = headsrcrect.w = 16;
	headdstrect.h = headsrcrect.h = 16;
	headdstrect.x = m_Rect.x;
	headdstrect.y = m_Rect.y+13;
	CSprite &KeenHeadSprite = g_pGfxEngine->getSprite(PMAPDOWNFRAME);
	SDL_BlitSurface( KeenHeadSprite.getSDLSurface(), &headsrcrect, blitsurface, &headdstrect);

	int sprite=0;
	size_t Episode = g_pBehaviorEngine->getEpisode();
	if(Episode == 1) sprite = OBJ_RAY_DEFSPRITE_EP1;
	else if(Episode == 2) sprite = OBJ_RAY_DEFSPRITE_EP2;
	else if(Episode == 3) sprite = OBJ_RAY_DEFSPRITE_EP3;
	
	// Draw the pistol
	CSprite &KeenGunSprite = g_pGfxEngine->getSprite(sprite);
	headdstrect.w = headsrcrect.w = KeenGunSprite.getWidth();
	headdstrect.h = headsrcrect.h = KeenGunSprite.getHeight();
	headdstrect.x = m_Rect.x+49-(headsrcrect.w/2);
	headdstrect.y = m_Rect.y+21-(headsrcrect.h/2);
	SDL_BlitSurface( KeenGunSprite.getSDLSurface(), &headsrcrect, blitsurface, &headdstrect);

	CFont &Font = g_pGfxEngine->getFont(0);
	Font.setFGColour(blitsurface->format, 0x000000);
	// Draw the score
	Font.drawFont(blitsurface, getRightAligned(itoa(score),9), 5+m_Rect.x, 2+m_Rect.y);

	// Draw the lives
	Font.drawFont(blitsurface, getRightAligned(itoa(lives),2), 21+m_Rect.x, 17+m_Rect.y);

	// Draw the charges
	Font.drawFont(blitsurface, getRightAligned(itoa(charges),2), 62+m_Rect.x, 17+m_Rect.y);

	Font.setFGColour(blitsurface->format, 0x0);
}


CHUD::~CHUD() {
	if(mp_Background) SDL_FreeSurface(mp_Background);
}
