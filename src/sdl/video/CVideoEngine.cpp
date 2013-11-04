/*
 * CVideoEngine.cpp
 *
 *  Created on: 05.02.2011
 *      Author: gerhard
 */

#include "sdl/CTimer.h"
#include "CVideoEngine.h"
#include "CLogFile.h"
#include <SDL_syswm.h>
#include "scalers/scalebit.h"


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
    sdlTexture(nullptr),
#else
    mDisplaySfc(nullptr),
#endif
ScrollSurface(nullptr),       // Squared scroll buffer
m_VidConfig(VidConfig),
mSbufferx(0),
mSbuffery(0),
m_Mode(0)
{}


CVideoEngine::~CVideoEngine()
{
#if SDL_VERSION_ATLEAST(2, 0, 0)

    if(sdlTexture)
    {
        SDL_DestroyTexture(sdlTexture);
    }

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
    #if !SDL_VERSION_ATLEAST(2, 0, 0)
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

void CVideoEngine::updateAspectRect(const CRect<Uint16>& displayRes, const int aspWidth, const int aspHeight)
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
        //mAspectCorrectionRect.h = displayRes.h-displayRes.h%aspHeight;
        //mAspectCorrectionRect.w = displayRes.h/aspHeight*aspWidth;
        mAspectCorrectionRect.h = displayRes.h;
        mAspectCorrectionRect.w = (displayRes.h*aspWidth)/aspHeight;
    }
    else // Taller than width:height so shrink height
	{
        //mAspectCorrectionRect.w = displayRes.w-displayRes.w%aspWidth;
        //mAspectCorrectionRect.h = displayRes.w/aspWidth*aspHeight;
        mAspectCorrectionRect.w = displayRes.w;
        mAspectCorrectionRect.h = (displayRes.w*aspHeight)/aspWidth;
    }
    mAspectCorrectionRect.x = (displayRes.w-mAspectCorrectionRect.w)/2;
    mAspectCorrectionRect.y = (displayRes.h-mAspectCorrectionRect.h)/2;
}

SDL_Surface* CVideoEngine::createSurface( std::string name, bool alpha, int width, int height, int bpp, int mode)
{
	SDL_Surface *temporary, *optimized;

    //Colormask mask = getColourMask32bit();

    //temporary = SDL_CreateRGBSurface( mode, width, height, bpp, mask.r, mask.g, mask.b, mask.a);
    temporary = SDL_CreateRGBSurface( mode, width, height, bpp, 0, 0, 0, 0);
//#if SDL_VERSION_ATLEAST(2, 0, 0)
    //Temporary fix until we figure out how to create our own version of DisplayFormatAlpha and DisplayFormat
//#else

    //if(!BlitSurface)
    {
       return temporary;
    }

    optimized = SDL_ConvertSurface(temporary, mpGameSfc->format, mpGameSfc->flags );

    /*if (alpha && bpp==32)
        optimized = g_pVideoDriver->convertThroughBlitSfc( temporary );
	else
        optimized = g_pVideoDriver->convertThroughBlitSfc( temporary );*/

    SDL_FreeSurface(temporary);
//#endif

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
    // Configure the Scaler
    Scaler.setFilterFactor(m_VidConfig.m_ScaleXFilter);
    Scaler.setFilterType(m_VidConfig.m_normal_scale);

    const CRect<Uint16> &gamerect = m_VidConfig.m_GameRect;

    g_pLogFile->textOut("Blitsurface creation!\n");


    mpGameSfc.reset(SDL_CreateRGBSurface( m_Mode, gamerect.w, gamerect.h, RES_BPP,
                                          0x00FF0000,
                                          0x0000FF00,
                                          0x000000FF,
                                          0xFF000000), SDL_FreeSurface );

#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetSurfaceBlendMode(mpGameSfc.get(), SDL_BLENDMODE_NONE);
#endif


    const int squareSize = getPowerOfTwo( gamerect.h > gamerect.w ? gamerect.h : gamerect.w );

    // This function creates the surfaces which are needed for the game.
    ScrollSurface = SDL_CreateRGBSurface( m_Mode,
                                          squareSize,
                                          squareSize, 32,
                                          0x00FF0000,
                                          0x0000FF00,
                                          0x000000FF,
                                          0x00000000);

    g_pLogFile->textOut("FilteredSurface creation!\n");

    auto blit = mpGameSfc.get();
    auto *format = blit->format;

    if(m_VidConfig.m_ScaleXFilter > 1)
    {
        mpScreenSfc.reset(SDL_CreateRGBSurface( m_Mode,
                                                blit->w*m_VidConfig.m_ScaleXFilter,
                                                blit->h*m_VidConfig.m_ScaleXFilter,
                                                RES_BPP,
                                                format->Rmask,
                                                format->Gmask,
                                                format->Bmask,
                                                format->Amask ), SDL_FreeSurface);
    }
    else
    {
        mpScreenSfc = mpGameSfc;
    }

    m_dst_slice = mpScreenSfc->pitch;

    initOverlaySurface(false, blit->w, blit->h);

    /*Scaler.setFilterFactor(m_VidConfig.m_ScaleXFilter);
    Scaler.setFilterType(m_VidConfig.m_normal_scale);
    Scaler.setDynamicFactor( float(FilteredSurface->w)/float(aspectCorrectionRect.w),
                             float(FilteredSurface->h)/float(aspectCorrectionRect.h));*/

    return true;
}


