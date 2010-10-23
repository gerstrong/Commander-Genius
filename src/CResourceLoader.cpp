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
mp_thread(NULL),
m_style(PROGRESS_STYLE_TEXT)
{}

/**
 * Set the style how the loading screen will be shown
 */
void CResourceLoader::setStyle(ProgressStyle style)
{
	m_style = style;
}

int _processThread(void *data)
{
	g_pResourceLoader->processThread();
	return 0;
}

/**
 * This will start up the thread for the load display
 */
void CResourceLoader::startLoadingSequence()
{
    //Create and run the thread
	mp_thread = SDL_CreateThread( _processThread, NULL );
	m_threadrunning = true;
}

/**
 * This is the cycle performed while resources are being loaded.
 */
void CResourceLoader::processThread()
{
	m_permil = 0;
	renderLoadingGraphic();
	// Render the screen
	// When enabled, it also will apply Filters
	g_pVideoDriver->updateScreen();

	while(m_threadrunning)
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
		rect.h = 2;

		// RGB - Fade from red to blue
		Uint32 color = 0xFF0000-(((0xFF*m_permil)/1000)<<16);
		color += ((0x0000FF*m_permil)/1000);
		SDL_FillRect(sfc, &rect, color);
	}
}

/**
 * This is called when the program finished loading the requested resources
 */
void CResourceLoader::finishLoadingSequence()
{
	int dummy;
	m_permil = 1000;
	m_threadrunning = false;
	SDL_WaitThread(mp_thread, &dummy);
	mp_thread = NULL;

	renderLoadingGraphic();
	g_pVideoDriver->updateScreen();
}

CResourceLoader::~CResourceLoader() {
	if(mp_thread != NULL)
		SDL_KillThread(mp_thread);
}
