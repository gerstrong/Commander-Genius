/*
 * CMessageBoxBitmapGalaxy.cpp
 *
 *  Created on: 04.05.2011
 *      Author: gerstrong
 */

#include "CMessageBoxBitmapGalaxy.h"

#include "sdl/CVideoDriver.h"


CMessageBoxBitmapGalaxy::CMessageBoxBitmapGalaxy( const std::string& Text, const Uint16 BitmapId ) :
CMessageBoxGalaxy(Text),
m_Bitmap(g_pGfxEngine->getBitmap(BitmapId))
{
	// Looking if the Bitmap is too big for the Message box. In that case enlarge it!
	if( m_Bitmap.getHeight() > m_boxrect.h )
	{
		const int new_height = m_Bitmap.getHeight();
		m_TextPos.x = 10+m_Bitmap.getWidth();
		m_TextPos.y = ((new_height+16)/2) - 8*m_Lines.size();

		// Resize
		mp_DlgFrame->resize(m_boxrect.w+m_Bitmap.getWidth()+2, new_height+16);

		// Now calculate new coordinates and remove the box
		SDL_Rect gamerect = g_pVideoDriver->getGameResolution();
		m_boxrect.x = (gamerect.w - mp_DlgFrame->m_w)/2;
		m_boxrect.y = (gamerect.h - mp_DlgFrame->m_h)/2;
		mp_DlgFrame->setPos(m_boxrect.x, m_boxrect.y);
	}
}


void CMessageBoxBitmapGalaxy::process()
{
	CMessageBoxGalaxy::process();

	SDL_Surface *sfc = g_pVideoDriver->mp_VideoEngine->getFGLayerSurface();

	m_Bitmap.draw(sfc,m_boxrect.x+8, m_boxrect.y+8);
}
