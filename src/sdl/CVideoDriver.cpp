
/*
 * CVideoDriver.cpp
 *
 *  Created on: 17.03.2009
 *      Author: gerstrong
 */
#include "CVideoDriver.h"
#include "CInput.h"
#include "CTimer.h"

#include "../keen.h"
#include "../graphics/CGfxEngine.h"
#include "../scale2x/scalebit.h"
#include "../CLogFile.h"
#include "../FindFile.h"
#include <iostream>
#include <fstream>

#define CKLOGFILENAME            	"genius.log"

#define MAX_CONSOLE_MESSAGES     	3
#define CONSOLE_MESSAGE_X        	3
#define CONSOLE_MESSAGE_Y        	3
#define CONSOLE_MESSAGE_SPACING  	9
#define CONSOLE_EXPIRE_RATE      	250

// pointer to the line in VRAM to start blitting to when stretchblitting.
// this may not be the first line on the display as it is adjusted to
// center the image on the screen when in fullscreen.
unsigned char *VRAMPtr;
char blitsurface_alloc = 0;

typedef struct stConsoleMessage
{
	char msg[80];
} stConsoleMessage;
stConsoleMessage cmsg[MAX_CONSOLE_MESSAGES];
int NumConsoleMessages = 0;
int ConsoleExpireTimer = 0;


CVideoDriver::CVideoDriver() {
	// Default values

	showfps=true;
#if defined(WIZ) || defined(GP2X)
	m_Resolution.width=320;
	m_Resolution.height=240;
#if defined(GP2X)
	m_Resolution.depth=32;
#else
	m_Resolution.depth=16;
#endif
	Mode=0;
	Fullscreen=true;
	m_ScaleXFilter=1;
	Zoom=1;
	m_targetfps = 60;	// Enable automatic frameskipping by default at 30
#else
	m_Resolution.width=320;
	m_Resolution.height=200;
	m_Resolution.depth=32;
	Mode=0;
	Fullscreen=false;
	m_ScaleXFilter=1;
	Zoom=2;
	m_targetfps = 60;
#endif
#ifdef USE_OPENGL
	m_opengl_filter = GL_NEAREST;
	mp_OpenGL = NULL;
	m_opengl = false; // Must stay optional for better compatibility
#endif
	m_aspect_correction = true;
	m_aspect_ratio = 8.0f/5.0f;

	screenrect.x=0;
	screenrect.y=0;
	screenrect.h=0;
	screenrect.w=0;

	ScrollSurface=NULL;       // 512x512 scroll buffer
	FGLayerSurface=NULL;       // Scroll buffer for Messages
	BlitSurface=NULL;
	m_special_fx = true;

	mp_sbufferx = mp_sbuffery = NULL;

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0)
	{
		g_pLogFile->textOut(RED,"Could not initialize SDL: %s<br>", SDL_GetError());
	}
	else
	{
		g_pLogFile->textOut(GREEN,"SDL was successfully initialized!<br>");
		m_maxwidth = SDL_GetVideoInfo()->current_w;
	}

	initResolutionList();

	// take the first default resolution. It might be changed if there is a config file already created
	setMode(m_Resolutionlist.front().width, m_Resolutionlist.front().height, m_Resolutionlist.front().depth);
}

