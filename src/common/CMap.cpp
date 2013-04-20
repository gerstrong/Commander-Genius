/*
 * CMap.cpp
 *
 *  Created on: 21.09.2009
 *      Author: gerstrong
 */

#include "CMap.h"
#include "common/CBehaviorEngine.h"
#include "FindFile.h"
#include "CLogFile.h"
#include "sdl/CVideoDriver.h"
#include "sdl/CTimer.h"
#include "graphics/CGfxEngine.h"
#include <iostream>
#include <fstream>

CMap::CMap():
m_width(0), m_height(0),
m_worldmap(false),
m_animation_enabled(true),
m_Dark(false),
mNumFuses(0),
mFuseInLevel(false),
m_Tilemaps(g_pGfxEngine->getTileMaps()),
mAnimtileTimer(0.0f)
{
	resetScrolls();
	m_Level = 0;
	isSecret = false;
}

////////////////////////////
// Initialization Routine //
////////////////////////////

void CMap::setLevel( const Uint16 level )
{
    m_Level = level;

}

Uint16 CMap::getLevel()
{
    return m_Level;
}

void CMap::setLevelName( const std::string& name )
{
    m_LevelName = name;
}

std::string CMap::getLevelName()
{
    return m_LevelName;
}


/**
 * \brief	Create an empty data plane used for the map data
 * \param	blocksize	size in bytes of the datablock that has to be created
 */
bool CMap::createEmptyDataPlane(size_t plane, Uint32 width, Uint32 height)
{
	m_width = width;
	m_height = height;
	m_Plane[plane].createDataMap(m_width, m_height);
	
	return true;
}

void CMap::resetScrolls()
{
	m_scrollx = m_scrolly = 0;

	g_pVideoDriver->mpVideoEngine->resetScrollbuffer();

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
		return m_Plane[t].getMapDataAt(x,y);
	else
		return 0;
}

//////////////////////////
// returns the object/sprite/level which is set at the given coordinates
Uint16 CMap::getObjectat(Uint16 x, Uint16 y)
{
    return at(x,y,2);
}

/**
 * \brief	Gets the pointer to the plane data of the map
 * \param	PlaneNum number of the requested Plane
 */
word *CMap::getData(Uint8 PlaneNum)
{
	return m_Plane[PlaneNum].getMapDataPtr();
}

word *CMap::getInfoData()
{
	return m_Plane[2].getMapDataPtr();
}

word *CMap::getForegroundData()
{
	return m_Plane[1].getMapDataPtr();
}

word *CMap::getBackgroundData()
{
	return m_Plane[0].getMapDataPtr();
}

void CMap::collectBlockersCoordiantes()
{
    scrollBlockX.clear();
    scrollBlockY.clear();

    scrollBlockY.push_back(1<<CSF);
    scrollBlockX.push_back(1<<CSF);

    int ep = g_pBehaviorEngine->getEpisode();

    if(g_pBehaviorEngine->getEngine() == ENGINE_GALAXY)
    {
	const word* map_ptr = m_Plane[2].getMapDataPtr();

	for(int y=0 ; y<(int)m_height ; y++)
	{
	    for(int x=0 ; x<(int)m_width ; x++)
	    {
		// Check the row for a blocker which has the proper value
		if(*map_ptr == 0x19)
		    scrollBlockY.push_back(y<<(CSF));
		// In Keen 5 it is only used on the map and stands for the teleporter from some in level
		if(*map_ptr == 0x1A && ep != 5)
		    scrollBlockX.push_back(x<<(CSF));

		map_ptr++;
	    }
	}

    }

    scrollBlockY.push_back((m_height-1)<<(CSF));
    scrollBlockX.push_back((m_width-1)<<(CSF));

}

void CMap::fetchNearestVertBlockers(const int x, int &leftCoord, int &rightCoord)
{
    int blockXleft = 0;
    int blockXright = 0;
    std::vector<int>::iterator left  = scrollBlockX.begin();
    std::vector<int>::iterator right = left;
    right++;

    // Find the vertical edges coordinates
    for( ; right != scrollBlockX.end() ; )
    {
	blockXleft = *left;
	blockXright = *right;

	if( x > blockXleft && x < blockXright )
	{
	    leftCoord = blockXleft;
	    rightCoord = blockXright;
	    return;
	}

	left++;
	right++;
    }

    leftCoord = blockXleft;
    rightCoord = blockXright;
}

