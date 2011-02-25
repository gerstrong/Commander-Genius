
/*
 * CVideoDriver.cpp
 *
 *  Created on: 17.03.2009
 *      Author: gerstrong
 */
#include "CVideoDriver.h"
#include "CInput.h"
#include "CTimer.h"

#include "video/CSDLVideo.h"
#include "video/COpenGL.h"

#include "keen.h"
#include "graphics/CGfxEngine.h"
#include "scale2x/scalebit.h"
#include "CLogFile.h"
#include "FindFile.h"
#include "graphics/PerSurfaceAlpha.h"
#include <iostream>
#include <fstream>

#define CKLOGFILENAME            	"genius.log"

#define MAX_CONSOLE_MESSAGES     	3
#define CONSOLE_MESSAGE_X        	3
#define CONSOLE_MESSAGE_Y        	3
#define CONSOLE_MESSAGE_SPACING  	9
#define CONSOLE_EXPIRE_RATE      	250

typedef struct stConsoleMessage
{
	char msg[80];
} stConsoleMessage;
stConsoleMessage cmsg[MAX_CONSOLE_MESSAGES];
int NumConsoleMessages = 0;
int ConsoleExpireTimer = 0;


CVideoDriver::CVideoDriver() :
mp_VideoEngine(NULL),
m_mustrefresh(false),
mp_sbufferx(NULL),
mp_sbuffery(NULL)
{
	resetSettings();
}

void CVideoDriver::resetSettings() {

	m_VidConfig.reset();

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0)
		g_pLogFile->textOut(RED,"Could not initialize SDL: %s<br>", SDL_GetError());
	else
		g_pLogFile->textOut(GREEN,"SDL was successfully initialized!<br>");

	initResolutionList();

	// take the first default resolution. It might be changed if there is a config file already created
	setMode(m_Resolutionlist.front().width, m_Resolutionlist.front().height, m_Resolutionlist.front().depth);
}

// initResolutionList() reads the local list of available resolution.
// This function can only be called internally
void CVideoDriver::initResolutionList()
{
	st_resolution resolution;
	m_Resolutionlist.clear();

#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
	m_Resolution.width = 320; //  320;
	m_Resolution.height = 200; //  480;
	m_Resolution.depth = 32;
	m_Resolutionlist.clear();
	m_Resolutionlist.push_back(m_Resolution);
#elif defined(ANDROID)
	m_Resolution.width = 320;
	m_Resolution.height = 200;
	m_Resolution.depth = 16;
	m_Resolutionlist.clear();
	m_Resolutionlist.push_back(m_Resolution);
#else
	char buf[256];

	std::ifstream ResolutionFile; OpenGameFileR(ResolutionFile, "resolutions.cfg");
	if(!ResolutionFile)
	{
		g_pLogFile->textOut(PURPLE,"Warning: resolutions.cfg could not be read! Maybe your files weren't extracted correctly!<br>");
		g_pLogFile->textOut(PURPLE,"Using default resolution...<br>");
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

		if(!m_VidConfig.Fullscreen)
		{
			int e = 1;
			resolution.width = 320;
			resolution.height = 200;
#if defined(WIZ)
			resolution.depth = 16;
#else
			resolution.depth = 32;
#endif

			int maxwidth = SDL_GetVideoInfo()->current_w;

			while(resolution.width < maxwidth)
			{
				resolution.width = 320 * e;
				resolution.height = 200 * e;

				// Now check if it's possible to use this resolution
				checkResolution( resolution, 0 );
				e++;
			}
		}
	}
#endif

	if(m_Resolutionlist.empty()) {
		resolution.width = 320;
		resolution.height = 200;
		resolution.depth = 32;
		m_Resolutionlist.push_back(resolution);
	}

	m_Resolution_pos = m_Resolutionlist.begin();
}

