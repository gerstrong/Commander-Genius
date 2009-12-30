/*
 * CFinale.cpp
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#include "CFinale.h"
#include "../../sdl/CVideoDriver.h"
#include "../../CLogFile.h"
#include "../../FindFile.h"
#include <fstream>

CFinale::CFinale() : m_mustfinishgame(false), mp_TextViewer(NULL) {
}

void CFinale::showEndingText()
{
	if(!mp_TextViewer)
	{
		std::string text;
		initEpilogue(text);
		mp_TextViewer = new CTextViewer(g_pVideoDriver->FGLayerSurface, 0, 0, 320, 120);
		mp_TextViewer->loadText(text);
	}

	mp_TextViewer->process();

	if(mp_TextViewer->hasClosed())
	{
		delete mp_TextViewer;
		mp_TextViewer = NULL;
		m_step++;
	}
}

void CFinale::initEpilogue(std::string &text)
{
    std::ifstream endfile;

    std::string filename =  mp_Map->m_gamepath + "endtext.ck" + itoa(m_Episode);

    OpenGameFileR(endfile, filename);
    if (endfile.is_open())
    {
        while(!endfile.eof())
        {
        	text.push_back(endfile.get());
        }

        endfile.close();
    }
    else
    {
    	g_pLogFile->textOut("Error reading \"" + filename + "\". Check if this file is in your directory!");
    }

}

CFinale::~CFinale() {
	if(mp_TextViewer)	delete mp_TextViewer;
}
