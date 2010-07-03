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
m_Tilemaps(g_pGfxEngine->getTileMaps()),
m_animtiletimer(0)
{
	memset( m_AnimTileInUse, 0, sizeof(m_AnimTileInUse));
	memset( m_animtiles, 0, sizeof(m_animtiles));
	resetScrolls();
	memset(m_objectlayer, 0, sizeof(m_objectlayer));
}

////////////////////////////
// Initialization Routine //
////////////////////////////


//void CMap::setTileMaps( std::vector<CTilemap> &Tilemap ){
//	mp_Tilemap = &Tilemap;
//}
void CMap::setScrollSurface( SDL_Surface *surface )
{  mp_scrollsurface = surface; }


/**
 * \brief	Create an empty data plane used for the map data
 * \param	blocksize	size in bytes of the datablock that has to be created
 */
bool CMap::createEmptyDataPlane(size_t plane, size_t blocksize)
{
	if(!blocksize) return false;

	m_Plane[plane].createDataMap(blocksize, m_width, m_height);

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
bool CMap::findTile(unsigned int tile, int *xout, int *yout)
{
	unsigned int x,y;

	for(y=2;y<m_height-2;y++)
	{
		for(x=2;x<m_width-2;x++)
		{
			if (m_Plane[1].getMapDataAt(x,y)==tile)
			{
				*xout = x;
				*yout = y;
				return true;
			}
		}
	}
	return false;
}

bool CMap::setTile(Uint16 x, Uint16 y, Uint16 t)
{
	if( x<m_width && y<m_height )
	{
		//mp_foreground_data[y*m_width + x] = t;
		m_Plane[1].setMapDataAt(t, x, y);
		return true;
	}
	else
		return false;
}

bool CMap::setTile(Uint16 x, Uint16 y, Uint16 t, bool update)
{
	if(setTile( x, y, t))
	{
		if(update) redrawAt(x,y);
		return true;
	}
	else return false;
}

// Called in level. This function does the same as setTile, but also draws directly to the scrollsurface
// used normally, when items are picked up
bool CMap::changeTile(Uint16 x, Uint16 y, Uint16 t, Uint8 tilemap)
{
	if( setTile( x, y, t ) )
	{
		m_Tilemaps.at(tilemap).drawTile(mp_scrollsurface, (x<<4)&511, (y<<4)&511, t);
		registerAnimation( (x<<4)&511, (y<<4)&511, t );
		return true;
	}
	return false;
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
		if(m_scrollx_buf>=511) m_scrollx_buf=0; else m_scrollx_buf++;

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
		if(m_scrollx_buf==0) m_scrollx_buf=511; else m_scrollx_buf--;

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
		if(m_scrolly_buf>=511) m_scrolly_buf=0; else m_scrolly_buf++;

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
		if(m_scrolly_buf==0) m_scrolly_buf=511; else m_scrolly_buf--;

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
void CMap::redrawAt(int mx, int my)
{
	for(size_t tilemap=0 ; tilemap<2 ; tilemap++)
	{
		if(m_Plane[tilemap].getMapDataPtr() == NULL)
			continue;
		int c = m_Plane[tilemap].getMapDataAt(mx, my);
		m_Tilemaps.at(tilemap).drawTile(mp_scrollsurface, (mx<<4)&511, (my<<4)&511, c);
		registerAnimation( (mx<<4)&511, (my<<4)&511, c );
	}
}

// draws all the map area. This is used for the title screen, when game starts and other passive scenes.
// Don't use it, when the game is scrolling. Use redrawAt instead,
// for the correct and fast update of tiles
void CMap::drawAll()
{
	Uint32 num_h_tiles = mp_scrollsurface->h/16;
	Uint32 num_v_tiles = mp_scrollsurface->w/16;

	if(num_v_tiles+m_mapx >= m_width)
		num_v_tiles = m_width-m_mapx;

	if(num_h_tiles+m_mapy >= m_height)
		num_h_tiles = m_height-m_mapy;

	bool has_background = true;
	std::vector<CTilemap>::iterator Tilemap = m_Tilemaps.begin();
	for(size_t plane=0 ; plane<2 ; plane++, Tilemap++)
	{
		if(m_Plane[plane].getMapDataPtr() != NULL)
		{
			for(Uint32 y=0;y<num_h_tiles;y++)
			{
				for(Uint32 x=0;x<num_v_tiles;x++)
				{
					Uint32 c = m_Plane[plane].getMapDataAt(x+m_mapx, y+m_mapy);
					if(has_background && c==0)
						continue;

					Tilemap->drawTile(mp_scrollsurface, ((x<<4)+m_mapxstripepos)&511, ((y<<4)+m_mapystripepos)&511, c);
					registerAnimation( ((x<<4)+m_mapxstripepos)&511, ((y<<4)+m_mapystripepos)&511, c );
				}
			}
		}
		else
			has_background = false;
	}
}

// draw a horizontal stripe, for vertical scrolling
void CMap::drawHstripe(unsigned int y, unsigned int mpy)
{
	if(mpy >= m_height) return;
	Uint32 num_v_tiles= mp_scrollsurface->w/16;
	
	if( num_v_tiles+m_mapx >= m_width )
		num_v_tiles = m_width-m_mapx;

	bool has_background = true;
	std::vector<CTilemap>::iterator Tilemap = m_Tilemaps.begin();
	for(size_t plane=0 ; plane<2 ; plane++, Tilemap++)
	{
		if(m_Plane[plane].getMapDataPtr() != NULL)
		{
			for(Uint32 x=0;x<num_v_tiles;x++)
			{
				Uint32 c = m_Plane[plane].getMapDataAt(x+m_mapx, mpy);
				if(has_background && c==0)
					continue;

				Tilemap->drawTile(mp_scrollsurface, ((x<<4)+m_mapxstripepos)&511, y, c);
				registerAnimation( ((x<<4)+m_mapxstripepos)&511, y, c );
			}
		}
		else
			has_background = false;
	}
}

// draws a vertical stripe from map position mapx to scrollbuffer position x
void CMap::drawVstripe(unsigned int x, unsigned int mpx)
{
	if(mpx >= m_width) return;

	Uint32 num_h_tiles= mp_scrollsurface->h/16;

	if( num_h_tiles+m_mapy >= m_height )
		num_h_tiles = m_height-m_mapy;

	bool has_background = true;
	std::vector<CTilemap>::iterator Tilemap = m_Tilemaps.begin();
	for(size_t plane=0 ; plane<2 ; plane++, Tilemap++)
	{
		if(m_Plane[plane].getMapDataPtr() != NULL)
		{
			for(Uint32 y=0;y<num_h_tiles;y++)
			{
				Uint32 c = m_Plane[plane].getMapDataAt(mpx, y+m_mapy);
				if(has_background && c==0)
					continue;

				Tilemap->drawTile(mp_scrollsurface, x, ((y<<4)+m_mapystripepos)&511, c);
				registerAnimation( x, ((y<<4)+m_mapystripepos)&511, c );
			}
		}
		else
			has_background = false;
	}
}

/////////////////////////
// Animation functions //
/////////////////////////
// searches for animated tiles at the map position (X,Y) and
// unregisters them from animtiles
void CMap::deAnimate(int x, int y)
{
	int px,py;
	// figure out pixel position of map tile (x,y)
    px = ((m_mapxstripepos+((x-m_mapx)<<4))&511);
    py = ((m_mapystripepos+((y-m_mapy)<<4))&511);
	
    // find it!
    for(int i=1;i<MAX_ANIMTILES-1;i++)
    {
		if (m_animtiles[i].x == px && m_animtiles[i].y == py)
		{
			m_animtiles[i].slotinuse = 0;
			m_animtiles[i].tile = 0;
			m_AnimTileInUse[px>>4][py>>4] = 0;
			return;
		}
    }
}

// Draw an animated tile. If it's not animated draw it anyway
void CMap::drawAnimatedTile(SDL_Surface *dst, Uint16 mx, Uint16 my, Uint16 tile)
{
	CTileProperties &TileProperty = g_pBehaviorEngine->getTileProperties().at(tile);
	std::vector<CTilemap>::iterator Tilemap = m_Tilemaps.begin()+1;

	if(!TileProperty.animationtime)
	{ // Unanimated tiles
		Tilemap->drawTile( dst, mx, my, tile );
	}
	else
	{ // animate animated tiles
		for(int i=1;i<MAX_ANIMTILES-1;i++)
		{
			if ( m_animtiles[i].slotinuse )
			{
				if(m_animtiles[i].x == mx+m_scrollx_buf &&
						m_animtiles[i].y == my+m_scrolly_buf)
				{
					Tilemap->drawTile( dst, mx, my,
							m_animtiles[i].tile );
				}
			}
		}
	}
}

void CMap::animateAllTiles()
{
	std::vector<CTilemap>::iterator Tilemap = m_Tilemaps.begin()+1;

	/* animate animated tiles */
	if (m_animation_enabled)
	{
		if(m_animtiletimer<ANIM_TILE_TIME)
			m_animtiletimer++;
		else
			m_animtiletimer = 0;
	}

	/* re-draw all animated tiles */
	for(int i=1;i<MAX_ANIMTILES-1;i++)
	{
		if ( m_animtiles[i].slotinuse )
		{
			CTileProperties &TileProperties =
					g_pBehaviorEngine->getTileProperties().at(m_animtiles[i].tile);

			if( (m_animtiletimer % TileProperties.animationtime) == 0)
			{
				Tilemap->drawTile( mp_scrollsurface, m_animtiles[i].x, m_animtiles[i].y,
						m_animtiles[i].tile);
				// get the next tile which will be drawn in the next animation cycle
				m_animtiles[i].tile += TileProperties.nextTile;
			}
		}
	}
}

// unregisters all animated tiles with baseframe tile
void CMap::unregisterAnimtiles(int tile)
{
	/*int i;
	for(i=0;i<MAX_ANIMTILES-1;i++)
	{
        if (m_animtiles[i].baseframe == tile)
			m_animtiles[i].slotinuse = 0;
	}*/
}

// register the tiles which has to be animated
void CMap::registerAnimation(Uint32 x, Uint32 y, int c)
{
	// we just drew over an animated tile which we must unregister
    if (m_AnimTileInUse[x>>4][y>>4])
    {
		m_animtiles[m_AnimTileInUse[x>>4][y>>4]].slotinuse = 0;
		m_AnimTileInUse[x>>4][y>>4] = 0;
    }

	CTileProperties &TileProperty =
			g_pBehaviorEngine->getTileProperties().at(c);
    // we just drew an animated tile which we will now register
	if( TileProperty.animationtime )
    {
		for(int i=1 ; i<MAX_ANIMTILES-1 ; i++)
		{
			if (!m_animtiles[i].slotinuse)
			{  // we found an unused slot
				m_animtiles[i].x = x;
				m_animtiles[i].y = y;
				m_animtiles[i].tile = c;
				m_animtiles[i].slotinuse = 1;
				m_AnimTileInUse[x>>4][y>>4] = i;
				break;
			}
		}
    }
}

CMap::~CMap() {
	memset( m_AnimTileInUse, 0, sizeof(m_AnimTileInUse));
	memset( m_animtiles, 0, sizeof(m_animtiles));
}



