/*
 * CSDLVideo.cpp
 *
 *  Created on: 05.02.2011
 *      Author: gerstrong
 */

#include "CSDLVideo.h"
#include "CVideoEngine.h"

CSDLVideo::CSDLVideo(const CVidConfig& VidConfig) :
CVideoEngine(VidConfig)
{
	// TODO Auto-generated constructor stub

}

void CSDLVideo::updateScreen()
{
	// if we're doing zoom then we have copied the scroll buffer into
	// another offscreen buffer, and must now stretchblit it to the screen
	if (m_VidConfig.Zoom == 1 && m_VidConfig.m_Resolution.width != game_resolution_rect.w )
	{
		//LockSurface(BlitSurface);
		//LockSurface(screen);

		if(m_VidConfig.m_ScaleXFilter == 1)
		{
			SDL_Rect scrrect, dstrect;
			scrrect.y = 0;
			scrrect.x = 0;
			scrrect.h = game_resolution_rect.h;
			scrrect.w = game_resolution_rect.w;
			dstrect.x = (m_VidConfig.m_Resolution.width-game_resolution_rect.w)/2;
			dstrect.y = (m_VidConfig.m_Resolution.height-game_resolution_rect.h)/2;
			dstrect.w = game_resolution_rect.w;
			dstrect.h = game_resolution_rect.h;

			SDL_BlitSurface(BlitSurface, &scrrect, screen, &dstrect);
		}
		else
		{
			g_pLogFile->textOut(PURPLE,"Sorry, but this filter doesn't work at that zoom mode<br>");
			g_pLogFile->textOut(PURPLE,"Try to use a higher zoom factor. Switching to no-filter<br>");
			m_VidConfig.m_ScaleXFilter = 1;
		}
		//UnlockSurface(screen);
		//UnlockSurface(BlitSurface);
	}
	if (m_VidConfig.Zoom == 2)
	{
		LockSurface(BlitSurface);
		LockSurface(screen);

		if(m_VidConfig.m_ScaleXFilter == 1)
		{
			scale2xnofilter((char*)VRAMPtr, (char*)BlitSurface->pixels, screen->format->BytesPerPixel);
		}
		else if(m_VidConfig.m_ScaleXFilter == 2)
		{
			scale(m_VidConfig.m_ScaleXFilter, VRAMPtr, m_dst_slice, BlitSurface->pixels,
					m_src_slice, screen->format->BytesPerPixel,
					game_resolution_rect.w, game_resolution_rect.h);
		}
		else
		{
			g_pLogFile->textOut(PURPLE,"Sorry, but this filter doesn't work at that zoom mode<br>");
			g_pLogFile->textOut(PURPLE,"Try to use a higher zoom factor. Switching to no-filter<br>");
			m_VidConfig.m_ScaleXFilter = 1;
		}

		UnlockSurface(screen);
		UnlockSurface(BlitSurface);
	}
	else if (m_VidConfig.Zoom == 3)
	{
		LockSurface(BlitSurface);
		LockSurface(screen);

		if(m_VidConfig.m_ScaleXFilter == 1)
		{
			scale3xnofilter((char*)VRAMPtr, (char*)BlitSurface->pixels, screen->format->BytesPerPixel);
		}
		else if(m_VidConfig.m_ScaleXFilter == 2 || m_VidConfig.m_ScaleXFilter == 3)
		{
			scale(m_VidConfig.m_ScaleXFilter, VRAMPtr, m_dst_slice, BlitSurface->pixels,
					m_src_slice, screen->format->BytesPerPixel,
					game_resolution_rect.w, game_resolution_rect.h);
		}
		else
		{
			g_pLogFile->textOut(PURPLE,"Sorry, but this filter doesn't work at that zoom mode<br>");
			g_pLogFile->textOut(PURPLE,"Try to use a higher zoom factor. Switching to no-filter<br>");
			m_VidConfig.m_ScaleXFilter = 1;
		}
		UnlockSurface(screen);
		UnlockSurface(BlitSurface);
	}
	else if (m_VidConfig.Zoom == 4)
	{
		LockSurface(BlitSurface);
		LockSurface(screen);

		if(m_VidConfig.m_ScaleXFilter == 1)
		{
			scale4xnofilter((char*)VRAMPtr, (char*)BlitSurface->pixels, screen->format->BytesPerPixel);
		}
		else if(m_VidConfig.m_ScaleXFilter >= 2 && m_VidConfig.m_ScaleXFilter <= 4 )
		{
			scale(m_VidConfig.m_ScaleXFilter, VRAMPtr, m_dst_slice, BlitSurface->pixels,
					m_src_slice, screen->format->BytesPerPixel,
					game_resolution_rect.w, game_resolution_rect.h);
		}
		else
		{
			g_pLogFile->textOut(PURPLE,"Sorry, but this filter doesn't work at that zoom mode<br>");
			g_pLogFile->textOut(PURPLE,"Try to use a higher zoom factor. Switching to no-filter<br>");
			m_VidConfig.m_ScaleXFilter = 1;
		}
		UnlockSurface(screen);
		UnlockSurface(BlitSurface);
	}

	SDL_Flip(screen);

	// Flush the FG-Layer
	SDL_FillRect(FGLayerSurface, NULL, SDL_MapRGB(FGLayerSurface->format, 0, 0xFF, 0xFE));
}
