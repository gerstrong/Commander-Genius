/*
 * CMap.cpp
 *
 *  Created on: 21.09.2009
 *      Author: gerstrong
 */

#include "CMap.h"
#include "engine/core/CBehaviorEngine.h"
#include <base/interface/FindFile.h>
#include <base/GsLogging.h>
#include <base/video/CVideoDriver.h>
#include <base/GsTimer.h>
#include "graphics/GsGraphics.h"
#include <iostream>
#include <fstream>

CMap::CMap():
m_Tilemaps(gGraphics.getTileMaps())
{
    resetScrolls();
    m_Level = 0;
    isSecret = false;
}


bool CMap::isEmpty() const
{
    return mScrollingPlanes.empty();
}

////////////////////////////
// Initialization Routine //
////////////////////////////

bool CMap::setupEmptyDataPlanes(const unsigned int numScrollingPlanes,
                                const int tileSize,
                                const Uint32 width,
                                const Uint32 height)
{
    m_width = width;
    m_height = height;

    mScrollingPlanes.clear();

    for(unsigned int i=0 ; i<numScrollingPlanes ; i++)
    {
        ScrollingPlane plane(i, (i>0));
        plane.createDataMap(m_width, m_height, tileSize);
        mScrollingPlanes.push_back(plane);
    }

    mInfoPlane.createDataMap(m_width, m_height);

    return true;
}

void CMap::resetScrolls()
{
    for( auto &scrollPlane : mScrollingPlanes )
    {
        scrollPlane.resetScrolling();
    }

    gVideoDriver.resetScrollBuffers();
}


void CMap::refreshStripes()
{
    for( auto &plane : mScrollingPlanes )
    {
        GsTilemap &tilemap = m_Tilemaps.at(plane.getTilemapIdx());
        plane.refreshStripes(tilemap);
    }
}


/////////////////////////
// Getters and Setters //
/////////////////////////
// returns the tile which is set at the given coordinates
Uint16 CMap::at(int x, int y, int t)
{
    if(x < int(m_width) && y < int(m_height) )
    {
        return mScrollingPlanes[t].getMapDataAt(x,y);
    }
    else
    {
        return 0;
    }
}

//////////////////////////
// returns the object/sprite/level which is set at the given coordinates
Uint16 CMap::getObjectat(const Uint16 x, const Uint16 y)
{
    return getInfoData(GsVec2D<Uint32>(x,y));
}

/**
 * \brief	Gets the pointer to the scrolling plane data of the map
 * \param	PlaneNum number of the requested Plane
 */
word *CMap::getData(const Uint8 PlaneNum)
{
    assert(mScrollingPlanes.size() > PlaneNum);
    return mScrollingPlanes[PlaneNum].getMapDataPtr();
}

word *CMap::getInfoData()
{
    return mInfoPlane.getMapDataPtr();
}


Uint16 CMap::getInfoData(const GsVec2D<Uint32> pos) const
{
    return mInfoPlane.getMapDataAt(pos.x,pos.y);
}

word *CMap::getForegroundData()
{
    assert(mScrollingPlanes.size() > 1);
    return mScrollingPlanes[1].getMapDataPtr();
}

word *CMap::getBackgroundData()
{
    assert(mScrollingPlanes.size() > 0);
    return mScrollingPlanes[0].getMapDataPtr();
}


void CMap::setupAnimationTimerOfTile(const int tilemapIdx)
{
    auto &tileProperties = gBehaviorEngine.getTileProperties(tilemapIdx);
    word *p_back_tile = mScrollingPlanes[tilemapIdx].getMapDataPtr();

    auto &timers = mScrollingPlanes[tilemapIdx].getTimers();
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
    assert(mScrollingPlanes.size() >= 2);

    setupAnimationTimerOfTile(0);
    setupAnimationTimerOfTile(1);
}




void CMap::unlock()
{
    mLocked = false;
}

void CMap::lock()
{
    mLocked = true;
}

bool CMap::updateTextures()
{
    bool ok = true;

    for(auto &plane : mScrollingPlanes)
    {
        auto &tilemap = m_Tilemaps.at(plane.getTilemapIdx());
        ok &= plane.updateTextures(tilemap);
    }

    // TODO: Do we really need to return true here?
    return ok;
}

