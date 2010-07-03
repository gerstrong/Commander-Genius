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
m_animtiletimer(0),
m_Background(false)
{
	resetScrolls();
	memset(m_objectlayer, 0, sizeof(m_objectlayer));
}

////////////////////////////
// Initialization Routine //
////////////////////////////


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
bool CMap::changeTile(Uint16 x, Uint16 y, Uint16 t)
{
	if( setTile( x, y, t ) )
	{
		m_Tilemaps.at(1).drawTile(mp_scrollsurface, (x<<4)&511, (y<<4)&511, t);
		registerAnimation( (x<<4)&511, (y<<4)&511, 0, t );
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
	if( m_Background )
	{
		size_t bg = m_Plane[0].getMapDataAt(mx, my);
		size_t fg = m_Plane[1].getMapDataAt(mx, my);
		m_Tilemaps.at(0).drawTile(mp_scrollsurface, (mx<<4)&511, (my<<4)&511, bg);
		m_Tilemaps.at(1).drawTile(mp_scrollsurface, (mx<<4)&511, (my<<4)&511, fg);
		registerAnimation( (mx<<4)&511, (my<<4)&511, bg, fg );
	}
	else
	{
		size_t fg = m_Plane[1].getMapDataAt(mx, my);
		m_Tilemaps.at(1).drawTile(mp_scrollsurface, (mx<<4)&511, (my<<4)&511, fg);
		registerAnimation( (mx<<4)&511, (my<<4)&511, 0, fg );
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
				registerAnimation( ((x<<4)+m_mapxstripepos)&511,((y<<4)+m_mapystripepos)&511, bg, fg );
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
				registerAnimation( ((x<<4)+m_mapxstripepos)&511,((y<<4)+m_mapystripepos)&511, 0, fg );
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
			registerAnimation( ((x<<4)+m_mapxstripepos)&511, y, bg, fg );
		}
	}
	else
	{
		for(Uint32 x=0;x<num_v_tiles;x++)
		{
			Uint32 fg = m_Plane[1].getMapDataAt(x+m_mapx, mpy);
			m_Tilemaps.at(1).drawTile(mp_scrollsurface, ((x<<4)+m_mapxstripepos)&511, y, fg);
			registerAnimation( ((x<<4)+m_mapxstripepos)&511, y, 0, fg );
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
		registerAnimation( x, ((y<<4)+m_mapystripepos)&511, bg, fg );
	}
	}
	else
	{
		for(Uint32 y=0;y<num_h_tiles;y++)
		{
			Uint32 fg = m_Plane[1].getMapDataAt(mpx, y+m_mapy);

			m_Tilemaps.at(1).drawTile(mp_scrollsurface, x, ((y<<4)+m_mapystripepos)&511, fg);
			registerAnimation( x, ((y<<4)+m_mapystripepos)&511, 0, fg );
		}
	}
}

/////////////////////////
// Animation functions //
/////////////////////////
// searches for animated tiles at the map position (X,Y) and
// unregisters them from animtiles
/**
 * auxiliary list predictor which finds elements at some coordinates
 */
class CMap::usedCoord
{
public:
	usedCoord(size_t x, size_t y): m_x(x), m_y(y) {};

  bool operator() (const stAnimationSlot& value)
  {  return (m_x == value.x && m_y == value.y ); }
private : size_t m_x,m_y;
};

void CMap::deAnimate(int x, int y)
{
    const int px = ((m_mapxstripepos+((x-m_mapx)<<4))&511);
    const int py = ((m_mapystripepos+((y-m_mapy)<<4))&511);
   	m_AnimationSlots.remove_if(usedCoord(px, py));
}

