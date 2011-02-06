/*
 * CVideoEngine.cpp
 *
 *  Created on: 05.02.2011
 *      Author: gerhard
 */

#include "CVideoEngine.h"
#include "CLogFile.h"

CVideoEngine::CVideoEngine(const CVidConfig& VidConfig) :
BlitSurface(NULL),
FGLayerSurface(NULL),       // Scroll buffer for Messages
ScrollSurface(NULL),       // 512x512 scroll buffer
FXSurface(NULL),
m_blitsurface_alloc(false),
m_VidConfig(VidConfig)
{}

/*CVideoEngine::~CVideoEngine() {
	// TODO Auto-generated destructor stub
}*/

SDL_Surface* CVideoEngine::createSurface( std::string name, bool alpha, int width, int height, int bpp, int mode, SDL_PixelFormat* format )
{
	SDL_Surface *temporary, *optimized;

	temporary = SDL_CreateRGBSurface( mode, width, height, bpp, format->Rmask, format->Gmask, format->Bmask, format->Amask);
	if (alpha && bpp==32) {
		optimized = SDL_DisplayFormatAlpha( temporary );
	} else {
		optimized = SDL_DisplayFormat( temporary );
	}
	SDL_FreeSurface(temporary);
	if (!optimized)
	{
		g_pLogFile->textOut(RED,"VideoDriver: Couldn't create surface:" + name +"<br>");
		return NULL;
	}
	bpp = optimized->format->BitsPerPixel;
	return optimized;
}


bool CVideoEngine::createSurfaces()
{
	// This function creates the surfaces which are needed for the game.
    ScrollSurface = createSurface( "ScrollSurface", true,
								  512,
								  512,
								  m_VidConfig.m_Resolution.depth,
								  m_VidConfig.Mode, screen->format );

    if (m_VidConfig.m_Resolution.width == game_resolution_rect.w && !m_VidConfig.m_opengl)
    {
    	g_pLogFile->textOut("Blitsurface = Screen<br>");
    	BlitSurface = screen;
    	m_blitsurface_alloc = false;
    }
    else
    {

    	g_pLogFile->textOut("Blitsurface = creatergbsurfacefrom<br>");

#ifdef USE_OPENGL
    	if(m_VidConfig.m_opengl)
    	{
            BlitSurface = createSurface( "BlitSurface", true,
            				getPowerOfTwo(game_resolution_rect.w),
            				getPowerOfTwo(game_resolution_rect.h),
            						m_VidConfig.m_Resolution.depth,
            						m_VidConfig.Mode, screen->format );
    	}
    	else
#endif
    	{
    		BlitSurface = createSurface( "BlitSurface", true,
									game_resolution_rect.w,
									game_resolution_rect.h,
									m_VidConfig.m_Resolution.depth,
									m_VidConfig.Mode, screen->format );
    	}
        m_blitsurface_alloc = true;
    }
    VRAMPtr = (unsigned char*)screen->pixels +
	screenrect.y*screen->pitch + (screenrect.x*m_VidConfig.m_Resolution.depth>>3);

    // Some surfaces could get 320x240 and the screenspace is extended.
    // The video class must be changed for any further resolutions
    game_resolution_rect.x = 0; game_resolution_rect.y = 0;

#ifdef USE_OPENGL
	if(m_VidConfig.m_opengl && m_VidConfig.m_ScaleXFilter == 1)
	{
		FGLayerSurface = createSurface( "FGLayerSurface", true,
						getPowerOfTwo(game_resolution_rect.w),
						getPowerOfTwo(game_resolution_rect.h),
									m_VidConfig.m_Resolution.depth,
									m_VidConfig.Mode, screen->format );
	}
	else
#endif
	{
		FGLayerSurface = createSurface( "FGLayerSurface", false,
								   game_resolution_rect.w,
								   game_resolution_rect.h,
								   m_VidConfig.m_Resolution.depth,
								   m_VidConfig.Mode, screen->format );

		SDL_SetColorKey( FGLayerSurface, SDL_SRCCOLORKEY,
						SDL_MapRGB(FGLayerSurface->format, 0, 0xFF, 0xFE) );

	}

	//Set surface alpha
	SDL_SetAlpha( FGLayerSurface, SDL_SRCALPHA, 225 );

#ifdef USE_OPENGL
	if(m_VidConfig.m_opengl && m_VidConfig.m_ScaleXFilter == 1)
	{
		FXSurface = createSurface( "FXSurface", true,
				getPowerOfTwo(game_resolution_rect.w),
				getPowerOfTwo(game_resolution_rect.h),
						m_VidConfig.m_Resolution.depth,
						m_VidConfig.Mode, screen->format );
	}
	else
#endif
	{
	    FXSurface = createSurface( "FXSurface", false,
	    						game_resolution_rect.w,
	    						game_resolution_rect.h,
	    						m_VidConfig.m_Resolution.depth,
								m_VidConfig.Mode, screen->format );
	}

	g_pGfxEngine->Palette.setFXSurface( FXSurface );

	return true;
}