// initResolutionList() reads the local list of available resolution.
// This function can only be called internally
void CVideoDriver::initResolutionList()
{
	st_resolution resolution;
	char buf[256];
	m_Resolutionlist.clear();

	std::ifstream ResolutionFile; OpenGameFileR(ResolutionFile, "resolutions.cfg");
	if(!ResolutionFile)
	{
		g_pLogFile->textOut(PURPLE,"Warning: resolutions.cfg could not be read! Maybe your files weren't extracted correctly!<br>");
		g_pLogFile->textOut(PURPLE,"Using default resolution<br>");
	}
	else
	{
		while(!ResolutionFile.eof())
		{
			ResolutionFile.getline(buf,256);

			if(sscanf(buf,"%ix%ix%i", &resolution.width, &resolution.height, &resolution.depth) >= 2)

			// Now check if it's possible to use this resolution
			checkResolution( resolution, SDL_FULLSCREEN );
		}
		ResolutionFile.close();

		if(!Fullscreen)
		{
			int e = 1;
			resolution.width = 320;
			resolution.height = 200;
#ifdef WIZ
			resolution.depth = 16;
#else
			resolution.depth = 32;
#endif

			while(resolution.width < m_maxwidth)
			{
				resolution.width = 320 * e;
				resolution.height = 200 * e;

				// Now check if it's possible to use this resolution
				checkResolution( resolution, 0 );

				e++;
			}
		}
	}

	if(m_Resolutionlist.empty()) {
		resolution.width = 320;
		resolution.height = 200;
		resolution.depth = 16;
		m_Resolutionlist.push_back(resolution);
	}

	m_Resolution_pos = m_Resolutionlist.begin();
}

void CVideoDriver::checkResolution( st_resolution& resolution, int flags )
{
	int depth;

	for ( depth = resolution.depth; depth >= 16; depth -= 16 )
	{
		resolution.depth = depth;
		resolution.depth = SDL_VideoModeOK( resolution.width, resolution.height, resolution.depth, flags );

		if(resolution.depth)
		{
			std::list<st_resolution> :: iterator i;
			for( i = m_Resolutionlist.begin() ; i != m_Resolutionlist.end() ; i++ )
			{
				if(i->width  == resolution.width  &&
				   i->height == resolution.height &&
				   i->depth  == resolution.depth) break;
			}

			if(i == m_Resolutionlist.end())
			{
#ifdef DEBUG
				printf( "%ix%ix%i %X added\n", resolution.width, resolution.height, resolution.depth, flags );
#endif
				m_Resolutionlist.push_back(resolution);
			}
		}

		if (resolution.depth==16) break;
	}
}

st_resolution CVideoDriver::getNextResolution()
{
	m_Resolution_pos++;

	if( m_Resolution_pos == m_Resolutionlist.end() )
		m_Resolution_pos = m_Resolutionlist.begin();

	return *m_Resolution_pos;
}

// Sets the resolution. you can pass st_resolution-structure or the variables separated.
void CVideoDriver::setMode(st_resolution Resolution)
{
	setMode(Resolution.width, Resolution.height, Resolution.depth);
}

void CVideoDriver::setSpecialFXMode(bool SpecialFX)
{	m_special_fx = SpecialFX;	}

void CVideoDriver::setMode(int width, int height,int depth)
{
	m_Resolution.width = width;
	m_Resolution.height = height;
	m_Resolution.depth = depth;

	// TODO: Cycle through the list until the matching resolution is matched. If it doesn't exist
	// add it;
	for(m_Resolution_pos = m_Resolutionlist.begin() ; m_Resolution_pos != m_Resolutionlist.end() ; m_Resolution_pos++)
		if( m_Resolution_pos->width == width )
			if( m_Resolution_pos->height == height )
				if( m_Resolution_pos->depth == depth )
					break;

	if(m_Resolution_pos == m_Resolutionlist.end())
	{
		m_Resolutionlist.push_back(m_Resolution);
		m_Resolution_pos--;
	}
}

void CVideoDriver::stop(void)
{
    if(blitsurface_alloc && BlitSurface)
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
#ifdef USE_OPENGL
	if(mp_OpenGL) { delete mp_OpenGL; mp_OpenGL = NULL; }
#endif
	g_pLogFile->textOut(GREEN,"CVideoDriver Close%s<br>", SDL_GetError());
}


bool CVideoDriver::start(void)
{
	bool retval = false;

	SDL_WM_SetCaption("Commander Genius (CKP)", NULL);
	// When the program is through executing, call SDL_Quit
	atexit(SDL_Quit);

	if(!applyMode())
	{
		g_pLogFile->textOut(RED,"VideoDriver: Error applying mode! Your Videocard doesn't seem to work on CKP<br>");
		g_pLogFile->textOut(RED,"Check, if you have the most recent drivers installed!<br>");
		return false;
	}

	retval = createSurfaces();
	initOpenGL();

	return retval;
}

