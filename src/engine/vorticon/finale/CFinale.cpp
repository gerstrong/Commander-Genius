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

CFinale::CFinale(std::list< SmartPointer<CMessageBoxVort> > &messageBoxes,
					const SmartPointer<CMap> &pMap,
					std::vector<CVorticonSpriteObject*> &Object) :
m_mustfinishgame(false),
mMessageBoxes(messageBoxes),
mpMap(pMap),
m_Object(Object)
{}

void CFinale::addMsgBoxString(const std::string &text)
{
	mMessageBoxes.push_back( new CMessageBoxVort(g_pBehaviorEngine->getString(text), true) );
}


void CFinale::showEndingText()
{
	if(mpTextViewer.empty())
	{
		std::string text;
		mpTextViewer = new CTextViewer(0, 0, 320, 120);
		mpTextViewer->loadTextfromFile(mpMap->m_gamepath + "endtext.ck" + itoa(m_Episode));
	}

	mpTextViewer->process();

	if(mpTextViewer->hasClosed())
	{
		mpTextViewer = NULL;
		m_step++;
	}
}
