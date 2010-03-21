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
	CFont &Font = g_pGfxEngine->getFont();
	Font.DestroySurface();
	Font.CreateSurface( g_pGfxEngine->Palette.m_Palette, SDL_SWSURFACE, g_pVideoDriver->getDepth() );
	Font.optimizeSurface();
	std::string path = GetFullFileName("data/res/gfx/menufonts.bmp");
	if( Font.loadHiColourFont(path) )
		g_pLogFile->textOut(GREEN, "VGA Fontmap for the gamemenu has been loaded successfully!");

	Font.generateSpecialTwirls();
	Font.generateGlowFonts();
	Font.generateInverseFonts();
	Font.generateDisabledFonts();
	return true;
}

CEGAGraphics::~CEGAGraphics() {

}