void CMap::fetchNearestVertBlockers(const int x,
                                    int &leftCoord,
                                    int &rightCoord)
{
    for(auto &plane: mScrollingPlanes)
    {
        plane.fetchNearestVertBlockers(x, leftCoord, rightCoord);
    }
}

bool CMap::detectTopBlocker(const int y)
{
    int blockYup1 = 0;
    int blockYdown1 = 0;
    int blockYup2 = 0;
    int blockYdown2 = 0;

    fetchNearestHorBlockers(y<<STC, blockYup2, blockYdown2);

    if( y < (3<<TILE_S) ) // Not enough space to compare two blockers!
    {
        return ( (y>>TILE_S)<=(blockYup2>>CSF) );
    }
    else
    {
        fetchNearestHorBlockers((y<<STC)-(1<<CSF), blockYup1, blockYdown1);
        return ( blockYup1 != blockYup2 || blockYdown1 != blockYdown2 );
    }
}

bool CMap::detectBottomBlocker(const int y)
{
    int blockYup1 = 0;
    int blockYdown1 = 0;
    int blockYup2 = 0;
    int blockYdown2 = 0;

    fetchNearestHorBlockers((y<<STC), blockYup1, blockYdown1);

    if( y > int((m_height-2)<<TILE_S) ) // Not enough space to compare two blockers!
    {
        return ( (y>>TILE_S)>=(blockYdown1>>CSF) );
    }
    else
    {
        fetchNearestHorBlockers((y<<STC)-(1<<CSF), blockYup2, blockYdown2);
        return ( blockYup1 != blockYup2 || blockYdown1 != blockYdown2 );
    }
}

bool CMap::detectLeftBlocker(const int x)
{
    int blockXleft1 = 0;
    int blockXright1 = 0;
    int blockXleft2 = 0;
    int blockXright2 = 0;

    fetchNearestVertBlockers(x<<STC, blockXleft2, blockXright2);

    if( x < (3<<TILE_S) ) // Not enough space for comparing two blockers!
    {
        return ( (x>>TILE_S)<=(blockXleft2>>CSF) );
    }
    else
    {
        fetchNearestVertBlockers((x<<STC)-(1<<CSF), blockXleft1, blockXright1);
        return ( blockXleft1 != blockXleft2 || blockXright1 != blockXright2 );
    }
}

bool CMap::detectRightBlocker(const int x)
{
    int blockXleft1 = 0;
    int blockXright1 = 0;
    int blockXleft2 = 0;
    int blockXright2 = 0;

    fetchNearestVertBlockers(x<<STC, blockXleft1, blockXright1);


    if( x > int((m_width-2)<<TILE_S) ) // Not enough space to compare two blockers!
    {
        return ( (x>>TILE_S)>=(blockXright1>>CSF) );
    }
    else
    {
        fetchNearestVertBlockers((x<<STC)-(1<<CSF), blockXleft2, blockXright2);
        return ( blockXleft1 != blockXleft2 || blockXright1 != blockXright2 );
    }
}

void CMap::fetchNearestHorBlockers(const int y,
                                   int &upCoord,
                                   int &downCoord)
{
    for(auto &plane: mScrollingPlanes)
    {
        plane.fetchNearestHorBlockers(y, upCoord, downCoord);
    }
}


