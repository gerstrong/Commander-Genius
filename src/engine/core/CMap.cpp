/*
 * CMap.cpp
 *
 *  Created on: 21.09.2009
 *      Author: gerstrong
 */

#include "CMap.h"
#include "engine/core/CBehaviorEngine.h"
#include <base/utils/FindFile.h>
#include <base/GsLogging.h>
#include <base/video/CVideoDriver.h>
#include <base/GsTimer.h>
#include "graphics/GsGraphics.h"
#include <iostream>
#include <fstream>

CMap::CMap():
m_width(0), m_height(0),
m_worldmap(false),
m_animation_enabled(true),
m_Dark(false),
mNumFuses(0),
mFuseInLevel(false),
m_Tilemaps(gGraphics.getTileMaps()),
mAnimtileTimer(0.0f),
mLocked(false),
mShakeCounter(0),
mMaxShakeCounter(0),
mMaxShakeVAmt(0),
mShakeDir(0)
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

    gVideoDriver.mpVideoEngine->resetScrollbuffer();

    m_scrollpix = m_scrollpixy = 0;
    m_mapx = m_mapy = 0;           // map X location shown at scrollbuffer row 0
    m_mapxstripepos = m_mapystripepos = 0;  // X pixel position of next stripe row
}


void CMap::refreshStripes()
{
    const int oldx = m_mapx<<4;
    const int oldy = m_mapy<<4;

    resetScrolls();

    gotoPos(oldx, oldy);
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

    scrollBlockY.push_back(2<<CSF);
    scrollBlockX.push_back(2<<CSF);

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
                {
                    scrollBlockY.push_back(y<<(CSF));
                }
                // In Keen 5 it is only used on the map and stands for the teleporter from some in level
                if(*map_ptr == 0x1A && ep != 5)
                    scrollBlockX.push_back(x<<(CSF));

                map_ptr++;
            }
        }

    }

    scrollBlockY.push_back((m_height-2)<<(CSF));
    scrollBlockX.push_back((m_width-2)<<(CSF));

}

void CMap::fetchNearestVertBlockers(const int x, int &leftCoord, int &rightCoord)
{
    int blockXleft = 0;
    int blockXright = 0;

    if(scrollBlockX.empty())
    {
        leftCoord = blockXleft;
        rightCoord = blockXright;
        return;
    }

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

            if(leftCoord > (2<<CSF) &&  g_pBehaviorEngine->getEngine() == ENGINE_GALAXY)
            {
                // This will hide even more level blockers in Galaxy. In Vorticon
                // this is not needed
                leftCoord += (1<<CSF);
            }

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
    int blockYup = 0;
    int blockYdown = 0;

    if(scrollBlockY.empty())
    {
        upCoord = blockYup;
        downCoord = blockYdown;
        return;
    }


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

            if(g_pBehaviorEngine->getEngine() == ENGINE_GALAXY)
            {
                // This will hide even more level blockers in Galaxy. In Vorticon
                // this is not needed
                upCoord += (1<<CSF);
            }

            downCoord = blockYdown;
            return;
        }

        up++;
        down++;
    }
    upCoord = blockYup;
    downCoord = blockYdown;
}



void CMap::unlock()
{
    mLocked = false;
}

void CMap::lock()
{
    mLocked = true;
}


bool CMap::findVerticalScrollBlocker(const int x)
{
    int blockXleft = 0;
    int blockXright = 0;

    fetchNearestVertBlockers(x, blockXleft, blockXright);

    if(x < blockXleft)
        return true;
    return false;
}