void CVideoDriver::checkResolution( st_resolution& resolution, int flags )
{
	resolution.depth = SDL_VideoModeOK( resolution.width, resolution.height, resolution.depth, flags );

	if(resolution.depth)
	{
		std::list<st_resolution> :: iterator i;
		for( i = m_Resolutionlist.begin() ; i != m_Resolutionlist.end() ; i++ )
		{
			if(*i == resolution)
				break;
		}

		if(i == m_Resolutionlist.end())
		{
#ifdef DEBUG
			g_pLogFile->ftextOut(BLUE, "Resolution %ix%ix%i %X added\n", resolution.width, resolution.height, resolution.depth);
#endif
			m_Resolutionlist.push_back(resolution);
		}
	}
}

void CVideoDriver::setVidConfig(const CVidConfig& VidConf)
{
	m_VidConfig = VidConf;
	setMode(m_VidConfig.m_Resolution);
}

void CVideoDriver::setSpecialFXMode(bool SpecialFX)
{	m_VidConfig.m_special_fx = SpecialFX;	}

void CVideoDriver::setMode(int width, int height,int depth)
{
	st_resolution res(width, height, depth);
	setMode(res);
}

void CVideoDriver::setMode(st_resolution &res)
{
	m_VidConfig.setResolution(res);

	// TODO: Cycle through the list until the matching resolution is matched. If it doesn't exist
	// add it;
	for(m_Resolution_pos = m_Resolutionlist.begin() ; m_Resolution_pos != m_Resolutionlist.end() ; m_Resolution_pos++)
		if( *m_Resolution_pos == res )
			break;

	if(m_Resolution_pos == m_Resolutionlist.end())
	{
		m_Resolutionlist.push_back(res);
		m_Resolution_pos--;
	}
}

#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
extern "C" void iPhoneRotateScreen();
#endif

bool CVideoDriver::applyMode()
{
	const st_resolution &Res = m_VidConfig.m_Resolution;
	const SDL_Rect &GameRect = m_VidConfig.m_Gamescreen;

	// Before the resolution is set, check, if the zoom factor is too high!
	while(((Res.width/GameRect.w) < m_VidConfig.Zoom || (Res.height/GameRect.h) < m_VidConfig.Zoom) && (m_VidConfig.Zoom > 1))
		m_VidConfig.Zoom--;

#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR) || defined(ANDROID)
	// Force the default settings on iPhone.
	// There is no reason yet to play with it, most likely other settings will
	// either not work, will crash or will just be totally screwed up.
	resetSettings();
#endif

	// Check if some zoom/filter modes are illogical
	// TODO: Make this call better to understand
	// It must be able to change the resolution, and if it fails, roll back.

	if( (m_VidConfig.Zoom == 3 && m_VidConfig.m_ScaleXFilter == 1) && !m_VidConfig.m_opengl )
		m_VidConfig.Zoom = 2;

	if( m_VidConfig.Zoom == 0 )
		m_VidConfig.Zoom = 1;

	m_VidConfig.m_Resolution = *m_Resolution_pos;
	return true;
}

bool CVideoDriver::start()
{
	bool retval;
	std::string caption = "Commander Genius (CKP)";
	SDL_WM_SetCaption(caption.c_str(), caption.c_str());
	// When the program is through executing, call SDL_Quit
	atexit(SDL_Quit);

#ifdef USE_OPENGL
	if(m_VidConfig.m_opengl) // If OpenGL could be set, initialize the matrices
	{
		mp_VideoEngine = new COpenGL(m_VidConfig, mp_sbufferx, mp_sbuffery);
		retval = mp_VideoEngine->init();

		if(!retval)
		{
			delete mp_VideoEngine;
			m_VidConfig.m_opengl = false;
			applyMode();
			mp_VideoEngine = new CSDLVideo(m_VidConfig, mp_sbufferx, mp_sbuffery);
			retval = mp_VideoEngine->init();
		}
	}
	else
	{
#endif
		mp_VideoEngine = new CSDLVideo(m_VidConfig, mp_sbufferx, mp_sbuffery);
		retval = mp_VideoEngine->init();

#ifdef USE_OPENGL
	}
#endif

#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
	iPhoneRotateScreen();
#endif

	// Now SDL will tell if the bpp works or changes it, if not supported.
	// this value is updated here!
	m_VidConfig.m_Resolution.depth = mp_VideoEngine->getScreenSurface()->format->BitsPerPixel;
	retval &= mp_VideoEngine->createSurfaces();
	m_mustrefresh = true;

	return retval;
}

