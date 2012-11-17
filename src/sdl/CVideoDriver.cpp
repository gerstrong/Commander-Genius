/*
 * CVideoDriver.cpp
 *
 *  Created on: 17.03.2009
 *      Author: gerstrong
 *
 *  Driver which will handle the Video Functions. This Driver handle the window mode and resolution mgmt.
 *  The Rendering itself is performed under COpenGL or CSDLVideo Class depending on what is enabled.
 */
#include "CVideoDriver.h"
#include "input/CInput.h"
#include "CTimer.h"

#include "video/CSDLVideo.h"
#include "video/COpenGL.h"
#include "resolutionlist.h"

#include "graphics/CGfxEngine.h"
#include "CLogFile.h"
#include "FindFile.h"
#include "graphics/PerSurfaceAlpha.h"
#include <iostream>
#include <fstream>

const std::string CKLOGFILENAME = "genius.log";

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
m_mustrefresh(false)
{
	resetSettings();
}

// TODO: This should return something!
void CVideoDriver::resetSettings() {

	m_VidConfig.reset();

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0)
		g_pLogFile->textOut(RED, "Could not initialize SDL: %s<br>",
				SDL_GetError());
	else
		g_pLogFile->textOut(GREEN, "SDL was successfully initialized!<br>");

	initResolutionList();

	// take the first default resolution. It might be changed if there is a config file already created
	// If there are at least two possible resolutions choose the second one, as this is normally one basic small resolution
	setMode(m_Resolutionlist.front());
}

// initResolutionList() reads the local list of available resolution.
// This function can only be called internally
// TODO: This should return something!
void CVideoDriver::initResolutionList() {
	// This call will get the resolution we have right now and set it up for the system
	// On Handheld devices this means, they will only take that resolution and that would it be.
	// On the PC, this is the current resolution but will add others.
	CRect<Uint16> resolution(SDL_GetVideoInfo());
	CRect<Uint16> desktopResolution(resolution);

	// We have a resolution list, clear it and create a new one.

	m_Resolutionlist.clear();

// TODO: Not sure if those defines are really needed anymore.
#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
	resolution.w = 320; //  320;
	resolution.h = 200;//  480;
#elif defined(ANDROID)
	resolution.w = 320;
	resolution.h = 200;
#endif

	// Now on non-handheld devices let's check for more resolutions.
#if !defined(EMBEDDED)
	for (unsigned int c = 0; c < NUM_MAIN_RESOLUTIONS; c++) {
		// Depth won't be read anymore! Take the one the system is using actually
		if (sscanf(ResolutionsList[c], "%hux%hu", &resolution.w, &resolution.h)
				>= 2) {
			// Now check if it's possible to use this resolution
			verifyResolution(resolution, SDL_FULLSCREEN);
		}
	}

	// In case there is no fullscreen, we will adapt the resolution it fits best to the window
	if (!m_VidConfig.Fullscreen) {
		int e = 1;
		resolution.w = 320;
		resolution.h = 200;

		int maxwidth = SDL_GetVideoInfo()->current_w;

		while (resolution.w < maxwidth) {
			resolution.w = 320 * e;
			resolution.h = 200 * e;

			// Now check if it's possible to use this resolution
			verifyResolution(resolution, 0);
			e++;
		}
	}
#endif

	// The last resolution in the list is the desktop normally, therefore the highest
	m_Resolutionlist.push_back(desktopResolution);

	m_Resolution_pos = m_Resolutionlist.begin();
}

void CVideoDriver::verifyResolution(CRect<Uint16>& resolution,
		const int flags) {
	if (SDL_VideoModeOK(resolution.w, resolution.h, 32, flags)) {
		std::list<CRect<Uint16> >::iterator i;
		for (i = m_Resolutionlist.begin(); i != m_Resolutionlist.end(); i++) {
			if (*i == resolution)
				break;
		}

		if (i == m_Resolutionlist.end()) {
#ifdef DEBUG
			g_pLogFile->ftextOut(BLUE, "Resolution %ix%ix%i %X added\n",
					resolution.w, resolution.h, 32);
#endif
			m_Resolutionlist.push_back(resolution);
		}
	}
}

