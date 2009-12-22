/*
 * CFinale.cpp
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#include "CFinale.h"
#include "../../sdl/CVideoDriver.h"
#include "../../fileio/CExeFile.h"

CFinale::CFinale() : m_mustfinishgame(false), mp_TextViewer(NULL) {
}

void CFinale::init_ToBeContinued()
{
	/*int i;
	 std::string text;
	 int dlgX, dlgY, dlgW, dlgH;

	 // remove all objects because eseq_showmsg will call drawobjects
	 for(i=0;i<MAX_OBJECTS;i++)
     objects[i].exists = 0;

	 text = getstring("TO_BE_CONTINUED");
	 dlgX = GetStringAttribute("TO_BE_CONTINUED", "LEFT");
	 dlgY = GetStringAttribute("TO_BE_CONTINUED", "TOP");
	 dlgW = GetStringAttribute("TO_BE_CONTINUED", "WIDTH");
	 dlgH = GetStringAttribute("TO_BE_CONTINUED", "HEIGHT");*/
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
	unsigned char *filebuf;
	unsigned long startflag=0x1652A-512, endflag=0x1679A-512; // where story begins and ends!

	CExeFile *ExeFile = new CExeFile(m_Episode, mp_Map->m_gamepath);
	if(!ExeFile) return;
	if(!ExeFile->readData()) return;
	filebuf = ExeFile->getData() + startflag;

	for( unsigned long i=0 ; i<endflag-startflag ; i++ )
	{
		if(filebuf[i])
			text.push_back(filebuf[i]);
	}
	delete ExeFile;
}

CFinale::~CFinale() {
	if(mp_TextViewer)	delete mp_TextViewer;
}
