/*
 * CSDLVideo.cpp
 *
 *  Created on: 05.02.2011
 *      Author: gerstrong
 */

#include "CSDLVideo.h"
#include "CVideoEngine.h"
#include "CLogFile.h"

#include "scalers/scalers.h"
#include "graphics/CGfxEngine.h"
#include "graphics/PerSurfaceAlpha.h"

CSDLVideo::CSDLVideo(const CVidConfig& VidConfig, Sint16 *&p_sbufferx, Sint16 *&p_sbuffery) :
CVideoEngine(VidConfig, p_sbufferx, p_sbuffery)
{}

bool CSDLVideo::createSurfaces()
{
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
	scaleUp(FilteredSurface, BlitSurface, SCALEX);

	// Now scale up to the new DisplayRect (FilteredScreen -> screen)
	scaleUp(screen, FilteredSurface, DYNAMIC);






	// pointer to the line in VRAM to start blitting to when stretchblitting.
	// this may not be the first line on the display as it is adjusted to
	// center the image on the screen when in fullscreen.
	//Uint8 *ScreenPtr;
	//Uint8 *BlitPtr;
	//unsigned int width, height;

	// if we're doing zoom then we have copied the scroll buffer into
	// another offscreen buffer, and must now stretchblit it to the screen
	//if (m_VidConfig.Zoom == 1 && Resrect.width != gamerect.w )
	//{
	/*}
	else
	{
		fetchStartScreenPixelPtrs(ScreenPtr, BlitPtr, width, height);
	}

	if (m_VidConfig.Zoom == 2)
	{
		SDL_LockSurface(BlitSurface);
		SDL_LockSurface(screen);

		if(m_VidConfig.m_ScaleXFilter == 1)
		{
			scale2xnofilter((char*)ScreenPtr, (char*)BlitPtr, screen->format->BytesPerPixel);
		}
		else if(m_VidConfig.m_ScaleXFilter == 2)
		{
			scale(m_VidConfig.m_ScaleXFilter, ScreenPtr, m_dst_slice, BlitPtr,
					m_src_slice, screen->format->BytesPerPixel,
					width, height);
		}

		SDL_UnlockSurface(screen);
		SDL_UnlockSurface(BlitSurface);
	}
	else if (m_VidConfig.Zoom == 3)
	{
		SDL_LockSurface(BlitSurface);
		SDL_LockSurface(screen);

		if(m_VidConfig.m_ScaleXFilter == 1)
		{
			scale3xnofilter((char*)ScreenPtr, (char*)BlitPtr, screen->format->BytesPerPixel);
		}
		else if(m_VidConfig.m_ScaleXFilter == 2 || m_VidConfig.m_ScaleXFilter == 3)
		{
			scale(m_VidConfig.m_ScaleXFilter, ScreenPtr, m_dst_slice, BlitPtr,
					m_src_slice, screen->format->BytesPerPixel,
					width, height);
		}
		SDL_UnlockSurface(screen);
		SDL_UnlockSurface(BlitSurface);
	}
	else if (m_VidConfig.Zoom == 4)
	{
		SDL_LockSurface(BlitSurface);
		SDL_LockSurface(screen);

		if(m_VidConfig.m_ScaleXFilter == 1)
		{
			scale4xnofilter((char*)ScreenPtr, (char*)BlitPtr, screen->format->BytesPerPixel);
		}
		else if(m_VidConfig.m_ScaleXFilter >= 2 && m_VidConfig.m_ScaleXFilter <= 4 )
		{
			scale(m_VidConfig.m_ScaleXFilter, ScreenPtr, m_dst_slice, BlitPtr,
					m_src_slice, screen->format->BytesPerPixel,
					width, height);
		}
		SDL_UnlockSurface(screen);
		SDL_UnlockSurface(BlitSurface);
	}*/

	SDL_Flip(screen);

	// Flush the FG-Layer
	SDL_FillRect(FGLayerSurface, NULL, SDL_MapRGB(FGLayerSurface->format, 0, 0xFF, 0xFE));
}
