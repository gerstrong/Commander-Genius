/*
 * CSDLVideo.cpp
 *
 *  Created on: 05.02.2011
 *      Author: gerstrong
 */

#include "CSDLVideo.h"
#include "CVideoEngine.h"
#include "CLogFile.h"

#include "graphics/CGfxEngine.h"
#include "graphics/PerSurfaceAlpha.h"

CSDLVideo::CSDLVideo(const CVidConfig& VidConfig) :
CVideoEngine(VidConfig)
{}

bool CSDLVideo::resizeDisplayScreen(const CRect<Uint16>& newDim)
{
	// NOTE: try not to free the last SDL_Surface of the screen, this is freed automatically by SDL
	
    const int w = m_VidConfig.mAspectCorrection.w;
    const int h = m_VidConfig.mAspectCorrection.h;
  
  
#if SDL_VERSION_ATLEAST(2, 0, 0)
    window = SDL_CreateWindow("Commander Genius", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_VidConfig.m_DisplayRect.w, m_VidConfig.m_DisplayRect.h, SDL_WINDOW_BORDERLESS|SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, 0, 0);
     
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    
    aspectCorrectResizing(newDim, w, h);
#else
    screen = SDL_SetVideoMode( newDim.w, newDim.h, 32, m_Mode );

	if (!screen)
	{
		g_pLogFile->textOut(RED,"VidDrv_Start(): Couldn't create a SDL surface: %s<br>", SDL_GetError());
		return false;
	}

	aspectCorrectResizing(newDim, w, h);

	if(FilteredSurface)
	{
		Scaler.setDynamicFactor( float(FilteredSurface->w)/float(aspectCorrectionRect.w),
								 float(FilteredSurface->h)/float(aspectCorrectionRect.h));
	}
#endif

	return true;
}

bool CSDLVideo::createSurfaces()
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
    
#else
	// Configure the Scaler
	Scaler.setFilterFactor(m_VidConfig.m_ScaleXFilter);
	Scaler.setFilterType(m_VidConfig.m_normal_scale);

	// This function creates the surfaces which are needed for the game.
	const CRect<Uint16> &gamerect = m_VidConfig.m_GameRect;
	ScrollSurface = createSurface( "ScrollSurface", false,//true,
			512,
			512,
			RES_BPP,
			m_Mode, screen->format );

	g_pLogFile->textOut("Blitsurface creation!\n");

	BlitSurface = createSurface( "BlitSurface", true,
				gamerect.w,
				gamerect.h,
				RES_BPP,
				m_Mode, screen->format );

	g_pLogFile->textOut("FilteredSurface creation!\n");

	FilteredSurface = createSurface( "FilteredSurface", true,
				BlitSurface->w*m_VidConfig.m_ScaleXFilter,
				BlitSurface->h*m_VidConfig.m_ScaleXFilter,
				RES_BPP,
				m_Mode, screen->format );

	 m_dst_slice = FilteredSurface->w*screen->format->BytesPerPixel;

    initOverlaySurface(false, BlitSurface->w, BlitSurface->h);

	Scaler.setFilterFactor(m_VidConfig.m_ScaleXFilter);
	Scaler.setFilterType(m_VidConfig.m_normal_scale);
	Scaler.setDynamicFactor( float(FilteredSurface->w)/float(aspectCorrectionRect.w),
							 float(FilteredSurface->h)/float(aspectCorrectionRect.h));
#endif

	return true;
}

void CSDLVideo::collectSurfaces()
{
    SDL_Surface *overlay = mpOverlaySurface.get();

    if( getPerSurfaceAlpha(overlay) )
        SDL_BlitSurface(overlay, NULL, BlitSurface, NULL);
}

void CSDLVideo::clearSurfaces()
{
    SDL_FillRect(mpOverlaySurface.get(), NULL, 0x0);
	SDL_FillRect(BlitSurface, NULL, 0x0);
}



void CSDLVideo::updateScreen()
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_RenderPresent(renderer);
#else
	if( Scaler.filterFactor() <= 1 &&
			BlitSurface->h == aspectCorrectionRect.h &&
			BlitSurface->w == aspectCorrectionRect.w )
	{
		SDL_Rect scrrect, dstrect;
		scrrect.y = 0;
		scrrect.x = 0;
		dstrect.y = aspectCorrectionRect.y;
		dstrect.x = aspectCorrectionRect.x;
		dstrect.h = scrrect.h = BlitSurface->h;
		dstrect.w = scrrect.w = BlitSurface->w;

		SDL_BlitSurface(BlitSurface, &scrrect, screen, &dstrect);
	}
	else
	{
		// First apply the conventional filter if any (GameScreen -> FilteredScreen)
		Scaler.scaleUp(FilteredSurface, BlitSurface, SCALEX, aspectCorrectionRect);

		// Now scale up to the new DisplayRect (FilteredScreen -> screen)
		Scaler.scaleUp(screen, FilteredSurface, DYNAMIC, aspectCorrectionRect);
	}

	// Flip the screen (We use double-buffering on some systems.)
    SDL_Flip(screen);
#endif
}