// Draw an animated tile. If it's not animated draw it anyway
void CMap::drawAnimatedTile(SDL_Surface *dst, Uint16 mx, Uint16 my, Uint16 tile)
{
	CTileProperties &TileProperty = g_pBehaviorEngine->getTileProperties(1).at(tile);

	if(TileProperty.animationtime)
	{ // animate animated tile

		std::list<stAnimationSlot>::iterator slot = m_AnimationSlots.begin();
		for( ; slot != m_AnimationSlots.end() ; slot++ )
		{
			if( (slot->x == static_cast<size_t>(mx+m_scrollx_buf)) &&
				(slot->y == static_cast<size_t>(my+m_scrolly_buf)) )
			{
				m_Tilemaps.at(0).drawTile( dst, mx, my, slot->fgtile );
			}
		}
	}
	else
	{ // Unanimated tile
		m_Tilemaps.at(1).drawTile( dst, mx, my, tile );
	}

}

void CMap::animateAllTiles()
{
	// Let the animation timer tick!!
	m_animtiletimer++;

	// Go throught the list and just draw all the tiles that need to be animated
	std::list<stAnimationSlot>::iterator slot = m_AnimationSlots.begin();
	if(m_Background)
	{
		for( ; slot != m_AnimationSlots.end() ; slot++ )
		{	// Get the right property of the tile
			CTileProperties &TilePropertiesBG =
					g_pBehaviorEngine->getTileProperties(0).at(slot->bgtile);
			CTileProperties &TilePropertiesFG =
					g_pBehaviorEngine->getTileProperties(1).at(slot->fgtile);


			m_Tilemaps.at(0).drawTile( mp_scrollsurface, slot->x, slot->y, slot->bgtile);
			if(slot->fgtile)
				m_Tilemaps.at(1).drawTile( mp_scrollsurface, slot->x, slot->y, slot->fgtile);

			// get the next tile which will be drawn in the next animation cycle

			if(TilePropertiesBG.animationtime)
				if( (m_animtiletimer % TilePropertiesBG.animationtime) == 0)
					slot->bgtile += TilePropertiesBG.nextTile;
			if(TilePropertiesFG.animationtime)
				if( (m_animtiletimer % TilePropertiesFG.animationtime) == 0)
					slot->fgtile += TilePropertiesFG.nextTile;
		}
	}
	else
	{
		for( ; slot != m_AnimationSlots.end() ; slot++ )
		{	// Get the right property of the tile
			CTileProperties &TilePropertiesFG =
					g_pBehaviorEngine->getTileProperties(1).at(slot->fgtile);

			m_Tilemaps.at(1).drawTile( mp_scrollsurface, slot->x, slot->y, slot->fgtile);


			// get the next tile which will be drawn in the next animation cycle
			if(TilePropertiesFG.animationtime)
				if( (m_animtiletimer % TilePropertiesFG.animationtime) == 0)
					slot->fgtile += TilePropertiesFG.nextTile;
		}
	}
}

// register the tiles which has to be animated
void CMap::registerAnimation(Uint32 x, Uint32 y, size_t bg, size_t fg)
{
	// first check if some of the Animation slots are out of place and remove them
	m_AnimationSlots.remove_if(usedCoord(x, y));

	CTileProperties &TilePropertyFG =
			g_pBehaviorEngine->getTileProperties(1).at(fg);
	// we just drew an animated tile which we will now register
	if( m_Background )
	{
		CTileProperties &TilePropertyBG =
				g_pBehaviorEngine->getTileProperties(0).at(bg);

		if( TilePropertyBG.animationtime && m_Background )
		{
			stAnimationSlot NewAnimationSlot;
			NewAnimationSlot.x = x;
			NewAnimationSlot.y = y;
			NewAnimationSlot.bgtile = bg;
			NewAnimationSlot.fgtile = fg;
			m_AnimationSlots.push_back(NewAnimationSlot);
		}
	}

	if( TilePropertyFG.animationtime )
	{
		stAnimationSlot NewAnimationSlot;
		NewAnimationSlot.x = x;
		NewAnimationSlot.y = y;
		NewAnimationSlot.bgtile = bg;
		NewAnimationSlot.fgtile = fg;
		m_AnimationSlots.push_back(NewAnimationSlot);
	}
}

CMap::~CMap() {
}



