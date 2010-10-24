/*
 * CResourceLoader.cpp
 *
 *  Created on: 23.10.2010
 *      Author: gerstrong
 */

#include "CResourceLoader.h"
#include "graphics/CGfxEngine.h"
#include "sdl/CVideoDriver.h"
#include "sdl/CTimer.h"
#include "StringUtils.h"
#include <cassert>


CResourceLoader::CResourceLoader() :
m_permil(0),
m_min_permil(0),
m_max_permil(1000),
m_style(PROGRESS_STYLE_TEXT),
mp_Thread(NULL)
{}

/**
 * Set the style how the loading screen will be shown
 */
void CResourceLoader::setStyle(ProgressStyle style)
{
	m_style = style;
}

/**
 * This will start up the thread for the load display and process the display of loading
 * and then return
 */
void CResourceLoader::RunLoadAction(Action* act, const std::string &threadname, int min_permil, int max_permil)
{
	assert(mp_Thread == 0);
	m_max_permil = max_permil;
	m_min_permil = min_permil;
	m_permil = m_min_permil;
	mp_Thread = threadPool->start(act, threadname, true);
	process();
}

bool CResourceLoader::process()
{
	SDL_FillRect(g_pVideoDriver->getBlitSurface(), NULL, 0x0);

	if(!mp_Thread)
		return false;
	
	// Do rendering here and the cycle
	while(!threadPool->finalizeIfReady(mp_Thread))
	{
		g_pTimer->TimeToLogic();

		// Render the Screen
		if (g_pTimer->TimeToRender()) {
			renderLoadingGraphic();
			setPermilage(m_permil+1);
			g_pVideoDriver->updateScreen();
		}

		// delay time remaining in current loop
		g_pTimer->TimeToDelay();
	}

	mp_Thread = NULL;
	
	m_permil = m_max_permil;

	return true;
}


/**
 * Set the percentage of progress
 */
void CResourceLoader::setPermilage(int permil)
{
	if(permil<m_max_permil && permil>m_min_permil)
		m_permil = permil;
	else
		m_permil = m_max_permil;
}

/**
 * Progress the shown graphic here
 */
void CResourceLoader::renderLoadingGraphic()
{
	if(m_style == PROGRESS_STYLE_TEXT)
	{
		// Draw Loading Font... here!
		CFont &Font = g_pGfxEngine->getFont(0);
		int percent = m_permil/10;
		int rest = m_permil%10;
		std::string text = "Loading ... " + itoa(percent)+"."+ itoa(rest)+" \%";
		Font.drawFont(g_pVideoDriver->getBlitSurface(), text , 80, 100);
	}
	else if(m_style == PROGRESS_STYLE_BITMAP)
	{
		CBitmap &Bitmap = *g_pGfxEngine->getBitmap("ONEMOMEN");
		SDL_Surface *sfc = g_pVideoDriver->getBlitSurface();
		SDL_Rect rect;
		int width = Bitmap.getWidth();
		int height = Bitmap.getHeight();
		Bitmap.draw(sfc, (320-width)/2, (200-height)/2);
		rect.x = (320-width)/2;
		rect.w = (width*m_permil)/1000;
		rect.y = (200+height)/2;
		rect.h = 4;

		// RGB - Fade from red to blue
		Uint32 color = 0xFF0000-(((0xFF*m_permil)/1000)<<16);
		color += ((0x0000FF*m_permil)/1000);
		SDL_FillRect(sfc, &rect, color);
	}
}
