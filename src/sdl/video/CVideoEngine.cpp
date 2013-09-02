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


Uint16 getPowerOfTwo(const Uint16 value)
{
    Uint16 output = 1;
    while (output<value)
        output <<= 1;
    return output;
}

CVideoEngine::CVideoEngine(const CVidConfig& VidConfig) :
BlitSurface(NULL),
FilteredSurface(NULL),
ScrollSurface(NULL),       // Squared scroll buffer
m_VidConfig(VidConfig),
mSbufferx(0),
mSbuffery(0),
#if SDL_VERSION_ATLEAST(2, 0, 0)
    window(nullptr),
    renderer(nullptr),
    sdlTexture(nullptr),
#else
    screen(nullptr),
#endif
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
	const CRect<Uint16> &GameRect = m_VidConfig.m_GameRect;

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
        if( int ok = SDL_GetWMInfo(&info) )
        {
            if(ok > 0)
            {
                ShowWindow(info.window, SW_SHOWNORMAL);
            }
        }

    }
	#endif

#if SDL_VERSION_ATLEAST(2, 0, 0)
    
#else
 	m_src_slice = GameRect.w*screen->format->BytesPerPixel;
#endif

	return true;
}

void CVideoEngine::aspectCorrectResizing(const CRect<Uint16>& newDim, const int width, const int height)
{
	if (width == 0 || height == 0)
	{
		aspectCorrectionRect.x = aspectCorrectionRect.y = 0;
		aspectCorrectionRect.w = newDim.w;
		aspectCorrectionRect.h = newDim.h;
		return;
	}
	
	if (height*newDim.w >= width*newDim.h) // Wider than width:3, so shrink newDim.w
	{
		aspectCorrectionRect.h = newDim.h-newDim.h%height;
		aspectCorrectionRect.w = newDim.h/height*width;
	}
	else // Taller than width:height so shrink newDim.h
	{
		aspectCorrectionRect.w = newDim.w-newDim.w%width;
		aspectCorrectionRect.h = newDim.w/width*height;
	}
	aspectCorrectionRect.x = (newDim.w-aspectCorrectionRect.w)/2;
	aspectCorrectionRect.y = (newDim.h-aspectCorrectionRect.h)/2;
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

    optimized = SDL_ConvertSurface(temporary, BlitSurface->format, BlitSurface->flags );

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



void CVideoEngine::blitScrollSurface() // This is only for tiles
{									   // The name should be changed
	SDL_Rect srcrect, dstrect;
	Sint16 sbufferx, sbuffery;
	const SDL_Rect Gamerect = m_VidConfig.m_GameRect.SDLRect();

    const int squareSize = ScrollSurface->w;

	dstrect.x = 0; dstrect.y = 0;
	srcrect.x =	sbufferx = mSbufferx;
	srcrect.y = sbuffery = mSbuffery;

    const bool wraphoz = (sbufferx > (squareSize-Gamerect.w));
    const bool wrapvrt = (sbuffery > (squareSize-Gamerect.h));

	dstrect.w = (Gamerect.w>sbufferx) ? Gamerect.w-sbufferx : Gamerect.w ;
	dstrect.h = (Gamerect.h>sbuffery) ? Gamerect.h-sbuffery : Gamerect.h ;
    srcrect.w = wraphoz ? (squareSize-sbufferx) : Gamerect.w;
    srcrect.h = wrapvrt ? (squareSize-sbuffery) : Gamerect.h;

	SDL_BlitSurface(ScrollSurface, &srcrect, BlitSurface, &dstrect);

	const Uint16 saveDstw = dstrect.w;
	const Uint16 saveSrcw = srcrect.w;

	if (wraphoz)
	{
		dstrect.x = srcrect.w;
		dstrect.w = Gamerect.w - dstrect.x;
		srcrect.x = 0;
		srcrect.w = Gamerect.w - srcrect.w;
		SDL_BlitSurface(ScrollSurface, &srcrect, BlitSurface, &dstrect);

		if(!wrapvrt)
			return;
	}

	if (wrapvrt)
	{
		dstrect.y = srcrect.h;
		dstrect.h = Gamerect.h - dstrect.y;
		srcrect.y = 0;
		srcrect.h = Gamerect.h - srcrect.h;
		SDL_BlitSurface(ScrollSurface, &srcrect, BlitSurface, &dstrect);

		if(!wraphoz)
			return;
	}

	srcrect.x = sbufferx;
	srcrect.w = saveSrcw;
	dstrect.x = 0;
	dstrect.w = saveDstw;
	SDL_BlitSurface(ScrollSurface, &srcrect, BlitSurface, &dstrect);
}

void CVideoEngine::stop()
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

    if(ScrollSurface && (ScrollSurface->map != NULL))
    {
        SDL_FreeSurface(ScrollSurface);
        g_pLogFile->textOut("freed ScrollSurface<br>");
        ScrollSurface=NULL;
    }    
}



void CVideoEngine::shutdown()
{
	stop();
}
