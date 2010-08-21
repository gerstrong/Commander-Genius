/*
 * CMap.cpp
 *
 *  Created on: 21.09.2009
 *      Author: gerstrong
 */

#include "../keen.h"
#include "CMap.h"
#include <iostream>
#include <fstream>
#include "CBehaviorEngine.h"
#include "../FindFile.h"
#include "../CLogFile.h"
#include "../sdl/CVideoDriver.h"
#include "../graphics/CGfxEngine.h"

#define SAFE_DELETE_ARRAY(x)	if(x) { delete [] x; x = NULL; }

CMap::CMap():
m_width(0), m_height(0),
m_worldmap(false),
m_animation_enabled(true),
m_Dark(false),
m_Difficulty(1),
m_PlatExtending(false),
mp_scrollsurface(NULL),
m_Tilemaps(g_pGfxEngine->getTileMaps()),
m_animtiletimer(0),
m_Background(false)
{
	m_Level = 0;
	resetScrolls();
	memset(m_objectlayer, 0, sizeof(m_objectlayer));
}

////////////////////////////
// Initialization Routine //
////////////////////////////
void CMap::setScrollSurface( SDL_Surface *surface )
{  mp_scrollsurface = surface; }


void CMap::setLevel(Uint16 Level)
{	m_Level = Level;	}

Uint16 CMap::getLevel()
{	return m_Level;	}

/**
 * \brief	Create an empty data plane used for the map data
 * \param	blocksize	size in bytes of the datablock that has to be created
 */
bool CMap::createEmptyDataPlane(size_t plane, size_t blocksize)
{
	if(!blocksize) return false;

	m_Plane[plane].createDataMap(blocksize, m_width, m_height);
	m_Background = (m_Plane[0].getMapDataPtr() != NULL);

	return true;
}

void CMap::resetScrolls()
{
	m_scrollx = m_scrolly = 0;

	m_scrollx_buf = m_scrolly_buf = 0;

	m_scrollpix = m_scrollpixy = 0;
	m_mapx = m_mapy = 0;           // map X location shown at scrollbuffer row 0
	m_mapxstripepos = m_mapystripepos = 0;  // X pixel position of next stripe row
}

/////////////////////////
// Getters and Setters //
/////////////////////////
// returns the tile which is set at the given coordinates
Uint16 CMap::at(Uint16 x, Uint16 y, Uint16 t)
{
	if(x < m_width && y < m_height )
	{
		return m_Plane[t].getMapDataAt(x,y);
	}
	else
		return 0;
}

//////////////////////////
// returns the object/sprite/level which is set at the given coordinates
Uint16 CMap::getObjectat(Uint16 x, Uint16 y)
{	return m_objectlayer[x][y];	}

/**
 * \brief	Gets the pointer to the plane data of the map
 * \param	PlaneNum number of the requested Plane
 */
word *CMap::getData(Uint8 PlaneNum)
{
	return m_Plane[PlaneNum].getMapDataPtr();
}

word *CMap::getForegroundData()
{
	return m_Plane[1].getMapDataPtr();
}

word *CMap::getBackgroundData()
{
	return m_Plane[0].getMapDataPtr();
}


// searches the map's object layer for object OBJ.
// if it is found returns nonzero and places the
// coordinates of the first occurance of the object
// in (xout,yout)
bool CMap::findObject(unsigned int obj, int *xout, int *yout)
{
	unsigned int x,y;
	
	for(y=2;y<m_height-2;y++)
	{
		for(x=2;x<m_width-2;x++)
		{
			if (m_objectlayer[x][y]==obj)
			{
				*xout = x;
				*yout = y;
				return true;
			}
		}
	}
	return false;
}

// searches the map's tile layer for tile TILE.
// if it is found returns nonzero and places the
// coordinates of the first occurance of the tile
// in (xout,yout)
bool CMap::findTile(unsigned int tile, int *xout, int *yout, int plane)
{
	unsigned int x,y;

	for(y=2;y<m_height-2;y++)
	{
		for(x=2;x<m_width-2;x++)
		{
			if (m_Plane[plane].getMapDataAt(x,y)==tile)
			{
				*xout = x;
				*yout = y;
				return true;
			}
		}
	}
	return false;
}

