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
    CRect<Uint16> gameRes = g_pVideoDriver->getGameResolution();
    rect.x = 0;		rect.y = 0;
    rect.w = gameRes.w;	rect.h = gameRes.h;

    mpProgressSfc.reset( CG_CreateRGBSurface( rect ), &SDL_FreeSurface );
    mpProgressSfc.reset( g_pVideoDriver->convertThroughBlitSfc(mpProgressSfc.get()), &SDL_FreeSurface );
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

    // Background go black while rendering
    SDL_Surface *blit = g_pVideoDriver->getBlitSurface();
    SDL_FillRect( blit, nullptr, SDL_MapRGB(blit->format, 0, 0, 0) );

    
    // Draw the first Frame, so transition looks complete!
    renderLoadingGraphic();
    g_pVideoDriver->updateDisplay();
    
    start = timerTicks();

    bool threadFinalized = false;
    
	// Now, do rendering here and the cycle
    while(!threadFinalized || g_pGfxEngine->applyingEffects())
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
		    acc -= logicLatency;

            // Apply graphical effects if any. It does not render, it only prepares for the rendering task.
            g_pGfxEngine->ponder();

		}	
		
		// Pass all the surfaces to one
		g_pVideoDriver->collectSurfaces();

        // Apply graphical effects if any.
        g_pGfxEngine->render();
		
		// Now you really render the screen
		// When enabled, it also will apply Filters
        g_pVideoDriver->updateDisplay();
		
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

        if(!threadFinalized)
            threadFinalized = threadPool->finalizeIfReady(mp_Thread.get(), ret);
	}
	
	// Draw the last Frame, so transition looks complete!
	setPermilageForce(m_max_permil);
	setPermilage(m_max_permil);
	renderLoadingGraphic();
    g_pVideoDriver->updateDisplay();
	
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
    
    CRect<Uint16> gameRes = g_pVideoDriver->getGameResolution();

    const int gameWidth = gameRes.w;
    const int gameHeight = gameRes.h;
    const int halfWidth = gameWidth/2;

	if(m_style == PROGRESS_STYLE_TEXT)
	{
		// Draw Loading Font... here!
		CFont &Font = g_pGfxEngine->getFont(0);
		int percent = m_permil/10;
		int rest = m_permil%10;
		std::string text = "Loading ... " + itoa(percent)+"."+ itoa(rest)+" \%";
		
        SDL_Rect textRect;
        CRect<Uint16> gameRes = g_pVideoDriver->getGameResolution();
        const float scaleUpW = float(gameRes.w)/320.0f;
        const float scaleUpH = float(gameRes.h)/200.0f;
        textRect.x = (int)(80.0*scaleUpW);        textRect.y = (int)(100.0*scaleUpH);
        textRect.w = 200;        textRect.h = 10;
        SDL_FillRect(sfc, &textRect, SDL_MapRGB(sfc->format, 0, 0, 0));
        Font.drawFont(sfc, text , textRect.x, textRect.y, true);
	}
	else if(m_style == PROGRESS_STYLE_BITMAP)
	{
		CBitmap &Bitmap = *g_pGfxEngine->getBitmap("ONEMOMEN");
		SDL_Rect rect;
		int width = Bitmap.getWidth();
		int height = Bitmap.getHeight();
        Bitmap._draw( (gameWidth-width)/2, (gameHeight-height)/2, sfc);
		
        rect.x = (gameWidth-width)/2;
        rect.y = (gameHeight+height)/2;
		
        rect.w = (width*m_permil)/1000;
		rect.h = 4;

		// Fade from yellow to green with this formula
		Uint32 color = SDL_MapRGB(sfc->format, 200-(200*m_permil)/1000, 200, 0 );
		
		SDL_FillRect(sfc, &rect, color);
	}
	else if(m_style == PROGRESS_STYLE_BAR)
	{			
		SDL_Rect rect;
		SDL_Rect bgRect;
        rect.x = (gameWidth-halfWidth)/2;
        rect.y = gameHeight/2;
		
        rect.w = (halfWidth*m_permil)/1000;
        rect.h = gameHeight/50;
		
		bgRect = rect;
		bgRect.x--;
		bgRect.y--;
        bgRect.w = halfWidth+2;
        bgRect.h = rect.h+2;

		// Fade from yellow to green with this formula
        Uint32 color = SDL_MapRGB(sfc->format, gameHeight-(gameHeight*m_permil)/1000, gameHeight, 0 );
		
		SDL_FillRect(sfc, &bgRect, SDL_MapRGB(sfc->format, 128, 128, 128));
		SDL_FillRect(sfc, &rect, color);
	}
	
    SDL_BlitSurface( mpProgressSfc.get(), nullptr, g_pVideoDriver->getBlitSurface(), nullptr );
}
