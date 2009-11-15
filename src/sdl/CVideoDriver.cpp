
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
#include "video/colourtable.h"
#include "../graphics/CGfxEngine.h"
#include "../scale2x/scalebit.h"
#include "../CLogFile.h"
#include "../FindFile.h"
#include <iostream>
#include <fstream>
using namespace std;

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
#ifdef WIZGP2X
	m_Resolution.width=320;
	m_Resolution.height=240;
	m_Resolution.depth=16;
	Mode=0;
	Fullscreen=true;
	Filtermode=0;
	Zoom=1;
	FrameSkip=2;
	m_targetfps = 50;	// Enable automatic frameskipping by default at 30
#else
	m_Resolution.width=640;
	m_Resolution.height=400;
	m_Resolution.depth=32;
	Mode=0;
	Fullscreen=false;
	Filtermode=1;
	Zoom=2;
	FrameSkip=2;
	m_targetfps = 60;
#endif
#ifdef USE_OPENGL
	m_opengl_filter = GL_NEAREST;
	mp_OpenGL = NULL;
	m_opengl = false; // Must stay optional for better compatibility
#endif
	m_aspect_correction = true;
	
	screenrect.x=0;
	screenrect.y=0;
	screenrect.h=0;
	screenrect.w=0;
	
	ScrollSurface=NULL;       // 512x512 scroll buffer
	FGLayerSurface=NULL;       // Scroll buffer for Messages
	BlitSurface=NULL;
	m_fading = false;
	
	m_scrollx_buf = m_scrolly_buf = 0;
	
	m_Resolution_pos = m_Resolutionlist.begin();
	
	
	initResolutionList();
}

CVideoDriver::~CVideoDriver() {
	stop();
}

void CVideoDriver::initResolutionList()
{
	st_resolution resolution;
	char buf[256];
	m_Resolutionlist.clear();
	int g,j;
	
	ifstream ResolutionFile; OpenGameFileR(ResolutionFile, "resolutions.cfg");
	if(!ResolutionFile)
	{
		g_pLogFile->textOut(PURPLE,"Warning: resolutions.cfg could not be read! Maybe your files weren't extracted correctly!<br>");
	}
	else
	{
		// Init SDL in order to check, if the resolutions are really supported
		if(SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			g_pLogFile->textOut(RED,"Could not initialize SDL for mode detection: %s<br>", SDL_GetError());
			return;
		}
		else
			g_pLogFile->textOut(GREEN,"SDL-Video was successfully initialized for mode detections!<br>");
		
		
		std::list<st_resolution> :: iterator i;
		
		/* Load the resolutions from the file. */
		while(!ResolutionFile.eof())
		{
			ResolutionFile.getline(buf,256);
			if(sscanf(buf,"%ix%i", &resolution.width,
					  &resolution.height) == 2)
				// Now check if it's possible to use this resolution
				resolution.depth = 32;
			resolution.depth = SDL_VideoModeOK(resolution.width, resolution.height,
											   resolution.depth, SDL_FULLSCREEN);
			
			if(resolution.depth)
			{
				for( i = m_Resolutionlist.begin() ; i != m_Resolutionlist.end() ; i++ )
					if(i->width == resolution.width &&
					   i->height == resolution.height &&
					   i->depth == resolution.depth) break;
				
				if(i == m_Resolutionlist.end())
					m_Resolutionlist.push_back(resolution);
			}
		}
		if(!getFullscreen())
		{
			for (g=1; g != 20; g++) {
				if (g*320>m_Resolutionlist.back().width or g*240>m_Resolutionlist.back().height)
				{
					j=g;
					break;
				}
			}
			m_Resolutionlist.clear();
			for (g=1; g!=j; g++) {
				resolution.width=g*320;
				resolution.height=g*240;
				resolution.depth=32;
				m_Resolutionlist.push_back(resolution);
			}
		}
		ResolutionFile.close();
		
		//SDL_Quit();
		
		// shutdown SDL, so the game can initialize it correctly
	}
	
	if(m_Resolutionlist.empty()) {
#ifdef WIZGP2X
		resolution.width = 320;
		resolution.height = 240;
		resolution.depth = 16;
		m_Resolutionlist.push_back(resolution);
#else
		resolution.width = 640;
		resolution.height = 480;
		resolution.depth = 32;
		m_Resolutionlist.push_back(resolution);
#endif
	}
	
	// will set the default mode; CSettings::loadDrvConfig will reset this if config file loaded successfully
#ifdef WIZGP2X
	setMode(320, 240, 16, 320, 240, 16);
#else
	setMode(m_Resolutionlist.front().width, m_Resolutionlist.front().height, m_Resolutionlist.front().depth);
#endif
}

