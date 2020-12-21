
#include "scrollingplane.h"
#include <base/video/CVideoDriver.h>

ScrollingPlane::ScrollingPlane()
{

}


void ScrollingPlane::refreshStripes(GsTilemap &tilemap)
{
    const int oldx = m_mapx<<mTileSizeBase;
    const int oldy = m_mapy<<mTileSizeBase;

    resetScrolling();

    gotoPos(tilemap, {oldx, oldy});
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


bool ScrollingPlane::scrollLeft(GsTilemap &tilemap)
{
    auto &scrollSfc = gVideoDriver.getScrollSurfaceVec().at(mScrollSfcIdx);
    {
        auto &scroll = mScrollCoords;

        if( scroll.x <= 32 )
            return false;

        scroll.x--;

        scrollSfc.UpdateScrollBufX(scroll.x);
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
        drawVstripe(tilemap, m_mapxstripepos, m_mapx);

        m_scrollpix = (1<<mTileSizeBase)-1;
    } else m_scrollpix--;

    return true;
}
// scrolls the map one pixel right
bool ScrollingPlane::scrollRight(GsTilemap &tilemap)
{
    const int res_width = gVideoDriver.getGameResolution().dim.x;

    auto &scrollSfc = gVideoDriver.getScrollSurfaceVec().at(mScrollSfcIdx);
    {
        auto &scroll = mScrollCoords;

        if(scroll.x >= int(((mWidth-2)<<mTileSizeBase) - res_width))
            return false;

        scroll.x++;

        scrollSfc.UpdateScrollBufX(scroll.x);
    }

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

bool ScrollingPlane::scrollUp(GsTilemap &tilemap, const bool force)
{
    auto &scrollSfc = gVideoDriver.getScrollSurfaceVec().at(mScrollSfcIdx);
    {
        auto &scroll = mScrollCoords;

        if( scroll.y <= 32 )
            return false;

        scroll.y--;

        scrollSfc.UpdateScrollBufY(scroll.y);
    }


    if (m_scrollpixy==0)
    {  // need to draw a new stripe
        if(m_mapy>0) m_mapy--;

        if (m_mapystripepos == 0)
        {
            // TODO: Problem with different squared sizes here
            auto &scrollSfc = gVideoDriver.getScrollSurface(mScrollSfcIdx);
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
    const int res_height = gVideoDriver.getGameResolution().dim.y;

    auto &scrollSfc = gVideoDriver.getScrollSurfaceVec().at(mScrollSfcIdx);
    auto &scroll = mScrollCoords;
    {
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

        scroll.y++;
    }

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
    if(mpy >= mHeight) return;

    for(auto &scrollSfc : gVideoDriver.getScrollSurfaceVec() )
    {
        const auto dim = scrollSfc.getSquareSize();

        Uint32 num_v_tiles= dim>>mTileSizeBase;

        // TODO: Do we really need to apply AND operation with drawMask? Check!
        const int drawMask = dim-1;

        if( num_v_tiles+m_mapx >= mWidth )
            num_v_tiles = mWidth-m_mapx;

        for(Uint32 x=0;x<num_v_tiles;x++)
        {
            Uint32 tile = getMapDataAt(x+m_mapx, mpy);

            //TODO: Consider 0th always to be transparent, except for the first one
            //if(!tile && planeIdx > 0)
            //    continue;

            tilemap.drawTile(scrollSfc.getScrollSurface(),
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

        //TODO: Consider 0th always to be transparent, except for the first one
        //if(tile == 0 && planeIdx > 0)
        //    continue;

        // TODO: Do we really need to apply AND operation with drawMask? Check!
        const int drawMask = dim-1;

        tilemap.drawTile(scrollSfc.getScrollSurface(),
                            x,
                            ((y<<mTileSizeBase)+m_mapystripepos) & drawMask, tile);
    }

}


void ScrollingPlane::drawAll(GsTilemap &tilemap)
{
    auto &scroll = mScrollCoords;
    auto &scrollSfc = gVideoDriver.getScrollSurfaceVec().at(mScrollSfcIdx);

    const auto dim = scrollSfc.getSquareSize();

    scrollSfc.updateScrollBuf(scroll);

    Uint32 num_v_tiles = dim>>mTileSizeBase;
    if(num_v_tiles+m_mapx >= mWidth)
        num_v_tiles = mWidth-m_mapx;

    Uint32 num_h_tiles = dim>>mTileSizeBase;
    if(num_h_tiles+m_mapy >= mHeight)
        num_h_tiles = mHeight-m_mapy;

    for(Uint32 y=0;y<num_h_tiles;y++)
    {
        for(Uint32 x=0;x<num_v_tiles;x++)
        {
            Uint32 tile = getMapDataAt(x+m_mapx, y+m_mapy);

            //TODO: Consider 0th always to be transparent, except for the first one
            //if(!tile && planeIdx > 0)
//                continue;

            tilemap.drawTile(scrollSfc,
                                ((x<<mTileSizeBase)+m_mapxstripepos),
                                ((y<<mTileSizeBase)+m_mapystripepos),
                                tile);
        }
    }

}

bool ScrollingPlane::gotoPos(GsTilemap &tilemap,
                             const GsVec2D<int> pos)
{
    bool retval = false;

    auto &scroll =  mScrollCoords;

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

void ScrollingPlane::collectBlockersCoordiantes()
{
    scrollBlockX.clear();
    scrollBlockY.clear();

    insertVertBlocker(1<<CSF);
    insertHorBlocker(1<<CSF);
    assert(0);
/*
    int ep = gBehaviorEngine.getEpisode();

    if(gBehaviorEngine.getEngine() == ENGINE_GALAXY)
    {
        const word* map_ptr = mInfoPlane.getMapDataPtr();

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
    */
}


void ScrollingPlane::insertVertBlocker(const int x)
{
    scrollBlockX.insert(x);
}

void ScrollingPlane::fetchNearestVertBlockers(const int x,
                                    int &leftCoord,
                                    int &rightCoord)
{
    int blockXleft = 0;
    int blockXright = 0;

    assert(0);
/*
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
    */
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

    assert(0);
    /*

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
    */
}


void ScrollingPlane::redrawPlaneAt(GsTilemap &tilemap,
                                   const Uint32 mx,
                                   const Uint32 my)
{
    auto &scrollSfc = gVideoDriver.getScrollSurfaceVec().at(mScrollSfcIdx);
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
        const size_t tile = getMapDataAt(mx, my);

        // TODO: Check cases of transparent 0th tile here
        //if(!tile && planeIdx > 0)
          //  return;

        const Uint16 loc_x = (((mx-m_mapx)<<mTileSizeBase)+m_mapxstripepos)&drawMask;
        const Uint16 loc_y = (((my-m_mapy)<<mTileSizeBase)+m_mapystripepos)&drawMask;
        tilemap.drawTile(ScrollSurface, loc_x, loc_y, tile);
    }
}
