/*
 * CVideoEngine.cpp
 *
 *  Created on: 05.02.2011
 *      Author: gerhard
 */

#include "CVideoEngine.h"
#include "CLogFile.h"
#include "sys/param.h"

CVideoEngine::CVideoEngine(const CVidConfig& VidConfig, Sint16 *&p_sbufferx, Sint16 *&p_sbuffery) :
BlitSurface(NULL),
FGLayerSurface(NULL),       // Scroll buffer for Messages
ScrollSurface(NULL),       // 512x512 scroll buffer
FXSurface(NULL),
m_blitsurface_alloc(false),
m_VidConfig(VidConfig),
mp_sbufferx(p_sbufferx),
mp_sbuffery(p_sbuffery),
screen(NULL),
m_Mode(0)
{}

bool CVideoEngine::init()
{
	const st_resolution &Res = m_VidConfig.m_Resolution;
	const SDL_Rect &GameRect = m_VidConfig.m_Gamescreen;

	// Setup mode depends on some systems.
#if defined(CAANOO) || defined(WIZ) || defined(DINGOO) || defined(NANONOTE) || defined(ANDROID)
	m_Mode = SDL_SWSURFACE;
#elif defined(GP2X)
	m_Mode = SDL_DOUBLEBUF | SDL_HWSURFACE;
#else
	// Support for double-buffering
	m_Mode = SDL_DOUBLEBUF | SDL_HWPALETTE | SDL_HWSURFACE;
#endif

	// Enable OpenGL
#ifdef USE_OPENGL
	if(m_VidConfig.m_opengl)
	{
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	#if SDL_VERSION_ATLEAST(1, 3, 0)
	#else
		SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
	#endif

		m_Mode |= SDL_OPENGL;
	}
#endif

	// Now we decide if it will be fullscreen or windowed mode.
	if(m_VidConfig.Fullscreen)
		m_Mode |= SDL_FULLSCREEN;

	// And leave the rest to SDL!
	screen = SDL_SetVideoMode( Res.width, Res.height, Res.depth, m_Mode );

	if(!screen)
	{
		g_pLogFile->textOut(RED,"VidDrv_Start(): Couldn't create a SDL surface: %s<br>", SDL_GetError());
		return false;
	}

	// If Fullscreen hide the mouse cursor.
	// Anyway, it just can point but does not interact yet
 	SDL_ShowCursor(!m_VidConfig.Fullscreen);

 	m_dst_slice = Res.width*screen->format->BytesPerPixel;
 	m_src_slice = GameRect.w*screen->format->BytesPerPixel;

	return true;
}

SDL_Surface* CVideoEngine::createSurface( std::string name, bool alpha, int width, int height, int bpp, int mode, SDL_PixelFormat* format )
{
	SDL_Surface *temporary, *optimized;

	temporary = SDL_CreateRGBSurface( mode, width, height, bpp, format->Rmask, format->Gmask, format->Bmask, format->Amask);
	if (alpha && bpp==32)
		optimized = SDL_DisplayFormatAlpha( temporary );
	else
		optimized = SDL_DisplayFormat( temporary );

	SDL_FreeSurface(temporary);

	if (!optimized)
	{
		g_pLogFile->textOut(RED,"VideoDriver: Couldn't create surface:" + name +"<br>");
		return NULL;
	}

	bpp = optimized->format->BitsPerPixel;
	return optimized;
}

