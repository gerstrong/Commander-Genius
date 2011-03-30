/*
 * CMessageBox.cpp
 *
 *  Created on: 04.12.2009
 *      Author: gerstrong
 */

#include "CMessageBox.h"
#include "sdl/CVideoDriver.h"
#include "sdl/CInput.h"
#include "graphics/CGfxEngine.h"
#include "StringUtils.h"

CMessageBox::CMessageBox(const std::string& Text, bool lower, bool keymsg, bool leftbound) :
m_mustclose(false),
m_keymsg(keymsg)
{
	// Split up the text in lines, so can calculate the textbox height
	size_t width=0; // determined by checking if the text is getting wider

	std::string buf = "";
	for( size_t i=0 ; i<Text.size() ; i++ )
	{
		if( endofText(Text.substr(i)) )
		{
			if( width<buf.size() ) width=buf.size();

			m_Lines.push_back(buf);
			buf.clear();
		}
		else
			buf += Text[i];
	}

	size_t pos = 0;
	if(!buf.empty())
		while( (pos = buf.find('\n')) != std::string::npos )
			buf.erase(pos,1);
	m_Lines.push_back(buf);
	if( width<buf.size() ) width=buf.size();

	if(m_keymsg)
	{
		m_Lines.push_back("");
		buf = "Press any KEY/BUTTON  ";
		if( width<buf.size() ) width=buf.size();
		else
		{
			size_t trail = width-buf.size();
			buf.insert((size_t)0,trail,' ');
		}
		m_Lines.push_back(buf);
	}

	// try to center that dialog box
	m_gamerect = g_pVideoDriver->getGameResolution();

	int h = m_gamerect.h;
	m_gamerect.x = m_gamerect.w/2;
	m_gamerect.y = m_gamerect.h/2;

	m_gamerect.h = (m_Lines.size()+2)*8;
	m_gamerect.w = (width+2)*8;

	m_gamerect.x -= m_gamerect.w/2;

	if(!lower)
		m_gamerect.y -= m_gamerect.h/2;
	else
		m_gamerect.y = h-m_gamerect.h;

	if(leftbound) m_gamerect.x = 0;


	m_gamerect.h /= 8;
	m_gamerect.w /= 8;

	mp_DlgFrame = new CDlgFrame(m_gamerect.x, m_gamerect.y,
								m_gamerect.w, m_gamerect.h, DLG_THEME_VORTICON);
}

bool CMessageBox::isFinished()
{ return m_mustclose; }