bool CVideoDriver::initOpenGL()
{
#ifdef USE_OPENGL
	if(m_opengl) // If OpenGL could be set, initialize the matrices
	{
		mp_OpenGL = new COpenGL(game_resolution_rect);
		if(!(mp_OpenGL->initGL(m_Resolution.width, m_Resolution.height, m_Resolution.depth,
							   m_opengl_filter, m_ScaleXFilter, m_aspect_ratio)))
		{
			delete mp_OpenGL;
			mp_OpenGL = NULL;
			m_opengl = false;
		}
		else
			mp_OpenGL->setSurface(BlitSurface);
	}
#endif

	return m_opengl;
}

bool CVideoDriver::applyMode()
{
	// Check if some zoom/filter modes are illogical
	// TODO: Make this call better to understand
	// It must be able to change the resolution, and if it fails, roll back.

	// bool rollback=false;
	//if(!checkMode())
	// rollback = true;

	if( (Zoom == 3 && m_ScaleXFilter == 1) && !m_opengl )
		Zoom = 2;

	m_Resolution = *m_Resolution_pos;

	// Setup mode depends on some systems.
#if defined(WIZ)
    Mode = SDL_SWSURFACE;
#elif defined(GP2X)
    Mode = SDL_DOUBLEBUF | SDL_HWSURFACE;
#else
	// Support for doublebuffering
	Mode = SDL_DOUBLEBUF | SDL_HWPALETTE | SDL_HWSURFACE;
#endif

	// Enable OpenGL
#ifdef USE_OPENGL
	if(m_opengl)
	{
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
		Mode |= SDL_OPENGL;
	}
#endif

	// This is the default game resolution
    game_resolution_rect.w = 320;
	game_resolution_rect.h = 200;

	// Here we check, if we can enhance the screen-resolution and do it if yes
	//game_resolution_rect = adaptGameResolution();

	// Now we decide if it will be fullscreen or windowed mode.
	if(Fullscreen)
		Mode |= SDL_FULLSCREEN;

	// Before the resolution is set, check, if the zoom factor is too high!
	while(((m_Resolution.width/game_resolution_rect.w) < Zoom || (m_Resolution.height/game_resolution_rect.h) < Zoom) && (Zoom > 1))
		Zoom--;

    // Center the screen!
    blitrect.x = 0;
    blitrect.y = 0;
	screenrect.w = blitrect.w = game_resolution_rect.w*Zoom;
	screenrect.h = blitrect.h = game_resolution_rect.h*Zoom;
	screenrect.x = (m_Resolution.width-screenrect.w)/2;
	screenrect.y = (m_Resolution.height-screenrect.h)/2;

	// And leave the rest to SDL!
	screen = SDL_SetVideoMode(m_Resolution.width,m_Resolution.height,m_Resolution.depth,Mode);

	if(!screen)
	{
		g_pLogFile->textOut(RED,"VidDrv_Start(): Couldn't create a SDL surface: %s<br>", SDL_GetError());
		return false;
	}

	// Now SDL will tell if the bpp works or changes it, if not supported.
	// this value is updated here!
	m_Resolution.depth = screen->format->BitsPerPixel;

	// If Fullscreen hide the mouse cursor.
	// Anyway, it just can point but does not interact yet
 	SDL_ShowCursor(!Fullscreen);

 	m_bytes_per_pixel = screen->format->BytesPerPixel;
 	m_dst_slice = m_Resolution.width*m_bytes_per_pixel;
 	m_src_slice = game_resolution_rect.w*m_bytes_per_pixel;

	return true;
}

