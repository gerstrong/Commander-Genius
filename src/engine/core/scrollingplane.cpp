
#include "scrollingplane.h"
#include <base/video/CVideoDriver.h>
#include <engine/core/CBehaviorEngine.h>

ScrollingPlane::ScrollingPlane(const int scrollSfcIdx,
                               const bool transparent) :
    mScrollSfcIdx(scrollSfcIdx),
    mHasTransparentTile(transparent)
{
}

void ScrollingPlane::createDataMap(const int width,
                                   const int height,
                                   const int tileSize)
{
    CPlane::createDataMap(width, height);

    if(tileSize == 8)
    {
        mTileSizeBase = 3;
    }
    else
    {
        mTileSizeBase = 4;
    }
}

void ScrollingPlane::refreshStripes(GsTilemap &tilemap)
{
    const int oldx = m_mapx<<mTileSizeBase;
    const int oldy = m_mapy<<mTileSizeBase;

    resetScrolling();

    gotoPos(tilemap, {oldx, oldy});
}


GsVec2D<int> ScrollingPlane::getScrollCoords() const
{
    return mScrollCoords;
}

int ScrollingPlane::getTilemapIdx() const
{
    return mScrollSfcIdx;
}

void ScrollingPlane::resetScrolling()
{
    mScrollCoords = {0,0};

    m_scrollpix = m_scrollpixy = 0;
    m_mapx = m_mapy = 0;           // map X location shown at scrollbuffer row 0
    m_mapxstripepos = m_mapystripepos = 0;  // X pixel position of next stripe row
}

void ScrollingPlane::drawTile(GsTilemap &tilemap,
                              const GsVec2D<Uint16> pos,
                              const Uint16 t)
{
    auto &scrollSfc = gVideoDriver.getScrollSurfaceVec().at(mScrollSfcIdx);
    const int drawMask = scrollSfc.getScrollSurface().width()-1;

    tilemap.drawTile(scrollSfc,
                     (pos.x<<mTileSizeBase)&drawMask,
                     (pos.y<<mTileSizeBase)&drawMask, t);

}

bool ScrollingPlane::scrollLeftTest()
{
    auto &scroll = mScrollCoords;
    return ( scroll.x <= 0 ) ? true : false;
}

bool ScrollingPlane::scrollLeft(GsTilemap &tilemap)
{
    auto &scrollSfc = gVideoDriver.getScrollSurfaceVec().at(mScrollSfcIdx);
    auto &scroll = mScrollCoords;

    if(mSubscrollCoords.x > 0)
    {
        mSubscrollCoords.x--;
        return true;
    }

    mSubscrollCoords.x = mSubscrollUnits;

    scroll.x--;
    scrollSfc.UpdateScrollBufX(scroll.x);


    if (m_scrollpix==0)
    {  // need to draw a new stripe
        if(m_mapx>0) m_mapx--;
        if (m_mapxstripepos == 0)
        {
            const int squareSize = scrollSfc.getSquareSize();
            m_mapxstripepos = (squareSize - (1<<mTileSizeBase));
        }
        else
        {
            m_mapxstripepos -= (1<<mTileSizeBase);
        }
        drawVstripe(tilemap, m_mapxstripepos, m_mapx);

        m_scrollpix = (1<<mTileSizeBase)-1;
    } else m_scrollpix--;

    return true;
}

bool ScrollingPlane::scrollRightTest()
{
    const int res_width = gVideoDriver.getGameResolution().dim.x;
    auto &scroll = mScrollCoords;

    if(scroll.x >= int(((mWidth-2)<<mTileSizeBase) - res_width))
        return false;

    return true;
}

// scrolls the map one pixel right
bool ScrollingPlane::scrollRight(GsTilemap &tilemap)
{
    auto &scrollSfc = gVideoDriver.getScrollSurfaceVec().at(mScrollSfcIdx);
    auto &scroll = mScrollCoords;

    if(mSubscrollCoords.x < mSubscrollUnits)
    {
        mSubscrollCoords.x++;
        return true;
    }

    mSubscrollCoords.x = 0;

    scroll.x++;

    scrollSfc.UpdateScrollBufX(scroll.x);

    m_scrollpix++;
    if (m_scrollpix >= (1<<mTileSizeBase))
    {  // need to draw a new stripe

        // TODO: Problem with different squared sizes here
        const int squareSize = scrollSfc.getSquareSize();
        const int totalNumTiles = squareSize>>mTileSizeBase;
        drawVstripe(tilemap, m_mapxstripepos, m_mapx + totalNumTiles);

        m_mapx++;
        m_mapxstripepos += (1<<mTileSizeBase);
        if (m_mapxstripepos >= squareSize) m_mapxstripepos = 0;
        m_scrollpix = 0;
    }
    return true;
}