st_resolution CVideoDriver::setNextResolution()
{
	m_Resolution_pos++;
	
	if(m_Resolution_pos == m_Resolutionlist.end())
		m_Resolution_pos = m_Resolutionlist.begin();
	
	return *m_Resolution_pos;
}

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
	
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0)
	{
		g_pLogFile->textOut(RED,"Could not initialize SDL: %s<br>", SDL_GetError());
		return false;
	}
	else
		g_pLogFile->textOut(GREEN,"SDL was successfully initialized!<br>");
	
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
		mp_OpenGL = new COpenGL();
		if(!(mp_OpenGL->initGL(m_Resolution.width, m_Resolution.height, m_Resolution.depth,
							   m_opengl_filter, Filtermode+1, m_aspect_correction)))
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

bool CVideoDriver::applyMode(void)
{
	// Check if some zoom/filter modes are illogical
	// TODO: Make this call clearer to understand
	// TODO: Improve this function.
	// It must be able to change the resolution, and if it fails, roll back.
	if( (Zoom == 3 && Filtermode == 1) && !m_opengl )
		Zoom = 2;
	
	m_Resolution = *m_Resolution_pos;
	
#ifdef WIZGP2X
#ifdef GP2X
    Mode = SDL_DOUBLEBUF | SDL_HWSURFACE;
#else
	Mode = SDL_SWSURFACE;
#endif
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
	
    game_resolution_rect.w = 320;
    if( m_Resolution.height % 240 == 0 )
    {
		game_resolution_rect.h = 240;
    }
    else
    {
		game_resolution_rect.h = 240;
    }
	
	// Now we decide if it will be fullscreen or windowed mode.
	if(Fullscreen)
		Mode |= SDL_FULLSCREEN;
	
	// Before the resolution is set, check, if the zoom factor is too high!
	while(((m_Resolution.width/game_resolution_rect.w) < Zoom || (m_Resolution.height/game_resolution_rect.h) < Zoom) && (Zoom > 1))
		Zoom--;
	
    // Try to center the screen!
	screenrect.w = blitrect.w = game_resolution_rect.w*Zoom;
	screenrect.h = blitrect.h = game_resolution_rect.h*Zoom;
	screenrect.x = (m_Resolution.width-screenrect.w)>>1;
	
	//if(m_Resolution.width == 320)
	screenrect.y = 0;
	//else
	//	screenrect.y = (m_Resolution.height-screenrect.h)>>1;
    blitrect.x = 0;
    blitrect.y = 0;
	
	// And Display can be setup.
	screen = SDL_SetVideoMode(m_Resolution.width,m_Resolution.height,m_Resolution.depth,Mode);
	
	if( !screen )
	{
		g_pLogFile->textOut(RED,"VidDrv_Start(): Couldn't create a SDL surface: %s<br>", SDL_GetError());
		return false;
	}
	
	m_Resolution.depth = screen->format->BitsPerPixel;
	
 	if(!Fullscreen)
		SDL_ShowCursor(SDL_ENABLE);
	else
		SDL_ShowCursor(SDL_DISABLE);
	
	return true;
}
void CVideoDriver::setFilter(short value)
{
	Filtermode = value;
}
void CVideoDriver::setZoom(short value)
{
	Zoom = value;
}

