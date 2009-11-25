/*
 * CHelp.cpp
 *
 *  Created on: 16.11.2009
 *      Author: gerstrong
 *
 *  This class is used for showing the page of the story
 */

#include <fstream>

#include "CHelp.h"
#include "../../fileio/CExeFile.h"
#include "../../common/CMapLoader.h"
#include "../../graphics/CGfxEngine.h"
#include "../../sdl/CVideoDriver.h"
#include "../../sdl/CInput.h"
#include "../../FindFile.h"

CHelp::CHelp(std::string &DataDirectory, char &episode, std::string type)
{
	std::string Text;
	
	// Read the Storytext
	if(type == "Game")
	{
		if(episode==1)
		{
			// We suppose that we are using version 131. Maybe it must be extended
			std::string filename = DataDirectory;
			if(DataDirectory != "")
				filename += "/";
			
			filename += "helptext.ck1";
			
			std::ifstream File; OpenGameFileR(File, filename, std::ios::binary);
			
			if(!File) return;
			
			while(!File.eof())
				Text.push_back(File.get());
			
			File.close();
		}
		else
		{
			// Here the Text file is within the EXE-File
			unsigned long startflag=0, endflag=0;
			unsigned char *text_data = NULL;
			
			CExeFile *ExeFile = new CExeFile(episode, DataDirectory);
			ExeFile->readData();
			
			if(!ExeFile->getData()) return;
			
			if(episode == 2)
			{
				startflag = 0x15DC0-512;
				endflag = 0x1659F-512;
			}
			else // Episode 3
			{
				startflag = 0x17BD0-512;
				endflag = 0x1839F-512;
			}
			
			text_data = ExeFile->getData();
			
			for(unsigned long i=startflag ; i<endflag ; i++ )
				Text.push_back(text_data[i]);
			
			delete ExeFile;
		}
	}
	else
	{
		Text = "This is the Commander Genius help.";
	}
	
	// Create the Text ViewerBox and stores the text there!
	mp_TextViewer = new CTextViewer(g_pVideoDriver->FGLayerSurface, 0, 0, 320, 160);
	mp_TextViewer->loadText(Text);
}

void CHelp::process() {
	// NOTE: Animation is performed here too, because the story plane is drawn over the other
	// map that is open. That is desired!
	
	// Blit the background
	//g_pVideoDriver->blitScrollSurface();
	
	mp_TextViewer->process();
	
	if(mp_TextViewer->hasClosed())
		m_destroy_me=true;
}

CHelp::~CHelp() {
	delete mp_TextViewer;
}

