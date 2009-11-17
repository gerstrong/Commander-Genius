
/*
 * CVideoDriver.cpp
 *
 *  Created on: 17.03.2009
 *      Author: gerstrong
 */
#include "CVideoDriver.h"
#include "CInput.h"

#include "../keen.h"
#include "video/colourconvert.h"
#include "video/colourtable.h"
#include "../scale2x/scalebit.h"
#include "../CLogFile.h"
#include "../CGraphics.h"
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

#define GAME_STD_WIDTH            320
#define GAME_STD_HEIGHT           200

// pointer to the line in VRAM to start blitting to when stretchblitting.
// this may not be the first line on the display as it is adjusted to
// center the image on the screen when in fullscreen.
unsigned char *VRAMPtr;
char blitsurface_alloc = 0;

SDL_Rect dstrect;

typedef struct stConsoleMessage
{
  char msg[80];
} stConsoleMessage;
stConsoleMessage cmsg[MAX_CONSOLE_MESSAGES];
int NumConsoleMessages = 0;
int ConsoleExpireTimer = 0;


void CVideoDriver::resetSettings() {
	// Default values
	
	m_updateFG = false;
	
	showfps=true;
#ifdef WIZ
	m_Resolution.width=320;
	m_Resolution.height=240;
	m_Resolution.depth=16;
	Mode=0;
	Fullscreen=true;
	Filtermode=0;
	Zoom=1;
	FrameSkip=0;
	m_targetfps = 30;	// Enable automatic frameskipping by default at 30
#else
	m_Resolution.width=640;
	m_Resolution.height=480;
	m_Resolution.depth=0;
	Mode=0;
	Fullscreen=false;
	Filtermode=1;
	Zoom=2;
	FrameSkip=2;
	m_targetfps = 50;
#endif
	m_opengl = false;
#ifdef USE_OPENGL
	m_opengl_filter = GL_NEAREST;
	mp_OpenGL = NULL;
	m_opengl = true; // use it if possible
#endif
	m_aspect_correction = true;
	
	screenrect.x=0;
	screenrect.y=0;
	screenrect.h=0;
	screenrect.w=0;
	
	ScrollSurface=NULL;       // 512x512 scroll buffer
	FGLayerSurface=NULL;       // Scroll buffer for Messages
	BGLayerSurface=NULL;
	BlitSurface=NULL;
	
#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)	
	Zoom = 1;
	Filtermode = 0;
	FrameSkip=1;
	m_targetfps = 30;
	m_aspect_correction = false;
#endif	
	
	m_Resolution_pos = m_Resolutionlist.begin();
	initResolutionList();
}

CVideoDriver::CVideoDriver() {
	resetSettings();
}

CVideoDriver::~CVideoDriver() {
	stop();
}

void CVideoDriver::initResolutionList()
{
#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
	m_Resolution.width = 320; //  320;
	m_Resolution.height = 200; //  480;
	m_Resolution.depth = 32;
	m_Resolutionlist.push_back(m_Resolution);
	m_Resolution_pos = m_Resolutionlist.begin();
	
	return;
#endif	
	
	  st_resolution resolution;
	  char buf[256];

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
		  while(!ResolutionFile.eof())
		  {
			  ResolutionFile.getline(buf,256);
			  if(sscanf(buf,"%hdx%hdx%hd", &resolution.width,
									  &resolution.height,
									  &resolution.depth) == 3)
				  // Now check if it's possible to use this resolution
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
		  ResolutionFile.close();

		  SDL_Quit();

		  // shutdown SDL, so the game can initialize it correctly
	  }
	
	if(m_Resolutionlist.empty()) {
		resolution.width = 640;
		resolution.height = 480;
		resolution.depth = 32;
		m_Resolutionlist.push_back(resolution);
		
		resolution.depth = 16;
		m_Resolutionlist.push_back(resolution);
	}
	
	// will set the default mode; CSettings::loadDrvConfig will reset this if config file loaded successfully
	setMode(640, 480, 32);
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
	if(screen) { SDL_FreeSurface(screen); g_pLogFile->textOut("freed screen<br>"); screen = NULL; }
	if(ScrollSurface && (ScrollSurface->map != NULL)) { SDL_FreeSurface(ScrollSurface); g_pLogFile->textOut("freed scrollsurface<br>"); ScrollSurface = NULL; }
	if(blitsurface_alloc) { blitsurface_alloc = 0; SDL_FreeSurface(BlitSurface); g_pLogFile->textOut("freed blitsurface<br>"); BlitSurface=NULL; }
#ifdef USE_OPENGL
	if(mp_OpenGL) { delete mp_OpenGL; mp_OpenGL = NULL; }
#endif
	g_pLogFile->textOut(GREEN,"CVideoDriver Close%s<br>", SDL_GetError());
}

