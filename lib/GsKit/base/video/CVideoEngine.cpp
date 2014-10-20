/*
 * CVideoEngine.cpp
 *
 *  Created on: 05.02.2011
 *      Author: gerhard
 */

#include <base/GsTimer.h>
#include "CVideoEngine.h"
#include <base/GsLogging.h>
#include <SDL_syswm.h>
#include <base/video/scaler/scalebit.h>
#include <SDL_version.h>



Uint16 getPowerOfTwo(const Uint16 value)
{
    Uint16 output = 1;
    while (output<value)
        output <<= 1;
    return output;
}

CVideoEngine::CVideoEngine(const CVidConfig& VidConfig) :
#if SDL_VERSION_ATLEAST(2, 0, 0)
    window(nullptr),
    renderer(nullptr),
#endif
mpScreenSfc(nullptr),
ScrollSurface(nullptr),       // Squared scroll buffer
m_VidConfig(VidConfig),
mSbufferx(0),
mSbuffery(0),
m_Mode(0)
{}


CVideoEngine::~CVideoEngine()
{
#if SDL_VERSION_ATLEAST(2, 0, 0)

    if(renderer)
    {
        SDL_DestroyRenderer(renderer);
    }

    if(window)
    {
        SDL_DestroyWindow(window);
    }

#endif

}


bool CVideoEngine::init()
{
	// Setup mode depends on some systems.
#if defined(CAANOO) || defined(WIZ) || defined(DINGOO) || defined(NANONOTE) || defined(ANDROID) || defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
	m_Mode = SDL_SWSURFACE;
#elif defined(GP2X)
	m_Mode = SDL_HWSURFACE;
#else
	// Support for double-buffering
    #if SDL_VERSION_ATLEAST(2, 0, 0)
    #else
        m_Mode = SDL_HWPALETTE;
    #endif
#endif

	// Enable OpenGL
#ifdef USE_OPENGL
	if(m_VidConfig.m_opengl)
	{
		if(m_VidConfig.vsync)
		{
			SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	#if SDL_VERSION_ATLEAST(2, 0, 0)
	#else
		SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
	#endif
		}

#if SDL_VERSION_ATLEAST(2, 0, 0)

#else
        m_Mode |= SDL_OPENGL;
#endif
	}
	else
#endif
	{
		if(m_VidConfig.vsync)
		{
#if SDL_VERSION_ATLEAST(2, 0, 0)

#else
            m_Mode |= (SDL_DOUBLEBUF | SDL_HWSURFACE);
#endif
		}
	}

	// Now we decide if it will be fullscreen or windowed mode.
#if SDL_VERSION_ATLEAST(2, 0, 0)

#else
    if(m_VidConfig.Fullscreen)
		m_Mode |= SDL_FULLSCREEN;
	else
		m_Mode |= SDL_RESIZABLE;
#endif

	// And set the proper Display Dimensions
	// The screen is also setup in this function
	if( !resizeDisplayScreen(m_VidConfig.m_DisplayRect) )
	{
		return false;
	}

	#ifdef _WIN32 // So far this only works under windows
    else
    {
        SDL_SysWMinfo info;
        SDL_VERSION(&info.version);

    #if SDL_VERSION_ATLEAST(2, 0, 0)
        int ok = SDL_GetWindowWMInfo(window,&info);
    #else
        int ok = SDL_GetWMInfo(&info);
    #endif

        if( ok )
        {
            if(ok > 0)
            {
                #if SDL_VERSION_ATLEAST(2, 0, 0)
                    SDL_ShowWindow(window);
                #else
                    ShowWindow(info.window, SW_SHOWNORMAL);
                #endif
            }
        }

    }
	#endif

	return true;
}