bool CVideoDriver::createSurfaces(void)
{
	unsigned stretch_blit_yoff = 0;
	
	// This function creates the surfaces which are needed for the game.
    ScrollSurface = createSurface( "ScrollSurface", true
								  , 512
								  , 512
								  , m_Resolution.depth
								  , Mode, screen->format );
	
    if (m_Resolution.width == 320 && !m_opengl)
    {
    	g_pLogFile->textOut("Blitsurface = Screen<br>");
    	BlitSurface = screen;
    	blitsurface_alloc = 0;
    }
    else
    {
    	g_pLogFile->textOut("Blitsurface = creatergbsurfacefrom<br>");
        BlitSurface = createSurface( "BlitSurface", true
									, game_resolution_rect.w
									, game_resolution_rect.h
									, m_Resolution.depth
									, Mode, screen->format );
		blitsurface_alloc = 1;
    }
    VRAMPtr = (unsigned char*)screen->pixels +
	((m_Resolution.width * stretch_blit_yoff * m_Resolution.depth)>>3)+
	screenrect.y*screen->pitch + (screenrect.x*m_Resolution.depth>>3);
	
    // Some surfaces could get 320x240 and the screenspace is extended.
    // The video class must be changed for any further resolutions
    game_resolution_rect.x = 0; game_resolution_rect.y = 0;
	
    FGLayerSurface = createSurface( "FGLayerSurface", false
								   , game_resolution_rect.w
								   , game_resolution_rect.h
								   , m_Resolution.depth
								   , Mode, screen->format );
	
	SDL_SetColorKey( FGLayerSurface, SDL_SRCCOLORKEY,
					SDL_MapRGB(FGLayerSurface->format, 0, 0xFF, 0xFE) );
	//Set surface alpha
	SDL_SetAlpha( FGLayerSurface, SDL_SRCALPHA, 225 );
	
    FXSurface = createSurface( "FXSurface", false
							  , game_resolution_rect.w
							  , game_resolution_rect.h
							  , m_Resolution.depth
							  , Mode, screen->format );
	
	g_pGfxEngine->Palette.setFXSurface( FXSurface );
	
	return true;
}

