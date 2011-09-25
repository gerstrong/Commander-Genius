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

bool CSDLVideo::createSurfaces()
{
	// This function creates the surfaces which are needed for the game.
	const SDL_Rect &gamerect = m_VidConfig.m_Gamescreen;
	ScrollSurface = createSurface( "ScrollSurface", true,
			512,
			512,
			m_VidConfig.m_Resolution.depth,
			m_Mode, screen->format );

	if (m_VidConfig.m_Resolution.width == gamerect.w )
	{
		g_pLogFile->textOut("Blitsurface = Screen<br>");
		BlitSurface = screen;
		m_blitsurface_alloc = false;
	}
	else
	{
		g_pLogFile->textOut("Blitsurface = creatergbsurfacefrom<br>");

		BlitSurface = createSurface( "BlitSurface", true,
				gamerect.w,
				gamerect.h,
				m_VidConfig.m_Resolution.depth,
				m_Mode, screen->format );

		m_blitsurface_alloc = true;
	}

	FGLayerSurface = createSurface( "FGLayerSurface", false,
			gamerect.w,
			gamerect.h,
			m_VidConfig.m_Resolution.depth,
			m_Mode, screen->format );

	SDL_SetColorKey( FGLayerSurface, SDL_SRCCOLORKEY,
			SDL_MapRGB(FGLayerSurface->format, 0, 0xFF, 0xFE) );

	FXSurface = createSurface( "FXSurface", false,
			gamerect.w,
			gamerect.h,
			m_VidConfig.m_Resolution.depth,
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
	const SDL_Rect &gamerect = m_VidConfig.m_Gamescreen;
	const resolution_t &Resrect = m_VidConfig.m_Resolution;

	// pointer to the line in VRAM to start blitting to when stretchblitting.
	// this may not be the first line on the display as it is adjusted to
	// center the image on the screen when in fullscreen.
	Uint8 *ScreenPtr;
	Uint8 *BlitPtr;
	unsigned int width, height;

	// if we're doing zoom then we have copied the scroll buffer into
	// another offscreen buffer, and must now stretchblit it to the screen
	if (m_VidConfig.Zoom == 1 && Resrect.width != gamerect.w )
	{
		SDL_Rect scrrect, dstrect;
		scrrect.y = 0;
		scrrect.x = 0;
		dstrect.h = scrrect.h = gamerect.h;
		dstrect.w = scrrect.w = gamerect.w;
		dstrect.x = (m_VidConfig.m_Resolution.width-gamerect.w)/2;
		dstrect.y = (m_VidConfig.m_Resolution.height-gamerect.h)/2;

		SDL_BlitSurface(BlitSurface, &scrrect, screen, &dstrect);
	}
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
	}

	SDL_Flip(screen);

	// Flush the FG-Layer
	SDL_FillRect(FGLayerSurface, NULL, SDL_MapRGB(FGLayerSurface->format, 0, 0xFF, 0xFE));
}