bool ScrollingPlane::scrollUp(GsTilemap &tilemap, [[maybe_unused]] const bool force)
{
    if(mLockAxisY)
        return true;

    auto &scrollSfc = gVideoDriver.getScrollSurfaceVec().at(mScrollSfcIdx);
    auto &scroll = mScrollCoords;

    if( scroll.y <= 0 )
        return false;

    if(mSubscrollCoords.y > 0)
    {
        mSubscrollCoords.y--;
        return true;
    }

    mSubscrollCoords.y = mSubscrollUnits;

    scroll.y--;

    scrollSfc.UpdateScrollBufY(scroll.y);

    if (m_scrollpixy==0)
    {  // need to draw a new stripe
        if(m_mapy>0) m_mapy--;

        if (m_mapystripepos == 0)
        {
            // TODO: Problem with different squared sizes here
            const int squareSize = scrollSfc.getSquareSize();
            m_mapystripepos = (squareSize - (1<<mTileSizeBase));
        }
        else
        {
            m_mapystripepos -= (1<<mTileSizeBase);
        }
        drawHstripe(tilemap, m_mapystripepos, m_mapy);

        m_scrollpixy = ((1<<mTileSizeBase)-1);
    } else m_scrollpixy--;

    return true;
}

bool ScrollingPlane::scrollDown(GsTilemap &tilemap, const bool force)
{
    if(mLockAxisY)
        return true;

    const int res_height = gVideoDriver.getGameResolution().dim.y;

    auto &scrollSfc = gVideoDriver.getScrollSurfaceVec().at(mScrollSfcIdx);
    auto &scroll = mScrollCoords;

    if(!force)
    {
        const int bottom_y = (scroll.y+res_height)<<STC;
        int blockYup = 0;
        int blockYdown = 0;

        fetchNearestHorBlockers(bottom_y, blockYup, blockYdown);

        const int new_bottom_y = ((scroll.y+1)+res_height)<<STC;

        if(new_bottom_y >= blockYdown)
        {
            return false;
        }
    }

    if(mSubscrollCoords.y < mSubscrollUnits)
    {
        mSubscrollCoords.y++;
        return true;
    }

    mSubscrollCoords.y = 0;

    scroll.y++;


    m_scrollpixy++;

    scrollSfc.UpdateScrollBufY(scroll.y);

    if ( m_scrollpixy >= (1<<mTileSizeBase) )
    {
        const int squareSize = scrollSfc.getSquareSize();

        if( scroll.y < int(((mHeight-2)<<mTileSizeBase) - res_height) )
        {
            // need to draw a new stripe
            const int totalNumTiles = squareSize>>mTileSizeBase;
            drawHstripe(tilemap, m_mapystripepos, m_mapy + totalNumTiles);
        }

        m_mapy++;
        m_mapystripepos += (1<<mTileSizeBase);
        if (m_mapystripepos >= squareSize) m_mapystripepos = 0;
        m_scrollpixy = 0;
    }

    return true;
}


void ScrollingPlane::drawHstripe(GsTilemap &tilemap,
                                 const unsigned int y,
                                 const unsigned int mpy)
{
    if(int(mpy) >= mHeight) return;

    auto &scrollSfc = gVideoDriver.getScrollSurfaceVec().at(mScrollSfcIdx);

    const auto dim = scrollSfc.getSquareSize();

    Uint32 num_v_tiles= dim>>mTileSizeBase;

    // TODO: Do we really need to apply AND operation with drawMask? Check!
    const int drawMask = dim-1;

    if( int(num_v_tiles+m_mapx) >= mWidth )
        num_v_tiles = mWidth-m_mapx;

    for(Uint32 x=0;x<num_v_tiles;x++)
    {
        Uint32 tile = getMapDataAt(x+m_mapx, mpy);

        tilemap.drawTransparentTile(scrollSfc,
                         ((x<<mTileSizeBase)+m_mapxstripepos)&drawMask,
                         y);

        if(tile == 0 && mHasTransparentTile)
        {
            continue;
        }
        else
        {
            tilemap.drawTile(scrollSfc,
                             ((x<<mTileSizeBase)+m_mapxstripepos)&drawMask,
                             y, tile);
        }
    }
}

