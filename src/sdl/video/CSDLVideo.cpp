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

CSDLVideo::CSDLVideo(const CVidConfig& VidConfig, Sint16 *&p_sbufferx, Sint16 *&p_sbuffery) :
CVideoEngine(VidConfig, p_sbufferx, p_sbuffery)
{}

bool CSDLVideo::resizeDisplayScreen(const CRect& newDim)
{
	// NOTE: try not to free the last SDL_Surface of the screen, this is freed automatically by SDL
	screen = SDL_SetVideoMode( newDim.w, newDim.h, 32, m_Mode );

	if (!screen)
	{
		g_pLogFile->textOut(RED,"VidDrv_Start(): Couldn't create a SDL surface: %s<br>", SDL_GetError());
		return false;
	}

	if(FilteredSurface)
	{
		Scaler.setDynamicFactor( float(FilteredSurface->w)/float(screen->w),
								 float(FilteredSurface->h)/float(screen->h));
	}

	return true;
}

bool CSDLVideo::createSurfaces()
{
	// Configure the Scaler
	Scaler.setFilterFactor(m_VidConfig.m_ScaleXFilter);

	// This function creates the surfaces which are needed for the game.
	const CRect &gamerect = m_VidConfig.m_GameRect;
	ScrollSurface = createSurface( "ScrollSurface", true,
			512,
			512,
			32,
			m_Mode, screen->format );

	g_pLogFile->textOut("Blitsurface created!\n");

	BlitSurface = createSurface( "BlitSurface", true,
				gamerect.w,
				gamerect.h,
				32,
				m_Mode, screen->format );

	g_pLogFile->textOut("FilteredSurface created!\n");

	FilteredSurface = createSurface( "FilteredSurface", true,
				BlitSurface->w*m_VidConfig.m_ScaleXFilter,
				BlitSurface->h*m_VidConfig.m_ScaleXFilter,
				32,
				m_Mode, screen->format );

	 m_dst_slice = FilteredSurface->w*screen->format->BytesPerPixel;


	FGLayerSurface = createSurface( "FGLayerSurface", false,
			BlitSurface->w,
			BlitSurface->h,
			32,
			m_Mode, screen->format );

	SDL_SetColorKey( FGLayerSurface, SDL_SRCCOLORKEY,
			SDL_MapRGB(FGLayerSurface->format, 0, 0xFF, 0xFE) );

	FXSurface = createSurface( "FXSurface", false,
			BlitSurface->w,
			BlitSurface->h,
			32,
			m_Mode, screen->format );

	//Set surface alpha
	SDL_SetAlpha( FGLayerSurface, SDL_SRCALPHA, 225 );
	g_pGfxEngine->Palette.setFXSurface( FXSurface );

	Scaler.setFilterFactor(m_VidConfig.m_ScaleXFilter);
	Scaler.setDynamicFactor( float(FilteredSurface->w)/float(screen->w),
							 float(FilteredSurface->h)/float(screen->h));

	return true;
}

void CSDLVideo::collectSurfaces()
{
	SDL_BlitSurface(FGLayerSurface, NULL, BlitSurface, NULL);

	if( getPerSurfaceAlpha(FXSurface) )
		SDL_BlitSurface(FXSurface, NULL, BlitSurface, NULL);
}

void CSDLVideo::clearSurfaces()
{
	SDL_FillRect(FGLayerSurface,NULL, 0x0);
	SDL_FillRect(FXSurface,NULL, 0x0);
	SDL_FillRect(BlitSurface,NULL, 0x0);
}



void CSDLVideo::updateScreen()
{
	const CRect &GameRect = m_VidConfig.m_GameRect;
	const CRect &DisplayRect = m_VidConfig.m_DisplayRect;


	// First apply the conventional filter if any (GameScreen -> FilteredScreen)
	Scaler.scaleUp(FilteredSurface, BlitSurface, SCALEX);


	// Now scale up to the new DisplayRect (FilteredScreen -> screen)
	Scaler.scaleUp(screen, FilteredSurface, DYNAMIC);


	// Flip the screen (We use double-buffering on some systems.)
	SDL_Flip(screen);


	// Flush the FG-Layer
	SDL_FillRect(FGLayerSurface, NULL, SDL_MapRGB(FGLayerSurface->format, 0, 0xFF, 0xFE));
}