bool CMap::setTile(Uint16 x, Uint16 y, Uint16 t, Uint16 plane)
{
	if( x<m_width && y<m_height )
	{
		//mp_foreground_data[y*m_width + x] = t;
		m_Plane[plane].setMapDataAt(t, x, y);
		return true;
	}
	else
		return false;
}

bool CMap::setTile(Uint16 x, Uint16 y, Uint16 t, bool update, Uint16 plane)
{
	if(setTile( x, y, t, plane))
	{
		if( update )
		{
			redrawAt(x,y);
		}
		return true;
	}
	else return false;
}

// Called in level. This function does the same as setTile, but also draws directly to the scrollsurface
// used normally, when items are picked up
bool CMap::changeTile(Uint16 x, Uint16 y, Uint16 t)
{
	if( setTile( x, y, t ) )
	{
		m_Tilemaps.at(1).drawTile(mp_scrollsurface, (x<<4)&511, (y<<4)&511, t);
		return true;
	}
	return false;
}


/**
 * \brief here we removed some unwanted borders in Y-direction in case of getting them when using a
 * 		  higher screenspace in Y-direction, than the original used to..
 */
void CMap::changeTileArrayY(Uint16 x, Uint16 y, Uint16 h, Uint16 tile)
{
	SDL_Rect gameres = g_pVideoDriver->getGameResolution();
	const Uint16 x2 = x+gameres.w/16;
	const Uint16 y2 = y+h;

	for(Uint16 c_y=y ; c_y<y2 ; c_y++)
		for(Uint16 c_x=x ; c_x<x2 ; c_x++)
			changeTile(c_x, c_y, tile);
}

////
// Scrolling Routines
////
bool CMap::gotoPos(int x, int y)
{
	int dx,dy;
	bool retval = false;
	dx = x - m_scrollx;
	dy = y - m_scrolly;
	
	if( dx > 0 )
		for( int scrollx=0 ; scrollx<dx ; scrollx++) scrollRight();
	else retval = true;
	
	if( dx < 0 )
		for( int scrollx=0 ; scrollx<-dx ; scrollx++) scrollLeft();
	else retval = true;
	
	if( dy > 0 )
		for( int scrolly=0 ; scrolly<dy ; scrolly++) scrollDown();
	else retval = true;
	
	if( dy < 0 )
		for( int scrolly=0 ; scrolly<-dy ; scrolly++) scrollUp();
	else retval = true;

	return retval;
}

// scrolls the map one pixel right
void CMap::scrollRight(void)
{
	if(m_scrollx < (m_width<<4) - g_pVideoDriver->getGameResolution().w)
	{
		m_scrollx++;
		m_scrollx_buf = m_scrollx&511;

		m_scrollpix++;
		if (m_scrollpix>=16)
		{  // need to draw a new stripe
			drawVstripe(m_mapxstripepos, m_mapx + 32);
			m_mapx++;
			m_mapxstripepos += 16;
			if (m_mapxstripepos >= 512) m_mapxstripepos = 0;
			m_scrollpix = 0;
		}
	}
}

// scrolls the map one pixel left
void CMap::scrollLeft(void)
{
	if(m_scrollx>0)
	{
		m_scrollx--;
		m_scrollx_buf = m_scrollx&511;

		if (m_scrollpix==0)
		{  // need to draw a new stripe
			if(m_mapx>0) m_mapx--;
			if (m_mapxstripepos == 0)
			{
				m_mapxstripepos = (512 - 16);
			}
			else
			{
				m_mapxstripepos -= 16;
			}
			drawVstripe(m_mapxstripepos, m_mapx);

			m_scrollpix = 15;
		} else m_scrollpix--;
	}
}

void CMap::scrollDown(void)
{
	if(m_scrolly < (m_height<<4) - g_pVideoDriver->getGameResolution().h )
	{
		m_scrolly++;
		m_scrolly_buf = m_scrolly&511;

		m_scrollpixy++;
		if (m_scrollpixy>=16)
		{  // need to draw a new stripe
			drawHstripe(m_mapystripepos, m_mapy + 32);
			m_mapy++;
			m_mapystripepos += 16;
			if (m_mapystripepos >= 512) m_mapystripepos = 0;
			m_scrollpixy = 0;
		}
	}
}