#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
extern "C" void iPhoneRotateScreen();
#endif

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

	  if(!applyMode())
	  {
		  g_pLogFile->textOut(RED,"VideoDriver: Error applying mode! Your Videocard doesn't seem to work on CKP<br>");
		  g_pLogFile->textOut(RED,"Check, if you have the most recent drivers installed!<br>");
		  return false;
	  }

#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
	iPhoneRotateScreen();
#endif
	
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
#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
	// Force the default settings on iPhone.
	// There is no reason yet to play with it, most likely other settings will
	// either not work, will crash or will just be totally screwed up.
	resetSettings();
#endif
	
	// Check if some zoom/filter modes are illogical
	// TODO: Make this call clearer to understand
	// TODO: Improve this function.
	// It must be able to change the resolution, and if it fails, roll back.
	if( (Zoom == 3 && Filtermode == 1) && !m_opengl )
		Zoom = 2;

	// Grab a surface on the screen
	Mode = SDL_HWPALETTE | SDL_HWSURFACE;

	m_Resolution = *m_Resolution_pos;

#ifndef WIZ
	// Support for doublebuffering
	Mode |= SDL_DOUBLEBUF;
#endif

	// Enable OpenGL
#ifdef USE_OPENGL
	if(m_opengl)
	{
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		// TODO: unknown in SDL 1.3. important?
//		SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
		Mode |= SDL_OPENGL;
	}
