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
	mp_Scrollsurface = g_pVideoDriver->ScrollSurface;
	mp_Map = new CMap(mp_Scrollsurface, g_pGfxEngine->Tilemap);
	CMapLoader Maploader(mp_Map);
	std::string Text;
	char *c_text;

	// Read the Storytext
	readStoryText(&c_text, episode, DataDirectory);
	Text = c_text;
	Maploader.load(episode, 90, DataDirectory);

	// Create the Text ViewerBox and stores the text there!
	mp_TextViewer = new CTextViewer(g_pVideoDriver->FGLayerSurface, 0, 0, 320, 160);
	mp_TextViewer->loadText(Text);

	// Now Scroll to the position of the player and center him

	mp_Map->gotoPos( 32+2*320, 0 );

	// draw level map
	mp_Map->drawAll();
}

void CStory::process() {
	// NOTE: Animation is performed here too, because the story plane is drawn over the other
	// map that is open. That is desired!

	// Animate the tiles
	mp_Map->animateAllTiles();

	// Blit the background
	//g_pVideoDriver->blitScrollSurface();

	mp_TextViewer->process();

	if(mp_TextViewer->hasClosed())
		m_destroy_me=true;
}

CStory::~CStory() {
	delete mp_TextViewer;
	delete mp_Map;
}