void ScrollingPlane::drawVstripe(GsTilemap &tilemap,
                                 const unsigned int x,
                                 const unsigned int mpx)
{        
    auto &scrollSfc = gVideoDriver.getScrollSurfaceVec().at(mScrollSfcIdx);
    const auto dim = scrollSfc.getSquareSize();

    int num_h_tiles = (dim>>mTileSizeBase);

    if( num_h_tiles+m_mapy >= mHeight )
        num_h_tiles = mHeight-m_mapy;

    for(int y=0 ; y<num_h_tiles ; y++)
    {
        Uint32 tile = getMapDataAt(mpx, y+m_mapy);

        const int drawMask = dim-1;

        tilemap.drawTransparentTile(scrollSfc, x,
                         ((y<<mTileSizeBase)+m_mapystripepos) & drawMask);

        if(tile == 0 && mHasTransparentTile)
        {
            continue;
        }
        else
        {
            tilemap.drawTile(scrollSfc, x,
                             ((y<<mTileSizeBase)+m_mapystripepos) & drawMask, tile);
        }
    }

}


void ScrollingPlane::drawAll(GsTilemap &tilemap)
{
    auto &scroll = mScrollCoords;
    auto &scrollSfc = gVideoDriver.getScrollSurfaceVec().at(mScrollSfcIdx);

    const auto dim = scrollSfc.getSquareSize();

    scrollSfc.updateScrollBuf(scroll);

    Uint32 num_v_tiles = dim>>mTileSizeBase;
    if(int(num_v_tiles+m_mapx) >= mWidth)
        num_v_tiles = mWidth-m_mapx;

    Uint32 num_h_tiles = dim>>mTileSizeBase;
    if(int(num_h_tiles+m_mapy) >= mHeight)
        num_h_tiles = mHeight-m_mapy;


    for(Uint32 y=0;y<num_h_tiles;y++)
    {
        for(Uint32 x=0;x<num_v_tiles;x++)
        {
            Uint32 tile = getMapDataAt(x+m_mapx, y+m_mapy);

            tilemap.drawTransparentTile(scrollSfc,
                             ((x<<mTileSizeBase)+m_mapxstripepos),
                             ((y<<mTileSizeBase)+m_mapystripepos));

            if(tile == 0 && mHasTransparentTile)
            {
                continue;
            }
            else
            {
                tilemap.drawTile(scrollSfc,
                                 ((x<<mTileSizeBase)+m_mapxstripepos),
                                 ((y<<mTileSizeBase)+m_mapystripepos),
                                 tile);
            }

        }
    }

}

void ScrollingPlane::animateAllTiles(GsTilemap &tilemap)
{
    auto &scrollSfc = gVideoDriver.getScrollSurfaceVec().at(mScrollSfcIdx);
    auto &tileProperties = gBehaviorEngine.getTileProperties(mScrollSfcIdx);
    word *tilePtr = getMapDataPtr();

    auto &timers = getTimers();

    const auto dim = scrollSfc.getSquareSize();
    const int drawMask = dim-1;
    Uint16 num_v_tiles = dim>>mTileSizeBase;
    Uint16 num_h_tiles = dim>>mTileSizeBase;

    for( int y=0 ; y<mHeight ; y++)
    {
        const int stride = mWidth*y;

        for( int x=0 ; x<mWidth ; x++)
        {
            bool draw = false;

            const auto offset = stride + x;

            const CTileProperties &tileProp = tileProperties[*tilePtr];

            if( tileProp.animationTime )
            {
                timers[offset]--;

                if(timers[offset] == 0)
                {
                    *tilePtr += tileProp.nextTile;
                    timers[offset] = tileProperties[*tilePtr].animationTime;
                    draw = true;
                }
            }


            if(draw && x >= m_mapx && y >= m_mapy &&
               x < m_mapx + num_v_tiles && y < m_mapy + num_h_tiles)
            {

                const Uint16 loc_x = (((x-m_mapx)<<mTileSizeBase)+m_mapxstripepos) & drawMask;
                const Uint16 loc_y = (((y-m_mapy)<<mTileSizeBase)+m_mapystripepos) & drawMask;


                const Uint16 tile = *tilePtr;

                tilemap.drawTransparentTile(scrollSfc, loc_x, loc_y);
                tilemap.drawTile(scrollSfc, loc_x, loc_y, tile);

            }
            tilePtr++;
        }
    }

}


