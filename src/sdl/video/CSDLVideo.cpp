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
	
#if SDL_VERSION_ATLEAST(2, 0, 0)
    
#else
    screen = SDL_SetVideoMode( newDim.w, newDim.h, 32, m_Mode );
#endif

	if (!screen)
	{
		g_pLogFile->textOut(RED,"VidDrv_Start(): Couldn't create a SDL surface: %s<br>", SDL_GetError());
		return false;
	}

	aspectCorrectResizing(newDim);

	if(FilteredSurface)
	{
		Scaler.setDynamicFactor( float(FilteredSurface->w)/float(aspectCorrectionRect.w),
								 float(FilteredSurface->h)/float(aspectCorrectionRect.h));
	}

	return true;
}

bool CSDLVideo::createSurfaces()
{
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


	FXSurface = createSurface( "FXSurface", false,
			BlitSurface->w,
			BlitSurface->h,
			RES_BPP,
			m_Mode, screen->format );

	//Set surface alpha
	g_pGfxEngine->Palette.setFXSurface( FXSurface );

	Scaler.setFilterFactor(m_VidConfig.m_ScaleXFilter);
	Scaler.setFilterType(m_VidConfig.m_normal_scale);
	Scaler.setDynamicFactor( float(FilteredSurface->w)/float(aspectCorrectionRect.w),
							 float(FilteredSurface->h)/float(aspectCorrectionRect.h));

	return true;
}

void CSDLVideo::collectSurfaces()
{
	if( getPerSurfaceAlpha(FXSurface) )
		SDL_BlitSurface(FXSurface, NULL, BlitSurface, NULL);
}

void CSDLVideo::clearSurfaces()
{
	SDL_FillRect(FXSurface, NULL, 0x0);
	SDL_FillRect(BlitSurface, NULL, 0x0);
}



void CSDLVideo::updateScreen()
{
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
#if SDL_VERSION_ATLEAST(2, 0, 0)
    
#else
    SDL_Flip(screen);
#endif
}