#endif

	// Now we decide if it will be fullscreen or windowed mode.
	if(Fullscreen)
		Mode |= SDL_FULLSCREEN;

	// Before the resolution is set, check, if the zoom factor is too high!
	while(((m_Resolution.width/GAME_STD_WIDTH) < Zoom || (m_Resolution.height/GAME_STD_HEIGHT) < Zoom) && (Zoom > 1))
		Zoom--;

    // Try to center the screen!
	screenrect.w = blitrect.w = GAME_STD_WIDTH*Zoom;
	screenrect.h = blitrect.h = GAME_STD_HEIGHT*Zoom;
	screenrect.x = (m_Resolution.width-screenrect.w)>>1;

	if(m_Resolution.width == 320)
		screenrect.y = 0;
	else
		screenrect.y = (m_Resolution.height-screenrect.h)>>1;
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
void CVideoDriver::setFrameskip(unsigned short value)
{
	FrameSkip = value;
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
	static const Uint32 RGBA[] = {
#if SDL_BYTEORDER == SDL_LIL_ENDIAN // OpenGL RGBA masks 
	0x000000FF, 
	0x0000FF00, 
	0x00FF0000, 
	0xFF000000
#else
	0xFF000000,
	0x00FF0000, 
	0x0000FF00, 
	0x000000FF
#endif
	};
	
	// This function creates the surfaces which are needed for the game.
	unsigned stretch_blit_yoff;

	stretch_blit_yoff = 0;

	ScrollSurface = SDL_CreateRGBSurfaceFrom(g_pGraphics->getScrollbuffer(), 512, 512, 8, 512, screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
	SDL_SetColorKey(ScrollSurface, SDL_SRCCOLORKEY, COLOUR_MASK);
	if (!ScrollSurface)
	{
		g_pLogFile->textOut(RED,"VideoDriver: Couldn't create ScrollSurface!<br>");
	  return false;
	}

	BGLayerSurface = SDL_CreateRGBSurface(SDL_SWSURFACE/*Mode*/,512, 256, 24, 0x0000ff, 0x00ff00, 0xff0000, 0x0);
	//BGLayerSurface = SDL_CreateRGBSurface(SDL_SWSURFACE/*Mode*/,320, 200, m_Resolution.depth,  screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
	if (!BGLayerSurface)
	{
		g_pLogFile->textOut(RED,"VideoDriver: Couldn't create BGLayerSurface!<br>");
	  return false;
	}

	FGLayerSurface = SDL_CreateRGBSurface(SDL_SWSURFACE/*Mode*/,512, 256, 32, RGBA[0], RGBA[1], RGBA[2], RGBA[3]);
	//FGLayerSurface = SDL_CreateRGBSurface(SDL_SWSURFACE/*Mode*/,320, 200, m_Resolution.depth,  screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
	if (!FGLayerSurface)
	{
		g_pLogFile->textOut(RED,"VideoDriver: Couldn't create FGLayerSurface!<br>");
	  return false;
	}
	SDL_SetColorKey( FGLayerSurface, SDL_SRCCOLORKEY,
					SDL_MapRGB(FGLayerSurface->format, 0, 0, 0) );

	//Set surface alpha
	SDL_SetAlpha( FGLayerSurface, SDL_SRCALPHA, 225 );

	SDL_Rect r = {0, 0, 512, 256};
	SDL_FillRect(FGLayerSurface, &r, SDL_MapRGBA(FGLayerSurface->format, 0, 0, 0, 0));
	
	
    if(m_Resolution.width == 320 && !m_opengl)
    {
    	g_pLogFile->textOut("Blitsurface = Screen<br>");
    	BlitSurface = screen;
    	blitsurface_alloc = 0;
    	VRAMPtr = (unsigned char*)screen->pixels +
    			((m_Resolution.width * stretch_blit_yoff * m_Resolution.depth)>>3)+
    			screenrect.y*screen->pitch + (screenrect.x*m_Resolution.depth>>3);
    }
    else
    {
    	g_pLogFile->textOut("Blitsurface = creatergbsurfacefrom<br>");
#ifdef USE_OPENGL
		// We must create a surface with the size being a power of two.
		// Also, we will use a handy format.
    	BlitSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, 512, 256, 24, 0x0000ff, 0x00ff00, 0xff0000, 0x0);
#else
		BlitSurface = SDL_CreateRGBSurface(SDL_SWSURFACE /*Mode & ~SDL_DOUBLEBUF & ~SDL_HWPALETTE & ~SDL_HWSURFACE*/,GAME_STD_WIDTH, GAME_STD_HEIGHT, m_Resolution.depth,  screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
#endif
		
		if (!BlitSurface)
		{
			g_pLogFile->textOut(RED,"VidDrv_Start(): Couldn't create BlitSurface!<br>");
			return false;
		}
		blitsurface_alloc = 1;
		VRAMPtr = (unsigned char*)screen->pixels + ((m_Resolution.width * stretch_blit_yoff * m_Resolution.depth)>>3)+screenrect.y*screen->pitch + (screenrect.x*m_Resolution.depth>>3);
    }

    dstrect.x = 0; dstrect.y = 0;
	dstrect.w = GAME_STD_WIDTH;
	dstrect.h = GAME_STD_HEIGHT;

	return true;
}

// alter the color palette. the palette is not actually altered
// on-screen until pal_apply() is called.
void CVideoDriver::pal_set(short colour, char red, char green, char blue)
{
  MyPalette[colour].r = red;
  MyPalette[colour].g = green;
  MyPalette[colour].b = blue;
}

// applies all changes to the palette made with pal_set
void CVideoDriver::pal_apply(void)
{
  SDL_SetColors(screen, (SDL_Color *) &MyPalette, 0, 256);
  SDL_SetColors(ScrollSurface, (SDL_Color *) &MyPalette, 0, 256);
  if (blitsurface_alloc)
  {
    SDL_SetColors(BlitSurface, (SDL_Color *) &MyPalette, 0, 256);
  }
}

static void sb_lowblit(SDL_Surface* src, SDL_Rect* srcrect, SDL_Surface* dst, SDL_Rect* dstrect) {
#if !defined(USE_OPENGL) && (defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR))
	// SDL_BlitSurface doesn't work for some reason
	int _xend = srcrect->x + dstrect->w;
	int _yend = srcrect->y + dstrect->h;
	int _dst_xdiff = dstrect->x - srcrect->x;
	int _dst_ydiff = dstrect->y - srcrect->y;
	for(int x = srcrect->x; x < _xend; ++x)
		for(int y = srcrect->y; y < _yend; ++y)
			SDL_DrawPoint(dst, x + _dst_xdiff, y + _dst_ydiff, convert4to32BPPcolor( ((Uint8*)src->pixels)[y * 512 + x], dst ));
#else
	SDL_BlitSurface(src, srcrect, dst, dstrect);
#endif
}

void CVideoDriver::sb_blit(void)
{
SDL_Rect srcrect;
char wraphoz, wrapvrt;
int save_dstx, save_dstw, save_srcx, save_srcw;
char tempbuf[80];

   dstrect.x = 0; dstrect.y = 0;
   dstrect.w = 320; dstrect.h = 200;

   srcrect.x = scrollx_buf;
   srcrect.y = scrolly_buf;

   blitBGLayer();

   if (scrollx_buf > (512-320))
   { // need to wrap right side
     srcrect.w = (512-scrollx_buf);
     wraphoz = 1;
   }
   else
   { // single blit for whole horizontal copy
     srcrect.w = 320;
     wraphoz = 0;
   }

   if (scrolly_buf > (512-200))
   { // need to wrap on bottom
     srcrect.h = (512-scrolly_buf);
     wrapvrt = 1;
   }
   else
   { // single blit for whole bottom copy
     srcrect.h = 200;
     wrapvrt = 0;
   }

   sb_lowblit(ScrollSurface, &srcrect, BlitSurface, &dstrect);
	
   if (wraphoz && wrapvrt)
   {
      // first do same thing we do for wraphoz
      save_dstx = dstrect.x;
      save_dstw = dstrect.w;
      save_srcx = srcrect.x;
      save_srcw = srcrect.w;
      dstrect.x = srcrect.w;
      dstrect.w = 320 - dstrect.x;
      srcrect.x = 0;
      srcrect.w = (320 - srcrect.w);
      sb_lowblit(ScrollSurface, &srcrect, BlitSurface, &dstrect);

      // now repeat for the bottom
      // (lower-right square)
      dstrect.y = srcrect.h;
      dstrect.h = 200 - dstrect.y;
      srcrect.y = 0;
      srcrect.h = (200 - srcrect.h);
      sb_lowblit(ScrollSurface, &srcrect, BlitSurface, &dstrect);
      // (lower-left square)
      dstrect.x = save_dstx;
      dstrect.w = save_dstw;
      srcrect.x = save_srcx;
      srcrect.w = save_srcw;
      sb_lowblit(ScrollSurface, &srcrect, BlitSurface, &dstrect);
   }
   else if (wraphoz)
   {
      dstrect.x = srcrect.w;
      dstrect.w = 320 - dstrect.x;
      srcrect.x = 0;
      srcrect.w = (320 - srcrect.w);
      sb_lowblit(ScrollSurface, &srcrect, BlitSurface, &dstrect);
   }
   else if (wrapvrt)
   {
      dstrect.y = srcrect.h;
      dstrect.h = 200 - dstrect.y;
      srcrect.y = 0;
      srcrect.h = (200 - srcrect.h);
      sb_lowblit(ScrollSurface, &srcrect, BlitSurface, &dstrect);
   }

   drawConsoleMessages();
   if (showfps)
   {

#ifdef DEBUG
     sprintf(tempbuf, "FPS: %03d; x = %ld ; y = %d", fps, player[0].x >>CSF, player[0].y >>CSF);
	   g_pGraphics->drawFont( tempbuf, 320-3-(strlen( (char *) tempbuf)<<3), 3, 1);

#else
	   // Note: commented this out, probably only for devs important
	   // If there is request for it, we can make an option to enable this even in release.
	   // But the majority probably want to have that disabled.
     //sprintf(tempbuf, "FPS: %03d", fps);
#endif
   }

   update_screen();
}
void CVideoDriver::blitBGLayer(void)
{
#ifndef USE_OPENGL	
	SDL_BlitSurface(BGLayerSurface, NULL, BlitSurface, NULL);
#endif
}

void CVideoDriver::update_screen(void)
{
#ifdef USE_OPENGL
   if(m_opengl)
   {
	   if(m_updateFG)
		   mp_OpenGL->reloadFG(FGLayerSurface);
	   
	   mp_OpenGL->render(m_updateFG);

	   if(m_updateFG) {
		   // Flush the layers
		   SDL_Rect r = {0, 0, 512, 256};
		   SDL_FillRect(FGLayerSurface, &r, SDL_MapRGBA(FGLayerSurface->format, 0, 0, 0, 0));

		   m_updateFG = false;
	   }
   }
   else // No OpenGL but Software Rendering
#endif
   {
	   SDL_BlitSurface(FGLayerSurface, NULL, BlitSurface, NULL);

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
					   GAME_STD_WIDTH*(m_Resolution.depth>>3), (m_Resolution.depth>>3), GAME_STD_WIDTH, GAME_STD_HEIGHT);
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
					   GAME_STD_WIDTH*(m_Resolution.depth>>3), (m_Resolution.depth>>3), GAME_STD_WIDTH, GAME_STD_HEIGHT);
		   }
		   else if(Filtermode == 2)
		   {
			   scale(3, VRAMPtr, m_Resolution.width*(m_Resolution.depth>>3), BlitSurface->pixels,
					   GAME_STD_WIDTH*(m_Resolution.depth>>3), (m_Resolution.depth>>3), GAME_STD_WIDTH, GAME_STD_HEIGHT);
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
					   GAME_STD_WIDTH*(m_Resolution.depth>>3), (m_Resolution.depth>>3), GAME_STD_WIDTH, GAME_STD_HEIGHT);
		   }
		   else if(Filtermode == 2)
		   {
			   scale(3, VRAMPtr, m_Resolution.width*(m_Resolution.depth>>3), BlitSurface->pixels,
					   GAME_STD_WIDTH*(m_Resolution.depth>>3), (m_Resolution.depth>>3), GAME_STD_WIDTH, GAME_STD_HEIGHT);
		   }
		   else if(Filtermode == 3)
		   {
			   scale(4, VRAMPtr, m_Resolution.width*(m_Resolution.depth>>3), BlitSurface->pixels,
					   GAME_STD_WIDTH*(m_Resolution.depth>>3), (m_Resolution.depth>>3), GAME_STD_WIDTH, GAME_STD_HEIGHT);
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
	   //SDL_UpdateRect(screen, screenrect.x, screenrect.y, screenrect.w, screenrect.h);

	   LockSurface(FGLayerSurface);
	   // Flush the layers
	   memset(FGLayerSurface->pixels,SDL_MapRGB(FGLayerSurface->format, 0, 0, 0),
			   GAME_STD_WIDTH*GAME_STD_HEIGHT*FGLayerSurface->format->BytesPerPixel);
	   UnlockSurface(FGLayerSurface);
   }
}