void CVideoEngine::updateAspectRect(const GsRect<Uint16>& displayRes, const int aspWidth, const int aspHeight)
{
    if (aspWidth == 0 || aspHeight == 0)
	{
        mAspectCorrectionRect.x = mAspectCorrectionRect.y = 0;
        mAspectCorrectionRect.w = displayRes.w;
        mAspectCorrectionRect.h = displayRes.h;
		return;
    }

    if (aspHeight*displayRes.w >= aspWidth*displayRes.h) // Wider than width:height, so shrink width
	{
        mAspectCorrectionRect.h = displayRes.h;
        mAspectCorrectionRect.w = (displayRes.h*aspWidth)/aspHeight;
    }
    else // Taller than width:height so shrink height
	{
        mAspectCorrectionRect.w = displayRes.w;
        mAspectCorrectionRect.h = (displayRes.w*aspHeight)/aspWidth;
    }
    mAspectCorrectionRect.x = (displayRes.w-mAspectCorrectionRect.w)/2;
    mAspectCorrectionRect.y = (displayRes.h-mAspectCorrectionRect.h)/2;
}



bool CVideoEngine::createSurfaces()
{
    return createSurfaces(m_VidConfig.m_GameRect);
}


bool CVideoEngine::createSurfaces(const GsRect<Uint16> &gamerect)
{
    gLogging.ftextOut("Blitsurface creation of %dx%d!\n<br>",
                     gamerect.w, gamerect.h );

    mGameSfc.create(m_Mode, gamerect.w, gamerect.h, RES_BPP,
                    0x00FF0000,
                    0x0000FF00,
                    0x000000FF,
                    0xFF000000);
#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetSurfaceBlendMode(mGameSfc.getSDLSurface(), SDL_BLENDMODE_NONE);
#endif



    const int squareSize = getPowerOfTwo( gamerect.h > gamerect.w ? gamerect.h : gamerect.w );

    gLogging.ftextOut("ScrollSurface creation of %dx%d!\n<br>",
                     squareSize, squareSize );

    // This function creates the surfaces which are needed for the game.
    ScrollSurface = SDL_CreateRGBSurface( m_Mode,
                                          squareSize,
                                          squareSize, 32,
                                          0x00FF0000,
                                          0x0000FF00,
                                          0x000000FF,
                                          0x00000000);

    auto blit = mGameSfc.getSDLSurface();

    gLogging.ftextOut("ScreenSurface creation of %dx%d!\n<br>",
                     blit->w, blit->h );


    if(m_VidConfig.m_ScaleXFilter > 1)
    {
        mFilteredSfc.create(m_Mode, blit->w*m_VidConfig.m_ScaleXFilter, blit->h*m_VidConfig.m_ScaleXFilter,
                            RES_BPP, 0, 0, 0, 0);

        mpScreenSfc = &mFilteredSfc;
    }
    else
    {
        mpScreenSfc = &mGameSfc;
    }

    initOverlaySurface(blit->w, blit->h);       

#if SDL_VERSION_ATLEAST(2, 0, 0)
    mpSdlTexture.reset( SDL_CreateTexture(renderer,
                                   SDL_PIXELFORMAT_ARGB8888,
                                   SDL_TEXTUREACCESS_STREAMING,
                                   gamerect.w*m_VidConfig.m_ScaleXFilter,
                                   gamerect.h*m_VidConfig.m_ScaleXFilter) );
#endif


    return true;
}


