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
m_keymsg(keymsg),
m_text_width(0)
{
	// Split up the text in lines, so can calculate the textbox dimensions
	std::string buf = "";
	for( size_t i=0 ; i<Text.size() ; i++ )
	{
		if( endofText(Text.substr(i)) )
		{
			if( m_text_width<buf.size() ) m_text_width=buf.size();

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

	if( m_text_width<buf.size() )
		m_text_width=buf.size();
}

bool CMessageBox::isFinished()
{ return m_mustclose; }