void CVideoDriver::setVidConfig(const CVidConfig& VidConf) {
	m_VidConfig = VidConf;
	setMode(m_VidConfig.m_DisplayRect);
}

void CVideoDriver::setSpecialFXMode(bool SpecialFX) {
	m_VidConfig.m_special_fx = SpecialFX;
}

void CVideoDriver::setMode(int width, int height, int depth) {
	CRect<Uint16> res(width, height);
	setMode(res);
}

void CVideoDriver::setMode(const CRect<Uint16>& res) {
	m_VidConfig.setResolution(res);

	// TODO: Cycle through the list until the matching resolution is matched. If it doesn't exist
	// add it;
	for (m_Resolution_pos = m_Resolutionlist.begin();
			m_Resolution_pos != m_Resolutionlist.end(); m_Resolution_pos++)
		if (*m_Resolution_pos == res)
			break;

	if (m_Resolution_pos == m_Resolutionlist.end()) {
		m_Resolutionlist.push_back(res);
		m_Resolution_pos--;
	}
}

#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
extern "C" void iPhoneRotateScreen();
#endif

bool CVideoDriver::applyMode() {
	const CRect<Uint16> &Res = m_VidConfig.m_DisplayRect;
	const CRect<Uint16> &GameRect = m_VidConfig.m_GameRect;

	// Before the resolution is set, check, if the zoom factor is too high!
	while (((Res.w / GameRect.w) < m_VidConfig.Zoom
			|| (Res.h / GameRect.h) < m_VidConfig.Zoom)
			&& (m_VidConfig.Zoom > 1))
		m_VidConfig.Zoom--;

	// Check if some zoom/filter modes are illogical and roll them back accordingly
	if ((m_VidConfig.Zoom == 3 && m_VidConfig.m_ScaleXFilter == 1)
			&& !m_VidConfig.m_opengl)
		m_VidConfig.Zoom = 2;

	if (m_VidConfig.Zoom == 0)
		m_VidConfig.Zoom = 1;

	m_VidConfig.m_DisplayRect = *m_Resolution_pos;
	return true;
}

bool CVideoDriver::start() {
	bool retval;
	std::string caption = "Commander Genius";
	SDL_WM_SetCaption(caption.c_str(), caption.c_str());
	// When the program is through executing, call SDL_Quit
	atexit(SDL_Quit);

	g_pLogFile->textOut("Starting graphics driver...<br>");

#ifdef USE_OPENGL
	if (m_VidConfig.m_opengl) // If OpenGL could be set, initialize the
	{
		mpVideoEngine.reset(new COpenGL(m_VidConfig));
		retval = mpVideoEngine->init();

		if (!retval) 
		{
			m_VidConfig.m_opengl = false;
			applyMode();
			mpVideoEngine.reset(new CSDLVideo(m_VidConfig));
			retval = mpVideoEngine->init();
		}
	} else {
#endif
		mpVideoEngine.reset(new CSDLVideo(m_VidConfig));
		retval = mpVideoEngine->init();

#ifdef USE_OPENGL
	}
#endif

#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
	iPhoneRotateScreen();
#endif

	// Now SDL will tell if the bpp works or changes it, if not supported.
	// this value is updated here!
	retval &= mpVideoEngine->createSurfaces();
	m_mustrefresh = true;

	return retval;
}

void CVideoDriver::setFilter(short value) {
	m_VidConfig.m_ScaleXFilter = value;
} // 1 means no filter

void CVideoDriver::setZoom(short value) {
	m_VidConfig.Zoom = value;
}

void CVideoDriver::setScaleType(bool IsNormal)
{ m_VidConfig.m_normal_scale = IsNormal; }