void CVideoEngine::blitScrollSurface() // This is only for tiles
{									   // The name should be changed
	SDL_Rect srcrect, dstrect;
    int sbufferx, sbuffery;
    auto BlitSurface = mpGameSfc.get();
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


    SDL_FillRect(BlitSurface, nullptr, SDL_MapRGB(BlitSurface->format, 0, 0, 0));

    srcrect.x =	sbufferx;
    srcrect.y = sbuffery;

    const bool wraphoz = (sbufferx+Gamerect.w > scrollSfcWidth);
    const bool wrapvrt = (sbuffery+Gamerect.h > scrollSfcHeight);

    // Upper-Left Part to draw from the Scrollbuffer
    srcrect.w = wraphoz ? (scrollSfcWidth-sbufferx) : Gamerect.w;
    srcrect.h = wrapvrt ? (scrollSfcHeight-sbuffery) : Gamerect.h;

    SDL_BlitSurface(ScrollSurface, &srcrect, BlitSurface, &dstrect);

    const Uint16 upperLeftW = srcrect.w;
    const Uint16 upperLeftH =  srcrect.h;

    // upper-right part
    if (wraphoz)
	{
        srcrect.w = Gamerect.w - upperLeftW;
        srcrect.x = 0;
        dstrect.x = Gamerect.x + upperLeftW;

		SDL_BlitSurface(ScrollSurface, &srcrect, BlitSurface, &dstrect);
    }

    // lower-right part
    if (wrapvrt)
	{
        srcrect.h = Gamerect.h - upperLeftH;
        srcrect.y = 0;
        dstrect.y = Gamerect.y + upperLeftH;

		SDL_BlitSurface(ScrollSurface, &srcrect, BlitSurface, &dstrect);
	}

    if(!wraphoz || !wrapvrt)
        return;

    // and lower-left part
    srcrect.x = sbufferx;
    srcrect.y = 0;
    srcrect.w = upperLeftW;

    dstrect.x = Gamerect.x;
    dstrect.y = Gamerect.y+upperLeftH;

    SDL_BlitSurface(ScrollSurface, &srcrect, BlitSurface, &dstrect);
}

/*void CVideoEngine::stop()
{
	g_pLogFile->textOut(GREEN, "Freeing the following graphical surfaces:<br>\n");

    if( BlitSurface )
    {
        SDL_FreeSurface(BlitSurface);
        g_pLogFile->textOut("freed BlitSurface<br>");
        BlitSurface=NULL;
    }

    if( FilteredSurface )
    {
        SDL_FreeSurface(FilteredSurface);
        g_pLogFile->textOut("freed FilteredSurface<br>");
        FilteredSurface = NULL;
    }

    if( ScrollSurface && (ScrollSurface->map != NULL) )
    {
        SDL_FreeSurface(ScrollSurface);
        g_pLogFile->textOut("freed ScrollSurface<br>");
        ScrollSurface = NULL;
    }
}*/


void CVideoEngine::filterUp()
{
    // If ScaleX is enabled scaleup to screensurface
    // Otherwise screensurface point to gameSfc and nothing needs to be done
    if(m_VidConfig.m_ScaleXFilter > 1)
    {
        auto srcSfc = mpGameSfc.get();
        auto dstSfc = mpScreenSfc.get();

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