void CMap::scrollUp(void)
{
	if(m_scrolly>0)
	{
		m_scrolly--;
		m_scrolly_buf = m_scrolly&511;

		if (m_scrollpixy==0)
		{  // need to draw a new stripe
			if(m_mapy>0) m_mapy--;
			if (m_mapystripepos == 0)
			{
				m_mapystripepos = (512 - 16);
			}
			else
			{
				m_mapystripepos -= 16;
			}
			drawHstripe(m_mapystripepos, m_mapy);

			m_scrollpixy = 15;
		} else m_scrollpixy--;
	}
}

//////////////////////
// Drawing Routines //
//////////////////////
// Draws the entire map to the scroll buffer
// called at start of level to draw the upper-left corner of the map
// onto the scrollbuffer...from then on the map will only be drawn
// in stripes as it scrolls around.
// TODO: This function has a mayor problem! It only works, when tile same on the screen
// are changed. In Vorticons this might be the case. In Galaxy it's different!
void CMap::redrawAt(int mx, int my)
{
	// Go throught the list and just draw all the tiles that need to be animated
	const Uint32 num_h_tiles = mp_scrollsurface->h/16;
	const Uint32 num_v_tiles = mp_scrollsurface->w/16;

	if(  mx >= m_mapx && my >= m_mapy &&
			mx < m_mapx + num_v_tiles && my < m_mapy + num_h_tiles  	)
	{
		const Uint16 loc_x = (((mx-m_mapx)<<4)+m_mapxstripepos)&511;
		const Uint16 loc_y = (((my-m_mapy)<<4)+m_mapystripepos)&511;

		if( m_Background )
		{
			size_t bg = m_Plane[0].getMapDataAt(mx, my);
			size_t fg = m_Plane[1].getMapDataAt(mx, my);


			m_Tilemaps.at(0).drawTile(mp_scrollsurface, loc_x, loc_y, bg);
			if(fg)
				m_Tilemaps.at(1).drawTile(mp_scrollsurface, loc_x, loc_y, fg);
		}
		else
		{
			size_t fg = m_Plane[1].getMapDataAt(mx, my);
			m_Tilemaps.at(1).drawTile(mp_scrollsurface, loc_x, loc_y, fg);
		}
	}
}

// draws all the map area. This is used for the title screen, when game starts and other passive scenes.
// Don't use it, when the game is scrolling. Use redrawAt instead,
// for the correct and fast update of tiles or
void CMap::drawAll()
{
	Uint32 num_h_tiles = mp_scrollsurface->h/16;
	Uint32 num_v_tiles = mp_scrollsurface->w/16;

	if(num_v_tiles+m_mapx >= m_width)
		num_v_tiles = m_width-m_mapx;

	if(num_h_tiles+m_mapy >= m_height)
		num_h_tiles = m_height-m_mapy;

	if( m_Background )
	{
		for(Uint32 y=0;y<num_h_tiles;y++)
		{
			for(Uint32 x=0;x<num_v_tiles;x++)
			{
				Uint32 bg = m_Plane[0].getMapDataAt(x+m_mapx, y+m_mapy);
				Uint32 fg = m_Plane[1].getMapDataAt(x+m_mapx, y+m_mapy);

				m_Tilemaps.at(0).drawTile(mp_scrollsurface, ((x<<4)+m_mapxstripepos)&511,((y<<4)+m_mapystripepos)&511, bg);
				if(fg)
					m_Tilemaps.at(1).drawTile(mp_scrollsurface, ((x<<4)+m_mapxstripepos)&511,((y<<4)+m_mapystripepos)&511, fg);
			}
		}
	}
	else
	{
		for(Uint32 y=0;y<num_h_tiles;y++)
		{
			for(Uint32 x=0;x<num_v_tiles;x++)
			{
				Uint32 fg = m_Plane[1].getMapDataAt(x+m_mapx, y+m_mapy);
				m_Tilemaps.at(1).drawTile(mp_scrollsurface, ((x<<4)+m_mapxstripepos)&511,((y<<4)+m_mapystripepos)&511, fg);
			}
		}
	}
}

