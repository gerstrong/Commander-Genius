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
    window = SDL_CreateWindow("Commander Genius",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              m_VidConfig.m_DisplayRect.w,
                              m_VidConfig.m_DisplayRect.h,
                              /*SDL_WINDOW_BORDERLESS|SDL_WINDOW_OPENGL|*/SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, 0);
     
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    
    aspectCorrectResizing(newDim, w, h);

    const CRect<Uint16> &gamerect = m_VidConfig.m_GameRect;

    SDL_RenderSetLogicalSize(renderer, gamerect.w, gamerect.h);


    if(FilteredSurface)
    {
        sdlTexture = SDL_CreateTexture(renderer,
                                       SDL_PIXELFORMAT_ARGB8888,
                                       SDL_TEXTUREACCESS_STREAMING,
                                       gamerect.w*m_VidConfig.m_ScaleXFilter,
                                       gamerect.h*m_VidConfig.m_ScaleXFilter);
    }
    else
    {
        sdlTexture = SDL_CreateTexture(renderer,
                                       SDL_PIXELFORMAT_ARGB8888,
                                       SDL_TEXTUREACCESS_STREAMING,
                                       gamerect.w,
                                       gamerect.h);
    }

#else
    screen = SDL_SetVideoMode( newDim.w, newDim.h, 32, m_Mode );

	if (!screen)
	{
		g_pLogFile->textOut(RED,"VidDrv_Start(): Couldn't create a SDL surface: %s<br>", SDL_GetError());
		return false;
	}

	aspectCorrectResizing(newDim, w, h);

#endif

    if(FilteredSurface)
    {
        Scaler.setDynamicFactor( float(FilteredSurface->w)/float(aspectCorrectionRect.w),
                                 float(FilteredSurface->h)/float(aspectCorrectionRect.h));
    }

	return true;
}


bool CSDLVideo::initOverlaySurface( const bool useAlpha,
                                       const Uint16 width,
                                       const Uint16 height )
{

    SDL_Surface *overlay = createSurface( "OverlaySurface",
                                         useAlpha,
                                         width,
                                         height,
                                         RES_BPP,
                                         m_Mode );

    mpOverlaySurface.reset( overlay );


#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetSurfaceAlphaMod( overlay, 0);
#else

    SDL_SetAlpha( overlay, SDL_SRCALPHA, 0);
#endif

    if(!mpOverlaySurface)
        return false;

    return true;
}


void CSDLVideo::setLightIntensity(const float intensity)
{
    Uint8 intense = Uint8(intensity*255.0f);

    auto *sfc = mpOverlaySurface.get();
    Uint32 color = SDL_MapRGB(sfc->format, 0, 0, 0);

    #if SDL_VERSION_ATLEAST(2, 0, 0)
        //SDL_SetAlpha( sfc, SDL_SRCALPHA, 255-intense); //TODO: Make it working...
    #else
        SDL_SetAlpha( sfc, SDL_SRCALPHA, 255-intense);
    #endif

    SDL_FillRect( sfc, nullptr, color);
}

bool CSDLVideo::createSurfaces()
{
    // Configure the Scaler
    Scaler.setFilterFactor(m_VidConfig.m_ScaleXFilter);
    Scaler.setFilterType(m_VidConfig.m_normal_scale);

    const CRect<Uint16> &gamerect = m_VidConfig.m_GameRect;

    g_pLogFile->textOut("Blitsurface creation!\n");


    BlitSurface = SDL_CreateRGBSurface( m_Mode, gamerect.w, gamerect.h, RES_BPP,
                                          0x00FF0000,
                                          0x0000FF00,
                                          0x000000FF,
                                          0xFF000000);

    SDL_SetSurfaceBlendMode(BlitSurface, SDL_BLENDMODE_NONE);


    // This function creates the surfaces which are needed for the game.    
    /*ScrollSurface = createSurface( "ScrollSurface", false,//true,
            512,
            512,
            RES_BPP,
            m_Mode );*/
    ScrollSurface = SDL_CreateRGBSurface( m_Mode, 512, 512, 32,
                                          0x00FF0000,
                                          0x0000FF00,
                                          0x000000FF,
                                          0x00000000);
    /*SDL_SetPaletteColors(ScrollSurface->format->palette, g_pGfxEngine->Palette.m_Palette, 0, 255);
    SDL_SetColorKey(ScrollSurface, SDL_TRUE, COLORKEY);*/



    g_pLogFile->textOut("FilteredSurface creation!\n");

    auto *format = BlitSurface->format;

    FilteredSurface = SDL_CreateRGBSurface( m_Mode,
                                BlitSurface->w*m_VidConfig.m_ScaleXFilter,
                                BlitSurface->h*m_VidConfig.m_ScaleXFilter,
                                RES_BPP,
                                format->Rmask,
                                format->Gmask,
                                format->Bmask,
                                format->Amask );


    m_dst_slice = FilteredSurface->pitch;

    initOverlaySurface(false, BlitSurface->w, BlitSurface->h);

    Scaler.setFilterFactor(m_VidConfig.m_ScaleXFilter);
    Scaler.setFilterType(m_VidConfig.m_normal_scale);
    Scaler.setDynamicFactor( float(FilteredSurface->w)/float(aspectCorrectionRect.w),
                             float(FilteredSurface->h)/float(aspectCorrectionRect.h));

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

        #if SDL_VERSION_ATLEAST(2, 0, 0)
            SDL_LockSurface(BlitSurface);
            SDL_UpdateTexture(sdlTexture, nullptr, BlitSurface->pixels, BlitSurface->w * sizeof (Uint32));
            SDL_UnlockSurface(BlitSurface);
        #else
            SDL_BlitSurface(BlitSurface, &scrrect, screen, &dstrect);
        #endif
	}
	else
	{
		// First apply the conventional filter if any (GameScreen -> FilteredScreen)
        Scaler.scaleUp(FilteredSurface, BlitSurface, SCALEX, aspectCorrectionRect);

        #if SDL_VERSION_ATLEAST(2, 0, 0)
            SDL_LockSurface(FilteredSurface);
            SDL_UpdateTexture(sdlTexture, nullptr, FilteredSurface->pixels, FilteredSurface->w * sizeof (Uint32));
            SDL_UnlockSurface(FilteredSurface);
        #else
            // Now scale up to the new DisplayRect (FilteredScreen -> screen)
            Scaler.scaleUp(screen, FilteredSurface, DYNAMIC, aspectCorrectionRect);
        #endif
	}


#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
#else
	// Flip the screen (We use double-buffering on some systems.)
    SDL_Flip(screen);
#endif
}