// This function adapts the gamescreenspace to the monitor format.
// 16:9, 16:10, 4:3, everthing is supported as far it fits to the height!
SDL_Rect CVideoDriver::adaptGameResolution()
{
	float scalefactor;
	SDL_Rect Gamerect;

	Gamerect = game_resolution_rect;
	scalefactor = ((float)m_Resolution.width) / ((float)Gamerect.w);
	Gamerect.h = (int)( ((float)m_Resolution.height) / scalefactor );

	return Gamerect;
}

void CVideoDriver::setFilter(short value) { m_ScaleXFilter = value; } // 1 means no filter of course

void CVideoDriver::setZoom(short value) { Zoom = value; }

bool CVideoDriver::createSurfaces()
{
	// This function creates the surfaces which are needed for the game.
    ScrollSurface = createSurface( "ScrollSurface", true,
								  512,
								  512,
								  m_Resolution.depth,
								  Mode, screen->format );

    if (m_Resolution.width == game_resolution_rect.w && !m_opengl)
    {
    	g_pLogFile->textOut("Blitsurface = Screen<br>");
    	BlitSurface = screen;
    	blitsurface_alloc = 0;
    }
    else
    {
    	g_pLogFile->textOut("Blitsurface = creatergbsurfacefrom<br>");
        BlitSurface = createSurface( "BlitSurface", true,
									game_resolution_rect.w,
									game_resolution_rect.h,
									m_Resolution.depth,
									Mode, screen->format );
		blitsurface_alloc = 1;
    }
    VRAMPtr = (unsigned char*)screen->pixels +
	screenrect.y*screen->pitch + (screenrect.x*m_Resolution.depth>>3);

    // Some surfaces could get 320x240 and the screenspace is extended.
    // The video class must be changed for any further resolutions
    game_resolution_rect.x = 0; game_resolution_rect.y = 0;

    FGLayerSurface = createSurface( "FGLayerSurface", false,
								   game_resolution_rect.w,
								   game_resolution_rect.h,
								   m_Resolution.depth,
								   Mode, screen->format );

	SDL_SetColorKey( FGLayerSurface, SDL_SRCCOLORKEY,
					SDL_MapRGB(FGLayerSurface->format, 0, 0xFF, 0xFE) );
	//Set surface alpha
	SDL_SetAlpha( FGLayerSurface, SDL_SRCALPHA, 225 );

    FXSurface = createSurface( "FXSurface", false,
							  game_resolution_rect.w,
							  game_resolution_rect.h,
							  m_Resolution.depth,
							  Mode, screen->format );

	g_pGfxEngine->Palette.setFXSurface( FXSurface );

	return true;
}

SDL_Surface* CVideoDriver::createSurface( std::string name, bool alpha, int width, int height, int bpp, int mode, SDL_PixelFormat* format )
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
		return false;
	}
	bpp = optimized->format->BitsPerPixel;
	return optimized;
}

// defines the scroll-buffer that is used for blitScrollSurface(). It's normally passed by a CMap Object
// It might have when a level-map is loaded.
void CVideoDriver::setScrollBuffer(Sint16 *pbufx, Sint16 *pbufy)
{
	mp_sbufferx = pbufx;
	mp_sbuffery = pbufy;
}