// draw a horizontal stripe, for vertical scrolling
void CMap::drawHstripe(unsigned int y, unsigned int mpy)
{
	if(mpy >= m_height) return;
	Uint32 num_v_tiles= mp_scrollsurface->w/16;
	
	if( num_v_tiles+m_mapx >= m_width )
		num_v_tiles = m_width-m_mapx;

	if( m_Background )
	{
		for(Uint32 x=0;x<num_v_tiles;x++)
		{
			Uint32 bg = m_Plane[0].getMapDataAt(x+m_mapx, mpy);
			Uint32 fg = m_Plane[1].getMapDataAt(x+m_mapx, mpy);

			m_Tilemaps.at(0).drawTile(mp_scrollsurface, ((x<<4)+m_mapxstripepos)&511, y, bg);
			if(fg)
				m_Tilemaps.at(1).drawTile(mp_scrollsurface, ((x<<4)+m_mapxstripepos)&511, y, fg);
		}
	}
	else
	{
		for(Uint32 x=0;x<num_v_tiles;x++)
		{
			Uint32 fg = m_Plane[1].getMapDataAt(x+m_mapx, mpy);
			m_Tilemaps.at(1).drawTile(mp_scrollsurface, ((x<<4)+m_mapxstripepos)&511, y, fg);
		}
	}
}

// draws a vertical stripe from map position mapx to scrollbuffer position x
void CMap::drawVstripe(unsigned int x, unsigned int mpx)
{
	if(mpx >= m_width) return;

	Uint32 num_h_tiles= mp_scrollsurface->h/16;

	if( num_h_tiles+m_mapy >= m_height )
		num_h_tiles = m_height-m_mapy;

	if( m_Background )
	{
		for(Uint32 y=0;y<num_h_tiles;y++)
		{
			Uint32 bg = m_Plane[0].getMapDataAt(mpx, y+m_mapy);
			Uint32 fg = m_Plane[1].getMapDataAt(mpx, y+m_mapy);

			m_Tilemaps.at(0).drawTile(mp_scrollsurface, x, ((y<<4)+m_mapystripepos)&511, bg);
			if(fg)
				m_Tilemaps.at(1).drawTile(mp_scrollsurface, x, ((y<<4)+m_mapystripepos)&511, fg);
		}
	}
	else
	{
		for(Uint32 y=0;y<num_h_tiles;y++)
		{
			Uint32 fg = m_Plane[1].getMapDataAt(mpx, y+m_mapy);
			m_Tilemaps.at(1).drawTile(mp_scrollsurface, x, ((y<<4)+m_mapystripepos)&511, fg);
		}
	}
}

/**
 * \brief This function draws all the masked and foreground tiles
 */
void CMap::drawForegroundTiles()
{
	SDL_Surface* surface = g_pVideoDriver->getBlitSurface();
	const Uint16 num_h_tiles = surface->h;
	const Uint16 num_v_tiles = surface->w;
	const Uint16 x1 = m_scrollx>>TILE_S;
	const Uint16 y1 = m_scrolly>>TILE_S;
	const Uint16 x2 = (m_scrollx+num_v_tiles)>>TILE_S;
	const Uint16 y2 = (m_scrolly+num_h_tiles)>>TILE_S;

	std::vector<CTileProperties> &TileProperties =
			g_pBehaviorEngine->getTileProperties(1);
	for( size_t y=y1 ; y<=y2 ; y++)
	{
		for( size_t x=x1 ; x<=x2 ; x++)
		{
			Uint16 fg = m_Plane[1].getMapDataAt(x,y);

			const Uint16 loc_x = (x<<TILE_S)-m_scrollx;
			const Uint16 loc_y = (y<<TILE_S)-m_scrolly;

			if(!m_Background)
			{
    			if(TileProperties[fg].behaviour == -2) // case when when has a masked graphic
    				drawAnimatedTile(surface, loc_x, loc_y, fg+1);
    			else if (TileProperties[fg].behaviour == -1) // case when tile is just foreground
    				drawAnimatedTile(surface, loc_x, loc_y, fg);
			}
			else if(fg != 0)
			{
				if(TileProperties[fg].behaviour < 0)
					drawAnimatedTile(surface, loc_x, loc_y, fg);
			}
		}
	}
}

