/*
 * CResourceLoader.cpp
 *
 *  Created on: 23.10.2010
 *      Author: gerstrong
 */

#include "CResourceLoader.h"
#include "graphics/GsGraphics.h"
#include "sdl/extensions.h"
#include <base/video/CVideoDriver.h>
#include <base/GsTimer.h>
#include <base/utils/StringUtils.h>
#include <cassert>


CResourceLoader::CResourceLoader() :
m_permil(0),
m_permiltarget(0),
m_min_permil(0),
m_max_permil(1000),
m_style(PROGRESS_STYLE_TEXT)
{
    SDL_Rect rect;
    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();
    rect.x = 0;		rect.y = 0;
    rect.w = gameRes.w;	rect.h = gameRes.h;

    mpProgressSfc.reset( CG_CreateRGBSurface( rect ), &SDL_FreeSurface );
    mpProgressSfc.reset( gVideoDriver.convertThroughBlitSfc(mpProgressSfc.get()), &SDL_FreeSurface );
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
int CResourceLoader::RunLoadAction(Action* act,
                                   const std::string &threadname,
                                   const int min_permil,
                                   const int max_permil)
{
	assert(!mp_Thread);
	m_max_permil = max_permil;
	m_min_permil = min_permil;
	m_permil = m_min_permil;
	m_permiltarget = m_min_permil;
    mp_Thread = threadPool->start(act, threadname);
	gVideoDriver.clearSurfaces();
	
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
    SDL_Surface *blit = gVideoDriver.getBlitSurface();
    SDL_FillRect( blit, nullptr, SDL_MapRGB(blit->format, 0, 0, 0) );

    
    // Draw the first Frame, so transition looks complete!
    renderLoadingGraphic();
    gVideoDriver.updateDisplay();
    
    start = timerTicks();

    bool threadFinalized = false;
    
	// Now, do rendering here and the cycle
    while(!threadFinalized || gEffectController.applyingEffects())
	{                
        const float logicLatency = gTimer.LogicLatency();
        const float renderLatency = gTimer.RenderLatency();

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
            gEffectController.run(logicLatency);
        }
		
		// Pass all the surfaces to one
		gVideoDriver.collectSurfaces();

        // Apply graphical effects if any.
        gEffectController.render();
		
		// Now you really render the screen
		// When enabled, it also will apply Filters
        gVideoDriver.updateDisplay();
		
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
            gTimer.setTimeforLastLoop(total_elapsed/100.0f);
		    total_elapsed = 0.0f;
		}

        if(!threadFinalized)
            threadFinalized = threadPool->finalizeIfReady(mp_Thread, ret);
	}
	
	// Draw the last Frame, so transition looks complete!
	setPermilageForce(m_max_permil);
	setPermilage(m_max_permil);
	renderLoadingGraphic();
    gVideoDriver.updateDisplay();
	
	m_permiltarget = m_permil = m_min_permil;
	
    mp_Thread = nullptr;
	
    gTimer.setLogicReset(true);
	
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
    
    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();

    const int gameWidth = gameRes.w;
    const int gameHeight = gameRes.h;
    const int halfWidth = gameWidth/2;

	if(m_style == PROGRESS_STYLE_TEXT)
	{
		// Draw Loading Font... here!
        GsFont &Font = gGraphics.getFont(0);
		int percent = m_permil/10;
		int rest = m_permil%10;
		std::string text = "Loading ... " + itoa(percent)+"."+ itoa(rest)+" \%";
		
        SDL_Rect textRect;
        GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();
        const float scaleUpW = float(gameRes.w)/320.0f;
        const float scaleUpH = float(gameRes.h)/200.0f;
        textRect.x = (int)(80.0*scaleUpW);        textRect.y = (int)(100.0*scaleUpH);
        textRect.w = 200;        textRect.h = 10;
        SDL_FillRect(sfc, &textRect, SDL_MapRGB(sfc->format, 0, 0, 0));
        Font.drawFont(sfc, text , textRect.x, textRect.y, true);
	}
	else if(m_style == PROGRESS_STYLE_BITMAP)
	{
        GsBitmap &Bitmap = *gGraphics.getBitmapFromStr("ONEMOMEN");
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
	
    // In there is garbage of other drawn stuff clean it up.
    auto blit = gVideoDriver.getBlitSurface();

    SDL_FillRect( blit, nullptr, SDL_MapRGB(blit->format, 0,0,0) );

    BlitSurface( mpProgressSfc.get(), nullptr, blit, nullptr );
}



