void CVideoEngine::collectSurfaces()
{
#ifdef USE_OPENGL
	if(m_VidConfig.m_opengl && m_VidConfig.m_ScaleXFilter == 1)
	{
		// TODO: Create a solid concept for rendering more textures instead of just one that is binded
		// to BlitSurface. It's not that easy, because doing that and using scaleX will mean, that you
		// to scaleX multiple times. So COpenGL must have separate cases. With or without ScaleX.
		// It's might only be faster if scaleX is never used in that TODO-case

		mp_OpenGL->setFGSurface(FGLayerSurface);

		if(getPerSurfaceAlpha(FXSurface))
			mp_OpenGL->setFXSurface(FXSurface);
		else
			mp_OpenGL->setFXSurface(NULL);
	}
	else
#endif
	{
		SDL_BlitSurface(FGLayerSurface, NULL, BlitSurface, NULL);

		if(getPerSurfaceAlpha(FXSurface))
			SDL_BlitSurface(FXSurface, NULL, BlitSurface, NULL);
	}
}

void CVideoEngine::blitScrollSurface() // This is only for tiles
									   // The name should be changed
{
	SDL_Rect srcrect;
	SDL_Rect dstrect;
	Sint16 sbufferx, sbuffery;
	char wraphoz, wrapvrt;
	int save_dstx, save_dstw, save_srcx, save_srcw;

	dstrect.x = 0; dstrect.y = 0;

	srcrect.x =	sbufferx = *mp_sbufferx;
	srcrect.y = sbuffery = *mp_sbuffery;

	dstrect.w = game_resolution_rect.w-sbufferx;
	dstrect.h = game_resolution_rect.h-sbuffery;

	if (sbufferx > (Uint16)(512-game_resolution_rect.w))
	{ // need to wrap right side
		srcrect.w = (512-sbufferx);
		wraphoz = 1;
	}
	else
	{ // single blit for whole horizontal copy
		srcrect.w = game_resolution_rect.w;
		wraphoz = 0;
	}

	if (sbuffery > (Uint16)(512-game_resolution_rect.h))
	{ // need to wrap on bottom
		srcrect.h = (512-sbuffery);
		wrapvrt = 1;
	}
	else
	{ // single blit for whole bottom copy
		srcrect.h = game_resolution_rect.h;
		wrapvrt = 0;
	}

	SDL_BlitSurface(ScrollSurface, &srcrect, BlitSurface, &dstrect);

	if (wraphoz && wrapvrt)
	{
		// first do same thing we do for wraphoz
		save_dstx = dstrect.x;
		save_dstw = dstrect.w;
		save_srcx = srcrect.x;
		save_srcw = srcrect.w;
		dstrect.x = srcrect.w;
		dstrect.w = game_resolution_rect.w - dstrect.x;
		srcrect.x = 0;
		srcrect.w = (game_resolution_rect.w - srcrect.w);
		SDL_BlitSurface(ScrollSurface, &srcrect, BlitSurface, &dstrect);

		// now repeat for the bottom
		// (lower-right square)
		dstrect.y = srcrect.h;
		dstrect.h = game_resolution_rect.h - dstrect.y;
		srcrect.y = 0;
		srcrect.h = (game_resolution_rect.h - srcrect.h);
		SDL_BlitSurface(ScrollSurface, &srcrect, BlitSurface, &dstrect);
		// (lower-left square)
		dstrect.x = save_dstx;
		dstrect.w = save_dstw;
		srcrect.x = save_srcx;
		srcrect.w = save_srcw;
		SDL_BlitSurface(ScrollSurface, &srcrect, BlitSurface, &dstrect);
	}
	else if (wraphoz)
	{
		dstrect.x = srcrect.w;
		dstrect.w = game_resolution_rect.w - dstrect.x;
		srcrect.x = 0;
		srcrect.w = game_resolution_rect.w - srcrect.w;
		SDL_BlitSurface(ScrollSurface, &srcrect, BlitSurface, &dstrect);
	}
	else if (wrapvrt)
	{
		dstrect.y = srcrect.h;
		dstrect.h = game_resolution_rect.h - dstrect.y;
		srcrect.y = 0;
		srcrect.h = (game_resolution_rect.h - srcrect.h);
		SDL_BlitSurface(ScrollSurface, &srcrect, BlitSurface, &dstrect);
	}

	drawConsoleMessages();
}


void CVideoEngine::stop(void)
{
	g_pLogFile->textOut(GREEN, "Freeing the following graphical surfaces:<br>\n");
    if(m_blitsurface_alloc && BlitSurface)
    {
        SDL_FreeSurface(BlitSurface);
        g_pLogFile->textOut("freed BlitSurface<br>");
        BlitSurface=NULL;
    }
    if(FGLayerSurface)
    {
        SDL_FreeSurface(FGLayerSurface);
        g_pLogFile->textOut("freed FGLayerSurface<br>");
        FGLayerSurface=NULL;
    }
    if(ScrollSurface && (ScrollSurface->map != NULL))
    {
        SDL_FreeSurface(ScrollSurface);
        g_pLogFile->textOut("freed ScrollSurface<br>");
        ScrollSurface=NULL;
    }
    if(FXSurface)
    {
        SDL_FreeSurface(FXSurface);
        g_pLogFile->textOut("freed FXSurface<br>");
        FXSurface=NULL;
    }
}