// defines the scroll-buffer that is used for blitScrollSurface(). It's normally passed by a CMap Object
// it might have when a level-map is loaded.
void CVideoDriver::updateScrollBuffer(CMap &map) 
{
	map.drawAll();
	mpVideoEngine->UpdateScrollBufX(map.m_scrollx);
	mpVideoEngine->UpdateScrollBufY(map.m_scrolly);
}

void CVideoDriver::blitScrollSurface() // This is only for tiles
// Therefore the name should be changed
{
	mpVideoEngine->blitScrollSurface();
	drawConsoleMessages();
}

void CVideoDriver::collectSurfaces() {
	mpVideoEngine->collectSurfaces();
}

void CVideoDriver::clearSurfaces() {
	mpVideoEngine->clearSurfaces();
}

void CVideoDriver::updateScreen() {
	mpVideoEngine->updateScreen();
}

// "Console" here refers to the capability to pop up in-game messages
// in the upper-left corner during game play ala Doom.
void CVideoDriver::drawConsoleMessages() {
	if (!NumConsoleMessages)
		return;

	if (!ConsoleExpireTimer) {
		NumConsoleMessages--;
		if (!NumConsoleMessages)
			return;
		ConsoleExpireTimer = CONSOLE_EXPIRE_RATE;
	} else
		ConsoleExpireTimer--;

	int y = CONSOLE_MESSAGE_Y;
	for (int i = 0; i < NumConsoleMessages; i++) {
		g_pGfxEngine->getFont(1).drawFont(mpVideoEngine->getBlitSurface(),
				cmsg[i].msg, CONSOLE_MESSAGE_X, y, true);
		y += CONSOLE_MESSAGE_SPACING;
	}
}

// removes all console messages
void CVideoDriver::DeleteConsoleMsgs(void) {
	NumConsoleMessages = 0;
}

// adds a console msg to the top of the screen and scrolls any
// other existing messages downwards
void CVideoDriver::AddConsoleMsg(const char *the_msg) {
	for (int i = MAX_CONSOLE_MESSAGES - 2; i >= 0; i--) {
		strcpy(cmsg[i + 1].msg, cmsg[i].msg);
	}
	strcpy(cmsg[0].msg, the_msg);

	if (NumConsoleMessages < MAX_CONSOLE_MESSAGES)
		NumConsoleMessages++;
	ConsoleExpireTimer = CONSOLE_EXPIRE_RATE;
}

void CVideoDriver::saveCameraBounds(st_camera_bounds &CameraBounds) 
{
	int &left = CameraBounds.left;
	int &up = CameraBounds.up;
	int &right = CameraBounds.right;
	int &down = CameraBounds.down;
	int &speed = CameraBounds.speed;

	if (left > right) {
		const int halfWidth = (left - right) / 2;
		left -= halfWidth;
		right += halfWidth;
		if (left > right)
			left--;
	}

	if (up > down) {
		const int halfHeight = (up - down) / 2;
		up -= halfHeight;
		down += halfHeight;
		if (up > down)
			up--;
	}

	bool invalid_value = (left < 50) || (up < 50) || (right < 50) || (down < 50)
			|| (speed < 1) || (left > 270) || (up > 150) || (right > 270)
			|| (down > 150) || (speed > 50);

	st_camera_bounds &cam = m_VidConfig.m_CameraBounds;

	if (invalid_value)
		cam.reset();
	else
		cam = CameraBounds;
}

CVidConfig &CVideoDriver::getVidConfig() 
{
	return m_VidConfig;
}

short CVideoDriver::getZoomValue() {
	return m_VidConfig.Zoom;
}

void CVideoDriver::isFullscreen(const bool value) {
	m_VidConfig.Fullscreen = value;
}

bool CVideoDriver::getFullscreen() {
	return m_VidConfig.Fullscreen;
}

unsigned int CVideoDriver::getWidth() const {
	return m_VidConfig.m_DisplayRect.w;
}

unsigned int CVideoDriver::getHeight() const {
	return m_VidConfig.m_DisplayRect.h;
}

