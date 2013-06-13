/*
 * CResourceLoader.cpp
 *
 *  Created on: 23.10.2010
 *      Author: gerstrong
 */

#include "CResourceLoader.h"
#include "graphics/CGfxEngine.h"
#include "sdl/extensions.h"
#include "sdl/CVideoDriver.h"
#include "sdl/CTimer.h"
#include "StringUtils.h"
#include <cassert>


CResourceLoader::CResourceLoader() :
m_permil(0),
m_permiltarget(0),
m_min_permil(0),
m_max_permil(1000),
m_style(PROGRESS_STYLE_TEXT)
{
    SDL_Rect rect;
    rect.x = 0;		rect.y = 0;
    rect.w = 320;	rect.h = 200;
    mpProgressSfc.reset( CG_CreateRGBSurface( rect ), &SDL_FreeSurface );
#if SDL_VERSION_ATLEAST(2, 0, 0)
    
#else
    mpProgressSfc.reset( SDL_DisplayFormat(mpProgressSfc.get()), &SDL_FreeSurface );
#endif
}

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
int CResourceLoader::RunLoadAction(Action* act, const std::string &threadname, int min_permil, int max_permil)
{
	assert(!mp_Thread);
	m_max_permil = max_permil;
	m_min_permil = min_permil;
	m_permil = m_min_permil;
	m_permiltarget = m_min_permil;
	mp_Thread.reset(threadPool->start(act, threadname));
	g_pVideoDriver->clearSurfaces();
	
	int ret = 0;
	process(&ret);
	return ret;
}

bool CResourceLoader::process(int* ret)
{
  float acc = 0.0f;
  float start = 0.0f;
  float elapsed = 0.0f;
  float total_elapsed = 0.0f;
  float curr = 0.0f;
  int counter = 0;    
        
    if(!mp_Thread)
	return false;
    
    // Draw the first Frame, so transition looks complete!
    g_pVideoDriver->clearDrawingTasks();
    renderLoadingGraphic();
    g_pVideoDriver->pollDrawingTasks();
    g_pVideoDriver->updateScreen();
    
    start = timerTicks();
    
	// Now, do rendering here and the cycle
	while(!threadPool->finalizeIfReady(mp_Thread.get(), ret))
	{
		const float logicLatency = g_pTimer->LogicLatency();
		const float renderLatency = g_pTimer->RenderLatency();
		
		curr = timerTicks();
		
		elapsed = curr - start;      
		acc += elapsed;
		
		start = timerTicks();
		
		// Perform the game cycle
		while( acc > logicLatency )
		{
		    renderLoadingGraphic();
		    
		    if(m_permil >= m_permiltarget)
		    {
                setPermilage(m_permil+1);
		    }
		    else
		    {
			int delta_permil = (m_permiltarget-m_permil)/2;
			
			if(delta_permil == 0)			
			    setPermilageForce(m_permil+1);
			else
			    setPermilageForce(m_permil+delta_permil);
		    }
		    
		    // Here we try to process all the drawing related Tasks not yet done
		    g_pVideoDriver->pollDrawingTasks();
		    
		    acc -= logicLatency;
		}	
		
		// Pass all the surfaces to one
		g_pVideoDriver->collectSurfaces();
		
		// Now you really render the screen
		// When enabled, it also will apply Filters
		g_pVideoDriver->updateScreen();
		
		elapsed = timerTicks() - start;
		total_elapsed += elapsed;
		
		int waitTime = renderLatency - elapsed;
		
		// wait time remaining in current loop
		if( waitTime > 0 )
		    timerDelay(waitTime);	
		
		total_elapsed += static_cast<float>(waitTime);
		
		// This will refresh the fps display, so it stays readable and calculates an average value.
		counter++;	
		if(counter >= 100)
		{
		    counter = 0;
		    g_pTimer->setTimeforLastLoop(total_elapsed/100.0f);
		    total_elapsed = 0.0f;
		}
	}
	
	// Draw the last Frame, so transition looks complete!
	setPermilageForce(m_max_permil);
	setPermilage(m_max_permil);
	renderLoadingGraphic();
	g_pVideoDriver->pollDrawingTasks();
	g_pVideoDriver->updateScreen();	
	
	m_permiltarget = m_permil = m_min_permil;
	
	// Put everything to zero!
	mp_Thread.release();
	
	g_pTimer->setLogicReset(true);
	
	return true;
}


/**
 * Set the percentage of progress
 */
void CResourceLoader::setPermilageForce(int permil)
{
	if(permil<m_max_permil && permil>m_min_permil)
		m_permil = permil;
	else
		m_permil = m_max_permil;
}

void CResourceLoader::setPermilage(int permil)
{
	if(permil<m_max_permil && permil>=m_min_permil)
	{
		m_permiltarget = permil;
	}
	else
	{
		m_permil = m_permiltarget = m_max_permil;
	}
}


/**
 * Progress the shown graphic here
 */
void CResourceLoader::renderLoadingGraphic()
{
    SDL_Surface *sfc = mpProgressSfc.get();
    SDL_FillRect(sfc, nullptr, 0x0);
    
	if(m_style == PROGRESS_STYLE_TEXT)
	{
		// Draw Loading Font... here!
		CFont &Font = g_pGfxEngine->getFont(0);
		int percent = m_permil/10;
		int rest = m_permil%10;
		std::string text = "Loading ... " + itoa(percent)+"."+ itoa(rest)+" \%";
		
		Font.drawFont(sfc, text , 80, 100, true);
	}
	else if(m_style == PROGRESS_STYLE_BITMAP)
	{
		CBitmap &Bitmap = *g_pGfxEngine->getBitmap("ONEMOMEN");
		SDL_Rect rect;
		int width = Bitmap.getWidth();
		int height = Bitmap.getHeight();
		Bitmap._draw(sfc, (320-width)/2, (200-height)/2);
		
		rect.x = (320-width)/2;
		rect.y = (200+height)/2;
		
		rect.w = (width*m_permil)/1000;		
		rect.h = 4;

		// Fade from yellow to green with this formula
		Uint32 color = SDL_MapRGB(sfc->format, 200-(200*m_permil)/1000, 200, 0 );
		
		SDL_FillRect(sfc, &rect, color);
	}
	else if(m_style == PROGRESS_STYLE_BAR)
	{		
		const int width = 160;
		const int height = 0;
	
		SDL_Rect rect;
		SDL_Rect bgRect;
		rect.x = (320-width)/2;
		rect.y = (200+height)/2;
		
		rect.w = (width*m_permil)/1000;		
		rect.h = 4;
		
		bgRect = rect;
		bgRect.x--;
		bgRect.y--;
		bgRect.w = width+2;
		bgRect.h = 6;

		// Fade from yellow to green with this formula
		Uint32 color = SDL_MapRGB(sfc->format, 200-(200*m_permil)/1000, 200, 0 );
		
		SDL_FillRect(sfc, &bgRect, SDL_MapRGB(sfc->format, 128, 128, 128));
		SDL_FillRect(sfc, &rect, color);
	}
	
    SDL_BlitSurface( mpProgressSfc.get(), nullptr, g_pVideoDriver->getBlitSurface(), nullptr );
}