/////////////////////////
// Animation functions //
/////////////////////////
// searches for animated tiles at the map position (X,Y) and
// unregisters them from animtiles
// Draw an animated tile. If it's not animated draw it anyway

Uint8 CMap::getAnimtiletimer()
{	return m_animtiletimer;	}

void CMap::drawAnimatedTile(SDL_Surface *dst, Uint16 mx, Uint16 my, Uint16 tile)
{
	m_Tilemaps.at(1).drawTile( dst, mx, my, tile );
}

void CMap::animateAllTiles()
{
	if(!m_animation_enabled)
		return;

	// Let the animation timer tick!!
	m_animtiletimer++;

	// Go throught the list and just draw all the tiles that need to be animated
	Uint32 num_h_tiles = mp_scrollsurface->h/16;
	Uint32 num_v_tiles = mp_scrollsurface->w/16;

	if(num_v_tiles+m_mapx >= m_width)
		num_v_tiles = m_width-m_mapx;

	if(num_h_tiles+m_mapy >= m_height)
		num_h_tiles = m_height-m_mapy;

	std::vector<CTileProperties> &frontTileProperties =
			g_pBehaviorEngine->getTileProperties(1);
	word *p_front_tile = m_Plane[1].getMapDataPtr();

	if(m_Background)
	{
		std::vector<CTileProperties> &backTileProperties =
				g_pBehaviorEngine->getTileProperties(0);
		word *p_back_tile = m_Plane[0].getMapDataPtr();
		for( size_t y=0 ; y<m_height ; y++)
		{
			for( size_t x=0 ; x<m_width ; x++)
			{
				bool draw = false;

				CTileProperties &back_tile = backTileProperties[*p_back_tile];
				if( back_tile.animationtime && (m_animtiletimer % back_tile.animationtime == 0) )
				{
					*p_back_tile += back_tile.nextTile;
					draw = true;
				}
				p_back_tile++;


				CTileProperties &front_tile = frontTileProperties[*p_front_tile];
				if( front_tile.animationtime && (m_animtiletimer % front_tile.animationtime == 0) )
				{
					*p_front_tile += front_tile.nextTile;
					draw = true;
				}
				p_front_tile++;

				if( draw && x >= m_mapx && y >= m_mapy &&
						x < m_mapx + num_v_tiles && y < m_mapy + num_h_tiles  	)
				{
					Uint16 bg = m_Plane[0].getMapDataAt(x,y);
					Uint16 fg = m_Plane[1].getMapDataAt(x,y);
					const Uint16 loc_x = (((x-m_mapx)<<4)+m_mapxstripepos)&511;
					const Uint16 loc_y = (((y-m_mapy)<<4)+m_mapystripepos)&511;
					m_Tilemaps.at(0).drawTile(mp_scrollsurface, loc_x, loc_y, bg);
					if(fg)
						m_Tilemaps.at(1).drawTile(mp_scrollsurface, loc_x, loc_y, fg);
				}
			}
		}
	}
	else
	{
		for( size_t y=0 ; y<m_height ; y++)
		{
			for( size_t x=0 ; x<m_width ; x++)
			{
				bool draw = false;

				CTileProperties &front_tile = frontTileProperties[*p_front_tile];
				if( front_tile.animationtime && (m_animtiletimer % front_tile.animationtime == 0) )
				{
					*p_front_tile += front_tile.nextTile;
					draw = true;
				}
				p_front_tile++;

				if( draw && x >= m_mapx && y >= m_mapy &&
						x < m_mapx + num_v_tiles && y < m_mapy + num_h_tiles  	)
				{
					Uint16 fg = m_Plane[1].getMapDataAt(x,y);
					const Uint16 loc_x = (((x-m_mapx)<<4)+m_mapxstripepos)&511;
					const Uint16 loc_y = (((y-m_mapy)<<4)+m_mapystripepos)&511;
					m_Tilemaps.at(1).drawTile(mp_scrollsurface, loc_x, loc_y, fg);
				}
			}
		}
	}
}


