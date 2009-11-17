/*
 * CStory.cpp
 *
 *  Created on: 16.11.2009
 *      Author: gerstrong
 *
 *  This class is used for showing the page of the story
 */

#include "CStory.h"
#include "../../include/fileio/story.h"
#include "../../common/CMapLoader.h"
#include "../../graphics/CGfxEngine.h"
#include "../../sdl/CVideoDriver.h"
#include "../../sdl/CInput.h"

CStory::CStory(std::string &DataDirectory, char &episode)
{
	mp_Map = new CMap(g_pVideoDriver->BlitSurface, g_pGfxEngine->Tilemap);
	CMapLoader Maploader(mp_Map);
	std::string Text;
	char *c_text;

	// Read the Storytext
	readStoryText(&c_text, episode, DataDirectory);
	Text = c_text;
	Maploader.load(episode, 90, DataDirectory);
	mp_Map->gotoPos(500,0); // Coords of the story background
	mp_Map->drawAll();

	// Create the Text ViewerBox and stores the text there!
	mp_TextViewer = new CTextViewer(g_pVideoDriver->FGLayerSurface, 0, 0, 320, 160);
	mp_TextViewer->loadText(Text);
	mp_Scrollsurface = g_pVideoDriver->ScrollSurface;
}

void CStory::process() {
	// Animate the tiles
	//g_pGfxEngine->Tilemap->animateAllTiles( mp_Scrollsurface );

	// NOTE: Animation and blit are performed here too, because the story plane is drawn over the other
	// map that is open. That is intention!

	mp_TextViewer->process();

	if(mp_TextViewer->hasClosed())
		m_destroy_me=true;
}

CStory::~CStory() {
	delete mp_TextViewer;
	delete mp_Map;
}

