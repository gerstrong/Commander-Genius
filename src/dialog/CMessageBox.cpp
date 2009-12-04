/*
 * CMessageBox.cpp
 *
 *  Created on: 04.12.2009
 *      Author: gerstrong
 */

#include "CMessageBox.h"
#include "../sdl/CVideoDriver.h"
#include "../sdl/CInput.h"
#include "../graphics/CGfxEngine.h"

CMessageBox::CMessageBox(const std::string& Text) :
m_mustclose(false)
{
	// Split up the text in lines, so can calculate the textbox height
	size_t width=0; // determined by checking if the text is getting wider

	std::string buf = "";
	for( size_t i=0 ; i<Text.size() ; i++ )
	{
		if(Text[i]=='\n')
		{
			if( width<buf.size() ) width=buf.size();
			m_Lines.push_back(buf);
			buf = "";
		}
		else
			buf += Text[i];
	}

	m_Lines.push_back(buf);
	if( width<buf.size() ) width=buf.size();

	// try to center that dialog box
	m_gamerect = g_pVideoDriver->getGameResolution();

	m_gamerect.x = m_gamerect.w/2;
	m_gamerect.y = m_gamerect.h/2;
	m_gamerect.h = (m_Lines.size()+2)*8;
	m_gamerect.w = (width+2)*8;
	m_gamerect.x -= m_gamerect.w/2;
	m_gamerect.y -= m_gamerect.h/2;

	m_gamerect.h /= 8;
	m_gamerect.w /= 8;

	mp_DlgFrame = new CDlgFrame(m_gamerect.x, m_gamerect.y,
								m_gamerect.w, m_gamerect.h, DLG_THEME_OLDSCHOOL);

	for(int c=0 ; c<m_Lines.size() ; c++)
	{
		printf("%s", m_Lines[c].c_str());
	}

}

void CMessageBox::process()
{
	SDL_Surface *sfc = g_pVideoDriver->FGLayerSurface;

	// Look, if somebody pressed a button, and close this dialog!
	if(g_pInput->getPressedAnyCommand()) {
		m_mustclose = true;
		return;
	}

	// Draw the empty Dialog Box
	mp_DlgFrame->draw(sfc);

	// Draw the Text on our surface
	for( size_t i=0 ; i<m_Lines.size() ; i++)
		g_pGfxEngine->Font->drawFont(sfc, m_Lines[i], m_gamerect.x+8, m_gamerect.y+((i+1)*8) );
}

bool CMessageBox::isFinished()
{ return m_mustclose; }

CMessageBox::~CMessageBox() {
	delete mp_DlgFrame;
}
