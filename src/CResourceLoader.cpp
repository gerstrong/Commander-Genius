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


CResourceLoader::CResourceLoader() :
m_threadrunning(false),
m_permil(0),
m_style(PROGRESS_STYLE_TEXT),
m_ThreadPool(1)
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
int CResourceLoader::RunLoadAction(Action* act, const std::string &threadname)
{
	mp_Thread = m_ThreadPool.start(act, threadname, true);
	process();
	return mp_Thread->ret;
}

bool CResourceLoader::process()
{
	if(!mp_Thread.get())
		return false;
	// Do rendering here and the cycle
	m_permil = 0;
	while(!m_ThreadPool.finished(mp_Thread.get()))
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

	m_ThreadPool.waitAll();

	return true;
}


/**
 * Set the percentage of progress
 */
void CResourceLoader::setPermilage(int permil)
{
	if(permil<1000 && permil>0)
		m_permil = permil;
	else
		m_permil = 1000;
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
		std::string text = "Loading ...         ";
		Font.drawFont(g_pVideoDriver->getBlitSurface(), text , 80, 100);
		text = "Loading ... " + itoa(percent)+"."+ itoa(rest)+" \%";
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
		rect.h = 2;

		// RGB - Fade from red to blue
		Uint32 color = 0xFF0000-(((0xFF*m_permil)/1000)<<16);
		color += ((0x0000FF*m_permil)/1000);
		SDL_FillRect(sfc, &rect, color);
	}
}