void CVideoDriver::noscale(char *dest, char *src, short bbp)
{
	// just passes a blitsurface to the screen
	int i;
	for(i=0 ; i < 200 ; i++)
		memcpy(dest+(i*m_Resolution.width)*bbp,src+(i*GAME_STD_WIDTH)*bbp,320*bbp);
}

void CVideoDriver::scale2xnofilter(char *dest, char *src, short bbp)
{
	// workaround for copying correctly stuff to the screen, so the screen is scaled normally
    // to 2x (without filter). This applies to 16 and 32-bit colour depth.
	// use bit shifting method for faster blit!
	bbp >>= 1;

	int i,j;
	for(i=0 ; i < 200 ; i++)
	{
		for(j = 0; j < 320 ; j++)
		{
			memcpy(dest+((j<<1)<<bbp)+(((i<<1)*m_Resolution.width)<<bbp),src+(j<<bbp)+((i*GAME_STD_WIDTH)<<bbp),bbp<<1);
			memcpy(dest+(((j<<1)+1)<<bbp)+(((i<<1)*m_Resolution.width)<<bbp),src+(j<<bbp)+((i*GAME_STD_WIDTH)<<bbp),bbp<<1);
		}
		memcpy(dest+(((i<<1)+1)*(m_Resolution.width<<bbp)),(dest+(i<<1)*(m_Resolution.width<<bbp)),(bbp<<2)*GAME_STD_WIDTH);
	}
}