bool CMap::findHorizontalScrollBlocker(const int y)
{
    int blockYup = 0;
    int blockYdown = 0;

    fetchNearestHorBlockers(y, blockYup, blockYdown);

    if(y < blockYup)
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
    const int drawMask = gVideoDriver.getScrollSurface()->w-1;

	if( setTile( x, y, t ) )
	{
        m_Tilemaps.at(1).drawTile(gVideoDriver.getScrollSurface(), (x<<4)&drawMask, (y<<4)&drawMask, t);
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
    SDL_Rect gameres = gVideoDriver.getGameResolution().SDLRect();
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
    {
        for( int scrollx=0 ; scrollx<dx ; scrollx++)
        {
            scrollRight(true);
        }
    }
    else
    {
        retval = true;
    }

	if( dx < 0 )
		for( int scrollx=0 ; scrollx<-dx ; scrollx++) scrollLeft(true);
	else retval = true;

	if( dy > 0 )
		for( int scrolly=0 ; scrolly<dy ; scrolly++) scrollDown(true);
	else retval = true;

	if( dy < 0 )
		for( int scrolly=0 ; scrolly<-dy ; scrolly++) scrollUp(true);
	else retval = true;

    calcVisibleArea();
    refreshVisibleArea();

	return retval;
}

// scrolls the map one pixel right
bool CMap::scrollRight(const bool force)
{
    const int res_width = gVideoDriver.getGameResolution().w;

    if( !force && findVerticalScrollBlocker((m_scrollx+res_width)<<STC) )
		return false;

    const int squareSize = gVideoDriver.getScrollSurface()->w;

    if(m_scrollx < ((m_width-2)<<4) - res_width)
	{
		m_scrollx++;
        gVideoDriver.mpVideoEngine->UpdateScrollBufX(m_scrollx, squareSize-1);

		m_scrollpix++;
        if (m_scrollpix >= 16)
		{  // need to draw a new stripe
            const int totalNumTiles = squareSize/16;
            drawVstripe(m_mapxstripepos, m_mapx + totalNumTiles);
			m_mapx++;
			m_mapxstripepos += 16;
            if (m_mapxstripepos >= squareSize) m_mapxstripepos = 0;
			m_scrollpix = 0;
		}

        refreshVisibleArea();
		return true;
	}
	return false;
}

// scrolls the map one pixel left
bool CMap::scrollLeft(const bool force)
{
    if( !force && findVerticalScrollBlocker((m_scrollx)<<STC) )
        return false;

    const int squareSize = gVideoDriver.getScrollSurface()->w;

	if( m_scrollx > 32 )
	{
		m_scrollx--;
        gVideoDriver.mpVideoEngine->UpdateScrollBufX(m_scrollx, squareSize-1);

		if (m_scrollpix==0)
		{  // need to draw a new stripe
			if(m_mapx>0) m_mapx--;
			if (m_mapxstripepos == 0)
			{
                m_mapxstripepos = (squareSize - 16);
			}
			else
			{
				m_mapxstripepos -= 16;
			}
			drawVstripe(m_mapxstripepos, m_mapx);

			m_scrollpix = 15;
		} else m_scrollpix--;

        refreshVisibleArea();
		return true;
	}
	return false;
}

bool CMap::scrollDown(const bool force)
{
    const int res_height = gVideoDriver.getGameResolution().h;

    if( !force && findHorizontalScrollBlocker((m_scrolly+res_height)<<STC) )
		return false;

    const int squareSize = gVideoDriver.getScrollSurface()->w;

	if(m_scrolly < ((m_height-2)<<4) - res_height )
	{
		m_scrolly++;
        gVideoDriver.mpVideoEngine->UpdateScrollBufY(m_scrolly, squareSize-1);

		m_scrollpixy++;
		if (m_scrollpixy>=16)
		{  // need to draw a new stripe
            const int totalNumTiles = squareSize/16;
            drawHstripe(m_mapystripepos, m_mapy + totalNumTiles);
			m_mapy++;
			m_mapystripepos += 16;
            if (m_mapystripepos >= squareSize) m_mapystripepos = 0;
			m_scrollpixy = 0;
		}

        refreshVisibleArea();
		return true;
	}
	return false;
}




bool CMap::scrollUp(const bool force)
{
    if( !force && findHorizontalScrollBlocker((m_scrolly-1)<<STC) )
		return false;

    const int squareSize = gVideoDriver.getScrollSurface()->w;

	if( m_scrolly > 32 )
	{
		m_scrolly--;
        gVideoDriver.mpVideoEngine->UpdateScrollBufY(m_scrolly, squareSize-1);

		if (m_scrollpixy==0)
		{  // need to draw a new stripe
			if(m_mapy>0) m_mapy--;
			if (m_mapystripepos == 0)
			{
                m_mapystripepos = (squareSize - 16);
			}
			else
			{
				m_mapystripepos -= 16;
			}
			drawHstripe(m_mapystripepos, m_mapy);

			m_scrollpixy = 15;
		} else m_scrollpixy--;

        refreshVisibleArea();

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



void CMap::calcVisibleArea()
{
    // Here we need to get the scroll boundaries and
    // derive the rect from it.
    int x2, y2;
    fetchNearestVertBlockers( mGamePlayPos.x, mVisArea.x, x2);
    fetchNearestHorBlockers( mGamePlayPos.y, mVisArea.y, y2);

    mVisArea.w = x2 - mVisArea.x;
    mVisArea.h = y2 - mVisArea.y;
}


void CMap::refreshVisibleArea()
{
    GsRect<int> relativeVisGameArea;

    relativeVisGameArea.x = (mVisArea.x>>STC)-m_scrollx;
    relativeVisGameArea.y = (mVisArea.y>>STC)-m_scrolly;
    relativeVisGameArea.w = (mVisArea.w>>STC)-16;
    relativeVisGameArea.h = (mVisArea.h>>STC)-16;

    GsRect<int> gameResolution(gVideoDriver.getGameResolution());

    // Using the GameResolution to intersect the
    // calculated visible area we get another one
    // which is the rect allowed for blit operations
    gameResolution.intersect(relativeVisGameArea);

    gVideoDriver.mpVideoEngine->mRelativeVisGameArea = gameResolution;
}



void CMap::redrawAt(const Uint32 mx, const Uint32 my)
{
    SDL_Surface *ScrollSurface = gVideoDriver.getScrollSurface();
	// Go throught the list and just draw all the tiles that need to be animated
	const Uint32 num_h_tiles = ScrollSurface->h/16;
	const Uint32 num_v_tiles = ScrollSurface->w/16;

    const int drawMask = gVideoDriver.getScrollSurface()->w-1;

	if(  mx >= m_mapx && my >= m_mapy &&
			mx < m_mapx + num_v_tiles && my < m_mapy + num_h_tiles 	)
	{
        const Uint16 loc_x = (((mx-m_mapx)<<4)+m_mapxstripepos)&drawMask;
        const Uint16 loc_y = (((my-m_mapy)<<4)+m_mapystripepos)&drawMask;

		const size_t bg = m_Plane[0].getMapDataAt(mx, my);
		const size_t fg = m_Plane[1].getMapDataAt(mx, my);

		m_Tilemaps.at(0).drawTile(ScrollSurface, loc_x, loc_y, bg);
		if(fg)
		  m_Tilemaps.at(1).drawTile(ScrollSurface, loc_x, loc_y, fg);
	}
}

// draws all the map area. This is used for the title screen, when game starts and other passive scenes.
// Don't use it, when the game is scrolling.
// For an faster update of tiles use redrawAt instead.
void CMap::drawAll()
{
    refreshVisibleArea();

    SDL_Surface *ScrollSurface = gVideoDriver.getScrollSurface();

    const int drawMask = ScrollSurface->w-1;

    Uint32 num_h_tiles = ScrollSurface->h/16;
    Uint32 num_v_tiles = ScrollSurface->w/16;

    gVideoDriver.mpVideoEngine->UpdateScrollBufX(m_scrollx, drawMask);
    gVideoDriver.mpVideoEngine->UpdateScrollBufY(m_scrolly, drawMask);

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

            m_Tilemaps.at(0).drawTile(ScrollSurface, ((x<<4)+m_mapxstripepos)&drawMask,((y<<4)+m_mapystripepos)&drawMask, bg);
            if(fg)
                m_Tilemaps.at(1).drawTile(ScrollSurface, ((x<<4)+m_mapxstripepos)&drawMask,((y<<4)+m_mapystripepos)&drawMask, fg);
        }
    }
}

// draw a horizontal stripe, for vertical scrolling
void CMap::drawHstripe(unsigned int y, unsigned int mpy)
{
	if(mpy >= m_height) return;

    SDL_Surface *ScrollSurface = gVideoDriver.getScrollSurface();
	Uint32 num_v_tiles= ScrollSurface->w/16;

    const int drawMask = gVideoDriver.getScrollSurface()->w-1;

	if( num_v_tiles+m_mapx >= m_width )
		num_v_tiles = m_width-m_mapx;

	for(Uint32 x=0;x<num_v_tiles;x++)
	{
	  Uint32 bg = m_Plane[0].getMapDataAt(x+m_mapx, mpy);
	  Uint32 fg = m_Plane[1].getMapDataAt(x+m_mapx, mpy);

      m_Tilemaps.at(0).drawTile(ScrollSurface, ((x<<4)+m_mapxstripepos)&drawMask, y, bg);
	  if(fg)
        m_Tilemaps.at(1).drawTile(ScrollSurface, ((x<<4)+m_mapxstripepos)&drawMask, y, fg);
	}
}

// draws a vertical stripe from map position mapx to scrollbuffer position x
void CMap::drawVstripe(unsigned int x, unsigned int mpx)
{
    SDL_Surface *ScrollSurface = gVideoDriver.getScrollSurface();

    const int drawMask = ScrollSurface->w-1;

	if(mpx >= m_width) return;

	Uint32 num_h_tiles= ScrollSurface->h/16;

	if( num_h_tiles+m_mapy >= m_height )
		num_h_tiles = m_height-m_mapy;

	for(Uint32 y=0;y<num_h_tiles;y++)
	{
	  Uint32 bg = m_Plane[0].getMapDataAt(mpx, y+m_mapy);
	  Uint32 fg = m_Plane[1].getMapDataAt(mpx, y+m_mapy);

      m_Tilemaps.at(0).drawTile(ScrollSurface, x, ((y<<4)+m_mapystripepos)&drawMask, bg);
	  if(fg)
        m_Tilemaps.at(1).drawTile(ScrollSurface, x, ((y<<4)+m_mapystripepos)&drawMask, fg);
	}
}


void CMap::triggerShake( const unsigned int cycles, const unsigned int vAmount )
{
    mMaxShakeVAmt = vAmount;
    mMaxShakeCounter = cycles;
    mShakeDir = 1;
}


void CMap::renderShaking()
{
    if(mMaxShakeCounter == 0)
        return;

    if(mShakeCounter < mMaxShakeCounter)
    {
        // shake by changing the relative position
        if(mShakeDir > 0)
        { // Shake downwards
            for(int i=0 ; i<mMaxShakeVAmt ; i++)
                scrollDown();

            mShakeDir = -1;
        }
        else
        { // Shake upwards
            for(int i=0 ; i<mMaxShakeVAmt ; i++)
                scrollUp();

            mShakeDir = 1;
        }
        mShakeCounter++;
    }
    else
    {
        // Stop the shaking
        mMaxShakeCounter = 0;
        mShakeDir = 0;
        mShakeCounter = 0;
    }
}



/**
 * \brief This function draws all the masked and foreground tiles
 * \TODO: I think this function should be splat up into vorticon and galaxy engine function somehow.
 */
void CMap::_drawForegroundTiles()
{
    // TODO: This event pushing here is to much information that is pushed to the event list. Please reduce this!
    SDL_Surface *surface = gVideoDriver.getBlitSurface();
	const Uint16 num_h_tiles = surface->h;
	const Uint16 num_v_tiles = surface->w;
    Uint16 x1 = m_scrollx>>TILE_S;
    Uint16 y1 = m_scrolly>>TILE_S;
    Uint16 x2 = (m_scrollx+num_v_tiles)>>TILE_S;
    Uint16 y2 = (m_scrolly+num_h_tiles)>>TILE_S;

	std::vector<CTileProperties> &TileProperties =
			g_pBehaviorEngine->getTileProperties(1);

    auto visGA = gVideoDriver.mpVideoEngine->mRelativeVisGameArea;

    for( size_t y=y1 ; y<=y2 ; y++)
    {
        for( size_t x=x1 ; x<=x2 ; x++)
        {
            Uint16 fg = m_Plane[1].getMapDataAt(x,y);

            const int loc_x = (x<<TILE_S)-m_scrollx;
            const int loc_y = (y<<TILE_S)-m_scrolly;

            if(fg != 0)
            {
                if(TileProperties[fg].behaviour < 0)
                {
                    if( loc_x+16 < visGA.x || loc_x > visGA.x+visGA.w )
                        continue;

                    if( loc_y+16 < visGA.y || loc_y > visGA.y+visGA.h )
                        continue;

                    m_Tilemaps[1].drawTile(surface, loc_x, loc_y, fg );
                }
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
    m_Tilemaps[1].drawTile( gVideoDriver.getBlitSurface(), mx, my, tile);
}

void CMap::animateAllTiles()
{
    if(!m_animation_enabled)
        return;

    if(gVideoDriver.getRefreshSignal())
    {
        refreshStripes();
        drawAll();

        gVideoDriver.blitScrollSurface();
        gVideoDriver.setRefreshSignal(false);
    }


    SDL_Surface *ScrollSurface = gVideoDriver.getScrollSurface();

    const int drawMask = ScrollSurface->w-1;


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
                const Uint16 loc_x = (((x-m_mapx)<<4)+m_mapxstripepos)&drawMask;
                const Uint16 loc_y = (((y-m_mapy)<<4)+m_mapystripepos)&drawMask;
                m_Tilemaps.at(0).drawTile(ScrollSurface, loc_x, loc_y, bg);
                if(fg)
                    m_Tilemaps.at(1).drawTile(ScrollSurface, loc_x, loc_y, fg);
            }
        }
    }
}


