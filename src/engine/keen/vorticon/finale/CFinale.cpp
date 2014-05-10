/*
 * CFinale.cpp
 *
 *  Created on: 04.11.2009
 *      Author: gerstrong
 */

#include "CFinale.h"
//#include "sdl/CVideoDriver.h"
#include <base/GsLogging.h>
#include <base/utils/FindFile.h>
#include <fstream>


CFinale::CFinale(std::list< std::shared_ptr<CMessageBoxVort> > &messageBoxes,
					const std::shared_ptr<CMap> &pMap,
					std::vector< std::unique_ptr<CVorticonSpriteObject> > &Object) :
m_mustfinishgame(false),
mMessageBoxes(messageBoxes),
mpMap(pMap),
m_Object(Object)
{}

void CFinale::addMsgBoxString(const std::string &text)
{
    std::unique_ptr<CMessageBoxVort> msg( new CMessageBoxVort(g_pBehaviorEngine->getString(text), true) );
    mMessageBoxes.push_back( move(msg) );
}


void CFinale::showEndingText()
{
	if(!mpTextViewer)
	{
		std::string text;
		mpTextViewer.reset(new CTextViewer(0, 0, 320, 120));
		mpTextViewer->loadTextfromFile(mpMap->m_gamepath + "endtext.ck" + itoa(m_Episode));
	}

    mpTextViewer->ponder(0);

	if(mpTextViewer->hasClosed())
	{
		mpTextViewer.release();
		m_step++;
	}
}


void CFinale::renderEndingText()
{
    if(mpTextViewer)
    {
        mpTextViewer->render();
    }
}