void CVideoDriver::setFilter(short value) { m_VidConfig.m_ScaleXFilter = value; } // 1 means no filter of course

void CVideoDriver::setZoom(short value) { m_VidConfig.Zoom = value; }


// defines the scroll-buffer that is used for blitScrollSurface(). It's normally passed by a CMap Object
// It might have when a level-map is loaded.
void CVideoDriver::setScrollBuffer(Sint16 *pbufx, Sint16 *pbufy)
{
	mp_sbufferx = pbufx;
	mp_sbuffery = pbufy;
}
void CVideoDriver::blitScrollSurface() // This is only for tiles
									   // Therefore the name should be changed
{
	mp_VideoEngine->blitScrollSurface();
	drawConsoleMessages();
}

void CVideoDriver::collectSurfaces()
{
	mp_VideoEngine->collectSurfaces();
}

void CVideoDriver::updateScreen()
{
	mp_VideoEngine->updateScreen();
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
		g_pGfxEngine->getFont(0).drawFont( mp_VideoEngine->getFGLayerSurface(), cmsg[i].msg, CONSOLE_MESSAGE_X, y, true);
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

void CVideoDriver::saveCameraBounds(st_camera_bounds &CameraBounds)
{
	int i;

	int &left = CameraBounds.left;
	int &up = CameraBounds.up;
	int &right = CameraBounds.right;
	int &down = CameraBounds.down;
	int &speed = CameraBounds.speed;
	if(left>right)
	{
		i = left-right;
		i = i/2;
		left = left-i;
		right = right+i;
		if(left>right)
			left = left-1;
	}
	if(up>down)
	{
		i = up-down;
		i = i/2;
		up = up-i;
		down = down+i;
		if(up>down)
			up = up-1;
	}
	bool invalid_value = (left<50) || (up<50) || (right<50) || (down<50) || (speed<1) || (left>270) || (up>150) || (right>270) || (down>150) || (speed>50);

	st_camera_bounds &cam = m_VidConfig.m_CameraBounds;

	if(invalid_value)
		cam.reset();
	else
		cam = CameraBounds;
}

CVidConfig &CVideoDriver::getVidConfig() { return m_VidConfig; }

short CVideoDriver::getZoomValue(void){ return m_VidConfig.Zoom; }

void CVideoDriver::isFullscreen(bool value) {
	m_VidConfig.Fullscreen = value;
}

short CVideoDriver::getFiltermode(void)
{
	if(m_VidConfig.m_ScaleXFilter < 1)
		m_VidConfig.m_ScaleXFilter = 1;
	return m_VidConfig.m_ScaleXFilter;
}

bool CVideoDriver::getFullscreen(void)
{	return m_VidConfig.Fullscreen;	}
unsigned int CVideoDriver::getWidth() const
{	return m_VidConfig.m_Resolution.width;	}
unsigned int CVideoDriver::getHeight() const
{	return m_VidConfig.m_Resolution.height;	}
unsigned short CVideoDriver::getDepth() const
{	return m_VidConfig.m_Resolution.depth;	}
SDL_Surface *CVideoDriver::getScrollSurface(void)
{	return mp_VideoEngine->getScrollSurface(); }

st_camera_bounds &CVideoDriver::getCameraBounds()
{ return m_VidConfig.m_CameraBounds; }

void CVideoDriver::stop()
{
	if(mp_VideoEngine)
		delete mp_VideoEngine;
	mp_VideoEngine = NULL;
}

CVideoDriver::~CVideoDriver()
{
 	stop();
}