// searches the map's object layer for object OBJ.
// if it is found returns non-zero and places the
// coordinates of the first occurrence of the object
// in (xout,yout)
bool CMap::findObject(unsigned int obj, int *xout, int *yout)
{
    for(unsigned int y=2 ; y<m_height-2 ; y++)
    {
        for(unsigned int x=2 ; x<m_width-2 ; x++)
        {
            if (mInfoPlane.getMapDataAt(x,y)==obj)
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
    assert(int(mScrollingPlanes.size()+1) > plane);

    // The last plane is seen by many engines as the info. Should be changed in future
    if(plane >= int(mScrollingPlanes.size()))
    {
        return mInfoPlane.findTile(tile, xout, yout);
    }
    else
    {
        return mScrollingPlanes.at(plane).findTile(tile, xout, yout);
    }
}

bool CMap::setInfoTile(const GsVec2D<Uint32> pos,
                       const Uint16 tile)
{
    if( pos.x>=m_width || pos.y>=m_height )
        return false;

    mInfoPlane.setMapDataAt(tile, pos.x, pos.y);
    return true;
}

bool CMap::setTile(const Uint16 x, const Uint16 y,
                   const Uint16 t, const Uint16 plane)
{
    if( x<m_width && y<m_height )
    {
        assert(mScrollingPlanes.size() > plane);
        mScrollingPlanes[plane].setMapDataAt(t, x, y);
        return true;
    }
    else
        return false;
}

bool CMap::setTile(Uint16 x, Uint16 y, Uint16 t,
                   bool redraw, Uint16 plane)
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
// Used in many cases when items are picked up (especially in Vorticon Keen)
bool CMap::changeTile(Uint16 x, Uint16 y, Uint16 t)
{
    if( setTile( x, y, t ) )
    {
        for( auto &plane : mScrollingPlanes )
        {
            plane.drawTile(m_Tilemaps.at(1), {x, y}, t);
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
bool CMap::gotoPos(const int x, const int y)
{
    bool retval = false;

    for(auto &plane : mScrollingPlanes)
    {
        retval = plane.gotoPos(m_Tilemaps.at(plane.getTilemapIdx()),
                               {x,y} );

        calcVisibleArea();

        refreshVisibleArea();
    }

    return retval;
}


// scrolls the map one pixel right
bool CMap::scrollRight(const bool force)
{
    bool ok = true;
    const int res_width = gVideoDriver.getGameResolution().dim.x;

    for(auto &plane : mScrollingPlanes)
    {
        const auto scroll = plane.getScrollCoords();

        if( !force && detectRightBlocker(scroll.x+res_width))
            return false;

        ok &= plane.scrollRightTest();
    }

    if(!ok)
        return false;

    for(auto &plane : mScrollingPlanes)
    {
        auto &tilemap = m_Tilemaps.at(plane.getTilemapIdx());
        ok &= plane.scrollRight(tilemap);
    }

    for(auto &scrollPlane : mScrollingPlanes)
    {
        refreshVisibleArea(scrollPlane.getScrollCoords());
    }
    return ok;
}

// scrolls the map one pixel left
bool CMap::scrollLeft(const bool force)
{
    bool ok = true;

    for(auto &plane : mScrollingPlanes)
    {
        const auto scroll = plane.getScrollCoords();
        if( !force && detectLeftBlocker(scroll.x-1) )
            return false;

        ok &= plane.scrollLeftTest();
    }

    if(!ok)
        return false;

    for(auto &plane : mScrollingPlanes)
    {
        //auto scroll = plane.getScrollCoords();

        auto &tilemap = m_Tilemaps.at(plane.getTilemapIdx());
        ok &= plane.scrollLeft(tilemap);

        //scroll = plane.getScrollCoords();
    }

    for(auto &scrollPlane : mScrollingPlanes)
    {
        refreshVisibleArea(scrollPlane.getScrollCoords());
    }
    return ok;
}

bool CMap::scrollDown(const bool force)
{
    bool ok = true;
    const int res_height = gVideoDriver.getGameResolution().dim.y;

    for(auto &plane : mScrollingPlanes)
    {
        const auto scroll = plane.getScrollCoords();

        if(!plane.isAxisYLocked() &&
           !force && detectBottomBlocker(scroll.y+res_height) )
        {
            return false;
        }

        ok &= plane.scrollDown(m_Tilemaps.at(plane.getTilemapIdx()), force);
        refreshVisibleArea(plane.getScrollCoords());
    }

    for(auto &scrollPlane : mScrollingPlanes)
    {
        refreshVisibleArea(scrollPlane.getScrollCoords());
    }
    return ok;
}




bool CMap::scrollUp(const bool force)
{
    bool ok = true;

    for(auto &plane : mScrollingPlanes)
    {
        const auto scroll = plane.getScrollCoords();

        if(!plane.isAxisYLocked() &&
           !force && detectTopBlocker(scroll.y-1))
        {
            return false;
        }

        ok &= plane.scrollUp(m_Tilemaps.at(plane.getTilemapIdx()), force);
        //refreshVisibleArea(plane.getScrollCoords());
    }

    for(auto &scrollPlane : mScrollingPlanes)
    {
        refreshVisibleArea(scrollPlane.getScrollCoords());
    }
    return ok;
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

void CMap::setSubscrollUnits(const int plane, const int subscrollUnits)
{
    assert(plane < int(mScrollingPlanes.size()));
    assert(plane >= 0);
    assert(subscrollUnits >= 0);
    mScrollingPlanes.at(plane).setSubscrollUnits(subscrollUnits);
}

void CMap::refreshVisibleArea()
{
    for(auto &plane : mScrollingPlanes)
    {
        refreshVisibleArea(plane.getScrollCoords());
    }
}

void CMap::refreshVisibleArea(const GsVec2D<int> scroll)
{
    GsRect<int> relVisGameArea;

    relVisGameArea.pos.x = (mVisArea.pos.x>>STC)-scroll.x;
    relVisGameArea.pos.y = (mVisArea.pos.y>>STC)-scroll.y;
    relVisGameArea.dim.x = (mVisArea.dim.x>>STC)-16;
    relVisGameArea.dim.y = (mVisArea.dim.y>>STC)-16;

    GsRect<int> gameResolution(gVideoDriver.getGameResolution());

    // Using the GameResolution to intersect the
    // calculated visible area we get another one
    // which is the rect allowed for blit operations
    gameResolution.intersect(relVisGameArea);

    gVideoDriver.mpVideoEngine->mRelativeVisGameArea = gameResolution;
}


void CMap::insertHorBlocker(const int y)
{
      for(auto &plane : mScrollingPlanes)
      {
          plane.insertHorBlocker(y);
      }
}

void CMap::insertVertBlocker(const int x)
{
    for(auto &plane : mScrollingPlanes)
    {
        plane.insertVertBlocker(x);
    }
}


void CMap::resetScrollBlocker()
{
    for(auto &plane : mScrollingPlanes)
    {
        plane.resetScrollBlocker();
    }
}

void CMap::collectBlockersCoordiantes()
{
    const auto engine = gBehaviorEngine.getEngine();

    for(auto &plane : mScrollingPlanes)
    {
        int ep = gBehaviorEngine.getEpisode();

        if(engine == ENGINE_GALAXY)
        {
            // The first two rows and columns should not be seen
            plane.insertVertBlocker(0);
            plane.insertHorBlocker(0);
            plane.insertVertBlocker(1<<CSF);
            plane.insertHorBlocker(1<<CSF);

            const word* map_ptr = mInfoPlane.getMapDataPtr();

            for(int y=0 ; y<(int)m_height ; y++)
            {
                for(int x=0 ; x<(int)m_width ; x++)
                {
                    // Check the row for a blocker which has the proper value
                    if(*map_ptr == 0x19)
                    {
                        plane.insertHorBlocker(y<<(CSF));
                    }

                    // In Keen 5 it is only used on the map and stands for an in level teleporter
                    if(*map_ptr == 0x1A && ep != 5)
                        plane.insertVertBlocker(x<<(CSF));

                    map_ptr++;
                }
            }

            // There exist end-of-map tiles we don't want to see
            plane.insertHorBlocker(((m_height-1)<<(CSF)));
            plane.insertVertBlocker((m_width-1)<<(CSF));
            plane.insertHorBlocker(((m_height-2)<<(CSF)));
            plane.insertVertBlocker((m_width-2)<<(CSF));
        }
        if(engine == ENGINE_VORTICON)
        {
            // The first two rows and columns should not be seen. Is this correct for all maps?
            plane.insertVertBlocker(0);
            plane.insertHorBlocker(0);
            plane.insertVertBlocker(1<<CSF);
            plane.insertHorBlocker(1<<CSF);

            // There exist end-of-map tiles we don't want to see
            //plane.insertHorBlocker(((m_height-1)<<(CSF)));
            //plane.insertVertBlocker((m_width-1)<<(CSF));
            plane.insertHorBlocker(((m_height-2)<<(CSF)));
            plane.insertVertBlocker((m_width-2)<<(CSF));
        }
    }
}



//////////////////////
// Drawing Routines //
//////////////////////


// It works like this. Normally we draw the entire map to the scroll buffer
// called at start of level to draw the upper-left corner of the map
// onto the scrollbuffer...from then on the map will only be drawn
// in stripes as it scrolls around.

void CMap::redrawAt(const Uint32 mx, const Uint32 my)
{
    for(decltype(mScrollingPlanes.size()) i=0 ; i<mScrollingPlanes.size() ; i++)
    {
        auto &plane = mScrollingPlanes.at(i);
        auto &tilemap = m_Tilemaps.at(i);
        plane.redrawPlaneAt(tilemap, mx, my);
    }
}

// draws all the map area.
// This also is used for the title screen, when game starts and other passive scenes.
// Don't use it, when the game is scrolling.
// For an faster update of tiles use redrawAt instead.


void CMap::drawAllOfPlane(const int planeIdx)
{
    auto &scrollSfcVec = gVideoDriver.getScrollSurfaceVec();
    if(planeIdx >= int(scrollSfcVec.size()))
        return;

    auto &plane = mScrollingPlanes.at(planeIdx);
    auto &tilemap = m_Tilemaps.at(planeIdx);
    plane.drawAll(tilemap);
}

void CMap::drawAll()
{
    for(decltype(mScrollingPlanes.size()) i=0 ; i<mScrollingPlanes.size() ; i++)
    {
        drawAllOfPlane(i);
    }
}

void CMap::drawHstripeOfPlane(const int planeIdx,
                              const unsigned int y,
                              const unsigned int mpy)
{
    auto &plane = mScrollingPlanes.at(planeIdx);
    auto &tilemap = m_Tilemaps.at(plane.getTilemapIdx());
    plane.drawHstripe(tilemap, y, mpy);
}

// draw a horizontal stripe, for vertical scrolling
void CMap::drawHstripe(const unsigned int y,
                       const unsigned int mpy)
{
    for(decltype(mScrollingPlanes.size()) i=0 ; i<mScrollingPlanes.size() ; i++)
    {
        drawHstripeOfPlane(i, y, mpy);
    }
}
/*
void CMap::drawVstripeOfPlane(const int planeIdx,
                              const unsigned int x,
                              const unsigned int mpx)
{
    if(mpx >= m_width) return;

    auto &plane = mScrollingPlanes.at(planeIdx);
    auto &tilemap = m_Tilemaps.at(plane.getTilemapIdx());
    plane.drawVstripe(tilemap, x, mpx);
}
*/
// draws a vertical stripe from map position mapx to scrollbuffer position x
void CMap::drawVstripe(const unsigned int x,
                       const unsigned int mpx)
{
    for(auto &plane : mScrollingPlanes)
    {
        const auto tmidx = plane.getTilemapIdx();
        auto &tilemap = m_Tilemaps.at(tmidx);
        plane.drawVstripe(tilemap, x, mpx);
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
 * \brief This function draws all the masked and
 *        foreground tiles to the blit surface directly

 */
void CMap::_drawForegroundTiles()
{
    mScrollingPlanes.at(1)._drawForegroundTiles(m_Tilemaps[1]);
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

    // Some refreshes need to be forced. In such case everything is redrawn
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

    for(auto &plane : mScrollingPlanes)
    {
        auto &tilemap = m_Tilemaps.at(plane.getTilemapIdx());
        plane.animateAllTiles(tilemap);
    }
}

/*
auto CMap::getlevelat(const int x,
                const int y) -> int
{
    assert(0);

    return mInfoPlane.getMapDataAt(x>>mTileSizeBase,
                                   y>>mTileSizeBase);

}
*/
auto CMap::getPlaneDataAt(const int plane,
                      const int x,
                      const int y) const -> int
{
    assert(int(mScrollingPlanes.size()+1) > plane);

    if(plane > 1)
    {
        return mInfoPlane.getMapDataAt(x>>CSF, y>>CSF);
    }
    else
    {
        return mScrollingPlanes[plane].getMapDataAt(x>>CSF, y>>CSF);
    }
}

Uint16 CMap::getPlaneDataAt(const int plane, GsVec2D<Uint32> pos) const
{
    assert(int(mScrollingPlanes.size()+1) > plane);

    if(plane >= int(mScrollingPlanes.size()))
    {
        return mInfoPlane.getMapDataAt(pos.x>>CSF, pos.y>>CSF);
    }
    else
    {
        return mScrollingPlanes[plane].getMapDataAt(pos.x>>CSF, pos.y>>CSF);
    }
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

GsVec2D<int> CMap::getScrollCoords(const unsigned int idx) const
{
    return mScrollingPlanes.at(idx).getScrollCoords();
}


GsVec2D<int> CMap::getMainScrollCoords() const
{
    return getScrollCoords(0);
}

void CMap::lockAxisY(const unsigned int idx, const bool value)
{
    assert(idx < mScrollingPlanes.size());

    return mScrollingPlanes.at(idx).lockAxisY(value);
}
