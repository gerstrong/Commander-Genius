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

/**
 * \brief	Create an empty data plane used for the map data
 * \param	blocksize	size in bytes of the datablock that has to be created
 */
bool CMap::setupEmptyDataPlanes(const int numPlanes,
                                const int tileSize,
                                const Uint32 width,
                                const Uint32 height)
{    
    if(tileSize == 8)
    {
        mTileSizeBase = 3;
    }
    else
    {
        mTileSizeBase = 4;
    }


	m_width = width;
	m_height = height;

    mPlanes.clear();

    for(int i=0 ; i<numPlanes ; i++)
    {
        CPlane plane;
        plane.createDataMap(m_width, m_height);
        mPlanes.push_back(plane);
    }

	return true;
}

void CMap::resetScrolls()
{
    m_scrollx = m_scrolly = 0;

    gVideoDriver.resetScrollBuffers();

    m_scrollpix = m_scrollpixy = 0;
    m_mapx = m_mapy = 0;           // map X location shown at scrollbuffer row 0
    m_mapxstripepos = m_mapystripepos = 0;  // X pixel position of next stripe row
}


void CMap::refreshStripes()
{    
    const int oldx = m_mapx<<mTileSizeBase;
    const int oldy = m_mapy<<mTileSizeBase;

    resetScrolls();

    gotoPos(oldx, oldy);
}