void CMap::fetchNearestHorBlockers(const int y, int &upCoord, int &downCoord)
{
    int blockYup, blockYdown;
    std::vector<int>::iterator up = scrollBlockY.begin();
    std::vector<int>::iterator down= up;
    down++;

    blockYup = *up;
    blockYdown = *down;

    for( ; down != scrollBlockY.end() ; )
    {
	blockYup = *up;
	blockYdown = *down;

	if( y > blockYup && y < blockYdown )
	{
	    upCoord = blockYup;
	    downCoord = blockYdown;
	    return;
	}

	up++;
	down++;
    }
    upCoord = blockYup;
    downCoord = blockYdown;
}




bool CMap::findVerticalScrollBlocker(const int x)
{
    int blockXleft = 0;
    int blockXright = 0;

    fetchNearestVertBlockers(x, blockXleft, blockXright);

    if(x-(1<<CSF) < blockXleft)
	return true;
    return false;
}


bool CMap::findHorizontalScrollBlocker(const int y)
{
    int blockYup, blockYdown;
    fetchNearestHorBlockers(y, blockYup, blockYdown);

    if(y-(1<<CSF) < blockYup)
	return true;
    return false;
}


// searches the map's object layer for object OBJ.
// if it is found returns non-zero and places the
// coordinates of the first occurrence of the object
// in (xout,yout)
bool CMap::findObject(unsigned int obj, int *xout, int *yout)
{
	unsigned int x,y;

	for(y=2;y<m_height-2;y++)
	{
		for(x=2;x<m_width-2;x++)
		{
			if (m_Plane[2].getMapDataAt(x,y)==obj)
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
		m_Tilemaps.at(1).drawTile(g_pVideoDriver->getScrollSurface(), (x<<4)&511, (y<<4)&511, t);
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
	SDL_Rect gameres = g_pVideoDriver->getGameResolution().SDLRect();
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
		for( int scrollx=0 ; scrollx<dx ; scrollx++) scrollRight(true);
	else retval = true;

	if( dx < 0 )
		for( int scrollx=0 ; scrollx<-dx ; scrollx++) scrollLeft(true);
	else retval = true;

	if( dy > 0 )
		for( int scrolly=0 ; scrolly<dy ; scrolly++) scrollDown(true);
	else retval = true;

	if( dy < 0 )
		for( int scrolly=0 ; scrolly<-dy ; scrolly++) scrollUp(true);
	else retval = true;

	return retval;
}

// scrolls the map one pixel right
bool CMap::scrollRight(const bool force)
{
        const int res_width = g_pVideoDriver->getGameResolution().w;

    	if( !force && findVerticalScrollBlocker((m_scrollx+res_width)<<STC) )
		return false;


	if(m_scrollx < ((m_width-2)<<4) - g_pVideoDriver->getGameResolution().w)
	{
		m_scrollx++;
		g_pVideoDriver->mpVideoEngine->UpdateScrollBufX(m_scrollx);

		m_scrollpix++;
		if (m_scrollpix>=16)
		{  // need to draw a new stripe
			drawVstripe(m_mapxstripepos, m_mapx + 32);
			m_mapx++;
			m_mapxstripepos += 16;
			if (m_mapxstripepos >= 512) m_mapxstripepos = 0;
			m_scrollpix = 0;
		}
		return true;
	}
	return false;
}

// scrolls the map one pixel left
bool CMap::scrollLeft(const bool force)
{
    	if( !force && findVerticalScrollBlocker((m_scrollx-2)<<STC) )
		return false;

	if( m_scrollx > 32 )
	{
		m_scrollx--;
		g_pVideoDriver->mpVideoEngine->UpdateScrollBufX(m_scrollx);

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
		return true;
	}
	return false;
}

bool CMap::scrollDown(const bool force)
{
	const int res_height = g_pVideoDriver->getGameResolution().h;

	if( !force && findHorizontalScrollBlocker((m_scrolly+res_height)<<STC) )
		return false;

	if(m_scrolly < ((m_height-2)<<4) - res_height )
	{
		m_scrolly++;
		g_pVideoDriver->mpVideoEngine->UpdateScrollBufY(m_scrolly);

		m_scrollpixy++;
		if (m_scrollpixy>=16)
		{  // need to draw a new stripe
			drawHstripe(m_mapystripepos, m_mapy + 32);
			m_mapy++;
			m_mapystripepos += 16;
			if (m_mapystripepos >= 512) m_mapystripepos = 0;
			m_scrollpixy = 0;
		}
		return true;
	}
	return false;
}




bool CMap::scrollUp(const bool force)
{
	if( !force && findHorizontalScrollBlocker((m_scrolly-1)<<STC) )
		return false;

	if( m_scrolly > 32 )
	{
		m_scrolly--;
		g_pVideoDriver->mpVideoEngine->UpdateScrollBufY(m_scrolly);

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
		return true;
	}
	return false;
}




//////////////////////
// Drawing Routines //
//////////////////////
// Draws the entire map to the scroll buffer
// called at start of level to draw the upper-left corner of the map
// onto the scrollbuffer...from then on the map will only be drawn
// in stripes as it scrolls around.
void CMap::redrawAt(const Uint32 mx, const Uint32 my)
{
	SDL_Surface *ScrollSurface = g_pVideoDriver->getScrollSurface();
	// Go throught the list and just draw all the tiles that need to be animated
	const Uint32 num_h_tiles = ScrollSurface->h/16;
	const Uint32 num_v_tiles = ScrollSurface->w/16;

	if(  mx >= m_mapx && my >= m_mapy &&
			mx < m_mapx + num_v_tiles && my < m_mapy + num_h_tiles 	)
	{
		const Uint16 loc_x = (((mx-m_mapx)<<4)+m_mapxstripepos)&511;
		const Uint16 loc_y = (((my-m_mapy)<<4)+m_mapystripepos)&511;

		size_t fg = m_Plane[1].getMapDataAt(mx, my);
		m_Tilemaps.at(1).drawTile(ScrollSurface, loc_x, loc_y, fg);
	}
}

// draws all the map area. This is used for the title screen, when game starts and other passive scenes.
// Don't use it, when the game is scrolling.
// For an faster update of tiles use redrawAt instead.
void CMap::drawAll()
{
  SDL_Surface *ScrollSurface = g_pVideoDriver->getScrollSurface();
  
  Uint32 num_h_tiles = ScrollSurface->h/16;
  Uint32 num_v_tiles = ScrollSurface->w/16;
  
  g_pVideoDriver->mpVideoEngine->UpdateScrollBufX(m_scrollx);
  g_pVideoDriver->mpVideoEngine->UpdateScrollBufY(m_scrolly);
  
  if(num_v_tiles+m_mapx >= m_width)
    num_v_tiles = m_width-m_mapx;
  
  if(num_h_tiles+m_mapy >= m_height)
    num_h_tiles = m_height-m_mapy;
  
  for(Uint32 y=0;y<num_h_tiles;y++)
  {
    for(Uint32 x=0;x<num_v_tiles;x++)
    {
      Uint32 bg = m_Plane[0].getMapDataAt(x+m_mapx, y+m_mapy);
      Uint32 fg = m_Plane[1].getMapDataAt(x+m_mapx, y+m_mapy);
      
      m_Tilemaps.at(0).drawTile(ScrollSurface, ((x<<4)+m_mapxstripepos)&511,((y<<4)+m_mapystripepos)&511, bg);
      if(fg)
	m_Tilemaps.at(1).drawTile(ScrollSurface, ((x<<4)+m_mapxstripepos)&511,((y<<4)+m_mapystripepos)&511, fg);
    }
  }
}

// draw a horizontal stripe, for vertical scrolling
void CMap::drawHstripe(unsigned int y, unsigned int mpy)
{
	if(mpy >= m_height) return;
	SDL_Surface *ScrollSurface = g_pVideoDriver->getScrollSurface();
	Uint32 num_v_tiles= ScrollSurface->w/16;

	if( num_v_tiles+m_mapx >= m_width )
		num_v_tiles = m_width-m_mapx;

	for(Uint32 x=0;x<num_v_tiles;x++)
	{
	  Uint32 bg = m_Plane[0].getMapDataAt(x+m_mapx, mpy);
	  Uint32 fg = m_Plane[1].getMapDataAt(x+m_mapx, mpy);
	  
	  m_Tilemaps.at(0).drawTile(ScrollSurface, ((x<<4)+m_mapxstripepos)&511, y, bg);
	  if(fg)
	    m_Tilemaps.at(1).drawTile(ScrollSurface, ((x<<4)+m_mapxstripepos)&511, y, fg);
	}	
}

// draws a vertical stripe from map position mapx to scrollbuffer position x
void CMap::drawVstripe(unsigned int x, unsigned int mpx)
{
	SDL_Surface *ScrollSurface = g_pVideoDriver->getScrollSurface();

	if(mpx >= m_width) return;

	Uint32 num_h_tiles= ScrollSurface->h/16;

	if( num_h_tiles+m_mapy >= m_height )
		num_h_tiles = m_height-m_mapy;

	for(Uint32 y=0;y<num_h_tiles;y++)
	{
	  Uint32 bg = m_Plane[0].getMapDataAt(mpx, y+m_mapy);
	  Uint32 fg = m_Plane[1].getMapDataAt(mpx, y+m_mapy);
	  
	  m_Tilemaps.at(0).drawTile(ScrollSurface, x, ((y<<4)+m_mapystripepos)&511, bg);
	  if(fg)
	    m_Tilemaps.at(1).drawTile(ScrollSurface, x, ((y<<4)+m_mapystripepos)&511, fg);
	}
}

/**
 * \brief This function draws all the masked and foreground tiles
 * \TODO: I think this function should be splat up into vorticon and galaxy engine function somehow.
 */
void CMap::_drawForegroundTiles()
{
    // TODO: This event pushing here is to much information that is pushed to the event list. Please reduce this!
	SDL_Surface *surface = g_pVideoDriver->getBlitSurface();
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

			if(fg != 0)
			{
			   if(TileProperties[fg].behaviour < 0)
				m_Tilemaps[1].drawTile(surface, loc_x, loc_y, fg );
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
{	return mAnimtileTimer;	}

void CMap::drawAnimatedTile(SDL_Surface *dst, Uint16 mx, Uint16 my, Uint16 tile)
{
	g_pVideoDriver->mDrawTasks.add( new DrawAnimatedTileTask( &(m_Tilemaps[1]), mx, my, tile ) );
}

void CMap::animateAllTiles()
{
	if(!m_animation_enabled)
		return;

	if(g_pVideoDriver->getRefreshSignal())
	{
		drawAll();

		g_pVideoDriver->blitScrollSurface();
		g_pVideoDriver->setRefreshSignal(false);
	}


	SDL_Surface *ScrollSurface = g_pVideoDriver->getScrollSurface();

	// Let the animation timer tick!!
	mAnimtileTimer += 1.0f;
	const Uint8 animtileTimerInt = static_cast<Uint8>(mAnimtileTimer);

	if( mAnimtileTimer > 256.0f )
	    mAnimtileTimer = 0.0f;

	// Go throught the list and just draw all the tiles that need to be animated
	Uint32 num_h_tiles = ScrollSurface->h/16;
	Uint32 num_v_tiles = ScrollSurface->w/16;

	if(num_v_tiles+m_mapx >= m_width)
		num_v_tiles = m_width-m_mapx;

	if(num_h_tiles+m_mapy >= m_height)
		num_h_tiles = m_height-m_mapy;

	std::vector<CTileProperties> &frontTileProperties =
			g_pBehaviorEngine->getTileProperties(1);
	word *p_front_tile = m_Plane[1].getMapDataPtr();

	std::vector<CTileProperties> &backTileProperties =
	g_pBehaviorEngine->getTileProperties(0);
	word *p_back_tile = m_Plane[0].getMapDataPtr();
	for( size_t y=0 ; y<m_height ; y++)
	{
	  for( size_t x=0 ; x<m_width ; x++)
	  {
	    bool draw = false;
	    
	    CTileProperties &back_tile = backTileProperties[*p_back_tile];
	    if( back_tile.animationtime && (animtileTimerInt % back_tile.animationtime == 0) )
	    {
	      *p_back_tile += back_tile.nextTile;
	      draw = true;
	    }
	    p_back_tile++;
	    
	    
	    CTileProperties &front_tile = frontTileProperties[*p_front_tile];
	    if( front_tile.animationtime && (animtileTimerInt % front_tile.animationtime == 0) )
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
	      m_Tilemaps.at(0).drawTile(ScrollSurface, loc_x, loc_y, bg);
	      if(fg)
		m_Tilemaps.at(1).drawTile(ScrollSurface, loc_x, loc_y, fg);
	    }
	  }
	}
}


