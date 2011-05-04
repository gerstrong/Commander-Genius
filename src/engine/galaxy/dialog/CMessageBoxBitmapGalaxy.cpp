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
	SDL_Rect gamerect = g_pVideoDriver->getGameResolution();


	// Looking if the Bitmap is too big for the Message box. In that case enlarge it!
	if( m_Bitmap.getHeight() > m_boxrect.h )
	{
		const int new_height = m_Bitmap.getHeight();
		m_TextPos.x = 10+m_Bitmap.getWidth();
		m_TextPos.y = ((new_height+16)/2) - 8*m_Lines.size();

		// Resize
		mp_DlgFrame->resize(m_boxrect.w+m_Bitmap.getWidth()+2, new_height+16);

		// Now calculate new coordinates and remove the box
		m_boxrect.x = (gamerect.w - m_boxrect.w)/2;
		m_boxrect.y = (gamerect.h - m_boxrect.h)/2;
	}
}


void CMessageBoxBitmapGalaxy::process()
{
	CMessageBoxGalaxy::process();

	SDL_Surface *sfc = g_pVideoDriver->mp_VideoEngine->getFGLayerSurface();

	m_Bitmap.draw(sfc,m_boxrect.x+8, m_boxrect.y+8);
}