unsigned short CVideoDriver::getDepth() const 
{
	return 32;
}

SDL_Surface *CVideoDriver::getScrollSurface() 
{
	return mpVideoEngine->getScrollSurface();
}

st_camera_bounds &CVideoDriver::getCameraBounds() 
{
	return m_VidConfig.m_CameraBounds;
}

////
//// Drawing stuff related Stuff
////

void CVideoDriver::pollDrawingTasks()
{
	while (!mDrawTasks.empty())
	{
		// Sprite Section
		if ( DrawSpriteTask *drawSpriteTask = mDrawTasks.occurredEvent<DrawSpriteTask>())
		{
			CSprite *Sprite = drawSpriteTask->mSpritePtr;

			Sprite->_drawSprite(getBlitSurface(), drawSpriteTask->mx,
					drawSpriteTask->my, drawSpriteTask->mAlpha);
		}
		else if ( DrawBlinkingSpriteTask *drawSpriteTask = mDrawTasks.occurredEvent<DrawBlinkingSpriteTask>())
		{
			CSprite *Sprite = drawSpriteTask->mSpritePtr;

			Sprite->_drawBlinkingSprite(getBlitSurface(), drawSpriteTask->mx,
					drawSpriteTask->my);
		}

		// Bitmap Section
		else if ( DrawBitmapTask *drawBitmapTask = mDrawTasks.occurredEvent<DrawBitmapTask>())
		{
			CBitmap *Bitmap = drawBitmapTask->mBmpPtr;

			Bitmap->_draw(getBlitSurface(), drawBitmapTask->mx,
					drawBitmapTask->my);
		}

		// Tiles Section which are in front or/and animated
		else if ( DrawAnimatedTileTask *drawAnimatedTileTask =
		    mDrawTasks.occurredEvent<DrawAnimatedTileTask>())
		{
			CTilemap *TilemapPtr = drawAnimatedTileTask->mTileMapPtr;

			TilemapPtr->drawTile(getBlitSurface(), drawAnimatedTileTask->mx,
					drawAnimatedTileTask->my, drawAnimatedTileTask->mtile);
		}

		else if ( DrawForegroundTilesTask *drawForegroundTiles =
		    mDrawTasks.occurredEvent<DrawForegroundTilesTask>())
		{
			drawForegroundTiles->mMap._drawForegroundTiles();
		}
				

		// GUI Rendering based Task
		else if ( DrawGUIRenderTask *renderTask =
		mDrawTasks.occurredEvent<DrawGUIRenderTask>())
		{
			renderTask->mpDialog->processRendering();
		}

		else if (mDrawTasks.occurredEvent<BlitScrollSurfaceTask>())
		{
			blitScrollSurface();
		}

		// Other Surface Blits
		else if ( BlitSurfaceTask *blitSurfaceTask =
					mDrawTasks.occurredEvent<BlitSurfaceTask>())
		{
			SDL_BlitSurface(blitSurfaceTask->mSfcToBlit.get(),
					blitSurfaceTask->mSrcRect.get(), getBlitSurface(),
					blitSurfaceTask->mDstRect.get());
		}

		// If none of the Events fit here, please warn this incident
		else
		{
			g_pLogFile->textOut(
					"Warning: Unknown Drawing task. Please let the developers debug this!");
		}

		mDrawTasks.pop_Event();
	}
}

SDL_Rect CVideoDriver::toBlitRect(const CRect<float> &rect)
{
	CRect<Uint16> GameRes = getGameResolution();
	CRect<float> screenRect(0, 0, GameRes.w, GameRes.h);
	CRect<float> RectDispCoordFloat = rect;

	// Transform to the blit coordinates
	RectDispCoordFloat.transform(screenRect);

	CRect<Uint16> RectDispCoord;
	RectDispCoord = RectDispCoordFloat;
	return RectDispCoord.SDLRect();
}

void CVideoDriver::clearDrawingTasks() {
	if (!mDrawTasks.empty()) 
	{
		mDrawTasks.clear();
	}
}