void CVideoDriver::scale3xnofilter(char *dest, char *src, short bbp)
{
	// workaround for copying correctly stuff to the screen, so the screen is scaled normally
    // to 2x (without filter). This applies to 16 and 32-bit colour depth.
	// Optimization of using bit shifting
	bbp >>= 1;

	int i,j;
	for(i=0 ; i < 200 ; i++)
	{
		for(j = 0; j < 320 ; j++)
		{
			// j*3 = (j<<1) + j
			memcpy(dest+(((j<<1)+j)<<bbp)+((((i<<1) + i)*m_Resolution.width)<<bbp),src+(j<<bbp)+((i*GAME_STD_WIDTH)<<bbp),bbp<<1);
			memcpy(dest+(((j<<1)+j+1)<<bbp)+((((i<<1) + i)*m_Resolution.width)<<bbp),src+(j<<bbp)+((i*GAME_STD_WIDTH)<<bbp),bbp<<1);
			memcpy(dest+(((j<<1)+j+2)<<bbp)+((((i<<1) + i)*m_Resolution.width)<<bbp),src+(j<<bbp)+((i*GAME_STD_WIDTH)<<bbp),bbp<<1);
		}
		memcpy(dest+((i<<1)+i+1)*(m_Resolution.width<<bbp),dest+((i<<1)+i)*(m_Resolution.width<<bbp),(3<<bbp)*GAME_STD_WIDTH);
		memcpy(dest+((i<<1)+i+2)*(m_Resolution.width<<bbp),dest+((i<<1)+i)*(m_Resolution.width<<bbp),(3<<bbp)*GAME_STD_WIDTH);
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
	for(i=0 ; i < GAME_STD_HEIGHT ; i++)
	{
		for(j = 0; j < GAME_STD_WIDTH ; j++)
		{
			// j*4 = (j<<2)
			srctemp = src+((j+(i*GAME_STD_WIDTH))<<bbp);
			desttemp = dest+((4*(j+(i*m_Resolution.width)))<<bbp);
			memcpy(desttemp,srctemp,bbp<<1);
			memcpy(desttemp+(1<<bbp),srctemp,bbp<<1);
			memcpy(desttemp+(2<<bbp),srctemp,bbp<<1);
			memcpy(desttemp+(3<<bbp),srctemp,bbp<<1);
		}
		srctemp = dest+(((i<<2)*m_Resolution.width)<<bbp);
		desttemp = dest+((((i<<2)+1)*m_Resolution.width)<<bbp);
		size = GAME_STD_WIDTH*(bbp<<1<<2);

		memcpy(desttemp,srctemp,size);
		memcpy(desttemp+(m_Resolution.width<<bbp),srctemp,size);
		memcpy(desttemp+((m_Resolution.width<<bbp)<<1),srctemp,size);
	}
}

// functions to directly set and retrieve pixels from the VGA display
void CVideoDriver::setpixel(unsigned int x, unsigned int y, unsigned char c)
{
	if( x >= GAME_STD_WIDTH || y >= GAME_STD_HEIGHT ) // out of Bonds!!!
		return;

    if(FGLayerSurface->format->BitsPerPixel == 16)
    {
    	Uint16 *ubuff16;
        ubuff16 = (Uint16*) FGLayerSurface->pixels;
    	ubuff16 += (y * FGLayerSurface->pitch/FGLayerSurface->format->BytesPerPixel) + x;
    	*ubuff16 = convert4to16BPPcolor(c, FGLayerSurface);
    }
    else if(FGLayerSurface->format->BitsPerPixel == 32)
    {
    	Uint32 *ubuff32;
        ubuff32 = (Uint32*) FGLayerSurface->pixels;
    	ubuff32 += (y * FGLayerSurface->pitch/FGLayerSurface->format->BytesPerPixel) + x;
    	*ubuff32 = convert4to32BPPcolor(c, FGLayerSurface);
    }
    else
    { // 8bit surface assumed
    	Uint8 *ubuff8;
        ubuff8 = (Uint8*) FGLayerSurface->pixels;
    	ubuff8 += (y * FGLayerSurface->pitch) + x;
    	*ubuff8 = (Uint8) c;
    }
	
	m_updateFG = true;
}
unsigned char CVideoDriver::getpixel(int x, int y)
{
	return 15;
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
	 g_pGraphics->drawFont( cmsg[i].msg, CONSOLE_MESSAGE_X, y, 1);
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

unsigned short CVideoDriver::getFrameskip(void)
{
	return FrameSkip;
}
bool CVideoDriver::getShowFPS(void)
{
	return showfps;
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
SDL_Surface *CVideoDriver::getBGLayerSurface(void)
{	return BGLayerSurface; }

void CVideoDriver::updateBG() {
#ifdef USE_OPENGL
	if(m_opengl)
		mp_OpenGL->reloadBG(BGLayerSurface);
#endif
}