void CVideoEngine::fetchStartScreenPixelPtrs(Uint8 *&ScreenPtr, Uint8 *&BlitPtr,
										unsigned int &width, unsigned int &height)
{
	const st_resolution &Res = m_VidConfig.m_Resolution;
	const SDL_Rect &GameScreen = m_VidConfig.m_Gamescreen;
	const int xpos = (Res.width-GameScreen.w*m_VidConfig.Zoom)/2;
	const int ypos = (Res.height-GameScreen.h*m_VidConfig.Zoom)/2;
	const int xoffset = xpos*(Res.depth>>3);
	const int yoffset = ypos*screen->pitch;
	width = MIN(Res.width, GameScreen.w*m_VidConfig.Zoom);
	height = MIN(Res.height, GameScreen.h*m_VidConfig.Zoom);
	width /= m_VidConfig.Zoom;
	height /= m_VidConfig.Zoom;

	ScreenPtr = (Uint8*) screen->pixels;
	BlitPtr = (Uint8*) BlitSurface->pixels;

	if(xoffset>0)
		ScreenPtr += xoffset;

	if(yoffset>0)
		ScreenPtr += yoffset;
}

//void CVideoEngine::scale2xnofilter(char* dest, char* src, short bbp)
void CVideoEngine::scale2xnofilter(char* restrict dest, char* restrict src, short bbp)
{
	// workaround for copying correctly stuff to the screen, so the screen is scaled normally
    // to 2x (without filter). This applies to 16 and 32-bit colour depth.
	// It uses bit shifting method for faster blit!
	bbp >>= 1;
	const st_resolution &Res = m_VidConfig.m_Resolution;
	const SDL_Rect &GameRect = m_VidConfig.m_Gamescreen;

	for( int i=0, j=0 ; i < GameRect.h ; i++ )
	{
		for( j = 0 ; j < GameRect.w ; j++)
		{
			memcpy(dest+((j<<1)<<bbp)+(((i<<1)*Res.width)<<bbp),src+(j<<bbp)+((i*GameRect.w)<<bbp),bbp<<1);
			memcpy(dest+(((j<<1)+1)<<bbp)+(((i<<1)*Res.width)<<bbp),src+(j<<bbp)+((i*GameRect.w)<<bbp),bbp<<1);
		}
		memcpy(dest+(((i<<1)+1)*(Res.width<<bbp)),(dest+(i<<1)*(Res.width<<bbp)),(bbp<<2)*GameRect.w);
	}
}

//void CVideoEngine::scale3xnofilter(char *dest, char *src, short bbp)
void CVideoEngine::scale3xnofilter(char* restrict dest, char* restrict src, short bbp)
{
	// workaround for copying correctly stuff to the screen, so the screen is scaled normally
    // to 2x (without filter). This applies to 16 and 32-bit colour depth.
	// Optimization of using bit shifting
	bbp >>= 1;
	const st_resolution &Res = m_VidConfig.m_Resolution;
	const SDL_Rect &GameRect = m_VidConfig.m_Gamescreen;

	for( int i=0, j=0 ; i < GameRect.h ; i++ )
	{
		for(j = 0; j < GameRect.w ; j++)
		{
			// j*3 = (j<<1) + j
			memcpy(dest+(((j<<1)+j)<<bbp)+((((i<<1) + i)*Res.width)<<bbp),src+(j<<bbp)+((i*GameRect.w)<<bbp),bbp<<1);
			memcpy(dest+(((j<<1)+j+1)<<bbp)+((((i<<1) + i)*Res.width)<<bbp),src+(j<<bbp)+((i*GameRect.w)<<bbp),bbp<<1);
			memcpy(dest+(((j<<1)+j+2)<<bbp)+((((i<<1) + i)*Res.width)<<bbp),src+(j<<bbp)+((i*GameRect.w)<<bbp),bbp<<1);
		}
		memcpy(dest+((i<<1)+i+1)*(Res.width<<bbp),dest+((i<<1)+i)*(Res.width<<bbp),(3<<bbp)*GameRect.w);
		memcpy(dest+((i<<1)+i+2)*(Res.width<<bbp),dest+((i<<1)+i)*(Res.width<<bbp),(3<<bbp)*GameRect.w);
	}
}