void CVideoEngine::blitScrollSurface() // This is only for tiles
{									   // The name should be changed
	SDL_Rect srGsRect, dstrect;
    int sbufferx, sbuffery;
    auto blitSurface = mGameSfc.getSDLSurface();
    SDL_Rect Gamerect = m_VidConfig.m_GameRect.SDLRect();

    const int scrollSfcWidth = ScrollSurface->w;
    const int scrollSfcHeight = ScrollSurface->h;

    Gamerect.x = mRelativeVisGameArea.x;
    Gamerect.y = mRelativeVisGameArea.y;
    Gamerect.w = mRelativeVisGameArea.w+16;
    Gamerect.h = mRelativeVisGameArea.h+16;

    dstrect.x = Gamerect.x;
    dstrect.y = Gamerect.y;
    sbufferx = mSbufferx + dstrect.x;
    sbuffery = mSbuffery + dstrect.y;

    // Clip the scrollbuffer correctly
    if(sbufferx > scrollSfcWidth)
        sbufferx -= scrollSfcWidth;
    if(sbuffery > scrollSfcHeight)
        sbuffery -= scrollSfcHeight;


    SDL_FillRect(blitSurface, nullptr, SDL_MapRGB(blitSurface->format, 0, 0, 0));

    srGsRect.x =	sbufferx;
    srGsRect.y = sbuffery;

    const bool wraphoz = (sbufferx+Gamerect.w > scrollSfcWidth);
    const bool wrapvrt = (sbuffery+Gamerect.h > scrollSfcHeight);

    // Upper-Left Part to draw from the Scrollbuffer
    srGsRect.w = wraphoz ? (scrollSfcWidth-sbufferx) : Gamerect.w;
    srGsRect.h = wrapvrt ? (scrollSfcHeight-sbuffery) : Gamerect.h;

    BlitSurface(ScrollSurface, &srGsRect, blitSurface, &dstrect);

    const Uint16 upperLeftW = srGsRect.w;
    const Uint16 upperLeftH =  srGsRect.h;

    // upper-right part
    if (wraphoz)
	{
        srGsRect.w = Gamerect.w - upperLeftW;
        srGsRect.x = 0;
        dstrect.x = Gamerect.x + upperLeftW;

        BlitSurface(ScrollSurface, &srGsRect, blitSurface, &dstrect);
    }

    // lower-right part
    if (wrapvrt)
	{
        srGsRect.h = Gamerect.h - upperLeftH;
        srGsRect.y = 0;
        dstrect.y = Gamerect.y + upperLeftH;

        BlitSurface(ScrollSurface, &srGsRect, blitSurface, &dstrect);
	}

    if(!wraphoz || !wrapvrt)
        return;

    // and lower-left part
    srGsRect.x = sbufferx;
    srGsRect.y = 0;
    srGsRect.w = upperLeftW;

    dstrect.x = Gamerect.x;
    dstrect.y = Gamerect.y+upperLeftH;

    BlitSurface(ScrollSurface, &srGsRect, blitSurface, &dstrect);
}

/*void CVideoEngine::stop()
{
	gLogging.textOut(GREEN, "Freeing the following graphical surfaces:<br>\n");

    if( BlitSurface )
    {
        SDL_FreeSurface(BlitSurface);
        gLogging.textOut("freed BlitSurface<br>");
        BlitSurface=NULL;
    }

    if( FilteredSurface )
    {
        SDL_FreeSurface(FilteredSurface);
        gLogging.textOut("freed FilteredSurface<br>");
        FilteredSurface = NULL;
    }

    if( ScrollSurface && (ScrollSurface->map != NULL) )
    {
        SDL_FreeSurface(ScrollSurface);
        gLogging.textOut("freed ScrollSurface<br>");
        ScrollSurface = NULL;
    }
}*/


void CVideoEngine::filterUp()
{
    // If ScaleX is enabled scaleup to screensurface
    // Otherwise screensurface point to gameSfc and nothing needs to be done
    if(m_VidConfig.m_ScaleXFilter > 1)
    {
        auto srcSfc = mGameSfc.getSDLSurface();
        auto dstSfc = mFilteredSfc.getSDLSurface();

        SDL_LockSurface( srcSfc );
        SDL_LockSurface( dstSfc );

        scale(m_VidConfig.m_ScaleXFilter,
              dstSfc->pixels,
              dstSfc->pitch,
              srcSfc->pixels,
              srcSfc->pitch,
              dstSfc->format->BytesPerPixel,
              srcSfc->w,
              srcSfc->h	);

        SDL_UnlockSurface( dstSfc );
        SDL_UnlockSurface( srcSfc );
    }

}

void CVideoEngine::shutdown()
{
    //stop();
}
