/*
 * CStory.cpp
 *
 *  Created on: 16.11.2009
 *      Author: gerstrong
 *
 *  This class is used for showing the page of the story
 */

#include <fstream>

#include "CStory.h"
#include "../../common/CMapLoader.h"
#include "../../fileio/CExeFile.h"
#include "../../graphics/CGfxEngine.h"
#include "../../sdl/CVideoDriver.h"
#include "../../sdl/CInput.h"
#include "../../FindFile.h"

#define SAFE_DELETE(x) if(x) { delete x; x=NULL; }

CStory::CStory(std::string &DataDirectory, char &episode)
{
	mp_Scrollsurface = g_pVideoDriver->ScrollSurface;
	mp_Map = new CMap;
	mp_Map->setScrollSurface(mp_Scrollsurface);
	mp_Map->setTileMap(g_pGfxEngine->Tilemap);

	CMapLoader Maploader(mp_Map);
	std::string Text;
	
	// Read the Storytext
	if(episode==1)
	{
		// We suppose that we are using version 131. Maybe it must be extended
		std::string filename = DataDirectory;
		if(DataDirectory != "")
			filename += "/";
		
		filename += "storytxt.ck1";
		
		std::ifstream File; OpenGameFileR(File, filename, std::ios::binary);
		
		if(!File) return;
		
		while(!File.eof())
			Text.push_back(File.get());
		
		File.close();
		Text.erase(Text.size()-1);
	}
	else
	{
		// Here the Text file is within the EXE-File
		unsigned long startflag=0, endflag=0;
		unsigned char *text_data = NULL;
		
		CExeFile *ExeFile = new CExeFile(episode, DataDirectory);
		ExeFile->readData();
		
		if(!ExeFile->getRawData()) return;
		
		if(episode == 2)
		{
			startflag = 0x16CC0-512;
			endflag = 0x17958-512;
		}
		else // Episode 3
		{
			startflag = 0x18DD0-512;
			endflag = 0x199F3-512;
		}
		
		text_data = ExeFile->getRawData();
		
		for(unsigned long i=startflag ; i<endflag ; i++ )
			Text.push_back(text_data[i]);
		
		delete ExeFile;
	}
	
	Maploader.load(episode, 90, DataDirectory);
	
	// Create the Text ViewerBox and stores the text there!
	mp_TextViewer = new CTextViewer(g_pVideoDriver->FGLayerSurface, 0, 0, 320, 136);
	mp_TextViewer->loadText(Text);
	
	// Now Scroll to the position of the player and center him
	
	mp_Map->gotoPos( 32+2*320, 32 );
	
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
	SAFE_DELETE(mp_TextViewer);
	SAFE_DELETE(mp_Map);
}

