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
#include "common/CVorticonMapLoader.h"
#include "fileio/CExeFile.h"
#include "graphics/CGfxEngine.h"
#include "sdl/CVideoDriver.h"
#include "sdl/input/CInput.h"
#include "FindFile.h"


void CStory::init()
{
    CInfoScene::init();
	CExeFile &ExeFile = g_pBehaviorEngine->m_ExeFile;
	const char episode = ExeFile.getEpisode();
	std::string DataDirectory = ExeFile.getDataDirectory();
    
	mpMap.reset(new CMap());
	CVorticonMapLoaderBase Maploader(mpMap);
    
	std::string Text;
    
	// Read the Storytext
	if(episode==1)
	{
		// We suppose that we are using version 131. Maybe it must be extended
		std::string filename = ExeFile.getDataDirectory();
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
		
		text_data = ExeFile.getRawData();
		if(!text_data) return;
        
		for(unsigned long i=startflag ; i<endflag ; i++ )
			Text.push_back(text_data[i]);
	}
	
	Maploader.load(episode, 90, DataDirectory, false);
	
	// Create the Text ViewerBox and stores the text there!
	mpTextViewer.reset(new CTextViewer(0, 0, 320, 136));
    
	mpTextViewer->formatText(Text);
	
	// Scroll to the map where you see Keen with his rocket.
	mpMap->gotoPos( 32+2*320, 32 );
	mpMap->drawAll();
}

void CStory::process()
{
	mpMap->animateAllTiles();
	g_pVideoDriver->mDrawTasks.add( new BlitScrollSurfaceTask() );
    
	if(mpTextViewer)
	{
		mpTextViewer->process();
		if(mpTextViewer->hasClosed())
			m_destroy_me=true;
	}
	else
	{
		m_destroy_me=true;
	}
}

void CStory::teardown()
{
	mpMap = NULL;
	CEventContainer &EventContainer = g_pBehaviorEngine->EventList();
	EventContainer.add(new ResetScrollSurface);
}
