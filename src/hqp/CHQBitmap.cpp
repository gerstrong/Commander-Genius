/*
 * CHQBitmap.cpp
 *
 *  Created on: 18.04.2009
 *      Author: gerstrong
 */

#include "CHQBitmap.h"
#include "../CLogFile.h"
#include <stdlib.h>

CHQBitmap::CHQBitmap(SDL_Rect screenrect) {
	m_scrimg = NULL;
	m_blackscreen = NULL;
	m_active = false;
	m_alpha = 0;
	memcpy(&m_screenrect,&screenrect,sizeof(SDL_Rect));
}

CHQBitmap::~CHQBitmap() {
	if(m_scrimg){ SDL_FreeSurface(m_scrimg); m_scrimg = NULL;}
	if(m_blackscreen){ SDL_FreeSurface(m_blackscreen); m_blackscreen = NULL;}
}

bool CHQBitmap::loadImage(const char *pFilename, int wsize, int hsize)
{
	m_scrimg = SDL_LoadBMP(pFilename);

	m_active = false;

	if(m_scrimg)
	{

		if( ((m_scrimg->h>>4) > hsize) || ((m_scrimg->w>>4) > wsize) )
		{
			g_pLogFile->textOut(PURPLE,"HQBitmapLoader : The dimensions of the bitmap don't match to the dimensions of the level.<br>");
			g_pLogFile->ftextOut("Please use a proper bitmap with %dx%d dimensions.<br>", m_scrimg->w, m_scrimg->h);
			g_pLogFile->ftextOut("Your bitmap is of %dx%d.<br>", wsize, hsize);
			g_pLogFile->textOut(BLUE,"HQBitmapLoader : Loading the level without HQBitmap.<br>");
		}
		else
		{
			m_active = true;
			// Create a empty black surface for alpha blending with black
			m_blackscreen = SDL_CreateRGBSurface(SDL_SWSURFACE,
								320,240,32,0,0,0,0);
		}
	}
	return m_active;
}

void CHQBitmap::updateHQBitmap(SDL_Surface *m_surface, unsigned int x, unsigned int y)
{
	m_imagerect.x = x;
	m_imagerect.y = y;
	m_imagerect.w = m_surface->w;
	m_imagerect.h = m_surface->h;

	SDL_BlitSurface(m_scrimg, &m_imagerect, m_surface, NULL);

	if(m_alpha == 255)
		return;

	SDL_BlitSurface(m_blackscreen, NULL, m_surface, NULL);
}
void CHQBitmap::setAlphaBlend(Uint8 alpha)
{

	if(alpha == 20)
	{
		m_alpha = 255;
		return;
	}

	m_alpha = alpha*10;
	SDL_SetAlpha(m_blackscreen, SDL_SRCALPHA, 255-m_alpha);
}
void CHQBitmap::offsetAlphaBlend(Uint8 alpha)
{
	m_alpha += alpha;
	SDL_SetAlpha(m_blackscreen, SDL_SRCALPHA, alpha);
}
