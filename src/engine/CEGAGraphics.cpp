/*
 * CEGAGraphics.cpp
 *
 *  Created on: 11.07.2009
 *      Author: gerstrong
 */

#include "CEGAGraphics.h"
#include "../FindFile.h"
#include "../CLogFile.h"
#include "../sdl/CVideoDriver.h"

CEGAGraphics::CEGAGraphics(short episode, const std::string& path) {
	m_episode = episode;
	m_path = path;
}

///
// Loads resources for the menu. Caution: This is only used for the game launcher menu
// If you want to load the game resources, look at CEGAGraphicsVort or CEGAGraphicsGalaxy subclasses
///
bool CEGAGraphics::loadData()
{
	// This is only for the menu. We only need one fontmap
	g_pGfxEngine->createEmptyFontmaps(1);
	CFont &Font = g_pGfxEngine->getFont(0);
	Font.destroySurface();
	Font.CreateSurface( g_pGfxEngine->Palette.m_Palette, SDL_SWSURFACE, g_pVideoDriver->getDepth() );
	Font.optimizeSurface();
	if( Font.loadHiColourFont("data/res/gfx/menufonts.bmp") )
		g_pLogFile->textOut(GREEN, "VGA Fontmap for the gamemenu has been loaded successfully!");

	g_pGfxEngine->createEmptyCursorMap(Font.getSDLSurface());
	CCursor *pCursor = g_pGfxEngine->getCursor();
	//Font.generateSpecialTwirls();
	pCursor->generateTwirls(Font);
	Font.generateGlowFonts();
	Font.generateInverseFonts();
	Font.generateDisabledFonts();

	return true;
}

CEGAGraphics::~CEGAGraphics() {

}


