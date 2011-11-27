/*
 * CMessageBoxBitmapGalaxy.cpp
 *
 *  Created on: 04.05.2011
 *      Author: gerstrong
 */

#include "CMessageBoxBitmapGalaxy.h"
#include "sdl/CVideoDriver.h"


CMessageBoxBitmapGalaxy::CMessageBoxBitmapGalaxy( const std::string& Text, const Uint16 BitmapId,
											const direction_t alignment ) :
CMessageBoxGalaxy(Text),
m_Bitmap(g_pGfxEngine->getBitmap(BitmapId)),
m_alignment(alignment)
{
	int new_height = m_boxrect.h;

	// Looking if the Bitmap is too big for the Message box. In that case enlarge it!
	if( m_Bitmap.getHeight() > m_boxrect.h )
	{
		new_height = m_Bitmap.getHeight();
	}

	if( m_alignment == RIGHT )
		m_TextPos.x = 10;
	else
		m_TextPos.x = 10+m_Bitmap.getWidth();

	// I think this depends on the font height itself. I don't get why 6 is correct here.
	m_TextPos.y = ( (new_height+16)/2 ) - 6*m_Lines.size();

	// Resize
	mp_DlgFrame->resize(m_boxrect.w+m_Bitmap.getWidth()+2, new_height+16);

	// Now calculate new coordinates and remove the box
	SDL_Rect gamerect = g_pVideoDriver->getGameResolution().SDLRect();
	m_boxrect.x = (gamerect.w - mp_DlgFrame->m_w)/2;
	m_boxrect.y = (gamerect.h - mp_DlgFrame->m_h)/2;
	mp_DlgFrame->setPos(m_boxrect.x, m_boxrect.y);

}


void CMessageBoxBitmapGalaxy::process()
{
	CMessageBoxGalaxy::process();

	int bitmap_xcoord;

	// now compute where the bitmap is to be drawn
	if( m_alignment == RIGHT )
		bitmap_xcoord = mp_DlgFrame->m_x+mp_DlgFrame->m_w-(m_Bitmap.getWidth()+8);
	else
		bitmap_xcoord = m_boxrect.x+8;

	SDL_Surface *sfc = g_pVideoDriver->mp_VideoEngine->getBlitSurface();

	m_Bitmap.draw( sfc,bitmap_xcoord, m_boxrect.y+8 );
}