/////////////////////////
// Getters and Setters //
/////////////////////////
// returns the tile which is set at the given coordinates
Uint16 CMap::at(int x, int y, int t)
{
    if(x < int(m_width) && y < int(m_height) )
    {
        return mPlanes[t].getMapDataAt(x,y);
    }
	else
    {
		return 0;
    }
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
word *CMap::getData(const Uint8 PlaneNum)
{
    assert(mPlanes.size() > PlaneNum);
    return mPlanes[PlaneNum].getMapDataPtr();
}

word *CMap::getInfoData()
{
    assert(mPlanes.size() > 2);
    return mPlanes[2].getMapDataPtr();
}

word *CMap::getForegroundData()
{
    assert(mPlanes.size() > 1);
    return mPlanes[1].getMapDataPtr();
}

word *CMap::getBackgroundData()
{
    assert(mPlanes.size() > 0);
    return mPlanes[0].getMapDataPtr();
}

void CMap::collectBlockersCoordiantes()
{
    scrollBlockX.clear();
    scrollBlockY.clear();

    insertVertBlocker(1<<CSF);
    insertHorBlocker(1<<CSF);

    int ep = gBehaviorEngine.getEpisode();

    if(gBehaviorEngine.getEngine() == ENGINE_GALAXY)
    {
        assert(mPlanes.size() > 2);
        const word* map_ptr = mPlanes[2].getMapDataPtr();

        for(int y=0 ; y<(int)m_height ; y++)
        {
            for(int x=0 ; x<(int)m_width ; x++)
            {
                // Check the row for a blocker which has the proper value
                if(*map_ptr == 0x19)
                {
                    insertHorBlocker(y<<(CSF));
                }

                // In Keen 5 it is only used on the map and stands for an in level teleporter
                if(*map_ptr == 0x1A && ep != 5)
                    insertVertBlocker(x<<(CSF));

                map_ptr++;
            }
        }

    }
    // There exist end-of-map tiles, we don't want to see
    insertHorBlocker(((m_height-2)<<(CSF)));
    insertVertBlocker((m_width-2)<<(CSF));
}

void CMap::setupAnimationTimerOfTile(const int tilemapIdx)
{
    auto &tileProperties = gBehaviorEngine.getTileProperties(tilemapIdx);

    assert(static_cast<decltype (mPlanes)::size_type>(tilemapIdx) < mPlanes.size());
    auto &curPlane = mPlanes[tilemapIdx];

    word *p_back_tile = curPlane.getMapDataPtr();
    auto &timers = curPlane.getTimers();

    for( size_t y=0 ; y<m_height ; y++)
    {
        const size_t stride = m_width*y;

        for( size_t x=0 ; x<m_width ; x++)
        {
            const auto offset = stride + x;
            timers[offset] = tileProperties[*p_back_tile].animationTime;
            p_back_tile++;
        }
    }
}

void CMap::setupAnimationTimer()
{
    assert(mPlanes.size() > 2);

    setupAnimationTimerOfTile(0);
    setupAnimationTimerOfTile(1);
}

void CMap::insertVertBlocker(const int x)
{
    scrollBlockX.insert(x);
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

    auto left  = scrollBlockX.begin();
    auto right = left;
    right++;

    // Find the vertical edges coordinates
    for( ; right != scrollBlockX.end() ; )
    {
        blockXleft = *left;
        blockXright = *right;

        if( x > blockXleft && x < blockXright )
        {
            leftCoord = blockXleft;

            if(leftCoord > (2<<CSF) &&  gBehaviorEngine.getEngine() == ENGINE_GALAXY)
            {
                // This will hide even more level blockers in Galaxy.
                // In the vorticon games not required
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

void CMap::insertHorBlocker(const int y)
{
    scrollBlockY.insert(y);
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


    auto up = scrollBlockY.begin();
    auto down = up;
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

            if(gBehaviorEngine.getEngine() == ENGINE_GALAXY)
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
    assert(mPlanes.size() > 2);

	unsigned int x,y;

	for(y=2;y<m_height-2;y++)
	{
		for(x=2;x<m_width-2;x++)
		{
            if (mPlanes[2].getMapDataAt(x,y)==obj)
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
// if it is found returns true and places the
// coordinates of the first occurance of the tile
// in (xout,yout)
bool CMap::findTile(const unsigned int tile,
                    int &xout,
                    int &yout,
                    const int plane)
{
    assert(int(mPlanes.size()) > plane);

	unsigned int x,y;

	for(y=2;y<m_height-2;y++)
	{
		for(x=2;x<m_width-2;x++)
		{
            if (mPlanes[plane].getMapDataAt(x,y) == tile)
			{
                xout = x;
                yout = y;
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
        assert(mPlanes.size() > plane);
        mPlanes[plane].setMapDataAt(t, x, y);
		return true;
	}
	else
		return false;
}

bool CMap::setTile(Uint16 x, Uint16 y, Uint16 t, bool redraw, Uint16 plane)
{
	if(setTile( x, y, t, plane))
	{
        if( redraw )
		{
			redrawAt(x,y);
		}
		return true;
	}

    return false;
}

// Called in level. This function does the same as setTile, but also draws directly to the scrollsurface.
// Used normally, when items are picked up
bool CMap::changeTile(Uint16 x, Uint16 y, Uint16 t)
{
    auto &scrollSfcVec = gVideoDriver.mpVideoEngine->getScrollSurfaceVec();

    if( setTile( x, y, t ) )
	{
        for( auto &scrollSfc : scrollSfcVec )
        {
            const int drawMask = scrollSfc.getScrollSurface().width()-1;
            m_Tilemaps.at(1).drawTile(scrollSfc,
                                      (x<<mTileSizeBase)&drawMask,
                                      (y<<mTileSizeBase)&drawMask, t);
        }

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
    {
        for(Uint16 c_x=x ; c_x<x2 ; c_x++)
        {
            changeTile(c_x, c_y, tile);
        }
    }
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
    {
        for( int scrollx=0 ; scrollx<-dx ; scrollx++) scrollLeft(true);
    }
	else retval = true;

	if( dy > 0 )
    {
        for( int scrolly=0 ; scrolly<dy ; scrolly++) scrollDown(true);
    }
	else retval = true;

	if( dy < 0 )
    {
        for( int scrolly=0 ; scrolly<-dy ; scrolly++) scrollUp(true);
    }
	else retval = true;

    calcVisibleArea();
    refreshVisibleArea();

	return retval;
}


// scrolls the map one pixel right
bool CMap::scrollRight(const bool force)
{
    const int res_width = gVideoDriver.getGameResolution().dim.x;

    if( !force && findVerticalScrollBlocker((m_scrollx+res_width)<<STC) )
		return false;

    if(m_scrollx >= ((m_width-2)<<mTileSizeBase) - res_width)
        return false;


    m_scrollx++;

    for(auto &scrollSfc : gVideoDriver.getScrollSurfaceVec() )
    {
        scrollSfc.UpdateScrollBufX(m_scrollx);
    }

    m_scrollpix++;
    if (m_scrollpix >= (1<<mTileSizeBase))
    {  // need to draw a new stripe

        // TODO: Problem with different squared sizes here
        auto &scrollSfc = gVideoDriver.getScrollSurface(0);
        const int squareSize = scrollSfc.getSquareSize();
        const int totalNumTiles = squareSize>>mTileSizeBase;
        drawVstripe(m_mapxstripepos, m_mapx + totalNumTiles);

        m_mapx++;
        m_mapxstripepos += (1<<mTileSizeBase);
        if (m_mapxstripepos >= squareSize) m_mapxstripepos = 0;
        m_scrollpix = 0;
    }

    refreshVisibleArea();
    return true;
}

// scrolls the map one pixel left
bool CMap::scrollLeft(const bool force)
{
    if( !force && findVerticalScrollBlocker((m_scrollx)<<STC) )
        return false;

    if( m_scrollx <= 32 )
        return false;

    m_scrollx--;

    for(auto &scrollSfc : gVideoDriver.getScrollSurfaceVec() )
    {
        scrollSfc.UpdateScrollBufX(m_scrollx);
    }

    if (m_scrollpix==0)
    {  // need to draw a new stripe
        if(m_mapx>0) m_mapx--;
        if (m_mapxstripepos == 0)
        {
            // TODO: Problem with different squared sizes here
            auto &scrollSfc = gVideoDriver.getScrollSurface(0);
            const int squareSize = scrollSfc.getSquareSize();
            m_mapxstripepos = (squareSize - (1<<mTileSizeBase));
        }
        else
        {
            m_mapxstripepos -= (1<<mTileSizeBase);
        }
        drawVstripe(m_mapxstripepos, m_mapx);

        m_scrollpix = (1<<mTileSizeBase)-1;
    } else m_scrollpix--;

    refreshVisibleArea();

    return true;

}

bool CMap::scrollDown(const bool force)
{
    const int res_height = gVideoDriver.getGameResolution().dim.y;

    if(!force)
    {
        const int bottom_y = (m_scrolly+res_height)<<STC;
        int blockYup = 0;
        int blockYdown = 0;

        fetchNearestHorBlockers(bottom_y, blockYup, blockYdown);

        const int new_bottom_y = ((m_scrolly+1)+res_height)<<STC;

        if(new_bottom_y >= blockYdown)
        {
            return false;
        }
    }

    m_scrolly++;
    m_scrollpixy++;

    for(auto &scrollSfc : gVideoDriver.getScrollSurfaceVec() )
    {
        scrollSfc.UpdateScrollBufY(m_scrolly);

        if ( m_scrollpixy >= (1<<mTileSizeBase) )
        {
            const int squareSize = scrollSfc.getSquareSize();

            if(m_scrolly < ((m_height-2)<<mTileSizeBase) - res_height )
            {
                // need to draw a new stripe
                const int totalNumTiles = squareSize>>mTileSizeBase;
                drawHstripe(m_mapystripepos, m_mapy + totalNumTiles);
            }

            m_mapy++;
            m_mapystripepos += (1<<mTileSizeBase);
            if (m_mapystripepos >= squareSize) m_mapystripepos = 0;
            m_scrollpixy = 0;
        }

        refreshVisibleArea();
    }
    return true;
}




bool CMap::scrollUp(const bool force)
{
    if( !force && findHorizontalScrollBlocker((m_scrolly-1)<<STC) )
        return false;

    if( m_scrolly <= 32 )
        return false;

    m_scrolly--;

    for(auto &scrollSfc : gVideoDriver.getScrollSurfaceVec() )
    {
        scrollSfc.UpdateScrollBufY(m_scrolly);
    }

    if (m_scrollpixy==0)
    {  // need to draw a new stripe
        if(m_mapy>0) m_mapy--;

        if (m_mapystripepos == 0)
        {
            // TODO: Problem with different squared sizes here
            auto &scrollSfc = gVideoDriver.getScrollSurface(0);
            const int squareSize = scrollSfc.getSquareSize();
            m_mapystripepos = (squareSize - (1<<mTileSizeBase));
        }
        else
        {
            m_mapystripepos -= (1<<mTileSizeBase);
        }
        drawHstripe(m_mapystripepos, m_mapy);

        m_scrollpixy = ((1<<mTileSizeBase)-1);
    } else m_scrollpixy--;

    refreshVisibleArea();

    return true;
}


void CMap::calcVisibleArea()
{
    // Here we need to get the scroll boundaries and
    // derive the rect from it.
    int x2, y2;
    fetchNearestVertBlockers( mGamePlayPos.x, mVisArea.pos.x, x2);
    fetchNearestHorBlockers( mGamePlayPos.y, mVisArea.pos.y, y2);

    mVisArea.dim.x = x2 - mVisArea.pos.x;
    mVisArea.dim.y = y2 - mVisArea.pos.y;
}


void CMap::refreshVisibleArea()
{
    GsRect<int> relativeVisGameArea;

    relativeVisGameArea.pos.x = (mVisArea.pos.x>>STC)-m_scrollx;
    relativeVisGameArea.pos.y = (mVisArea.pos.y>>STC)-m_scrolly;
    relativeVisGameArea.dim.x = (mVisArea.dim.x>>STC)-16;
    relativeVisGameArea.dim.y = (mVisArea.dim.y>>STC)-16;

    GsRect<int> gameResolution(gVideoDriver.getGameResolution());

    // Using the GameResolution to intersect the
    // calculated visible area we get another one
    // which is the rect allowed for blit operations
    gameResolution.intersect(relativeVisGameArea);

    gVideoDriver.mpVideoEngine->mRelativeVisGameArea = gameResolution;
}



//////////////////////
// Drawing Routines //
//////////////////////


// It works like this. Normally we draw the entire map to the scroll buffer
// called at start of level to draw the upper-left corner of the map
// onto the scrollbuffer...from then on the map will only be drawn
// in stripes as it scrolls around.

void CMap::redrawPlaneAt(const int planeIdx, const Uint32 mx, const Uint32 my)
{
    for(auto &scrollSfc : gVideoDriver.getScrollSurfaceVec() )
    {
        SDL_Surface *ScrollSurface = scrollSfc.getScrollSurface().getSDLSurface();

        // Go through the list and just draw all the tiles that need to be animated
        const Uint32 num_h_tiles = ScrollSurface->h>>mTileSizeBase;
        const Uint32 num_v_tiles = ScrollSurface->w>>mTileSizeBase;

        const int drawMask = ScrollSurface->w-1;

        if(  mx >= m_mapx &&
             my >= m_mapy &&
             mx < m_mapx + num_v_tiles &&
             my < m_mapy + num_h_tiles 	)
        {
            if(mPlanes[planeIdx].isInfo())
                return;

            const size_t tile = mPlanes[planeIdx].getMapDataAt(mx, my);

            if(!tile && planeIdx > 0)
                return;

            const Uint16 loc_x = (((mx-m_mapx)<<mTileSizeBase)+m_mapxstripepos)&drawMask;
            const Uint16 loc_y = (((my-m_mapy)<<mTileSizeBase)+m_mapystripepos)&drawMask;
            m_Tilemaps.at(planeIdx).drawTile(ScrollSurface, loc_x, loc_y, tile);
        }
    }
}

void CMap::redrawAt(const Uint32 mx, const Uint32 my)
{
    for(decltype(mPlanes.size()) i=0 ; i<mPlanes.size() ; i++)
    {
        redrawPlaneAt(i, mx, my);
    }
}

// draws all the map area.
// This also is used for the title screen, when game starts and other passive scenes.
// Don't use it, when the game is scrolling.
// For an faster update of tiles use redrawAt instead.


void CMap::drawAllOfPlane(const int planeIdx)
{
    for(auto &scrollSfc : gVideoDriver.getScrollSurfaceVec() )
    {
        const auto dim = scrollSfc.getSquareSize();

        scrollSfc.UpdateScrollBufX(m_scrollx);
        scrollSfc.UpdateScrollBufY(m_scrolly);

        Uint32 num_v_tiles = dim>>mTileSizeBase;
        if(num_v_tiles+m_mapx >= m_width)
            num_v_tiles = m_width-m_mapx;

        Uint32 num_h_tiles = dim>>mTileSizeBase;
        if(num_h_tiles+m_mapy >= m_height)
            num_h_tiles = m_height-m_mapy;

        auto &curPlane = mPlanes.at(planeIdx);

        if(curPlane.isInfo())
            return;

        auto &curTilemap = m_Tilemaps.at(planeIdx);

        for(Uint32 y=0;y<num_h_tiles;y++)
        {
            for(Uint32 x=0;x<num_v_tiles;x++)
            {
                Uint32 tile = curPlane.getMapDataAt(x+m_mapx, y+m_mapy);

                if(!tile && planeIdx > 0)
                    continue;

                curTilemap.drawTile(scrollSfc,
                                    ((x<<mTileSizeBase)+m_mapxstripepos),
                                    ((y<<mTileSizeBase)+m_mapystripepos),
                                    tile);
            }
        }
    }
}

void CMap::drawAll()
{
    refreshVisibleArea();

    for(decltype(mPlanes.size()) i=0 ; i<mPlanes.size() ; i++)
    {
        drawAllOfPlane(i);
    }
}

void CMap::drawHstripeOfPlane(const int planeIdx,
                              const unsigned int y,
                              const unsigned int mpy)
{
    if(mpy >= m_height) return;

    for(auto &scrollSfc : gVideoDriver.getScrollSurfaceVec() )
    {
        const auto dim = scrollSfc.getSquareSize();

        Uint32 num_v_tiles= dim>>mTileSizeBase;

        const int drawMask = dim-1;

        if( num_v_tiles+m_mapx >= m_width )
            num_v_tiles = m_width-m_mapx;

        const auto &curPlane = mPlanes[planeIdx];

        if(curPlane.isInfo())
            return;

        auto &curTilemap = m_Tilemaps.at(planeIdx);

        for(Uint32 x=0;x<num_v_tiles;x++)
        {
            Uint32 tile = curPlane.getMapDataAt(x+m_mapx, mpy);

            if(!tile && planeIdx > 0)
                continue;

            curTilemap.drawTile(scrollSfc.getScrollSurface(),
                                ((x<<mTileSizeBase)+m_mapxstripepos)&drawMask,
                                y, tile);
        }
    }
}

// draw a horizontal stripe, for vertical scrolling
void CMap::drawHstripe(const unsigned int y,
                       const unsigned int mpy)
{
    for(decltype(mPlanes.size()) i=0 ; i<mPlanes.size() ; i++)
    {
        drawHstripeOfPlane(i, y, mpy);
    }
}

void CMap::drawVstripeOfPlane(const int planeIdx,
                              const unsigned int x,
                              const unsigned int mpx)
{   
    if(mpx >= m_width) return;

    auto &curPlane = mPlanes[planeIdx];

    if(curPlane.isInfo())
        return;

    auto &curTilemap = m_Tilemaps.at(planeIdx);

    for(auto &scrollSfc : gVideoDriver.getScrollSurfaceVec() )
    {
        const auto dim = scrollSfc.getSquareSize();

        Uint32 num_h_tiles = (dim>>mTileSizeBase);

        if( num_h_tiles+m_mapy >= m_height )
            num_h_tiles = m_height-m_mapy;

        for(Uint32 y=0 ; y<num_h_tiles ; y++)
        {
            Uint32 tile = curPlane.getMapDataAt(mpx, y+m_mapy);

            if(!tile && planeIdx > 0)
                continue;

            const int drawMask = dim-1;

            curTilemap.drawTile(scrollSfc.getScrollSurface(), x,
                                ((y<<mTileSizeBase)+m_mapystripepos) & drawMask, tile);
        }
    }
}

// draws a vertical stripe from map position mapx to scrollbuffer position x
void CMap::drawVstripe(const unsigned int x, const unsigned int mpx)
{
    for(decltype(mPlanes.size()) i=0 ; i<mPlanes.size() ; i++)
    {
        drawVstripeOfPlane(i, x, mpx);
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

 */
void CMap::_drawForegroundTiles()
{

    SDL_Surface *surface = gVideoDriver.getBlitSurface();
	const Uint16 num_h_tiles = surface->h;
    const Uint16 num_v_tiles = surface->w;
    Uint16 x1 = m_scrollx>>mTileSizeBase;
    Uint16 y1 = m_scrolly>>mTileSizeBase;
    Uint16 x2 = (m_scrollx+num_v_tiles)>>mTileSizeBase;
    Uint16 y2 = (m_scrolly+num_h_tiles)>>mTileSizeBase;

	std::vector<CTileProperties> &TileProperties =
			gBehaviorEngine.getTileProperties(1);

    const auto &visGA = gVideoDriver.mpVideoEngine->mRelativeVisGameArea;
    const auto &visBlendGA = gVideoDriver.mpVideoEngine->mRelativeBlendVisGameArea;

    const int visX1 = visGA.pos.x;
    const int visX2 = visGA.pos.x+visGA.dim.x;
    const int visY1 = visGA.pos.y;
    const int visY2 = visGA.pos.y+visGA.dim.y;

    const int visBlendX1 = visBlendGA.pos.x;
    const int visBlendX2 = visBlendGA.pos.x+visBlendGA.dim.x;
    const int visBlendY1 = visBlendGA.pos.y;
    const int visBlendY2 = visBlendGA.pos.y+visBlendGA.dim.y;

    auto &tilemap = m_Tilemaps[1];

    for( size_t y=y1 ; y<=y2 ; y++)
    {
        for( size_t x=x1 ; x<=x2 ; x++)
        {
            const auto fg = mPlanes[1].getMapDataAt(x,y);

            const int loc_x = (x<<mTileSizeBase)-m_scrollx;
            const int loc_y = (y<<mTileSizeBase)-m_scrolly;

            if( loc_x+16 < visX1 || loc_x > visX2 )
                continue;

            if( loc_y+16 < visY1 || loc_y > visY2 )
                continue;

            if(fg != 0)
            {
                if(TileProperties[fg].behaviour < 0)
                {
#if !defined(EMBEDDED)
                    if( ( loc_x > visBlendX1 && loc_x < visBlendX2 ) &&
                        ( loc_y > visBlendY1 && loc_y < visBlendY2 ) )
                    {
                        tilemap.drawTileBlended(surface, loc_x, loc_y, fg, 192 );
                    }
                    else
#endif
                    {
                        tilemap.drawTile(surface, loc_x, loc_y, fg );
                    }
                }
            }
        }
    }
}

/////////////////////////
// Animation functions //
/////////////////////////
// Searches for animated tiles at the map position (X,Y) and
// unregisters them from animtiles
// Draw an animated tile. If it's not animated, draw it anyways

Uint8 CMap::getAnimtiletimer()
{
    return mAnimtileTimer;
}

void CMap::animateAllTiles()
{
    if(m_height == 0 || m_width == 0)
        return;

    if(!m_animation_enabled)
        return;

    // Some refreshes need to be forced. In this case everything is redrawn
    if(gVideoDriver.getRefreshSignal())
    {
        refreshStripes();
        drawAll();

        gVideoDriver.blitScrollSurfaces();
        gVideoDriver.setRefreshSignal(false);
    }



    // Tick, tock!!
    mAnimtileTimer += 1.0f;

    if( mAnimtileTimer > 256.0f )
    {
        mAnimtileTimer = 0.0f;
    }

    static int animtileTimerInt = 0;

    animtileTimerInt++;

    auto &frontTileProperties = gBehaviorEngine.getTileProperties(1);
    word *p_front_tile = mPlanes[1].getMapDataPtr();

    auto &backTileProperties = gBehaviorEngine.getTileProperties(0);
    word *p_back_tile = mPlanes[0].getMapDataPtr();

    auto &timersBack = mPlanes[0].getTimers();
    auto &timersFront = mPlanes[1].getTimers();

    for( size_t y=0 ; y<m_height ; y++)
    {
        const int stride = m_width*y;

        for( size_t x=0 ; x<m_width ; x++)
        {
            bool draw = false;

            const auto offset = stride + x;

            const CTileProperties &back_tile = backTileProperties[*p_back_tile];
            const CTileProperties &front_tile = frontTileProperties[*p_front_tile];

            if( back_tile.animationTime )
            {
                timersBack[offset]--;

                if(timersBack[offset] == 0)
                {
                    *p_back_tile += back_tile.nextTile;
                    timersBack[offset] = backTileProperties[*p_back_tile].animationTime;
                    draw = true;
                }
            }

            if( front_tile.animationTime )
            {
                timersFront[offset]--;

                if(timersFront[offset] == 0)
                {
                    *p_front_tile += front_tile.nextTile;
                    timersFront[offset] = frontTileProperties[*p_front_tile].animationTime;
                    draw = true;
                }
            }


            if(draw)
            {
                for(auto &scrollSfc : gVideoDriver.getScrollSurfaceVec() )
                {
                    const auto dim = scrollSfc.getSquareSize();
                    const int drawMask = dim-1;

                    // Go through the list and just draw all the tiles that need to be animated
                    Uint32 num_h_tiles = dim/16;
                    Uint32 num_v_tiles = dim/16;

                    if(num_v_tiles+m_mapx >= m_width)
                        num_v_tiles = m_width-m_mapx;

                    if(num_h_tiles+m_mapy >= m_height)
                        num_h_tiles = m_height-m_mapy;

                    if( x >= m_mapx &&
                        y >= m_mapy &&
                        x <  m_mapx + num_v_tiles &&
                        y <  m_mapy + num_h_tiles )
                    {
                        const Uint16 bgTile = *p_back_tile;
                        const Uint16 fgTile = *p_front_tile;
                        const Uint16 loc_x = (((x-m_mapx)<<mTileSizeBase)+m_mapxstripepos) & drawMask;
                        const Uint16 loc_y = (((y-m_mapy)<<mTileSizeBase)+m_mapystripepos) & drawMask;

                        auto &scrollSDLSfc = scrollSfc.getScrollSurface();

                        m_Tilemaps[0].drawTile(scrollSDLSfc, loc_x, loc_y, bgTile);

                        if(fgTile)
                        {
                            m_Tilemaps[1].drawTile(scrollSDLSfc, loc_x, loc_y, fgTile);
                        }
                    }
                }
            }

            p_back_tile++;
            p_front_tile++;
        }
    }
}


auto CMap::getlevelat(const int x,
                const int y) -> int
{
    assert(mPlanes.size() > 2);
    return mPlanes[2].getMapDataAt(x>>mTileSizeBase,
                                   y>>mTileSizeBase);
}

auto CMap::getPlaneDataAt(const int plane,
                      const int x,
                      const int y) const -> int
{
    assert(int(mPlanes.size()) > plane);
    return mPlanes[plane].getMapDataAt(x>>CSF, y>>CSF);
}

Uint16 CMap::getPlaneDataAt(const int plane, GsVec2D<Uint32> pos) const
{
    assert(int(mPlanes.size()) > plane);
    return mPlanes[plane].getMapDataAt(pos.x>>CSF, pos.y>>CSF);
}

bool CMap::locked() const
{
    return mLocked;
}


void CMap::setSpriteOrigin(const int sprId, const GsVec2D<int> &origin)
{
    mSpriteOriginList[sprId] = origin;
}

GsVec2D<int> CMap::getSpriteOrigin(const int sprId)
{
    return mSpriteOriginList[sprId];
}

void CMap::setInfoPlane(const int plane, const bool value)
{
    assert(static_cast<decltype (mPlanes.size())>(plane) < mPlanes.size());
    mPlanes[plane].setInfo(value);
}