//void CVideoEngine::scale4xnofilter(char *dest, char *src, short bbp)
void CVideoEngine::scale4xnofilter(char* restrict dest, char* restrict src, short bbp)
{
	// workaround for copying correctly stuff to the screen, so the screen is scaled normally
    // to 4x (without filter). This applies to 16 and 32-bit colour depth.
	// use bit shifting method for faster blit!
	bbp >>= 1;
	const st_resolution &Res = m_VidConfig.m_Resolution;
	const SDL_Rect &GameRect = m_VidConfig.m_Gamescreen;

	char* restrict srctemp;
	char* restrict desttemp;
	int size;

	for( int i=0, j=0 ; i < GameRect.h ; i++ )
	{
		for(j = 0; j < GameRect.w ; j++)
		{
			// j*4 = (j<<2)
			srctemp = src+((j+(i*GameRect.w))<<bbp);
			desttemp = dest+((4*(j+(i*Res.width)))<<bbp);
			memcpy(desttemp,srctemp,bbp<<1);
			memcpy(desttemp+(1<<bbp),srctemp,bbp<<1);
			memcpy(desttemp+(2<<bbp),srctemp,bbp<<1);
			memcpy(desttemp+(3<<bbp),srctemp,bbp<<1);
		}
		srctemp = dest+(((i<<2)*Res.width)<<bbp);
		desttemp = dest+((((i<<2)+1)*Res.width)<<bbp);
		size = GameRect.w*(bbp<<1<<2);

		memcpy(desttemp,srctemp,size);
		memcpy(desttemp+(Res.width<<bbp),srctemp,size);
		memcpy(desttemp+((Res.width<<bbp)<<1),srctemp,size);
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
	const SDL_Rect &Gamerect = m_VidConfig.m_Gamescreen;

	dstrect.x = 0; dstrect.y = 0;

	srcrect.x =	sbufferx = *mp_sbufferx;
	srcrect.y = sbuffery = *mp_sbuffery;

	dstrect.w = (Gamerect.w>sbufferx) ? Gamerect.w-sbufferx : Gamerect.w ;
	dstrect.h = (Gamerect.h>sbuffery) ? Gamerect.h-sbuffery : Gamerect.h ;

	if (sbufferx > (Uint16)(512-Gamerect.w))
	{ // need to wrap right side
		srcrect.w = (512-sbufferx);
		wraphoz = 1;
	}
	else
	{ // single blit for whole horizontal copy
		srcrect.w = Gamerect.w;
		wraphoz = 0;
	}

	if (sbuffery > (Uint16)(512-Gamerect.h))
	{ // need to wrap on bottom
		srcrect.h = (512-sbuffery);
		wrapvrt = 1;
	}
	else
	{ // single blit for whole bottom copy
		srcrect.h = Gamerect.h;
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
		dstrect.w = Gamerect.w - dstrect.x;
		srcrect.x = 0;
		srcrect.w = (Gamerect.w - srcrect.w);
		SDL_BlitSurface(ScrollSurface, &srcrect, BlitSurface, &dstrect);

		// now repeat for the bottom
		// (lower-right square)
		dstrect.y = srcrect.h;
		dstrect.h = Gamerect.h - dstrect.y;
		srcrect.y = 0;
		srcrect.h = (Gamerect.h - srcrect.h);
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
		dstrect.w = Gamerect.w - dstrect.x;
		srcrect.x = 0;
		srcrect.w = Gamerect.w - srcrect.w;
		SDL_BlitSurface(ScrollSurface, &srcrect, BlitSurface, &dstrect);
	}
	else if (wrapvrt)
	{
		dstrect.y = srcrect.h;
		dstrect.h = Gamerect.h - dstrect.y;
		srcrect.y = 0;
		srcrect.h = (Gamerect.h - srcrect.h);
		SDL_BlitSurface(ScrollSurface, &srcrect, BlitSurface, &dstrect);
	}
}

void CVideoEngine::stop()
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

void CVideoEngine::shutdown()
{
	stop();
}
