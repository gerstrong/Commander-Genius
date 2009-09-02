/*
 * CGfxEngine.cpp
 *
 *  Created on: 26.08.2009
 *      Author: gerstrong
 */

#include "CGfxEngine.h"

CGfxEngine::CGfxEngine() {
	m_Palette = NULL;
}

CGfxEngine::~CGfxEngine() {
	// TODO Auto-generated destructor stub
}

void CGfxEngine::createEmptySprites(Uint16 num_sprites)
{
	CSprite tmp_spr;
	Sprite.reserve(num_sprites);
	for(Uint16 i=0 ; i<num_sprites ; i++ )
		Sprite.push_back(tmp_spr);
}

// Needed when the fade effect is called.
void CGfxEngine::setColorPalettes(SDL_Color *Palette)
{
	m_Palette = Palette;
	Font.setColorPalette(Palette);
	Tilemap.setColorPalette(Palette);
	for( Uint16 i=0 ; i<Sprite.size() ; i++ )
		Sprite[i].setColorPalette(Palette);
}

void CGfxEngine::copyTileToSprite( Uint16 t, Uint16 s, Uint16 ntilestocopy )
{
	SDL_Rect src_rect, dst_rect;

	src_rect.w = src_rect.h = 16;
	dst_rect.w = dst_rect.h = 16;


	Sprite[s].setSize( 16, 16*ntilestocopy );
	Sprite[s].createSurface( Tilemap.getSDLSurface()->flags, m_Palette );

	for(Uint8 i=0 ; i<ntilestocopy ; i++)
	{
		src_rect.x = 16*((t+i)%13);
		src_rect.y = 16*((t+i)/13);

		dst_rect.x = 0;
		dst_rect.y = 16*i;

		SDL_BlitSurface( Tilemap.getSDLSurface(), &src_rect, Sprite[s].getSDLSurface(), &dst_rect);
	}
}

// draw an empty dialog box, for youseeinyourmind(), etc.
// Maybe this should go to CFont, because it's drawn by fonts...
void CGfxEngine::drawDialogBox(SDL_Surface *DialogSurface, int x1, int y1, int w, int h)
{
	int x,y,i,j;

	Font.drawCharacter(DialogSurface, 1, x1*8, y1*8);
	Font.drawCharacter(DialogSurface, 3, (x1+w)*8, y1*8);
	for(x=(x1*8)+8,i=0;i<w-1;i++)
	{
		Font.drawCharacter(DialogSurface, 2, x, y1*8);
		x+=8;
	}
	y=(y1+1)*8;
	for(j=0;j<h-2;j++)
	{
		for(x=(x1*8),i=0;i<=w;i++)
		{
			if (i==0) Font.drawCharacter(DialogSurface, 4, x, y);
			else if (i==w) Font.drawCharacter(DialogSurface, 5, x, y);
			else Font.drawCharacter(DialogSurface, ' ', x, y);
			x+=8;
		}
		y+=8;
  }
    for(x=(x1*8),i=0;i<=w;i++)
    {
      if (i==0) Font.drawCharacter(DialogSurface, 6, x, y);
      else if (i==w) Font.drawCharacter(DialogSurface, 8, x, y);
      else Font.drawCharacter(DialogSurface, 7, x, y);
      x+=8;
    }
}
