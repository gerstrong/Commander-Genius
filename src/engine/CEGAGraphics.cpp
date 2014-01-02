/*
 * CEGAGraphics.cpp
 *
 *  Created on: 11.07.2009
 *      Author: gerstrong
 */

#include "CEGAGraphics.h"
#include "fileio/ResourceMgmt.h"
#include "FindFile.h"
#include <lib/base/GsLogging.h>
#include "sdl/CVideoDriver.h"
#include "graphics/CGfxEngine.h"

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

	Font.loadinternalFont();

	return true;
}

