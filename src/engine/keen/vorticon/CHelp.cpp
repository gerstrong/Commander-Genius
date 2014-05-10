/*
 * CHelp.cpp
 *
 *  Created on: 16.11.2009
 *      Author: gerstrong
 *
 *  This class is used for showing the page of a vorticon story
 *  Can also be used for other informational things
 */

#include "CHelp.h"
#include "fileio/CExeFile.h"
#include "CVorticonMapLoader.h"
#include "graphics/GsGraphics.h"
//#include "sdl/CVideoDriver.h"
#include <base/CInput.h>
#include <base/utils/FindFile.h>

#include <fstream>

#include <fileio/KeenFiles.h>

CHelp::CHelp(const std::string &type) :
mTextType(type)
{}

void CHelp::init()
{
    CInfoScene::init();
	std::string Text;
    CExeFile &ExeFile = gKeenFiles.exeFile;
    const std::string DataDirectory = gKeenFiles.gameDir;
	const char episode = ExeFile.getEpisode();
	
	// Read the Storytext
	if(mTextType == "Game")
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
			
			
			if(!ExeFile.getHeaderData()) return;
			
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
			
			text_data = ExeFile.getRawData();
			
			for(unsigned long i=startflag ; i<endflag ; i++ )
				Text.push_back(text_data[i]);
		}
	}
	else
	{
		std::string filename = "HELPTEXT.CKP";

		std::ifstream File; OpenGameFileR(File, filename, std::ios::binary);

		if(!File) return;

		while(!File.eof())
			Text.push_back(File.get());

		File.close();
		Text.erase(Text.size()-1);
	}

	// Creates the Text ViewerBox and stores the text there!
	mpTextViewer.reset(new CTextViewer(0, 8, 320, 160));
	mpTextViewer->formatText(Text);
}

void CHelp::teardown()
{
	CEventContainer &EventContainer = gEventManager;
	EventContainer.add(new ResetScrollSurface);
}


void CHelp::ponder()
{
	// NOTE: Animation is performed here too, because the story plane is drawn over the other
	// map that is open. That is desired!

	if(mpTextViewer)
	{
        mpTextViewer->ponder(0);
		if(mpTextViewer->hasClosed())
			m_destroy_me=true;
	}
	else
	{
		m_destroy_me=true;
	}
}

void CHelp::render()
{
    if(mpTextViewer)
    {
        mpTextViewer->render();
    }
}
