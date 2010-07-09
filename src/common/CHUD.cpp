/*
 * CHUD.cpp
 *
 *  Created on: 01.07.2010
 *      Author: gerstrong
 */

#include "CHUD.h"
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
	const Uint32 amask = pBlitsurface->format->Amask;
	mp_Background = SDL_CreateRGBSurface(flags, m_Rect.w, m_Rect.h, depth, rmask, gmask, bmask, amask);

	// Draw the background
	SDL_Rect text, outline;
	//SDL_FillRect(mp_Background, NULL, SDL_MapRGBA(mp_Background->format, 0,255,0,255)); // Black
	outline.x = 4;
	outline.y = 0;
	outline.w = 72;
	outline.h = 12;
	SDL_FillRect(mp_Background, &outline, SDL_MapRGBA(mp_Background->format, 0,0,0,255)); // Black
	outline.x = 2;
	outline.y = 1;
	outline.w = 76;
	outline.h = 10;
	SDL_FillRect(mp_Background, &outline, SDL_MapRGBA(mp_Background->format, 0,0,0,255)); // Black
	outline.x = 1;
	outline.y = 2;
	outline.w = 78;
	outline.h = 8;
	SDL_FillRect(mp_Background, &outline, SDL_MapRGBA(mp_Background->format, 0,0,0,255)); // Black
	outline.x = 0;
	outline.y = 4;
	outline.w = 80;
	outline.h = 4;
	SDL_FillRect(mp_Background, &outline, SDL_MapRGBA(mp_Background->format, 0,0,0,255)); // Black
	outline.x = 21;
	outline.y = 15;
	outline.w = 14;
	outline.h = 12;
	SDL_FillRect(mp_Background, &outline, SDL_MapRGBA(mp_Background->format, 0,0,0,255)); // Black
	outline.x = 19;
	outline.y = 16;
	outline.w = 18;
	outline.h = 10;
	SDL_FillRect(mp_Background, &outline, SDL_MapRGBA(mp_Background->format, 0,0,0,255)); // Black
	outline.x = 18;
	outline.y = 17;
	outline.w = 20;
	outline.h = 8;
	SDL_FillRect(mp_Background, &outline, SDL_MapRGBA(mp_Background->format, 0,0,0,255)); // Black
	outline.x = 17;
	outline.y = 19;
	outline.w = 22;
	outline.h = 4;
	SDL_FillRect(mp_Background, &outline, SDL_MapRGBA(mp_Background->format, 0,0,0,255)); // Black
	outline.x = 62;
	outline.y = 15;
	outline.w = 14;
	outline.h = 12;
	SDL_FillRect(mp_Background, &outline, SDL_MapRGBA(mp_Background->format, 0,0,0,255)); // Black
	outline.x = 60;
	outline.y = 16;
	outline.w = 18;
	outline.h = 10;
	SDL_FillRect(mp_Background, &outline, SDL_MapRGBA(mp_Background->format, 0,0,0,255)); // Black
	outline.x = 59;
	outline.y = 17;
	outline.w = 20;
	outline.h = 8;
	SDL_FillRect(mp_Background, &outline, SDL_MapRGBA(mp_Background->format, 0,0,0,255)); // Black
	outline.x = 58;
	outline.y = 19;
	outline.w = 22;
	outline.h = 4;
	SDL_FillRect(mp_Background, &outline, SDL_MapRGBA(mp_Background->format, 0,0,0,255)); // Black
	text.x = 4;
	text.y = 1;
	text.w = 72;
	text.h = 10;
	SDL_FillRect(mp_Background, &text, SDL_MapRGBA(mp_Background->format, 255,255,255,255)); // White
	text.x = 2;
	text.y = 2;
	text.w = 76;
	text.h = 8;
	SDL_FillRect(mp_Background, &text, SDL_MapRGBA(mp_Background->format, 255,255,255,255)); // White
	text.x = 1;
	text.y = 4;
	text.w = 78;
	text.h = 4;
	SDL_FillRect(mp_Background, &text, SDL_MapRGBA(mp_Background->format, 255,255,255,255)); // White
	text.x = 21;
	text.y = 16;
	text.w = 14;
	text.h = 10;
	SDL_FillRect(mp_Background, &text, SDL_MapRGBA(mp_Background->format, 255,255,255,255)); // White
	text.x = 19;
	text.y = 17;
	text.w = 18;
	text.h = 8;
	SDL_FillRect(mp_Background, &text, SDL_MapRGBA(mp_Background->format, 255,255,255,255)); // White
	text.x = 18;
	text.y = 19;
	text.w = 20;
	text.h = 4;
	SDL_FillRect(mp_Background, &text, SDL_MapRGBA(mp_Background->format, 255,255,255,255)); // White
	text.x = 62;
	text.y = 16;
	text.w = 14;
	text.h = 10;
	SDL_FillRect(mp_Background, &text, SDL_MapRGBA(mp_Background->format, 255,255,255,255)); // White
	text.x = 60;
	text.y = 17;
	text.w = 18;
	text.h = 8;
	SDL_FillRect(mp_Background, &text, SDL_MapRGBA(mp_Background->format, 255,255,255,255)); // White
	text.x = 59;
	text.y = 19;
	text.w = 20;
	text.h = 4;
	SDL_FillRect(mp_Background, &text, SDL_MapRGBA(mp_Background->format, 255,255,255,255)); // White
	
	// Draw the keen head icon
	SDL_Rect headsrcrect, headdstrect;
	headsrcrect.x = 0;
	headsrcrect.y = 0;
	headdstrect.w = headsrcrect.w = 16;
	headdstrect.h = headsrcrect.h = 10;
	headdstrect.x = 1;
	headdstrect.y = 16;
	CSprite &KeenHeadSprite = g_pGfxEngine->getSprite(PFIREFRAME);
	SDL_FillRect(mp_Background, &headdstrect, SDL_MapRGBA(mp_Background->format, 0,0,0,255)); // Black
	SDL_BlitSurface( KeenHeadSprite.getSDLSurface(), &headsrcrect, mp_Background, &headdstrect);

	// Draw the pistol
	headsrcrect.x = 2;
	headsrcrect.y = 9;
	headdstrect.w = headsrcrect.w = 16;
	headdstrect.h = headsrcrect.h = 10;
	headdstrect.x = 41;
	headdstrect.y = 16;
	SDL_FillRect(mp_Background, &headdstrect, SDL_MapRGBA(mp_Background->format, 0,0,0,255)); // Black
	SDL_BlitSurface( KeenHeadSprite.getSDLSurface(), &headsrcrect, mp_Background, &headdstrect);

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

	CFont &Font = g_pGfxEngine->getFont(0);
	Font.setFGColour(blitsurface->format, 0x000000);
	// Draw the score
	Font.drawFont(blitsurface, getRightAligned(itoa(score),9), 4+m_Rect.x, 2+m_Rect.y);

	// Draw the lives
	Font.drawFont(blitsurface, getRightAligned(itoa(lives),2), 20+m_Rect.x, 17+m_Rect.y);

	// Draw the charges
	Font.drawFont(blitsurface, getRightAligned(itoa(charges),2), 61+m_Rect.x, 17+m_Rect.y);

	Font.setFGColour(blitsurface->format, 0x0);
}


CHUD::~CHUD() {
	if(mp_Background) SDL_FreeSurface(mp_Background);
}
