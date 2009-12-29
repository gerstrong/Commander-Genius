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
			Text.erase(Text.size()-1);
		}
		else
		{
			// Here the Text file is within the EXE-File
			unsigned long startflag=0, endflag=0;
			unsigned char *text_data = NULL;
			
			CExeFile *ExeFile = new CExeFile(episode, DataDirectory);
			ExeFile->readData();
			
			if(!ExeFile->getHeaderData()) return;
			
			if(episode == 2)
			{
				startflag = 0x15DC0;
				endflag = 0x1659F;
			}
			else // Episode 3
			{
				startflag = 0x17BD0;
				endflag = 0x1839B;
			}
			
			text_data = ExeFile->getRawData();
			
			for(unsigned long i=startflag ; i<endflag ; i++ )
				Text.push_back(text_data[i]);
			
			delete ExeFile;
		}
	}
	else
	{
			// We suppose that we are using version 131. Maybe it must be extended
			std::string filename = DataDirectory;
			if(DataDirectory != "")
				filename += "/../../";
			
			filename += "helptext.ckp";
			
			std::ifstream File; OpenGameFileR(File, filename, std::ios::binary);
			
			if(!File) return;
			
			while(!File.eof())
				Text.push_back(File.get());
			
			File.close();
			Text.erase(Text.size()-1);
	}
	
	// Create the Text ViewerBox and stores the text there!
	mp_TextViewer = new CTextViewer(g_pVideoDriver->FGLayerSurface, 0, 8, 320, 160);
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

