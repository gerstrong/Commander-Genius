/*
 * CFinale.cpp
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#include "CFinale.h"
#include "sdl/CVideoDriver.h"
#include "CLogFile.h"
#include "FindFile.h"
#include <fstream>

CFinale::CFinale(CMap &map, std::vector<CObject*> &Object) :
m_mustfinishgame(false),
mp_TextViewer(NULL),
m_Object(Object)
{
	//mMap =  new CMap(map);
}

void CFinale::showEndingText()
{
	if(!mp_TextViewer)
	{
		std::string text;
		mp_TextViewer = new CTextViewer(0, 0, 320, 120);
		mp_TextViewer->loadTextfromFile(mMap->m_gamepath + "endtext.ck" + itoa(m_Episode));
	}

	mp_TextViewer->process();

	if(mp_TextViewer->hasClosed())
	{
		delete mp_TextViewer;
		mp_TextViewer = NULL;
		m_step++;
	}
}

CFinale::~CFinale() {
	if(mp_TextViewer)	delete mp_TextViewer;
}