void CVideoDriver::blitScrollSurface() // This is only for tiles
									   // The name should be changed
{
	SDL_Rect srcrect;
	SDL_Rect dstrect;
	Sint16 sbufferx, sbuffery;
	char wraphoz, wrapvrt;
	int save_dstx, save_dstw, save_srcx, save_srcw;

	dstrect.x = 0; dstrect.y = 0;

	srcrect.x = sbufferx = *mp_sbufferx;
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

void CVideoDriver::collectSurfaces()
{
	SDL_BlitSurface(FGLayerSurface, NULL, BlitSurface, NULL);

	if(FXSurface->format->alpha)
		SDL_BlitSurface(FXSurface, NULL, BlitSurface, NULL);
}

void CVideoDriver::updateScreen()
{

#ifdef USE_OPENGL
	if(m_opengl)
	{
		mp_OpenGL->render();

		// Flush the FG-Layer
		SDL_FillRect(FGLayerSurface, NULL, SDL_MapRGB(FGLayerSurface->format, 0, 0xFF, 0xFE));
	}
	else // No OpenGL but Software Rendering
	{
#endif
		// if we're doing zoom then we have copied the scroll buffer into
		// another offscreen buffer, and must now stretchblit it to the screen
		if (Zoom == 1 && m_Resolution.width != game_resolution_rect.w )
		{
			LockSurface(BlitSurface);
			LockSurface(screen);

			if(m_ScaleXFilter == 1)
			{
				SDL_Rect scrrect, dstrect;
				scrrect.y = 0;
				scrrect.x = 0;
				scrrect.h = game_resolution_rect.h;
				scrrect.w = game_resolution_rect.w;
				dstrect.x = (m_Resolution.width-game_resolution_rect.w)/2;
				dstrect.y = (m_Resolution.height-game_resolution_rect.h)/2;
				dstrect.w = game_resolution_rect.w;
				dstrect.h = game_resolution_rect.h;

				SDL_BlitSurface(BlitSurface, &scrrect, screen, &dstrect);
			}
			else
			{
				g_pLogFile->textOut(PURPLE,"Sorry, but this filter doesn't work at that zoom mode<br>");
				g_pLogFile->textOut(PURPLE,"Try to use a higher zoom factor. Switching to no-filter<br>");
				m_ScaleXFilter = 1;
			}
			UnlockSurface(screen);
			UnlockSurface(BlitSurface);
		}
		if (Zoom == 2)
		{
			LockSurface(BlitSurface);
			LockSurface(screen);

			if(m_ScaleXFilter == 1)
			{
				scale2xnofilter((char*)VRAMPtr, (char*)BlitSurface->pixels, m_bytes_per_pixel);
			}
			else if(m_ScaleXFilter == 2)
			{
				scale(m_ScaleXFilter, VRAMPtr, m_dst_slice, BlitSurface->pixels,
						m_src_slice, m_bytes_per_pixel,
						game_resolution_rect.w, game_resolution_rect.h);
			}
			else
			{
				g_pLogFile->textOut(PURPLE,"Sorry, but this filter doesn't work at that zoom mode<br>");
				g_pLogFile->textOut(PURPLE,"Try to use a higher zoom factor. Switching to no-filter<br>");
				m_ScaleXFilter = 1;
			}

			UnlockSurface(screen);
			UnlockSurface(BlitSurface);
		}
		else if (Zoom == 3)
		{
			LockSurface(BlitSurface);
			LockSurface(screen);

			if(m_ScaleXFilter == 1)
			{
				scale3xnofilter((char*)VRAMPtr, (char*)BlitSurface->pixels, m_bytes_per_pixel);
			}
			else if(m_ScaleXFilter == 2 || m_ScaleXFilter == 3)
			{
				scale(m_ScaleXFilter, VRAMPtr, m_dst_slice, BlitSurface->pixels,
						m_src_slice, m_bytes_per_pixel,
						game_resolution_rect.w, game_resolution_rect.h);
			}
			else
			{
				g_pLogFile->textOut(PURPLE,"Sorry, but this filter doesn't work at that zoom mode<br>");
				g_pLogFile->textOut(PURPLE,"Try to use a higher zoom factor. Switching to no-filter<br>");
				m_ScaleXFilter = 1;
			}
			UnlockSurface(screen);
			UnlockSurface(BlitSurface);
		}
		else if (Zoom == 4)
		{
			LockSurface(BlitSurface);
			LockSurface(screen);

			if(m_ScaleXFilter == 1)
			{
				scale4xnofilter((char*)VRAMPtr, (char*)BlitSurface->pixels, m_bytes_per_pixel);
			}
			else if(m_ScaleXFilter >= 2 && m_ScaleXFilter <= 4 )
			{
				scale(m_ScaleXFilter, VRAMPtr, m_dst_slice, BlitSurface->pixels,
						m_src_slice, m_bytes_per_pixel,
						game_resolution_rect.w, game_resolution_rect.h);
			}
			else
			{
				g_pLogFile->textOut(PURPLE,"Sorry, but this filter doesn't work at that zoom mode<br>");
				g_pLogFile->textOut(PURPLE,"Try to use a higher zoom factor. Switching to no-filter<br>");
				m_ScaleXFilter = 1;
			}
			UnlockSurface(screen);
			UnlockSurface(BlitSurface);
		}

#if defined(GP2X) || defined(WIZ)
		// Clear the lower 40 lines to black
		SDL_Rect lower = { 0, 200, 320, 40 };
		SDL_FillRect(screen, &lower, SDL_MapRGB(screen->format, 0, 0, 0) );
#endif

		SDL_Flip(screen);

		// Flush the FG-Layer
		SDL_FillRect(FGLayerSurface, NULL, SDL_MapRGB(FGLayerSurface->format, 0, 0xFF, 0xFE));

#ifdef USE_OPENGL
	}
#endif
}

void CVideoDriver::scale2xnofilter(char *dest, char *src, short bbp)
{
	// workaround for copying correctly stuff to the screen, so the screen is scaled normally
    // to 2x (without filter). This applies to 16 and 32-bit colour depth.
	// It uses bit shifting method for faster blit!
	bbp >>= 1;

	int i,j;
	for(i=0 ; i < game_resolution_rect.h ; i++)
	{
		for(j = 0; j < game_resolution_rect.w ; j++)
		{
			memcpy(dest+((j<<1)<<bbp)+(((i<<1)*m_Resolution.width)<<bbp),src+(j<<bbp)+((i*game_resolution_rect.w)<<bbp),bbp<<1);
			memcpy(dest+(((j<<1)+1)<<bbp)+(((i<<1)*m_Resolution.width)<<bbp),src+(j<<bbp)+((i*game_resolution_rect.w)<<bbp),bbp<<1);
		}
		memcpy(dest+(((i<<1)+1)*(m_Resolution.width<<bbp)),(dest+(i<<1)*(m_Resolution.width<<bbp)),(bbp<<2)*game_resolution_rect.w);
	}
}

void CVideoDriver::scale3xnofilter(char *dest, char *src, short bbp)
{
	// workaround for copying correctly stuff to the screen, so the screen is scaled normally
    // to 2x (without filter). This applies to 16 and 32-bit colour depth.
	// Optimization of using bit shifting
	bbp >>= 1;

	int i,j;
	for(i=0 ; i < game_resolution_rect.h ; i++)
	{
		for(j = 0; j < game_resolution_rect.w ; j++)
		{
			// j*3 = (j<<1) + j
			memcpy(dest+(((j<<1)+j)<<bbp)+((((i<<1) + i)*m_Resolution.width)<<bbp),src+(j<<bbp)+((i*game_resolution_rect.w)<<bbp),bbp<<1);
			memcpy(dest+(((j<<1)+j+1)<<bbp)+((((i<<1) + i)*m_Resolution.width)<<bbp),src+(j<<bbp)+((i*game_resolution_rect.w)<<bbp),bbp<<1);
			memcpy(dest+(((j<<1)+j+2)<<bbp)+((((i<<1) + i)*m_Resolution.width)<<bbp),src+(j<<bbp)+((i*game_resolution_rect.w)<<bbp),bbp<<1);
		}
		memcpy(dest+((i<<1)+i+1)*(m_Resolution.width<<bbp),dest+((i<<1)+i)*(m_Resolution.width<<bbp),(3<<bbp)*game_resolution_rect.w);
		memcpy(dest+((i<<1)+i+2)*(m_Resolution.width<<bbp),dest+((i<<1)+i)*(m_Resolution.width<<bbp),(3<<bbp)*game_resolution_rect.w);
	}
}

void CVideoDriver::scale4xnofilter(char *dest, char *src, short bbp)
{
	// workaround for copying correctly stuff to the screen, so the screen is scaled normally
    // to 2x (without filter). This applies to 16 and 32-bit colour depth.
	// use bit shifting method for faster blit!
	bbp >>= 1;

	char *srctemp;
	char *desttemp;
	int size;

	int i,j;
	for(i=0 ; i < game_resolution_rect.h ; i++)
	{
		for(j = 0; j < game_resolution_rect.w ; j++)
		{
			// j*4 = (j<<2)
			srctemp = src+((j+(i*game_resolution_rect.w))<<bbp);
			desttemp = dest+((4*(j+(i*m_Resolution.width)))<<bbp);
			memcpy(desttemp,srctemp,bbp<<1);
			memcpy(desttemp+(1<<bbp),srctemp,bbp<<1);
			memcpy(desttemp+(2<<bbp),srctemp,bbp<<1);
			memcpy(desttemp+(3<<bbp),srctemp,bbp<<1);
		}
		srctemp = dest+(((i<<2)*m_Resolution.width)<<bbp);
		desttemp = dest+((((i<<2)+1)*m_Resolution.width)<<bbp);
		size = game_resolution_rect.w*(bbp<<1<<2);

		memcpy(desttemp,srctemp,size);
		memcpy(desttemp+(m_Resolution.width<<bbp),srctemp,size);
		memcpy(desttemp+((m_Resolution.width<<bbp)<<1),srctemp,size);
	}
}

// "Console" here refers to the capability to pop up in-game messages
// in the upper-left corner during game play ala Doom.
void CVideoDriver::drawConsoleMessages(void)
{
	int i;
	int y;

	if (!NumConsoleMessages) return;
	if (!ConsoleExpireTimer)
	{
		NumConsoleMessages--;
		if (!NumConsoleMessages) return;
		ConsoleExpireTimer = CONSOLE_EXPIRE_RATE;
	}
	else ConsoleExpireTimer--;

	y = CONSOLE_MESSAGE_Y;
	for(i=0;i<NumConsoleMessages;i++)
	{
		g_pGfxEngine->getFont(0).drawFont( FGLayerSurface, cmsg[i].msg, CONSOLE_MESSAGE_X, y, true);
		y += CONSOLE_MESSAGE_SPACING;
	}
}

// removes all console messages
void CVideoDriver::DeleteConsoleMsgs(void)
{
	NumConsoleMessages = 0;
}

// adds a console msg to the top of the screen and scrolls any
// other existing messages downwards
void CVideoDriver::AddConsoleMsg(const char *the_msg)
{
	int i;
	for(i=MAX_CONSOLE_MESSAGES-2;i>=0;i--)
	{
		strcpy(cmsg[i+1].msg, cmsg[i].msg);
	}
	strcpy(cmsg[0].msg, the_msg);

	if (NumConsoleMessages < MAX_CONSOLE_MESSAGES) NumConsoleMessages++;
	ConsoleExpireTimer = CONSOLE_EXPIRE_RATE;
}

short CVideoDriver::getZoomValue(void){ return Zoom; }

void CVideoDriver::showFPS(bool value){ showfps = value; }

void CVideoDriver::isFullscreen(bool value) {
	Fullscreen = value;
}

short CVideoDriver::getFiltermode(void)
{
	if(m_ScaleXFilter < 1)
		m_ScaleXFilter = 1;
	return m_ScaleXFilter;
}

bool CVideoDriver::getFullscreen(void)
{	return Fullscreen;	}
unsigned int CVideoDriver::getWidth(void)
{	return m_Resolution.width;	}
unsigned int CVideoDriver::getHeight(void)
{	return m_Resolution.height;	}
unsigned short CVideoDriver::getDepth(void)
{	return m_Resolution.depth;	}
SDL_Surface *CVideoDriver::getScrollSurface(void)
{	return ScrollSurface; }

CVideoDriver::~CVideoDriver() {
 	stop();
}