bool ScrollingPlane::gotoPos(GsTilemap &tilemap,
                             const GsVec2D<int> pos)
{
    bool retval = false;

    const auto scroll = mScrollCoords;

    int dx = pos.x - scroll.x;
    int dy = pos.y - scroll.y;

    if( dx > 0 )
    {
        for( int scrollx=0 ; scrollx<dx ; scrollx++) scrollRight(tilemap);
    }
    else
    {
        retval = true;
    }

    if( dx < 0 )
    {
        for( int scrollx=0 ; scrollx<-dx ; scrollx++) scrollLeft(tilemap);
    }
    else retval = true;

    if( dy > 0 )
    {
        for( int scrolly=0 ; scrolly<dy ; scrolly++) scrollDown(tilemap, true);
    }
    else retval = true;

    if( dy < 0 )
    {
        for( int scrolly=0 ; scrolly<-dy ; scrolly++) scrollUp(tilemap, true);
    }
    else retval = true;

    return retval;
}




void ScrollingPlane::insertVertBlocker(const int x)
{
    scrollBlockX.insert(x);
}


void ScrollingPlane::resetScrollBlocker()
{
    scrollBlockX.clear();
    scrollBlockY.clear();
}

void ScrollingPlane::fetchNearestVertBlockers(const int x,
                                              int &leftCoord,
                                              int &rightCoord)
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

            if(leftCoord > (2<<CSF)/* &&  gBehaviorEngine.getEngine() == ENGINE_GALAXY*/)
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

void ScrollingPlane::insertHorBlocker(const int y)
{
    scrollBlockY.insert(y);
}

void ScrollingPlane::fetchNearestHorBlockers(const int y,
                                             int &upCoord,
                                             int &downCoord)
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

            /*if(gBehaviorEngine.getEngine() == ENGINE_GALAXY)*/
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


void ScrollingPlane::redrawPlaneAt(GsTilemap &tilemap,
                                   const Uint32 mx,
                                   const Uint32 my)
{
    auto &scrollSfc = gVideoDriver.getScrollSurfaceVec().at(mScrollSfcIdx);
    SDL_Surface *ScrollSurfaceRaw = scrollSfc.getScrollSurface().getSDLSurface();

    // Go through the list and just draw all the tiles that need to be animated
    const Uint32 num_h_tiles = ScrollSurfaceRaw->h>>mTileSizeBase;
    const Uint32 num_v_tiles = ScrollSurfaceRaw->w>>mTileSizeBase;

    const int drawMask = ScrollSurfaceRaw->w-1;

    if(  mx >= m_mapx &&
         my >= m_mapy &&
         mx < m_mapx + num_v_tiles &&
         my < m_mapy + num_h_tiles 	)
    {
        const size_t tile = getMapDataAt(mx, my);

        const Uint16 loc_x = (((mx-m_mapx)<<mTileSizeBase)+m_mapxstripepos)&drawMask;
        const Uint16 loc_y = (((my-m_mapy)<<mTileSizeBase)+m_mapystripepos)&drawMask;

        if( tile != 0 || !mHasTransparentTile )
        {
            tilemap.drawTile(ScrollSurfaceRaw, loc_x, loc_y, tile);
        }
        else if(tile == 0 || mHasTransparentTile)
        {
            tilemap.drawTransparentTile(scrollSfc, loc_x, loc_y);
        }
    }
}



void ScrollingPlane::_drawForegroundTiles(GsTilemap &tilemap)
{
    auto &scroll = mScrollCoords;

    SDL_Surface *surface = gVideoDriver.getBlitSurface();
    const Uint16 num_h_tiles = surface->h;
    const Uint16 num_v_tiles = surface->w;
    Uint16 x1 = scroll.x>>mTileSizeBase;
    Uint16 y1 = scroll.y>>mTileSizeBase;
    Uint16 x2 = (scroll.x+num_v_tiles)>>mTileSizeBase;
    Uint16 y2 = (scroll.y+num_h_tiles)>>mTileSizeBase;

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

    for( size_t y=y1 ; y<=y2 ; y++)
    {
        for( size_t x=x1 ; x<=x2 ; x++)
        {
            const auto fg = getMapDataAt(x,y);

            const int loc_x = (x<<mTileSizeBase)-scroll.x;
            const int loc_y = (y<<mTileSizeBase)-scroll.y;

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


void ScrollingPlane::setSubscrollUnits(const int subscrollUnits)
{
    assert(subscrollUnits > 0);

    mSubscrollUnits = subscrollUnits;
}

void ScrollingPlane::lockAxisY(const bool value)
{
    mLockAxisY = value;
}

bool ScrollingPlane::isAxisYLocked()
{
    return mLockAxisY;
}