SDL_Surface* CVideoDriver::createSurface( std::string name, bool alpha, int width, int height, int bpp, int mode, SDL_PixelFormat* format )
{
    SDL_Surface *temporary, *optimized;
	
	temporary = SDL_CreateRGBSurface( mode, width, height, bpp, format->Rmask, format->Gmask, format->Bmask, format->Amask);
	if (alpha) {
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
	return optimized;
}

void CVideoDriver::blitScrollSurface(Sint16 sbufferx, Sint16 sbuffery) // This is only for tiles
// The name should be changed
{
	SDL_Rect srcrect;
	SDL_Rect dstrect;
	char wraphoz, wrapvrt;
	int save_dstx, save_dstw, save_srcx, save_srcw;
	
	dstrect.x = 0; dstrect.y = 0;
	dstrect.w = game_resolution_rect.w;
	dstrect.h = game_resolution_rect.h;
	
	srcrect.x = sbufferx;
	srcrect.y = sbuffery;
	
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

void CVideoDriver::update_screen(void)
{
	SDL_BlitSurface(FGLayerSurface, NULL, BlitSurface, NULL);
	
	if(FXSurface->format->alpha)
		SDL_BlitSurface(FXSurface, NULL, BlitSurface, NULL);
	
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
		if (Zoom == 1 && m_Resolution.width != 320 )
		{
			LockSurface(BlitSurface);
			LockSurface(screen);
			
			if(Filtermode == 0)
			{
				noscale((char*)VRAMPtr, (char*)BlitSurface->pixels, (m_Resolution.depth>>3));
			}
			else
			{
				g_pLogFile->textOut(PURPLE,"Sorry, but this filter doesn't work at that zoom mode<br>");
				g_pLogFile->textOut(PURPLE,"Try to use a higher zoom factor. Switching to no-filter<br>");
				Filtermode = 0;
			}
			UnlockSurface(screen);
			UnlockSurface(BlitSurface);
		}
		if (Zoom == 2)
		{
			LockSurface(BlitSurface);
			LockSurface(screen);
			
			if(Filtermode == 0)
			{
				scale2xnofilter((char*)VRAMPtr, (char*)BlitSurface->pixels, (m_Resolution.depth>>3));
			}
			else if(Filtermode == 1)
			{
				scale(2, VRAMPtr, m_Resolution.width*(m_Resolution.depth>>3), BlitSurface->pixels,
					  game_resolution_rect.w*(m_Resolution.depth>>3), (m_Resolution.depth>>3),
					  game_resolution_rect.w, game_resolution_rect.h);
			}
			else
			{
				g_pLogFile->textOut(PURPLE,"Sorry, but this filter doesn't work at that zoom mode<br>");
				g_pLogFile->textOut(PURPLE,"Try to use a higher zoom factor. Switching to no-filter<br>");
				Filtermode = 0;
			}
			
			UnlockSurface(screen);
			UnlockSurface(BlitSurface);
		}
		else if (Zoom == 3)
		{
			LockSurface(BlitSurface);
			LockSurface(screen);
			
			if(Filtermode == 0)
			{
				scale3xnofilter((char*)VRAMPtr, (char*)BlitSurface->pixels, (m_Resolution.depth>>3));
			}
			else if(Filtermode == 1)
			{
				scale(2, VRAMPtr, m_Resolution.width*(m_Resolution.depth>>3), BlitSurface->pixels,
					  game_resolution_rect.w*(m_Resolution.depth>>3), (m_Resolution.depth>>3), game_resolution_rect.w, game_resolution_rect.h);
			}
			else if(Filtermode == 2)
			{
				scale(3, VRAMPtr, m_Resolution.width*(m_Resolution.depth>>3), BlitSurface->pixels,
					  game_resolution_rect.w*(m_Resolution.depth>>3), (m_Resolution.depth>>3), game_resolution_rect.w, game_resolution_rect.h);
			}
			else
			{
				g_pLogFile->textOut(PURPLE,"Sorry, but this filter doesn't work at that zoom mode<br>");
				g_pLogFile->textOut(PURPLE,"Try to use a higher zoom factor. Switching to no-filter<br>");
				Filtermode = 0;
			}
			UnlockSurface(screen);
			UnlockSurface(BlitSurface);
		}
		else if (Zoom == 4)
		{
			LockSurface(BlitSurface);
			LockSurface(screen);
			
			if(Filtermode == 0)
			{
				scale4xnofilter((char*)VRAMPtr, (char*)BlitSurface->pixels, (m_Resolution.depth>>3));
			}
			else if(Filtermode == 1)
			{
				scale(2, VRAMPtr, m_Resolution.width*(m_Resolution.depth>>3), BlitSurface->pixels,
					  game_resolution_rect.w*(m_Resolution.depth>>3), (m_Resolution.depth>>3), game_resolution_rect.w, game_resolution_rect.h);
			}
			else if(Filtermode == 2)
			{
				scale(3, VRAMPtr, m_Resolution.width*(m_Resolution.depth>>3), BlitSurface->pixels,
					  game_resolution_rect.w*(m_Resolution.depth>>3), (m_Resolution.depth>>3), game_resolution_rect.w, game_resolution_rect.h);
			}
			else if(Filtermode == 3)
			{
				scale(4, VRAMPtr, m_Resolution.width*(m_Resolution.depth>>3), BlitSurface->pixels,
					  game_resolution_rect.w*(m_Resolution.depth>>3), (m_Resolution.depth>>3), game_resolution_rect.w, game_resolution_rect.h);
			}
			else
			{
				g_pLogFile->textOut(PURPLE,"Sorry, but this filter doesn't work at that zoom mode<br>");
				g_pLogFile->textOut(PURPLE,"Try to use a higher zoom factor. Switching to no-filter<br>");
				Filtermode = 0;
			}
			UnlockSurface(screen);
			UnlockSurface(BlitSurface);
		}
		
		SDL_Flip(screen);
		
		// Flush the FG-Layer
		SDL_FillRect(FGLayerSurface, NULL, SDL_MapRGB(FGLayerSurface->format, 0, 0xFF, 0xFE));
		
#ifdef USE_OPENGL
	}
#endif
}

void CVideoDriver::noscale(char *dest, char *src, short bbp)
{
	// just passes a blitsurface to the screen
	int i;
	for(i=0 ; i < g_pVideoDriver->getGameResRect().h ; i++)
		memcpy(dest+(i*m_Resolution.width)*bbp,src+(i*game_resolution_rect.w)*bbp,320*bbp);
}

void CVideoDriver::scale2xnofilter(char *dest, char *src, short bbp)
{
	// workaround for copying correctly stuff to the screen, so the screen is scaled normally
    // to 2x (without filter). This applies to 16 and 32-bit colour depth.
	// use bit shifting method for faster blit!
	bbp >>= 1;
	
	int i,j;
	for(i=0 ; i < g_pVideoDriver->getGameResRect().w ; i++)
	{
		for(j = 0; j < g_pVideoDriver->getGameResRect().w ; j++)
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
	for(i=0 ; i < g_pVideoDriver->getGameResRect().w ; i++)
	{
		for(j = 0; j < g_pVideoDriver->getGameResRect().h ; j++)
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
		g_pGfxEngine->Font->drawFont( FGLayerSurface, cmsg[i].msg, CONSOLE_MESSAGE_X, y, 1);
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

void CVideoDriver::isFullscreen(bool value)
{
	Fullscreen = value;
	return;
}

short CVideoDriver::getFiltermode(void)
{
	if(Filtermode < 0)
		Filtermode = 0;
	return Filtermode;
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