///////////////////////  Background routine version ///////////////////
/// TODO: We have a well integrated workaround here, which will ///////
/// interprocess with GsApp and fit into its cycle ////////////////////
/// Goal is to be remove the original resourceloader //////////////////
///////////////////////////////////////////////////////////////////////


CResourceLoaderBackground::CResourceLoaderBackground() :
m_permil(0),
m_permiltarget(0),
m_min_permil(0),
m_max_permil(1000),
mRunning(false),
m_style(PROGRESS_STYLE_TEXT)
{
    SDL_Rect rect;
    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();
    rect.x = 0;		rect.y = 0;
    rect.w = gameRes.w;	rect.h = gameRes.h;

    mpProgressSfc.reset( CG_CreateRGBSurface( rect ), &SDL_FreeSurface );
    mpProgressSfc.reset( gVideoDriver.convertThroughBlitSfc(mpProgressSfc.get()), &SDL_FreeSurface );
}


/**
 * This will start up the thread for the load display and process the display of loading
 * and then return
 */
void CResourceLoaderBackground::RunLoadActionBackground(Action* act,
                                   const int min_permil,
                                   const int max_permil)
{
    mpAction = act;
    mRunning = true;
    m_max_permil = max_permil;
    m_min_permil = min_permil;
    m_permil = m_min_permil;
    m_permiltarget = m_min_permil;
}

void CResourceLoaderBackground::start()
{
    mpThread = threadPool->start(mpAction, "Loading Resources");
}



/**
 * Set the percentage of progress
 */
void CResourceLoaderBackground::setPermilageForce(const int permil)
{
    if(permil<m_max_permil && permil>m_min_permil)
        m_permil = permil;
    else
        m_permil = m_max_permil;
}



void CResourceLoaderBackground::setPermilage(const int permil)
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





void CResourceLoaderBackground::run(const float deltaT)
{
    int ret;
    if(threadPool->finalizeIfReady(mpThread, &ret))
    {
        mRunning = false;
        mpThread = nullptr;
    }

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
}



/**
 * Progress the shown graphic here
 */
void CResourceLoaderBackground::render()
{
    SDL_Surface *sfc = mpProgressSfc.get();
    SDL_FillRect(sfc, nullptr, 0x0);

    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();

    const int gameWidth = gameRes.w;
    const int gameHeight = gameRes.h;
    const int halfWidth = gameWidth/2;

    if(m_style == PROGRESS_STYLE_TEXT)
    {
        // Draw Loading Font... here!
        GsFont &Font = gGraphics.getFont(0);
        int percent = m_permil/10;
        int rest = m_permil%10;
        std::string text = "Loading ... " + itoa(percent)+"."+ itoa(rest)+" \%";

        SDL_Rect textRect;
        GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();
        const float scaleUpW = float(gameRes.w)/320.0f;
        const float scaleUpH = float(gameRes.h)/200.0f;
        textRect.x = (int)(80.0*scaleUpW);        textRect.y = (int)(100.0*scaleUpH);
        textRect.w = 200;        textRect.h = 10;
        SDL_FillRect(sfc, &textRect, SDL_MapRGB(sfc->format, 0, 0, 0));
        Font.drawFont(sfc, text , textRect.x, textRect.y, true);
    }
    else if(m_style == PROGRESS_STYLE_BITMAP)
    {
        GsBitmap &Bitmap = *gGraphics.getBitmapFromStr("ONEMOMEN");
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

    // In there is garbage of other drawn stuff clean it up.
    auto blit = gVideoDriver.getBlitSurface();

    SDL_FillRect( blit, nullptr, SDL_MapRGB(blit->format, 0,0,0) );

    BlitSurface( mpProgressSfc.get(), nullptr, blit, nullptr );
}



